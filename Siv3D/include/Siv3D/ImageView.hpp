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
# include <span>
# include "Color.hpp"
# include "PointVector.hpp"

namespace s3d
{
	class Image;
	class MutableImageView;
	struct Rect;

	/// @brief Image の矩形領域を参照する読み取り専用ビュー
	/// @remark 画素を所有しません。元画像の破棄・代入・サイズ変更・swap・ムーブ後は取得し直してください。
	/// @remark 座標はビューの左上を原点とします。行間隔は元画像のままで、画素全体が連続しているとは限りません。
	/// @remark ビューのコピーや一時ビューからのアクセスは、元の画素の寿命を延長しません。
	class ImageView
	{
	public:

		/// @brief 空のビューを作成します。
		[[nodiscard]]
		ImageView() = default;

		/// @brief 書き込み可能なビューから読み取り専用ビューを作成します。
		[[nodiscard]]
		ImageView(MutableImageView view) noexcept;

		/// @brief 幅（ピクセル）を返します。
		[[nodiscard]]
		int32 width() const noexcept;

		/// @brief 高さ（ピクセル）を返します。
		[[nodiscard]]
		int32 height() const noexcept;

		/// @brief 幅と高さ（ピクセル）を返します。
		[[nodiscard]]
		Size size() const noexcept;

		/// @brief 行の先頭間の距離（バイト）を返します。空なら 0 です。
		/// @remark 幅 × sizeof(Color) より大きい場合があります。
		[[nodiscard]]
		uint32 bytesPerRow() const noexcept;

		/// @brief ビューに含まれる画素数を返します。行間の画素は含みません。
		[[nodiscard]]
		size_t pixelCount() const noexcept;

		/// @brief 空のビューなら true を返します。
		[[nodiscard]]
		bool isEmpty() const noexcept;

		/// @brief 空でないビューなら true を返します。
		[[nodiscard]]
		explicit operator bool() const noexcept;

		/// @brief 左上の画素へのポインタを返します。空なら nullptr です。
		/// @remark Image::DataAlignment へのアラインメントは保証されません。
		[[nodiscard]]
		const Color* data() const noexcept;

		/// @brief 指定した行の画素を参照する span を返します。
		/// @pre y < height()
		[[nodiscard]]
		std::span<const Color> row(size_t y) const noexcept;

		/// @brief 指定した行の先頭ポインタを返します。view[y][x] で画素を参照できます。
		/// @pre y < height()
		[[nodiscard]]
		const Color* operator [](size_t y) const noexcept;

		/// @brief 局所座標の画素を参照します。
		/// @pre pos がビューの範囲内であること
		[[nodiscard]]
		const Color& operator [](Point pos) const noexcept;

		/// @brief 指定した行・列の画素を参照します。
		/// @pre y < height() かつ x < width()
		[[nodiscard]]
		const Color& operator [](size_t y, size_t x) const noexcept;

		/// @brief ビューと rect の交差領域を参照します。rect は局所座標です。
		/// @return 交差がない場合や rect の幅・高さが非正の場合は空のビュー
		[[nodiscard]]
		ImageView subview(Rect rect) const noexcept;

		/// @brief ビューの画素を独立した Image にコピーします。
		/// @return ビューと同じサイズの画像。ビューが空なら空の画像
		[[nodiscard]]
		Image cloned() const;

		/// @brief dst をビューと同じサイズにし、画素をコピーします。
		/// @param dst コピー先。空のビューでは clear() されます。
		/// @remark dst の記憶領域を再利用します。dst 内の領域を参照している場合もコピーできます。
		/// @throws std::bad_alloc メモリ確保に失敗した場合。dst は変更されません。
		void copyTo(Image& dst) const;

		/// @brief ビューの画素を dst の pos へ上書きコピーします。
		/// @param dst コピー先のビュー
		/// @param pos dst の局所座標でのコピー位置
		/// @remark dst の範囲外を切り捨てます。重複領域もコピー開始時の画素を使います。alpha 合成は行いません。
		void overwrite(MutableImageView dst, Point pos = Point{ 0, 0 }) const noexcept;

		/// @brief 画像全体をコピー先とする overwrite() です。
		void overwrite(Image& dst, Point pos = Point{ 0, 0 }) const noexcept;

	private:

		const Color* m_data = nullptr;
		Size m_size{ 0, 0 };
		int32 m_stride = 0;

		ImageView(const Color* data, Size size, int32 stride) noexcept;

		friend class Image;
	};
}

# include "detail/ImageView.ipp"
