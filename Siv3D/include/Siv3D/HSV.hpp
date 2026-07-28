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

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	HSV
	//
	////////////////////////////////////////////////////////////////

	/// @brief HSV 表色系の色を表現するクラスです。
	/// @remark H 成分には任意の有限値を保持でき、RGB への変換時に 360° 周期で扱われます。
	/// @remark S, V, A 成分は通常 0.0 以上 1.0 以下の値を持ちますが、範囲外の値も保持できます。
	struct HSV
	{
		/// @brief 色相 | Hue
		double h;

		/// @brief 彩度 | Saturation
		double s;

		/// @brief 明度 | Value
		double v;

		/// @brief アルファ | Alpha
		double a = 1.0;

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		/// @brief デフォルトコンストラクタ
		/// @remark H, S, V 成分は初期化されません。A 成分は 1.0 で初期化されます。
		[[nodiscard]]	
		HSV() = default;

		/// @brief コピーコンストラクタ
		/// @param hsv コピーする色
		[[nodiscard]]
		HSV(const HSV& hsv) = default;

		/// @brief 色相から HSV を作成します。
		/// @param _h 色相
		/// @param _a アルファ値
		/// @remark S, V 成分は 1.0 で初期化されます。
		[[nodiscard]]
		explicit constexpr HSV(double _h, double _a = 1.0) noexcept;

		/// @brief 色相から HSV を作成します。
		/// @param _h 色相
		/// @param _a アルファ値
		/// @remark S, V 成分は 1.0 で初期化されます。
		[[nodiscard]]
		explicit constexpr HSV(Concept::Arithmetic auto _h, double _a = 1.0) noexcept;

		/// @brief HSV を作成します。
		/// @param _h 色相
		/// @param _s 彩度
		/// @param _v 明度
		/// @param _a アルファ値
		[[nodiscard]]
		constexpr HSV(double _h, double _s, double _v, double _a = 1.0) noexcept;

		/// @brief HSV を作成します。
		/// @param _h 色相
		/// @param _s 彩度
		/// @param _v 明度
		/// @param _a アルファ値
		[[nodiscard]]
		constexpr HSV(Concept::Arithmetic auto _h, double _s, double _v, double _a = 1.0) noexcept;

		/// @brief 別の HSV から HSV を作成します。
		/// @param hsv 別の HSV
		/// @param _a アルファ値
		/// @remark 引数 `hsv` のアルファ値は無視されます。
		[[nodiscard]]
		constexpr HSV(const HSV& hsv, double _a) noexcept;

		/// @brief Color から HSV を作成します。
		/// @param color Color
		[[nodiscard]]
		HSV(Color color) noexcept;

		/// @brief ColorF から HSV を作成します。
		/// @param color ColorF
		/// @remark RGB の各成分には [0.0, 1.0] の範囲の有限値を指定してください。実行時の範囲チェックは行いません。
		[[nodiscard]]
		HSV(const ColorF& color) noexcept;

		/// @brief Vec3 から HSV を作成します。
		/// @param hsv 色相・彩度・明度
		/// @param _a アルファ値
		[[nodiscard]]
		constexpr HSV(const Vec3& hsv, double _a) noexcept;

		/// @brief Vec4 から HSV を作成します。
		/// @param hsva 色相・彩度・明度・アルファ値
		[[nodiscard]]
		constexpr HSV(const Vec4& hsva) noexcept;

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

		/// @brief 色をコピー代入します。
		/// @param hsv コピーする色
		/// @return *this
		constexpr HSV& operator =(const HSV& hsv) noexcept = default;

		////////////////////////////////////////////////////////////////
		//
		//	operator ==
		//
		////////////////////////////////////////////////////////////////

		/// @brief 2 つの色が等しいかを返します。
		/// @param lhs 一方の色
		/// @param rhs もう一方の色
		/// @return HSVA の全成分がそれぞれ等しい場合 true, それ以外の場合は false
		/// @remark 各成分を浮動小数点数として厳密に比較します。
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
		/// @param _h 新しい H 成分
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
		//	hueShifted
		//
		////////////////////////////////////////////////////////////////

		/// @brief 色相をシフトした色を返します。
		/// @param degrees 色相のシフト量（度）
		/// @return 色相をシフトした色
		/// @remark 結果の H 成分は [0.0, 360.0) の範囲に正規化されません。
		[[nodiscard]]
		constexpr HSV hueShifted(double degrees) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	lerp
		//
		////////////////////////////////////////////////////////////////

		/// @brief 2 つの色の間を線形補間した色を返します。
		/// @param other もう一方の色
		/// @param f 補間係数
		/// @return 補間された色
		/// @remark H 成分を含む各成分を数直線上で補間します。色相の最短経路補間は行いません。
		/// @remark `f` はクランプされません。範囲 [0.0, 1.0] の外では外挿します。
		[[nodiscard]]
		constexpr HSV lerp(const HSV& other, double f) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	lerpShortestHue
		//
		////////////////////////////////////////////////////////////////

		/// @brief 色相の最短経路を使って 2 つの色の間を補間した色を返します。
		/// @param other もう一方の色
		/// @param f 補間係数
		/// @return 色相の最短経路を使って補間された色
		/// @remark S, V, A 成分は数直線上で補間します。
		/// @remark 色相差が正確に 180° の場合は `Math::LerpAngle()` と同じ規則で方向を選択します。
		/// @remark `f` はクランプされません。範囲 [0.0, 1.0] の外では外挿します。
		[[nodiscard]]
		HSV lerpShortestHue(const HSV& other, double f) const noexcept;

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
		//	srgbToLinear, linearToSRGB
		//
		////////////////////////////////////////////////////////////////

		/// @brief sRGB 色空間からリニア色空間に変換した色を返します。
		/// @return リニア色空間に変換した色
		/// @remark HSV を RGB に変換してから、RGB 成分に sRGB の逆伝達関数を適用します。アルファ値は維持します。
		[[nodiscard]]
		ColorF srgbToLinear() const noexcept;

		/// @brief リニア色空間から sRGB 色空間に変換した色を返します。
		/// @return sRGB 色空間に変換した色
		/// @remark HSV を RGB に変換してから、RGB 成分に sRGB の伝達関数を適用します。アルファ値は維持します。
		[[nodiscard]]
		ColorF linearToSRGB() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	toColor
		//
		////////////////////////////////////////////////////////////////
	
		/// @brief 色を Color で返します。
		/// @return Color
		/// @remark H 成分には有限値を、S, V, A 成分には [0.0, 1.0] の範囲の有限値を指定してください。実行時の範囲チェックは行いません。
		[[nodiscard]]
		Color toColor() const noexcept;

		/// @brief 色を Color で返します。
		/// @param alpha アルファ値 [0, 255]
		/// @return Color
		/// @remark 自身の A 成分は無視されます。
		/// @remark H 成分には有限値を、S, V 成分には [0.0, 1.0] の範囲の有限値を指定してください。実行時の範囲チェックは行いません。
		[[nodiscard]]
		Color toColor(uint32 alpha) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	toColorF
		//
		////////////////////////////////////////////////////////////////

		/// @brief 色を ColorF で返します。
		/// @return ColorF
		/// @remark H 成分には有限値を、S, V, A 成分には [0.0, 1.0] の範囲の有限値を指定してください。実行時の範囲チェックは行いません。
		[[nodiscard]]
		ColorF toColorF() const noexcept;

		/// @brief 色を ColorF で返します。
		/// @param alpha アルファ値 [0.0, 1.0]
		/// @return ColorF
		/// @remark 自身の A 成分は無視されます。
		/// @remark H 成分には有限値を、S, V 成分には [0.0, 1.0] の範囲の有限値を指定してください。実行時の範囲チェックは行いません。
		[[nodiscard]]
		ColorF toColorF(double alpha) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	hsv
		//
		////////////////////////////////////////////////////////////////

		/// @brief Vec3{ h, s, v } を返します。
		/// @return Vec3{ h, s, v }
		[[nodiscard]]
		constexpr Vec3 hsv() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	sva
		//
		////////////////////////////////////////////////////////////////

		/// @brief Vec3{ s, v, a } を返します。
		/// @return Vec3{ s, v, a }
		[[nodiscard]]
		constexpr Vec3 sva() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	hsva
		//
		////////////////////////////////////////////////////////////////

		/// @brief Vec4{ h, s, v, a } を返します。
		/// @return Vec4{ h, s, v, a }
		[[nodiscard]]
		constexpr Vec4 hsva() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator <<
		//
		////////////////////////////////////////////////////////////////

		/// @brief 色を出力ストリームに書き込みます。
		/// @param output 出力ストリーム
		/// @param value 色
		/// @return 出力ストリーム
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

		/// @brief 入力ストリームから色を読み込みます。
		/// @param input 入力ストリーム
		/// @param value 読み込んだ色の格納先
		/// @return 入力ストリーム
		/// @remark `(h, s, v, a)` の形式に対応します。
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

		/// @brief 色を文字列に変換します。
		/// @param formatData 文字列バッファ
		/// @param value 色
		/// @remark この関数は Format 用の関数です。通常、ユーザーが直接呼び出す必要はありません。
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
	/// @remark `hue` には有限値を指定してください。
	[[nodiscard]]
	Color HueToColor(double hue) noexcept;

	////////////////////////////////////////////////////////////////
	//
	//	HueToColorF
	//
	////////////////////////////////////////////////////////////////

	/// @brief 色相から ColorF を作成します。
	/// @param hue 色相
	/// @return ColorF
	/// @remark `HSV{ hue, 1.0, 1.0 }.toColorF()` と同じです。
	/// @remark `hue` には有限値を指定してください。
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

	s3d::BufferContext::iterator format(const s3d::HSV& value, s3d::BufferContext& ctx) const;
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
