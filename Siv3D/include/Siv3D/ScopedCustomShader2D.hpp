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
# include "VertexShader.hpp"
# include "PixelShader.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	ScopedCustomShader2D
	//
	////////////////////////////////////////////////////////////////

	/// @brief 2D 描画のカスタムシェーダを一時的に設定するスコープオブジェクト
	/// @remark 指定したステージのシェーダを保存・設定し、破棄時に復元します。Flush() とフレーム境界を越えて有効です。
	/// @remark 定数バッファ設定は変更・復元しません。定数バッファの復元には ScopedVSConstantBuffer2D / ScopedPSConstantBuffer2D を使用します。
	/// @remark 標準 VS 定数は先頭 48 バイトが変換行列、続く 16 バイトが float4 の ColorMul です。
	/// @remark HLSL は VS b0 の row_major float3x4、Metal は vertex buffer 1 の float3x4 として行列を読みます。
	/// @remark HLSL の mul(float3(position, 1), transform)、Metal の transform * float3(position, 1) が同次座標を返します。Z は 0、W はその結果を保持します。
	class ScopedCustomShader2D
	{
	public:

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		ScopedCustomShader2D() = default;

		[[nodiscard]]
		explicit ScopedCustomShader2D(const VertexShader& vs);

		[[nodiscard]]
		explicit ScopedCustomShader2D(const PixelShader& ps);

		[[nodiscard]]
		ScopedCustomShader2D(const VertexShader& vs, const PixelShader& ps);

		[[nodiscard]]
		ScopedCustomShader2D(const PixelShader& ps, const VertexShader& vs);

		[[nodiscard]]
		ScopedCustomShader2D(ScopedCustomShader2D&& other) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	(destructor)
		//
		////////////////////////////////////////////////////////////////

		~ScopedCustomShader2D();

		////////////////////////////////////////////////////////////////
		//
		//	(deleted)
		//
		////////////////////////////////////////////////////////////////

		ScopedCustomShader2D(const ScopedCustomShader2D&) = delete;

		ScopedCustomShader2D& operator =(const ScopedCustomShader2D&) = delete;

	private:

		Optional<Optional<VertexShader>> m_oldVS;

		Optional<Optional<PixelShader>> m_oldPS;

		void clear() noexcept;
	};
}
