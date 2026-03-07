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
# include <Siv3D/Stopwatch.hpp>
# include <Siv3D/Duration.hpp>
# include <Siv3D/Pentablet/IPentablet.hpp>

namespace s3d
{
	class CPentablet final : public ISiv3DPentablet
	{
	public:

		~CPentablet() override;

		void initLibrary() override;

		void resetDevice() override;

		void update() override;

		void onProximity(bool inProximity, const Optional<bool>& isPen) override;

		void onPenMove(double normalPressure, double tangentialPressure, double tiltX, double tiltY) override;

		bool isAvailable() override;

		bool isConnected() override;

		const String& getName() override;

		const PenCaps& getCaps() override;

		const PenState& getState() override;

	private:
		
		// この時間以上 tablet のイベントが発生しなかった場合、ペンが離れたと判断する閾値
		static constexpr Duration ProximityTimeout{ 0.25 };
		
		bool m_available = false;
		
		Stopwatch m_proximityStopwatch;

		struct DeviceInfo
		{
			String name;

			PenCaps caps;

			PenState state;

		} m_deviceInfo;
	};
}
