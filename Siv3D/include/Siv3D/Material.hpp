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
# include "ColorF.hpp"
# include "MaterialAlphaMode.hpp"
# include "MaterialTexture.hpp"
# include "Optional.hpp"
# include "String.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	Material
	//
	////////////////////////////////////////////////////////////////

	/// @brief PBR の metallic-roughness モデルに基づく 3D マテリアル
	struct Material
	{
		/// @brief マテリアル名
		String name = U"Material";

		/// @brief ベースカラーとアルファ値
		ColorF baseColor{ 1.0 };

		/// @brief 金属度。通常は 0.0 以上 1.0 以下
		double metallic = 0.0;

		/// @brief 粗さ。通常は 0.0 以上 1.0 以下
		double roughness = 1.0;

		/// @brief エミッシブカラー。アルファ成分は使用しません。
		ColorF emissive{ 0.0 };

		/// @brief アルファ値の扱い
		MaterialAlphaMode alphaMode = MaterialAlphaMode::Opaque;

		/// @brief `alphaMode` が `MaterialAlphaMode::Mask` の場合に使用するアルファ値のしきい値
		double alphaCutoff = 0.5;

		/// @brief 両面を描画するか
		bool doubleSided = false;

		/// @brief ベースカラーテクスチャ
		Optional<MaterialTexture> baseColorTexture;

		/// @brief metallic-roughness テクスチャ
		/// @remark G チャンネルに roughness、B チャンネルに metallic を格納します。
		Optional<MaterialTexture> metallicRoughnessTexture;

		/// @brief 法線テクスチャ
		Optional<MaterialTexture> normalTexture;

		/// @brief 法線テクスチャの X, Y 成分に適用するスケール
		double normalScale = 1.0;

		/// @brief アンビエントオクルージョンテクスチャ
		/// @remark R チャンネルを使用します。
		Optional<MaterialTexture> occlusionTexture;

		/// @brief アンビエントオクルージョンの強さ。通常は 0.0 以上 1.0 以下
		double occlusionStrength = 1.0;

		/// @brief エミッシブテクスチャ
		Optional<MaterialTexture> emissiveTexture;
	};
}
