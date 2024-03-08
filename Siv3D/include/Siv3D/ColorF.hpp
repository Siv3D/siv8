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
# include "FormatData.hpp"
# include "PointVector.hpp"
# include "FmtExtension.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	ColorF
	//
	////////////////////////////////////////////////////////////////

	/// @brief 色を RGBA 各要素について浮動小数点数で表現するクラスです。
	/// @remark 各要素は通常 0.0 以上 1.0 以下の値を持ちます。
	struct ColorF
	{
		/// @brief 赤 | Red
		double r;

		/// @brief 緑 | Green
		double g;

		/// @brief 青 | Blue
		double b;

		/// @brief アルファ | Alpha
		double a;

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		/// @brief デフォルトコンストラクタ
		[[nodiscard]]
		ColorF() = default;

		/// @brief コピーコンストラクタ
		[[nodiscard]]
		ColorF(const ColorF&) = default;

		/// @brief 色を作成します。
		/// @param _r 赤成分
		/// @param _g 緑成分
		/// @param _b 青成分
		/// @param _a アルファ成分
		[[nodiscard]]
		constexpr ColorF(double _r, double _g, double _b, double _a = 1.0) noexcept;

		[[nodiscard]]
		explicit constexpr ColorF(double rgb, double _a = 1.0) noexcept;

		[[nodiscard]]
		constexpr ColorF(const ColorF& rgb, double _a) noexcept;

		//[[nodiscard]]
		//explicit constexpr ColorF(const Vec3& rgb, double _a = 1.0) noexcept;

		//[[nodiscard]]
		//explicit constexpr ColorF(const Vec4& rgba) noexcept;

		[[nodiscard]]
		constexpr ColorF(Color color) noexcept;

		[[nodiscard]]
		constexpr ColorF(Color rgb, double _a) noexcept;

		[[nodiscard]]
		ColorF(const HSV& hsva) noexcept;

		[[nodiscard]]
		ColorF(const HSV& hsv, double _a) noexcept;

		[[nodiscard]]
		explicit constexpr ColorF(StringView code) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	elem
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr double elem(size_t index) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	getPointer
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr double* getPointer() noexcept;

		[[nodiscard]]
		constexpr const double* getPointer() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator =
		//
		////////////////////////////////////////////////////////////////

		constexpr ColorF& operator =(const ColorF&) noexcept = default;

		constexpr ColorF& operator =(Color color) noexcept;

		ColorF& operator =(const HSV& hsva) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator +
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr ColorF operator +(const ColorF& rgb) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator +=
		//
		////////////////////////////////////////////////////////////////

		constexpr ColorF& operator +=(const ColorF& rgb) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator -
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr ColorF operator -(const ColorF& rgb) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator -=
		//
		////////////////////////////////////////////////////////////////

		constexpr ColorF& operator -=(const ColorF& rgb) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator *
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr ColorF operator *(double s) const noexcept;

		[[nodiscard]]
		constexpr ColorF operator *(const ColorF& rgba) const noexcept;

		[[nodiscard]]
		friend constexpr ColorF operator *(double s, const ColorF& color) noexcept
		{
			return{ (s * color.r), (s * color.g), (s * color.b), (color.a) };
		}

		////////////////////////////////////////////////////////////////
		//
		//	operator *=
		//
		////////////////////////////////////////////////////////////////

		constexpr ColorF& operator *=(double s) noexcept;

		constexpr ColorF& operator *=(const ColorF& rgba) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator ==
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		friend constexpr bool operator ==(const ColorF& lhs, const ColorF& rhs) noexcept
		{
			return ((lhs.r == rhs.r)
				&&  (lhs.g == rhs.g)
				&&  (lhs.b == rhs.b)
				&&  (lhs.a == rhs.a));
		}

		////////////////////////////////////////////////////////////////
		//
		//	withR, withG, withB, withA
		//
		////////////////////////////////////////////////////////////////

		/// @brief r 成分のみを変更した自身のコピーを返します。
		/// @param _r r 成分
		/// @return r 成分を変更したコピー
		[[nodiscard]]
		constexpr ColorF withR(double _r) const noexcept;

		/// @brief g 成分のみを変更した自身のコピーを返します。
		/// @param _g g 成分
		/// @return g 成分を変更したコピー
		[[nodiscard]]
		constexpr ColorF withG(double _g) const noexcept;

		/// @brief b 成分のみを変更した自身のコピーを返します。
		/// @param _b b 成分
		/// @return b 成分を変更したコピー
		[[nodiscard]]
		constexpr ColorF withB(double _b) const noexcept;

		/// @brief a 成分のみを変更した自身のコピーを返します。
		/// @param _a a 成分
		/// @return a 成分を変更したコピー
		[[nodiscard]]
		constexpr ColorF withA(double _a) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	withAlpha
		//
		////////////////////////////////////////////////////////////////

		constexpr ColorF withAlpha(double _a) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	setR, setG, setB, setA
		//
		////////////////////////////////////////////////////////////////

		constexpr ColorF& setR(double _r) noexcept;

		constexpr ColorF& setG(double _g) noexcept;

		constexpr ColorF& setB(double _b) noexcept;

		constexpr ColorF& setA(double _a) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	setRGB
		//
		////////////////////////////////////////////////////////////////

		constexpr ColorF& setRGB(double rgb) noexcept;

		constexpr ColorF& setRGB(double _r, double _g, double _b) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	set
		//
		////////////////////////////////////////////////////////////////

		constexpr ColorF& set(double rgb, double _a = 1.0) noexcept;

		constexpr ColorF& set(double _r, double _g, double _b, double _a = 1.0) noexcept;

		constexpr ColorF& set(const ColorF& ColorF) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	grayscale
		//
		////////////////////////////////////////////////////////////////

		/// @brief グレースケール値を返します。
		/// @remark グレースケール値は、`(0.299 * r) + (0.587 * g) + (0.114 * b)` で計算されます。
		/// @return グレースケール値
		[[nodiscard]]
		constexpr double grayscale() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	minRGBComponent, maxRGBComponent
		//
		////////////////////////////////////////////////////////////////

		/// @brief RGB 各成分の最小値を返します。
		/// @return RGB 各成分の最小値
		[[nodiscard]]
		constexpr double minRGBComponent() const noexcept;

		/// @brief RGB 各成分の最大値を返します。
		/// @return RGB 各成分の最大値
		[[nodiscard]]
		constexpr double maxRGBComponent() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	minComponent, maxComponent
		//
		////////////////////////////////////////////////////////////////

		/// @brief RGBA 各成分の最小値を返します。
		/// @return RGBA 各成分の最小値
		[[nodiscard]]
		constexpr double minComponent() const noexcept;

		/// @brief RGBA 各成分の最大値を返します。
		/// @return RGBA 各成分の最大値
		[[nodiscard]]
		constexpr double maxComponent() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	lerp
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr ColorF lerp(const ColorF& other, double f) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	lightened, darkened
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr ColorF lightened(double amount) const noexcept;

		[[nodiscard]]
		constexpr ColorF darkened(double amount) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	gamma
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		ColorF gamma(double gamma) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	removeSRGBCurve, applySRGBCurve
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		ColorF removeSRGBCurve() const noexcept;

		[[nodiscard]]
		ColorF applySRGBCurve() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	hash
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		uint64 hash() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	toColor
		//
		////////////////////////////////////////////////////////////////

		/// @brief 色を Color で返します。
		/// @remark 各成分は 0.0～1.0 の範囲にクランプされます。
		/// @return 変換された Color
		[[nodiscard]]
		constexpr Color toColor() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	toFloat4
		//
		////////////////////////////////////////////////////////////////

		///// @brief Float4{ r, g, b, a } を返します。
		///// @return Float4{ r, g, b, a }
		//[[nodiscard]]
		//constexpr Float4 toFloat4() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	toVec4
		//
		////////////////////////////////////////////////////////////////

		///// @brief Vec4{ r, g, b, a } を返します。
		///// @return Vec4{ r, g, b, a }
		//[[nodiscard]]
		//constexpr Vec4 toVec4() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	rg, gb, ba
		//
		////////////////////////////////////////////////////////////////

		/// @brief Vec2{ r, g } を返します。
		/// @return Vec2{ r, g }
		[[nodiscard]]
		constexpr Vec2 rg() const noexcept;

		/// @brief Vec2{ g, b } を返します。
		/// @return Vec2{ g, b }
		[[nodiscard]]
		constexpr Vec2 gb() const noexcept;

		/// @brief Vec2{ b, a } を返します。
		/// @return Vec2{ b, a }
		[[nodiscard]]
		constexpr Vec2 ba() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	rgb, gba, bgr
		//
		////////////////////////////////////////////////////////////////

		///// @brief Vec3{ r, g, b } を返します。
		///// @return Vec3{ r, g, b }
		//[[nodiscard]]
		//constexpr Vec3 rgb() const noexcept;

		///// @brief Vec3{ g, b, a } を返します。
		///// @return Vec3{ g, b, a }
		//[[nodiscard]]
		//constexpr Vec3 gba() const noexcept;

		///// @brief Vec3{ b, g, r } を返します。
		///// @return Vec3{ b, g, r }
		//[[nodiscard]]
		//constexpr Vec3 bgr() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	rgba, rgb0, rgb1, argb, abgr
		//
		////////////////////////////////////////////////////////////////

		///// @brief Vec4{ r, g, b, a } を返します。
		///// @remark `toVec4()` と同じです。
		///// @return Vec4{ r, g, b, a }
		//[[nodiscard]]
		//constexpr Vec4 rgba() const noexcept;

		///// @brief Vec4{ r, g, b, 0 } を返します。
		///// @return Vec4{ r, g, b, 0 }
		//[[nodiscard]]
		//constexpr Vec4 rgb0() const noexcept;

		///// @brief Vec4{ r, g, b, 1 } を返します。
		///// @return Vec4{ r, g, b, 1 }
		//[[nodiscard]]
		//constexpr Vec4 rgb1() const noexcept;

		///// @brief Vec4{ a, r, g, b } を返します。
		///// @return Vec4{ a, r, g, b }
		//[[nodiscard]]
		//constexpr Vec4 argb() const noexcept;

		///// @brief Vec4{ a, b, g, r } を返します。
		///// @return Vec4{ a, b, g, r }
		//[[nodiscard]]
		//constexpr Vec4 abgr() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator <<
		//
		////////////////////////////////////////////////////////////////

		template <class CharType>
		friend std::basic_ostream<CharType>& operator <<(std::basic_ostream<CharType>& output, const ColorF& value)
		{
			return output << CharType('(')
				<< value.r << CharType(',') << CharType(' ')
				<< value.g << CharType(',') << CharType(' ')
				<< value.b << CharType(',') << CharType(' ')
				<< value.a << CharType(')');
		}

		////////////////////////////////////////////////////////////////
		//
		//	operator >>
		//
		////////////////////////////////////////////////////////////////

		template <class CharType>
		friend std::basic_istream<CharType>& operator >>(std::basic_istream<CharType>& input, ColorF& value)
		{
			CharType unused;
			input >> unused;

			if (unused == CharType('#'))
			{
				String code;
				input >> code;
				value = ColorF(U'#' + code);
			}
			else
			{
				input >> value.r >> unused
					>> value.g >> unused
					>> value.b >> unused;

				if (unused == CharType(','))
				{
					input >> value.a >> unused;
				}
				else
				{
					value.a = 1.0;
				}
			}

			return input;
		}

		////////////////////////////////////////////////////////////////
		//
		//	Zero
		//
		////////////////////////////////////////////////////////////////

		/// @brief ColorF{ 0, 0, 0, 0 } を返します。
		/// @return ColorF{ 0, 0, 0, 0 }
		[[nodiscard]]
		static constexpr ColorF Zero() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	One
		//
		////////////////////////////////////////////////////////////////

		/// @brief ColorF{ 1, 1, 1, 1 } を返します。
		/// @return ColorF{ 1, 1, 1, 1 }
		[[nodiscard]]
		static constexpr ColorF One() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	Formatter
		//
		////////////////////////////////////////////////////////////////

		friend void Formatter(FormatData& formatData, const ColorF& value);
	};

	////////////////////////////////////////////////////////////////
	//
	//	AlphaF
	//
	////////////////////////////////////////////////////////////////

	[[nodiscard]]
	constexpr ColorF AlphaF(double alpha) noexcept;
}

////////////////////////////////////////////////////////////////
//
//	fmt
//
////////////////////////////////////////////////////////////////

template <>
struct fmt::formatter<s3d::ColorF, s3d::char32>
{
	std::u32string tag;

	s3d::ParseContext::iterator parse(s3d::ParseContext& ctx);

	s3d::BufferContext::iterator format(const s3d::ColorF& value, s3d::BufferContext& ctx);
};

////////////////////////////////////////////////////////////////
//
//	std::hash
//
////////////////////////////////////////////////////////////////

template <>
struct std::hash<s3d::ColorF>
{
	[[nodiscard]]
	size_t operator ()(const s3d::ColorF& value) const noexcept
	{
		return value.hash();
	}
};
