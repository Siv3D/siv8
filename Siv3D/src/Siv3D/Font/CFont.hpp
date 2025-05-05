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
# include "IFont.hpp"

namespace s3d
{
	class CFont final : public ISiv3DFont
	{
	public:

		CFont();

		~CFont();

		void init() override;

	private:

	};
}
