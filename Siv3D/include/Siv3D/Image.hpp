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
# include "Array.hpp"
# include "AlignedAllocator.hpp"
# include "ColorHSV.hpp"
# include "Emoji.hpp"
# include "ImageFormat.hpp"
# include "PredefinedYesNo.hpp"

namespace s3d
{
	class IReader;

	////////////////////////////////////////////////////////////////
	//
	//	Image
	//
	////////////////////////////////////////////////////////////////

	/// @brief 画像データ
	class Image
	{
	public:

		/// @brief 作成可能な最大の画像の幅（ピクセル）
		static constexpr int32 MaxWidth		= 16384;

		/// @brief 作成可能な最大の画像の高さ（ピクセル）
		static constexpr int32 MaxHeight	= 16384;
		
		/// @brief 画像データのアライメント（バイト）
		/// @remark メモリはこの値の倍数で確保されます。
		static constexpr int32 DataAlignment	= 32;

		static constexpr int32 PixelAlignment	= (DataAlignment / sizeof(Color));

		using base_type					= Array<Color, AlignedAllocator<Color, DataAlignment>>;
		using iterator					= base_type::iterator;
		using const_iterator			= base_type::const_iterator;
		using reverse_iterator			= base_type::reverse_iterator;
		using const_reverse_iterator	= base_type::const_reverse_iterator;

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		/// @brief デフォルトコンストラクタ
		[[nodiscard]]
		Image() = default;

		/// @brief コピーコンストラクタ
		[[nodiscard]]
		Image(const Image&) = default;

		/// @brief ムーブコンストラクタ
		/// @param image ムーブする画像データ
		[[nodiscard]]
		Image(Image&& image) noexcept;

		/// @brief 画像データを作成します。
		/// @param size 画像の幅と高さ（ピクセル）
		[[nodiscard]]
		explicit Image(Concept::Integral auto size);

		/// @brief 画像データを作成します。
		/// @param size 画像の幅と高さ（ピクセル）
		/// @param color 塗りつぶしの色
		[[nodiscard]]
		explicit Image(Concept::Integral auto size, Color color);

		/// @brief 画像データを作成します。
		/// @param width 画像の幅（ピクセル）
		/// @param height 画像の高さ（ピクセル）
		[[nodiscard]]
		Image(Concept::Integral auto width, Concept::Integral auto height);

		/// @brief 画像データを作成します。
		/// @param width 画像の幅（ピクセル）
		/// @param height 画像の高さ（ピクセル）
		/// @param color 塗りつぶしの色
		[[nodiscard]]
		Image(Concept::Integral auto width, Concept::Integral auto height, Color color);

		/// @brief 画像データを作成します。
		/// @param size 画像の幅と高さ（ピクセル）
		[[nodiscard]]
		explicit Image(Size size);

		/// @brief 画像データを作成します。
		/// @param size 画像の幅と高さ（ピクセル）
		/// @param color 塗りつぶしの色
		[[nodiscard]]
		Image(Size size, Color color);

		[[nodiscard]]
		Image(Concept::Integral auto size, Arg::generator_<FunctionRef<Color()>> generator);

		[[nodiscard]]
		Image(Concept::Integral auto size, Arg::generator_<FunctionRef<Color(int32, int32)>> generator);

		[[nodiscard]]
		Image(Concept::Integral auto size, Arg::generator_<FunctionRef<Color(Point)>> generator);

		[[nodiscard]]
		Image(Concept::Integral auto size, Arg::generator0_1_<FunctionRef<Color(double, double)>> generator);

		[[nodiscard]]
		Image(Concept::Integral auto size, Arg::generator0_1_<FunctionRef<Color(Vec2)>> generator);

		[[nodiscard]]
		Image(Concept::Integral auto width, Concept::Integral auto height, Arg::generator_<FunctionRef<Color()>> generator);

		[[nodiscard]]
		Image(Concept::Integral auto width, Concept::Integral auto height, Arg::generator_<FunctionRef<Color(int32, int32)>> generator);

		[[nodiscard]]
		Image(Concept::Integral auto width, Concept::Integral auto height, Arg::generator_<FunctionRef<Color(Point)>> generator);

		[[nodiscard]]
		Image(Concept::Integral auto width, Concept::Integral auto height, Arg::generator0_1_<FunctionRef<Color(double, double)>> generator);

		[[nodiscard]]
		Image(Concept::Integral auto width, Concept::Integral auto height, Arg::generator0_1_<FunctionRef<Color(Vec2)>> generator);

		[[nodiscard]]
		Image(Size size, Arg::generator_<FunctionRef<Color()>> generator);

