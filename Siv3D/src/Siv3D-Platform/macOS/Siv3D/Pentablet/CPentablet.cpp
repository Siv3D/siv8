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
# include <Siv3D/Pentablet/IPentablet.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>
# include <Siv3D/EngineLog.hpp>

namespace s3d
{
	namespace
	{
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
		if (not m_initialized)
		{
			return;
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	onProximity
	//
	////////////////////////////////////////////////////////////////

	void CPentablet::onProximity(const bool inProximity, const Optional<bool>& isPen)
	{
		LOG_INFO(fmt::format("onProximity: enter={}, isPen={}", inProximity, isPen.value_or(true)));
	}

	////////////////////////////////////////////////////////////////
	//
	//	onPenMove
	//
	////////////////////////////////////////////////////////////////

	void CPentablet::onPenMove(const double normalPressure, const double tangentialPressure, const double tiltX, const double tiltY)
	{
		LOG_INFO(fmt::format("onPenMove: normalPressure={}, tangentialPressure={}, tiltX={}, tiltY={}", normalPressure, tangentialPressure, tiltX, tiltY));
	}

	////////////////////////////////////////////////////////////////
	//
	//	isAvailable
	//
	////////////////////////////////////////////////////////////////

	bool CPentablet::isAvailable()
	{
		return true;
	}

	////////////////////////////////////////////////////////////////
	//
	//	isConnected
	//
	////////////////////////////////////////////////////////////////

	bool CPentablet::isConnected()
	{
		return false;
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
