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
# include "ConstantBuffer.hpp"
# include "ShaderStage.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	ScopedConstantBuffer2D
	//
	////////////////////////////////////////////////////////////////

	/// @brief 2D 描画の独自定数バッファを一時的に設定するスコープオブジェクト
	/// @tparam Stage 頂点またはピクセルシェーダのステージ
	/// @remark 構築時の値をコピーして後続の描画に適用します。その後の元の定数バッファの変更・破棄は設定に影響しません。
	/// @remark 破棄時に指定したステージ・スロットの以前の値を復元します。以前の設定がなければ解除します。
	/// @remark 同じステージ・スロットのスコープは入れ子にし、構築と逆の順序で破棄してください。Flush() とフレーム境界を越えて有効です。
	/// @remark シェーダが参照するすべての独自スロットを描画前に設定してください。ScopedCustomShader2D は定数バッファ設定を変更・復元しません。
	template <ShaderStage Stage>
	class ScopedConstantBuffer2D
	{
	public:

		static_assert((Stage == ShaderStage::Vertex) || (Stage == ShaderStage::Pixel));

		/// @brief 設定を変更しないスコープオブジェクトを作成します。
		[[nodiscard]]
		ScopedConstantBuffer2D() = default;

		/// @brief 指定スロットの設定を保存し、定数バッファを設定します。
		/// @tparam Type 定数データの型
		/// @param slot スロット番号。2～13。HLSL の register(bN)、MSL の buffer(N) に対応します。0 と 1 はエンジン用に予約されています。
		/// @param buffer 設定時に値をコピーする定数バッファ
		/// @throw Error slot が範囲外の場合
		template <class Type>
		[[nodiscard]]
		ScopedConstantBuffer2D(uint32 slot, const ConstantBuffer<Type>& buffer);

		/// @brief 設定の復元を引き継ぎます。移動元は復元を行いません。
		/// @param other 移動元
		[[nodiscard]]
		ScopedConstantBuffer2D(ScopedConstantBuffer2D&& other) noexcept;

		/// @brief 指定スロットの以前の設定を復元します。
		/// @remark 記録済みの描画には影響しません。
		~ScopedConstantBuffer2D();

		ScopedConstantBuffer2D(const ScopedConstantBuffer2D&) = delete;

		ScopedConstantBuffer2D& operator =(const ScopedConstantBuffer2D&) = delete;

	private:

		uint32 m_slot = 0;

		uint32 m_previous = 0;

		bool m_active = false;

		ScopedConstantBuffer2D(uint32 slot, const void* data, size_t size);
	};

	/// @brief 頂点シェーダの独自定数バッファ設定スコープ
	using ScopedVSConstantBuffer2D = ScopedConstantBuffer2D<ShaderStage::Vertex>;

	/// @brief ピクセルシェーダの独自定数バッファ設定スコープ
	using ScopedPSConstantBuffer2D = ScopedConstantBuffer2D<ShaderStage::Pixel>;
}

# include "detail/ScopedConstantBuffer2D.ipp"
