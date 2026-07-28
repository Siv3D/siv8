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
# include <bit>
# include "Common.hpp"
# include "Utility.hpp"
# include "StringView.hpp"
# include "Char.hpp"
# include "FormatLiteral.hpp"

namespace s3d
{
	struct ColorF;
	struct HSV;
	class HalfFloat;
	template <Concept::FloatingPoint Float> struct Vector2D;
	using Float2 = Vector2D<float>;
	template <Concept::FloatingPoint Float> struct Vector4D;
	using Float4 = Vector4D<float>;

	////////////////////////////////////////////////////////////////
	//
	//	Color
	//
	////////////////////////////////////////////////////////////////

	/// @brief 各成分を 8-bit 符号なし整数で表現する RGBA カラー
	struct Color
	{
		/// @brief 各色成分の型
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
		/// @remark 各成分は初期化されません。
		[[nodiscard]]
		Color() = default;

		/// @brief コピーコンストラクタ
		/// @param color コピーする色
		[[nodiscard]]
		Color(const Color& color) = default;

		/// @brief Color を作成します。
		/// @param rgb RGB 成分 [0, 255]
		[[nodiscard]]
		explicit constexpr Color(value_type rgb) noexcept;

		/// @brief Color を作成します。
		/// @param rgb RGB 成分 [0, 255]
		[[nodiscard]]
		explicit constexpr Color(Concept::Integral auto rgb) noexcept;

		/// @brief Color を作成します。
		/// @param rgb RGB 成分 [0, 255]
		/// @param _a アルファ値 [0, 255]
		[[nodiscard]]
		constexpr Color(value_type rgb, value_type _a) noexcept;

		/// @brief Color を作成します。
		/// @param rgb RGB 成分 [0, 255]
		/// @param _a アルファ値 [0, 255]
		[[nodiscard]]
		constexpr Color(Concept::Integral auto rgb, Concept::Integral auto _a) noexcept;

		/// @brief Color を作成します。
		/// @param _r 赤成分 [0, 255]
		/// @param _g 緑成分 [0, 255]
		/// @param _b 青成分 [0, 255]
		[[nodiscard]]
		constexpr Color(value_type _r, value_type _g, value_type _b) noexcept;

		/// @brief Color を作成します。
		/// @param _r 赤成分 [0, 255]
		/// @param _g 緑成分 [0, 255]
		/// @param _b 青成分 [0, 255]
		[[nodiscard]]
		constexpr Color(Concept::Integral auto _r, Concept::Integral auto _g, Concept::Integral auto _b) noexcept;

		/// @brief Color を作成します。
		/// @param _r 赤成分 [0, 255]
		/// @param _g 緑成分 [0, 255]
		/// @param _b 青成分 [0, 255]
		/// @param _a アルファ値 [0, 255]
		[[nodiscard]]
		constexpr Color(value_type _r, value_type _g, value_type _b, value_type _a) noexcept;

		/// @brief Color を作成します。
		/// @param _r 赤成分 [0, 255]
		/// @param _g 緑成分 [0, 255]
		/// @param _b 青成分 [0, 255]
		/// @param _a アルファ値 [0, 255]
		[[nodiscard]]
		constexpr Color(Concept::Integral auto _r, Concept::Integral auto _g, Concept::Integral auto _b, Concept::Integral auto _a) noexcept;

		/// @brief 別の Color から Color を作成します。
		/// @param rgb Color
		/// @param _a アルファ値 [0, 255]
		/// @remark `Color{ rgb.r, rgb.g, rgb.b, _a }` と同じです。
		[[nodiscard]]
		constexpr Color(Color rgb, value_type _a) noexcept;

		/// @brief 別の Color から Color を作成します。
		/// @param rgb Color
		/// @param _a アルファ値 [0, 255]
		/// @remark `Color{ rgb.r, rgb.g, rgb.b, _a }` と同じです。
		[[nodiscard]]
		constexpr Color(Color rgb, Concept::Integral auto _a) noexcept;

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

		/// @brief ColorF から Color を作成します。
		/// @param color ColorF
		/// @param _a アルファ値 [0, 255]
		/// @remark 引数 `color` のアルファ値は無視されます。
		[[nodiscard]]
		constexpr Color(const ColorF& color, Concept::Integral auto _a) noexcept;

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

