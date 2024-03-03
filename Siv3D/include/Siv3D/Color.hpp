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

namespace s3d
{
	struct ColorF;
	struct HSV;

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

		/// @brief デフォルトコンストラクタ
		[[nodiscard]]
		Color() = default;

		/// @brief コピーコンストラクタ
		[[nodiscard]]
		Color(const Color&) = default;

		[[nodiscard]]
		constexpr Color(value_type _r, value_type _g, value_type _b, value_type _a = 255) noexcept;

		[[nodiscard]]
		explicit constexpr Color(value_type rgb, value_type _a = 255) noexcept;

		[[nodiscard]]
		constexpr Color(Color rgb, value_type _a) noexcept;

		[[nodiscard]]
		constexpr Color(const ColorF& color) noexcept;

		[[nodiscard]]
		constexpr Color(const ColorF& color, value_type _a) noexcept;

		[[nodiscard]]
		Color(const HSV& hsva) noexcept;

		[[nodiscard]]
		Color(const HSV& hsv, value_type _a) noexcept;

		[[nodiscard]]
		explicit constexpr Color(StringView code) noexcept;

		constexpr Color& operator =(const Color&) noexcept = default;

		constexpr Color& operator =(const ColorF& color) noexcept;

		constexpr Color& operator =(const HSV& hsva) noexcept;

		[[nodiscard]]
		constexpr Color operator ~() const noexcept;

		/// @brief 2 つの色が等しいかを返します。
		/// @param lhs 比較する色
		/// @param rhs 比較する色
		/// @return 2 つの色が等しい場合 true, それ以外の場合は false
		[[nodiscard]]
		friend constexpr bool operator ==(Color lhs, Color rhs) noexcept
		{
			return (std::bit_cast<uint32>(lhs) == std::bit_cast<uint32>(rhs));
		}

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

		constexpr Color withAlpha(uint32 _a) const noexcept;

		constexpr Color& setR(uint32 _r) noexcept;

		constexpr Color& setG(uint32 _g) noexcept;

		constexpr Color& setB(uint32 _b) noexcept;

		constexpr Color& setA(uint32 _a) noexcept;

		constexpr Color& setRGB(uint32 rgb) noexcept;

		constexpr Color& setRGB(uint32 _r, uint32 _g, uint32 _b) noexcept;

		constexpr Color& set(uint32 rgb, uint32 _a = 255) noexcept;

		constexpr Color& set(uint32 _r, uint32 _g, uint32 _b, uint32 _a = 255) noexcept;

		constexpr Color& set(Color color) noexcept;

		[[nodiscard]]
		constexpr uint8 grayscale0_255() const noexcept;

		[[nodiscard]]
		constexpr double grayscale() const noexcept;

		[[nodiscard]]
		constexpr uint8 minRGBComponent() const noexcept;

		[[nodiscard]]
		constexpr uint8 maxRGBComponent() const noexcept;

		[[nodiscard]]
		constexpr uint8 minComponent() const noexcept;

		[[nodiscard]]
		constexpr uint8 maxComponent() const noexcept;

		[[nodiscard]]
		constexpr uint32 asUint32() const noexcept;

		[[nodiscard]]
		constexpr Color lerp(Color other, double f) const noexcept;

		[[nodiscard]]
		constexpr Color lightened(double amount) const noexcept;

		[[nodiscard]]
		constexpr Color darkened(double amount) const noexcept;

		[[nodiscard]]
		Color gamma(double gamma) const noexcept;

		[[nodiscard]]
		ColorF removeSRGBCurve() const noexcept;

		[[nodiscard]]
		ColorF applySRGBCurve() const noexcept;

		[[nodiscard]]
		String toHex() const;

		[[nodiscard]]
		uint64 hash() const noexcept;

		[[nodiscard]]
		static constexpr Color Zero() noexcept;

		[[nodiscard]]
		static constexpr uint8 ToUint8(float x) noexcept;

		[[nodiscard]]
		static constexpr uint8 ToUint8(double x) noexcept;

		[[nodiscard]]
		static constexpr Color FromRGBA(uint32 rgba) noexcept;

		[[nodiscard]]
		static constexpr Color FromABGR(uint32 abgr) noexcept;

		template <class CharType>
		friend std::basic_ostream<CharType>& operator <<(std::basic_ostream<CharType>& output, const Color& value)
		{
			return output << CharType('(')
				<< value.r << CharType(',') << CharType(' ')
				<< value.g << CharType(',') << CharType(' ')
				<< value.b << CharType(',') << CharType(' ')
				<< value.a << CharType(')');
		}

		//template <class CharType>
		//friend std::basic_istream<CharType>& operator >>(std::basic_istream<CharType>& input, Color& value)
		//{

		//}

		friend void Formatter(FormatData& formatData, const Color& value);
	};

	[[nodiscard]]
	constexpr Color Alpha(uint32 alpha) noexcept;
}

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
