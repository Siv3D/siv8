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
# include <Siv3D/Image.hpp>

namespace s3d
{
	class SIV3D_NOVTABLE ISiv3DEmoji
	{
	public:

		[[nodiscard]]
		static ISiv3DEmoji* Create();

		virtual ~ISiv3DEmoji() = default;

		virtual void init() = 0;

		virtual bool hasEmoji(StringView emoji) const = 0;

		virtual Image renderEmoji(StringView emoji, const Size& size) = 0;
	};
}
