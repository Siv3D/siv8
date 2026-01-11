//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2026 Ryo Suzuki
//	Copyright (c) 2016-2026 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once
# include <Siv3D/Common.hpp>
# include <Siv3D/StringView.hpp>

namespace s3d
{
	class SIV3D_NOVTABLE ISiv3DPrint
	{
	public:

		[[nodiscard]]
		static ISiv3DPrint* Create();

		virtual ~ISiv3DPrint() = default;

		virtual void init() = 0;

		virtual void write(StringView s) = 0;

		virtual void writeln(StringView s) = 0;

		virtual void clear() = 0;

		virtual void draw() = 0;
	};
}
