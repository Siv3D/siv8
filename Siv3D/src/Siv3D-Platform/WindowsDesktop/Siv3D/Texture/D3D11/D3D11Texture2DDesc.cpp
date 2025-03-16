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

# include "D3D11Texture2DDesc.hpp"

namespace s3d
{
	namespace
	{
		static constexpr std::array TextureTypeStrings =
		{
			U"Default"_sv,
			U"Dynamic"_sv,
			U"Render"_sv,
			U"MSRender"_sv,
		};
	}

	////////////////////////////////////////////////////////////////
	//
	//	makeD3D11_TEXTURE2D_DESC
	//
	////////////////////////////////////////////////////////////////

	D3D11_TEXTURE2D_DESC D3D11Texture2DDesc::makeD3D11_TEXTURE2D_DESC(const uint32 bindFlags, const D3D11_USAGE usage, const uint32 CPUAccessFlags, const std::pair<uint32, uint32>& multiSample, const uint32 miscFlags) const noexcept
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

	////////////////////////////////////////////////////////////////
	//
	//	makeD3D11_SHADER_RESOURCE_VIEW_DESC
	//
	////////////////////////////////////////////////////////////////

	D3D11_SHADER_RESOURCE_VIEW_DESC D3D11Texture2DDesc::makeD3D11_SHADER_RESOURCE_VIEW_DESC() const noexcept
	{
		return{
			.Format			= DXGIFormat(),
			.ViewDimension	= (isMSAA() ? D3D11_SRV_DIMENSION_TEXTURE2DMS : D3D11_SRV_DIMENSION_TEXTURE2D),
			.Texture2D		= { 0, mipLevels }
		};
	}

	////////////////////////////////////////////////////////////////
	//
	//	toString
	//
	////////////////////////////////////////////////////////////////

	String D3D11Texture2DDesc::toString() const
	{
		return U"(Type: {0}, Size: {1}x{2}, MipLevels: {3}, Format: {4}, HasDepth: {5})"_fmt(
			TextureTypeStrings[FromEnum(type)],	size.x, size.y, mipLevels, format.name(), (hasDepth ? U"Yes" : U"No"));
	}
}