		[[nodiscard]]
		Image(Size size, Arg::generator_<FunctionRef<Color(int32, int32)>> generator);

		[[nodiscard]]
		Image(Size size, Arg::generator_<FunctionRef<Color(Point)>> generator);

		[[nodiscard]]
		Image(Size size, Arg::generator0_1_<FunctionRef<Color(double, double)>> generator);

		[[nodiscard]]
		Image(Size size, Arg::generator0_1_<FunctionRef<Color(Vec2)>> generator);

		/// @brief 画像ファイルの内容から画像データを作成します。
		/// @param path 画像ファイルのパス
		/// @param premultiplyAlpha アルファ乗算処理を適用するか
		/// @param format 画像ファイルのフォーマット。`ImageFormat::Unspecified` の場合は自動で判断
		[[nodiscard]]
		explicit Image(FilePathView path, PremultiplyAlpha premultiplyAlpha = PremultiplyAlpha::Yes, ImageFormat format = ImageFormat::Unspecified);

		/// @brief Reader から画像ファイルを読み込んで画像データを作成します。
		/// @param reader Reader オブジェクト
		/// @param premultiplyAlpha アルファ乗算処理を適用するか
		/// @param format 画像ファイルのフォーマット。`ImageFormat::Unspecified` の場合は自動で判断
		[[nodiscard]]
		explicit Image(IReader&& reader, PremultiplyAlpha premultiplyAlpha = PremultiplyAlpha::Yes, ImageFormat format = ImageFormat::Unspecified);

		/// @brief 一方の画像ファイルから RGB チャンネルを、もう一方の画像ファイルからアルファチャンネルを読み込んで画像データを作成します。
		/// @param rgb RGB チャンネルの画像ファイルのパス
		/// @param alpha アルファチャンネルの画像ファイルのパス（R 成分がアルファチャンネルとして使用されます）
		/// @param premultiplyAlpha アルファ乗算処理を適用するか
		[[nodiscard]]
		Image(FilePathView rgb, FilePathView alpha, PremultiplyAlpha premultiplyAlpha = PremultiplyAlpha::Yes);

		/// @brief 指定した色を RGB チャンネルに、指定した画像ファイルの内容をアルファチャンネルに使用して画像データを作成します。
		/// @param rgb RGB チャンネルの色
		/// @param alpha アルファチャンネルの画像ファイルのパス（R 成分がアルファチャンネルとして使用されます）
		/// @param premultiplyAlpha アルファ乗算処理を適用するか
		[[nodiscard]]
		Image(Color rgb, FilePathView alpha, PremultiplyAlpha premultiplyAlpha = PremultiplyAlpha::Yes);

		[[nodiscard]]
		explicit Image(const Emoji& emoji, int32 size = Emoji::DefaultSize);

		//[[nodiscard]]
		//explicit Image(const Icon& icon, int32 size);

		///// @brief 二次元配列から画像データを作成します。
		///// @param grid 二次元配列
		//[[nodiscard]]
		//explicit Image(const Grid<Color>& grid);

		///// @brief 二次元配列から画像データを作成します。
		///// @param grid 二次元配列
		//[[nodiscard]]
		//explicit Image(const Grid<ColorF>& grid);

		//template <class Type, class Fty, std::enable_if_t<std::is_invocable_r_v<Color, Fty, Type>>* = nullptr>
		//[[nodiscard]]
		//explicit Image(const Grid<Type>& grid, Fty converter);

		////////////////////////////////////////////////////////////////
		//
		//	operator =
		//
		////////////////////////////////////////////////////////////////

		Image& operator =(const Image&) = default;

		Image& operator =(Image&& image) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	width, height
		//
		////////////////////////////////////////////////////////////////

		/// @brief 画像の幅（ピクセル）を返します。
		/// @return 画像の幅（ピクセル）
		[[nodiscard]]
		int32 width() const noexcept;

		/// @brief 画像の高さ（ピクセル）を返します。
		/// @return 画像の高さ（ピクセル）
		[[nodiscard]]
		int32 height() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	bytesPerRow
		//
		////////////////////////////////////////////////////////////////

		/// @brief 画像の各行のサイズ（バイト）を返します。
		/// @remark `(width() * sizeof(Color))` です。
		/// @return 画像の各行のサイズ（バイト）
		[[nodiscard]]
		uint32 bytesPerRow() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	size
		//
		////////////////////////////////////////////////////////////////

		/// @brief 画像の幅と高さ（ピクセル）を返します。
		/// @return 画像の幅と高さ（ピクセル）		
		[[nodiscard]]
		Size size() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	size_bytes
		//
		////////////////////////////////////////////////////////////////

