//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2024 Ryo Suzuki
//	Copyright (c) 2016-2024 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once
# include <Siv3D/Common.hpp>
# include <Siv3D/Array.hpp>
# include "CursorState.hpp"

namespace s3d
{
	struct Mat3x2;

	class SIV3D_NOVTABLE ISiv3DCursor
	{
	public:

		[[nodiscard]]
		static ISiv3DCursor* Create();

		virtual ~ISiv3DCursor() = default;

		virtual void init() = 0;

		virtual bool update() = 0;

		virtual void updateHighTemporalResolutionCursorPos(Point rawClientPos) = 0;

		virtual const CursorState& getState() const = 0;

		virtual Array<std::pair<int64, Point>> getHighTemporalResolutionCursorPos() const = 0;

		//virtual void setPos(Point pos) = 0;

		//virtual const Mat3x2& getBaseWindowTransform() const noexcept = 0;

		//virtual const Mat3x2& getCameraTransform() const noexcept = 0;

		//virtual const Mat3x2& getLocalTransform() const noexcept = 0;

		//virtual void setBaseWindowTransform(const Mat3x2& matrix) = 0;

		//virtual void setCameraTransform(const Mat3x2& matrix) = 0;

		//virtual void setLocalTransform(const Mat3x2& matrix) = 0;
	};
}
