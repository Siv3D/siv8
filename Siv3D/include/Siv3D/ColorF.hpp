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
# include "FormatLiteral.hpp"

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

		/// @brief ColorF を作成します。
		/// @param _r R 成分
		/// @param _g G 成分
		/// @param _b B 成分
		/// @param _a アルファ値
		[[nodiscard]]
		constexpr ColorF(double _r, double _g, double _b, double _a = 1.0) noexcept;

		/// @brief ColorF を作成します。
		/// @param rgb RGB 成分
		/// @param _a アルファ値
		[[nodiscard]]
		explicit constexpr ColorF(double rgb, double _a = 1.0) noexcept;

		/// @brief 別の ColorF から ColorF を作成します。
		/// @param rgb 別の ColorF
		/// @param _a アルファ値
		/// @remark `ColorF{ rgb.r, rgb.g, rgb.b, _a }` と同じです。
		[[nodiscard]]
		constexpr ColorF(const ColorF& rgb, double _a) noexcept;

		/// @brief Vec3 から ColorF を作成します。
		/// @param rgb Vec3
		/// @param _a アルファ値
		/// @remark `ColorF{ rgb.x, rgb.y, rgb.z, _a }` と同じです。
		[[nodiscard]]
		explicit constexpr ColorF(const Vec3& rgb, double _a = 1.0) noexcept;

		/// @brief Vec4 から ColorF を作成します。
		/// @param rgba Vec4
		/// @remark `ColorF{ rgba.x, rgba.y, rgba.z, rgba.w }` と同じです。
		[[nodiscard]]
		explicit constexpr ColorF(const Vec4& rgba) noexcept;

		/// @brief Color から ColorF を作成します。
		/// @param color Color
		[[nodiscard]]
		constexpr ColorF(Color color) noexcept;

		/// @brief Color から ColorF を作成します。
		/// @param rgb Color
		/// @param _a アルファ値
		/// @remark 引数 `color` のアルファ値は無視されます。
		[[nodiscard]]
		constexpr ColorF(Color rgb, double _a) noexcept;

		/// @brief HSV から ColorF を作成します。
		/// @param hsva HSV
		[[nodiscard]]
		ColorF(const HSV& hsva) noexcept;

		/// @brief HSV から ColorF を作成します。
		/// @param hsv HSV
		/// @param _a アルファ値
		/// @remark 引数 `hsv` のアルファ値は無視されます。
		[[nodiscard]]
		ColorF(const HSV& hsv, double _a) noexcept;

		/// @brief 16 進数のカラーコードから ColorF を作成します。
		/// @param code 16 進数のカラーコード
		/// @remark #RGB, #RGBA, RRGGBB, #RRGGBB, RRGGBBAA, #RRGGBBAA に対応します。
		[[nodiscard]]
		explicit constexpr ColorF(StringView code) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	elem
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定したインデックスの成分を返します。
		/// @param index インデックス
		/// @return 指定したインデックスの成分。範囲外の場合は 0.0
		/// @remark 0 で R 成分、1 で G 成分、2 で B 成分、3 で アルファ値にアクセスできます。
		[[nodiscard]]
		constexpr double elem(size_t index) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	getPointer
		//
		////////////////////////////////////////////////////////////////

		/// @brief R 成分へのポインタを返します。
		/// @return R 成分へのポインタ
		/// @remark [0] で R 成分、[1] で G 成分、[2] で B 成分、[3] で アルファ値にアクセスできます。
		[[nodiscard]]
		constexpr double* getPointer() noexcept;

		/// @brief R 成分へのポインタを返します。
		/// @return R 成分へのポインタ
		/// @remark [0] で R 成分、[1] で G 成分、[2] で B 成分、[3] で アルファ値にアクセスできます。
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
		//	operator ~
		//
		////////////////////////////////////////////////////////////////

		/// @brief 色の各成分を反転した色を返します。
		/// @return 色の各成分を反転した色
		/// @remark `ColorF{ (1.0 - r), (1.0 - g), (1.0 - b), a }` を返します。
		[[nodiscard]]
		constexpr ColorF operator ~() const noexcept;

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

		/// @brief R 成分のみを変更した自身のコピーを返します。
		/// @param _r 新しい R 成分
		/// @return R 成分を変更したコピー
		[[nodiscard]]
		constexpr ColorF withR(double _r) const noexcept;

		/// @brief G 成分のみを変更した自身のコピーを返します。
		/// @param _g 新しい G 成分
		/// @return G 成分を変更したコピー
		[[nodiscard]]
		constexpr ColorF withG(double _g) const noexcept;

		/// @brief B 成分のみを変更した自身のコピーを返します。
		/// @param _b 新しい B 成分
		/// @return B 成分を変更したコピー
		[[nodiscard]]
		constexpr ColorF withB(double _b) const noexcept;

		/// @brief アルファ値のみを変更した自身のコピーを返します。
		/// @param _a 新しいアルファ値
		/// @return アルファ値を変更したコピー
		[[nodiscard]]
		constexpr ColorF withA(double _a) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	withAlpha
		//
		////////////////////////////////////////////////////////////////

		/// @brief アルファ値のみを変更した自身のコピーを返します。
		/// @param _a 新しいアルファ値
		/// @return アルファ値を変更したコピー
		/// @remark `.withA(_a)` と同じです。
		constexpr ColorF withAlpha(double _a) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	setR, setG, setB, setA
		//
		////////////////////////////////////////////////////////////////

		/// @brief R 成分を変更します。
		/// @param _r 新しい R 成分
		/// @return *this
		constexpr ColorF& setR(double _r) noexcept;

		/// @brief G 成分を変更します。
		/// @param _g 新しい G 成分
		/// @return *this
		constexpr ColorF& setG(double _g) noexcept;

		/// @brief B 成分を変更します。
		/// @param _b 新しい B 成分
		/// @return *this
		constexpr ColorF& setB(double _b) noexcept;

		/// @brief アルファ値を変更します。
		/// @param _a 新しいアルファ値
		/// @return *this
		constexpr ColorF& setA(double _a) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	setRGB
		//
		////////////////////////////////////////////////////////////////

		/// @brief RGB 成分を変更します。
		/// @param rgb 新しい RGB 成分
		/// @return *this
		constexpr ColorF& setRGB(double rgb) noexcept;

		/// @brief RGB 成分を変更します。
		/// @param _r 新しい R 成分
		/// @param _g 新しい G 成分
		/// @param _b 新しい B 成分
		/// @return *this
		constexpr ColorF& setRGB(double _r, double _g, double _b) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	set
		//
		////////////////////////////////////////////////////////////////

		/// @brief 色を変更します。
		/// @param rgb 新しい RGB 成分
		/// @param _a 新しいアルファ値
		/// @return *this
		constexpr ColorF& set(double rgb, double _a = 1.0) noexcept;

		/// @brief 色を変更します。
		/// @param _r 新しい R 成分
		/// @param _g 新しい G 成分
		/// @param _b 新しい B 成分
		/// @param _a 新しいアルファ値
		/// @return *this
		constexpr ColorF& set(double _r, double _g, double _b, double _a = 1.0) noexcept;

		/// @brief 色を変更します。
		/// @param color 新しい色
		/// @return *this
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

		/// @brief RGB 成分のうち最小の値を返します。
		/// @return RGB 成分のうち最小の値
		[[nodiscard]]
		constexpr double minRGBComponent() const noexcept;

		/// @brief RGB 成分のうち最大の値を返します。
		/// @return RGB 成分のうち最大の値
		[[nodiscard]]
		constexpr double maxRGBComponent() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	minComponent, maxComponent
		//
		////////////////////////////////////////////////////////////////

		/// @brief RGBA 成分のうち最小の値を返します。
		/// @return RGBA 成分のうち最小の値
		[[nodiscard]]
		constexpr double minComponent() const noexcept;

		/// @brief RGBA 成分のうち最大の値を返します。
		/// @return RGBA 成分のうち最大の値
		[[nodiscard]]
		constexpr double maxComponent() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	lerp
		//
		////////////////////////////////////////////////////////////////

		/// @brief 2 つの色の間を線形補間した色を返します。
		/// @param other もう一方の色
		/// @param f 補間係数
		/// @return 補間された色
		[[nodiscard]]
		constexpr ColorF lerp(const ColorF& other, double f) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	lightened, darkened
		//
		////////////////////////////////////////////////////////////////

		/// @brief 色を明るくした色を返します。
		/// @param amount 明るくする量 [0.0, 1.0]
		/// @return 明るくした色
		/// @remark `ColorF{ (r + (1.0 - r) * amount), (g + (1.0 - g) * amount), (b + (1.0 - b) * amount), a }` を返します。
		[[nodiscard]]
		constexpr ColorF lightened(double amount) const noexcept;

		/// @brief 色を暗くした色を返します。
		/// @param amount 暗くする量 [0.0, 1.0]
		/// @return 暗くした色
		/// @remark `ColorF{ (r * (1.0 - amount)), (g * (1.0 - amount)), (b * (1.0 - amount)), a }` を返します。
		[[nodiscard]]
		constexpr ColorF darkened(double amount) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	adjustHue
		//
		////////////////////////////////////////////////////////////////

		/// @brief 色相を調整した色を返します。
		/// @param amount 色相の調整量
		/// @return 色相を調整した色
		[[nodiscard]]
		ColorF adjustHue(double amount) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	inverted
		//
		////////////////////////////////////////////////////////////////

		/// @brief 反転した色を返します。
		/// @return 反転した色
		/// @remark `ColorF{ (1.0 - r), (1.0 - g), (1.0 - b), a }` を返します。
		/// @remark `operator ~` と同じです。
		[[nodiscard]]
		constexpr ColorF inverted() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	complemented
		//
		////////////////////////////////////////////////////////////////

		/// @brief 補色を返します。
		/// @return 補色
		[[nodiscard]]
		ColorF complemented() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	gamma
		//
		////////////////////////////////////////////////////////////////

		/// @brief 色をガンマ補正した色を返します。
		/// @param gamma ガンマ値
		/// @return ガンマ補正された色
		[[nodiscard]]
		ColorF gamma(double gamma) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	removeSRGBCurve, applySRGBCurve
		//
		////////////////////////////////////////////////////////////////

		/// @brief リニアカラースペースに変換した色を返します。
		/// @return リニアカラースペースに変換した色
		[[nodiscard]]
		ColorF removeSRGBCurve() const noexcept;

		/// @brief sRGB カーブを適用した色を返します。
		/// @return sRGB カーブを適用した色
		[[nodiscard]]
		ColorF applySRGBCurve() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	hash
		//
		////////////////////////////////////////////////////////////////

		/// @brief ハッシュ値を返します。
		/// @return ハッシュ値
		[[nodiscard]]
		uint64 hash() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	toColor
		//
		////////////////////////////////////////////////////////////////

		/// @brief 色を Color で返します。
		/// @return Color
		/// @remark 0.0 未満の成分は 0 に、1.0 より大きい成分は 255 になります。
		[[nodiscard]]
		constexpr Color toColor() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	toFloat4
		//
		////////////////////////////////////////////////////////////////

		/// @brief Float4{ r, g, b, a } を返します。
		/// @return Float4{ r, g, b, a }
		[[nodiscard]]
		constexpr Float4 toFloat4() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	toVec4
		//
		////////////////////////////////////////////////////////////////

		/// @brief Vec4{ r, g, b, a } を返します。
		/// @return Vec4{ r, g, b, a }
		[[nodiscard]]
		constexpr Vec4 toVec4() const noexcept;

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

		/// @brief Vec3{ r, g, b } を返します。
		/// @return Vec3{ r, g, b }
		[[nodiscard]]
		constexpr Vec3 rgb() const noexcept;

		/// @brief Vec3{ g, b, a } を返します。
		/// @return Vec3{ g, b, a }
		[[nodiscard]]
		constexpr Vec3 gba() const noexcept;

		/// @brief Vec3{ b, g, r } を返します。
		/// @return Vec3{ b, g, r }
		[[nodiscard]]
		constexpr Vec3 bgr() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	rgba, rgb0, rgb1, argb, abgr
		//
		////////////////////////////////////////////////////////////////

		/// @brief Vec4{ r, g, b, a } を返します。
		/// @remark `toVec4()` と同じです。
		/// @return Vec4{ r, g, b, a }
		[[nodiscard]]
		constexpr Vec4 rgba() const noexcept;

		/// @brief Vec4{ r, g, b, 0 } を返します。
		/// @return Vec4{ r, g, b, 0 }
		[[nodiscard]]
		constexpr Vec4 rgb0() const noexcept;

		/// @brief Vec4{ r, g, b, 1 } を返します。
		/// @return Vec4{ r, g, b, 1 }
		[[nodiscard]]
		constexpr Vec4 rgb1() const noexcept;

		/// @brief Vec4{ a, r, g, b } を返します。
		/// @return Vec4{ a, r, g, b }
		[[nodiscard]]
		constexpr Vec4 argb() const noexcept;

		/// @brief Vec4{ a, b, g, r } を返します。
		/// @return Vec4{ a, b, g, r }
		[[nodiscard]]
		constexpr Vec4 abgr() const noexcept;
	
		////////////////////////////////////////////////////////////////
		//
		//	PremultiplyAlpha
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		static constexpr ColorF PremultiplyAlpha(ColorF color) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	UnpremultiplyAlpha
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		static constexpr ColorF UnpremultiplyAlpha(ColorF color) noexcept;

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
			CharType unused{};
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

	/// @brief ColorF{ 1.0, 1.0, 1.0, alpha } を返します。
	/// @param alpha アルファ値
	/// @return ColorF{ 1.0, 1.0, 1.0, alpha }
	[[nodiscard]]
	constexpr ColorF AlphaF(double alpha) noexcept;
}

////////////////////////////////////////////////////////////////
//
//	fmt
//
////////////////////////////////////////////////////////////////

template <>
struct fmt::formatter<s3d::ColorF>
{
	std::string tag;

	constexpr auto parse(format_parse_context& ctx)
	{
		return s3d::FmtHelper::GetFormatTag(tag, ctx);
	}

	format_context::iterator format(const s3d::ColorF& value, format_context& ctx);
};

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
