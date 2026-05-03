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

# include "CPentablet.hpp"
# include <Siv3D/DLL.hpp>
# include <Siv3D/Error/InternalEngineError.hpp>
# include <Siv3D/Window/IWindow.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>
# include <Siv3D/EngineLog.hpp>

namespace s3d
{
	namespace
	{
		[[nodiscard]]
		static double GetValue(const int32 value, const AXIS& caps) noexcept
		{
			if (const auto range = (caps.axMax - caps.axMin))
			{
				return (static_cast<double>(value) / caps.axMax);
			}
			else
			{
				return 0.0;
			}
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	isConnected
	//
	////////////////////////////////////////////////////////////////

	bool CPentablet::Wintab::isConnected() const
	{
		if (not available)
		{
			return false;
		}

		if (UINT numDevices = 0;
			0 < p_WTInfoW(WTI_INTERFACE, IFC_NDEVICES, &numDevices))
		{
			return (0 < numDevices);
		}
		else
		{
			return false;
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	(destructor)
	//
	////////////////////////////////////////////////////////////////

	CPentablet::~CPentablet()
	{
		LOG_SCOPED_DEBUG("CPentablet::~CPentablet()");

		closeContext();

		DLL::Unload(m_wintab.library);
	}

	////////////////////////////////////////////////////////////////
	//
	//	initLibrary
	//
	////////////////////////////////////////////////////////////////

	void CPentablet::initLibrary()
	{
		assert(not m_initialized);
		m_initialized = true;

		m_wintab.library = DLL::LoadSystemLibraryNoThrow(L"Wintab32.dll");
		if (not m_wintab.library)
		{
			LOG_INFO("ℹ️ Wintab is not available");
			return;
		}

		try
		{
			m_wintab.p_WTInfoW			= DLL::GetFunction(m_wintab.library, "WTInfoW");
			m_wintab.p_WTClose			= DLL::GetFunction(m_wintab.library, "WTClose");
			m_wintab.p_WTOpenW			= DLL::GetFunction(m_wintab.library, "WTOpenW");
			m_wintab.p_WTQueueSizeGet	= DLL::GetFunction(m_wintab.library, "WTQueueSizeGet");
			m_wintab.p_WTPacketsGet		= DLL::GetFunction(m_wintab.library, "WTPacketsGet");
		}
		catch (const InternalEngineError&)
		{
			LOG_INFO("ℹ️ Failed to load Wintab functions");
			return;
		}

		m_wintab.available = true;

		resetDevice();
	}

	////////////////////////////////////////////////////////////////
	//
	//	resetDevice
	//
	////////////////////////////////////////////////////////////////

	void CPentablet::resetDevice()
	{
		if (not m_wintab.available)
		{
			return;
		}

		closeContext();

		// デバイス情報をリセット
		m_deviceInfo = {};
		m_connected = m_wintab.isConnected();

		// コンテキスト作成
		{
			LOGCONTEXTW context{};
			if (not m_wintab.p_WTInfoW(WTI_DEFCONTEXT, 0, &context))
			{
				LOG_INFO(U"✖ WTInfoW(WTI_DEFCONTEXT, 0) failed");
				return;
			}

			::wsprintfW(context.lcName, L"Siv3D Wintab %p", ::GetModuleHandleW(nullptr));
			context.lcOptions	|= CXO_SYSTEM;
			context.lcPktData	= PACKETDATA;
			context.lcPktMode	= PACKETMODE;
			context.lcMoveMask	= PACKETDATA;
			context.lcBtnUpMask	= context.lcBtnDnMask;

			const HWND hWnd = static_cast<HWND>(SIV3D_ENGINE(Window)->getHandle());
			m_context = m_wintab.p_WTOpenW(hWnd, &context, TRUE);

			if (not m_context)
			{
				LOG_INFO("✖ WTOpenW failed");
				return;
			}
		}

		// デバイス名
		if (wchar_t name[256]{}; m_wintab.p_WTInfoW(WTI_DEVICES, DVC_NAME, name))
		{
			m_deviceInfo.name = Unicode::FromWstring(name);
			LOG_INFO(fmt::format("ℹ️ Pentablet: {}", Unicode::FromWstring(name)));
		}
		else
		{
			LOG_INFO("✖ WTInfoW(WTI_DEVICES, DVC_NAME) failed");
		}

		// PnP ID
		if (wchar_t pnpId[256]{}; m_wintab.p_WTInfoW(WTI_DEVICES, DVC_PNPID, pnpId))
		{
			if (const String pid = Unicode::FromWstring(pnpId))
			{
				LOG_INFO(fmt::format("ℹ️ PID: {}", Unicode::FromWstring(pnpId)));
			}
		}
		else
		{
			LOG_INFO("✖ WTInfoW(WTI_DEVICES, DVC_PNPID) failed");
		}

		// デバイスの種類
		if (UINT flag = 0; m_wintab.p_WTInfoW(WTI_DEVICES, DVC_HARDWARE, &flag))
		{
			LOG_INFO((flag & HWC_INTEGRATED) ? "ℹ️ Device type: integrated" : "ℹ️ Device type: non-integrated");
		}

		// デバイスの能力
		{
			// パケットレート
			{
				if (UINT packetRate = 0; 
					0 < m_wintab.p_WTInfoW(WTI_DEVICES, DVC_PKTRATE, &packetRate))
				{
					LOG_INFO(fmt::format("ℹ️ Packet Rate: {} packets/s", packetRate));
					m_deviceInfo.packetRate = static_cast<int32>(packetRate);
				}
				else
				{
					LOG_INFO("✖ WTInfoW(WTI_DEVICES, DVC_PKTRATE) failed");
				}
			}

			// normal pressure
			{
				m_deviceInfo.caps.normalPressure = (0 < m_wintab.p_WTInfoW(WTI_DEVICES, DVC_NPRESSURE, &m_deviceInfo.pressureCaps));

				if (m_deviceInfo.caps.normalPressure)
				{
					m_deviceInfo.caps.normalPressureLevels = static_cast<int32>(m_deviceInfo.pressureCaps.axMax - m_deviceInfo.pressureCaps.axMin + 1);
					LOG_INFO(fmt::format("ℹ️ Normal Pressure: {} levels", *m_deviceInfo.caps.normalPressureLevels));
				}
			}

			// tangential pressure
			{
				m_deviceInfo.caps.tangentialPressure = (0 < m_wintab.p_WTInfoW(WTI_DEVICES, DVC_TPRESSURE, &m_deviceInfo.tangentPressureCaps));

				if (m_deviceInfo.caps.tangentialPressure)
				{
					m_deviceInfo.caps.tangentialPressureLevels = static_cast<int32>(m_deviceInfo.tangentPressureCaps.axMax - m_deviceInfo.tangentPressureCaps.axMin + 1);
					LOG_INFO(fmt::format("ℹ️ Tangential Pressure: {} levels", *m_deviceInfo.caps.tangentialPressureLevels));
				}
			}

			// orientation
			{
				AXIS orientationCaps[3]{};
				m_deviceInfo.caps.orientation = (0 < m_wintab.p_WTInfoW(WTI_DEVICES, DVC_ORIENTATION, orientationCaps));
				m_deviceInfo.azimuthCaps	= orientationCaps[0];
				m_deviceInfo.altitudeCaps	= orientationCaps[1];
				m_deviceInfo.twistCaps		= orientationCaps[2];

				if (m_deviceInfo.caps.orientation)
				{
					LOG_INFO(fmt::format("ℹ️ Orientation: azimuth(min = {}, max = {}), altitude(min = {}, max = {}), twist(min = {}, max = {})",
						m_deviceInfo.azimuthCaps.axMin, m_deviceInfo.azimuthCaps.axMax,
						m_deviceInfo.altitudeCaps.axMin, m_deviceInfo.altitudeCaps.axMax,
						m_deviceInfo.twistCaps.axMin, m_deviceInfo.twistCaps.axMax));
				}
			}

			// rotation
			{
				AXIS rotationCaps[3]{};
				m_deviceInfo.caps.rotation = (0 < m_wintab.p_WTInfoW(WTI_DEVICES, DVC_ROTATION, rotationCaps));
				m_deviceInfo.pitchCaps	= rotationCaps[0];
				m_deviceInfo.rollCaps	= rotationCaps[1];
				m_deviceInfo.yawCaps	= rotationCaps[2];

				if (m_deviceInfo.caps.rotation)
				{
					LOG_INFO(fmt::format("ℹ️ Rotation: pitch(min = {}, max = {}), roll(min = {}, max = {}), yaw(min = {}, max = {})",
						m_deviceInfo.pitchCaps.axMin, m_deviceInfo.pitchCaps.axMax,
						m_deviceInfo.rollCaps.axMin, m_deviceInfo.rollCaps.axMax,
						m_deviceInfo.yawCaps.axMin, m_deviceInfo.yawCaps.axMax));
				}
			}
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	update
	//
	////////////////////////////////////////////////////////////////

	void CPentablet::update()
	{
		if (not m_initialized)
		{
			return;
		}

		const bool oldConnected = m_connected;
		m_connected = m_wintab.isConnected();

		if ((not oldConnected) && m_connected)
		{
			LOG_INFO("✅ Pentablet connected");
			resetDevice();
		}
		else if (oldConnected && (not m_connected))
		{
			LOG_INFO("📤 Pentablet disconnected");
			closeContext();
		}

		if (not m_connected)
		{
			m_inProximity.store(false, std::memory_order_relaxed);
			m_deviceInfo.state = {};
			return;
		}

		const int32 queueSize = m_wintab.p_WTQueueSizeGet(m_context);
		m_packetsBuffer.resize(queueSize);

		const bool inProximity = m_inProximity.load(std::memory_order_relaxed);
		m_deviceInfo.state.inProximity = inProximity;

		if (const int32 n = m_wintab.p_WTPacketsGet(m_context, static_cast<int32>(m_packetsBuffer.size()), m_packetsBuffer.data()))
		{
			const auto& packet = m_packetsBuffer[n - 1];

			if (packet.pkCursor == 1) // ペン
			{
				m_deviceInfo.state.isPen = true;
				m_deviceInfo.state.isEraser = false;
			}
			if (packet.pkCursor == 2) // 消しゴム
			{
				m_deviceInfo.state.isPen = false;
				m_deviceInfo.state.isEraser = true;
			}

			if (m_deviceInfo.caps.normalPressure)
			{
				m_deviceInfo.state.normalPressure = GetValue(packet.pkNormalPressure, m_deviceInfo.pressureCaps);
			}

			if (m_deviceInfo.caps.tangentialPressure)
			{
				m_deviceInfo.state.tangentialPressure = GetValue(packet.pkTangentPressure, m_deviceInfo.tangentPressureCaps);
			}

			if (m_deviceInfo.caps.orientation)
			{
				m_deviceInfo.state.azimuth	= (GetValue(packet.pkOrientation.orAzimuth, m_deviceInfo.azimuthCaps) * 360.0);
				m_deviceInfo.state.altitude	= (GetValue(packet.pkOrientation.orAltitude, m_deviceInfo.altitudeCaps) * 90.0);
				m_deviceInfo.state.twist	= GetValue(packet.pkOrientation.orTwist, m_deviceInfo.twistCaps);
			}

			if (m_deviceInfo.caps.rotation)
			{
				m_deviceInfo.state.pitch	= GetValue(packet.pkRotation.roPitch, m_deviceInfo.pitchCaps);
				m_deviceInfo.state.roll		= GetValue(packet.pkRotation.roRoll, m_deviceInfo.rollCaps);
				m_deviceInfo.state.yaw		= GetValue(packet.pkRotation.roYaw, m_deviceInfo.yawCaps);
			}
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	onProximity
	//
	////////////////////////////////////////////////////////////////

	void CPentablet::onProximity(const bool inProximity, const Optional<bool>&)
	{
		m_inProximity.store(inProximity, std::memory_order_relaxed);
	}

	////////////////////////////////////////////////////////////////
	//
	//	onPenMove
	//
	////////////////////////////////////////////////////////////////

	void CPentablet::onPenMove(double, double, double, double) {} // Windows 版では不使用

	////////////////////////////////////////////////////////////////
	//
	//	isAvailable
	//
	////////////////////////////////////////////////////////////////

	bool CPentablet::isAvailable()
	{
		if (not m_initialized)
		{
			initLibrary();
		}
		
		return m_wintab.available;
	}

	////////////////////////////////////////////////////////////////
	//
	//	isConnected
	//
	////////////////////////////////////////////////////////////////

	bool CPentablet::isConnected()
	{
		if (not m_initialized)
		{
			initLibrary();
		}

		return m_connected;
	}

	////////////////////////////////////////////////////////////////
	//
	//	getName
	//
	////////////////////////////////////////////////////////////////

	const String& CPentablet::getName()
	{
		if (not m_initialized)
		{
			initLibrary();
		}
	
		return m_deviceInfo.name;
	}

	////////////////////////////////////////////////////////////////
	//
	//	getCaps
	//
	////////////////////////////////////////////////////////////////

	const PenCaps& CPentablet::getCaps()
	{
		if (not m_initialized)
		{
			initLibrary();
		}

		return m_deviceInfo.caps;
	}

	////////////////////////////////////////////////////////////////
	//
	//	getState
	//
	////////////////////////////////////////////////////////////////

	const PenState& CPentablet::getState()
	{
		if (not m_initialized)
		{
			initLibrary();
		}

		return m_deviceInfo.state;
	}

	////////////////////////////////////////////////////////////////
	//
	//	(private function)
	//
	////////////////////////////////////////////////////////////////

	void CPentablet::closeContext()
	{
		if (not m_context)
		{
			return;
		}

		m_wintab.p_WTClose(m_context);
		m_context = nullptr;
	}
}