		/// @brief 画像のデータサイズ（バイト）を返します。
		/// @remark `(stride() * height())` です。
		/// @return 画像のデータサイズ（バイト）
		[[nodiscard]]
		size_t size_bytes() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	num_pixels
		//
		////////////////////////////////////////////////////////////////

		/// @brief 画像の総ピクセル数を返します。
		/// @remark `(width() * height())` です。
		/// @return 画像の総ピクセル数
		[[nodiscard]]
		size_t num_pixels() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	isEmpty
		//
		////////////////////////////////////////////////////////////////

		/// @brief 画像が空であるかを返します。
		/// @return 画像が空である場合 true, それ以外の場合は false
		[[nodiscard]]
		bool isEmpty() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator bool
		//
		////////////////////////////////////////////////////////////////

		/// @brief 画像が空でないかを返します。
		/// @return 画像が空でない場合 true, それ以外の場合は false
		[[nodiscard]]
		explicit operator bool() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	horizontalAspectRatio
		//
		////////////////////////////////////////////////////////////////

		template <Concept::FloatingPoint Float = double>
		[[nodiscard]]
		Float horizontalAspectRatio() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	shrink_to_fit
		//
		////////////////////////////////////////////////////////////////

		/// @brief 使用するメモリ量を現在のサイズまで切り詰めます。
		void shrink_to_fit();

		////////////////////////////////////////////////////////////////
		//
		//	clear
		//
		////////////////////////////////////////////////////////////////

		/// @brief 画像を消去し、空の画像にします。
		/// @remark メモリを解放したい場合は、さらに shrink_to_fit() を呼びます。
		void clear() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	release
		//
		////////////////////////////////////////////////////////////////

		/// @brief 画像を消去して空の画像にし、使用するメモリ量を切り詰めます。
		/// @remark `clear()` + `shrink_to_fit()` と同じです。
		void release();

		////////////////////////////////////////////////////////////////
		//
		//	swap
		//
		////////////////////////////////////////////////////////////////

		/// @brief 画像を別の画像と交換します。
		/// @param image 交換する画像
		void swap(Image& image) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	cloned
		//
		////////////////////////////////////////////////////////////////

		/// @brief 内容をコピーした新しい画像を作成して返します。
		/// @return 内容をコピーした新しい画像
		[[nodiscard]]
		Image cloned() const&;

		/// @brief 内容をコピーした新しい画像を作成して返します。
		/// @return 内容をコピーした新しい画像
		[[nodiscard]]
		Image cloned() &&;

		////////////////////////////////////////////////////////////////
		//
		//	indexInBounds
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		bool indexInBounds(int64 y, int64 x) const noexcept;

		[[nodiscard]]
		bool indexInBounds(Point pos) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator []
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した行の先頭ポインタを返します。
		/// @param y 位置（行）
		/// @remark image[y][x] で指定したピクセルにアクセスします。
		/// @return 指定した行の先頭ポインタ
		[[nodiscard]]
		Color* operator [](size_t y);

		/// @brief 指定した行の先頭ポインタを返します。
		/// @param y 位置（行）
		/// @remark image[y][x] で指定したピクセルにアクセスします。
		/// @return 指定した行の先頭ポインタ
		[[nodiscard]]
		const Color* operator [](size_t y) const;

		/// @brief 指定した位置のピクセルの参照を返します。
		/// @param pos 位置
		/// @return 指定した位置のピクセルの参照
		[[nodiscard]]
		Color& operator [](Point pos)&;

		/// @brief 指定した位置のピクセルの参照を返します。
		/// @param pos 位置
		/// @return 指定した位置のピクセルの参照
		[[nodiscard]]
		const Color& operator [](Point pos) const&;

		/// @brief 指定した位置のピクセルを返します。
		/// @param pos 位置
		/// @return 指定した位置のピクセル
		[[nodiscard]]
		Color operator [](Point pos)&&;

	# ifdef __cpp_multidimensional_subscript

		/// @brief 指定した位置のピクセルの参照を返します。
		/// @param y 位置（行）
		/// @param x 位置（列）
		/// @remark image[y, x] で指定したピクセルにアクセスします。
		/// @return 指定した位置のピクセルの参照
		[[nodiscard]]
		Color& operator [](size_t y, size_t x)&;

		/// @brief 指定した位置のピクセルの参照を返します。
		/// @param y 位置（行）
		/// @param x 位置（列）
		/// @remark image[y, x] で指定したピクセルにアクセスします。
		/// @return 指定した位置のピクセルの参照
		[[nodiscard]]
		const Color& operator [](size_t y, size_t x) const&;

