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

	/// @brief 頂点シェーダ
	class VertexShader : public AssetHandle<VertexShader>
	{
	public:

		[[nodiscard]]
		VertexShader();

		virtual ~VertexShader();

		[[nodiscard]]
		const Blob& getBinary() const noexcept;

		void swap(VertexShader& other) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	swap
		//
		////////////////////////////////////////////////////////////////

		friend void swap(VertexShader& lhs, VertexShader& rhs) noexcept
		{
			lhs.swap(rhs);
		}

		[[nodiscard]]
		static VertexShader HLSL(FilePathView path, StringView entryPoint = U"VS");

		[[nodiscard]]
		static VertexShader MSL(StringView entryPoint, FilePathView path = {});

	private:

		[[nodiscard]]
		VertexShader(FilePathView path, StringView entryPoint);
	};
}
