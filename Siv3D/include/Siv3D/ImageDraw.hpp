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
# include "LineCap.hpp"
# include "2DShapesFwd.hpp"

namespace s3d
{
    namespace ImageDraw
    {
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

        /// @brief 整数座標の長方形の枠を描画します。
        /// @param image 描画先画像
        /// @param rect 基準となる長方形
        /// @param innerThickness rect の内側に描画する太さ
        /// @param outerThickness rect の外側に描画する太さ
        /// @param color 描画色
        /// @param blendMode 合成方式
        /// @param dstAlpha 描画先 alpha の扱い
        /// @remark 描画領域は rect を外側に outerThickness 拡張した長方形から、rect を内側に innerThickness 縮小した長方形を除いた領域です。
        /// @remark 画像の範囲外はクリップされます。
        /// @remark innerThickness または outerThickness が負の場合、この関数は何もしません。
        void RectFrame(
            Image& image,
            const Rect& rect,
            int32 innerThickness,
            int32 outerThickness,
            Color color,
            ImagePixel::BlendMode blendMode = ImagePixel::BlendMode::SourceOver,
            DstAlpha dstAlpha = DstAlpha::Preserve);

        /// @brief 浮動小数点座標の長方形の枠を描画します。
        /// @param image 描画先画像
        /// @param rect 基準となる長方形
        /// @param innerThickness rect の内側に描画する太さ
        /// @param outerThickness rect の外側に描画する太さ
        /// @param color 描画色
        /// @param blendMode 合成方式
        /// @param dstAlpha 描画先 alpha の扱い
        /// @remark 描画領域は rect を外側に outerThickness 拡張した長方形から、rect を内側に innerThickness 縮小した長方形を除いた領域です。
        /// @remark 小数部分は coverage に変換されます。
        /// @remark coverage は outer coverage から inner coverage を引いた値として 1 回だけ合成します。
        /// @remark 画像の範囲外はクリップされます。
        /// @remark innerThickness または outerThickness が負の場合、この関数は何もしません。
        void RectFrame(
            Image& image,
            const RectF& rect,
            double innerThickness,
            double outerThickness,
            Color color,
            ImagePixel::BlendMode blendMode = ImagePixel::BlendMode::SourceOver,
            DstAlpha dstAlpha = DstAlpha::Preserve);

        /// @brief 円を描画します。
        /// @param image 描画先画像
        /// @param circle 描画する円
        /// @param color 描画色
        /// @param blendMode 合成方式
        /// @param enableAntialiasing アンチエイリアス設定
        /// @param dstAlpha 描画先 alpha の扱い
        /// @remark アンチエイリアスありでは距離ベースの coverage を使用します。
        /// @remark 実行時性能を優先した近似です。
        /// @remark 画像の範囲外はクリップされます。
        void Fill(
            Image& image,
            const Circle& circle,
            Color color,
            ImagePixel::BlendMode blendMode = ImagePixel::BlendMode::SourceOver,
            EnableAntialiasing enableAntialiasing = EnableAntialiasing::Yes,
            DstAlpha dstAlpha = DstAlpha::Preserve);

        /// @brief 円の枠を描画します。
        /// @param image 描画先画像
        /// @param circle 基準となる円
        /// @param innerThickness circle の内側に描画する太さ
        /// @param outerThickness circle の外側に描画する太さ
        /// @param color 描画色
        /// @param blendMode 合成方式
        /// @param enableAntialiasing アンチエイリアス設定
        /// @param dstAlpha 描画先 alpha の扱い
        /// @remark 描画領域は circle を外側に outerThickness 拡張した円から、circle を内側に innerThickness 縮小した円を除いた領域です。
        /// @remark アンチエイリアスありでは距離ベースの coverage を使用します。
        /// @remark coverage は outer coverage から inner coverage を引いた値として 1 回だけ合成します。
        /// @remark 画像の範囲外はクリップされます。
        /// @remark innerThickness または outerThickness が負の場合、この関数は何もしません。
        void CircleFrame(
            Image& image,
            const Circle& circle,
            double innerThickness,
            double outerThickness,
            Color color,
            ImagePixel::BlendMode blendMode = ImagePixel::BlendMode::SourceOver,
            EnableAntialiasing enableAntialiasing = EnableAntialiasing::Yes,
            DstAlpha dstAlpha = DstAlpha::Preserve);

        /// @brief 楕円を描画します。
        /// @param image 描画先画像
        /// @param ellipse 描画する楕円
        /// @param color 描画色
        /// @param blendMode 合成方式
        /// @param enableAntialiasing アンチエイリアス設定
        /// @param dstAlpha 描画先 alpha の扱い
        /// @remark アンチエイリアスありでは楕円境界までの距離に基づく coverage を使用します。
        /// @remark 画像の範囲外はクリップされます。
        void Fill(
            Image& image,
            const Ellipse& ellipse,
            Color color,
            ImagePixel::BlendMode blendMode = ImagePixel::BlendMode::SourceOver,
            EnableAntialiasing enableAntialiasing = EnableAntialiasing::Yes,
            DstAlpha dstAlpha = DstAlpha::Preserve);

