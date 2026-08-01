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
# include <memory>
# include <Siv3D/Image.hpp>
# include <Siv3D/PredefinedYesNo.hpp>

namespace s3d
{
	class IReader;

	namespace detail
	{
		[[nodiscard]]
		Image DecodeFirstGIFFrame(
			std::unique_ptr<IReader> reader,
			PremultiplyAlpha premultiplyAlpha);
	}
}
