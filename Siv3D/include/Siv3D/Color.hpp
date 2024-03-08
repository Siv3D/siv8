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
# include <bit>
# include "Common.hpp"
# include "Utility.hpp"
# include "StringView.hpp"
# include "Char.hpp"
# include "FmtExtension.hpp"

namespace s3d
{
	struct ColorF;
	struct HSV;

	////////////////////////////////////////////////////////////////
	//
	//	Color
	//
	////////////////////////////////////////////////////////////////

	struct Color
	{
		using value_type = uint8;

		/// @brief 赤 | Red
		value_type r;

		/// @brief 緑 | Green
		value_type g;

		/// @brief 青 | Blue
		value_type b;

		/// @brief アルファ | Alpha
		value_type a;

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		/// @brief デフォルトコンストラクタ
		[[nodiscard]]
		Color() = default;

		/// @brief コピーコンストラクタ
		[[nodiscard]]
		Color(const Color&) = default;

		/// @brief Color を作成します。
		/// @param _r 赤成分 [0, 255]
		/// @param _g 緑成分 [0, 255]
		/// @param _b 青成分 [0, 255]
		/// @param _a アルファ値 [0, 255]
		[[nodiscard]]
		constexpr Color(value_type _r, value_type _g, value_type _b, value_type _a = 255) noexcept;

		/// @brief Color を作成します。
		/// @param rgb RGB 成分 [0, 255]
		/// @param _a アルファ値 [0, 255]
		[[nodiscard]]
		explicit constexpr Color(value_type rgb, value_type _a = 255) noexcept;

		/// @brief 別の Color から Color を作成します。
		/// @param rgb Color
		/// @param _a アルファ値 [0, 255]
		/// @remark 引数 `rgb` のアルファ値は無視されます。
		[[nodiscard]]
		constexpr Color(Color rgb, value_type _a) noexcept;

		/// @brief ColorF から Color を作成します。
		/// @param color ColorF
		[[nodiscard]]
		constexpr Color(const ColorF& color) noexcept;

		/// @brief ColorF から Color を作成します。
		/// @param color ColorF
		/// @param _a アルファ値 [0, 255]
		/// @remark 引数 `color` のアルファ値は無視されます。
		[[nodiscard]]
		constexpr Color(const ColorF& color, value_type _a) noexcept;

		/// @brief HSV から Color を作成します。
		/// @param hsva HSV
		[[nodiscard]]
		Color(const HSV& hsva) noexcept;

		/// @brief HSV から Color を作成します。
		/// @param hsv HSV
		/// @param _a アルファ値 [0, 255]
		/// @remark 引数 `hsv` のアルファ値は無視されます。
		[[nodiscard]]
		Color(const HSV& hsv, value_type _a) noexcept;

		/// @brief 16 進数のカラーコードから Color を作成します。
		/// @param code 16 進数のカラーコード
		/// @remark #RGB, #RGBA, RRGGBB, #RRGGBB, RRGGBBAA, #RRGGBBAA に対応します。
		[[nodiscard]]
		explicit constexpr Color(StringView code) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator =
		//
		////////////////////////////////////////////////////////////////

		constexpr Color& operator =(const Color&) noexcept = default;

		constexpr Color& operator =(const ColorF& color) noexcept;

		Color& operator =(const HSV& hsva) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator ~
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr Color operator ~() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator ==
		//
		////////////////////////////////////////////////////////////////

