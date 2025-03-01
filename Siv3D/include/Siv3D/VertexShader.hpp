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
# include "AssetHandle.hpp"
# include "StringView.hpp"

namespace s3d
{
	class Blob;

	////////////////////////////////////////////////////////////////
	//
	//	VertexShader
	//
	////////////////////////////////////////////////////////////////

	/// @brief 頂点シェーダ
	class VertexShader : public AssetHandle<VertexShader>
	{
	public:

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		/// @brief デフォルトコンストラクタ
		[[nodiscard]]
		VertexShader();

		////////////////////////////////////////////////////////////////
		//
		//	(destructor)
		//
		////////////////////////////////////////////////////////////////

		/// @brief デストラクタ
		virtual ~VertexShader();

		////////////////////////////////////////////////////////////////
		//
		//	getBytecode
		//
		////////////////////////////////////////////////////////////////

		/// @brief 頂点シェーダのバイトコードを返します。
		/// @return 頂点シェーダのバイトコード
		[[nodiscard]]
		const Blob& getBytecode() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	swap
		//
		////////////////////////////////////////////////////////////////

		/// @brief 別の VertexShader と内容を交換します。
		/// @param other 別の VertexShader
		void swap(VertexShader& other) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	swap
		//
		////////////////////////////////////////////////////////////////

		/// @brief 2 つの VertexShader を入れ替えます。
		/// @param lhs 一方の VertexShader
		/// @param rhs もう一方の VertexShader
		friend void swap(VertexShader& lhs, VertexShader& rhs) noexcept
		{
			lhs.swap(rhs);
		}

		////////////////////////////////////////////////////////////////
		//
		//	HLSL
		//
		////////////////////////////////////////////////////////////////

		/// @brief HLSL ファイルから頂点シェーダを作成します。
		/// @param path HLSL ファイルのパス
		/// @param entryPoint エントリーポイント
		/// @return 頂点シェーダ
		[[nodiscard]]
		static VertexShader HLSL(FilePathView path, StringView entryPoint = U"VS");

		/// @brief HLSL バイトコードから頂点シェーダを作成します。
		/// @param bytecode HLSL バイトコード
		/// @return 頂点シェーダ
		[[nodiscard]]
		static VertexShader HLSL(const Blob& bytecode);

		////////////////////////////////////////////////////////////////
		//
		//	MSL
		//
		////////////////////////////////////////////////////////////////

		/// @brief MSL ファイルから頂点シェーダを作成します。
		/// @param entryPoint エントリーポイント
		/// @return 頂点シェーダ
		[[nodiscard]]
		static VertexShader MSL(StringView entryPoint);

		/// @brief MSL ファイルから頂点シェーダを作成します。
		/// @param path MSL ファイルのパス
		/// @param entryPoint エントリーポイント
		/// @return 頂点シェーダ
		[[nodiscard]]
		static VertexShader MSL(FilePathView path, StringView entryPoint);

	private:

		[[nodiscard]]
		VertexShader(FilePathView path, StringView entryPoint);

		[[nodiscard]]
		explicit VertexShader(const Blob& bytecode);
	};
}
