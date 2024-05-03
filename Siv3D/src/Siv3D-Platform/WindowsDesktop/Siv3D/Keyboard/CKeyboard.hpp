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
# include <Siv3D/Keyboard/IKeyboard.hpp>

namespace s3d
{
	class CKeyboard final : public ISiv3DKeyboard
	{
	public:

		CKeyboard() = default;

		~CKeyboard() override;

		void init() override;

		bool update() override;

	private:

	};
}
