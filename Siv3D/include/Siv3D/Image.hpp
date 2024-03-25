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
# include "Array.hpp"
# include "AlignedAllocator.hpp"
# include "ColorHSV.hpp"

namespace s3d
{
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
		explicit Image(size_t size);

		/// @brief 画像データを作成します。
		/// @param size 画像の幅と高さ（ピクセル）
		/// @param color 塗りつぶしの色
		[[nodiscard]]
		explicit Image(size_t size, Color color);

		/// @brief 画像データを作成します。
		/// @param width 画像の幅（ピクセル）
		/// @param height 画像の高さ（ピクセル）
		[[nodiscard]]
		Image(size_t width, size_t height);

		/// @brief 画像データを作成します。
		/// @param width 画像の幅（ピクセル）
		/// @param height 画像の高さ（ピクセル）
		/// @param color 塗りつぶしの色
		[[nodiscard]]
		Image(size_t width, size_t height, Color color);

		/// @brief 画像データを作成します。
		/// @param size 画像の幅と高さ（ピクセル）
		[[nodiscard]]
		explicit Image(Size size);

		/// @brief 画像データを作成します。
		/// @param size 画像の幅と高さ（ピクセル）
		/// @param color 塗りつぶしの色
		[[nodiscard]]
		Image(Size size, Color color);

		//template <class Fty, std::enable_if_t<std::disjunction_v<std::is_invocable_r<Color, Fty>, std::is_invocable_r<Color, Fty, Point>, std::is_invocable_r<Color, Fty, int32, int32>>>* = nullptr>
		//[[nodiscard]]
		//Image(size_t size, Arg::generator_<Fty> generator);

		//template <class Fty, std::enable_if_t<std::disjunction_v<std::is_invocable_r<Color, Fty, Vec2>, std::is_invocable_r<Color, Fty, double, double>>>* = nullptr>
		//[[nodiscard]]
		//Image(size_t size, Arg::generator0_1_<Fty> generator);

		//template <class Fty, std::enable_if_t<std::disjunction_v<std::is_invocable_r<Color, Fty>, std::is_invocable_r<Color, Fty, Point>, std::is_invocable_r<Color, Fty, int32, int32>>>* = nullptr>
		//[[nodiscard]]
		//Image(size_t width, size_t height, Arg::generator_<Fty> generator);

		//template <class Fty, std::enable_if_t<std::disjunction_v<std::is_invocable_r<Color, Fty, Vec2>, std::is_invocable_r<Color, Fty, double, double>>>* = nullptr>
		//[[nodiscard]]
		//Image(size_t width, size_t height, Arg::generator0_1_<Fty> generator);

		//template <class Fty, std::enable_if_t<std::disjunction_v<std::is_invocable_r<Color, Fty>, std::is_invocable_r<Color, Fty, Point>, std::is_invocable_r<Color, Fty, int32, int32>>>* = nullptr>
		//[[nodiscard]]
		//Image(Size size, Arg::generator_<Fty> generator);

		//template <class Fty, std::enable_if_t<std::disjunction_v<std::is_invocable_r<Color, Fty, Vec2>, std::is_invocable_r<Color, Fty, double, double>>>* = nullptr>
		//[[nodiscard]]
		//Image(Size size, Arg::generator0_1_<Fty> generator);

		///// @brief 画像ファイルの内容から画像データを作成します。
		///// @param path 画像ファイルのパス
		///// @param format 画像ファイルのフォーマット。`ImageFormat::Unspecified` の場合は自動で判断
		//[[nodiscard]]
		//explicit Image(FilePathView path, ImageFormat format = ImageFormat::Unspecified);

		///// @brief IReader から画像データを作成します。
		///// @param reader IReader オブジェクト
		///// @param format 画像ファイルのフォーマット。`ImageFormat::Unspecified` の場合は自動で判断
		//[[nodiscard]]
		//explicit Image(IReader&& reader, ImageFormat format = ImageFormat::Unspecified);

		//[[nodiscard]]
		//Image(FilePathView rgb, FilePathView alpha);

		//[[nodiscard]]
		//Image(Color rgb, FilePathView alpha);

