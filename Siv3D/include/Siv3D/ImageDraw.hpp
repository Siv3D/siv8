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
# include "JoinStyle.hpp"
# include "2DShapesFwd.hpp"
# include "Polygon.hpp"

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
		/// @remark アンチエイリアスを無効にすると、中心座標と半径を整数に丸めて描画します。
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

		/// @brief 円の扇形を描画します。
		/// @param image 描画先画像
		/// @param circle 基準となる円
		/// @param startAngle 開始角度（ラジアン）
		/// @param angle 描画する角度（ラジアン）
		/// @param color 描画色
		/// @param blendMode 合成方式
		/// @param enableAntialiasing アンチエイリアス設定
		/// @param dstAlpha 描画先 alpha の扱い
		/// @remark startAngle は 12 時方向を 0 とし、時計回りに増加します。
		/// @remark angle が正の場合は時計回り、負の場合は反時計回りに描画します。
		/// @remark |angle| が 2π 以上の場合は円全体を描画します。
		/// @remark 円弧境界と 2 本の半径境界を含む最終 coverage を作り、1 回だけ合成します。
		/// @remark 画像の範囲外はクリップされます。
		/// @remark circle.r が正でない場合、または angle が 0 の場合、この関数は何もしません。
		void CirclePie(
			Image& image,
			const Circle& circle,
			double startAngle,
			double angle,
			Color color,
			ImagePixel::BlendMode blendMode = ImagePixel::BlendMode::SourceOver,
			EnableAntialiasing enableAntialiasing = EnableAntialiasing::Yes,
			DstAlpha dstAlpha = DstAlpha::Preserve);

		/// @brief 円弧の枠を描画します。
		/// @param image 描画先画像
		/// @param circle 基準となる円
		/// @param startAngle 開始角度（ラジアン、12 時方向を 0 として時計回り）
		/// @param angle 円弧の角度（ラジアン、正は時計回り、負は反時計回り）
		/// @param innerThickness 基準の円周から内側方向への太さ
		/// @param outerThickness 基準の円周から外側方向への太さ
		/// @param color 描画色
		/// @param blendMode 合成方式
		/// @param enableAntialiasing アンチエイリアス設定
		/// @param dstAlpha 描画先 alpha の扱い
		/// @remark 線端は LineCap::Flat です。
		/// @see @ref image_draw_circle_arc
		void CircleArc(
			Image& image,
			const Circle& circle,
			double startAngle,
			double angle,
			double innerThickness,
			double outerThickness,
			Color color,
			ImagePixel::BlendMode blendMode = ImagePixel::BlendMode::SourceOver,
			EnableAntialiasing enableAntialiasing = EnableAntialiasing::Yes,
			DstAlpha dstAlpha = DstAlpha::Preserve);

		/// @anchor image_draw_circle_arc
		/// @brief 円弧の枠を描画します。
		/// @param image 描画先画像
		/// @param circle 基準となる円
		/// @param lineCap 線端の形状
		/// @param startAngle 開始角度（ラジアン、12 時方向を 0 として時計回り）
		/// @param angle 円弧の角度（ラジアン、正は時計回り、負は反時計回り）
		/// @param innerThickness 基準の円周から内側方向への太さ
		/// @param outerThickness 基準の円周から外側方向への太さ
		/// @param color 描画色
		/// @param blendMode 合成方式
		/// @param enableAntialiasing アンチエイリアス設定
		/// @param dstAlpha 描画先 alpha の扱い
		/// @remark Flat は半径方向に切り落とし、Square は接線方向へ線幅の半分だけ延長し、Round は半円の線端を付けます。
		/// @remark 内側が中心に達する場合、線端の幅は中心から外周までの幅になります。
		/// @remark angle が 0 の場合、Flat は何も描かず、Round は線端の円、Square は接線方向に沿った正方形を描きます。
		/// @remark |angle| が 2π 以上の場合は、線端を付けずに CircleFrame() と同じ領域を描画します。
		/// @remark 円弧と線端が重なる部分も一度だけ描き込みます。画像の範囲外はクリップされます。
		/// @remark circle.r が正でない場合、太さのいずれかが負の場合、または両方の太さが 0 の場合は何もしません。
		void CircleArc(
			Image& image,
			const Circle& circle,
			LineCap lineCap,
			double startAngle,
			double angle,
			double innerThickness,
			double outerThickness,
			Color color,
			ImagePixel::BlendMode blendMode = ImagePixel::BlendMode::SourceOver,
			EnableAntialiasing enableAntialiasing = EnableAntialiasing::Yes,
			DstAlpha dstAlpha = DstAlpha::Preserve);

		/// @anchor image_draw_circle_segment
		/// @brief 円の弓形を描画します。
		/// @param image 描画先画像
		/// @param circle 基準となる円
		/// @param arcMidpointAngle 弧の中央の方向（ラジアン、12 時方向を 0 として時計回り）
		/// @param height 弦から弧の中央までの高さ（ピクセル）
		/// @param color 描画色
		/// @param blendMode 合成方式
		/// @param enableAntialiasing アンチエイリアス設定
		/// @param dstAlpha 描画先 alpha の扱い
		/// @remark height が直径以上の場合は円全体を描画します。
		/// @remark circle.r または height が正でない場合は何もしません。
		/// @remark 画像の範囲外はクリップされます。
		void CircleSegment(
			Image& image,
			const Circle& circle,
			double arcMidpointAngle,
			double height,
			Color color,
			ImagePixel::BlendMode blendMode = ImagePixel::BlendMode::SourceOver,
			EnableAntialiasing enableAntialiasing = EnableAntialiasing::Yes,
			DstAlpha dstAlpha = DstAlpha::Preserve);

		/// @anchor image_draw_circle_segment_angles
		/// @brief 円の弓形を描画します。
		/// @param image 描画先画像
		/// @param circle 基準となる円
		/// @param startAngle 弧の開始角度（ラジアン、12 時方向を 0 として時計回り）
		/// @param angle 弧の角度（ラジアン、正は時計回り、負は反時計回り）
		/// @param color 描画色
		/// @param blendMode 合成方式
		/// @param enableAntialiasing アンチエイリアス設定
		/// @param dstAlpha 描画先 alpha の扱い
		/// @remark |angle| が 2π 以上の場合は円全体を描画します。
		/// @remark circle.r が正でない場合、または angle が 0 の場合は何もしません。
		/// @remark 画像の範囲外はクリップされます。
		void CircleSegmentFromAngles(
			Image& image,
			const Circle& circle,
			double startAngle,
			double angle,
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

		/// @anchor image_draw_ellipse_pie
		/// @brief 楕円の扇形を描画します。
		/// @param image 描画先画像
		/// @param ellipse 基準となる楕円
		/// @param startAngle 中心から見た開始方向（ラジアン、12 時方向を 0 として時計回り）
		/// @param angle 扇形の角度（ラジアン、正は時計回り、負は反時計回り）
		/// @param color 描画色
		/// @param blendMode 合成方式
		/// @param enableAntialiasing アンチエイリアス設定
		/// @param dstAlpha 描画先 alpha の扱い
		/// @remark |angle| が 2π 以上の場合は楕円全体を描画します。
		/// @remark 半径のいずれかが正でない場合、または angle が 0 の場合は何もしません。
		/// @remark 画像の範囲外はクリップされます。
		void EllipsePie(
			Image& image,
			const Ellipse& ellipse,
			double startAngle,
			double angle,
			Color color,
			ImagePixel::BlendMode blendMode = ImagePixel::BlendMode::SourceOver,
			EnableAntialiasing enableAntialiasing = EnableAntialiasing::Yes,
			DstAlpha dstAlpha = DstAlpha::Preserve);

		/// @anchor image_draw_superellipse
		/// @brief スーパー楕円を描画します。
		/// @param image 描画先画像
		/// @param superEllipse 描画するスーパー楕円
		/// @param color 描画色
		/// @param blendMode 合成方式
		/// @param enableAntialiasing アンチエイリアス設定
		/// @param dstAlpha 描画先 alpha の扱い
		/// @remark 半径は絶対値を使用します。半径のいずれかが 0、または n が 0 以下の場合は何もしません。
		/// @remark 画像の範囲外はクリップされます。
		void Fill(
			Image& image,
			const SuperEllipse& superEllipse,
			Color color,
			ImagePixel::BlendMode blendMode = ImagePixel::BlendMode::SourceOver,
			EnableAntialiasing enableAntialiasing = EnableAntialiasing::Yes,
			DstAlpha dstAlpha = DstAlpha::Preserve);

		/// @anchor image_draw_superellipse_frame
		/// @brief スーパー楕円の輪郭を中心とする枠を描画します。
		/// @param image 描画先画像
		/// @param superEllipse 基準となるスーパー楕円
		/// @param thickness 枠の太さ（ピクセル）
		/// @param color 描画色
		/// @param blendMode 合成方式
		/// @param enableAntialiasing アンチエイリアス設定
		/// @param dstAlpha 描画先 alpha の扱い
		/// @remark 曲線は近似され、角は丸くなります。枠が重なる部分も色の濃さは変わりません。
		/// @remark 半径は絶対値を使用します。半径のいずれかが 0、n または thickness が 0 以下の場合は何もしません。
		/// @remark 画像の範囲外はクリップされます。
		void SuperEllipseFrame(
			Image& image,
			const SuperEllipse& superEllipse,
			double thickness,
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

		/// @brief 角丸長方形を描画します。
		/// @param image 描画先画像
		/// @param roundRect 描画する角丸長方形
		/// @param color 描画色
		/// @param blendMode 合成方式
		/// @param enableAntialiasing アンチエイリアス設定
		/// @param dstAlpha 描画先 alpha の扱い
		/// @remark 角丸長方形境界までの距離、またはそれと等価な coverage を使用します。
		/// @remark 画像の範囲外はクリップされます。
		void Fill(
			Image& image,
			const RoundRect& roundRect,
			Color color,
			ImagePixel::BlendMode blendMode = ImagePixel::BlendMode::SourceOver,
			EnableAntialiasing enableAntialiasing = EnableAntialiasing::Yes,
			DstAlpha dstAlpha = DstAlpha::Preserve);

		/// @brief 角丸長方形の枠を描画します。
		/// @param image 描画先画像
		/// @param roundRect 基準となる角丸長方形
		/// @param innerThickness roundRect の内側に描画する太さ
		/// @param outerThickness roundRect の外側に描画する太さ
		/// @param color 描画色
		/// @param blendMode 合成方式
		/// @param enableAntialiasing アンチエイリアス設定
		/// @param dstAlpha 描画先 alpha の扱い
		/// @remark 描画領域は角丸長方形境界からの符号付き距離が -innerThickness 以上 outerThickness 以下の領域です。
		/// @remark 実装は距離ベースまたは閉じた stroke の coverage 統合など、等価な結果を得られる方法を選択します。
		/// @remark coverage は最終的な枠領域として作り、1 回だけ合成します。
		/// @remark 画像の範囲外はクリップされます。
		/// @remark innerThickness または outerThickness が負の場合、この関数は何もしません。
		void RoundRectFrame(
			Image& image,
			const RoundRect& roundRect,
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

		/// @brief 角の結合方法を指定して、三角形の中央配置の枠を描画します。
		/// @param image 描画先画像
		/// @param triangle 基準となる三角形
		/// @param joinStyle 角の結合方法
		/// @param thickness 枠の太さ
		/// @param color 描画色
		/// @param blendMode 合成方式
		/// @param enableAntialiasing アンチエイリアス設定
		/// @param dstAlpha 描画先 alpha の扱い
		/// @see Triangle::paintFrame(Image&, JoinStyle, double, const Color&, EnableAntialiasing) const
		void TriangleFrame(
			Image& image,
			const Triangle& triangle,
			JoinStyle joinStyle,
			double thickness,
			Color color,
			ImagePixel::BlendMode blendMode = ImagePixel::BlendMode::SourceOver,
			EnableAntialiasing enableAntialiasing = EnableAntialiasing::Yes,
			DstAlpha dstAlpha = DstAlpha::Preserve);

		/// @brief 三角形の内側と外側の太さを指定して、枠を描画します。
		/// @param image 描画先画像
		/// @param triangle 基準となる三角形
		/// @param innerThickness 輪郭から内側への太さ
		/// @param outerThickness 輪郭から外側への太さ
		/// @param color 描画色
		/// @param blendMode 合成方式
		/// @param enableAntialiasing アンチエイリアス設定
		/// @param dstAlpha 描画先 alpha の扱い
		/// @see Triangle::paintFrame(Image&, double, double, const Color&, EnableAntialiasing) const
		void TriangleFrame(
			Image& image,
			const Triangle& triangle,
			double innerThickness,
			double outerThickness,
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

		/// @brief 角の結合方法を指定して、四角形の中央配置の枠を描画します。
		/// @param image 描画先画像
		/// @param quad 基準となる四角形
		/// @param joinStyle 角の結合方法
		/// @param thickness 枠の太さ
		/// @param color 描画色
		/// @param blendMode 合成方式
		/// @param enableAntialiasing アンチエイリアス設定
		/// @param dstAlpha 描画先 alpha の扱い
		/// @see Quad::paintFrame(Image&, JoinStyle, double, const Color&, EnableAntialiasing) const
		void QuadFrame(
			Image& image,
			const Quad& quad,
			JoinStyle joinStyle,
			double thickness,
			Color color,
			ImagePixel::BlendMode blendMode = ImagePixel::BlendMode::SourceOver,
			EnableAntialiasing enableAntialiasing = EnableAntialiasing::Yes,
			DstAlpha dstAlpha = DstAlpha::Preserve);

		/// @brief 四角形の内側と外側の太さを指定して、枠を描画します。
		/// @param image 描画先画像
		/// @param quad 基準となる四角形
		/// @param innerThickness 輪郭から内側への太さ
		/// @param outerThickness 輪郭から外側への太さ
		/// @param color 描画色
		/// @param blendMode 合成方式
		/// @param enableAntialiasing アンチエイリアス設定
		/// @param dstAlpha 描画先 alpha の扱い
		/// @see Quad::paintFrame(Image&, double, double, const Color&, EnableAntialiasing) const
		void QuadFrame(
			Image& image,
			const Quad& quad,
			double innerThickness,
			double outerThickness,
			Color color,
			ImagePixel::BlendMode blendMode = ImagePixel::BlendMode::SourceOver,
			EnableAntialiasing enableAntialiasing = EnableAntialiasing::Yes,
			DstAlpha dstAlpha = DstAlpha::Preserve);

		/// @brief 点列で表される polygon を描画します。
		/// @param image 描画先画像
		/// @param points polygon の頂点配列
		/// @param color 描画色
		/// @param blendMode 合成方式
		/// @param enableAntialiasing アンチエイリアス設定
		/// @param dstAlpha 描画先 alpha の扱い
		/// @remark 頂点列の向き（時計回り / 反時計回り）は問いません。
		/// @remark winding rule は even-odd です。
		/// @remark 自己交差を含む場合も even-odd rule に従って塗りつぶされます。
		/// @remark 複数 contour や穴を持つ Polygon には Fill(Image&, const Polygon&, ...) を使用してください。
		/// @remark 画像の範囲外はクリップされます。
		/// @remark points.size() が 3 未満の場合、この関数は何もしません。
		void FillPolygon(
			Image& image,
			std::span<const Vec2> points,
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

		/// @brief Polygon をオフセット付きで描画します。
		/// @param image 描画先画像
		/// @param polygon 描画する Polygon
		/// @param offset polygon 全体に加算する平行移動量
		/// @param color 描画色
		/// @param blendMode 合成方式
		/// @param enableAntialiasing アンチエイリアス設定
		/// @param dstAlpha 描画先 alpha の扱い
		/// @remark Polygon の外周と穴を考慮して描画します。
		/// @remark winding rule は even-odd です。
		/// @remark offset は rasterize 時に適用され、polygon 自体は変更されません。
		/// @remark 同じ Polygon を異なる位置に繰り返し描画する用途を想定しています。
		/// @remark 画像の範囲外はクリップされます。
		/// @remark polygon が空の場合、この関数は何もしません。
		void Fill(
			Image& image,
			const Polygon& polygon,
			const Vec2& offset,
			Color color,
			ImagePixel::BlendMode blendMode = ImagePixel::BlendMode::SourceOver,
			EnableAntialiasing enableAntialiasing = EnableAntialiasing::Yes,
			DstAlpha dstAlpha = DstAlpha::Preserve);

		/// @brief Shape2D の輪郭の内側を塗りつぶします。
		/// @anchor image_draw_shape2d
		/// @param image 描画先画像
		/// @param shape 描画する Shape2D
		/// @param color 描画色
		/// @param blendMode 合成方式
		/// @param enableAntialiasing アンチエイリアス設定
		/// @param dstAlpha 描画先 alpha の扱い
		/// @remark vertices() の頂点を順に結んだ輪郭を描きます。三角形インデックス indices() は使いません。
		/// @remark 輪郭が交差する場合は、辺を横切るたびに塗る領域と塗らない領域が切り替わります（偶奇規則）。
		/// @remark 頂点数が 3 未満の場合は何もしません。画像の範囲外はクリップされます。
		void Fill(
			Image& image,
			const Shape2D& shape,
			Color color,
			ImagePixel::BlendMode blendMode = ImagePixel::BlendMode::SourceOver,
			EnableAntialiasing enableAntialiasing = EnableAntialiasing::Yes,
			DstAlpha dstAlpha = DstAlpha::Preserve);

		/// @brief Shape2D の輪郭の内側を、指定した位置に移動して塗りつぶします。
		/// @param image 描画先画像
		/// @param shape 描画する Shape2D
		/// @param offset 全頂点に加算する平行移動量
		/// @param color 描画色
		/// @param blendMode 合成方式
		/// @param enableAntialiasing アンチエイリアス設定
		/// @param dstAlpha 描画先 alpha の扱い
		/// @see @ref image_draw_shape2d "Shape2D の塗りつぶし"
		void Fill(
			Image& image,
			const Shape2D& shape,
			const Vec2& offset,
			Color color,
			ImagePixel::BlendMode blendMode = ImagePixel::BlendMode::SourceOver,
			EnableAntialiasing enableAntialiasing = EnableAntialiasing::Yes,
			DstAlpha dstAlpha = DstAlpha::Preserve);

		/// @brief Shape2D の枠を描画します。
		/// @anchor image_draw_shape2d_frame
		/// @param image 描画先画像
		/// @param shape 描画する Shape2D
		/// @param thickness 輪郭を中心とした枠の太さ
		/// @param color 描画色
		/// @param blendMode 合成方式
		/// @param enableAntialiasing アンチエイリアス設定
		/// @param dstAlpha 描画先 alpha の扱い
		/// @remark vertices() の頂点を順に結んだ輪郭を描きます。三角形インデックス indices() は使いません。
		/// @remark 枠の角は丸くなります。線が重なる部分も一度だけ描き込みます。
		/// @remark 頂点数が 3 未満、または thickness が正でない場合は何もしません。画像の範囲外はクリップされます。
		void Shape2DFrame(
			Image& image,
			const Shape2D& shape,
			double thickness,
			Color color,
			ImagePixel::BlendMode blendMode = ImagePixel::BlendMode::SourceOver,
			EnableAntialiasing enableAntialiasing = EnableAntialiasing::Yes,
			DstAlpha dstAlpha = DstAlpha::Preserve);

		/// @brief Shape2D の枠をオフセット付きで描画します。
		/// @param image 描画先画像
		/// @param shape 描画する Shape2D
		/// @param offset 全頂点に加算する平行移動量
		/// @param thickness 輪郭を中心とした枠の太さ
		/// @param color 描画色
		/// @param blendMode 合成方式
		/// @param enableAntialiasing アンチエイリアス設定
		/// @param dstAlpha 描画先 alpha の扱い
		/// @see @ref image_draw_shape2d_frame "Shape2D の枠の描画"
		void Shape2DFrame(
			Image& image,
			const Shape2D& shape,
			const Vec2& offset,
			double thickness,
			Color color,
			ImagePixel::BlendMode blendMode = ImagePixel::BlendMode::SourceOver,
			EnableAntialiasing enableAntialiasing = EnableAntialiasing::Yes,
			DstAlpha dstAlpha = DstAlpha::Preserve);

		/// @brief Polygon の枠を描画します。
		/// @anchor image_draw_polygon_frame
		/// @param image 描画先画像
		/// @param polygon 描画する Polygon
		/// @param thickness 輪郭を中心とした枠の太さ
		/// @param color 描画色
		/// @param blendMode 合成方式
		/// @param enableAntialiasing アンチエイリアス設定
		/// @param dstAlpha 描画先 alpha の扱い
		/// @remark 外周と穴の枠を、角を丸めて描画します。枠が重なる部分も一度だけ描き込みます。
		/// @remark polygon が空、または thickness が正でない場合は何もしません。画像の範囲外はクリップされます。
		void PolygonFrame(
			Image& image,
			const Polygon& polygon,
			double thickness,
			Color color,
			ImagePixel::BlendMode blendMode = ImagePixel::BlendMode::SourceOver,
			EnableAntialiasing enableAntialiasing = EnableAntialiasing::Yes,
			DstAlpha dstAlpha = DstAlpha::Preserve);

		/// @brief Polygon の枠をオフセット付きで描画します。
		/// @param image 描画先画像
		/// @param polygon 描画する Polygon
		/// @param offset 全頂点に加算する平行移動量
		/// @param thickness 輪郭を中心とした枠の太さ
		/// @param color 描画色
		/// @param blendMode 合成方式
		/// @param enableAntialiasing アンチエイリアス設定
		/// @param dstAlpha 描画先 alpha の扱い
		/// @see @ref image_draw_polygon_frame "Polygon の枠の描画"
		void PolygonFrame(
			Image& image,
			const Polygon& polygon,
			const Vec2& offset,
			double thickness,
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
		/// @param thickness 線の太さ。正でない場合は何も描きません。
		/// @param color 描画色
		/// @param blendMode 合成方式
		/// @param enableAntialiasing アンチエイリアス設定
		/// @param lineCap 線端の形状
		/// @param dstAlpha 描画先 alpha の扱い
		/// @remark 線と丸い線端が重なる部分も一度だけ描き込みます。
		/// @remark 始点と終点が同じ場合、Round は直径 thickness の円、Square は一辺 thickness の正方形を描きます。Flat では何も描きません。
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

		/// @brief 太さのある線分を描画します。
		/// @param image 描画先画像
		/// @param from 始点
		/// @param to 終点
		/// @param thickness 線の太さ。正でない場合は何も描きません。
		/// @param color 描画色
		/// @param blendMode 合成方式
		/// @param enableAntialiasing アンチエイリアス設定
		/// @param startCap 始点の線端の形状
		/// @param endCap 終点の線端の形状
		/// @param dstAlpha 描画先 alpha の扱い
		/// @remark 線と丸い線端が重なる部分も一度だけ描き込みます。
		/// @remark 始点と終点が同じ場合、どちらかが Square なら一辺 thickness の正方形、そうでなく Round があれば直径 thickness の円を描きます。両方 Flat では何も描きません。
		/// @remark 画像の範囲外はクリップされます。
		void Line(
			Image& image,
			const Vec2& from,
			const Vec2& to,
			double thickness,
			Color color,
			ImagePixel::BlendMode blendMode,
			EnableAntialiasing enableAntialiasing,
			LineCap startCap,
			LineCap endCap,
			DstAlpha dstAlpha = DstAlpha::Preserve);

		/// @brief 始点から終点へ向かう矢印を描画します。
		/// @param image 描画先画像
		/// @param from 始点（尾側）
		/// @param to 終点（先端）
		/// @param width 矢印の線の幅
		/// @param headSize 先端で線幅に加える幅と先端の長さ（幅, 長さ）
		/// @param color 描画色
		/// @param blendMode 合成方式
		/// @param enableAntialiasing アンチエイリアス設定
		/// @param startCap 始点の線端の形状。Square は線幅の半分だけ後方へ延ばし、Round は半円を付けます。
		/// @param dstAlpha 描画先 alpha の扱い
		/// @remark 先端の長さが線分より長い場合、headSize を同じ比率で縮小します。
		/// @remark 始点と終点が同じ、または width、headSize のいずれかの成分が正でない場合は何も描きません。
		void Arrow(
			Image& image,
			const Vec2& from,
			const Vec2& to,
			double width,
			const SizeF& headSize,
			Color color,
			ImagePixel::BlendMode blendMode = ImagePixel::BlendMode::SourceOver,
			EnableAntialiasing enableAntialiasing = EnableAntialiasing::Yes,
			LineCap startCap = LineCap::Flat,
			DstAlpha dstAlpha = DstAlpha::Preserve);

		/// @brief 点列を太さのある連続線として描画します。
		/// @param image 描画先画像
		/// @param points 頂点配列
		/// @param thickness 線の太さ
		/// @param color 描画色
		/// @param blendMode 合成方式
		/// @param enableAntialiasing アンチエイリアス設定
		/// @param lineCap 線端の形状
		/// @param dstAlpha 描画先 alpha の扱い
		/// @remark 隣接する点を結ぶ線分列として描画します。
		/// @remark lineCap は線列全体の始端と終端にのみ適用されます。
		/// @remark 線分同士の接続部は round join として扱います。
		/// @remark 各線分、join, cap の coverage を統合してから 1 回だけ合成します。
		/// @remark 画像の範囲外はクリップされます。
		/// @remark points.size() が 2 未満、または thickness が正でない場合、この関数は何もしません。
		void LineString(
			Image& image,
			std::span<const Vec2> points,
			double thickness,
			Color color,
			ImagePixel::BlendMode blendMode = ImagePixel::BlendMode::SourceOver,
			EnableAntialiasing enableAntialiasing = EnableAntialiasing::Yes,
			LineCap lineCap = LineCap::Round,
			DstAlpha dstAlpha = DstAlpha::Preserve);

		/// @brief 点列を太さのある連続線として描画します。
		/// @param image 描画先画像
		/// @param points 頂点配列
		/// @param thickness 線の太さ
		/// @param color 描画色
		/// @param blendMode 合成方式
		/// @param enableAntialiasing アンチエイリアス設定
		/// @param startCap 始点の線端の形状
		/// @param endCap 終点の線端の形状
		/// @param dstAlpha 描画先 alpha の扱い
		/// @remark 隣接する点を結ぶ線分列として描画します。
		/// @remark 線端は点列全体の始点と終点にのみ適用され、接続部は丸くなります。
		/// @remark 2 点以上あり、全点が同じ位置の場合、線端の扱いは ImageDraw::Line() と同じです。
		/// @remark 線分、接続部、線端が重なる部分も一度だけ描き込みます。
		/// @remark 画像の範囲外はクリップされます。
		/// @remark points.size() が 2 未満、または thickness が正でない場合、この関数は何もしません。
		void LineString(
			Image& image,
			std::span<const Vec2> points,
			double thickness,
			Color color,
			ImagePixel::BlendMode blendMode,
			EnableAntialiasing enableAntialiasing,
			LineCap startCap,
			LineCap endCap,
			DstAlpha dstAlpha = DstAlpha::Preserve);

		/// @brief 点列を太さのある連続線として、offset を加えて描画します。
		/// @param image 描画先画像
		/// @param points 頂点配列
		/// @param offset 各頂点に加算するオフセット
		/// @param thickness 線の太さ
		/// @param color 描画色
		/// @param blendMode 合成方式
		/// @param enableAntialiasing アンチエイリアス設定
		/// @param lineCap 線端の形状
		/// @param dstAlpha 描画先 alpha の扱い
		/// @remark points の内容は変更せず、primitive 構築時に offset を加算して扱います。
		/// @remark lineCap は線列全体の始端と終端にのみ適用されます。
		/// @remark 線分同士の接続部は round join として扱います。
		/// @remark 各線分、join, cap の coverage を統合してから 1 回だけ合成します。
		/// @remark 画像の範囲外はクリップされます。
		/// @remark points.size() が 2 未満、または thickness が正でない場合、この関数は何もしません。
		void LineString(
			Image& image,
			std::span<const Vec2> points,
			const Vec2& offset,
			double thickness,
			Color color,
			ImagePixel::BlendMode blendMode = ImagePixel::BlendMode::SourceOver,
			EnableAntialiasing enableAntialiasing = EnableAntialiasing::Yes,
			LineCap lineCap = LineCap::Round,
			DstAlpha dstAlpha = DstAlpha::Preserve);

		/// @brief 点列を太さのある連続線として、offset を加えて描画します。
		/// @param image 描画先画像
		/// @param points 頂点配列
		/// @param offset 各頂点に加算するオフセット
		/// @param thickness 線の太さ
		/// @param color 描画色
		/// @param blendMode 合成方式
		/// @param enableAntialiasing アンチエイリアス設定
		/// @param startCap 始点の線端の形状
		/// @param endCap 終点の線端の形状
		/// @param dstAlpha 描画先 alpha の扱い
		/// @remark points の内容は変更しません。
		/// @remark 線端は点列全体の始点と終点にのみ適用され、接続部は丸くなります。
		/// @remark 2 点以上あり、全点が同じ位置の場合、線端の扱いは ImageDraw::Line() と同じです。
		/// @remark 線分、接続部、線端が重なる部分も一度だけ描き込みます。
		/// @remark 画像の範囲外はクリップされます。
		/// @remark points.size() が 2 未満、または thickness が正でない場合、この関数は何もしません。
		void LineString(
			Image& image,
			std::span<const Vec2> points,
			const Vec2& offset,
			double thickness,
			Color color,
			ImagePixel::BlendMode blendMode,
			EnableAntialiasing enableAntialiasing,
			LineCap startCap,
			LineCap endCap,
			DstAlpha dstAlpha = DstAlpha::Preserve);

		/// @brief 点列を太さのある閉じた連続線として描画します。
		/// @param image 描画先画像
		/// @param points 頂点配列
		/// @param thickness 線の太さ
		/// @param color 描画色
		/// @param blendMode 合成方式
		/// @param enableAntialiasing アンチエイリアス設定
		/// @param dstAlpha 描画先 alpha の扱い
		/// @remark 隣接する点を結ぶ線分列に加えて、最後の点から最初の点へ線分を結びます。
		/// @remark 閉じた線列のため、線端 cap はありません。
		/// @remark 線分同士の接続部は round join として扱います。
		/// @remark 各線分と join の coverage を統合してから 1 回だけ合成します。
		/// @remark 画像の範囲外はクリップされます。
		/// @remark points.size() が 3 未満、または thickness が正でない場合、この関数は何もしません。
		void ClosedLineString(
			Image& image,
			std::span<const Vec2> points,
			double thickness,
			Color color,
			ImagePixel::BlendMode blendMode = ImagePixel::BlendMode::SourceOver,
			EnableAntialiasing enableAntialiasing = EnableAntialiasing::Yes,
			DstAlpha dstAlpha = DstAlpha::Preserve);

		/// @brief 点列を太さのある閉じた連続線として、offset を加えて描画します。
		/// @param image 描画先画像
		/// @param points 頂点配列
		/// @param offset 各頂点に加算するオフセット
		/// @param thickness 線の太さ
		/// @param color 描画色
		/// @param blendMode 合成方式
		/// @param enableAntialiasing アンチエイリアス設定
		/// @param dstAlpha 描画先 alpha の扱い
		/// @remark points の内容は変更せず、primitive 構築時に offset を加算して扱います。
		/// @remark 隣接する点を結ぶ線分列に加えて、最後の点から最初の点へ線分を結びます。
		/// @remark 閉じた線列のため、線端 cap はありません。
		/// @remark 線分同士の接続部は round join として扱います。
		/// @remark 各線分と join の coverage を統合してから 1 回だけ合成します。
		/// @remark 画像の範囲外はクリップされます。
		/// @remark points.size() が 3 未満、または thickness が正でない場合、この関数は何もしません。
		void ClosedLineString(
			Image& image,
			std::span<const Vec2> points,
			const Vec2& offset,
			double thickness,
			Color color,
			ImagePixel::BlendMode blendMode = ImagePixel::BlendMode::SourceOver,
			EnableAntialiasing enableAntialiasing = EnableAntialiasing::Yes,
			DstAlpha dstAlpha = DstAlpha::Preserve);
	}
}
