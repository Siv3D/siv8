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

		/// @brief デフォルトコンストラクタ
		[[nodiscard]]
		SVG();

		/// @brief SVG をファイルから読み込みます。
		[[nodiscard]]
		explicit SVG(FilePathView path);

		/// @brief SVG を Reader から読み込みます。
		/// @tparam Reader IRader を継承したクラス
		/// @param reader Reader オブジェクト
		template <class Reader>
			requires (std::is_base_of_v<IReader, Reader> && (not std::is_lvalue_reference_v<Reader>))
		[[nodiscard]]
		explicit SVG(Reader&& reader);

		/// @brief SVG を Reader から読み込みます。
		/// @param reader Reader オブジェクト
		[[nodiscard]] 
		explicit SVG(std::unique_ptr<IReader>&& reader);

		////////////////////////////////////////////////////////////////
		//
		//	load
		//
		////////////////////////////////////////////////////////////////

		/// @brief SVG ファイルを読み込みます。
		/// @param path SVG ファイルのパス
		/// @return 読み込みに成功した場合 true, それ以外の場合は false
		bool load(FilePathView path);

		/// @brief SVG データを読み込みます。
		/// @tparam Reader IRader を継承したクラス
		/// @param reader Reader オブジェクト
		/// @return 読み込みに成功した場合 true, それ以外の場合は false
		template <class Reader>
			requires (std::is_base_of_v<IReader, Reader> && (not std::is_lvalue_reference_v<Reader>))
		bool load(Reader&& reader);

		/// @brief SVG データを読み込みます。
		/// @param reader Reader オブジェクト
		/// @return 読み込みに成功した場合 true, それ以外の場合は false
		bool load(std::unique_ptr<IReader>&& reader);

		////////////////////////////////////////////////////////////////
		//
		//	parse
		//
		////////////////////////////////////////////////////////////////

		/// @brief SVG データをパースします。
		/// @param source SVG データ
		/// @return パースに成功した場合 true, それ以外の場合は false
		bool parse(const std::string& source);

		/// @brief SVG データをパースします。
		/// @param source SVG データ
		/// @return パースに成功した場合 true, それ以外の場合は false
		bool parse(StringView source);

		////////////////////////////////////////////////////////////////
		//
		//	isEmpty
		//
		////////////////////////////////////////////////////////////////

		/// @brief SVG データが空であるかを返します。
		/// @return SVG データが空である場合 true, それ以外の場合は false
		[[nodiscard]]
		bool isEmpty() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator bool()
		//
		////////////////////////////////////////////////////////////////

		/// @brief SVG データが空でないかを返します。
		/// @return SVG データが空でない場合 true, それ以外の場合は false
		[[nodiscard]]
		explicit operator bool() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	width, height
		//
		////////////////////////////////////////////////////////////////

		/// @brief SVG の幅（ピクセル）を返します。
		/// @return SVG の幅（ピクセル）
		[[nodiscard]]
		int32 width() const;

		/// @brief SVG の高さ（ピクセル）を返します。
		/// @return SVG の高さ（ピクセル）
		[[nodiscard]]
		int32 height() const;

		////////////////////////////////////////////////////////////////
		//
		//	size
		//
		////////////////////////////////////////////////////////////////

		/// @brief SVG の幅と高さ（ピクセル）を返します。
		/// @return SVG の幅と高さ（ピクセル）
		[[nodiscard]]
		Size size() const;

		////////////////////////////////////////////////////////////////
		//
		//	widthF, heightF
		//
		////////////////////////////////////////////////////////////////

		/// @brief SVG の幅（ピクセル）を返します。
		/// @return SVG の幅（ピクセル）
		[[nodiscard]]
		double widthF() const;

		/// @brief SVG の高さ（ピクセル）を返します。
		/// @return SVG の高さ（ピクセル）
		[[nodiscard]]
		double heightF() const;

		////////////////////////////////////////////////////////////////
		//
		//	sizeF
		//
		////////////////////////////////////////////////////////////////

		/// @brief SVG の幅と高さ（ピクセル）を返します。
		/// @return SVG の幅と高さ（ピクセル）
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

		/// @brief SVG を Image にレンダリングします。
		/// @param maxWidth 幅の最大値（ピクセル）
		/// @param maxHeight 高さの最大値（ピクセル）
		/// @param background 無色部分の背景色
		/// @param premultiplyAlpha 乗算済みアルファを有効にするか
		/// @return レンダリング結果
		[[nodiscard]]
		Image render(const Optional<int32>& maxWidth, const Optional<int32>& maxHeight, const Color& background = Color{ 0, 0 }, PremultiplyAlpha premultiplyAlpha = PremultiplyAlpha::Yes) const;

		/// @brief SVG を Image にレンダリングします。
		/// @param maxSize 幅と高さの最大値（ピクセル）
		/// @param background 無色部分の背景色
		/// @param premultiplyAlpha 乗算済みアルファを有効にするか
		/// @return レンダリング結果
		[[nodiscard]]
		Image render(const Optional<Size>& maxSize = unspecified, const Color& background = Color{ 0, 0 }, PremultiplyAlpha premultiplyAlpha = PremultiplyAlpha::Yes) const;

		////////////////////////////////////////////////////////////////
		//
		//	Load
		//
		////////////////////////////////////////////////////////////////

		/// @brief SVG ファイルを読み込みます。
		/// @param path SVG ファイルのパス
		/// @return SVG オブジェクト
		[[nodiscard]]
		static SVG Load(FilePathView path);

		/// @brief SVG データを読み込みます。
		/// @tparam Reader IRader を継承したクラス
		/// @param reader Reader オブジェクト
		/// @return SVG オブジェクト
		template <class Reader>
			requires (std::is_base_of_v<IReader, Reader> && (not std::is_lvalue_reference_v<Reader>))
		[[nodiscard]]
		static SVG Load(Reader&& reader);

		/// @brief SVG データを読み込みます。
		/// @param reader Reader オブジェクト
		/// @return SVG オブジェクト
		[[nodiscard]]
		static SVG Load(std::unique_ptr<IReader>&& reader);

		////////////////////////////////////////////////////////////////
		//
		//	Parse
		//
		////////////////////////////////////////////////////////////////

		/// @brief SVG データをパースします。
		/// @param source SVG データ
		/// @return SVG オブジェクト
		[[nodiscard]]
		static SVG Parse(const std::string& source);

		/// @brief SVG データをパースします。
		/// @param source SVG データ
		/// @return SVG オブジェクト
		[[nodiscard]]
		static SVG Parse(StringView source);

	private:

		class SVGDetail;

		std::shared_ptr<SVGDetail> pImpl;
	};

}

# include "detail/SVG.ipp"