		/// @brief 2 つの色が等しいかを返します。
		/// @param lhs 比較する色
		/// @param rhs 比較する色
		/// @return 2 つの色が等しい場合 true, それ以外の場合は false
		[[nodiscard]]
		friend constexpr bool operator ==(Color lhs, Color rhs) noexcept
		{
			return (std::bit_cast<uint32>(lhs) == std::bit_cast<uint32>(rhs));
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
		constexpr Color withR(uint32 _r) const noexcept;

		/// @brief g 成分のみを変更した自身のコピーを返します。
		/// @param _g g 成分
		/// @return g 成分を変更したコピー
		[[nodiscard]]
		constexpr Color withG(uint32 _g) const noexcept;

		/// @brief b 成分のみを変更した自身のコピーを返します。
		/// @param _b b 成分
		/// @return b 成分を変更したコピー
		[[nodiscard]]
		constexpr Color withB(uint32 _b) const noexcept;

		/// @brief a 成分のみを変更した自身のコピーを返します。
		/// @param _a a 成分
		/// @return a 成分を変更したコピー
		[[nodiscard]]
		constexpr Color withA(uint32 _a) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	withAlpha
		//
		////////////////////////////////////////////////////////////////

		/// @brief a 成分のみを変更した自身のコピーを返します。
		/// @param _a a 成分
		/// @return a 成分を変更したコピー
		/// @remark `.withA(_a)` と同じです。
		constexpr Color withAlpha(uint32 _a) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	setR, setG, setB, setA
		//
		////////////////////////////////////////////////////////////////

		/// @brief R 成分を変更します。
		/// @param _r 新しい R 成分
		/// @return *this
		constexpr Color& setR(uint32 _r) noexcept;

		/// @brief G 成分を変更します。
		/// @param _g 新しい G 成分
		/// @return *this
		constexpr Color& setG(uint32 _g) noexcept;

		/// @brief B 成分を変更します。
		/// @param _b 新しい B 成分
		/// @return *this
		constexpr Color& setB(uint32 _b) noexcept;

		/// @brief アルファ値を変更します。
		/// @param _a 新しいアルファ値
		/// @return *this
		constexpr Color& setA(uint32 _a) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	setRGB
		//
		////////////////////////////////////////////////////////////////

		constexpr Color& setRGB(uint32 rgb) noexcept;

		constexpr Color& setRGB(uint32 _r, uint32 _g, uint32 _b) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	set
		//
		////////////////////////////////////////////////////////////////

		constexpr Color& set(uint32 rgb, uint32 _a = 255) noexcept;

		constexpr Color& set(uint32 _r, uint32 _g, uint32 _b, uint32 _a = 255) noexcept;

		constexpr Color& set(Color color) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	grayscale0_255
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr uint8 grayscale0_255() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	grayscale
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr double grayscale() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	minRGBComponent, maxRGBComponent
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr uint8 minRGBComponent() const noexcept;

		[[nodiscard]]
		constexpr uint8 maxRGBComponent() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	minComponent, maxComponent
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr uint8 minComponent() const noexcept;

		[[nodiscard]]
		constexpr uint8 maxComponent() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	asUint32
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr uint32 asUint32() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	lerp
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr Color lerp(Color other, double f) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	lightened, darkened
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr Color lightened(double amount) const noexcept;

		[[nodiscard]]
		constexpr Color darkened(double amount) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	gamma
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Color gamma(double gamma) const noexcept;

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
		//	toHex
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		String toHex() const;

		////////////////////////////////////////////////////////////////
		//
		//	hash
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		uint64 hash() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	Zero
		//
		////////////////////////////////////////////////////////////////

		/// @brief Color{ 0, 0, 0, 0 } を返します。
		/// @return Color{ 0, 0, 0, 0 }
		[[nodiscard]]
		static constexpr Color Zero() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	ToUint8
		//
		////////////////////////////////////////////////////////////////

		/// @brief 浮動小数点数を [0.0, 1.0] にクランプしたあと [0, 255] の範囲の整数に変換します。
		/// @param x 浮動小数点数
		/// @return [0, 255] の範囲にクランプされた整数
		[[nodiscard]]
		static constexpr uint8 ToUint8(float x) noexcept;

		/// @brief 浮動小数点数を [0.0, 1.0] にクランプしたあと [0, 255] の範囲の整数に変換します。
		/// @param x 浮動小数点数
		/// @return [0, 255] の範囲にクランプされた整数
		[[nodiscard]]
		static constexpr uint8 ToUint8(double x) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	FromRGBA
		//
		////////////////////////////////////////////////////////////////

		/// @brief 32 ビット整数に格納された RGBA を Color に変換します。
		/// @param rgba 32 ビット整数に格納された RGBA
		/// @return 変換された Color
		[[nodiscard]]
		static constexpr Color FromRGBA(uint32 rgba) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	FromABGR
		//
		////////////////////////////////////////////////////////////////

		/// @brief 32 ビット整数に格納された ABGR を Color に変換します。
		/// @param abgr 32 ビット整数に格納された ABGR
		/// @return 変換された Color
		[[nodiscard]]
		static constexpr Color FromABGR(uint32 abgr) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator <<
		//
		////////////////////////////////////////////////////////////////

		template <class CharType>
		friend std::basic_ostream<CharType>& operator <<(std::basic_ostream<CharType>& output, const Color& value)
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
		friend std::basic_istream<CharType>& operator >>(std::basic_istream<CharType>& input, Color& value)
		{
			CharType unused;
			input >> unused;

			if (unused == CharType('#'))
			{
				String code;
				input >> code;
				value = Color(U'#' + code);
			}
			else
			{
				uint32 cols[4];
				input
					>> cols[0] >> unused
					>> cols[1] >> unused
					>> cols[2] >> unused;

				if (unused == CharType(','))
				{
					input >> cols[3] >> unused;
				}
				else
				{
					cols[3] = 255;
				}

				value.r = static_cast<uint8>(cols[0]);
				value.g = static_cast<uint8>(cols[1]);
				value.b = static_cast<uint8>(cols[2]);
				value.a = static_cast<uint8>(cols[3]);
			}

			return input;
		}

		////////////////////////////////////////////////////////////////
		//
		//	Formatter
		//
		////////////////////////////////////////////////////////////////

		friend void Formatter(FormatData& formatData, const Color& value);
	};

	////////////////////////////////////////////////////////////////
	//
	//	Alpha
	//
	////////////////////////////////////////////////////////////////

	/// @brief Color{ 255, 255, 255, alpha } を返します。
	/// @param alpha アルファ値
	/// @return Color{ 255, 255, 255, alpha }
	[[nodiscard]]
	constexpr Color Alpha(uint32 alpha) noexcept;
}

////////////////////////////////////////////////////////////////
//
//	fmt
//
////////////////////////////////////////////////////////////////

template <>
struct fmt::formatter<s3d::Color, s3d::char32>
{
	std::u32string tag;

	s3d::ParseContext::iterator parse(s3d::ParseContext& ctx);

	s3d::BufferContext::iterator format(const s3d::Color& value, s3d::BufferContext& ctx);
};

////////////////////////////////////////////////////////////////
//
//	std::hash
//
////////////////////////////////////////////////////////////////

template <>
struct std::hash<s3d::Color>
{
	[[nodiscard]]
	size_t operator ()(const s3d::Color& value) const noexcept
	{
		return value.hash();
	}
};
