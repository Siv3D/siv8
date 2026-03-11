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
	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	template <ReaderObject Reader>
	Texture::Texture(Reader&& reader, const TextureDesc desc)
		: Texture{ std::make_unique<Reader>(std::forward<Reader>(reader)) } {}

	////////////////////////////////////////////////////////////////
	//
	//	horizontalAspectRatio
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	Float Texture::horizontalAspectRatio() const
	{
		return size().horizontalAspectRatio<Float>();
	}
}
