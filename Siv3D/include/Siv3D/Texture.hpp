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
# include "Image.hpp"
# include "TextureDesc.hpp"
# include "TextureFormat.hpp"
# include "AssetHandle.hpp"
# include "2DShapesFwd.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	Texture
	//
	////////////////////////////////////////////////////////////////

	/// @brief テクスチャ
	class Texture : public AssetHandle<Texture>
	{
	public:

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Texture();

		[[nodiscard]]
		explicit Texture(const Image& image, TextureDesc desc = TextureDesc::Default2D);

		[[nodiscard]]
		Texture(const Image& image, const Array<Image>& mipmaps, TextureDesc desc = TextureDesc::Default2D);

		[[nodiscard]]
		explicit Texture(FilePathView path, TextureDesc desc = TextureDesc::Default2D);

		[[nodiscard]]
		explicit Texture(IReader&& reader, TextureDesc desc = TextureDesc::Default2D);

		////////////////////////////////////////////////////////////////
		//
		//	(desctructor)
		//
		////////////////////////////////////////////////////////////////

		virtual ~Texture();
		



		////////////////////////////////////////////////////////////////
		//
		//	swap
		//
		////////////////////////////////////////////////////////////////

		/// @brief 別の Texture と内容を交換します。
		/// @param other 別の Texture
		void swap(Texture& other) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	swap
		//
		////////////////////////////////////////////////////////////////

		/// @brief 2 つの Texture を入れ替えます。
		/// @param lhs 一方の Texture
		/// @param rhs もう一方の Texture
		friend void swap(Texture& lhs, Texture& rhs) noexcept
		{
			lhs.swap(rhs);
		}

	};
}
