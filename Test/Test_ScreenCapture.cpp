//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2026 Ryo Suzuki
//	Copyright (c) 2016-2026 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include "Siv3DTest.hpp"
# include <Siv3D/Renderer/IRenderer.hpp>
# include <Siv3D/Scene/SceneStyle.hpp>
# include <Siv3D/ScreenCapture/CScreenCapture.hpp>

# if SIV3D_PLATFORM(WINDOWS)
# include <Siv3D/Engine/Siv3DEngine.hpp>
# include <Siv3D/Renderer/D3D11/CRenderer_D3D11.hpp>
# endif

namespace
{
	class CaptureRenderer final : public ISiv3DRenderer
	{
	public:
		Image image{ 3, 2, Palette::Red };
		bool captureResult = true;
		uint32 captureCalls = 0;
		mutable uint32 imageReads = 0;

		bool captureScreenshot() override
		{
			++captureCalls;
			return captureResult;
		}

		const Image& getScreenCapture() const override
		{
			++imageReads;
			return image;
		}

		StringView getName() const override { return U"Capture test renderer"; }
		EngineOption::Renderer getRendererType() const noexcept override { return EngineOption::Renderer::Direct3D11; }
		void init() override {}
		void waitForFrame() override {}
		void beginFrame() override {}
		void flush() override {}
		bool present() override { return true; }
		SceneStyle& getSceneStyle() noexcept override { return m_style; }
		void configureScene(const SceneConfig&) override {}
		SceneConfig getSceneConfig() const noexcept override { return SceneConfig{}; }
		void setSceneSampleCount(uint32) override {}
		uint32 getSceneSampleCount() const noexcept override { return 1; }
		const Size& getSceneSize() const noexcept override { return m_size; }
		const Size& getSceneBufferSize() const noexcept override { return m_size; }
		std::pair<double, RectF> getLetterboxComposition() const noexcept override { return { 1.0, RectF{ m_size } }; }
		void updateSceneSize() override {}
		void setVSyncEnabled(bool) override {}
		bool isVSyncEnabled() const override { return false; }

	private:
		SceneStyle m_style;
		Size m_size{ 3, 2 };
	};

	struct ScopedCaptureSettings
	{
		const FilePath directory = ScreenCapture::GetScreenshotDirectory();
		const FilePath currentDirectory = FileSystem::CurrentDirectory();
		const Array<InputGroup> shortcutKeys = ScreenCapture::GetShortcutKeys();

		ScopedCaptureSettings()
		{
			ScreenCapture::SetShortcutKeys({});
		}

		~ScopedCaptureSettings()
		{
			FileSystem::ChangeCurrentDirectory(currentDirectory);
			ScreenCapture::SetScreenshotDirectory(directory);
			ScreenCapture::SetShortcutKeys(shortcutKeys);
		}
	};

	void CheckSavedFrame(const FilePathView path)
	{
		INFO(Unicode::ToUTF8(path));
		REQUIRE(FileSystem::IsFile(path));
		const Image saved{ path };
		REQUIRE(saved);
		CHECK(saved == ScreenCapture::GetFrame());
	}
}

TEST_CASE("ScreenCapture.default_directory")
{
	CHECK(ScreenCapture::GetScreenshotDirectory()
		== FileSystem::PathAppend(FileSystem::GetFolderPath(SpecialFolder::Pictures), U"Screenshot/"));
}

TEST_CASE("ScreenCapture.capture_failure_does_not_publish_or_save_stale_image")
{
	CaptureRenderer renderer;
	CScreenCapture capture;
	capture.setScreenshotShortcutKeys({});
	const FilePath saved = Test::OutputPath(U"screen-capture/readback/saved.png");
	const FilePath failed = Test::OutputPath(U"screen-capture/readback/failed.png");

	// Multiple requests share one readback, including memory-only requests.
	capture.requestScreenCapture();
	capture.requestScreenCapture(saved);
	capture.update(renderer);
	REQUIRE(capture.hasNewFrame());
	CHECK(renderer.captureCalls == 1);
	CHECK(Image{ saved } == renderer.image);
	const uint32 previousReads = renderer.imageReads;

	// A failed readback may leave a nonempty previous image in the backend.
	renderer.captureResult = false;
	renderer.image.fill(Palette::Blue);
	capture.requestScreenCapture(saved);
	capture.requestScreenCapture(failed);
	capture.requestScreenCapture();
	capture.update(renderer);
	CHECK_FALSE(capture.hasNewFrame());
	CHECK(renderer.captureCalls == 2);
	CHECK(renderer.imageReads == previousReads);
	CHECK((Image{ saved } == Image{ 3, 2, Palette::Red }));
	CHECK_FALSE(FileSystem::Exists(failed));

	// Failed requests are consumed, and a later request can succeed.
	renderer.captureResult = true;
	capture.update(renderer);
	CHECK_FALSE(capture.hasNewFrame());
	CHECK(renderer.captureCalls == 2);
	CHECK_FALSE(FileSystem::Exists(failed));
	capture.requestScreenCapture(saved);
	capture.update(renderer);
	CHECK(capture.hasNewFrame());
	CHECK(renderer.captureCalls == 3);
	CHECK(Image{ saved } == renderer.image);
}

