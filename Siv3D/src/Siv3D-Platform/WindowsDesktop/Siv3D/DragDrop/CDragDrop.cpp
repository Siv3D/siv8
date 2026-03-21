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

# include "CDragDrop.hpp"
# include <Siv3D/Time.hpp>
# include <Siv3D/FileSystem.hpp>
# include <Siv3D/Scene.hpp>
# include <Siv3D/Window/IWindow.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>
# include <Siv3D/EngineLog.hpp>
# include <ShlObj_core.h>

namespace s3d
{
	namespace detail
	{
		namespace
		{
			[[nodiscard]]
			static constexpr FORMATETC MakeFormatEtc(const CLIPFORMAT format) noexcept
			{
				return FORMATETC
				{
					.cfFormat = format,
					.ptd = nullptr,
					.dwAspect = DVASPECT_CONTENT,
					.lindex = -1,
					.tymed = TYMED_HGLOBAL,
				};
			}

			[[nodiscard]]
			static Point ScreenToClientPoint(const HWND hWnd, const POINTL& pt) noexcept
			{
				POINT p{ pt.x, pt.y };
				::ScreenToClient(hWnd, &p);
				return{ p.x, p.y };
			}

			[[nodiscard]]
			static bool HasFormat(IDataObject* pDataObject, const CLIPFORMAT format)
			{
				FORMATETC formatEtc = MakeFormatEtc(format);
				return SUCCEEDED(pDataObject->QueryGetData(&formatEtc));
			}

			[[nodiscard]]
			static Optional<DragItemType> DetectDragItemType(IDataObject* pDataObject)
			{
				// 両方解釈可能な場合は FilePaths を優先
				if (HasFormat(pDataObject, CF_HDROP))
				{
					return DragItemType::FilePaths;
				}

				if (HasFormat(pDataObject, CF_UNICODETEXT))
				{
					return DragItemType::Text;
				}

				return none;
			}

			[[nodiscard]]
			static DWORD AllowedEffect(const bool accepted) noexcept
			{
				return accepted ? DROPEFFECT_COPY : DROPEFFECT_NONE;
			}

			class StgMediumHolder
			{
			public:

				STGMEDIUM medium{};

				~StgMediumHolder()
				{
					if (medium.tymed != TYMED_NULL)
					{
						::ReleaseStgMedium(&medium);
					}
				}
			};

			class GlobalLockGuard
			{
			public:

				explicit GlobalLockGuard(HGLOBAL hGlobal)
					: m_hGlobal(hGlobal)
					, m_pData(::GlobalLock(hGlobal)) {
				}

				~GlobalLockGuard()
				{
					if (m_pData)
					{
						::GlobalUnlock(m_hGlobal);
					}
				}

				[[nodiscard]]
				explicit operator bool() const noexcept
				{
					return (m_pData != nullptr);
				}

				template <class T>
				[[nodiscard]]
				T* get() const noexcept
				{
					return static_cast<T*>(m_pData);
				}

			private:

				HGLOBAL m_hGlobal = nullptr;
				void* m_pData = nullptr;
			};

			[[nodiscard]]
			static bool ReadDroppedFilePaths(IDataObject* pDataObject, const Point pos, const uint64 timeMillisec, Array<DroppedFilePath>& dst)
			{
				FORMATETC formatEtc = MakeFormatEtc(CF_HDROP);
				StgMediumHolder stgMedium;

				if (FAILED(pDataObject->GetData(&formatEtc, &stgMedium.medium)))
				{
					return false;
				}

				const HDROP hDrop = static_cast<HDROP>(stgMedium.medium.hGlobal);
				const uint32 numDrops = static_cast<uint32>(::DragQueryFileW(hDrop, 0xFFFFFFFFu, nullptr, 0));

				Array<DroppedFilePath> items;
				items.reserve(numDrops);

				for (uint32 i = 0; i < numDrops; ++i)
				{
					const uint32 length = static_cast<uint32>(::DragQueryFileW(hDrop, i, nullptr, 0));

					std::wstring path(length + 1, L'\0');
					::DragQueryFileW(hDrop, i, path.data(), (length + 1));
					path.resize(length);

					items.push_back(DroppedFilePath
						{
							.path = Unicode::FromWstring(path),
							.pos = pos,
							.timeMillisec = timeMillisec,
						});
				}

				dst.swap(items);
				return true;
			}

