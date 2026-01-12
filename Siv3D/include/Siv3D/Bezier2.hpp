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
# include "Common.hpp"
# include "PointVector.hpp"
# include "LineString.hpp"

namespace s3d
{
	/// @brief 二次ベジェ曲線
	struct Bezier2
	{
		using position_type = Vec2;

		using value_type = position_type::value_type;

		position_type p0;

		position_type p1;

		position_type p2;

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Bezier2() = default;

		[[nodiscard]]
		constexpr Bezier2(const position_type& _p0, const position_type& _p1, const position_type& _p2) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	withP0
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr Bezier2 withP0(value_type _x, value_type _y) const noexcept;

		[[nodiscard]]
		constexpr Bezier2 withP0(position_type _p0) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	withP1
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr Bezier2 withP1(value_type _x, value_type _y) const noexcept;
		
		[[nodiscard]]
		constexpr Bezier2 withP1(position_type _p1) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	withP2
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr Bezier2 withP2(value_type _x, value_type _y) const noexcept;
		
		[[nodiscard]]
		constexpr Bezier2 withP2(position_type _p2) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	withOffset
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr Bezier2 withOffset(value_type x, value_type y) const noexcept;

		[[nodiscard]]
		constexpr Bezier2 withOffset(position_type v) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	withOffsetX, withOffsetY
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr Bezier2 withOffsetX(value_type x) const noexcept;

		[[nodiscard]]
		constexpr Bezier2 withOffsetY(value_type y) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	pointAtIndex
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		position_type& pointAtIndex(size_t index);

		[[nodiscard]]
		const position_type& pointAtIndex(size_t index) const;

		////////////////////////////////////////////////////////////////
		//
		//	getPos
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr position_type getPos(const double t) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	getDerivative
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr Vec2 getDerivative(double t) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	getSecondDerivative
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr Vec2 getSecondDerivative() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	getUnitTangent
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Vec2 getUnitTangent(double t) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	computeLength
		//
		////////////////////////////////////////////////////////////////

		/// @brief 曲線の長さを計算します。
		/// @return 曲線の長さ
		[[nodiscard]]
		double computeLength() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	tAtLength
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した長さになる位置のパラメータ t を計算します。
		/// @param length 長さ
		/// @return パラメータ t（0.0 ～ 1.0）
		[[nodiscard]]
		double tAtLength(double length) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	getPosAtLength
		//
		////////////////////////////////////////////////////////////////

		/// @brief 始点からの長さが length になる位置の座標を計算します。
		/// @param length 長さ
		/// @return 座標
		[[nodiscard]]
		position_type getPosAtLength(double length) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	getLineString
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		LineString getLineString(const int32 segments = 24) const;

		////////////////////////////////////////////////////////////////
		//
		//	getLineStringAdaptive
		//
		////////////////////////////////////////////////////////////////

		LineString getLineStringAdaptive(double maxError = 0.48, int32 maxDepth = 5) const;

		////////////////////////////////////////////////////////////////
		//
		//	split
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr std::pair<Bezier2, Bezier2> split(double t) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	subcurve
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Bezier2 subcurve(double t0, double t1) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	movedBy
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr Bezier2 movedBy(value_type x, value_type y) const noexcept;

		[[nodiscard]]
		constexpr Bezier2 movedBy(position_type v) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	moveBy
		//
		////////////////////////////////////////////////////////////////

		constexpr Bezier2& moveBy(value_type x, value_type y) noexcept;

		constexpr Bezier2& moveBy(position_type v) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	reverse, reversed
		//
		////////////////////////////////////////////////////////////////

		constexpr Bezier2& reverse() noexcept;

		[[nodiscard]]
		constexpr Bezier2 reversed() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	boundingRect
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		RectF boundingRect() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	p0p1, p1p2, p0p2
		//
		////////////////////////////////////////////////////////////////

		/// @brief p0 から p1 への線分を返します。
		/// @return p0 から p1 への線分
		[[nodiscard]]
		constexpr Line p0p1() const noexcept;

		/// @brief p1 から p2 への線分を返します。
		/// @return p1 から p2 への線分
		[[nodiscard]]
		constexpr Line p1p2() const noexcept;

		/// @brief p0 から p2 への線分を返します。
		/// @return p0 から p2 への線分
		[[nodiscard]]
		constexpr Line p0p2() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	p1p0, p2p1, p2p0
		//
		////////////////////////////////////////////////////////////////

		/// @brief p1 から p0 への線分を返します。
		/// @return p1 から p0 への線分
		[[nodiscard]]
		constexpr Line p1p0() const noexcept;

		/// @brief p2 から p1 への線分を返します。
		/// @return p2 から p1 への線分
		[[nodiscard]]
		constexpr Line p2p1() const noexcept;

		/// @brief p2 から p0 への線分を返します。
		/// @return p2 から p0 への線分
		[[nodiscard]]
		constexpr Line p2p0() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	lerp
		//
		////////////////////////////////////////////////////////////////

		/// @brief 別の二次ベジェ曲線との線形補間を計算します。
		/// @param other 別の二次ベジェ曲線
		/// @param f 補間係数
		/// @return 線形補間された二次ベジェ曲線
		[[nodiscard]]
		constexpr Bezier2 lerp(const Bezier2& other, double f) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	toBezier3
		//
		////////////////////////////////////////////////////////////////