TEST_CASE("ScreenCapture.named_and_automatic_save")
{
	const ScopedCaptureSettings settings;
	const FilePath directory = Test::OutputPath(U"screen-capture/named-名前-😀/");
	// A trailing separator is optional, and configuration does not create folders.
	ScreenCapture::SetScreenshotDirectory(directory.substr(0, directory.size() - 1));
	CHECK(ScreenCapture::GetScreenshotDirectory() == directory);
	CHECK_FALSE(FileSystem::Exists(directory));

	REQUIRE(System::Update());
	Rect{ 10, 10, 40, 40 }.draw(Palette::Red);
	ScreenCapture::SaveCurrentFrame(U"boss-clear-勝利-😀.png");
	ScreenCapture::SaveCurrentFrame();
	CHECK_FALSE(FileSystem::Exists(directory));
	REQUIRE(System::Update());
	REQUIRE(ScreenCapture::HasNewFrame());

	const auto files = FileSystem::DirectoryContents(directory, Recursive::No);
	REQUIRE(files.size() == 2);
	CheckSavedFrame(directory + U"boss-clear-勝利-😀.png");
	for (const auto& file : files)
	{
		CheckSavedFrame(file);
		if (FileSystem::FileName(file) != U"boss-clear-勝利-😀.png")
		{
			const String name = FileSystem::FileName(file);
			CHECK(name.size() == 23); // yyyyMMdd-HHmmss-SSS.png
			CHECK(name.ends_with(U".png"));
		}
	}

	// Reusing a name overwrites the file with the newly captured frame.
	Rect{ 10, 10, 40, 40 }.draw(Palette::Blue);
	ScreenCapture::SaveCurrentFrame(U"boss-clear-勝利-😀.png");
	REQUIRE(System::Update());
	CheckSavedFrame(directory + U"boss-clear-勝利-😀.png");
	CHECK(FileSystem::DirectoryContents(directory, Recursive::No).size() == 2);
}

TEST_CASE("ScreenCapture.destinations_are_fixed_when_requested")
{
	const ScopedCaptureSettings settings;
	const FilePath root = Test::OutputPath(U"screen-capture/resolve/");
	const FilePath first = (root + U"first/");
	const FilePath second = (root + U"second/");
	REQUIRE(FileSystem::CreateDirectories(first));
	REQUIRE(FileSystem::CreateDirectories(second));
	REQUIRE(FileSystem::ChangeCurrentDirectory(first));
	ScreenCapture::SetScreenshotDirectory(U"named");
	CHECK(ScreenCapture::GetScreenshotDirectory() == (first + U"named/"));

	REQUIRE(System::Update());
	ScreenCapture::SaveCurrentFrame(U"first.png");
	ScreenCapture::SaveCurrentFrameTo(U"relative/sub/shot.png");
	ScreenCapture::SaveCurrentFrameTo(root + U"absolute/sub/shot.png");
	CHECK(FileSystem::ChangeCurrentDirectory(second));
	// Changing the working directory alone must not move the configured directory.
	ScreenCapture::SaveCurrentFrame(U"still-first.png");
	ScreenCapture::SetScreenshotDirectory(U"named/");
	ScreenCapture::SaveCurrentFrame(U"second.png");
	CHECK_FALSE(FileSystem::Exists(first + U"named/"));
	REQUIRE(System::Update());
	REQUIRE(ScreenCapture::HasNewFrame());

	for (const auto& path : { first + U"named/first.png", first + U"named/still-first.png",
		first + U"relative/sub/shot.png", root + U"absolute/sub/shot.png", second + U"named/second.png" })
	{
		CheckSavedFrame(path);
	}
	CHECK_FALSE(FileSystem::Exists(second + U"relative/"));
	CHECK_FALSE(FileSystem::Exists(second + U"named/first.png"));
	CHECK_FALSE(FileSystem::Exists(first + U"named/relative/"));
}

TEST_CASE("ScreenCapture.rejects_invalid_names_and_empty_paths")
{
	const ScopedCaptureSettings settings;
	const FilePath directory = Test::OutputPath(U"screen-capture/invalid/");
	ScreenCapture::SetScreenshotDirectory(directory);
	const String embeddedNul{ U"shot.png\0other.png", 18 };

	ScreenCapture::SetScreenshotDirectory(U"");
	CHECK(ScreenCapture::GetScreenshotDirectory() == directory);
	ScreenCapture::SetScreenshotDirectory(embeddedNul);
	CHECK(ScreenCapture::GetScreenshotDirectory() == directory);
	ScreenCapture::SetScreenshotDirectory(Resource(U"screen-capture/"));
	CHECK(ScreenCapture::GetScreenshotDirectory() == directory);

	REQUIRE(System::Update());
	for (const FilePathView name : { U"", U".", U"..", U"nested/shot.png", U"../shot.png",
		U"nested\\shot.png", U"C:shot.png", U"C:\\shot.png", U"/shot.png", U"\\\\server\\share\\shot.png" })
	{
		ScreenCapture::SaveCurrentFrame(name);
	}
	ScreenCapture::SaveCurrentFrame(embeddedNul);
	ScreenCapture::SaveCurrentFrameTo(U"");
	ScreenCapture::SaveCurrentFrameTo(embeddedNul);
	REQUIRE(System::Update());
	CHECK_FALSE(ScreenCapture::HasNewFrame());
	CHECK_FALSE(FileSystem::Exists(directory));

	// Memory-only capture remains explicit and does not create a directory.
	ScreenCapture::RequestCurrentFrame();
	REQUIRE(System::Update());
	CHECK(ScreenCapture::HasNewFrame());
	CHECK_FALSE(ScreenCapture::GetFrame().isEmpty());
	CHECK_FALSE(FileSystem::Exists(directory));
	REQUIRE(System::Update());
	CHECK_FALSE(ScreenCapture::HasNewFrame());
}

