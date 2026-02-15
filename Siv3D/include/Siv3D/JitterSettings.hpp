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

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	JitterSettings
	//
	////////////////////////////////////////////////////////////////

	/// @brief ジッター（手描き風揺れ）の設定 | Jitter settings
	struct JitterSettings
	{
		/// @brief ノイズ用シード | Seed for noise generation
		uint64 seed = 0;

		/// @brief 揺らぎの強さ | Jitter intensity
		double roughness = 1.0;

		/// @brief 分割の細かさ（ピクセル） | Segment length in pixels
		double segmentLength = 8.0;

		/// @brief 弓なり量 | Bowing amount
		/// @remark 線全体に曲げオフセットを与えます。 | Adds a bending offset along the entire stroke.
		double bowing = 0.0;

		/// @brief ノイズ周波数（入力スケール） | Noise frequency (input scale)
		double noiseFrequency = 0.05;
		
		/// @brief 法線方向ジッターのスケール（ピクセル） | Normal jitter scale (pixels)
		double normalScale = 10.0;
		
		/// @brief 接線方向ジッターのスケール（ピクセル） | Tangent jitter scale (pixels)
		double tangentScale = 3.0;

		/// @brief FBM のオクターブ数 | Number of FBM octaves
		int32 fbmOctaves = 3;

		/// @brief FBM の高周波成分の減衰率 | FBM persistence (amplitude decay per octave)
		double fbmPersistence = 0.5;
	};
}
