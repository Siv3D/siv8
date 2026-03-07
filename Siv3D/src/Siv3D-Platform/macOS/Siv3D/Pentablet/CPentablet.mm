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
# include <Siv3D/Math.hpp>
# include <Siv3D/Pentablet/IPentablet.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>
# include <Siv3D/EngineLog.hpp>
# import <Foundation/Foundation.h>
# include <IOKit/hid/IOHIDManager.h>
# include <IOKit/hid/IOHIDKeys.h>
# include <IOKit/hid/IOHIDUsageTables.h>

namespace s3d
{
	namespace
	{
		struct PenOrientation
		{
			/// @brief 方位角（0.0～360.0）
			double azimuth = 0.0;

			/// @brief 仰角（0.0～90.0）
			double altitude = 0.0;
		};
	
		PenOrientation CalcAzimuthAltitudeFromTilt(double tiltX, double tiltY)
		{
			constexpr double Eps = 1e-6;

			// 入力想定: tiltX, tiltY ∈ [-1.0, 1.0]
			tiltX = std::clamp(tiltX, -1.0, 1.0);
			tiltY = std::clamp(tiltY, -1.0, 1.0);

			// 正規化値 -> 傾斜角[deg]
			const double tiltXDeg = tiltX * 90.0;
			const double tiltYDeg = tiltY * 90.0;

			const double tiltXRad = Math::ToRadians(tiltXDeg);
			const double tiltYRad = Math::ToRadians(tiltYDeg);

			PenOrientation out;

			// 真上
			if ((std::abs(tiltXDeg) < Eps) && (std::abs(tiltYDeg) < Eps))
			{
				out.azimuth = 0.0;
				out.altitude = 90.0;
				return out;
			}

			// 真横に倒れている特異点
			// altitude は 0 になるが、azimuth は不安定/非一意になりやすい
			if ((std::abs(std::abs(tiltXDeg) - 90.0) < Eps) || (std::abs(std::abs(tiltYDeg) - 90.0) < Eps))
			{
				out.azimuth = 0.0;
				out.altitude = 0.0;
				return out;
			}

			// 傾きベクトル成分
			// tan(tiltX), tan(tiltY) を使うと、仕様系の tilt から方向と仰角を復元できる
			const double vx = std::tan(tiltXRad);
			const double vy = std::tan(tiltYRad);

			// 方位角:
			out.azimuth = Math::ToDegrees(std::atan2(vx, vy));
			if (out.azimuth < 0.0)
			{
				out.azimuth += 360.0;
			}

			// 仰角:
			out.altitude = Math::ToDegrees(std::atan(1.0 / std::sqrt(vx * vx + vy * vy)));

			return out;
		}
	
		void ReportPenEnter(const bool enter, const bool isPen)
		{
			SIV3D_ENGINE(Pentablet)->onProximity(enter, isPen);
		}

		void ReportTabletPoint(const double normalPressure, const double tangentialPressure, const double tiltX, const double tiltY)
		{
			SIV3D_ENGINE(Pentablet)->onPenMove(normalPressure, tangentialPressure, tiltX, tiltY);
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
	}

	////////////////////////////////////////////////////////////////
	//
	//	initLibrary
	//
	////////////////////////////////////////////////////////////////

	void CPentablet::initLibrary() {}

	////////////////////////////////////////////////////////////////
	//
	//	resetDevice
	//
	////////////////////////////////////////////////////////////////

	void CPentablet::resetDevice() {}

	////////////////////////////////////////////////////////////////
	//
	//	update
	//
	////////////////////////////////////////////////////////////////

	void CPentablet::update()
	{
		if (m_deviceInfo.state.inProximity && (ProximityTimeout < m_proximityStopwatch))
		{
			m_deviceInfo.state.inProximity = false;
			m_deviceInfo.state.normalPressure = 0.0;
			m_deviceInfo.state.tangentialPressure = 0.0;
			m_proximityStopwatch.reset();
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	onProximity
	//
	////////////////////////////////////////////////////////////////

	void CPentablet::onProximity(const bool inProximity, const Optional<bool>& isPen)
	{
		m_available = true;
		m_deviceInfo.state.inProximity = inProximity;
		m_deviceInfo.state.isEraser = (not isPen.value_or(true));
		
		if (not inProximity)
		{
			m_deviceInfo.state.normalPressure = 0.0;
			m_deviceInfo.state.tangentialPressure = 0.0;
			m_proximityStopwatch.reset();
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	onPenMove
	//
	////////////////////////////////////////////////////////////////

	void CPentablet::onPenMove(const double normalPressure, const double tangentialPressure, const double tiltX, const double tiltY)
	{
		m_available = true;
		m_deviceInfo.caps.normalPressure |= (normalPressure != 0.0);
		m_deviceInfo.caps.tangentialPressure |= (tangentialPressure != 0.0);
		m_deviceInfo.caps.orientation |= ((tiltX != 0.0) || (tiltY != 0.0));
		
		m_deviceInfo.state.normalPressure = normalPressure;
		m_deviceInfo.state.tangentialPressure = tangentialPressure;

		const PenOrientation orientation = CalcAzimuthAltitudeFromTilt(tiltX, tiltY);
		m_deviceInfo.state.azimuth  = orientation.azimuth;
		m_deviceInfo.state.altitude = orientation.altitude;
		
		if (m_deviceInfo.state.isEraser)
		{
			m_deviceInfo.state.altitude = -m_deviceInfo.state.altitude;
		}
		
		m_proximityStopwatch.restart();
	}

	////////////////////////////////////////////////////////////////
	//
	//	isAvailable
	//
	////////////////////////////////////////////////////////////////

	bool CPentablet::isAvailable()
	{
		return m_available;
	}

	////////////////////////////////////////////////////////////////
	//
	//	isConnected
	//
	////////////////////////////////////////////////////////////////

	bool CPentablet::isConnected()
	{
		return m_available;
	}

	////////////////////////////////////////////////////////////////
	//
	//	getName
	//
	////////////////////////////////////////////////////////////////

	const String& CPentablet::getName()
	{
		return m_deviceInfo.name;
	}

	////////////////////////////////////////////////////////////////
	//
	//	getCaps
	//
	////////////////////////////////////////////////////////////////

	const PenCaps& CPentablet::getCaps()
	{
		return m_deviceInfo.caps;
	}

	////////////////////////////////////////////////////////////////
	//
	//	getState
	//
	////////////////////////////////////////////////////////////////

	const PenState& CPentablet::getState()
	{
		return m_deviceInfo.state;
	}
}

extern "C"
{
	void GLFW_Siv3D_ReportPenEnter(const bool enter, const bool isPen)
	{
		s3d::ReportPenEnter(enter, isPen);
	}

	void GLFW_Siv3D_ReportTabletPoint(const double normalPressure, const double tangentialPressure, const double tiltX, const double tiltY)
	{
		s3d::ReportTabletPoint(normalPressure, tangentialPressure, tiltX, tiltY);
	}
}
