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

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	Mat3x2
	//
	////////////////////////////////////////////////////////////////

	/// @brief アフィン変換用の 3x2 行列クラス
	struct Mat3x2
	{
		using value_type = float;

		value_type _11, _12;
		value_type _21, _22;
		value_type _31, _32;

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Mat3x2() = default;

		[[nodiscard]]
		explicit constexpr Mat3x2(Concept::Arithmetic auto s) noexcept;

		[[nodiscard]]
		constexpr Mat3x2(float _11, float _12, float _21, float _22, float _31, float _32) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	translated
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Mat3x2 constexpr translated(Float2 v) const noexcept;

		[[nodiscard]]
		Mat3x2 constexpr translated(Concept::Arithmetic auto x, Concept::Arithmetic auto y) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	scaled
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Mat3x2 constexpr scaled(Concept::Arithmetic auto s, Float2 center = Float2{ 0, 0 }) const noexcept;

		[[nodiscard]]
		Mat3x2 constexpr scaled(Float2 scale, Float2 center = Float2{ 0, 0 }) const noexcept;

		[[nodiscard]]
		Mat3x2 constexpr scaled(Concept::Arithmetic auto sx, Concept::Arithmetic auto sy, Float2 center = Float2{ 0, 0 }) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	rotated
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Mat3x2 rotated(Concept::Arithmetic auto angle, Float2 center = Float2{ 0, 0 }) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	shearedX, shearedY
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr Mat3x2 shearedX(Concept::Arithmetic auto sx) const noexcept;

		[[nodiscard]]
		constexpr Mat3x2 shearedY(Concept::Arithmetic auto sy) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	determinant
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr float determinant() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	inverse
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr Mat3x2 inverse() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	setProduct
		//
		////////////////////////////////////////////////////////////////

		constexpr void setProduct(const Mat3x2& a, const Mat3x2& b) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator *
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr Mat3x2 operator *(const Mat3x2& other) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	transformPoint
		//
		////////////////////////////////////////////////////////////////

		/// @brief 点の座標を変換します
		/// @param pos 点の座標
		/// @return 変換後の座標
		[[nodiscard]]
		constexpr Float2 transformPoint(Point pos) const noexcept;

		/// @brief 点の座標を変換します
		/// @param pos 点の座標
		/// @return 変換後の座標
		[[nodiscard]]
		constexpr Float2 transformPoint(Float2 pos) const noexcept;

		/// @brief 点の座標を変換します
		/// @param pos 点の座標
		/// @return 変換後の座標
		[[nodiscard]]
		constexpr Vec2 transformPoint(Vec2 pos) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	hash
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		uint64 hash() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator ==
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		friend constexpr bool operator ==(const Mat3x2& lhs, const Mat3x2& rhs) noexcept
		{
			return ((lhs._11 == rhs._11) && (lhs._12 == rhs._12)
				 && (lhs._21 == rhs._21) && (lhs._22 == rhs._22)
				 && (lhs._31 == rhs._31) && (lhs._32 == rhs._32));
		}

		////////////////////////////////////////////////////////////////
		//
		//	Identity
		//
		////////////////////////////////////////////////////////////////

		/// @brief 何も変換をしない行列を返します。
		/// @return 何も変換をしない行列
		[[nodiscard]]
		static constexpr Mat3x2 Identity() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	Translate
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		static constexpr Mat3x2 Translate(Float2 v) noexcept;

		[[nodiscard]]
		static constexpr Mat3x2 Translate(Concept::Arithmetic auto x, Concept::Arithmetic auto y) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	Scale
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		static constexpr Mat3x2 Scale(Float2 scale, Float2 center = Float2{ 0, 0 }) noexcept;

		[[nodiscard]]
		static constexpr Mat3x2 Scale(Concept::Arithmetic auto s, Float2 center = Float2{ 0, 0 }) noexcept;

		[[nodiscard]]
		static constexpr Mat3x2 Scale(Concept::Arithmetic auto sx, Concept::Arithmetic auto sy, Float2 center = Float2{ 0, 0 }) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	Rotate
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		static Mat3x2 Rotate(Concept::Arithmetic auto angle, Float2 center = Float2{ 0, 0 }) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	ShearX, ShearY
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		static constexpr Mat3x2 ShearX(Concept::Arithmetic auto sx) noexcept;

		[[nodiscard]]
		static constexpr Mat3x2 ShearY(Concept::Arithmetic auto sy) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	Screen
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		static constexpr Mat3x2 Screen(Concept::Arithmetic auto width, Concept::Arithmetic auto height) noexcept;

		[[nodiscard]]
		static constexpr Mat3x2 Screen(Float2 size) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator <<
		//
		////////////////////////////////////////////////////////////////

		template <class CharType>
		friend std::basic_ostream<CharType>& operator <<(std::basic_ostream<CharType>& output, const Mat3x2& value)
		{
			return output << CharType('(')
				<< Float2{ value._11, value._12 } << CharType(',')
				<< Float2{ value._21, value._22 } << CharType(',')
				<< Float2{ value._31, value._32 } << CharType(')');
		}

		////////////////////////////////////////////////////////////////
		//
		//	operator >>
		//
		////////////////////////////////////////////////////////////////

		template <class CharType>
		friend std::basic_istream<CharType>& operator >>(std::basic_istream<CharType>& input, Mat3x2& value)
		{
			CharType unused;
			Float2 r1, r2, r3;

			input >> unused
				>> r1 >> unused
				>> r2 >> unused
				>> r3 >> unused;

			value._11 = r1.x;
			value._12 = r1.y;
			value._21 = r2.x;
			value._22 = r2.y;
			value._31 = r3.x;
			value._32 = r3.y;

			return input;
		}

		////////////////////////////////////////////////////////////////
		//
		//	Formatter
		//
		////////////////////////////////////////////////////////////////

		friend void Formatter(FormatData& formatData, const Mat3x2& value);
	};
}

////////////////////////////////////////////////////////////////
//
//	fmt
//
////////////////////////////////////////////////////////////////

template <>
struct fmt::formatter<s3d::Mat3x2, s3d::char32>
{
	std::u32string tag;

	s3d::ParseContext::iterator parse(s3d::ParseContext& ctx);

	s3d::BufferContext::iterator format(const s3d::Mat3x2& value, s3d::BufferContext& ctx);
};

////////////////////////////////////////////////////////////////
//
//	std::hash
//
////////////////////////////////////////////////////////////////

template <>
struct std::hash<s3d::Mat3x2>
{
	[[nodiscard]]
	size_t operator ()(const s3d::Mat3x2& value) const noexcept
	{
		return value.hash();
	}
};

# include "detail/Mat3x2.ipp"