TEST_CASE("ScreenCapture.failed_save_does_not_redirect_or_block_other_requests")
{
	const ScopedCaptureSettings settings;
	const FilePath root = Test::OutputPath(U"screen-capture/failure/");
	ScreenCapture::SetScreenshotDirectory(root + U"fallback/");
	const FilePath blocked = (root + U"directory.png/");

	REQUIRE(System::Update());
	ScreenCapture::SaveCurrentFrameTo(blocked.substr(0, blocked.size() - 1));
	// Turn the destination into a directory after accepting the file path.
	CHECK(FileSystem::CreateDirectories(blocked));
	ScreenCapture::SaveCurrentFrameTo(root + U"unknown.unsupported-image-extension");
	ScreenCapture::SaveCurrentFrameTo(root + U"success.png");
	ScreenCapture::RequestCurrentFrame();
	REQUIRE(System::Update());
	CHECK(ScreenCapture::HasNewFrame());
	CheckSavedFrame(root + U"success.png");
	CHECK(FileSystem::IsDirectory(blocked));
	CHECK(FileSystem::IsEmptyDirectory(blocked));
	CHECK_FALSE(FileSystem::Exists(root + U"unknown.unsupported-image-extension"));
	CHECK_FALSE(FileSystem::Exists(root + U"fallback/"));
}

# if SIV3D_PLATFORM(WINDOWS)

TEST_CASE("ScreenCapture.D3D11.failed_staging_resize_preserves_readback")
{
	auto* renderer = static_cast<CRenderer_D3D11*>(SIV3D_ENGINE(Renderer));
	const auto& device = renderer->getDevice();
	const Size size{ 7, 3 };
	auto source = D3D11InternalTexture2D::CreateRenderTexture(device.getDevice(), size);
	source.clear(device.getContext(), Palette::Red);
	D3D11ScreenCapture capture;
	capture.init(device);
	REQUIRE(capture.capture(source.getTexture(), size));
	const Color* const pixels = capture.getImage().data();
	CHECK((capture.getImage() == Image{ size, Palette::Red }));

	// Texture creation must fail before the mismatched source can be copied.
	const Size unsupportedSize{ D3D11_REQ_TEXTURE2D_U_OR_V_DIMENSION + 1, 1 };
	CHECK_FALSE(capture.capture(source.getTexture(), unsupportedSize));
	CHECK(capture.getImage().data() == pixels);
	CHECK((capture.getImage() == Image{ size, Palette::Red }));

	// The old-size staging texture must still be usable after the failed resize.
	source.clear(device.getContext(), Palette::Blue);
	REQUIRE(capture.capture(source.getTexture(), size));
	CHECK(capture.getImage().data() == pixels);
	CHECK((capture.getImage() == Image{ size, Palette::Blue }));
}

TEST_CASE("ScreenCapture.windows_drive_and_root_relative_paths")
{
	const ScopedCaptureSettings settings;
	const FilePath root = Test::OutputPath(U"screen-capture/windows/");
	REQUIRE(FileSystem::CreateDirectories(root));
	REQUIRE(FileSystem::ChangeCurrentDirectory(root));
	ScreenCapture::SetScreenshotDirectory(root + U"unused/");
	// A checkout on a UNC share has no drive letter for these cases.
	if ((root.size() < 3) || (root[1] != U':'))
	{
		SKIP("Drive-relative paths require a checkout on a drive-letter volume");
	}

	REQUIRE(System::Update());
	ScreenCapture::SaveCurrentFrameTo((root + U"absolute.png").replaced(U'/', U'\\'));
	ScreenCapture::SaveCurrentFrameTo(root.substr(0, 2) + U"drive-relative.png");
	ScreenCapture::SaveCurrentFrameTo((root.substr(2) + U"root-relative.png").replaced(U'/', U'\\'));
	CHECK(FileSystem::ChangeCurrentDirectory(settings.currentDirectory));
	REQUIRE(System::Update());
	for (const FilePathView name : { U"absolute.png", U"drive-relative.png", U"root-relative.png" })
	{
		CheckSavedFrame(root + name);
	}
	CHECK_FALSE(FileSystem::Exists(root + U"unused/"));
}

# endif