		/// @brief HSV から Color を作成します。
		/// @param hsv HSV
		/// @param _a アルファ値 [0, 255]
		/// @remark 引数 `hsv` のアルファ値は無視されます。
		[[nodiscard]]
		Color(const HSV& hsv, Concept::Integral auto _a) noexcept;

		/// @brief 16 進数のカラーコードから Color を作成します。
		/// @param code 16 進数のカラーコード
		/// @remark `#RGB`, `#RGBA`, `RRGGBB`, `#RRGGBB`, `RRGGBBAA`, `#RRGGBBAA` に対応します。
		/// @remark 形式に一致しない場合は `Color{ 0, 0, 0, 255 }` を作成します。
		[[nodiscard]]
		explicit constexpr Color(StringView code) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator =
		//
		////////////////////////////////////////////////////////////////

		/// @brief Color を代入します。
		/// @param color 代入する色
		/// @return *this
		constexpr Color& operator =(const Color& color) noexcept = default;

		/// @brief ColorF を変換して代入します。
		/// @param color 代入する色
		/// @return *this
		constexpr Color& operator =(const ColorF& color) noexcept;

		/// @brief HSV を変換して代入します。
		/// @param hsva 代入する色
		/// @return *this
		Color& operator =(const HSV& hsva) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator ~
		//
		////////////////////////////////////////////////////////////////

		/// @brief 色の各成分を反転した色を返します。
		/// @return 色の各成分を反転した色
		/// @remark `Color{ (255 - r), (255 - g), (255 - b), a }` を返します。
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

		/// @brief R 成分のみを変更した自身のコピーを返します。
		/// @param _r 新しい R 成分 [0, 255]
		/// @return R 成分を変更したコピー
		[[nodiscard]]
		constexpr Color withR(uint32 _r) const noexcept;

		/// @brief G 成分のみを変更した自身のコピーを返します。
		/// @param _g 新しい G 成分 [0, 255]
		/// @return G 成分を変更したコピー
		[[nodiscard]]
		constexpr Color withG(uint32 _g) const noexcept;

		/// @brief B 成分のみを変更した自身のコピーを返します。
		/// @param _b 新しい B 成分 [0, 255]
		/// @return B 成分を変更したコピー
		[[nodiscard]]
		constexpr Color withB(uint32 _b) const noexcept;

		/// @brief アルファ値のみを変更した自身のコピーを返します。
		/// @param _a 新しい アルファ値 [0, 255]
		/// @return アルファ値を変更したコピー
		[[nodiscard]]
		constexpr Color withA(uint32 _a) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	withAlpha
		//
		////////////////////////////////////////////////////////////////

		/// @brief アルファ値のみを変更した自身のコピーを返します。
		/// @param _a 新しい アルファ値
		/// @return アルファ値を変更したコピー
		/// @remark `.withA(_a)` と同じです。
		[[nodiscard]]
		constexpr Color withAlpha(uint32 _a) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	setR, setG, setB, setA
		//
		////////////////////////////////////////////////////////////////

		/// @brief R 成分を変更します。
		/// @param _r 新しい R 成分 [0, 255]
		/// @return *this
		constexpr Color& setR(uint32 _r) noexcept;

		/// @brief G 成分を変更します。
		/// @param _g 新しい G 成分 [0, 255]
		/// @return *this
		constexpr Color& setG(uint32 _g) noexcept;

		/// @brief B 成分を変更します。
		/// @param _b 新しい B 成分 [0, 255]
		/// @return *this
		constexpr Color& setB(uint32 _b) noexcept;

		/// @brief アルファ値を変更します。
		/// @param _a 新しいアルファ値 [0, 255]
		/// @return *this
		constexpr Color& setA(uint32 _a) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	setRGB
		//
		////////////////////////////////////////////////////////////////

		/// @brief RGB 成分を変更します。
		/// @param rgb 新しい RGB 成分 [0, 255]
		/// @return *this
		constexpr Color& setRGB(uint32 rgb) noexcept;

		/// @brief RGB 成分を変更します。
		/// @param _r 新しい R 成分 [0, 255]
		/// @param _g 新しい G 成分 [0, 255]
		/// @param _b 新しい B 成分 [0, 255]
		/// @return *this
		constexpr Color& setRGB(uint32 _r, uint32 _g, uint32 _b) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	set
		//
		////////////////////////////////////////////////////////////////

