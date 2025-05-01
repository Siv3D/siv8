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
# include <Siv3D/Array.hpp>
# include <Siv3D/Byte.hpp>
# include <Siv3D/PointVector.hpp>
# include <Siv3D/ColorHSV.hpp>
# include <Siv3D/TextureFormat.hpp>

namespace s3d
{
	[[nodiscard]]
	Array<Byte> GenerateInitialColorBuffer(const Size& size, const ColorF& color, const TextureFormat& format);

	void FillWithColor(void* const pDst, size_t bufferSizeBytes, const ColorF& color, const TextureFormat& format);
	
	void FillWithImage(void* const pDst, const Size& size, const uint32 dstBytesPerRow, std::span<const Byte> src, const uint32 srcBytesPerRow);
}
