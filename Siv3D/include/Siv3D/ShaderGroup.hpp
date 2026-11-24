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
# include "Common.hpp"
# include "Optional.hpp"
# include "HLSL.hpp"
# include "MSL.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	ShaderGroup
	//
	////////////////////////////////////////////////////////////////

	/// @brief プラットフォームごとのシェーダをまとめます。
	/// @remark Windows では HLSL、macOS では MSL を使用します。
	/// @remark VertexShader または PixelShader への変換には、対象プラットフォーム用のシェーダが設定されている必要があります。作成に失敗した場合は空のシェーダを返します。
	class ShaderGroup
	{
	public:

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		ShaderGroup() = default;

		[[nodiscard]]
		ShaderGroup(const Optional<HLSL>& hlsl, const Optional<MSL>& msl);

		////////////////////////////////////////////////////////////////
		//
		//	operator VertexShader
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		operator VertexShader() const;

		////////////////////////////////////////////////////////////////
		//
		//	operator PixelShader
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		operator PixelShader() const;

	private:

		Optional<HLSL> m_hlsl;
		
		Optional<MSL>  m_msl;
	};
}