		/// @brief 色を変更します。
		/// @param rgb 新しい RGB 成分 [0, 255]
		/// @param _a 新しいアルファ値 [0, 255]
		/// @return *this
		constexpr Color& set(uint32 rgb, uint32 _a = 255) noexcept;

		/// @brief 色を変更します。
		/// @param _r 新しい R 成分 [0, 255]
		/// @param _g 新しい G 成分 [0, 255]
		/// @param _b 新しい B 成分 [0, 255]
		/// @param _a 新しいアルファ値
		/// @return *this
		constexpr Color& set(uint32 _r, uint32 _g, uint32 _b, uint32 _a = 255) noexcept;

		/// @brief 色を変更します。
		/// @param color 新しい色
		/// @return *this
		constexpr Color& set(Color color) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	premultiplied
		//
		////////////////////////////////////////////////////////////////

		/// @brief アルファ乗算済みの色を返します。
		/// @return アルファ乗算済みの色
		[[nodiscard]]
		constexpr Color premultiplied() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	unpremultiplied
		//
		////////////////////////////////////////////////////////////////

		/// @brief アルファ乗算済みの色を通常の色に戻します。
		/// @return アルファ乗算を解除した色
		/// @remark アルファ値が 0 の場合は `Color::Zero()` を返します。
		[[nodiscard]]
		constexpr Color unpremultiplied() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	grayscale8
		//
		////////////////////////////////////////////////////////////////

		/// @brief グレースケール値を [0, 255] の範囲で返します。
		/// @return グレースケール値 [0, 255]
		/// @remark `(0.299 * r) + (0.587 * g) + (0.114 * b)` によって計算されます。
		[[nodiscard]]
		constexpr uint8 grayscale8() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	grayscale
		//
		////////////////////////////////////////////////////////////////

		/// @brief グレースケール値を [0.0, 1.0] の範囲で返します。
		/// @return グレースケール値 [0.0, 1.0]
		/// @remark `((0.299 * r) + (0.587 * g) + (0.114 * b)) / 255` によって計算されます。
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
		constexpr uint8 minRGBComponent() const noexcept;

		/// @brief RGB 成分のうち最大の値を返します。
		/// @return RGB 成分のうち最大の値
		[[nodiscard]]
		constexpr uint8 maxRGBComponent() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	minComponent, maxComponent
		//
		////////////////////////////////////////////////////////////////

		/// @brief RGBA 成分のうち最小の値を返します。
		/// @return RGBA 成分のうち最小の値
		[[nodiscard]]
		constexpr uint8 minComponent() const noexcept;

		/// @brief RGBA 成分のうち最大の値を返します。
		/// @return RGBA 成分のうち最大の値
		[[nodiscard]]
		constexpr uint8 maxComponent() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	asUint32
		//
		////////////////////////////////////////////////////////////////

		/// @brief Color のビット列を 32-bit 整数として解釈した値を返します。
		/// @return `0xAABBGGRR` の形式の値
		[[nodiscard]]
		constexpr uint32 asUint32() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	toRGBA8888
		//
		////////////////////////////////////////////////////////////////

		/// @brief RGBA 成分を 32-bit 整数に変換して返します。
		/// @return `0xRRGGBBAA` の形式の値
		[[nodiscard]]
		constexpr uint32 toRGBA8888() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	toABGR8888
		//
		////////////////////////////////////////////////////////////////

		/// @brief ABGR の順に並べた成分を 32-bit 整数に変換して返します。
		/// @return `0xAABBGGRR` の形式の値
		[[nodiscard]]
		constexpr uint32 toABGR8888() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	lerp
		//
		////////////////////////////////////////////////////////////////

		/// @brief 2 つの色の間を線形補間した色を返します。
		/// @param other もう一方の色
		/// @param f 補間係数。[0.0, 1.0] の範囲にクランプされます。
		/// @return 補間された色
		[[nodiscard]]
		constexpr Color lerp(Color other, double f) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	lightened, darkened
		//
		////////////////////////////////////////////////////////////////