		/// @brief 指定した位置のピクセルを返します。
		/// @param y 位置（行）
		/// @param x 位置（列）
		/// @remark image[y, x] で指定したピクセルにアクセスします。
		/// @return 指定した位置のピクセル
		[[nodiscard]]
		Color operator [](size_t y, size_t x)&&;

	# endif

		////////////////////////////////////////////////////////////////
		//
		//	data
		//
		////////////////////////////////////////////////////////////////

		/// @brief 画像データの先頭ポインタを返します。
		/// @return 画像データの先頭ポインタ
		/// @remark 先頭ポインタは `DataAlignment` にアラインメントされています。
		/// @remark メモリは `DataAlignment` の倍数になるよう（必要な場合は余分に）確保されています。
		[[nodiscard]]
		Color* data();

		/// @brief 画像データの先頭ポインタを返します。
		/// @return 画像データの先頭ポインタ
		/// @remark 先頭ポインタは `DataAlignment` にアラインメントされています。
		/// @remark メモリは `DataAlignment` の倍数になるよう（必要な場合は余分に）確保されています。
		[[nodiscard]]
		const Color* data() const;

		////////////////////////////////////////////////////////////////
		//
		//	dataAsUint8
		//
		////////////////////////////////////////////////////////////////

		/// @brief 画像データの先頭ポインタを uint8* 型で返します。
		/// @return 画像データの先頭ポインタ
		/// @remark 先頭ポインタは `DataAlignment` にアラインメントされています。
		/// @remark メモリは `DataAlignment` の倍数になるよう（必要な場合は余分に）確保されています。
		[[nodiscard]]
		uint8* dataAsUint8();

		/// @brief 画像データの先頭ポインタを uint8* 型で返します。
		/// @return 画像データの先頭ポインタ
		/// @remark 先頭ポインタは `DataAlignment` にアラインメントされています。
		/// @remark メモリは `DataAlignment` の倍数になるよう（必要な場合は余分に）確保されています。
		[[nodiscard]]
		const uint8* dataAsUint8() const;

		////////////////////////////////////////////////////////////////
		//
		//	begin, end
		//
		////////////////////////////////////////////////////////////////

		/// @brief ピクセル配列の先頭位置を指すイテレータを返します。
		/// @return ピクセル配列の先頭位置を指すイテレータ
		[[nodiscard]]
		iterator begin() noexcept;

		/// @brief ピクセル配列の終端位置を指すイテレータを返します。
		/// @return ピクセル配列の終端位置を指すイテレータ
		[[nodiscard]]
		iterator end() noexcept;

		/// @brief ピクセル配列の先頭位置を指すイテレータを返します。
		/// @return ピクセル配列の先頭位置を指すイテレータ
		[[nodiscard]]
		const_iterator begin() const noexcept;

		/// @brief ピクセル配列の終端位置を指すイテレータを返します。
		/// @return ピクセル配列の終端位置を指すイテレータ
		[[nodiscard]]
		const_iterator end() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	cbegin, cend
		//
		////////////////////////////////////////////////////////////////

		/// @brief ピクセル配列の先頭位置を指すイテレータを返します。
		/// @return ピクセル配列の先頭位置を指すイテレータ
		[[nodiscard]]
		const_iterator cbegin() const noexcept;

		/// @brief ピクセル配列の終端位置を指すイテレータを返します。
		/// @return ピクセル配列の終端位置を指すイテレータ
		[[nodiscard]]
		const_iterator cend() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	rbegin, rend
		//
		////////////////////////////////////////////////////////////////

		/// @brief ピクセル配列の末尾位置を指すリバース・イテレータを返します。
		/// @return ピクセル配列の末尾位置を指すリバース・イテレータ
		[[nodiscard]]
		reverse_iterator rbegin() noexcept;

		/// @brief ピクセル配列の先端位置を指すリバース・イテレータを返します。
		/// @return ピクセル配列の先端位置を指すリバース・イテレータ
		[[nodiscard]]
		reverse_iterator rend() noexcept;

		/// @brief ピクセル配列の末尾位置を指すリバース・イテレータを返します。
		/// @return ピクセル配列の末尾位置を指すリバース・イテレータ
		[[nodiscard]]
		const_reverse_iterator rbegin() const noexcept;

		/// @brief ピクセル配列の先端位置を指すリバース・イテレータを返します。
		/// @return ピクセル配列の先端位置を指すリバース・イテレータ
		[[nodiscard]]
		const_reverse_iterator rend() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	crbegin, crend
		//
		////////////////////////////////////////////////////////////////

