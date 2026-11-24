//-----------------------------------------------
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//-----------------------------------------------

# include "Siv3DTest.hpp"
# include "../Siv3D/src/Siv3D/Renderer2D/Renderer2DStateUpdate.hpp"
# include "../Siv3D/src/Siv3D/Engine/Siv3DEngine.hpp"
# include "../Siv3D/src/Siv3D/EngineShader/IEngineShader.hpp"
# include <functional>
# include <memory>

# if SIV3D_PLATFORM(WINDOWS)
# include "../Siv3D/src/Siv3D-Platform/WindowsDesktop/Siv3D/Renderer2D/D3D11/D3D11Renderer2DCommandManager.hpp"
# elif SIV3D_PLATFORM(MACOS)
# include "../Siv3D/src/Siv3D-Platform/macOS/Siv3D/Renderer2D/Metal/MetalRenderer2DCommandManager.hpp"
# endif

namespace
{
	enum class TestCommand : uint32
	{
		Resource = 1,
		Other = 2,
	};

	struct TestResource
	{
		int32 value;
		int32 id() const { return value; }
	};

	struct RecordedResource
	{
		int32 value = 1;
		mutable uint32 reads = 0;
		int32 back() const { ++reads; return value; }
	};

	struct RetentionCalls
	{
		Array<int32> ids;
		void try_emplace(int32 id, const TestResource&) { ids.push_back(id); }
	};
}

TEST_CASE("Renderer2D.resource_state_shader_retention_conditions")
{
	BatchStateTracker<TestCommand> tracker;
	tracker.set(TestCommand::Other);
	RecordedResource recorded;
	RetentionCalls retained;
	int32 current = 1;
	const auto set = [&](int32 id)
	{
		UpdateRenderer2DCustomShaderState(tracker, TestCommand::Resource, TestResource{ id }, current, recorded, retained);
	};

	set(1);
	CHECK(retained.ids.isEmpty());
	CHECK(recorded.reads == 0);
	CHECK_FALSE(tracker.has(TestCommand::Resource));
	set(2);
	CHECK(current == 2);
	CHECK(retained.ids == Array<int32>{ 2 });
	CHECK(recorded.reads == 0);
	CHECK(tracker.has(TestCommand::Resource));
	set(2);
	CHECK(retained.ids == Array<int32>{ 2, 2 });
	CHECK(recorded.reads == 1);
	set(3);
	CHECK(current == 3);
	CHECK(retained.ids == Array<int32>{ 2, 2, 3 });
	CHECK(recorded.reads == 2);
	set(1);
	CHECK(current == 1);
	CHECK_FALSE(tracker.has(TestCommand::Resource));
	CHECK(retained.ids == Array<int32>{ 2, 2, 3, 1 });
	CHECK(recorded.reads == 3);
	set(1);
	CHECK(retained.ids.size() == 4);
	CHECK(recorded.reads == 3);

	// An ID-only update can precede a custom shader with the same pending ID.
	UpdateRenderer2DState(tracker, TestCommand::Resource, 4, current, recorded);
	set(4);
	CHECK(retained.ids == Array<int32>{ 2, 2, 3, 1, 4 });
	CHECK(recorded.reads == 4);
	CHECK(tracker.has(TestCommand::Resource));
	CHECK(tracker.has(TestCommand::Other));
}

TEST_CASE("Renderer2D.resource_state_texture_retention_conditions")
{
	BatchStateTracker<TestCommand> tracker;
	tracker.set(TestCommand::Other);
	RecordedResource recorded;
	RetentionCalls retained;
	int32 current = 1;
	const auto set = [&](int32 id)
	{
		UpdateRenderer2DTextureState(tracker, TestCommand::Resource, TestResource{ id }, current, recorded, retained);
	};

	set(1);
	CHECK(retained.ids.isEmpty());
	CHECK(recorded.reads == 0);
	CHECK_FALSE(tracker.has(TestCommand::Resource));
	set(2);
	CHECK(current == 2);
	CHECK(retained.ids == Array<int32>{ 2 });
	CHECK(recorded.reads == 0);
	CHECK(tracker.has(TestCommand::Resource));
	set(2);
	CHECK(retained.ids.size() == 1);
	CHECK(recorded.reads == 0);
	set(3);
	CHECK(current == 3);
	CHECK(retained.ids == Array<int32>{ 2, 3 });
	CHECK(recorded.reads == 1);
	set(1);
	CHECK(current == 1);
	CHECK_FALSE(tracker.has(TestCommand::Resource));
	CHECK(retained.ids == Array<int32>{ 2, 3, 1 });
	CHECK(recorded.reads == 2);
	set(1);
	CHECK(retained.ids.size() == 3);
	CHECK(recorded.reads == 2);

	UpdateRenderer2DState(tracker, TestCommand::Resource, -1, current, recorded);
	set(1);
	CHECK(current == 1);
	CHECK_FALSE(tracker.has(TestCommand::Resource));
	CHECK(retained.ids == Array<int32>{ 2, 3, 1, 1 });
	CHECK(recorded.reads == 3);
	CHECK(tracker.has(TestCommand::Other));
}

