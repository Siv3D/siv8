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

namespace s3d
{
	inline bool AnimatedImage::isEmpty() const noexcept
	{
		return frames.empty();
	}

	inline AnimatedImage::operator bool() const noexcept
	{
		return (not frames.empty());
	}

	inline size_t AnimatedImage::size() const noexcept
	{
		return frames.size();
	}
}
