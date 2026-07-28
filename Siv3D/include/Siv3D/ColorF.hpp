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
# include "FormatLiteral.hpp"

namespace s3d
{
	struct FormatData;
	class HalfFloat;

	////////////////////////////////////////////////////////////////
	//
	//	ColorF
	//
	////////////////////////////////////////////////////////////////

	/// @brief 色を RGBA 各成分について浮動小数点数で表現するクラスです。
	/// @remark 各成分は通常 0.0 以上 1.0 以下の値を持ちますが、範囲外の値も保持できます。
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
		/// @remark 各成分は初期化されません。
		[[nodiscard]]
		ColorF() = default;

		/// @brief コピーコンストラクタ
		/// @param color コピーする色
		[[nodiscard]]
		ColorF(const ColorF& color) = default;

		/// @brief ColorF を作成します。
		/// @param _r R 成分
		/// @param _g G 成分
		/// @param _b B 成分
		/// @param _a アルファ値
		[[nodiscard]]
		constexpr ColorF(double _r, double _g, double _b, double _a = 1.0) noexcept;

		/// @brief ColorF を作成します。
		/// @param rgb RGB 成分
		[[nodiscard]]
		explicit constexpr ColorF(double rgb) noexcept;

		/// @brief ColorF を作成します。
		/// @param rgb RGB 成分
		/// @param _a アルファ値
		[[nodiscard]]
		constexpr ColorF(double rgb, double _a) noexcept;

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
		/// @remark 引数 `rgb` のアルファ値は無視されます。
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

		/// @brief 色をコピー代入します。
		/// @param color コピーする色
		/// @return *this
		constexpr ColorF& operator =(const ColorF& color) noexcept = default;

		/// @brief Color の各成分を [0.0, 1.0] に変換して代入します。
		/// @param color 代入する色
		/// @return *this
		constexpr ColorF& operator =(Color color) noexcept;

		/// @brief HSV を RGB に変換して代入します。
		/// @param hsva 代入する色
		/// @return *this
		ColorF& operator =(const HSV& hsva) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator +
		//
		////////////////////////////////////////////////////////////////

		/// @brief RGB 成分を加算した色を返します。
		/// @param rgb 加算する RGB 成分
		/// @return RGB 成分を加算し、アルファ値を維持した色
		[[nodiscard]]
		constexpr ColorF operator +(const ColorF& rgb) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator +=
		//
		////////////////////////////////////////////////////////////////

		/// @brief RGB 成分を加算します。
		/// @param rgb 加算する RGB 成分
		/// @return *this
		/// @remark アルファ値は変更しません。
		constexpr ColorF& operator +=(const ColorF& rgb) & noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator -
		//
		////////////////////////////////////////////////////////////////

		/// @brief RGB 成分を減算した色を返します。
		/// @param rgb 減算する RGB 成分
		/// @return RGB 成分を減算し、アルファ値を維持した色
		[[nodiscard]]
		constexpr ColorF operator -(const ColorF& rgb) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator -=
		//
		////////////////////////////////////////////////////////////////

		/// @brief RGB 成分を減算します。
		/// @param rgb 減算する RGB 成分
		/// @return *this
		/// @remark アルファ値は変更しません。
		constexpr ColorF& operator -=(const ColorF& rgb) & noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator *
		//
		////////////////////////////////////////////////////////////////

		/// @brief RGB 成分をスカラー倍した色を返します。
		/// @param s RGB 成分に掛ける値
		/// @return RGB 成分をスカラー倍し、アルファ値を維持した色
		[[nodiscard]]
		constexpr ColorF operator *(double s) const noexcept;

		/// @brief RGBA の各成分を乗算した色を返します。
		/// @param rgba 各成分に掛ける色
		/// @return RGBA の各成分を乗算した色
		[[nodiscard]]
		constexpr ColorF operator *(const ColorF& rgba) const noexcept;

		/// @brief 色の RGB 成分をスカラー倍した色を返します。
		/// @param s RGB 成分に掛ける値
		/// @param color 色
		/// @return RGB 成分をスカラー倍し、アルファ値を維持した色
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

		/// @brief RGB 成分をスカラー倍します。
		/// @param s RGB 成分に掛ける値
		/// @return *this
		/// @remark アルファ値は変更しません。
		constexpr ColorF& operator *=(double s) & noexcept;

		/// @brief RGBA の各成分を乗算します。
		/// @param rgba 各成分に掛ける色
		/// @return *this
		constexpr ColorF& operator *=(const ColorF& rgba) & noexcept;

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

		/// @brief 2 つの色が等しいかを返します。
		/// @param lhs 一方の色
		/// @param rhs もう一方の色
		/// @return RGBA の全成分がそれぞれ等しい場合 true, それ以外の場合は false
		/// @remark 各成分を浮動小数点数として厳密に比較します。
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
		[[nodiscard]]
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
		constexpr ColorF& set(const ColorF& color) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	premultiplied
		//
		////////////////////////////////////////////////////////////////

