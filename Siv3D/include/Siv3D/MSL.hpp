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
	class ShaderGroup;
	class HLSL;

	////////////////////////////////////////////////////////////////
	//
	//	MSL
	//
	////////////////////////////////////////////////////////////////

	/// @brief MSL (Metal Shading Language) ファイル
	class MSL
	{
	public:

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		/// @brief デフォルトコンストラクタ
		[[nodiscard]]
		MSL() = default;

		/// @brief エントリーポイントで MSL を指定します。
		/// @param entryPoint エントリーポイント
		[[nodiscard]]
		explicit MSL(String entryPoint);

		/// @brief ファイルパスとエントリーポイントで MSL を指定します。
		/// @param path MSL ファイルのパス
		/// @param entryPoint エントリーポイント
		[[nodiscard]]
		MSL(FilePath path, String entryPoint);

		////////////////////////////////////////////////////////////////
		//
		//	operator |
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		ShaderGroup operator |(const HLSL& hlsl) const;

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

		////////////////////////////////////////////////////////////////
		//
		//	FromSource
		//
		////////////////////////////////////////////////////////////////

		static MSL FromSource(std::string source);

		static MSL FromSource(std::string source, StringView entryPoint);

		static MSL FromSource(StringView source);

		static MSL FromSource(StringView source, StringView entryPoint);

	private:

		/// @brief MSL ファイルのパス
		FilePath m_path;

		/// @brief エントリーポイント
		String m_entryPoint;

		/// @brief MSL ソースコード
		std::string m_source;
	};
}
