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
# include "Common.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	ProfilerStat
	//
	////////////////////////////////////////////////////////////////

	/// @brief プロファイラの統計情報 | Profiler statistics
	struct ProfilerStat
	{
		/// @brief Draw コール回数 | Number of draw calls
		int32 drawCalls = 0;

		/// @brief 描画された三角形の数 | Number of drawn triangles
		int32 triangleCount = 0;

		/// @brief テクスチャの数 | Number of textures
		int32 textureCount = 0;

		/// @brief フォントの数 | Number of fonts
		int32 fontCount = 0;

		/// @brief メッシュの数 | Number of meshes
		int32 meshCount = 0;

		/// @brief オーディオの数 | Number of audio objects
		int32 audioCount = 0;

		/// @brief アクティブな音声の数 | Number of active audio objects
		int32 activeVoice = 0;

		void print() const;
	};
}