		/// @brief アルファ乗算済みの色を返します。
		/// @return アルファ乗算済みの色
		[[nodiscard]]
		constexpr ColorF premultiplied() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	unpremultiplied
		//
		////////////////////////////////////////////////////////////////

		/// @brief アルファ乗算済みの色を通常の色に戻します。
		/// @return アルファ乗算を解除した色
		/// @remark アルファ値が 0.0 の場合は `ColorF::Zero()` を返します。
		[[nodiscard]]
		constexpr ColorF unpremultiplied() const noexcept;

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
		/// @remark `f` はクランプされません。範囲 [0.0, 1.0] の外では外挿します。
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
		/// @remark `amount` はクランプされません。
		[[nodiscard]]
		constexpr ColorF lightened(double amount) const noexcept;

		/// @brief 色を暗くした色を返します。
		/// @param amount 暗くする量 [0.0, 1.0]
		/// @return 暗くした色
		/// @remark `ColorF{ (r * (1.0 - amount)), (g * (1.0 - amount)), (b * (1.0 - amount)), a }` を返します。
		/// @remark `amount` はクランプされません。
		[[nodiscard]]
		constexpr ColorF darkened(double amount) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	hueShifted
		//
		////////////////////////////////////////////////////////////////

		/// @brief 色相をシフトした色を返します。
		/// @param degrees 色相のシフト量（度）
		/// @return 色相をシフトした色
		[[nodiscard]]
		ColorF hueShifted(double degrees) const noexcept;

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
		/// @remark `gamma <= 0.0` の場合、RGB 成分が 0.0 の色を返します。アルファ値は維持します。
		[[nodiscard]]
		ColorF gamma(double gamma) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	srgbToLinear, linearToSRGB
		//
		////////////////////////////////////////////////////////////////

		/// @brief sRGB 色空間からリニア色空間に変換した色を返します。
		/// @return リニア色空間に変換した色
		/// @remark アルファ値は維持します。
		[[nodiscard]]
		ColorF srgbToLinear() const noexcept;

		/// @brief リニア色空間から sRGB 色空間に変換した色を返します。
		/// @return sRGB 色空間に変換した色
		/// @remark アルファ値は維持します。
		[[nodiscard]]
		ColorF linearToSRGB() const noexcept;

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
		/// @remark 各成分には有限値を指定してください。
		[[nodiscard]]
		constexpr Color toColor() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	toFloat3
		//
		////////////////////////////////////////////////////////////////

		/// @brief Float3{ r, g, b } を返します。
		/// @return Float3{ r, g, b }
		[[nodiscard]]
		constexpr Float3 toFloat3() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	toVec3
		//
		////////////////////////////////////////////////////////////////

		/// @brief Vec3{ r, g, b } を返します。
		/// @return Vec3{ r, g, b }
		[[nodiscard]]
		constexpr Vec3 toVec3() const noexcept;

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
		//	toHSV
		//
		////////////////////////////////////////////////////////////////

		/// @brief 色を HSV に変換して返します。
		/// @return HSV に変換した色
		[[nodiscard]]
		HSV toHSV() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	toR8_Unorm
		//
		////////////////////////////////////////////////////////////////

		/// @brief R 成分を `R8_Unorm` 形式で返します。
		/// @return R 成分を [0, 255] に量子化した値
		/// @remark R 成分は [0.0, 1.0] にクランプされます。
		[[nodiscard]]
		constexpr uint8 toR8_Unorm() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	toR8G8_Unorm
		//
		////////////////////////////////////////////////////////////////

		/// @brief RG 成分を `R8G8_Unorm` 形式で返します。
		/// @return bit 0-7 に R、bit 8-15 に G を格納した値
		/// @remark 各成分は [0.0, 1.0] にクランプされます。
		[[nodiscard]]
		constexpr uint16 toR8G8_Unorm() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	toR16_Float
		//
		////////////////////////////////////////////////////////////////

		/// @brief R 成分を `R16_Float` 形式で返します。
		/// @return R 成分を表す半精度浮動小数点数
		[[nodiscard]]
		HalfFloat toR16_Float() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	toR8G8B8A8_Unorm
		//
		////////////////////////////////////////////////////////////////

		/// @brief RGBA 成分を `R8G8B8A8_Unorm` 形式で返します。
		/// @return RGBA の各成分を [0, 255] に量子化した Color
		/// @remark 各成分は [0.0, 1.0] にクランプされます。
		[[nodiscard]]
		constexpr Color toR8G8B8A8_Unorm() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	toR16G16_Unorm
		//
		////////////////////////////////////////////////////////////////

