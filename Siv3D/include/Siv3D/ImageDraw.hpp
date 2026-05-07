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
# include "ImagePixel.hpp"
# include "2DShapesFwd.hpp"

namespace s3d
{
    namespace ImageDraw
    {
        /// @brief 図形のアンチエイリアス設定
        enum class Antialiasing : uint8
        {
            No,
            Yes,
        };

        /// @brief 描画先 alpha の扱い
        enum class DstAlpha : uint8
        {
            /// @brief dst の alpha を一般の straight alpha として扱います。
            Preserve,

            /// @brief dst の alpha が常に 255 であると仮定します。
            /// @remark 高速化のため、実行時には確認しません。
            /// @remark 仮定を満たさない場合の結果は保証されません。
            AssumeOpaque,
        };

        /// @brief 線分端点の形状
        enum class LineCap : uint8
        {
            /// @brief 線分の端点で平らに切ります。
            Flat,

            /// @brief 線分方向に半径分だけ延長して、端を四角くします。
            Square,

            /// @brief 線分の端を丸くします。
            Round,
        };

        //-------------------------------------------------------------------------
        //
        //  Filled shapes
        //
        //-------------------------------------------------------------------------

        /// @brief 整数座標の長方形を描画します。
        /// @param image 描画先画像
        /// @param rect 描画する長方形
        /// @param color 描画色
        /// @param blendMode 合成方式
        /// @param dstAlpha 描画先 alpha の扱い
        /// @remark 画像の範囲外はクリップされます。
        void Fill(
            Image& image,
            const Rect& rect,
            Color color,
            ImagePixel::BlendMode blendMode = ImagePixel::BlendMode::SourceOver,
            DstAlpha dstAlpha = DstAlpha::Preserve);

        /// @brief 浮動小数点座標の長方形を描画します。
        /// @param image 描画先画像
        /// @param rect 描画する長方形
        /// @param color 描画色
        /// @param blendMode 合成方式
        /// @param dstAlpha 描画先 alpha の扱い
        /// @remark 辺の小数部分は coverage に変換されます。
        /// @remark 画像の範囲外はクリップされます。
        void Fill(
            Image& image,
            const RectF& rect,
            Color color,
            ImagePixel::BlendMode blendMode = ImagePixel::BlendMode::SourceOver,
            DstAlpha dstAlpha = DstAlpha::Preserve);

        /// @brief 円を描画します。
        /// @param image 描画先画像
        /// @param circle 描画する円
        /// @param color 描画色
        /// @param blendMode 合成方式
        /// @param antialiasing アンチエイリアス設定
        /// @param dstAlpha 描画先 alpha の扱い
        /// @remark アンチエイリアスありでは距離ベースの coverage を使用します。
        /// @remark 実行時性能を優先した近似です。
        /// @remark 画像の範囲外はクリップされます。
        void Fill(
            Image& image,
            const Circle& circle,
            Color color,
            ImagePixel::BlendMode blendMode = ImagePixel::BlendMode::SourceOver,
            Antialiasing antialiasing = Antialiasing::Yes,
            DstAlpha dstAlpha = DstAlpha::Preserve);

        //-------------------------------------------------------------------------
        //
        //  Lines
        //
        //-------------------------------------------------------------------------

        /// @brief 太さのある線分を描画します。
        /// @param image 描画先画像
        /// @param from 始点
        /// @param to 終点
        /// @param thickness 線の太さ
        /// @param color 描画色
        /// @param blendMode 合成方式
        /// @param antialiasing アンチエイリアス設定
        /// @param lineCap 線端の形状
        /// @param dstAlpha 描画先 alpha の扱い
        /// @remark 非 AA では span ベースで描画します。
        /// @remark AA ありでは距離ベースの coverage を使用します。
        /// @remark Round cap では本体と cap の coverage を統合してから 1 回だけ合成します。
        /// @remark 画像の範囲外はクリップされます。
        void Line(
            Image& image,
            const Vec2& from,
            const Vec2& to,
            double thickness,
            Color color,
            ImagePixel::BlendMode blendMode = ImagePixel::BlendMode::SourceOver,
            Antialiasing antialiasing = Antialiasing::Yes,
            LineCap lineCap = LineCap::Round,
            DstAlpha dstAlpha = DstAlpha::Preserve);

