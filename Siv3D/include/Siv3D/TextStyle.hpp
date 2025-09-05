//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2025 Ryo Suzuki
//	Copyright (c) 2016-2025 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once
# include "Common.hpp"
# include "PointVector.hpp"
# include "ColorHSV.hpp"
# include "ReadingDirection.hpp"

namespace s3d
{
	/// @brief テキストのスタイル
	struct TextStyle
	{
		Float4 param{ 0.5f, 0.5f, 0.0f, 0.0f };

		Float4 outlineColor{ 0.0f, 0.0f, 0.0f, 1.0f };

		Float4 shadowColor{ 0.0f, 0.0f, 0.0f, 0.5f };

		enum class Type : uint8
		{
			Default,
			Outline,
			Shadow,
			OutlineShadow,
			Glow,
			CustomTextFontShader,
			CustomColorFontShader,
		} type = Type::Default;

		double lineSpacing = 1.0;

		double characterSpacing = 0.0;

		/// @brief デフォルトのスタイルを返します。
		/// @return デフォルトのスタイル
		[[nodiscard]]
		static constexpr TextStyle Default() noexcept;

		/// @brief カスタムテキストフォントシェーダ用のスタイルを返します。
		/// @return カスタムテキストフォントシェーダ用のスタイル
		[[nodiscard]]
		static constexpr TextStyle CustomTextFontShader() noexcept;

		/// @brief カスタムカラーフォントシェーダ用のスタイルを返します。
		/// @return カスタムカラーフォントシェーダ用のスタイル
		[[nodiscard]]
		static constexpr TextStyle CustomColorFontShader() noexcept;

		/// @brief 輪郭付きのスタイルを返します。
		/// @param p 輪郭の太さを示すパラメータ
		/// @param color 輪郭の色
		/// @return 輪郭付きのスタイル
		[[nodiscard]]
		static constexpr TextStyle Outline(double p, const ColorF& color = Palette::Black) noexcept;

		/// @brief 輪郭付きのスタイルを返します。
		/// @param inner 内側の輪郭の太さを示すパラメータ
		/// @param outer 外側の輪郭の太さを示すパラメータ
		/// @param color 輪郭の色
		/// @return 輪郭付きのスタイル
		[[nodiscard]]
		static constexpr TextStyle Outline(double inner, double outer, const ColorF& color = Palette::Black) noexcept;

		/// @brief シャドウ付きのスタイルを返します。
		/// @param offset シャドウのオフセットを示すパラメータ
		/// @param color シャドウの色
		/// @return シャドウ付きのスタイル
		[[nodiscard]]
		static constexpr TextStyle Shadow(const Vec2& offset, const ColorF& color = Palette::Black) noexcept;

		/// @brief 輪郭とシャドウ付きのスタイルを返します。
		/// @param p 輪郭の太さを示すパラメータ
		/// @param outlineColor 輪郭の色
		/// @param offset シャドウのオフセットを示すパラメータ
		/// @param shadowColor シャドウの色
		/// @return 輪郭とシャドウ付きのスタイル
		[[nodiscard]]
		static constexpr TextStyle OutlineShadow(double p, const ColorF& outlineColor, const Vec2& offset, const ColorF& shadowColor) noexcept;

		/// @brief 輪郭とシャドウ付きのスタイルを返します。
		/// @param inner 内側の輪郭の太さを示すパラメータ
		/// @param outer 外側の輪郭の太さを示すパラメータ
		/// @param outlineColor 輪郭の色
		/// @param offset シャドウのオフセットを示すパラメータ
		/// @param shadowColor シャドウの色
		/// @return 輪郭とシャドウ付きのスタイル
		[[nodiscard]]
		static constexpr TextStyle OutlineShadow(double inner, double outer, const ColorF& outlineColor, const Vec2& offset, const ColorF& shadowColor) noexcept;
		
		/// @brief 輪郭とシャドウ付きのスタイルを返します。
		/// @param p 輪郭の太さを示すパラメータ
		/// @param offset シャドウのオフセットを示すパラメータ
		/// @param outlineColor 輪郭の色
		/// @param shadowColor シャドウの色
		/// @return 輪郭とシャドウ付きのスタイル
		[[nodiscard]]
		static constexpr TextStyle OutlineShadow(double p, const Vec2& offset, const ColorF& outlineColor = Palette::Black, const ColorF& shadowColor = Palette::Black) noexcept;

		/// @brief 輪郭とシャドウ付きのスタイルを返します。
		/// @param inner 内側の輪郭の太さを示すパラメータ
		///	@param outer 外側の輪郭の太さを示すパラメータ
		/// @param offset シャドウのオフセットを示すパラメータ
		/// @param outlineColor 輪郭の色
		/// @param shadowColor シャドウの色
		/// @return 輪郭とシャドウ付きのスタイル
		[[nodiscard]]
		static constexpr TextStyle OutlineShadow(double inner, double outer, const Vec2& offset, const ColorF& outlineColor = Palette::Black, const ColorF& shadowColor = Palette::Black) noexcept;

		/// @brief グロー付きのスタイルを返します。
		/// @param p グローの大きさを示すパラメータ
		/// @return グロー付きのスタイル
		[[nodiscard]]
		static constexpr TextStyle Glow(double p) noexcept;
	};
}

# include "detail/TextStyle.ipp"