			[[nodiscard]]
			static bool ReadDroppedText(IDataObject* pDataObject, const Point pos, const uint64 timeMillisec, DroppedText& dst)
			{
				FORMATETC formatEtc = MakeFormatEtc(CF_UNICODETEXT);
				StgMediumHolder stgMedium;

				if (FAILED(pDataObject->GetData(&formatEtc, &stgMedium.medium)))
				{
					return false;
				}

				GlobalLockGuard lock(stgMedium.medium.hGlobal);

				if (not lock)
				{
					return false;
				}

				const wchar_t* pText = lock.get<const wchar_t>();

				dst = DroppedText
				{
					.text = Unicode::FromWstring(pText),
					.pos = pos,
					.timeMillisec = timeMillisec,
				};

				return true;
			}

			static void CopyFormatEtc(FORMATETC& dst, const FORMATETC& src)
			{
				dst = src;

				if (src.ptd)
				{
					dst.ptd = static_cast<DVTARGETDEVICE*>(::CoTaskMemAlloc(sizeof(DVTARGETDEVICE)));

					if (dst.ptd)
					{
						*(dst.ptd) = *(src.ptd);
					}
				}
			}

			[[nodiscard]]
			static bool MatchesFormatEtc(const FORMATETC& requested, const FORMATETC& stored) noexcept
			{
				return ((requested.cfFormat == stored.cfFormat)
					&& (requested.dwAspect == stored.dwAspect)
					&& (requested.lindex == stored.lindex)
					&& (requested.tymed & stored.tymed));
			}

			[[nodiscard]]
			static HGLOBAL DuplicateHGlobal(HGLOBAL hGlobal)
			{
				if (hGlobal == nullptr)
				{
					return nullptr;
				}

				const SIZE_T size = ::GlobalSize(hGlobal);

				if (size == 0)
				{
					return nullptr;
				}

				HGLOBAL copied = ::GlobalAlloc(GMEM_MOVEABLE, size);

				if (copied == nullptr)
				{
					return nullptr;
				}

				void* pSrc = ::GlobalLock(hGlobal);
				void* pDst = ::GlobalLock(copied);

				if ((pSrc == nullptr) || (pDst == nullptr))
				{
					if (pDst)
					{
						::GlobalUnlock(copied);
					}

					if (pSrc)
					{
						::GlobalUnlock(hGlobal);
					}

					::GlobalFree(copied);
					return nullptr;
				}

				std::memcpy(pDst, pSrc, size);

				::GlobalUnlock(copied);
				::GlobalUnlock(hGlobal);

				return copied;
			}

			class CEnumFormatEtc final : public IEnumFORMATETC
			{
			public:

				explicit CEnumFormatEtc(const FORMATETC& formatEtc)
				{
					CopyFormatEtc(m_formatEtc, formatEtc);
				}

				~CEnumFormatEtc()
				{
					::CoTaskMemFree(m_formatEtc.ptd);
				}

				HRESULT __stdcall QueryInterface(REFIID iid, void** ppvObject) override
				{
					if ((iid == IID_IUnknown) || (iid == IID_IEnumFORMATETC))
					{
						*ppvObject = static_cast<IEnumFORMATETC*>(this);
						AddRef();
						return S_OK;
					}

					*ppvObject = nullptr;
					return E_NOINTERFACE;
				}

				ULONG __stdcall AddRef() override
				{
					return ++m_refCount;
				}

				ULONG __stdcall Release() override
				{
					const ULONG count = --m_refCount;

					if (count == 0)
					{
						delete this;
					}

					return count;
				}

				HRESULT __stdcall Next(ULONG celt, FORMATETC* rgelt, ULONG* pceltFetched) override
				{
					if ((celt == 0) || (rgelt == nullptr))
					{
						return E_INVALIDARG;
					}

					if (m_index != 0)
					{
						if (pceltFetched)
						{
							*pceltFetched = 0;
						}

						return S_FALSE;
					}

					CopyFormatEtc(rgelt[0], m_formatEtc);
					m_index = 1;

					if (pceltFetched)
					{
						*pceltFetched = 1;
					}

					return (celt == 1) ? S_OK : S_FALSE;
				}

