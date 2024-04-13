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
# include <memory>
# include "Common.hpp"
# include "StringView.hpp"
# include "IReader.hpp"
# include "PointVector.hpp"
# include "ColorHSV.hpp"
# include "Image.hpp"
# include "Optional.hpp"
# include "PredefinedYesNo.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	SVG
	//
	////////////////////////////////////////////////////////////////

	/// @brief SVG データ
	class SVG
	{
	public:

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		SVG();

		[[nodiscard]]
		explicit SVG(FilePathView path);

		template <class Reader>
			requires (std::is_base_of_v<IReader, Reader> && (not std::is_lvalue_reference_v<Reader>))
		[[nodiscard]]
		explicit SVG(Reader&& reader);

		[[nodiscard]] 
		explicit SVG(std::unique_ptr<IReader>&& reader);

		////////////////////////////////////////////////////////////////
		//
		//	load
		//
		////////////////////////////////////////////////////////////////

		bool load(FilePathView path);

		template <class Reader>
			requires (std::is_base_of_v<IReader, Reader> && (not std::is_lvalue_reference_v<Reader>))
		bool load(Reader&& reader);

		bool load(std::unique_ptr<IReader>&& reader);

		////////////////////////////////////////////////////////////////
		//
		//	parse
		//
		////////////////////////////////////////////////////////////////

		bool parse(const std::string& source);

		bool parse(StringView source);

		////////////////////////////////////////////////////////////////
		//
		//	isEmpty
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		bool isEmpty() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator bool()
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		explicit operator bool() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	width, height
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		int32 width() const;

		[[nodiscard]]
		int32 height() const;

		////////////////////////////////////////////////////////////////
		//
		//	size
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Size size() const;

		////////////////////////////////////////////////////////////////
		//
		//	widthF, heightF
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		double widthF() const;

		[[nodiscard]]
		double heightF() const;

		////////////////////////////////////////////////////////////////
		//
		//	sizeF
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		SizeF sizeF() const;

		////////////////////////////////////////////////////////////////
		//
		//	clear
		//
		////////////////////////////////////////////////////////////////

		/// @brief SVG データを消去します。
		void clear();

		////////////////////////////////////////////////////////////////
		//
		//	render
		//
		////////////////////////////////////////////////////////////////

		/// @brief SVG を Image にレンダリングします。
		/// @param background 無色部分の背景色
		/// @param premultiplyAlpha 乗算済みアルファを有効にするか
		/// @return レンダリング結果
		[[nodiscard]]
		Image render(const Color& background, PremultiplyAlpha premultiplyAlpha = PremultiplyAlpha::Yes) const;

		[[nodiscard]]
		Image render(const Optional<int32>& maxWidth, const Optional<int32>& maxHeight, const Color& background = Color{ 0, 0 }, PremultiplyAlpha premultiplyAlpha = PremultiplyAlpha::Yes) const;

		[[nodiscard]]
		Image render(const Optional<Size>& maxSize = unspecified, const Color& background = Color{ 0, 0 }, PremultiplyAlpha premultiplyAlpha = PremultiplyAlpha::Yes) const;

		////////////////////////////////////////////////////////////////
		//
		//	Load
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		static SVG Load(FilePathView path);

		template <class Reader>
			requires (std::is_base_of_v<IReader, Reader> && (not std::is_lvalue_reference_v<Reader>))
		[[nodiscard]]
		static SVG Load(Reader&& reader);

		[[nodiscard]]
		static SVG Load(std::unique_ptr<IReader>&& reader);

		////////////////////////////////////////////////////////////////
		//
		//	Parse
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		static SVG Parse(const std::string& source);

		[[nodiscard]]
		static SVG Parse(StringView source);

	private:

		class SVGDetail;

		std::shared_ptr<SVGDetail> pImpl;
	};

}

# include "detail/SVG.ipp"
