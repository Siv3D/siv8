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
# include "Types.hpp"

namespace s3d
{
	/// @brief エンジンの設定
	struct EngineOption
	{
		/// @brief レンダラー
		enum class Renderer : uint8
		{
			/// @brief Direct3D 11
			/// @remark 対応: Windows
			Direct3D11 = 1,

			/// @brief Metal
			/// @remark 対応: macOS
			Metal = 2,
		};

		/// @brief Direct3D 11 ドライバーの種類 (Windows 版)
		enum class D3D11Driver : uint8
		{
			/// @brief ハードウェア
			Hardware,

			/// @brief ハードウェア (iGPU 優先)
			Hardware_FavorIntegrated,

			/// @brief WARP ドライバーを使用
			/// @remark やや実行が遅いですが、実行環境による影響を受けないドライバーです。
			WARP,

			/// @brief リファレンスドライバーを使用
			Reference
		};
	};

	struct EngineOptions
	{
		//EngineOption::Renderer renderer			= EngineOption::Renderer::PlatformDefault;

		EngineOption::D3D11Driver d3d11Driver	= EngineOption::D3D11Driver::Hardware;
	};

	namespace Internal
	{
		//int SetEngineOption(EngineOption::Renderer) noexcept;
		int SetEngineOption(EngineOption::D3D11Driver) noexcept;
	}

	extern EngineOptions g_engineOptions;
}

# define SIV3D_COMBINE_(X,Y) X##Y
# define SIV3D_COMBINE(X,Y) SIV3D_COMBINE_(X,Y)
# define SIV3D_SET(value) const int SIV3D_COMBINE(siv3d_engine_otpion_,__LINE__) = s3d::Internal::SetEngineOption(value);
