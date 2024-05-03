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
# include <Siv3D/Input.hpp>

namespace s3d
{
	struct InputState;

	class SIV3D_NOVTABLE ISiv3DKeyboard
	{
	public:

		[[nodiscard]]
		static ISiv3DKeyboard* Create();

		virtual ~ISiv3DKeyboard() = default;

		virtual void init() = 0;

		virtual void update() = 0;

		virtual InputState& getInputState(uint32 index) noexcept = 0;

		virtual const String& getName(uint32 index) const noexcept = 0;

		virtual const Array<Input>& getAllInput() const noexcept = 0;

		//virtual Array<KeyEvent> getEvents() const noexcept = 0;
	};
}
