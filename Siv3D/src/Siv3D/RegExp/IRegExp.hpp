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
# include <Siv3D/Common.hpp>

namespace s3d
{
	class SIV3D_NOVTABLE ISiv3DRegExp
	{
	public:

		[[nodiscard]]
		static ISiv3DRegExp* Create();

		virtual ~ISiv3DRegExp() = default;

		virtual void init() = 0;
	};
}
