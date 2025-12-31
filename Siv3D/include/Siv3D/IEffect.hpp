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
# include "Common.hpp"

namespace s3d
{
	/// @brief エフェクトインタフェース
	struct IEffect
	{
		////////////////////////////////////////////////////////////////
		//
		//	(destructor)
		//
		////////////////////////////////////////////////////////////////

		/// @brief デストラクタ
		virtual ~IEffect() = default;

		////////////////////////////////////////////////////////////////
		//
		//	update
		//
		////////////////////////////////////////////////////////////////

		/// @brief エフェクトの更新と描画を記述します。
		/// @param timeSec エフェクトの発生からの時間
		/// @return エフェクトを継続する場合は true を、破棄する場合は false を返します。
		virtual bool update(double timeSec) = 0;

		////////////////////////////////////////////////////////////////
		//
		//	getOrder
		//
		////////////////////////////////////////////////////////////////

		/// @brief エフェクトの描画順序を取得します。
		/// @return 描画順序（ソートが有効化されている Effect で実行した場合、小さいほど先に実行されます）
		virtual double getOrder() const noexcept;
	};
}

# include "detail/IEffect.ipp"
