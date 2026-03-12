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
# include "AssetHandle.hpp"
# include "StringView.hpp"
# include "IReader.hpp"

namespace s3d
{
	class Blob;

	////////////////////////////////////////////////////////////////
	//
	//	PixelShader
	//
	////////////////////////////////////////////////////////////////

	/// @brief ピクセルシェーダ
	class PixelShader : public AssetHandle<PixelShader>
	{
	public:

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		PixelShader();

		////////////////////////////////////////////////////////////////
		//
		//	(destructor)
		//
		////////////////////////////////////////////////////////////////

		virtual ~PixelShader();

		////////////////////////////////////////////////////////////////
		//
		//	getBytecode
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		const Blob& getBytecode() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	swap
		//
		////////////////////////////////////////////////////////////////

		void swap(PixelShader& other) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	swap
		//
		////////////////////////////////////////////////////////////////

		friend void swap(PixelShader& lhs, PixelShader& rhs) noexcept
		{
			lhs.swap(rhs);
		}

		////////////////////////////////////////////////////////////////
		//
		//	HLSL
		//
		////////////////////////////////////////////////////////////////

		/// @brief HLSL ファイルからピクセルシェーダを作成します。
		/// @param path HLSL ファイルのパス
		/// @param entryPoint エントリーポイント
		/// @return ピクセルシェーダ
		[[nodiscard]]
		static PixelShader HLSL(FilePathView path, StringView entryPoint);

		[[nodiscard]]
		static PixelShader HLSL(std::unique_ptr<IReader> reader, StringView entryPoint);

		template <ReaderObject Reader>
		[[nodiscard]]
		static PixelShader HLSL(Reader&& reader, StringView entryPoint);

		/// @brief HLSL バイトコードからピクセルシェーダを作成します。
		/// @param bytecode HLSL バイトコード
		/// @return ピクセルシェーダ
		[[nodiscard]]
		static PixelShader HLSL(const Blob& bytecode);

		/// @brief HLSL ソースコードからピクセルシェーダを作成します。
		/// @param source HLSL ソースコード
		/// @param entryPoint エントリーポイント
		/// @return ピクセルシェーダ
		[[nodiscard]]
		static PixelShader HLSL(const std::string& source, StringView entryPoint);

		////////////////////////////////////////////////////////////////
		//
		//	MSL
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		static PixelShader MSL(StringView entryPoint);

		[[nodiscard]]
		static PixelShader MSL(FilePathView path, StringView entryPoint);

		[[nodiscard]]
		static PixelShader MSL(std::unique_ptr<IReader> reader, StringView entryPoint);

		template <ReaderObject Reader>
		[[nodiscard]]
		static PixelShader MSL(Reader&& reader, StringView entryPoint);

		/// @brief MSL ソースコードからピクセルシェーダを作成します。
		/// @param source MSL ソースコード
		/// @param entryPoint エントリーポイント
		/// @return ピクセルシェーダ
		[[nodiscard]]
		static PixelShader MSL(const std::string& source, StringView entryPoint);

	private:

		[[nodiscard]]
		PixelShader(FilePathView path, StringView entryPoint);

		[[nodiscard]]
		PixelShader(std::unique_ptr<IReader> reader, StringView entryPoint);

		[[nodiscard]]
		explicit PixelShader(const Blob& bytecode);

		[[nodiscard]]
		PixelShader(const std::string& source, StringView entryPoint);
	};
}

# include "detail/PixelShader.ipp"
