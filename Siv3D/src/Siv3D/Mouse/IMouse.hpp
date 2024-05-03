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
# include <Siv3D/Optional.hpp>
# include <Siv3D/PointVector.hpp>
# include <Siv3D/Array.hpp>
# include <Siv3D/Input.hpp>

namespace s3d
{
	struct InputState;

	class SIV3D_NOVTABLE ISiv3DMouse
	{
	public:

		[[nodiscard]]
		static ISiv3DMouse* Create();

		virtual ~ISiv3DMouse() = default;

		virtual void init() = 0;

		virtual void update() = 0;

		virtual InputState& getInputState(uint32 index) noexcept = 0;

		virtual const Array<Input>& getAllInput() const noexcept = 0;

		virtual Vec2 wheel() const noexcept = 0;

		virtual void onScroll(double x, double y) = 0;

		virtual Optional<Point> getPrimaryTouchPos() = 0;
	};
}