        ////-------------------------------------------------------------------------
        ////
        ////  Convenience wrappers
        ////
        ////-------------------------------------------------------------------------

        //inline void Overwrite(Image& image, const Rect& rect, Color color)
        //{
        //    Fill(image, rect, color, ImagePixel::BlendMode::Overwrite, DstAlpha::Preserve);
        //}

        //inline void SourceOver(Image& image, const Rect& rect, Color color)
        //{
        //    Fill(image, rect, color, ImagePixel::BlendMode::SourceOver, DstAlpha::Preserve);
        //}

        //inline void SourceOverAssumeOpaqueDst(Image& image, const Rect& rect, Color color)
        //{
        //    Fill(image, rect, color, ImagePixel::BlendMode::SourceOver, DstAlpha::AssumeOpaque);
        //}

        //inline void Overwrite(Image& image, const RectF& rect, Color color)
        //{
        //    Fill(image, rect, color, ImagePixel::BlendMode::Overwrite, DstAlpha::Preserve);
        //}

        //inline void SourceOver(Image& image, const RectF& rect, Color color)
        //{
        //    Fill(image, rect, color, ImagePixel::BlendMode::SourceOver, DstAlpha::Preserve);
        //}

        //inline void SourceOverAssumeOpaqueDst(Image& image, const RectF& rect, Color color)
        //{
        //    Fill(image, rect, color, ImagePixel::BlendMode::SourceOver, DstAlpha::AssumeOpaque);
        //}

        //inline void Overwrite(
        //    Image& image,
        //    const Circle& circle,
        //    Color color,
        //    Antialiasing antialiasing = Antialiasing::Yes)
        //{
        //    Fill(image, circle, color, ImagePixel::BlendMode::Overwrite, antialiasing, DstAlpha::Preserve);
        //}

        //inline void SourceOver(
        //    Image& image,
        //    const Circle& circle,
        //    Color color,
        //    Antialiasing antialiasing = Antialiasing::Yes)
        //{
        //    Fill(image, circle, color, ImagePixel::BlendMode::SourceOver, antialiasing, DstAlpha::Preserve);
        //}

        //inline void SourceOverAssumeOpaqueDst(
        //    Image& image,
        //    const Circle& circle,
        //    Color color,
        //    Antialiasing antialiasing = Antialiasing::Yes)
        //{
        //    Fill(image, circle, color, ImagePixel::BlendMode::SourceOver, antialiasing, DstAlpha::AssumeOpaque);
        //}

        //inline void Overwrite(
        //    Image& image,
        //    const Vec2& from,
        //    const Vec2& to,
        //    double thickness,
        //    Color color,
        //    Antialiasing antialiasing = Antialiasing::Yes,
        //    LineCap lineCap = LineCap::Round)
        //{
        //    Line(image, from, to, thickness, color, ImagePixel::BlendMode::Overwrite, antialiasing, lineCap, DstAlpha::Preserve);
        //}

        //inline void SourceOver(
        //    Image& image,
        //    const Vec2& from,
        //    const Vec2& to,
        //    double thickness,
        //    Color color,
        //    Antialiasing antialiasing = Antialiasing::Yes,
        //    LineCap lineCap = LineCap::Round)
        //{
        //    Line(image, from, to, thickness, color, ImagePixel::BlendMode::SourceOver, antialiasing, lineCap, DstAlpha::Preserve);
        //}

        //inline void SourceOverAssumeOpaqueDst(
        //    Image& image,
        //    const Vec2& from,
        //    const Vec2& to,
        //    double thickness,
        //    Color color,
        //    Antialiasing antialiasing = Antialiasing::Yes,
        //    LineCap lineCap = LineCap::Round)
        //{
        //    Line(image, from, to, thickness, color, ImagePixel::BlendMode::SourceOver, antialiasing, lineCap, DstAlpha::AssumeOpaque);
        //}
    }
}