				HRESULT __stdcall Skip(ULONG celt) override
				{
					m_index = Min<size_t>(1, (m_index + celt));
					return (m_index < 1) ? S_OK : S_FALSE;
				}

				HRESULT __stdcall Reset() override
				{
					m_index = 0;
					return S_OK;
				}

				HRESULT __stdcall Clone(IEnumFORMATETC** ppEnum) override
				{
					if (ppEnum == nullptr)
					{
						return E_POINTER;
					}

					auto* p = new CEnumFormatEtc{ m_formatEtc };
					p->m_index = m_index;
					*ppEnum = p;
					return S_OK;
				}

			private:

				std::atomic<ULONG> m_refCount{ 1 };
				size_t m_index = 0;
				FORMATETC m_formatEtc{};
			};

			class DataObject final : public IDataObject
			{
			public:

				DataObject(const FORMATETC& formatEtc, STGMEDIUM&& medium)
					: m_medium(std::move(medium))
				{
					CopyFormatEtc(m_formatEtc, formatEtc);
				}

				~DataObject()
				{
					::CoTaskMemFree(m_formatEtc.ptd);
					::ReleaseStgMedium(&m_medium);
				}

				HRESULT __stdcall QueryInterface(REFIID iid, void** ppvObject) override
				{
					if ((iid == IID_IUnknown) || (iid == IID_IDataObject))
					{
						*ppvObject = static_cast<IDataObject*>(this);
						AddRef();
						return S_OK;
					}

					*ppvObject = nullptr;
					return E_NOINTERFACE;
				}

				ULONG __stdcall AddRef() override
				{
					return ++m_refCount;
				}

				ULONG __stdcall Release() override
				{
					const ULONG count = --m_refCount;

					if (count == 0)
					{
						delete this;
					}

					return count;
				}

				HRESULT __stdcall GetData(FORMATETC* pFormatEtc, STGMEDIUM* pMedium) override
				{
					if ((pFormatEtc == nullptr) || (pMedium == nullptr))
					{
						return E_POINTER;
					}

					if (not MatchesFormatEtc(*pFormatEtc, m_formatEtc))
					{
						return DV_E_FORMATETC;
					}

					if (m_medium.tymed != TYMED_HGLOBAL)
					{
						return DV_E_TYMED;
					}

					HGLOBAL copied = DuplicateHGlobal(m_medium.hGlobal);

					if (copied == nullptr)
					{
						return STG_E_MEDIUMFULL;
					}

					pMedium->tymed = TYMED_HGLOBAL;
					pMedium->hGlobal = copied;
					pMedium->pUnkForRelease = nullptr;
					return S_OK;
				}

				HRESULT __stdcall GetDataHere(FORMATETC*, STGMEDIUM*) override
				{
					return DATA_E_FORMATETC;
				}

				HRESULT __stdcall QueryGetData(FORMATETC* pFormatEtc) override
				{
					if (pFormatEtc == nullptr)
					{
						return E_POINTER;
					}

					return MatchesFormatEtc(*pFormatEtc, m_formatEtc) ? S_OK : DV_E_FORMATETC;
				}

				HRESULT __stdcall GetCanonicalFormatEtc(FORMATETC*, FORMATETC* pFormatEtcOut) override
				{
					if (pFormatEtcOut)
					{
						pFormatEtcOut->ptd = nullptr;
					}

					return E_NOTIMPL;
				}

				HRESULT __stdcall SetData(FORMATETC*, STGMEDIUM*, BOOL) override
				{
					return E_NOTIMPL;
				}

				HRESULT __stdcall EnumFormatEtc(DWORD dwDirection, IEnumFORMATETC** ppEnumFormatEtc) override
				{
					if (ppEnumFormatEtc == nullptr)
					{
						return E_POINTER;
					}

					if (dwDirection != DATADIR_GET)
					{
						return E_NOTIMPL;
					}

					*ppEnumFormatEtc = new CEnumFormatEtc{ m_formatEtc };
					return S_OK;
				}

