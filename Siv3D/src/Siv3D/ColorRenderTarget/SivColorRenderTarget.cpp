//-----------------------------------------------
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//-----------------------------------------------

# include <Siv3D/ColorRenderTarget.hpp>
# include <Siv3D/Texture/ITexture.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>

namespace s3d
{
	MSAASampleCount ColorRenderTarget::getMSAASampleCount() const
	{
		return isEmpty() ? MSAASampleCount::NoMSAA
			: static_cast<MSAASampleCount>(SIV3D_ENGINE(Texture)->getSampleCount(id()));
	}

	const ColorRenderTarget& ColorRenderTarget::clear(const ColorF& color) const
	{
		if (not isEmpty())
		{
			SIV3D_ENGINE(Texture)->clearRT(id(), color);
		}
		return *this;
	}

	void ColorRenderTarget::resolve() const
	{
		if (not isEmpty())
		{
			// Both backends return before recording commands for non-MS resources.
			SIV3D_ENGINE(Texture)->resolveMSRT(id());
		}
	}

	void ColorRenderTarget::generateMips() const
	{
		if (not isEmpty() && (1 < m_texture.mipLevels()))
		{
			SIV3D_ENGINE(Texture)->generateMips(id());
		}
	}
}