# if SIV3D_PLATFORM(WINDOWS) || SIV3D_PLATFORM(MACOS)

namespace
{
# if SIV3D_PLATFORM(WINDOWS)
	using CommandManager = D3D11Renderer2DCommandManager;
	using CommandType = D3D11Renderer2DCommandType;
# else
	using CommandManager = MetalRenderer2DCommandManager;
	using CommandType = MetalRenderer2DCommandType;
# endif

	template <class Resource>
	struct ObservedResource : Resource
	{
		explicit ObservedResource(const Resource& resource) : Resource{ resource } {}
		auto lifetime() const { return std::weak_ptr<typename Resource::AssetIDWrapperType>{ this->m_handle }; }
	};

	template <class Resource, class Push, class Unbind, class Get>
	void CheckResourceCommands(const CommandType command, const Resource& a, const Resource& b,
		Push push, Unbind unbind, Get get)
	{
		CAPTURE(FromEnum(command));
		const ObservedResource<Resource> first{ a }, second{ b };
		REQUIRE(first);
		REQUIRE(second);
		REQUIRE(first.id() != second.id());
		const auto firstLife = first.lifetime(), secondLife = second.lifetime();
		const auto firstRefs = firstLife.use_count(), secondRefs = secondLife.use_count();
		{
			CommandManager manager;
			const auto set = [&](const Resource& value) { std::invoke(push, manager, value); };
			set(first);
			CHECK(firstLife.use_count() == (firstRefs + 1));
			set(first);
			CHECK(firstLife.use_count() == (firstRefs + 1));
			manager.flush();
			const size_t start = manager.getCommands().size();

			manager.pushDraw(3);
			set(second);
			set(second);
			CHECK(secondLife.use_count() == (secondRefs + 1));
			set(first);
			manager.pushDraw(6);
			set(second);
			manager.pushDraw(12);
			manager.flush();
			const auto& commands = manager.getCommands();
			REQUIRE(commands.size() == (start + 3));
			REQUIRE(commands[start].type == CommandType::Draw);
			CHECK(manager.getDraw(commands[start].index).indexCount == 9);
			REQUIRE(commands[start + 1].type == command);
			CHECK(std::invoke(get, manager, commands[start + 1].index) == second.id());
			REQUIRE(commands.back().type == CommandType::Draw);
			CHECK(manager.getDraw(commands.back().index).indexCount == 12);

			set(second);
			std::invoke(unbind, manager);
			set(second);
			manager.flush();
			CHECK(commands.size() == (start + 3));
			std::invoke(unbind, manager);
			std::invoke(unbind, manager);
			manager.flush();
			REQUIRE(commands.size() == (start + 4));
			REQUIRE(commands.back().type == command);
			CHECK(std::invoke(get, manager, commands.back().index) == Resource::IDType::Invalid());
			CHECK(firstLife.use_count() == (firstRefs + 1));
			CHECK(secondLife.use_count() == (secondRefs + 1));

			manager.reset();
			CHECK(firstLife.use_count() == firstRefs);
			CHECK(secondLife.use_count() == secondRefs);
			set(first);
			CHECK(firstLife.use_count() == (firstRefs + 1));
		}
		CHECK(firstLife.use_count() == firstRefs);
		CHECK(secondLife.use_count() == secondRefs);
	}
}

TEST_CASE("Renderer2D.resource_state_shader_commands_and_lifetime")
{
	const auto* shaders = SIV3D_ENGINE(EngineShader);
	CheckResourceCommands(CommandType::SetVS, shaders->getVS(EngineVS::Shape2D), shaders->getVS(EngineVS::Pattern2D),
		&CommandManager::pushCustomVS,
		[](CommandManager& manager) { manager.pushEngineVS(VertexShader::IDType::Invalid()); },
		&CommandManager::getVS);
	CheckResourceCommands(CommandType::SetPS, shaders->getPS(EnginePS::Shape2D), shaders->getPS(EnginePS::Texture2D),
		&CommandManager::pushCustomPS,
		[](CommandManager& manager) { manager.pushEnginePS(PixelShader::IDType::Invalid()); },
		&CommandManager::getPS);
}