				HRESULT __stdcall DAdvise(FORMATETC*, DWORD, IAdviseSink*, DWORD*) override
				{
					return OLE_E_ADVISENOTSUPPORTED;
				}

				HRESULT __stdcall DUnadvise(DWORD) override
				{
					return OLE_E_ADVISENOTSUPPORTED;
				}

				HRESULT __stdcall EnumDAdvise(IEnumSTATDATA**) override
				{
					return OLE_E_ADVISENOTSUPPORTED;
				}

			private:

				std::atomic<ULONG> m_refCount{ 1 };
				FORMATETC m_formatEtc{};
				STGMEDIUM m_medium{};
			};

			class DropSource final : public IDropSource
			{
			public:

				HRESULT __stdcall QueryInterface(REFIID iid, void** ppvObject) override
				{
					if ((iid == IID_IUnknown) || (iid == IID_IDropSource))
					{
						*ppvObject = static_cast<IDropSource*>(this);
						AddRef();
						return S_OK;
					}

					*ppvObject = nullptr;
					return E_NOINTERFACE;
				}

				ULONG __stdcall AddRef() override
				{
					return ++m_refCount;
				}

				ULONG __stdcall Release() override
				{
					const ULONG count = --m_refCount;

					if (count == 0)
					{
						delete this;
					}

					return count;
				}

				HRESULT __stdcall QueryContinueDrag(BOOL escapePressed, DWORD keyState) override
				{
					if (escapePressed)
					{
						return DRAGDROP_S_CANCEL;
					}

					if ((keyState & (MK_LBUTTON | MK_RBUTTON)) == (MK_LBUTTON | MK_RBUTTON))
					{
						return DRAGDROP_S_CANCEL;
					}

					if ((keyState & (MK_LBUTTON | MK_RBUTTON)) == 0)
					{
						return DRAGDROP_S_DROP;
					}

					return S_OK;
				}

				HRESULT __stdcall GiveFeedback(DWORD) override
				{
					return DRAGDROP_S_USEDEFAULTCURSORS;
				}

			private:

				std::atomic<ULONG> m_refCount{ 1 };
			};

			[[nodiscard]]
			static HGLOBAL CreateHDrop(const Array<FilePath>& paths)
			{
				std::wstring data;

				for (const auto& path : paths)
				{
					const std::wstring fullPath = FileSystem::FullPath(path).toWstr();
					data.append(fullPath);
					data.push_back(L'\0');
				}

				data.push_back(L'\0');

				const SIZE_T bytes = (sizeof(DROPFILES) + (data.size() * sizeof(wchar_t)));
				HGLOBAL hGlobal = ::GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, bytes);

				if (hGlobal == nullptr)
				{
					return nullptr;
				}

				void* p = ::GlobalLock(hGlobal);

				if (p == nullptr)
				{
					::GlobalFree(hGlobal);
					return nullptr;
				}

				auto* pDropFiles = static_cast<DROPFILES*>(p);
				pDropFiles->pFiles = sizeof(DROPFILES);
				pDropFiles->pt = POINT{ 0, 0 };
				pDropFiles->fNC = FALSE;
				pDropFiles->fWide = TRUE;

				std::memcpy(static_cast<std::byte*>(p) + sizeof(DROPFILES), data.data(), (data.size() * sizeof(wchar_t)));

				::GlobalUnlock(hGlobal);
				return hGlobal;
			}

			[[nodiscard]]
			static HGLOBAL CreateUnicodeText(const String& text)
			{
				const std::wstring wtext = text.toWstr();
				const SIZE_T bytes = ((wtext.size() + 1) * sizeof(wchar_t));

				HGLOBAL hGlobal = ::GlobalAlloc(GMEM_MOVEABLE, bytes);

				if (hGlobal == nullptr)
				{
					return nullptr;
				}

				void* p = ::GlobalLock(hGlobal);

				if (p == nullptr)
				{
					::GlobalFree(hGlobal);
					return nullptr;
				}

				std::memcpy(p, wtext.c_str(), bytes);

				::GlobalUnlock(hGlobal);
				return hGlobal;
			}