		/// @brief 二次ベジェ曲線を三次ベジェ曲線に変換します。
		/// @return 三次ベジェ曲線
		[[nodiscard]]
		constexpr Bezier3 toBezier3() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	hash
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		uint64 hash() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	intersects
		//
		////////////////////////////////////////////////////////////////

		//template <class Shape2DType>
		//[[nodiscard]]
		//constexpr bool intersects(const Shape2DType& other) const;

		////////////////////////////////////////////////////////////////
		//
		//	intersectsAt
		//
		////////////////////////////////////////////////////////////////

		//template <class Shape2DType>
		//[[nodiscard]]
		//Optional<Array<Vec2>> intersectsAt(const Shape2DType& other) const;

		////////////////////////////////////////////////////////////////
		//
		//	draw
		//
		////////////////////////////////////////////////////////////////

		const Bezier2& draw(const ColorF& color = Palette::White, int32 segments = 24) const;

		const Bezier2& draw(double thickness, const ColorF& color = Palette::White, int32 segments = 24) const;

		const Bezier2& draw(LineCap lineCap, double thickness, const ColorF& color = Palette::White, int32 segments = 24) const;

		////////////////////////////////////////////////////////////////
		//
		//	drawAdaptive
		//
		////////////////////////////////////////////////////////////////

		const Bezier2& drawAdaptive(const ColorF& color = Palette::White, double maxError = 0.48, int32 maxDepth = 5) const;

		const Bezier2& drawAdaptive(double thickness, const ColorF& color = Palette::White, double maxError = 0.48, int32 maxDepth = 5) const;

		const Bezier2& drawAdaptive(LineCap lineCap, double thickness, const ColorF& color = Palette::White, double maxError = 0.48, int32 maxDepth = 5) const;

		////////////////////////////////////////////////////////////////
		//
		//	FromMidpoint
		//
		////////////////////////////////////////////////////////////////

		/// @brief t = 0.5 で指定した中間点を通る二次ベジェ曲線を作成します。
		/// @param p0 開始点
		/// @param midpoint t = 0.5 の中間点
		/// @param p2 終了点
		/// @return 二次ベジェ曲線
		[[nodiscard]]
		static constexpr Bezier2 FromMidpoint(const position_type& p0, const position_type& midpoint, const position_type& p2) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator <<
		//
		////////////////////////////////////////////////////////////////

		/// @brief 出力ストリームに Bezier2 の内容を出力します。
		/// @tparam CharType 出力ストリームの文字型
		/// @param output 出力ストリーム
		/// @param value Bezier2
		/// @return 出力ストリーム
		template <class CharType>
		friend std::basic_ostream<CharType>& operator <<(std::basic_ostream<CharType>& output, const Bezier2& value)
		{
			return output << CharType('(')
				<< value.p0 << CharType(',') << CharType(' ')
				<< value.p1 << CharType(',') << CharType(' ')
				<< value.p2 << CharType(')');
		}

		////////////////////////////////////////////////////////////////
		//
		//	operator >>
		//
		////////////////////////////////////////////////////////////////

		/// @brief 入力ストリームから Bezier2 の内容を読み込みます。
		/// @tparam CharType 入力ストリームの文字型
		/// @param input 入力ストリーム
		/// @param value Bezier2 の格納先
		/// @return 入力ストリーム
		template <class CharType>
		friend std::basic_istream<CharType>& operator >>(std::basic_istream<CharType>& input, Bezier2& value)
		{
			CharType unused;
			return input >> unused
				>> value.p0 >> unused
				>> value.p1 >> unused
				>> value.p2 >> unused;
		}

		////////////////////////////////////////////////////////////////
		//
		//	Formatter
		//
		////////////////////////////////////////////////////////////////

		/// @brief Bezier2 を文字列に変換します。
		/// @param formatData 文字列バッファ
		/// @param value Bezier2
		/// @remark この関数は Format 用の関数です。通常、ユーザーが直接呼び出す必要はありません。
		friend void Formatter(FormatData& formatData, const Bezier2& value);

	private:

		double speed(double t) const noexcept;

		double integrateSpeed(double a, double b) const noexcept;

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
struct fmt::formatter<s3d::Bezier2>
{
	std::string tag;

	constexpr auto parse(format_parse_context& ctx)
	{
		return s3d::FmtHelper::GetFormatTag(tag, ctx);
	}

	format_context::iterator format(const s3d::Bezier2& value, format_context& ctx);
};

template <>
struct fmt::formatter<s3d::Bezier2, s3d::char32>
{
	std::u32string tag;

	s3d::ParseContext::iterator parse(s3d::ParseContext& ctx);

	s3d::BufferContext::iterator format(const s3d::Bezier2& value, s3d::BufferContext& ctx);
};

////////////////////////////////////////////////////////////////
//
//	std::hash
//
////////////////////////////////////////////////////////////////

template <>
struct std::hash<s3d::Bezier2>
{
	[[nodiscard]]
	size_t operator ()(const s3d::Bezier2& value) const noexcept
	{
		return value.hash();
	}
};