TEST_CASE("Renderer2D.resource_state_texture_commands_and_lifetime")
{
	const Texture a{ Image{ 2, 2, Palette::Red } }, b{ Image{ 2, 2, Palette::Blue } };
	for (uint32 slot = 0; slot < Graphics::TextureSlotCount; ++slot)
	{
		CAPTURE(slot);
		CheckResourceCommands(ToEnum<CommandType>(FromEnum(CommandType::VSTexture0) + slot), a, b,
			[=](CommandManager& manager, const Texture& texture) { manager.pushVSTexture(slot, texture); },
			[=](CommandManager& manager) { manager.pushVSTextureUnbind(slot); },
			[=](const CommandManager& manager, uint32 index) { return manager.getVSTexture(slot, index); });
		CheckResourceCommands(ToEnum<CommandType>(FromEnum(CommandType::PSTexture0) + slot), a, b,
			[=](CommandManager& manager, const Texture& texture) { manager.pushPSTexture(slot, texture); },
			[=](CommandManager& manager) { manager.pushPSTextureUnbind(slot); },
			[=](const CommandManager& manager, uint32 index) { return manager.getPSTexture(slot, index); });
	}
}

TEST_CASE("Renderer2D.resource_state_independent_texture_slots")
{
	CommandManager manager;
	const ObservedResource<Texture> texture{ Texture{ Image{ 2, 2, Palette::White } } };
	REQUIRE(texture);
	const auto life = texture.lifetime();
	const auto refs = life.use_count();
	const size_t start = manager.getCommands().size();
	manager.pushDraw(3);
	for (uint32 slot = 0; slot < Graphics::TextureSlotCount; ++slot)
	{
		manager.pushVSTexture(slot, texture);
		manager.pushPSTexture(slot, texture);
	}
	for (uint32 slot = 0; slot < Graphics::TextureSlotCount; ++slot)
	{
		if ((slot % 2) == 0)
		{
			manager.pushVSTextureUnbind(slot);
		}
		else
		{
			manager.pushPSTextureUnbind(slot);
		}
	}
	manager.pushDraw(6);
	manager.flush();
	CHECK(life.use_count() == (refs + 1));
	const auto& commands = manager.getCommands();
	REQUIRE(commands.size() == (start + Graphics::TextureSlotCount + 2));
	REQUIRE(commands[start].type == CommandType::Draw);
	CHECK(manager.getDraw(commands[start].index).indexCount == 3);
	REQUIRE(commands.back().type == CommandType::Draw);
	CHECK(manager.getDraw(commands.back().index).indexCount == 6);
	std::array<bool, Graphics::TextureSlotCount> seenVS{}, seenPS{};
	for (size_t i = (start + 1); i < (commands.size() - 1); ++i)
	{
		const auto& command = commands[i];
		const uint32 type = FromEnum(command.type);
		if ((FromEnum(CommandType::VSTexture0) <= type) && (type <= FromEnum(CommandType::VSTexture7)))
		{
			const uint32 slot = (type - FromEnum(CommandType::VSTexture0));
			CHECK_FALSE(seenVS[slot]);
			seenVS[slot] = true;
			CHECK(manager.getVSTexture(slot, command.index) == texture.id());
		}
		else
		{
			REQUIRE(FromEnum(CommandType::PSTexture0) <= type);
			REQUIRE(type <= FromEnum(CommandType::PSTexture7));
			const uint32 slot = (type - FromEnum(CommandType::PSTexture0));
			CHECK_FALSE(seenPS[slot]);
			seenPS[slot] = true;
			CHECK(manager.getPSTexture(slot, command.index) == texture.id());
		}
	}
	for (uint32 slot = 0; slot < Graphics::TextureSlotCount; ++slot)
	{
		CAPTURE(slot);
		CHECK(seenVS[slot] == ((slot % 2) != 0));
		CHECK(seenPS[slot] == ((slot % 2) == 0));
	}
	manager.reset();
	CHECK(life.use_count() == refs);
}

TEST_CASE("Renderer2D.resource_state_texture_owner_release")
{
	CommandManager manager;
	std::weak_ptr<Texture::AssetIDWrapperType> life;
	{
		const ObservedResource<Texture> texture{ Texture{ Image{ 2, 2, Palette::White } } };
		REQUIRE(texture);
		life = texture.lifetime();
		manager.pushPSTexture(0, texture);
		manager.pushPSTextureUnbind(0);
	}
	CHECK_FALSE(life.expired());
	const size_t start = manager.getCommands().size();
	manager.pushDraw(3);
	manager.flush();
	CHECK(manager.getCommands().size() == (start + 1));
	CHECK_FALSE(life.expired());
	manager.reset();
	CHECK(life.expired());
}

# endif
