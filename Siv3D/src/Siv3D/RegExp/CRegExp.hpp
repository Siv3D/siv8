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
# include "IRegExp.hpp"

namespace s3d
{
	class CRegExp final : public ISiv3DRegExp
	{
	public:

		~CRegExp() override;

		void init() override;
	};
}
