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
# include <Siv3D/Console/IConsole.hpp>

namespace s3d
{
	class CConsole final : public ISiv3DConsole
	{
	public:

		CConsole();

		void open() override;

		bool attach() override;

		void close() override;
	};
}