		/// @brief 色を明るくした色を返します。
		/// @param amount 明るくする量 [0.0, 1.0]
		/// @return 明るくした色
		/// @remark `Color{ (r + (255 - r) * amount), (g + (255 - g) * amount), (b + (255 - b) * amount), a }` を返します。
		[[nodiscard]]
		constexpr Color lightened(double amount) const noexcept;

		/// @brief 色を暗くした色を返します。
		/// @param amount 暗くする量 [0.0, 1.0]
		/// @return 暗くした色
		/// @remark `Color{ (r * (1.0 - amount)), (g * (1.0 - amount)), (b * (1.0 - amount)), a }` を返します。
		[[nodiscard]]
		constexpr Color darkened(double amount) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	gamma
		//
		////////////////////////////////////////////////////////////////

		/// @brief 色をガンマ補正した色を返します。
		/// @param gamma ガンマ値。0.0 以下の場合、RGB 成分はすべて 0 になります。
		/// @return ガンマ補正された色
		[[nodiscard]]
		Color gamma(double gamma) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	hueShifted
		//
		////////////////////////////////////////////////////////////////

		/// @brief 色相をシフトした色を返します。
		/// @param degrees 色相のシフト量（度）
		/// @return 色相をシフトした色
		[[nodiscard]]
		Color hueShifted(double degrees) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	inverted
		//
		////////////////////////////////////////////////////////////////

		/// @brief 反転した色を返します。
		/// @return 反転した色
		/// @remark `Color{ (255 - r), (255 - g), (255 - b), a }` を返します。
		/// @remark `operator ~` と同じです。
		[[nodiscard]]
		constexpr Color inverted() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	complemented
		//
		////////////////////////////////////////////////////////////////

		/// @brief 補色を返します。
		/// @return 補色
		[[nodiscard]]
		Color complemented() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	srgbToLinear, linearToSRGB
		//
		////////////////////////////////////////////////////////////////

		/// @brief sRGB 色空間からリニア色空間に変換した色を返します。
		/// @return リニア色空間に変換した色
		/// @remark アルファ値は変更しません。
		[[nodiscard]]
		ColorF srgbToLinear() const noexcept;

		/// @brief リニア色空間から sRGB 色空間に変換した色を返します。
		/// @return sRGB 色空間に変換した色
		/// @remark アルファ値は変更しません。
		[[nodiscard]]
		ColorF linearToSRGB() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	toHexRGB, toHexRGBA
		//
		////////////////////////////////////////////////////////////////

		/// @brief 16 進数のカラーコード RRGGBB を返します。
		/// @return 16 進数のカラーコード RRGGBB
		/// @remark アルファ値は出力されません。
		[[nodiscard]]
		String toHexRGB() const;

		/// @brief 16 進数のカラーコード RRGGBBAA を返します。
		/// @return 16 進数のカラーコード RRGGBBAA
		[[nodiscard]]
		String toHexRGBA() const;

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
		//	abgr
		//
		////////////////////////////////////////////////////////////////

		/// @brief Color{ a, b, g, r } を返します。
		/// @return Color{ a, b, g, r }
		[[nodiscard]]
		constexpr Color abgr() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	toHSV
		//
		////////////////////////////////////////////////////////////////

		/// @brief 色を HSV 表色系に変換します。
		/// @return 変換された HSV
		[[nodiscard]]
		HSV toHSV() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	toFloat4
		//
		////////////////////////////////////////////////////////////////

		/// @brief RGBA 成分を [0.0, 1.0] に正規化した Float4 を返します。
		/// @return `Float4{ (r / 255.0f), (g / 255.0f), (b / 255.0f), (a / 255.0f) }`
		[[nodiscard]]
		Float4 toFloat4() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	toR8_Unorm
		//
		////////////////////////////////////////////////////////////////

		/// @brief R 成分を `R8_Unorm` 形式で返します。
		/// @return 下位 8 bit に R 成分を格納した値
		[[nodiscard]]
		constexpr uint8 toR8_Unorm() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	toR8G8_Unorm
		//
		////////////////////////////////////////////////////////////////

		/// @brief RG 成分を `R8G8_Unorm` 形式で返します。
		/// @return bit 0-7 に R、bit 8-15 に G を格納した値
		[[nodiscard]]
		constexpr uint16 toR8G8_Unorm() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	toR16_Float
		//
		////////////////////////////////////////////////////////////////