			[[nodiscard]]
			static ComPtr<IDataObject> CreateFileDataObject(const Array<FilePath>& paths)
			{
				ComPtr<IDataObject> result;

				HGLOBAL hGlobal = CreateHDrop(paths);

				if (hGlobal == nullptr)
				{
					return result;
				}

				FORMATETC formatEtc = MakeFormatEtc(CF_HDROP);

				STGMEDIUM medium{};
				medium.tymed = TYMED_HGLOBAL;
				medium.hGlobal = hGlobal;
				medium.pUnkForRelease = nullptr;

				result.Attach(new DataObject{ formatEtc, std::move(medium) });
				return result;
			}

			[[nodiscard]]
			static ComPtr<IDataObject> CreateTextDataObject(const String& text)
			{
				ComPtr<IDataObject> result;

				HGLOBAL hGlobal = CreateUnicodeText(text);

				if (hGlobal == nullptr)
				{
					return result;
				}

				FORMATETC formatEtc = MakeFormatEtc(CF_UNICODETEXT);

				STGMEDIUM medium{};
				medium.tymed = TYMED_HGLOBAL;
				medium.hGlobal = hGlobal;
				medium.pUnkForRelease = nullptr;

				result.Attach(new DataObject{ formatEtc, std::move(medium) });
				return result;
			}
		}

		struct DropTargetSnapshot
		{
			Array<DroppedFilePath> droppedFilePaths;
			Array<DroppedText> droppedTexts;
			Optional<DragStatus> dragOver;
		};

		class DropTargetState
		{
		public:

			void setAcceptFilePaths(const bool accept) noexcept
			{
				m_acceptFilePaths.store(accept, std::memory_order_relaxed);
			}

			void setAcceptText(const bool accept) noexcept
			{
				m_acceptText.store(accept, std::memory_order_relaxed);
			}

			[[nodiscard]]
			bool accepts(const DragItemType itemType) const noexcept
			{
				if (itemType == DragItemType::FilePaths)
				{
					return m_acceptFilePaths.load(std::memory_order_relaxed);
				}
				else
				{
					return m_acceptText.load(std::memory_order_relaxed);
				}
			}

			void setDragOver(const Point pos, const DragItemType itemType)
			{
				std::lock_guard lock{ m_mutex };
				m_dragOver = DragStatus
				{
					.cursorPos = pos,
					.itemType = itemType,
				};
			}

			void clearDragOver()
			{
				std::lock_guard lock{ m_mutex };
				m_dragOver.reset();
			}

			void pushDroppedFilePaths(Array<DroppedFilePath>&& items)
			{
				std::lock_guard lock{ m_mutex };

				m_dragOver.reset();

				if (items.isEmpty())
				{
					return;
				}

				m_droppedFilePaths.insert(m_droppedFilePaths.end(),
					std::make_move_iterator(items.begin()),
					std::make_move_iterator(items.end()));
			}

			void pushDroppedText(DroppedText&& item)
			{
				std::lock_guard lock{ m_mutex };
				m_dragOver.reset();
				m_droppedTexts.push_back(std::move(item));
			}

			[[nodiscard]]
			DropTargetSnapshot drain()
			{
				std::lock_guard lock{ m_mutex };

				DropTargetSnapshot snapshot;
				snapshot.droppedFilePaths.swap(m_droppedFilePaths);
				snapshot.droppedTexts.swap(m_droppedTexts);
				snapshot.dragOver = m_dragOver;
				return snapshot;
			}

		private:

			std::atomic_bool m_acceptFilePaths{ true };
			std::atomic_bool m_acceptText{ false };

			mutable std::mutex m_mutex;

			Array<DroppedFilePath> m_droppedFilePaths;
			Array<DroppedText> m_droppedTexts;
			Optional<DragStatus> m_dragOver;
		};

		class DropTarget final : public IDropTarget
		{
		public:

			DropTarget(HWND hWnd, std::shared_ptr<DropTargetState> pState)
				: m_hWnd(hWnd)
				, m_pState(std::move(pState)) {
			}

			HRESULT __stdcall QueryInterface(REFIID iid, void** ppvObject) override
			{
				if ((iid == IID_IUnknown) || (iid == IID_IDropTarget))
				{
					*ppvObject = static_cast<IDropTarget*>(this);
					AddRef();
					return S_OK;
				}

				*ppvObject = nullptr;
				return E_NOINTERFACE;
			}

