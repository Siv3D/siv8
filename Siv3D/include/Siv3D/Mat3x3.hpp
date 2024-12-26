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
# include "2DShapes.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	Mat3x3
	//
	////////////////////////////////////////////////////////////////

	struct Mat3x3
	{
		using value_type = float;

		value_type _11, _12, _13;
		value_type _21, _22, _23;
		value_type _31, _32, _33;

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Mat3x3() = default;
	
		[[nodiscard]]
		constexpr Mat3x3(float _11, float _12, float _13, float _21, float _22, float _23, float _31, float _32, float _33) noexcept;

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
		friend constexpr bool operator ==(const Mat3x3& lhs, const Mat3x3& rhs) noexcept = default;

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
		constexpr Mat3x3 inverse() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	transformPoint
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr Float2 transformPoint(Point pos) const noexcept;

		[[nodiscard]]
		constexpr Float2 transformPoint(Float2 pos) const noexcept;

		[[nodiscard]]
		constexpr Vec2 transformPoint(Vec2 pos) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	transformRect
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr Quad transformRect(const RectF& rect) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	Identity
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		static constexpr Mat3x3 Identity() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	Homography
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		static Mat3x3 Homography(const Quad& to);

		[[nodiscard]]
		static Mat3x3 Homography(const RectF& from, const RectF& to);

		[[nodiscard]]
		static Mat3x3 Homography(const RectF& from, const Quad& to);

		[[nodiscard]]
		static Mat3x3 Homography(const Quad& from, const RectF& to);

		[[nodiscard]]
		static Mat3x3 Homography(const Quad& from, const Quad& to);

		////////////////////////////////////////////////////////////////
		//
		//	operator <<
		//
		////////////////////////////////////////////////////////////////

		template <class CharType>
		friend std::basic_ostream<CharType>& operator <<(std::basic_ostream<CharType>& output, const Mat3x3& value)
		{
			return output << CharType('(')
				<< Float3{ value._11, value._12, value._13 } << CharType(',')
				<< Float3{ value._21, value._22, value._23 } << CharType(',')
				<< Float3{ value._31, value._32, value._33 } << CharType(')');
		}

		////////////////////////////////////////////////////////////////
		//
		//	operator >>
		//
		////////////////////////////////////////////////////////////////

		template <class CharType>
		friend std::basic_istream<CharType>& operator >>(std::basic_istream<CharType>& input, Mat3x3& value)
		{
			CharType unused;
			Float3 r1, r2, r3;

			input >> unused
				>> r1 >> unused
				>> r2 >> unused
				>> r3 >> unused;

			value._11 = r1.x;
			value._12 = r1.y;
			value._13 = r1.z;
			value._21 = r2.x;
			value._22 = r2.y;
			value._23 = r2.z;
			value._31 = r3.x;
			value._32 = r3.y;
			value._33 = r3.z;
			return input;
		}

		////////////////////////////////////////////////////////////////
		//
		//	Formatter
		//
		////////////////////////////////////////////////////////////////

		friend void Formatter(FormatData& formatData, const Mat3x3& value);
	};
}

////////////////////////////////////////////////////////////////
//
//	fmt
//
////////////////////////////////////////////////////////////////

template <>
struct fmt::formatter<s3d::Mat3x3, s3d::char32>
{
	std::u32string tag;

	s3d::ParseContext::iterator parse(s3d::ParseContext& ctx);

	s3d::BufferContext::iterator format(const s3d::Mat3x3& value, s3d::BufferContext& ctx);
};

////////////////////////////////////////////////////////////////
//
//	std::hash
//
////////////////////////////////////////////////////////////////

template <>
struct std::hash<s3d::Mat3x3>
{
	[[nodiscard]]
	size_t operator ()(const s3d::Mat3x3& value) const noexcept
	{
		return value.hash();
	}
};

# include "detail/Mat3x3.ipp"
