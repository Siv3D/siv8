//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2026 Ryo Suzuki
//
//	Copyright (c) 2016-2026 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include "Siv3DTest.hpp"

# if SIV3D_PLATFORM(MACOS)

# include <Siv3D/Engine/Siv3DEngine.hpp>
# include <Siv3D/Keyboard/IKeyboard.hpp>
# include <Siv3D/Window/IWindow.hpp>
# include <Siv3D/GLFW/GLFW.hpp>

extern "C" const char* glfwGetKeysSiv3D(GLFWwindow* window);

namespace s3d
{
	namespace
	{
		// Inject cached GLFW states to test frame polling without changing the OS
		// input source or generating text. Native scancodes are tested in Test_GLFW.
		class KeyboardFixture
		{
		public:

			KeyboardFixture()
			{
				keyboard->init();
				std::copy_n(keys, originalKeys.size(), originalKeys.begin());
				std::fill_n(keys, originalKeys.size(), GLFW_RELEASE);
				for (uint32 i = 0; i < Keyboard::NumKeys; ++i)
				{
					originalStates[i] = engineKeyboard->getInputState(i);
					// The isolated keyboard must not report user actions through the
					// engine keyboard's Input::down() while exercising its own states.
					engineKeyboard->getInputState(i).reset();
				}
			}

			~KeyboardFixture()
			{
				std::copy(originalKeys.begin(), originalKeys.end(), keys);
				for (uint32 i = 0; i < Keyboard::NumKeys; ++i)
				{
					engineKeyboard->getInputState(i) = originalStates[i];
				}
			}

			void check(const Input input, const bool down, const bool pressed, const bool up)
			{
				const auto& state = keyboard->getInputState(input.code());
				CHECK(state.down() == down);
				CHECK(state.pressed() == pressed);
				CHECK(state.up() == up);
				CHECK(keyboard->getAllInput().contains(input) == (pressed || up));
			}

			std::unique_ptr<ISiv3DKeyboard> keyboard{ ISiv3DKeyboard::Create() };
			char* keys = const_cast<char*>(glfwGetKeysSiv3D(
				static_cast<GLFWwindow*>(SIV3D_ENGINE(Window)->getHandle())));

		private:

			ISiv3DKeyboard* engineKeyboard = SIV3D_ENGINE(Keyboard);
			std::array<char, (GLFW_KEY_LAST + 1)> originalKeys{};
			std::array<InputState, Keyboard::NumKeys> originalStates;
		};
	}

	TEST_CASE("Keyboard.BackslashAndYenStateTransitions")
	{
		KeyboardFixture fixture;
		CHECK(KeyYen_JIS == KeyBackslash_US);
		for (const int key : { GLFW_KEY_BACKSLASH, SIV3D_KEY_JIS_YEN })
		{
			CAPTURE(key);
			fixture.keys[key] = GLFW_PRESS;
			fixture.keyboard->update();
			fixture.check(KeyYen_JIS, true, true, false);
			CHECK(fixture.keyboard->getAllInput() == Array<Input>{ KeyYen_JIS });

			fixture.keyboard->update();
			fixture.check(KeyYen_JIS, false, true, false);
			const Duration heldDuration = fixture.keyboard->getInputState(KeyYen_JIS.code()).pressedDuration;

			fixture.keys[key] = GLFW_RELEASE;
			fixture.keyboard->update();
			fixture.check(KeyYen_JIS, false, false, true);
			CHECK(fixture.keyboard->getInputState(KeyYen_JIS.code()).pressedDuration >= heldDuration);

			fixture.keyboard->update();
			fixture.check(KeyYen_JIS, false, false, false);
			CHECK(fixture.keyboard->getAllInput().isEmpty());
		}
	}

	TEST_CASE("Keyboard.BackslashAndYenOverlappingPresses")
	{
		KeyboardFixture fixture;
		fixture.keys[GLFW_KEY_BACKSLASH] = GLFW_PRESS;
		fixture.keyboard->update();
		fixture.check(KeyYen_JIS, true, true, false);
		fixture.keys[SIV3D_KEY_JIS_YEN] = GLFW_PRESS;
		fixture.keyboard->update();
		fixture.check(KeyYen_JIS, false, true, false);
		fixture.keys[GLFW_KEY_BACKSLASH] = GLFW_RELEASE;
		fixture.keyboard->update();
		fixture.check(KeyYen_JIS, false, true, false);
		fixture.keys[SIV3D_KEY_JIS_YEN] = GLFW_RELEASE;
		fixture.keyboard->update();
		fixture.check(KeyYen_JIS, false, false, true);
	}

	TEST_CASE("Keyboard.MacF13AndClearAliases")
	{
		KeyboardFixture fixture;
		const std::pair<Input, int> bindings[] = {
			{ KeyF13, GLFW_KEY_PRINT_SCREEN },
			{ KeyClear, GLFW_KEY_NUM_LOCK },
		};
		for (const auto& [input, key] : bindings)
		{
			CAPTURE(input.code());
			fixture.check(input, false, false, false);
			fixture.keys[key] = GLFW_PRESS;
			fixture.keyboard->update();
			fixture.check(input, true, true, false);
			if (input == KeyF13)
			{
				fixture.check(KeyPrintScreen, true, true, false);
			}
			if (input == KeyClear)
			{
				fixture.check(KeyNumLock, true, true, false);
			}
			fixture.keyboard->update();
			fixture.check(input, false, true, false);
			fixture.keys[key] = GLFW_RELEASE;
			fixture.keyboard->update();
			fixture.check(input, false, false, true);
			if (input == KeyF13)
			{
				fixture.check(KeyPrintScreen, false, false, true);
			}
			if (input == KeyClear)
			{
				fixture.check(KeyNumLock, false, false, true);
			}
			fixture.keyboard->update();
			fixture.check(input, false, false, false);
			CHECK(fixture.keyboard->getAllInput().isEmpty());
		}
	}

	TEST_CASE("Keyboard.KeyNamesDoNotGenerateGLFWErrors")
	{
		glfwGetError(nullptr);
		const std::unique_ptr<ISiv3DKeyboard> keyboard{ ISiv3DKeyboard::Create() };
		keyboard->init();
		CHECK(glfwGetError(nullptr) == GLFW_NO_ERROR);
		CHECK(keyboard->getName(KeyShift.code()) == U"Shift");
		CHECK(keyboard->getName(KeyCommand.code()) == U"Command");
		CHECK(keyboard->getName(KeyClear.code()) == U"Clear");
		CHECK(keyboard->getName(KeyF13.code()) == U"F13");
		for (const Input input : { KeyYen_JIS, KeyUnderscore_JIS })
		{
			CAPTURE(input.code());
			CHECK_FALSE(keyboard->getName(input.code()).isEmpty());
			CHECK_FALSE(keyboard->getName(input.code()).starts_with(U"0x"));
		}
	}

}

# endif
