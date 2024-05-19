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
# include "AssetHandle.hpp"
# include "StringView.hpp"

namespace s3d
{
	class Blob;

	/// @brief ピクセルシェーダ
	class PixelShader : public AssetHandle<PixelShader>
	{
	public:

		[[nodiscard]]
		PixelShader();

		virtual ~PixelShader();

		[[nodiscard]]
		const Blob& getBytecode() const noexcept;

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

		[[nodiscard]]
		static PixelShader HLSL(FilePathView path, StringView entryPoint = U"PS");

		[[nodiscard]]
		static PixelShader HLSL(const Blob& bytecode);

		[[nodiscard]]
		static PixelShader MSL(StringView entryPoint, FilePathView path = {});

	private:

		[[nodiscard]]
		PixelShader(FilePathView path, StringView entryPoint);

		[[nodiscard]]
		explicit PixelShader(const Blob& bytecode);
	};
}