		/// @brief R 成分を [0.0, 1.0] に正規化した `R16_Float` 形式で返します。
		/// @return R 成分を格納した半精度浮動小数点数
		[[nodiscard]]
		HalfFloat toR16_Float() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	toR8G8B8A8_Unorm
		//
		////////////////////////////////////////////////////////////////

		/// @brief RGBA 成分を `R8G8B8A8_Unorm` 形式で返します。
		/// @return *this
		[[nodiscard]]
		constexpr Color toR8G8B8A8_Unorm() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	toR16G16_Unorm
		//
		////////////////////////////////////////////////////////////////

		/// @brief RG 成分を [0, 65535] に正規化した `R16G16_Unorm` 形式で返します。
		/// @return bit 0-15 に R、bit 16-31 に G を格納した値
		[[nodiscard]]
		constexpr uint32 toR16G16_Unorm() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	toR16G16_Float
		//
		////////////////////////////////////////////////////////////////

		/// @brief RG 成分を [0.0, 1.0] に正規化した `R16G16_Float` 形式で返します。
		/// @return bit 0-15 に R、bit 16-31 に G の半精度浮動小数点数を格納した値
		[[nodiscard]]
		uint32 toR16G16_Float() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	toR32_Float
		//
		////////////////////////////////////////////////////////////////

		/// @brief R 成分を [0.0, 1.0] に正規化した `R32_Float` 形式で返します。
		/// @return `(r / 255.0f)`
		[[nodiscard]]
		constexpr float toR32_Float() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	toR10G10B10A2_Unorm
		//
		////////////////////////////////////////////////////////////////

		/// @brief RGBA 成分を `R10G10B10A2_Unorm` 形式で返します。
		/// @return bit 0-9 に R、bit 10-19 に G、bit 20-29 に B、bit 30-31 に A を格納した値
		[[nodiscard]]
		constexpr uint32 toR10G10B10A2_Unorm() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	toR11G11B10_UFloat
		//
		////////////////////////////////////////////////////////////////

		/// @brief RGB 成分を `R11G11B10_UFloat` 形式で返します。
		/// @return bit 0-10 に R、bit 11-21 に G、bit 22-31 に B を格納した値
		/// @remark 各成分を [0.0, 1.0] に正規化してから、符号なし浮動小数点数に変換します。
		[[nodiscard]]
		uint32 toR11G11B10_UFloat() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	toR16G16B16A16_Float
		//
		////////////////////////////////////////////////////////////////

		/// @brief RGBA 成分を [0.0, 1.0] に正規化した `R16G16B16A16_Float` 形式で返します。
		/// @return 下位から R, G, B, A の順に半精度浮動小数点数を格納した値
		[[nodiscard]]
		uint64 toR16G16B16A16_Float() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	toR32G32_Float
		//
		////////////////////////////////////////////////////////////////

		/// @brief RG 成分を [0.0, 1.0] に正規化した `R32G32_Float` 形式で返します。
		/// @return `Float2{ (r / 255.0f), (g / 255.0f) }`
		[[nodiscard]]
		Float2 toR32G32_Float() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	toR32G32B32A32_Float
		//
		////////////////////////////////////////////////////////////////

		/// @brief RGBA 成分を [0.0, 1.0] に正規化した `R32G32B32A32_Float` 形式で返します。
		/// @return `toFloat4()` と同じ値
		[[nodiscard]]
		Float4 toR32G32B32A32_Float() const noexcept;

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
		//	FromFloat
		//
		////////////////////////////////////////////////////////////////

		/// @brief 浮動小数点数を [0.0, 1.0] にクランプしたあと Color に変換します。
		/// @param rgb 浮動小数点数
		/// @return 変換された Color
		[[nodiscard]]
		static constexpr Color FromFloat(float rgb) noexcept;

		/// @brief 浮動小数点数を [0.0, 1.0] にクランプしたあと Color に変換します。
		/// @param rgb 浮動小数点数
		/// @return 変換された Color
		[[nodiscard]]
		static constexpr Color FromFloat(double rgb) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	FromRGBA8888
		//
		////////////////////////////////////////////////////////////////