        /// @brief 楕円の枠を描画します。
        /// @param image 描画先画像
        /// @param ellipse 基準となる楕円
        /// @param innerThickness ellipse の内側に描画する太さ
        /// @param outerThickness ellipse の外側に描画する太さ
        /// @param color 描画色
        /// @param blendMode 合成方式
        /// @param enableAntialiasing アンチエイリアス設定
        /// @param dstAlpha 描画先 alpha の扱い
        /// @remark 描画領域は楕円境界からの符号付き距離が -innerThickness 以上 outerThickness 以下の領域です。
        /// @remark アンチエイリアスありでは楕円境界までの距離に基づく coverage を使用します。
        /// @remark coverage は外側 offset coverage から内側 offset coverage を引いた値として 1 回だけ合成します。
        /// @remark 画像の範囲外はクリップされます。
        /// @remark innerThickness または outerThickness が負の場合、この関数は何もしません。
        void EllipseFrame(
            Image& image,
            const Ellipse& ellipse,
            double innerThickness,
            double outerThickness,
            Color color,
            ImagePixel::BlendMode blendMode = ImagePixel::BlendMode::SourceOver,
            EnableAntialiasing enableAntialiasing = EnableAntialiasing::Yes,
            DstAlpha dstAlpha = DstAlpha::Preserve);

        /// @brief 三角形を描画します。
        /// @param image 描画先画像
        /// @param triangle 描画する三角形
        /// @param color 描画色
        /// @param blendMode 合成方式
        /// @param enableAntialiasing アンチエイリアス設定
        /// @param dstAlpha 描画先 alpha の扱い
        /// @remark 画像の範囲外はクリップされます。
        void Fill(
            Image& image,
            const Triangle& triangle,
            Color color,
            ImagePixel::BlendMode blendMode = ImagePixel::BlendMode::SourceOver,
            EnableAntialiasing enableAntialiasing = EnableAntialiasing::Yes,
            DstAlpha dstAlpha = DstAlpha::Preserve);

        /// @brief 四角形を描画します。
        /// @param image 描画先画像
        /// @param quad 描画する四角形
        /// @param color 描画色
        /// @param blendMode 合成方式
        /// @param enableAntialiasing アンチエイリアス設定
        /// @param dstAlpha 描画先 alpha の扱い
        /// @remark 画像の範囲外はクリップされます。
        void Fill(
            Image& image,
            const Quad& quad,
            Color color,
            ImagePixel::BlendMode blendMode = ImagePixel::BlendMode::SourceOver,
            EnableAntialiasing enableAntialiasing = EnableAntialiasing::Yes,
            DstAlpha dstAlpha = DstAlpha::Preserve);

        /// @brief 点列で表される polygon を描画します。
        /// @param image 描画先画像
        /// @param points polygon の頂点配列
        /// @param count polygon の頂点数
        /// @param color 描画色
        /// @param blendMode 合成方式
        /// @param enableAntialiasing アンチエイリアス設定
        /// @param dstAlpha 描画先 alpha の扱い
        /// @remark 頂点列の向き（時計回り / 反時計回り）は問いません。
        /// @remark winding rule は even-odd です。
        /// @remark 自己交差を含む場合も even-odd rule に従って塗りつぶされます。
        /// @remark 複数 contour や穴を持つ Polygon には Fill(Image&, const Polygon&, ...) を使用してください。
        /// @remark 画像の範囲外はクリップされます。
        /// @remark points が nullptr または count が 3 未満の場合、この関数は何もしません。
        void FillPolygon(
            Image& image,
            const Vec2* points,
            size_t count,
            Color color,
            ImagePixel::BlendMode blendMode = ImagePixel::BlendMode::SourceOver,
            EnableAntialiasing enableAntialiasing = EnableAntialiasing::Yes,
            DstAlpha dstAlpha = DstAlpha::Preserve);

        /// @brief Polygon を描画します。
        /// @param image 描画先画像
        /// @param polygon 描画する Polygon
        /// @param color 描画色
        /// @param blendMode 合成方式
        /// @param enableAntialiasing アンチエイリアス設定
        /// @param dstAlpha 描画先 alpha の扱い
        /// @remark Polygon の外周と穴を考慮して描画します。
        /// @remark winding rule は even-odd です。
        /// @remark 画像の範囲外はクリップされます。
        /// @remark polygon が空の場合、この関数は何もしません。
        void Fill(
            Image& image,
            const Polygon& polygon,
            Color color,
            ImagePixel::BlendMode blendMode = ImagePixel::BlendMode::SourceOver,
            EnableAntialiasing enableAntialiasing = EnableAntialiasing::Yes,
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
        /// @param enableAntialiasing アンチエイリアス設定
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
            EnableAntialiasing enableAntialiasing = EnableAntialiasing::Yes,
            LineCap lineCap = LineCap::Round,
            DstAlpha dstAlpha = DstAlpha::Preserve);
    }
}