			ULONG __stdcall AddRef() override
			{
				return ++m_refCount;
			}

			ULONG __stdcall Release() override
			{
				const ULONG count = --m_refCount;

				if (count == 0)
				{
					delete this;
				}

				return count;
			}

			HRESULT __stdcall DragEnter(IDataObject* pDataObject, DWORD, POINTL pt, DWORD* pdwEffect) override
			{
				const Point clientPos = ScreenToClientPoint(m_hWnd, pt);

				m_currentItemType = DetectDragItemType(pDataObject);

				if (m_currentItemType && m_pState->accepts(*m_currentItemType))
				{
					m_pState->setDragOver(clientPos, *m_currentItemType);
					*pdwEffect = AllowedEffect(true);
				}
				else
				{
					m_pState->clearDragOver();
					*pdwEffect = AllowedEffect(false);
				}

				return S_OK;
			}

			HRESULT __stdcall DragOver(DWORD, POINTL pt, DWORD* pdwEffect) override
			{
				const Point clientPos = ScreenToClientPoint(m_hWnd, pt);

				if (m_currentItemType && m_pState->accepts(*m_currentItemType))
				{
					m_pState->setDragOver(clientPos, *m_currentItemType);
					*pdwEffect = AllowedEffect(true);
				}
				else
				{
					m_pState->clearDragOver();
					*pdwEffect = AllowedEffect(false);
				}

				return S_OK;
			}

			HRESULT __stdcall DragLeave() override
			{
				m_currentItemType.reset();
				m_pState->clearDragOver();
				return S_OK;
			}

			HRESULT __stdcall Drop(IDataObject* pDataObject, DWORD, POINTL pt, DWORD* pdwEffect) override
			{
				const Point clientPos = ScreenToClientPoint(m_hWnd, pt);
				const uint64 timeMillisec = Time::GetMillisec();

				m_pState->clearDragOver();

				const Optional<DragItemType> itemType = DetectDragItemType(pDataObject);
				m_currentItemType.reset();

				if (not itemType)
				{
					*pdwEffect = AllowedEffect(false);
					return S_OK;
				}

				if (not m_pState->accepts(*itemType))
				{
					*pdwEffect = AllowedEffect(false);
					return S_OK;
				}

				if (*itemType == DragItemType::FilePaths)
				{
					Array<DroppedFilePath> items;

					if (not ReadDroppedFilePaths(pDataObject, clientPos, timeMillisec, items))
					{
						*pdwEffect = AllowedEffect(false);
						return E_FAIL;
					}

					m_pState->pushDroppedFilePaths(std::move(items));
					*pdwEffect = AllowedEffect(true);
					return S_OK;
				}
				else
				{
					DroppedText item;

					if (not ReadDroppedText(pDataObject, clientPos, timeMillisec, item))
					{
						*pdwEffect = AllowedEffect(false);
						return E_FAIL;
					}

					m_pState->pushDroppedText(std::move(item));
					*pdwEffect = AllowedEffect(true);
					return S_OK;
				}
			}

		private:

