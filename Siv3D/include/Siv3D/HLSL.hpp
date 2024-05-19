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
# include "String.hpp"
# include "Result.hpp"
# include "ShaderStage.hpp"
# include "Blob.hpp"
# include <ThirdParty/EnumBitmask/EnumBitmask.hpp>

namespace s3d
{
	class VertexShader;
	class PixelShader;

	////////////////////////////////////////////////////////////////
	//
	//	HLSL
	//
	////////////////////////////////////////////////////////////////

	/// @brief HLSL ファイル
	struct HLSL
	{
		////////////////////////////////////////////////////////////////
		//
		//	CompileOption
		//
		////////////////////////////////////////////////////////////////

		enum class CompileOption
		{
			Debug					= (1 << 0),
			SkipValidation			= (1 << 1),
			SkipOptimization		= (1 << 2),
			PackMatrixRowMajor		= (1 << 3),
			PackMatrixColumnMajor	= (1 << 4),
			PartialPrecision		= (1 << 5),
			AvoidFlowControl		= (1 << 9),
			PreferFlowControl		= (1 << 10),
			EnableStrictness		= (1 << 11),
			EnableBackwardsCompat	= (1 << 12),
			EnableIEEEStrictness	= (1 << 13),
			OptimizationLevel0		= (1 << 14),
			OptimizationLevel1		= 0,
			OptimizationLevel2		= ((1 << 14) | (1 << 15)),
			OptimizationLevel3		= (1 << 15),
			WarningsAreErrors		= (1 << 18),
			Default					= (OptimizationLevel3 | WarningsAreErrors),
		};

		FilePath path;

		String entryPoint;

		Blob bytecode;

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		explicit HLSL(FilePath _path);

		[[nodiscard]]
		HLSL(FilePath _path, String _entryPoint);

		[[nodiscard]]
		explicit HLSL(const Blob& bytecode);

		//[[nodiscard]]
		//ShaderGroup operator |(const GLSL& glsl) const;

		//[[nodiscard]]
		//ShaderGroup operator |(const MSL& msl) const;

		////////////////////////////////////////////////////////////////
		//
		//	operator VertexShader
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		operator VertexShader() const;

		////////////////////////////////////////////////////////////////
		//
		//	operator PixelShader
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		operator PixelShader() const;
	};

	DEFINE_BITMASK_OPERATORS(HLSL::CompileOption);

	namespace Platform::Windows
	{
		////////////////////////////////////////////////////////////////
		//
		//	CompileHLSLFromSource
		//
		////////////////////////////////////////////////////////////////

		/// @brief HLSL コードをコンパイルします。
		/// @param source HLSL コード
		/// @param shaderStage シェーダの種類
		/// @param entryPoint エントリーポイント
		/// @param option コンパイルオプション
		/// @return コンパイルされたシェーダのバイトコード、失敗した場合はエラーメッセージ
		[[nodiscard]]
		Result<Blob, String> CompileHLSLFromSource(std::string_view source, ShaderStage shaderStage, StringView entryPoint, HLSL::CompileOption option = HLSL::CompileOption::Default);

		/// @brief HLSL コードをコンパイルします。
		/// @param source HLSL コード
		/// @param shaderStage シェーダの種類
		/// @param entryPoint エントリーポイント
		/// @param option コンパイルオプション
		/// @return コンパイルされたシェーダのバイトコード、失敗した場合はエラーメッセージ
		[[nodiscard]]
		Result<Blob, String> CompileHLSLFromSource(StringView source, ShaderStage shaderStage, StringView entryPoint, HLSL::CompileOption option = HLSL::CompileOption::Default);

		/// @brief HLSL ファイルからシェーダをコンパイルした結果をファイルに保存します。
		/// @param hlslFilePath HLSL ファイルのパス
		/// @param outputPath 保存先のファイルパス
		/// @param shaderStage シェーダの種類
		/// @param entryPoint エントリーポイント
		/// @param option コンパイルオプション
		/// @return 保存に成功した場合 true, それ以外の場合は false
		[[nodiscard]]
		bool CompileHLSLFromSource(std::string_view source, FilePathView outputPath, ShaderStage shaderStage, StringView entryPoint, HLSL::CompileOption option = HLSL::CompileOption::Default);

		/// @brief HLSL ファイルからシェーダをコンパイルした結果をファイルに保存します。
		/// @param hlslFilePath HLSL ファイルのパス
		/// @param outputPath 保存先のファイルパス
		/// @param shaderStage シェーダの種類
		/// @param entryPoint エントリーポイント
		/// @param option コンパイルオプション
		/// @return 保存に成功した場合 true, それ以外の場合は false
		[[nodiscard]]
		bool CompileHLSLFromSource(FilePathView source, FilePathView outputPath, ShaderStage shaderStage, StringView entryPoint, HLSL::CompileOption option = HLSL::CompileOption::Default);

		////////////////////////////////////////////////////////////////
		//
		//	CompileHLSLFromFile
		//
		////////////////////////////////////////////////////////////////

		/// @brief HLSL ファイルからシェーダをコンパイルします。
		/// @param hlslFilePath HLSL ファイルのパス
		/// @param shaderStage シェーダの種類
		/// @param entryPoint エントリーポイント
		/// @param option コンパイルオプション
		/// @return コンパイルされたシェーダのバイトコード、失敗した場合はエラーメッセージ
		[[nodiscard]]
		Result<Blob, String> CompileHLSLFromFile(FilePathView hlslFilePath, ShaderStage shaderStage, StringView entryPoint, HLSL::CompileOption option = HLSL::CompileOption::Default);

		/// @brief HLSL ファイルからシェーダをコンパイルした結果をファイルに保存します。
		/// @param hlslFilePath HLSL ファイルのパス
		/// @param outputPath 保存先のファイルパス
		/// @param shaderStage シェーダの種類
		/// @param entryPoint エントリーポイント
		/// @param option コンパイルオプション
		/// @return 保存に成功した場合 true, それ以外の場合は false
		[[nodiscard]]
		bool CompileHLSLFromFile(FilePathView hlslFilePath, FilePathView outputPath, ShaderStage shaderStage, StringView entryPoint, HLSL::CompileOption option = HLSL::CompileOption::Default);
	}
}
