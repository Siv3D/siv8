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
# include "PointVector.hpp"
# include "ColorHSV.hpp"
# include "Optional.hpp"
# include "PredefinedNamedParameter.hpp"
# include "PredefinedYesNo.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	Line
	//
	////////////////////////////////////////////////////////////////

	/// @brief 線分
	struct Line
	{
		using position_type	= Vec2;

		using vector_type	= position_type;

		using value_type	= position_type::value_type;

		/// @brief 線分の開始位置
		position_type start;

		/// @brief 線分の終点位置
		position_type end;

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		/// @brief デフォルトコンストラクタ
		[[nodiscard]]
		Line() = default;

		[[nodiscard]]
		constexpr Line(value_type startX, value_type startY, value_type endX, value_type endY) noexcept;

		[[nodiscard]]
		constexpr Line(Concept::Arithmetic auto startX, Concept::Arithmetic auto startY, Concept::Arithmetic auto endX, Concept::Arithmetic auto endY) noexcept;

		[[nodiscard]]
		constexpr Line(position_type _start, value_type endX, value_type endY) noexcept;

		[[nodiscard]]
		constexpr Line(position_type _start, Concept::Arithmetic auto endX, Concept::Arithmetic auto endY) noexcept;

		[[nodiscard]]
		constexpr Line(value_type startX, value_type startY, position_type _end) noexcept;

		[[nodiscard]]
		constexpr Line(Concept::Arithmetic auto startX, Concept::Arithmetic auto startY, position_type _end) noexcept;

		[[nodiscard]]
		constexpr Line(position_type _start, position_type _end) noexcept;

		[[nodiscard]]
		constexpr Line(value_type originX, value_type originY, Arg::direction_<vector_type> vector) noexcept;

		[[nodiscard]]
		constexpr Line(Concept::Arithmetic auto originX, Concept::Arithmetic auto originY, Arg::direction_<vector_type> vector) noexcept;

		[[nodiscard]]
		constexpr Line(position_type origin, Arg::direction_<vector_type> direction) noexcept;

		[[nodiscard]]
		Line(value_type originX, value_type originY, Arg::angle_<value_type> angle, value_type length) noexcept;

		[[nodiscard]]
		Line(value_type originX, value_type originY, Arg::angle_<value_type> angle, Concept::Arithmetic auto length) noexcept;

		[[nodiscard]]
		Line(position_type origin, Arg::angle_<value_type> angle, value_type length) noexcept;

		[[nodiscard]]
		Line(position_type origin, Arg::angle_<value_type> angle, Concept::Arithmetic auto length) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator ==
		//
		////////////////////////////////////////////////////////////////

		/// @brief 2 つの Line が等しいかを返します。
		/// @param lhs 一方の Line
		/// @param rhs もう一方の Line
		/// @return 2 つの Line が等しい場合 true, それ以外の場合は false
		[[nodiscard]]
		friend constexpr bool operator ==(const Line lhs, const Line rhs) noexcept
		{
			return ((lhs.start == rhs.start)
				 && (lhs.end == rhs.end));
		}

		////////////////////////////////////////////////////////////////
		//
		//	withStart
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr Line withStart(value_type startX, value_type startY) const noexcept;

		[[nodiscard]]
		constexpr Line withStart(position_type _start) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	withEnd
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr Line withEnd(value_type endX, value_type endY) const noexcept;

		[[nodiscard]]
		constexpr Line withEnd(position_type _end) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	setStart
		//
		////////////////////////////////////////////////////////////////

		constexpr Line& setStart(value_type startX, value_type startY) noexcept;

		constexpr Line& setStart(position_type _start) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	setEnd
		//
		////////////////////////////////////////////////////////////////

		constexpr Line& setEnd(value_type endX, value_type endY) noexcept;

		constexpr Line& setEnd(position_type _end) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	set
		//
		////////////////////////////////////////////////////////////////

		constexpr Line& set(value_type x0, value_type y0, value_type x1, value_type y1) noexcept;

		constexpr Line& set(position_type p0, value_type x1, value_type y1) noexcept;

		constexpr Line& set(value_type x0, value_type y0, position_type p1) noexcept;

		constexpr Line& set(position_type p0, position_type p1) noexcept;

		constexpr Line& set(const Line& line) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	movedBy
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr Line movedBy(value_type x, value_type y) const noexcept;

		[[nodiscard]]
		constexpr Line movedBy(vector_type v) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	moveBy
		//
		////////////////////////////////////////////////////////////////

		constexpr Line& moveBy(value_type x, value_type y) noexcept;

		constexpr Line& moveBy(vector_type v) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	stretched
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Line stretched(value_type extentionBothSides) const noexcept;

		[[nodiscard]]
		Line stretched(value_type extentionStart, value_type extentionEnd) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	scaledFromStart
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Line scaledFromStart(value_type scale) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	scaledFromEnd
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Line scaledFromEnd(value_type scale) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	scaledFromCenter
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Line scaledFromCenter(value_type scale) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	rotatedAtStart
		//
		////////////////////////////////////////////////////////////////
		
		[[nodiscard]]
		Line rotatedAtStart(double angle) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	rotatedAtEnd
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Line rotatedAtEnd(double angle) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	rotatedAtCenter
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Line rotatedAtCenter(double angle) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	vector
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr vector_type vector() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	normalizedVector
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		vector_type normalizedVector() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	perpendicularUnitVector
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		vector_type perpendicularUnitVector() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	reversed
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr Line reversed() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	reverse
		//
		////////////////////////////////////////////////////////////////

		constexpr Line& reverse() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	hasLength
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr bool hasLength() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	length
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		value_type length() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	lengthSq
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr value_type lengthSq() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	pointAtIndex
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		position_type& pointAtIndex(size_t index) noexcept;

		[[nodiscard]]
		const position_type& pointAtIndex(size_t index) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	interpolatedPointAt
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr position_type interpolatedPointAt(double t) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	center
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr position_type center() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	withCenter
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr Line withCenter(value_type x, value_type y) const noexcept;

		[[nodiscard]]
		constexpr Line withCenter(position_type pos) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	setCenter
		//
		////////////////////////////////////////////////////////////////

		constexpr Line& setCenter(value_type x, value_type y) noexcept;

		constexpr Line& setCenter(position_type pos) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	closestPointTo
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		position_type closestPointTo(position_type pos) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	boundingRect
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		RectF boundingRect() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	lerp
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr Line lerp(const Line& other, double f) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	hash
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		uint64 hash() const noexcept;

		//template <class Shape2DType>
		//[[nodiscard]]
		//constexpr bool intersects(const Shape2DType& other) const;

		//template <class Shape2DType>
		//[[nodiscard]]
		//Optional<Array<Vec2>> intersectsAt(const Shape2DType& other) const;

		///// @brief 別の線分 other との交点を返します。
		///// @param other 別の線分
		///// @return 交差しない場合は none, 交差する場合はその座標、2 つの線分が重なっている場合 (QNaN, QNaN)
		//[[nodiscard]]
		//Optional<position_type> intersectsAt(const Line& other) const;

		///// @brief 別の線分 other との交点を返します。`intersectsAt()` と異なり、両者の順序が異なっても一致する結果を返します。
		///// @param other 別の線分
		///// @return 交差しない場合は none, 交差する場合はその座標、2 つの線分が重なっている場合 (QNaN, QNaN)
		//[[nodiscard]]
		//Optional<position_type> intersectsAtPrecise(const Line& other) const;

		//const Line& paint(Image& dst, const Color& color) const;

		//const Line& paint(Image& dst, int32 thickness, const Color& color) const;

		//const Line& overwrite(Image& dst, const Color& color, Antialiased antialiased = Antialiased::Yes) const;

		//const Line& overwrite(Image& dst, int32 thickness, const Color& color, Antialiased antialiased = Antialiased::Yes) const;

		//const Line& paintArrow(Image& dst, double width, const Vec2& headSize, const Color& color) const;

		//const Line& overwriteArrow(Image& dst, double width, const Vec2& headSize, const Color& color) const;

		//const Line& paintDoubleHeadedArrow(Image& dst, double width, const Vec2& headSize, const Color& color) const;

		//const Line& overwriteDoubleHeadedArrow(Image& dst, double width, const Vec2& headSize, const Color& color) const;

		///// @brief 線分を描きます。
		///// @param color 色
		///// @return *this
		//const Line& draw(const ColorF& color = Palette::White) const;

		///// @brief 線分を描きます。
		///// @param colorBegin 始点側の色
		///// @param colorEnd 終点側の色
		///// @return *this
		//const Line& draw(const ColorF& colorBegin, const ColorF& colorEnd) const;

		///// @brief 線分を描きます。
		///// @param thickness 線分の太さ
		///// @param color 色
		///// @return *this
		//const Line& draw(double thickness, const ColorF& color = Palette::White) const;

		///// @brief 線分を描きます。
		///// @param thickness 線分の太さ
		///// @param colorBegin 始点側の色
		///// @param colorEnd 終点側の色
		///// @return *this
		//const Line& draw(double thickness, const ColorF& colorBegin, const ColorF& colorEnd) const;

		///// @brief 線分を描きます。
		///// @param style 線のスタイル
		///// @param thickness 線分の太さ
		///// @param color 色
		///// @return *this
		//const Line& draw(const LineStyle& style, double thickness, const ColorF& color = Palette::White) const;

		///// @brief 線分を描きます。
		///// @param style 線のスタイル
		///// @param thickness 線分の太さ
		///// @param colorBegin 始点側の色
		///// @param colorEnd 終点側の色
		///// @return *this
		//const Line& draw(const LineStyle& style, double thickness, const ColorF& colorBegin, const ColorF& colorEnd) const;

		///// @brief 線分をもとに矢印を描きます。
		///// @param width 矢印の線の幅
		///// @param headSize 矢印の三角形のサイズ
		///// @param color 色
		///// @return *this
		//const Line& drawArrow(double width = 1.0, const SizeF& headSize = SizeF{ 5.0, 5.0 }, const ColorF& color = Palette::White) const;

		///// @brief 線分をもとに両方向矢印を描きます。
		///// @param width 矢印の線の幅
		///// @param headSize 矢印の三角形のサイズ
		///// @param color 色
		///// @return *this
		//const Line& drawDoubleHeadedArrow(double width = 1.0, const SizeF& headSize = SizeF{ 5.0, 5.0 }, const ColorF& color = Palette::White) const;


		template <class CharType>
		friend std::basic_ostream<CharType>& operator <<(std::basic_ostream<CharType>& output, const Line& value)
		{
			return output << CharType('(')
				<< value.start << CharType(',') << CharType(' ')
				<< value.end << CharType(')');
		}

		template <class CharType>
		friend std::basic_istream<CharType>& operator >>(std::basic_istream<CharType>& input, Line& value)
		{
			CharType unused;
			return input >> unused
				>> value.start >> unused
				>> value.end >> unused;
		}

		////////////////////////////////////////////////////////////////
		//
		//	Formatter
		//
		////////////////////////////////////////////////////////////////

		friend void Formatter(FormatData& formatData, const Line& value);

	private:

		[[noreturn]]
		static void ThrowPointAtIndexOutOfRange();
	};
}

////////////////////////////////////////////////////////////////
//
//	fmt
//
////////////////////////////////////////////////////////////////

template <>
struct fmt::formatter<s3d::Line>
{
	std::string tag;

	constexpr auto parse(format_parse_context& ctx)
	{
		return s3d::FmtHelper::GetFormatTag(tag, ctx);
	}

	format_context::iterator format(const s3d::Line& value, format_context& ctx);
};

template <>
struct fmt::formatter<s3d::Line, s3d::char32>
{
	std::u32string tag;

	s3d::ParseContext::iterator parse(s3d::ParseContext& ctx);

	s3d::BufferContext::iterator format(const s3d::Line& value, s3d::BufferContext& ctx);
};

////////////////////////////////////////////////////////////////
//
//	std::hash
//
////////////////////////////////////////////////////////////////

template <>
struct std::hash<s3d::Line>
{
	[[nodiscard]]
	size_t operator ()(const s3d::Line& value) const noexcept
	{
		return value.hash();
	}
};