		/// @brief RG 成分を `R16G16_Unorm` 形式で返します。
		/// @return bit 0-15 に R、bit 16-31 に G を格納した値
		/// @remark 各成分は [0.0, 1.0] にクランプされます。
		/// @remark 各成分には有限値を指定してください。
		[[nodiscard]]
		constexpr uint32 toR16G16_Unorm() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	toR16G16_Float
		//
		////////////////////////////////////////////////////////////////

		/// @brief RG 成分を `R16G16_Float` 形式で返します。
		/// @return bit 0-15 に R、bit 16-31 に G の半精度浮動小数点数を格納した値
		[[nodiscard]]
		uint32 toR16G16_Float() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	toR32_Float
		//
		////////////////////////////////////////////////////////////////

		/// @brief R 成分を `R32_Float` 形式で返します。
		/// @return R 成分を float 型に変換した値
		[[nodiscard]]
		constexpr float toR32_Float() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	toR10G10B10A2_Unorm
		//
		////////////////////////////////////////////////////////////////

		/// @brief RGBA 成分を `R10G10B10A2_Unorm` 形式で返します。
		/// @return bit 0-9 に R、bit 10-19 に G、bit 20-29 に B、bit 30-31 に A を格納した値
		/// @remark 各成分は [0.0, 1.0] にクランプされます。
		/// @remark 各成分には有限値を指定してください。
		[[nodiscard]]
		constexpr uint32 toR10G10B10A2_Unorm() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	toR11G11B10_UFloat
		//
		////////////////////////////////////////////////////////////////

		/// @brief RGB 成分を `R11G11B10_UFloat` 形式で返します。
		/// @return bit 0-10 に R、bit 11-21 に G、bit 22-31 に B を格納した値
		/// @remark 負の成分は 0 に、表現可能な最大値を超える成分は最大の有限値にクランプされます。
		/// @remark R と G の最大値は 65024、B の最大値は 64512 です。
		/// @remark 各成分には有限値を指定してください。
		[[nodiscard]]
		uint32 toR11G11B10_UFloat() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	toR16G16B16A16_Float
		//
		////////////////////////////////////////////////////////////////

		/// @brief RGBA 成分を `R16G16B16A16_Float` 形式で返します。
		/// @return 下位から R, G, B, A の順に半精度浮動小数点数を格納した値
		[[nodiscard]]
		uint64 toR16G16B16A16_Float() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	toR32G32_Float
		//
		////////////////////////////////////////////////////////////////

		/// @brief RG 成分を `R32G32_Float` 形式で返します。
		/// @return `Float2{ static_cast<float>(r), static_cast<float>(g) }`
		[[nodiscard]]
		constexpr Float2 toR32G32_Float() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	toR32G32B32A32_Float
		//
		////////////////////////////////////////////////////////////////

		/// @brief RGBA 成分を `R32G32B32A32_Float` 形式で返します。
		/// @return `Float4{ static_cast<float>(r), static_cast<float>(g), static_cast<float>(b), static_cast<float>(a) }`
		[[nodiscard]]
		constexpr Float4 toR32G32B32A32_Float() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	PremultiplyAlpha
		//
		////////////////////////////////////////////////////////////////

		/// @brief アルファ乗算済みの色を返します。
		/// @param color 色
		/// @return RGB の各成分にアルファ値を乗算した色
		[[nodiscard]]
		static constexpr ColorF PremultiplyAlpha(ColorF color) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	UnpremultiplyAlpha
		//
		////////////////////////////////////////////////////////////////

		/// @brief アルファ乗算済みの色を通常の色に戻します。
		/// @param color アルファ乗算済みの色
		/// @return RGB の各成分をアルファ値で除算した色
		/// @remark アルファ値が 0.0 の場合は `ColorF::Zero()` を返します。
		[[nodiscard]]
		static constexpr ColorF UnpremultiplyAlpha(ColorF color) noexcept;

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

		/// @brief 入力ストリームから色を読み込みます。
		/// @param input 入力ストリーム
		/// @param value 読み込んだ色の格納先
		/// @return 入力ストリーム
		/// @remark `(r, g, b)`, `(r, g, b, a)`, 16 進数カラーコードの形式に対応します。
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

		/// @brief 色を文字列に変換します。
		/// @param formatData 文字列バッファ
		/// @param value 色
		/// @remark この関数は Format 用の関数です。通常、ユーザーが直接呼び出す必要はありません。
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

	format_context::iterator format(const s3d::ColorF& value, format_context& ctx) const;
};

template <>
struct fmt::formatter<s3d::ColorF, s3d::char32>
{
	std::u32string tag;

	s3d::ParseContext::iterator parse(s3d::ParseContext& ctx);

	s3d::BufferContext::iterator format(const s3d::ColorF& value, s3d::BufferContext& ctx) const;
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