		//[[nodiscard]]
		//explicit Image(const Emoji& emoji);

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
		//	stride
		//
		////////////////////////////////////////////////////////////////

		/// @brief 画像の各行のサイズ（バイト）を返します。
		/// @remark `(width() * sizeof(Color))` です。
		/// @return 画像の各行のサイズ（バイト）
		[[nodiscard]]
		size_t stride() const noexcept;

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

		template <class Type = double>
		[[nodiscard]]
		Type horizontalAspectRatio() const noexcept;

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
		Image cloned() const;

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
		Color& operator [](Point pos);

		/// @brief 指定した位置のピクセルの参照を返します。
		/// @param pos 位置
		/// @return 指定した位置のピクセルの参照
		[[nodiscard]]
		const Color& operator [](Point pos) const;

	# ifdef __cpp_multidimensional_subscript

		/// @brief 指定した位置のピクセルの参照を返します。
		/// @param x 位置（列）
		/// @param y 位置（行）
		/// @return 指定した位置のピクセルの参照
		[[nodiscard]]
		Color& operator [](size_t x, size_t y);

		/// @brief 指定した位置のピクセルの参照を返します。
		/// @param x 位置（列）
		/// @param y 位置（行）
		/// @return 指定した位置のピクセルの参照
		[[nodiscard]]
		const Color& operator [](size_t x, size_t y) const;

	# endif

		////////////////////////////////////////////////////////////////
		//
		//	data
		//
		////////////////////////////////////////////////////////////////

		/// @brief 画像データの先頭ポインタを返します。
		/// @return 画像データの先頭ポインタ
		[[nodiscard]]
		Color* data();

		/// @brief 画像データの先頭ポインタを返します。
		/// @return 画像データの先頭ポインタ
		[[nodiscard]]
		const Color* data() const;

		////////////////////////////////////////////////////////////////
		//
		//	dataAsUint8
		//
		////////////////////////////////////////////////////////////////

		/// @brief 画像データの先頭ポインタを uint8* 型で返します。
		/// @return 画像データの先頭ポインタ
		[[nodiscard]]
		uint8* dataAsUint8();

		/// @brief 画像データの先頭ポインタを uint8* 型で返します。
		/// @return 画像データの先頭ポインタ
		[[nodiscard]]
		const uint8* dataAsUint8() const;

		////////////////////////////////////////////////////////////////
		//
		//	begin, end
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		iterator begin() noexcept;

		[[nodiscard]]
		iterator end() noexcept;

		[[nodiscard]]
		const_iterator begin() const noexcept;

		[[nodiscard]]
		const_iterator end() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	cbegin, cend
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		const_iterator cbegin() const noexcept;

		[[nodiscard]]
		const_iterator cend() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	rbegin, rend
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		reverse_iterator rbegin() noexcept;

		[[nodiscard]]
		reverse_iterator rend() noexcept;

		[[nodiscard]]
		const_reverse_iterator rbegin() const noexcept;

		[[nodiscard]]
		const_reverse_iterator rend() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	crbegin, crend
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		const_reverse_iterator crbegin() const noexcept;

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

		/// @brief 画像のデータのサイズを変更します。
		/// @param width 新しい幅（ピクセル）
		/// @param height 新しい高さ（ピクセル）
		/// @remark サイズが変更された場合、画像データの内容は不定になります。
		void resize(size_t width, size_t height);

		/// @brief 画像のデータのサイズを変更します。
		/// @param size 新しい幅と高さ（ピクセル）
		/// @remark サイズが変更された場合、画像データの内容は不定になります。
		void resize(Size size);

		void resize(size_t width, size_t height, Color fillColor);

		void resize(Size size, Color fillColor);

		////////////////////////////////////////////////////////////////
		//
		//	resizeRows
		//
		////////////////////////////////////////////////////////////////

		void resizeRows(size_t rows, Color fillColor);











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

		Size m_size;

		base_type m_pixels;

		[[nodiscard]]
		static constexpr Size ClampImageSize(Size size) noexcept;
	};
}

# include "detail/Image.ipp"
