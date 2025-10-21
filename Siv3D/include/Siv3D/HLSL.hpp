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
# include "Blob.hpp"
# include <ThirdParty/EnumBitmask/EnumBitmask.hpp>

namespace s3d
{
	class VertexShader;
	class PixelShader;
	class ShaderGroup;
	class MSL;

	////////////////////////////////////////////////////////////////
	//
	//	HLSL
	//
	////////////////////////////////////////////////////////////////

	/// @brief HLSL シェーダ
	class HLSL
	{
	public:

		////////////////////////////////////////////////////////////////
		//
		//	CompileOption
		//
		////////////////////////////////////////////////////////////////

		/// @brief HLSL のコンパイルオプション
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

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		/// @brief デフォルトコンストラクタ
		[[nodiscard]]
		HLSL() = default;

		/// @brief ファイルパスで HLSL を指定します。
		/// @param path HLSL ファイルのパス
		[[nodiscard]]
		explicit HLSL(FilePath path);

		/// @brief ファイルパスとエントリーポイントで HLSL を指定します。
		/// @param path HLSL ファイルのパス
		/// @param entryPoint エントリーポイント
		[[nodiscard]]
		HLSL(FilePath path, String entryPoint);

		////////////////////////////////////////////////////////////////
		//
		//	operator |
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		ShaderGroup operator |(const MSL& msl) const;

		////////////////////////////////////////////////////////////////
		//
		//	operator VertexShader
		//
		////////////////////////////////////////////////////////////////

		/// @brief HLSL から頂点シェーダを作成します。
		[[nodiscard]]
		operator VertexShader() const;

		////////////////////////////////////////////////////////////////
		//
		//	operator PixelShader
		//
		////////////////////////////////////////////////////////////////

		/// @brief HLSL からピクセルシェーダを作成します。
		[[nodiscard]]
		operator PixelShader() const;

		////////////////////////////////////////////////////////////////
		//
		//	FromSource
		//
		////////////////////////////////////////////////////////////////

		static HLSL FromSource(std::string source);

		static HLSL FromSource(std::string source, StringView entryPoint);

		static HLSL FromSource(StringView source);

		static HLSL FromSource(StringView source, StringView entryPoint);

		////////////////////////////////////////////////////////////////
		//
		//	FromBytecode
		//
		////////////////////////////////////////////////////////////////

		static HLSL FromBytecode(Blob bytecode);

	private:

		/// @brief HLSL ファイルのパス
		FilePath m_path;

		/// @brief エントリーポイント
		String m_entryPoint;

		/// @brief HLSL ソースコード
		std::string m_source;

		/// @brief バイトコード
		Blob m_bytecode;
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

		/// @brief HLSL コードをコンパイルした結果をファイルに保存します。
		/// @param source HLSL コード
		/// @param outputPath 保存先のファイルパス
		/// @param shaderStage シェーダの種類
		/// @param entryPoint エントリーポイント
		/// @param option コンパイルオプション
		/// @return 保存に成功した場合 true, それ以外の場合は false
		bool CompileHLSLFromSource(std::string_view source, FilePathView outputPath, ShaderStage shaderStage, StringView entryPoint, HLSL::CompileOption option = HLSL::CompileOption::Default);

		/// @brief HLSL ファイルからシェーダをコンパイルした結果をファイルに保存します。
		/// @param source HLSL コード
		/// @param outputPath 保存先のファイルパス
		/// @param shaderStage シェーダの種類
		/// @param entryPoint エントリーポイント
		/// @param option コンパイルオプション
		/// @return 保存に成功した場合 true, それ以外の場合は false
		bool CompileHLSLFromSource(StringView source, FilePathView outputPath, ShaderStage shaderStage, StringView entryPoint, HLSL::CompileOption option = HLSL::CompileOption::Default);

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
		bool CompileHLSLFromFile(FilePathView hlslFilePath, FilePathView outputPath, ShaderStage shaderStage, StringView entryPoint, HLSL::CompileOption option = HLSL::CompileOption::Default);
	}
}
