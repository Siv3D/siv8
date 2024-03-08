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

namespace s3d
{
	/// @brief HSV 表色系の色
	struct HSV
	{
		/// @brief 色相 |Hue [0.0, 360.0)
		double h;

		/// @brief 彩度 | Saturation [0.0, 1.0]
		double s;

		/// @brief 明度 | Value [0.0, 1.0]
		double v;

		/// @brief アルファ | Alpha [0.0, 1.0]
		double a = 1.0;

		/// @brief デフォルトコンストラクタ
		[[nodiscard]]	
		HSV() = default;

		/// @brief コピーコンストラクタ
		[[nodiscard]]
		HSV(const HSV&) = default;

		[[nodiscard]]
		explicit constexpr HSV(double _h, double _a = 1.0) noexcept;

		[[nodiscard]]
		explicit constexpr HSV(Concept::Arithmetic auto _h, double _a = 1.0) noexcept;

		/// @brief HSV 表色系の色を作成します。
		/// @param _h 色相
		/// @param _s 彩度
		/// @param _v 明度
		/// @param _a アルファ
		[[nodiscard]]
		constexpr HSV(double _h, double _s, double _v, double _a = 1.0) noexcept;

		[[nodiscard]]
		constexpr HSV(Concept::Arithmetic auto _h, double _s, double _v, double _a = 1.0) noexcept;

		/// @brief HSV 表色系の色を作成します。
		/// @param hsv 色
		/// @param _a アルファ
		[[nodiscard]]
		constexpr HSV(const HSV& hsv, double _a) noexcept;

		/// @brief HSV 表色系の色を作成します。
		/// @param color 色
		[[nodiscard]]
		HSV(Color color) noexcept;

		/// @brief HSV 表色系の色を作成します。
		/// @param color 色
		[[nodiscard]]
		HSV(const ColorF& color) noexcept;

		[[nodiscard]]
		constexpr double elem(size_t index) const noexcept;

		[[nodiscard]]
		constexpr double* getPointer() noexcept;

		[[nodiscard]]
		constexpr const double* getPointer() const noexcept;

		constexpr HSV& operator =(const HSV&) = default;

		[[nodiscard]]
		constexpr HSV operator +(const HSV& hsv) const noexcept;

		[[nodiscard]]
		constexpr HSV operator -(const HSV& hsv) const noexcept;

		[[nodiscard]]
		friend constexpr bool operator ==(const HSV& lhs, const HSV& rhs) noexcept
		{
			return ((lhs.h == rhs.h)
				 && (lhs.s == rhs.s)
				 && (lhs.v == rhs.v)
				 && (lhs.a == rhs.a));
		}

		/// @brief h 成分のみを変更した自身のコピーを返します。
		/// @param _h h 成分
		/// @return h 成分を変更したコピー
		[[nodiscard]]
		constexpr HSV withH(double _h) const noexcept;

		/// @brief s 成分のみを変更した自身のコピーを返します。
		/// @param _s s 成分
		/// @return s 成分を変更したコピー
		[[nodiscard]]
		constexpr HSV withS(double _s) const noexcept;

		/// @brief v 成分のみを変更した自身のコピーを返します。
		/// @param _v v 成分
		/// @return v 成分を変更したコピー
		[[nodiscard]]
		constexpr HSV withV(double _v) const noexcept;

		/// @brief a 成分のみを変更した自身のコピーを返します。
		/// @param _a a 成分
		/// @return a 成分を変更したコピー
		[[nodiscard]]
		constexpr HSV withA(double _a) const noexcept;

		constexpr HSV withAlpha(double _a) const noexcept;

		constexpr HSV& setH(double _h) noexcept;

		constexpr HSV& setS(double _s) noexcept;

		constexpr HSV& setV(double _v) noexcept;

		constexpr HSV& setA(double _a) noexcept;

		constexpr HSV& setHSV(double _h, double _s, double _v) noexcept;

		constexpr HSV& set(double _h, double _s, double _v, double _a = 1.0) noexcept;

		constexpr HSV& set(const HSV& hsva) noexcept;

		[[nodiscard]]
		constexpr HSV lerp(const HSV& other, double f) const noexcept;

		[[nodiscard]]
		uint64 hash() const noexcept;

		[[nodiscard]]
		ColorF removeSRGBCurve() const noexcept;

		[[nodiscard]]
		ColorF applySRGBCurve() const noexcept;

		[[nodiscard]]
		Color toColor() const noexcept;

		[[nodiscard]]
		Color toColor(uint32 alpha) const noexcept;

		[[nodiscard]]
		ColorF toColorF() const noexcept;

		[[nodiscard]]
		ColorF toColorF(double alpha) const noexcept;

		//[[nodiscard]]
		//constexpr Vec3 hsv() const noexcept;

		//[[nodiscard]]
		//constexpr Vec4 hsva() const noexcept;

		template <class CharType>
		friend std::basic_ostream<CharType>& operator <<(std::basic_ostream<CharType>& output, const HSV& value)
		{
			return output << CharType('(')
				<< value.h << CharType(',') << CharType(' ')
				<< value.s << CharType(',') << CharType(' ')
				<< value.v << CharType(',') << CharType(' ')
				<< value.a << CharType(')');
		}

		template <class CharType>
		friend std::basic_istream<CharType>& operator >>(std::basic_istream<CharType>& input, HSV& value)
		{
			CharType unused;
			return input >> unused
				>> value.h >> unused
				>> value.s >> unused
				>> value.v >> unused
				>> value.a >> unused;
		}

		[[nodiscard]]
		static constexpr HSV Zero() noexcept;

		friend void Formatter(FormatData& formatData, const HSV& value);
	};

	[[nodiscard]]
	Color HueToColor(double hue) noexcept;

	[[nodiscard]]
	ColorF HueToColorF(double hue) noexcept;
}

////////////////////////////////////////////////////////////////
//
//	fmt
//
////////////////////////////////////////////////////////////////

template <>
struct fmt::formatter<s3d::HSV, s3d::char32>
{
	std::u32string tag;

	s3d::ParseContext::iterator parse(s3d::ParseContext& ctx);

	s3d::BufferContext::iterator format(const s3d::HSV& value, s3d::BufferContext& ctx);
};

////////////////////////////////////////////////////////////////
//
//	std::hash
//
////////////////////////////////////////////////////////////////

template <>
struct std::hash<s3d::HSV>
{
	[[nodiscard]]
	size_t operator ()(const s3d::HSV& value) const noexcept
	{
		return value.hash();
	}
};
