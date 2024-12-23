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

# include "D3D11TextureDesc.hpp"

namespace s3d
{
	D3D11_TEXTURE2D_DESC D3D11TextureDesc::makeD3D11_TEXTURE2D_DESC(const uint32 bindFlags, const D3D11_USAGE usage, const uint32 CPUAccessFlags, const std::pair<uint32, uint32>& multiSample, const uint32 miscFlags) const noexcept
	{
		return{
			.Width			= static_cast<uint32>(size.x),
			.Height			= static_cast<uint32>(size.y),
			.MipLevels		= mipLevels,
			.ArraySize		= 1,
			.Format			= DXGIFormat(),
			.SampleDesc		= { multiSample.first, multiSample.second },
			.Usage			= usage,
			.BindFlags		= bindFlags,
			.CPUAccessFlags	= CPUAccessFlags,
			.MiscFlags		= miscFlags
		};
	}

	[[nodiscard]]
	D3D11_SHADER_RESOURCE_VIEW_DESC D3D11TextureDesc::makeD3D11_SHADER_RESOURCE_VIEW_DESC() const noexcept
	{
		return{
			.Format			= DXGIFormat(),
			.ViewDimension	= (isMSAA() ? D3D11_SRV_DIMENSION_TEXTURE2DMS : D3D11_SRV_DIMENSION_TEXTURE2D),
			.Texture2D		= { 0, mipLevels }
		};
	}
}
