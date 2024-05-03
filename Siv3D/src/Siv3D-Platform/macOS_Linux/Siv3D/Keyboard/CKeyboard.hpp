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
# include <Siv3D/InputState.hpp>
# include <Siv3D/Keyboard.hpp>
# include <Siv3D/Keyboard/IKeyboard.hpp>
# include <Siv3D/GLFW/GLFW.hpp>

namespace s3d
{
	class CKeyboard final : public ISiv3DKeyboard
	{
	public:

		CKeyboard() = default;

		~CKeyboard() override;

		void init() override;

		void update() override;

		InputState& getInputState(size_t index) noexcept override;

		const String& name(uint32 index) const noexcept override;

		const Array<Input>& getAllInput() const noexcept override;
		
	private:

		GLFWwindow* m_window = nullptr;

		std::array<String, Keyboard::NumKeys> m_names;

		std::array<InputState, Keyboard::NumKeys> m_states;

		Array<Input> m_allInputs;
	};
}
