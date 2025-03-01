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
# include "String.hpp"
# include "Result.hpp"
# include "ShaderStage.hpp"

namespace s3d
{
	class VertexShader;
	class PixelShader;

	////////////////////////////////////////////////////////////////
	//
	//	MSL
	//
	////////////////////////////////////////////////////////////////

	/// @brief MSL (Metal Shading Language) ファイル
	struct MSL
	{
		/// @brief MSL ファイルのパス
		FilePath path;

		/// @brief エントリーポイント
		String entryPoint;

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		/// @brief エントリーポイントで MSL を指定します。
		/// @param _entryPoint エントリーポイント
		[[nodiscard]]
		explicit MSL(String _entryPoint);

		////////////////////////////////////////////////////////////////
		//
		//	operator VertexShader
		//
		////////////////////////////////////////////////////////////////

		/// @brief MSL から頂点シェーダを作成します。
		[[nodiscard]]
		operator VertexShader() const;

		////////////////////////////////////////////////////////////////
		//
		//	operator PixelShader
		//
		////////////////////////////////////////////////////////////////

		/// @brief MSL からピクセルシェーダを作成します。
		[[nodiscard]]
		operator PixelShader() const;
	};
}
