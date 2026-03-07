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

# pragma once
# include <Siv3D/Common.hpp>
# include <Siv3D/Array.hpp>
# include <Siv3D/PenCaps.hpp>
# include <Siv3D/PenState.hpp>
# include <Siv3D/Pentablet/IPentablet.hpp>
# include <Siv3D/Windows/Windows.hpp>

# define PACKETDATA	(PK_CURSOR | PK_NORMAL_PRESSURE | PK_TANGENT_PRESSURE | PK_ORIENTATION | PK_ROTATION)
# define PACKETMODE	PK_BUTTONS
# include <ThirdParty/Wintab/MSGPACK.H>
# include <ThirdParty/Wintab/WINTAB.H>
# include <ThirdParty/Wintab/PKTDEF.H>

namespace s3d
{
	class CPentablet final : public ISiv3DPentablet
	{
	public:

		~CPentablet() override;

		void initLibrary() override;

		void resetDevice() override;

		void update() override;

		void onProximity(bool inProximity) override;

		bool isAvailable() override;

		bool isConnected() override;

		const String& getName() override;

		const PenCaps& getCaps() override;

		const PenState& getState() override;

	private:

		bool m_initialized = false;

		struct Wintab
		{
			HINSTANCE library = nullptr;

			decltype(WTInfoW)* p_WTInfoW = nullptr;
			decltype(WTClose)* p_WTClose = nullptr;
			decltype(WTOpenW)* p_WTOpenW = nullptr;
			decltype(WTQueueSizeGet)* p_WTQueueSizeGet = nullptr;
			decltype(WTPacketsGet)* p_WTPacketsGet = nullptr;

			bool available = false;

			[[nodiscard]]
			bool isConnected() const;
		
		} m_wintab;

		HCTX m_context = nullptr;

		bool m_connected = false;

		std::atomic_bool m_inProximity{ false };

		struct DeviceInfo
		{
			String name;

			int32 packetRate = 0;

			AXIS pressureCaps{};
			AXIS tangentPressureCaps{};
			
			AXIS azimuthCaps{};
			AXIS altitudeCaps{};
			AXIS twistCaps{};

			AXIS pitchCaps{};
			AXIS rollCaps{};
			AXIS yawCaps{};

			PenCaps caps;

			PenState state;

		} m_deviceInfo;

		Array<PACKET> m_packetsBuffer;

		void closeContext();
	};
}
