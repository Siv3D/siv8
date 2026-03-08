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

# include <Siv3D/Dialog.hpp>
# include <Siv3D/ScopeExit.hpp>
# include <Siv3D/Window/IWindow.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>
# include <Siv3D/Windows/Windows.hpp>
# include <Siv3D/Windows/ComPtr.hpp>
# include <Shobjidl.h>

namespace s3d
{
	namespace
	{
		[[nodiscard]]
		static std::wstring MakeFilterPattern(const Array<String>& extensions)
		{
			if (extensions.isEmpty())
			{
				return L"*.*";
			}

			std::wstring pattern;
			bool first = true;

			for (const auto& ext : extensions)
			{
				// 空要素は無視
				if (ext.isEmpty())
				{
					continue;
				}

				if (not first)
				{
					pattern.append(L";");
				}

				pattern.append(L"*.");
				pattern.append(Unicode::ToWstring(ext));
				first = false;
			}

			// 全部空だった場合のフォールバック
			if (first)
			{
				return L"*.*";
			}

			return pattern;
		}

		struct DialogFilterData
		{
			Array<std::wstring> names;
			Array<std::wstring> patterns;
			Array<COMDLG_FILTERSPEC> specs;
		};

		[[nodiscard]]
		static DialogFilterData BuildFilterSpecs(Array<FileDialogFilter> filters)
		{
			if (filters.isEmpty())
			{
				filters = { FileDialogFilter::AllFiles() };
			}

			DialogFilterData data;
			data.names.reserve(filters.size());
			data.patterns.reserve(filters.size());
			data.specs.reserve(filters.size());

			for (const auto& filter : filters)
			{
				// description が空でも Windows 側で空表示にしないよう最低限補う
				std::wstring name = filter.description.isEmpty()
					? L"Files"
					: Unicode::ToWstring(filter.description);

				std::wstring pattern = MakeFilterPattern(filter.extensions);

				data.names.push_back(std::move(name));
				data.patterns.push_back(std::move(pattern));
			}

			for (size_t i = 0; i < data.names.size(); ++i)
			{
				data.specs.push_back(COMDLG_FILTERSPEC{
					.pszName = data.names[i].c_str(),
					.pszSpec = data.patterns[i].c_str()
				});
			}

			return data;
		}

		[[nodiscard]]
		static ComPtr<IShellItem> MakeShellItemFromPath(const FilePath& path)
		{
			if (path.isEmpty())
			{
				return nullptr;
			}

			const FilePath fullPath = FileSystem::FullPath(path);
			const std::wstring nativePath = FileSystem::NativePath(fullPath);

			ComPtr<IShellItem> item;
			if (FAILED(::SHCreateItemFromParsingName(
				nativePath.c_str(),
				nullptr,
				IID_PPV_ARGS(&item))))
			{
				return nullptr;
			}

			return item;
		}

		[[nodiscard]]
		static Optional<FilePath> GetFilePathFromShellItem(IShellItem* item)
		{
			if (item == nullptr)
			{
				return none;
			}

			PWSTR path = nullptr;
			if (FAILED(item->GetDisplayName(SIGDN_FILESYSPATH, &path)))
			{
				return none;
			}

			const ScopeExit cleanup = [path]() { ::CoTaskMemFree(path); };

			return FileSystem::FullPath(Unicode::FromWstring(path));
		}

		static void ApplyCommonOpenDialogOptions(IFileOpenDialog* dialog, const OpenFileDialogOptions& options, const bool multiSelect)
		{
			DWORD fos = 0;
			if (SUCCEEDED(dialog->GetOptions(&fos)))
			{
				fos |= FOS_FORCEFILESYSTEM;
				fos |= FOS_PATHMUSTEXIST;
				fos |= FOS_FILEMUSTEXIST;

				if (multiSelect)
				{
					fos |= FOS_ALLOWMULTISELECT;
				}

				if (options.showHiddenFiles)
				{
					fos |= FOS_FORCESHOWHIDDEN;
				}

				(void)dialog->SetOptions(fos);
			}

			if (not options.title.isEmpty())
			{
				(void)dialog->SetTitle(Unicode::ToWstring(options.title).c_str());
			}

			if (not options.defaultFileName.isEmpty())
			{
				(void)dialog->SetFileName(Unicode::ToWstring(options.defaultFileName).c_str());
			}

			if (not options.acceptButtonText.isEmpty())
			{
				(void)dialog->SetOkButtonLabel(Unicode::ToWstring(options.acceptButtonText).c_str());
			}

			// 空なら OS の最近使った場所を尊重
			// 空でなければ、そのセッションでは必ずそこを初期フォルダにする
			if (not options.initialDirectory.isEmpty())
			{
				if (auto folder = MakeShellItemFromPath(options.initialDirectory))
				{
					(void)dialog->SetFolder(folder.Get());
				}
			}
		}

		[[nodiscard]]
		static Array<FilePath> ShowOpenDialogImpl(const OpenFileDialogOptions& options, const bool multiSelect)
		{
			ComPtr<IFileOpenDialog> dialog;
			if (FAILED(::CoCreateInstance(
				CLSID_FileOpenDialog,
				nullptr,
				CLSCTX_INPROC_SERVER,
				IID_PPV_ARGS(&dialog))))
			{
				return{};
			}

			ApplyCommonOpenDialogOptions(dialog.Get(), options, multiSelect);

			// SetFileTypes() は 1 回しか呼べないので、ここで一度だけ構築して設定する
			const DialogFilterData filterData = BuildFilterSpecs(options.filters);
			if (not filterData.specs.isEmpty())
			{
				(void)dialog->SetFileTypes(
					static_cast<UINT>(filterData.specs.size()),
					filterData.specs.data());
			}

			{
				const HWND hWnd = static_cast<HWND>(SIV3D_ENGINE(Window)->getHandle());
				const HRESULT hr = dialog->Show(hWnd);

				if (hr == HRESULT_FROM_WIN32(ERROR_CANCELLED))
				{
					return{};
				}

				if (FAILED(hr))
				{
					return{};
				}
			}

			Array<FilePath> results;

			if (multiSelect)
			{
				ComPtr<IShellItemArray> items;
				if (FAILED(dialog->GetResults(&items)))
				{
					return{};
				}

				DWORD count = 0;
				if (FAILED(items->GetCount(&count)))
				{
					return{};
				}

				results.reserve(count);

				for (DWORD i = 0; i < count; ++i)
				{
					ComPtr<IShellItem> item;
					if (FAILED(items->GetItemAt(i, &item)))
					{
						continue;
					}

					if (auto path = GetFilePathFromShellItem(item.Get()))
					{
						results.push_back(std::move(*path));
					}
				}
			}
			else
			{
				ComPtr<IShellItem> item;
				if (FAILED(dialog->GetResult(&item)))
				{
					return{};
				}

				if (auto path = GetFilePathFromShellItem(item.Get()))
				{
					results.push_back(std::move(*path));
				}
			}

			return results;
		}
	}

	namespace Dialog
	{
		////////////////////////////////////////////////////////////////
		//
		//	OpenFile, OpenFiles
		//
		////////////////////////////////////////////////////////////////

		Optional<FilePath> OpenFile(const OpenFileDialogOptions& options)
		{
			Array<FilePath> results = ShowOpenDialogImpl(options, false);

			if (results.isEmpty())
			{
				return none;
			}

			return std::move(results.front());
		}

		Array<FilePath> OpenFiles(const OpenFileDialogOptions& options)
		{
			return ShowOpenDialogImpl(options, true);
		}
	}
}
