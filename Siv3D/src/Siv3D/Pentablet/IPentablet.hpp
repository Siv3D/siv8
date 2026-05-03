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
# include <Siv3D/Optional.hpp>

namespace s3d
{
	struct PenCaps;
	struct PenState;

	class SIV3D_NOVTABLE ISiv3DPentablet
	{
	public:

		[[nodiscard]]
		static ISiv3DPentablet* Create();

		virtual ~ISiv3DPentablet() = default;

		virtual void initLibrary() = 0;

		virtual void resetDevice() = 0;

		virtual void update() = 0;

		virtual void onProximity(bool inProximity, const Optional<bool>& isPen) = 0;

		virtual void onPenMove(double normalPressure, double tangentialPressure, double tiltX, double tiltY) = 0;

		[[nodiscard]]
		virtual bool isAvailable() = 0;

		[[nodiscard]]
		virtual bool isConnected() = 0;

		[[nodiscard]]
		virtual const String& getName() = 0;

		[[nodiscard]]
		virtual const PenCaps& getCaps() = 0;

		[[nodiscard]]
		virtual const PenState& getState() = 0;
	};
}