		/// @brief ピクセル配列の末尾位置を指すリバース・イテレータを返します。
		/// @return ピクセル配列の末尾位置を指すリバース・イテレータ
		[[nodiscard]]
		const_reverse_iterator crbegin() const noexcept;

		/// @brief ピクセル配列の先端位置を指すリバース・イテレータを返します。
		/// @return ピクセル配列の先端位置を指すリバース・イテレータ
		[[nodiscard]]
		const_reverse_iterator crend() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	fill
		//
		////////////////////////////////////////////////////////////////

		/// @brief 画像を指定した色で塗りつぶします。
		/// @param color 塗りつぶしの色
		void fill(Color color) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	resize
		//
		////////////////////////////////////////////////////////////////

		/// @brief 画像のサイズを変更します。
		/// @param width 新しい幅（ピクセル）
		/// @param height 新しい高さ（ピクセル）
		/// @remark サイズが変更された場合、画像の内容は不定になります。
		void resize(size_t width, size_t height);

		/// @brief 画像のサイズを変更します。
		/// @param size 新しい幅と高さ（ピクセル）
		/// @remark サイズが変更された場合、画像の内容は不定になります。
		void resize(Size size);

		/// @brief 画像のサイズを変更します。
		/// @param width 新しい幅（ピクセル）
		/// @param height 新しい高さ（ピクセル）
		/// @param fillColor リサイズ後の画像を塗りつぶす色
		/// @remark リサイズ後の画像は `fillColor` で塗りつぶされます。
		void resize(size_t width, size_t height, Color fillColor);

		/// @brief 画像のサイズを変更します。
		/// @param size 新しい幅と高さ（ピクセル）
		/// @param fillColor リサイズ後の画像を塗りつぶす色
		/// @remark リサイズ後の画像は `fillColor` で塗りつぶされます。
		void resize(Size size, Color fillColor);

		////////////////////////////////////////////////////////////////
		//
		//	resizeHeight
		//
		////////////////////////////////////////////////////////////////

		/// @brief 画像の高さを変更します。
		/// @param height 新しい高さ（ピクセル）
		/// @remark 高さが増えた部分は `Color{ 0, 0, 0, 0 }` で塗りつぶされます。
		void resizeHeight(size_t height);

		/// @brief 画像の高さを変更します。
		/// @param height 新しい高さ（ピクセル）
		/// @param fillColor 増えた部分を塗りつぶす色
		/// @remark 高さが増えた部分は `fillColor` で塗りつぶされます。
		void resizeHeight(size_t height, Color fillColor);
















		////////////////////////////////////////////////////////////////
		//
		//	premultiplyAlpha
		//
		////////////////////////////////////////////////////////////////

		void premultiplyAlpha(bool useSIMD = true);

		////////////////////////////////////////////////////////////////
		//
		//	bgraToRGBA
		//
		////////////////////////////////////////////////////////////////

		void bgraToRGBA(bool useSIMD = true);

		////////////////////////////////////////////////////////////////
		//
		//	save
		//
		////////////////////////////////////////////////////////////////

		/// @brief 画像をファイルに保存します。
		/// @param path 保存するファイルパス
		/// @param format 保存時のフォーマット。`ImageFormat::Unspecified` の場合拡張子から判断
		/// @return 保存に成功した場合 true, それ以外の場合は false
		bool save(FilePathView path, ImageFormat format = ImageFormat::Unspecified) const;



		////////////////////////////////////////////////////////////////
		//
		//	overwrite
		//
		////////////////////////////////////////////////////////////////

		void overwrite(Image& dst, Point pos) const;


		////////////////////////////////////////////////////////////////
		//
		//	swap
		//
		////////////////////////////////////////////////////////////////

		friend void swap(Image& lhs, Image& rhs) noexcept
		{
			lhs.swap(rhs);
		}


		////////////////////////////////////////////////////////////////
		//
		//	operator ==
		//
		////////////////////////////////////////////////////////////////

		/// @brief 2 つの画像データが等しいかを返します。
		/// @param lhs 一方の画像データ
		/// @param rhs もう一方の画像データ
		/// @return 2 つの画像データが等しい場合 true, それ以外の場合は false
		friend bool operator ==(const Image& lhs, const Image& rhs) noexcept
		{
			return ((lhs.m_size == rhs.m_size)
				&& (std::memcmp(lhs.m_pixels.data(), rhs.m_pixels.data(), lhs.size_bytes()) == 0));
		}

	private:

		Size m_size{ 0, 0 };

		base_type m_pixels;

		[[nodiscard]]
		static constexpr Size ValidImageSizeOrEmpty(Size size) noexcept;
	};
}

# include "detail/Image.ipp"
