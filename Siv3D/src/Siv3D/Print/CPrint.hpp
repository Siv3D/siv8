//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2025 Ryo Suzuki
//	Copyright (c) 2016-2025 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once
# include "IPrint.hpp"
# include "PrintFont.hpp"

namespace s3d
{
	class CPrint final : public ISiv3DPrint
	{
	public:

		CPrint();

		~CPrint() override;

		void init() override;

		void write(StringView s) override;

		void writeln(StringView s) override;

		void clear() override;

		void draw() override;

	private:

		std::unique_ptr<PrintFont> m_font;
	};
}
