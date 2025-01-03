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

# pragma once
# include "Types.hpp"

namespace s3d
{
	struct FormatData;

	////////////////////////////////////////////////////////////////
	//
	//	BlendFactor
	//
	////////////////////////////////////////////////////////////////

	/// @brief ブレンド係数 | Blend factor
	enum class BlendFactor : uint8
	{
		/// @brief ブレンド係数 (0, 0, 0, 0) 
		Zero						= 1,
		
		/// @brief ブレンド係数 (1, 1, 1, 1)
		One							= 2,
		
		/// @brief ブレンド係数 (Rs, Gs, Bs, As)
		/// @remark Rs, Gs, Bs, As はピクセルシェーダーからの RGBA 成分
		SourceColor					= 3,
		
		/// @brief ブレンド係数 (1 - Rs, 1 - Gs, 1 - Bs, 1 - As)
		/// @remark Rs, Gs, Bs, As はピクセルシェーダーからの RGBA 成分
		OneMinusSourceColor			= 4,
		
		/// @brief ブレンド係数 (As, As, As, As)
		/// @remark As はピクセルシェーダーからの A 成分
		SourceAlpha					= 5,
		
		/// @brief ブレンド係数 (1 - As, 1 - As, 1 - As, 1 - As)
		/// @remark As はピクセルシェーダーからの A 成分
		OneMinusSourceAlpha			= 6,

		/// @brief ブレンド係数 (Ad, Ad, Ad, Ad)
		/// @remark Ad はレンダーターゲットの A 成分
		DestinationAlpha			= 7,
		
		/// @brief ブレンド係数 (1 - Ad, 1 - Ad, 1 - Ad, 1 - Ad)
		/// @remark Ad はレンダーターゲットの A 成分
		OneMinusDestinationAlpha	= 8,
		
		/// @brief ブレンド係数 (Rd, Gd, Bd, Ad)
		/// @remark Rd, Gd, Bd, Ad はレンダーターゲットの RGBA 成分
		DestinationColor			= 9,
		
		/// @brief ブレンド係数 (1 - Rd, 1 - Gd, 1 - Bd, 1 - Ad)
		/// @remark Rd, Gd, Bd, Ad はレンダーターゲットの RGBA 成分
		OneMinusDestinationColor	= 10,
		
		/// @brief ブレンド係数 (min(As, 1 - Ad), min(As, 1 - Ad), min(As, 1 - Ad), 1)
		/// @remark As はピクセルシェーダーからの A 成分, Ad はレンダーターゲットの A 成分
		SourceAlphaSaturated		= 11,
		
		/// @brief ブレンド係数 (Rc, Gc, Bc, Ac)
		/// @remark Rc, Gc, Bc, Ac は `Graphics2D::SetBlendColor()` で指定された色
		BlendColor					= 14,
		
		/// @brief ブレンド係数 (1 - Rc, 1 - Gc, 1 - Bc, 1 - Ac)
		/// @remark Rc, Gc, Bc, Ac は `Graphics2D::SetBlendColor()` で指定された色
		OneMinusBlendColor			= 15,
		
		/// @brief ブレンド係数 (Rs1, Gs1, Bs1, As1)
		/// @remark Rs1, Gs1, Bs1, As1 はピクセルシェーダからの 2 番目の RGBA 成分
		Source1Color				= 16,
		
		/// @brief ブレンド係数 (1 - Rs1, 1 - Gs1, 1 - Bs1, 1 - As1)
		/// @remark Rs1, Gs1, Bs1, As1 はピクセルシェーダからの 2 番目の RGBA 成分
		OneMinusSource1Color		= 17,
		
		/// @brief ブレンド係数 (As1, As1, As1, As1)
		/// @remark As1 はピクセルシェーダからの 2 番目の A 成分
		Source1Alpha				= 18,
		
		/// @brief ブレンド係数 (1 - As1, 1 - As1, 1 - As1, 1 - As1)
		/// @remark As1 はピクセルシェーダからの 2 番目の A 成分
		OneMinusSource1Alpha		= 19,
	};

	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////
	
	/// @brief ブレンド係数を文字列に変換します。
	/// @param formatData 文字列バッファ
	/// @param value ブレンド係数
	/// @remark この関数は Format 用の関数です。通常、ユーザーが直接呼び出す必要はありません。
	void Formatter(FormatData& formatData, BlendFactor value);
}

