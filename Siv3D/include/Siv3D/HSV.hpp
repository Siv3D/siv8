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
	////////////////////////////////////////////////////////////////
	//
	//	HSV
	//
	////////////////////////////////////////////////////////////////

	/// @brief HSV 表色系の色
	struct HSV
	{
		/// @brief 色相 | Hue [0.0, 360.0)
		double h;

		/// @brief 彩度 | Saturation [0.0, 1.0]
		double s;

		/// @brief 明度 | Value [0.0, 1.0]
		double v;

		/// @brief アルファ | Alpha [0.0, 1.0]
		double a = 1.0;

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		/// @brief デフォルトコンストラクタ
		[[nodiscard]]	
		HSV() = default;

		/// @brief コピーコンストラクタ
		[[nodiscard]]
		HSV(const HSV&) = default;

		/// @brief HSV 表色系の色を作成します。
		/// @param _h 色相
		/// @param _a アルファ
		[[nodiscard]]
		explicit constexpr HSV(double _h, double _a = 1.0) noexcept;

		/// @brief HSV 表色系の色を作成します。
		/// @param _h 色相
		/// @param _a アルファ
		[[nodiscard]]
		explicit constexpr HSV(Concept::Arithmetic auto _h, double _a = 1.0) noexcept;

		/// @brief HSV 表色系の色を作成します。
		/// @param _h 色相
		/// @param _s 彩度
		/// @param _v 明度
		/// @param _a アルファ
		[[nodiscard]]
		constexpr HSV(double _h, double _s, double _v, double _a = 1.0) noexcept;

		/// @brief HSV 表色系の色を作成します。
		/// @param _h 色相
		/// @param _s 彩度
		/// @param _v 明度
		/// @param _a アルファ
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

		////////////////////////////////////////////////////////////////
		//
		//	elem
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定したインデックスの成分を返します。
		/// @param index インデックス
		/// @return 指定したインデックスの成分。範囲外の場合は 0.0
		/// @remark 0 で H 成分、1 で S 成分、2 で V 成分、3 で アルファ値を返します。
		[[nodiscard]]
		constexpr double elem(size_t index) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	getPointer
		//
		////////////////////////////////////////////////////////////////

		/// @brief H 成分へのポインタを返します。
		/// @return H 成分へのポインタ
		/// @remark [0] で H 成分、[1] で S 成分、[2] で V 成分、[3] で アルファ値にアクセスできます。
		[[nodiscard]]
		constexpr double* getPointer() noexcept;

		/// @brief H 成分へのポインタを返します。
		/// @return H 成分へのポインタ
		/// @remark [0] で H 成分、[1] で S 成分、[2] で V 成分、[3] で アルファ値にアクセスできます。
		[[nodiscard]]
		constexpr const double* getPointer() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator =
		//
		////////////////////////////////////////////////////////////////

		constexpr HSV& operator =(const HSV&) = default;

		////////////////////////////////////////////////////////////////
		//
		//	operator +
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr HSV operator +(const HSV& hsv) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator -
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr HSV operator -(const HSV& hsv) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator ==
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		friend constexpr bool operator ==(const HSV& lhs, const HSV& rhs) noexcept
		{
			return ((lhs.h == rhs.h)
				 && (lhs.s == rhs.s)
				 && (lhs.v == rhs.v)
				 && (lhs.a == rhs.a));
		}

		////////////////////////////////////////////////////////////////
		//
		//	withH, withS, withV, withA
		//
		////////////////////////////////////////////////////////////////

		/// @brief H 成分のみを変更した自身のコピーを返します。
		/// @param _h 新しい H 成分
		/// @return H 成分を変更したコピー
		[[nodiscard]]
		constexpr HSV withH(double _h) const noexcept;

		/// @brief S 成分のみを変更した自身のコピーを返します。
		/// @param _s 新しい S 成分
		/// @return S 成分を変更したコピー
		[[nodiscard]]
		constexpr HSV withS(double _s) const noexcept;

		/// @brief V 成分のみを変更した自身のコピーを返します。
		/// @param _v 新しい V 成分
		/// @return V 成分を変更したコピー
		[[nodiscard]]
		constexpr HSV withV(double _v) const noexcept;

		/// @brief アルファ値のみを変更した自身のコピーを返します。
		/// @param _a 新しいアルファ値
		/// @return アルファ値を変更したコピー
		[[nodiscard]]
		constexpr HSV withA(double _a) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	withAlpha
		//
		////////////////////////////////////////////////////////////////

		/// @brief アルファ値のみを変更した自身のコピーを返します。
		/// @param _a 新しいアルファ値
		/// @return アルファ値を変更したコピー
		/// @remark `.withA(_a)` と同じです。
		[[nodiscard]]
		constexpr HSV withAlpha(double _a) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	setH, setS, setV, setA
		//
		////////////////////////////////////////////////////////////////

		/// @brief H 成分を変更します。
		/// @param _r 新しい H 成分
		/// @return *this
		constexpr HSV& setH(double _h) noexcept;

		/// @brief S 成分を変更します。
		/// @param _s 新しい S 成分
		/// @return *this
		constexpr HSV& setS(double _s) noexcept;

		/// @brief V 成分を変更します。
		/// @param _v 新しい V 成分
		/// @return *this
		constexpr HSV& setV(double _v) noexcept;

		/// @brief アルファ値を変更します。
		/// @param _a 新しいアルファ値
		/// @return *this
		constexpr HSV& setA(double _a) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	setHSV
		//
		////////////////////////////////////////////////////////////////

		/// @brief H, S, V 成分を変更します。
		/// @param _h 新しい H 成分
		/// @param _s 新しい S 成分
		/// @param _v 新しい V 成分
		/// @return *this
		constexpr HSV& setHSV(double _h, double _s, double _v) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	set
		//
		////////////////////////////////////////////////////////////////

		/// @brief 色を変更します。
		/// @param _h 新しい H 成分
		/// @param _s 新しい S 成分
		/// @param _v 新しい V 成分
		/// @param _a 新しいアルファ値
		/// @return *this
		constexpr HSV& set(double _h, double _s, double _v, double _a = 1.0) noexcept;

		/// @brief 色を変更します。
		/// @param hsva 新しい色
		/// @return *this
		constexpr HSV& set(const HSV& hsva) noexcept;

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
		constexpr HSV lerp(const HSV& other, double f) const noexcept;

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
		//	toColor
		//
		////////////////////////////////////////////////////////////////
	
		/// @brief 色を Color で返します。
		/// @return Color
		[[nodiscard]]
		Color toColor() const noexcept;

		/// @brief 色を Color で返します。
		/// @param alpha アルファ値
		/// @return Color
		[[nodiscard]]
		Color toColor(uint32 alpha) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	toColorF
		//
		////////////////////////////////////////////////////////////////

		/// @brief 色を ColorF で返します。
		/// @return ColorF
		[[nodiscard]]
		ColorF toColorF() const noexcept;

		/// @brief 色を ColorF で返します。
		/// @param alpha アルファ値
		/// @return ColorF
		[[nodiscard]]
		ColorF toColorF(double alpha) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	hsv, hsva
		//
		////////////////////////////////////////////////////////////////

		/// @brief Vec3{ h, s, v } を返します。
		/// @return Vec3{ h, s, v }
		[[nodiscard]]
		constexpr Vec3 hsv() const noexcept;

		/// @brief Vec4{ h, s, v, a } を返します。
		/// @return Vec4{ h, s, v, a }
		[[nodiscard]]
		constexpr Vec4 hsva() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator <<
		//
		////////////////////////////////////////////////////////////////

		template <class CharType>
		friend std::basic_ostream<CharType>& operator <<(std::basic_ostream<CharType>& output, const HSV& value)
		{
			return output << CharType('(')
				<< value.h << CharType(',') << CharType(' ')
				<< value.s << CharType(',') << CharType(' ')
				<< value.v << CharType(',') << CharType(' ')
				<< value.a << CharType(')');
		}

		////////////////////////////////////////////////////////////////
		//
		//	operator >>
		//
		////////////////////////////////////////////////////////////////

		template <class CharType>
		friend std::basic_istream<CharType>& operator >>(std::basic_istream<CharType>& input, HSV& value)
		{
			CharType unused{};
			return input >> unused
				>> value.h >> unused
				>> value.s >> unused
				>> value.v >> unused
				>> value.a >> unused;
		}

		////////////////////////////////////////////////////////////////
		//
		//	Zero
		//
		////////////////////////////////////////////////////////////////

		/// @brief HSV{ 0.0, 0.0, 0.0, 0.0 } を返します。
		/// @return HSV{ 0.0, 0.0, 0.0, 0.0 }
		[[nodiscard]]
		static constexpr HSV Zero() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	Formatter
		//
		////////////////////////////////////////////////////////////////

		friend void Formatter(FormatData& formatData, const HSV& value);
	};

	////////////////////////////////////////////////////////////////
	//
	//	HueToColor
	//
	////////////////////////////////////////////////////////////////

	/// @brief 色相から Color を作成します。
	/// @param hue 色相
	/// @return Color
	/// @remark `HSV{ hue, 1.0, 1.0 }.toColor()` と同じです。
	[[nodiscard]]
	Color HueToColor(double hue) noexcept;

	////////////////////////////////////////////////////////////////
	//
	//	HueToColorF
	//
	////////////////////////////////////////////////////////////////

	/// @brief 色相から ColorF を作成します。
	/// @param hue 色相
	/// @return Color
	/// @remark `HSV{ hue, 1.0, 1.0 }.toColorF()` と同じです。
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