			HWND m_hWnd = nullptr;
			std::shared_ptr<DropTargetState> m_pState;
			std::atomic<ULONG> m_refCount{ 1 };
			Optional<DragItemType> m_currentItemType;
		};
	}

	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	CDragDrop::CDragDrop() = default;

	////////////////////////////////////////////////////////////////
	//
	//	(destructor)
	//
	////////////////////////////////////////////////////////////////

	CDragDrop::~CDragDrop()
	{
		LOG_SCOPED_DEBUG("CDragDrop::~CDragDrop()");

		if (m_registered)
		{
			::RevokeDragDrop(m_hWnd);
			m_registered = false;
		}

		m_pDropTarget.Reset();
		m_pDropTargetState.reset();
	}

	////////////////////////////////////////////////////////////////
	//
	//	init
	//
	////////////////////////////////////////////////////////////////

	void CDragDrop::init()
	{
		LOG_SCOPED_DEBUG("CDragDrop::init()");

		m_hWnd = static_cast<HWND>(SIV3D_ENGINE(Window)->getHandle());
		
		m_pDropTargetState = std::make_shared<detail::DropTargetState>();
		m_pDropTargetState->setAcceptFilePaths(m_acceptFilePaths.load(std::memory_order_relaxed));
		m_pDropTargetState->setAcceptText(m_acceptText.load(std::memory_order_relaxed));

		auto* pDropTarget = new detail::DropTarget{ m_hWnd, m_pDropTargetState };
		m_pDropTarget.Attach(pDropTarget);

		LOG_TRACE("RegisterDragDrop()");

		const HRESULT hr = ::RegisterDragDrop(m_hWnd, m_pDropTarget.Get());

		if (FAILED(hr))
		{
			LOG_WARN("RegisterDragDrop() failed");
		}

		m_registered = true;
	}

	////////////////////////////////////////////////////////////////
	//
	//	update
	//
	////////////////////////////////////////////////////////////////

	void CDragDrop::update()
	{
		if (not m_pDropTargetState)
		{
			return;
		}

		detail::DropTargetSnapshot snapshot = m_pDropTargetState->drain();

		m_droppedFilePaths.insert(m_droppedFilePaths.end(),
			std::make_move_iterator(snapshot.droppedFilePaths.begin()),
			std::make_move_iterator(snapshot.droppedFilePaths.end()));

		m_droppedTexts.insert(m_droppedTexts.end(),
			std::make_move_iterator(snapshot.droppedTexts.begin()),
			std::make_move_iterator(snapshot.droppedTexts.end()));

		m_dragOver = std::move(snapshot.dragOver);
	}

	////////////////////////////////////////////////////////////////
	//
	//	acceptFilePaths
	//
	////////////////////////////////////////////////////////////////

	void CDragDrop::acceptFilePaths(const bool accept)
	{
		m_acceptFilePaths.store(accept, std::memory_order_relaxed);

		if (m_pDropTargetState)
		{
			m_pDropTargetState->setAcceptFilePaths(accept);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	acceptText
	//
	////////////////////////////////////////////////////////////////

	void CDragDrop::acceptText(const bool accept)
	{
		m_acceptText.store(accept, std::memory_order_relaxed);

		if (m_pDropTargetState)
		{
			m_pDropTargetState->setAcceptText(accept);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	dragOver
	//
	////////////////////////////////////////////////////////////////

	Optional<DragStatus> CDragDrop::dragOver() const
	{
		if (not m_dragOver)
		{
			return none;
		}

		DragStatus status = *m_dragOver;
		status.cursorPos = Scene::ClientToScene(status.cursorPos).asPoint();
		return status;
	}

	////////////////////////////////////////////////////////////////
	//
	//	hasNewFilePaths
	//
	////////////////////////////////////////////////////////////////

	bool CDragDrop::hasNewFilePaths() const
	{
		return (not m_droppedFilePaths.isEmpty());
	}

	////////////////////////////////////////////////////////////////
	//
	//	hasNewText
	//
	////////////////////////////////////////////////////////////////

	bool CDragDrop::hasNewText() const
	{
		return (not m_droppedTexts.isEmpty());
	}

	////////////////////////////////////////////////////////////////
	//
	//	clear
	//
	////////////////////////////////////////////////////////////////

	void CDragDrop::clear()
	{
		m_droppedFilePaths.clear();
		m_droppedTexts.clear();
	}

	////////////////////////////////////////////////////////////////
	//
	//	extractDroppedFilePaths
	//
	////////////////////////////////////////////////////////////////

	Array<DroppedFilePath> CDragDrop::extractDroppedFilePaths()
	{
		Array<DroppedFilePath> result(std::move(m_droppedFilePaths));

		for (auto& item : result)
		{
			item.path = FileSystem::FullPath(item.path);
			item.pos = Scene::ClientToScene(item.pos).asPoint();
		}

		return result;
	}

	////////////////////////////////////////////////////////////////
	//
	//	extractDroppedTexts
	//
	////////////////////////////////////////////////////////////////

	Array<DroppedText> CDragDrop::extractDroppedTexts()
	{
		Array<DroppedText> result(std::move(m_droppedTexts));

		for (auto& item : result)
		{
			item.pos = Scene::ClientToScene(item.pos).asPoint();
		}

		return result;
	}

	////////////////////////////////////////////////////////////////
	//
	//	beginDragFile
	//
	////////////////////////////////////////////////////////////////

	bool CDragDrop::beginDragFile(const FilePathView path)
	{
		const FilePath fullPath = FileSystem::FullPath(path);

		if (fullPath.isEmpty() || (not FileSystem::IsFile(fullPath)))
		{
			return false;
		}

		std::lock_guard lock{ m_dragRequestMutex };

		if (m_dragInProgress.load(std::memory_order_relaxed) || m_dragRequest)
		{
			return false;
		}

		m_dragRequest = DragRequest
		{
			.type = DragRequestType::FilePaths,
			.filePaths = { fullPath },
		};

		return true;
	}

	////////////////////////////////////////////////////////////////
	//
	//	beginDragFiles
	//
	////////////////////////////////////////////////////////////////

	bool CDragDrop::beginDragFiles(const Array<FilePath>& paths)
	{
		if (paths.isEmpty())
		{
			return false;
		}

		Array<FilePath> fullPaths;
		fullPaths.reserve(paths.size());

		for (const auto& path : paths)
		{
			const FilePath fullPath = FileSystem::FullPath(path);

			if (fullPath.isEmpty() || (not FileSystem::IsFile(fullPath)))
			{
				return false;
			}

			fullPaths.push_back(fullPath);
		}

		std::lock_guard lock{ m_dragRequestMutex };

		if (m_dragInProgress.load(std::memory_order_relaxed) || m_dragRequest)
		{
			return false;
		}

		m_dragRequest = DragRequest
		{
			.type = DragRequestType::FilePaths,
			.filePaths = std::move(fullPaths),
		};

		return true;
	}

	////////////////////////////////////////////////////////////////
	//
	//	beginDragText
	//
	////////////////////////////////////////////////////////////////

	bool CDragDrop::beginDragText(const StringView text)
	{
		const String s{ text };

		if (s.isEmpty())
		{
			return false;
		}

		std::lock_guard lock{ m_dragRequestMutex };

		if (m_dragInProgress.load(std::memory_order_relaxed) || m_dragRequest)
		{
			return false;
		}

		m_dragRequest = DragRequest
		{
			.type = DragRequestType::Text,
			.text = s,
		};

		return true;
	}

	////////////////////////////////////////////////////////////////
	//
	//	process
	//
	////////////////////////////////////////////////////////////////

	void CDragDrop::process()
	{
		if (m_dragInProgress.load(std::memory_order_relaxed))
		{
			return;
		}

		Optional<DragRequest> request;

		{
			std::lock_guard lock{ m_dragRequestMutex };

			if (not m_dragRequest)
			{
				return;
			}

			request = std::move(m_dragRequest);
			m_dragRequest.reset();
		}

		if (not request)
		{
			return;
		}

		m_dragInProgress.store(true, std::memory_order_relaxed);

		struct ScopeGuard
		{
			std::atomic_bool& flag;

			~ScopeGuard()
			{
				flag.store(false, std::memory_order_relaxed);
			}
		} guard{ m_dragInProgress };

		ComPtr<IDataObject> pDataObject;
		ComPtr<IDropSource> pDropSource;

		if (request->type == DragRequestType::FilePaths)
		{
			if (request->filePaths.isEmpty())
			{
				return;
			}

			pDataObject = detail::CreateFileDataObject(request->filePaths);
		}
		else
		{
			if (request->text.isEmpty())
			{
				return;
			}

			pDataObject = detail::CreateTextDataObject(request->text);
		}

		if (not pDataObject)
		{
			return;
		}

		pDropSource.Attach(new detail::DropSource{});

		DWORD effect = DROPEFFECT_NONE;
		const HRESULT hr = ::DoDragDrop(pDataObject.Get(), pDropSource.Get(), DROPEFFECT_COPY, &effect);

		if ((hr != DRAGDROP_S_DROP) && (hr != DRAGDROP_S_CANCEL))
		{
			LOG_WARN("DoDragDrop() failed");
		}
	}
}