		/// @brief `0xRRGGBBAA` の形式の 32-bit 整数から Color を作成します。
		/// @param rgba `0xRRGGBBAA` の形式の値
		/// @return 変換された Color
		[[nodiscard]]
		static constexpr Color FromRGBA8888(uint32 rgba) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	FromABGR8888
		//
		////////////////////////////////////////////////////////////////

		/// @brief `0xAABBGGRR` の形式の 32-bit 整数から Color を作成します。
		/// @param abgr `0xAABBGGRR` の形式の値
		/// @return 変換された Color
		[[nodiscard]]
		static constexpr Color FromABGR8888(uint32 abgr) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	PremultiplyAlpha
		//
		////////////////////////////////////////////////////////////////

		/// @brief 色の RGB 成分にアルファ値を乗算します。
		/// @param color 色
		/// @return アルファ乗算済みの色
		[[nodiscard]]
		static constexpr Color PremultiplyAlpha(Color color) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	UnpremultiplyAlpha
		//
		////////////////////////////////////////////////////////////////

		/// @brief アルファ乗算済みの色を通常の色に戻します。
		/// @param color アルファ乗算済みの色
		/// @return アルファ乗算を解除した色
		/// @remark アルファ値が 0 の場合は `Color::Zero()` を返します。
		[[nodiscard]]
		static constexpr Color UnpremultiplyAlpha(Color color) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	Div255Round
		//
		////////////////////////////////////////////////////////////////

		/// @brief 非負整数を 255 で割り、最も近い整数に丸めます。
		/// @param x 非負整数
		/// @return `x / 255` を最も近い整数に丸めた値
		/// @remark `x` は [0, 65025] の範囲で指定してください。
		[[nodiscard]]
		static constexpr uint8 Div255Round(uint32 x) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator <<
		//
		////////////////////////////////////////////////////////////////

		/// @brief 出力ストリームに色を出力します。
		/// @param output 出力ストリーム
		/// @param value 色
		/// @return 出力ストリーム
		template <class CharType>
		friend std::basic_ostream<CharType>& operator <<(std::basic_ostream<CharType>& output, const Color& value)
		{
			return output << CharType('(')
				<< static_cast<uint32>(value.r) << CharType(',') << CharType(' ')
				<< static_cast<uint32>(value.g) << CharType(',') << CharType(' ')
				<< static_cast<uint32>(value.b) << CharType(',') << CharType(' ')
				<< static_cast<uint32>(value.a) << CharType(')');
		}

		////////////////////////////////////////////////////////////////
		//
		//	operator >>
		//
		////////////////////////////////////////////////////////////////

		/// @brief 入力ストリームから色を読み込みます。
		/// @param input 入力ストリーム
		/// @param value 読み込み先の色
		/// @return 入力ストリーム
		/// @remark `(r, g, b)`, `(r, g, b, a)`, `#RGB`, `#RGBA`, `#RRGGBB`, `#RRGGBBAA` 形式に対応します。
		template <class CharType>
		friend std::basic_istream<CharType>& operator >>(std::basic_istream<CharType>& input, Color& value)
		{
			CharType unused{};
			input >> unused;

			if (unused == CharType('#'))
			{
				String code;
				input >> code;
				value = Color(U'#' + code);
			}
			else
			{
				uint32 cols[4]{};
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

		/// @brief 色を文字列に変換します。
		/// @param formatData 文字列バッファ
		/// @param value 色
		/// @remark この関数は Format 用の関数です。通常、ユーザーが直接呼び出す必要はありません。
		friend void Formatter(FormatData& formatData, const Color& value);
	};

	////////////////////////////////////////////////////////////////
	//
	//	Alpha
	//
	////////////////////////////////////////////////////////////////

	/// @brief Color{ 255, 255, 255, alpha } を返します。
	/// @param alpha アルファ値 [0, 255]
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
struct fmt::formatter<s3d::Color>
{
	std::string tag;

	constexpr auto parse(format_parse_context& ctx)
	{
		return s3d::FmtHelper::GetFormatTag(tag, ctx);
	}

	format_context::iterator format(const s3d::Color& value, format_context& ctx) const;
};

template <>
struct fmt::formatter<s3d::Color, s3d::char32>
{
	std::u32string tag;

	s3d::ParseContext::iterator parse(s3d::ParseContext& ctx);

	s3d::BufferContext::iterator format(const s3d::Color& value, s3d::BufferContext& ctx) const;
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
