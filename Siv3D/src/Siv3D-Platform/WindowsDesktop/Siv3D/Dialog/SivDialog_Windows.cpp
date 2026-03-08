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
		static Array<FileDialogFilter> NormalizeFilters(Array<FileDialogFilter> filters)
		{
			if (filters.isEmpty())
			{
				filters = { FileDialogFilter::AllFiles() };
			}

			return filters;
		}

		[[nodiscard]]
		static DialogFilterData BuildFilterSpecs(const Array<FileDialogFilter>& filters)
		{
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

		static void ApplyCommonFileDialogSettings(
			IFileDialog* dialog,
			const FilePath& initialDirectory,
			const String& defaultFileName,
			const String& title,
			const String& acceptButtonText)
		{
			if (dialog == nullptr)
			{
				return;
			}

			if (not title.isEmpty())
			{
				(void)dialog->SetTitle(Unicode::ToWstring(title).c_str());
			}

			if (not defaultFileName.isEmpty())
			{
				(void)dialog->SetFileName(Unicode::ToWstring(defaultFileName).c_str());
			}

			if (not acceptButtonText.isEmpty())
			{
				(void)dialog->SetOkButtonLabel(Unicode::ToWstring(acceptButtonText).c_str());
			}

			// 空なら OS の最近使った場所を尊重
			// 空でなければ、そのセッションでは必ずそこを初期フォルダにする
			if (not initialDirectory.isEmpty())
			{
				if (auto folder = MakeShellItemFromPath(initialDirectory))
				{
					(void)dialog->SetFolder(folder.Get());
				}
			}
		}

		static void ApplyOpenDialogOptions(IFileOpenDialog* dialog, const OpenFileDialogOptions& options, const bool multiSelect)
		{
			if (dialog == nullptr)
			{
				return;
			}

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

			ApplyCommonFileDialogSettings(
				dialog,
				options.initialDirectory,
				options.defaultFileName,
				options.title,
				options.acceptButtonText);
		}

		static void ApplySaveDialogOptions(IFileSaveDialog* dialog, const SaveFileDialogOptions& options)
		{
			if (dialog == nullptr)
			{
				return;
			}

			DWORD fos = 0;
			if (SUCCEEDED(dialog->GetOptions(&fos)))
			{
				fos |= FOS_FORCEFILESYSTEM;
				fos |= FOS_PATHMUSTEXIST;
				fos |= FOS_OVERWRITEPROMPT;

				if (options.showHiddenFiles)
				{
					fos |= FOS_FORCESHOWHIDDEN;
				}

				(void)dialog->SetOptions(fos);
			}

			ApplyCommonFileDialogSettings(
				dialog,
				options.initialDirectory,
				options.defaultFileName,
				options.title,
				options.acceptButtonText);
		}

		static void ApplySelectFolderDialogOptions(IFileOpenDialog* dialog, const SelectFolderDialogOptions& options)
		{
			if (dialog == nullptr)
			{
				return;
			}

			DWORD fos = 0;
			if (SUCCEEDED(dialog->GetOptions(&fos)))
			{
				fos |= FOS_FORCEFILESYSTEM;
				fos |= FOS_PATHMUSTEXIST;
				fos |= FOS_PICKFOLDERS;

				if (options.showHiddenFiles)
				{
					fos |= FOS_FORCESHOWHIDDEN;
				}

				(void)dialog->SetOptions(fos);
			}

			ApplyCommonFileDialogSettings(
				dialog,
				options.initialDirectory,
				U"",
				options.title,
				options.acceptButtonText);
		}

		static void ApplyFilters(IFileDialog* dialog, const Array<FileDialogFilter>& filters)
		{
			if (dialog == nullptr)
			{
				return;
			}

			const DialogFilterData filterData = BuildFilterSpecs(filters);
			if (filterData.specs.isEmpty())
			{
				return;
			}

			// SetFileTypes() は 1 回しか呼べない
			(void)dialog->SetFileTypes(
				static_cast<UINT>(filterData.specs.size()),
				filterData.specs.data());

			// 先頭を初期選択
			(void)dialog->SetFileTypeIndex(1);
		}

		[[nodiscard]]
		static HRESULT ShowDialog(IFileDialog* dialog)
		{
			if (dialog == nullptr)
			{
				return E_POINTER;
			}

			const HWND hWnd = static_cast<HWND>(SIV3D_ENGINE(Window)->getHandle());
			return dialog->Show(hWnd);
		}

		[[nodiscard]]
		static Array<FilePath> GetOpenDialogResults(IFileOpenDialog* dialog, const bool multiSelect)
		{
			Array<FilePath> results;

			if (dialog == nullptr)
			{
				return results;
			}

			if (multiSelect)
			{
				ComPtr<IShellItemArray> items;
				if (FAILED(dialog->GetResults(&items)))
				{
					return results;
				}

				DWORD count = 0;
				if (FAILED(items->GetCount(&count)))
				{
					return results;
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
					return results;
				}

				if (auto path = GetFilePathFromShellItem(item.Get()))
				{
					results.push_back(std::move(*path));
				}
			}

			return results;
		}

		[[nodiscard]]
		static String GetDefaultExtensionFromFilter(const Array<FileDialogFilter>& filters, const uint32 filterIndex)
		{
			// IFileDialog::GetFileTypeIndex() は 1-based
			if ((filterIndex == 0) || (filters.size() < filterIndex))
			{
				return{};
			}

			for (const auto& extension : filters[filterIndex - 1].extensions)
			{
				if (not extension.isEmpty())
				{
					return extension;
				}
			}

			return{};
		}

		[[nodiscard]]
		static bool HasExtension(const FilePath& path)
		{
			return (not FileSystem::Extension(path).isEmpty());
		}

		[[nodiscard]]
		static FilePath AppendExtension(const FilePath& path, const String& extension)
		{
			if (path.isEmpty() || extension.isEmpty())
			{
				return path;
			}

			return FileSystem::FullPath(path + U"." + extension);
		}

		[[nodiscard]]
		static Array<FileDialogFilter> GetEffectiveSaveFilters(const SaveFileDialogOptions& options)
		{
			return NormalizeFilters(options.filters);
		}

		[[nodiscard]]
		static Optional<FilePath> PostProcessSavePath(
			IFileSaveDialog* dialog,
			const Array<FileDialogFilter>& effectiveFilters,
			const SaveFileDialogOptions& options,
			FilePath path)
		{
			if (path.isEmpty())
			{
				return none;
			}

			if (options.addExtensionIfMissing && (not HasExtension(path)))
			{
				UINT filterIndex = 0;
				if (SUCCEEDED(dialog->GetFileTypeIndex(&filterIndex)))
				{
					const String extension = GetDefaultExtensionFromFilter(effectiveFilters, filterIndex);

					if (not extension.isEmpty())
					{
						path = AppendExtension(path, extension);
					}
				}
			}

			return path;
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

			ApplyOpenDialogOptions(dialog.Get(), options, multiSelect);
			ApplyFilters(dialog.Get(), NormalizeFilters(options.filters));

			const HRESULT hr = ShowDialog(dialog.Get());

			if (hr == HRESULT_FROM_WIN32(ERROR_CANCELLED))
			{
				return{};
			}

			if (FAILED(hr))
			{
				return{};
			}

			return GetOpenDialogResults(dialog.Get(), multiSelect);
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

		////////////////////////////////////////////////////////////////
		//
		//	SaveFile
		//
		////////////////////////////////////////////////////////////////

		Optional<FilePath> SaveFile(const SaveFileDialogOptions& options)
		{
			ComPtr<IFileSaveDialog> dialog;
			if (FAILED(::CoCreateInstance(
				CLSID_FileSaveDialog,
				nullptr,
				CLSCTX_INPROC_SERVER,
				IID_PPV_ARGS(&dialog))))
			{
				return none;
			}

			const Array<FileDialogFilter> effectiveFilters = GetEffectiveSaveFilters(options);

			ApplySaveDialogOptions(dialog.Get(), options);
			ApplyFilters(dialog.Get(), effectiveFilters);

			const HRESULT hr = ShowDialog(dialog.Get());

			if (hr == HRESULT_FROM_WIN32(ERROR_CANCELLED))
			{
				return none;
			}

			if (FAILED(hr))
			{
				return none;
			}

			ComPtr<IShellItem> item;
			if (FAILED(dialog->GetResult(&item)))
			{
				return none;
			}

			auto path = GetFilePathFromShellItem(item.Get());
			if (not path)
			{
				return none;
			}

			return PostProcessSavePath(dialog.Get(), effectiveFilters, options, std::move(*path));
		}

		////////////////////////////////////////////////////////////////
		//
		//	SelectFolder
		//
		////////////////////////////////////////////////////////////////

		Optional<FilePath> SelectFolder(const SelectFolderDialogOptions& options)
		{
			ComPtr<IFileOpenDialog> dialog;
			if (FAILED(::CoCreateInstance(
				CLSID_FileOpenDialog,
				nullptr,
				CLSCTX_INPROC_SERVER,
				IID_PPV_ARGS(&dialog))))
			{
				return none;
			}

			ApplySelectFolderDialogOptions(dialog.Get(), options);

			const HRESULT hr = ShowDialog(dialog.Get());

			if (hr == HRESULT_FROM_WIN32(ERROR_CANCELLED))
			{
				return none;
			}

			if (FAILED(hr))
			{
				return none;
			}

			ComPtr<IShellItem> item;
			if (FAILED(dialog->GetResult(&item)))
			{
				return none;
			}

			return GetFilePathFromShellItem(item.Get());
		}
	}
}
