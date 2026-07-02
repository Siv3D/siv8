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

namespace s3d
{
	/// @brief 三次ベジェ曲線
	/// 4 点（開始点 p0、制御点 p1, p2、終了点 p3）で定義される三次ベジェ曲線です。
	/// パラメータ t（0.0～1.0）に対して曲線上の座標・導関数（接線 / 法線 / 曲率）・長さ・分割などを提供します。
	struct Bezier3
	{
		/// @brief 座標型
		using position_type = Vec2;

		/// @brief 要素型
		using value_type = position_type::value_type;

		/// @brief 開始点
		position_type p0;

		/// @brief 制御点 1
		position_type p1;

		/// @brief 制御点 2
		position_type p2;

		/// @brief 終了点
		position_type p3;

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		/// @brief デフォルトコンストラクタ
		[[nodiscard]]
		Bezier3() = default;

		/// @brief 4 点から三次ベジェ曲線を作成します。
		/// @param _p0 開始点
		/// @param _p1 制御点 1
		/// @param _p2 制御点 2
		/// @param _p3 終了点
		[[nodiscard]]
		constexpr Bezier3(const position_type& _p0, const position_type& _p1, const position_type& _p2, const position_type& _p3) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	withP0
		//
		////////////////////////////////////////////////////////////////

		/// @brief 開始点 p0 を変更した新しい曲線を返します。
		/// @param _x 開始点の X 座標
		/// @param _y 開始点の Y 座標
		/// @return p0 を変更した新しい三次ベジェ曲線
		[[nodiscard]]
		constexpr Bezier3 withP0(value_type _x, value_type _y) const noexcept;

		/// @brief 開始点 p0 を変更した新しい曲線を返します。
		/// @param _p0 開始点
		/// @return p0 を変更した新しい三次ベジェ曲線
		[[nodiscard]]
		constexpr Bezier3 withP0(position_type _p0) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	withP1
		//
		////////////////////////////////////////////////////////////////

		/// @brief 制御点 p1 を変更した新しい曲線を返します。
		/// @param _x 制御点の X 座標
		/// @param _y 制御点の Y 座標
		/// @return p1 を変更した新しい三次ベジェ曲線
		[[nodiscard]]
		constexpr Bezier3 withP1(value_type _x, value_type _y) const noexcept;

		/// @brief 制御点 p1 を変更した新しい曲線を返します。
		/// @param _p1 制御点
		/// @return p1 を変更した新しい三次ベジェ曲線
		[[nodiscard]]
		constexpr Bezier3 withP1(position_type _p1) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	withP2
		//
		////////////////////////////////////////////////////////////////

		/// @brief 制御点 p2 を変更した新しい曲線を返します。
		/// @param _x 制御点の X 座標
		/// @param _y 制御点の Y 座標
		/// @return p2 を変更した新しい三次ベジェ曲線
		[[nodiscard]]
		constexpr Bezier3 withP2(value_type _x, value_type _y) const noexcept;

		/// @brief 制御点 p2 を変更した新しい曲線を返します。
		/// @param _p2 制御点
		/// @return p2 を変更した新しい三次ベジェ曲線
		[[nodiscard]]
		constexpr Bezier3 withP2(position_type _p2) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	withP3
		//
		////////////////////////////////////////////////////////////////

		/// @brief 終了点 p3 を変更した新しい曲線を返します。
		/// @param _x 終了点の X 座標
		/// @param _y 終了点の Y 座標
		/// @return p3 を変更した新しい三次ベジェ曲線
		[[nodiscard]]
		constexpr Bezier3 withP3(value_type _x, value_type _y) const noexcept;

		/// @brief 終了点 p3 を変更した新しい曲線を返します。
		/// @param _p3 終了点
		/// @return p3 を変更した新しい三次ベジェ曲線
		[[nodiscard]]
		constexpr Bezier3 withP3(position_type _p3) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	withOffset
		//
		////////////////////////////////////////////////////////////////

		/// @brief 3 点すべてにオフセットを加えた新しい曲線を返します。
		/// @param x X 方向のオフセット
		/// @param y Y 方向のオフセット
		/// @return オフセット後の三次ベジェ曲線
		[[nodiscard]]
		constexpr Bezier3 withOffset(value_type x, value_type y) const noexcept;

		/// @brief 3 点すべてにオフセットを加えた新しい曲線を返します。
		/// @param v オフセット
		/// @return オフセット後の三次ベジェ曲線
		[[nodiscard]]
		constexpr Bezier3 withOffset(position_type v) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	withOffsetX, withOffsetY
		//
		////////////////////////////////////////////////////////////////

		/// @brief 3 点すべてに X 方向のオフセットを加えた新しい曲線を返します。
		/// @param x X 方向のオフセット
		/// @return オフセット後の三次ベジェ曲線
		[[nodiscard]]
		constexpr Bezier3 withOffsetX(value_type x) const noexcept;

		/// @brief 3 点すべてに Y 方向のオフセットを加えた新しい曲線を返します。
		/// @param y Y 方向のオフセット
		/// @return オフセット後の三次ベジェ曲線
		[[nodiscard]]
		constexpr Bezier3 withOffsetY(value_type y) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	movedBy
		//
		////////////////////////////////////////////////////////////////

		/// @brief 平行移動した新しい曲線を返します。
		/// @param x X 方向の移動量
		/// @param y Y 方向の移動量
		/// @return 移動後の三次ベジェ曲線
		[[nodiscard]]
		constexpr Bezier3 movedBy(value_type x, value_type y) const noexcept;

		/// @brief 平行移動した新しい曲線を返します。
		/// @param v 移動量
		/// @return 移動後の三次ベジェ曲線
		[[nodiscard]]
		constexpr Bezier3 movedBy(position_type v) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	moveBy
		//
		////////////////////////////////////////////////////////////////

		/// @brief 曲線を平行移動します。
		/// @param x X 方向の移動量
		/// @param y Y 方向の移動量
		/// @return *this
		constexpr Bezier3& moveBy(value_type x, value_type y) noexcept;

		/// @brief 曲線を平行移動します。
		/// @param v 移動量
		/// @return *this
		constexpr Bezier3& moveBy(position_type v) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	controlPoints
		//
		////////////////////////////////////////////////////////////////

		/// @brief 4 点の制御点を配列で返します。
		/// @return 制御点の配列
		[[nodiscard]]
		constexpr std::array<position_type, 4> controlPoints() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	controlPointAtIndex
		//
		////////////////////////////////////////////////////////////////

		/// @brief インデックスで制御点を取得します。
		/// @param index 0: p0, 1: p1, 2: p2, 3: p3
		/// @return 指定した点への参照
		/// @throw std::out_of_range index が 0～3 の範囲外の場合
		[[nodiscard]]
		position_type& controlPointAtIndex(size_t index);

		/// @brief インデックスで制御点を取得します。
		/// @param index 0: p0, 1: p1, 2: p2, 3: p3
		/// @return 指定した点への参照
		/// @throw std::out_of_range index が 0～3 の範囲外の場合
		[[nodiscard]]
		const position_type& controlPointAtIndex(size_t index) const;

		////////////////////////////////////////////////////////////////
		//
		//	pointAt
		//
		////////////////////////////////////////////////////////////////

		/// @brief パラメータ t に対応する曲線上の座標を返します。
		/// @param t パラメータ（0.0～1.0）
		/// @return 曲線上の座標
		[[nodiscard]]
		constexpr position_type pointAt(double t) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	derivativeAt
		//
		////////////////////////////////////////////////////////////////

		/// @brief パラメータ t における 1 階導関数（速度ベクトル）を返します。
		/// @param t パラメータ（0.0～1.0）
		/// @return 1 階導関数ベクトル
		[[nodiscard]]
		constexpr Vec2 derivativeAt(double t) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	secondDerivativeAt
		//
		////////////////////////////////////////////////////////////////

		/// @brief パラメータ t における 2 階導関数（加速度ベクトル）を返します。
		/// @param t パラメータ（0.0～1.0）
		/// @return 2 階導関数ベクトル
		[[nodiscard]]
		constexpr Vec2 secondDerivativeAt(double t) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	thirdDerivative
		//
		////////////////////////////////////////////////////////////////

		/// @brief 3 階導関数（ジャークベクトル）を返します。
		/// @return 3 階導関数ベクトル
		[[nodiscard]]
		constexpr Vec2 thirdDerivative() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	tangentAt
		//
		////////////////////////////////////////////////////////////////

		/// @brief 正規化された接線ベクトルを返します。
		/// @param t パラメータ（0.0 ～ 1.0）
		/// @return 接線ベクトル
		[[nodiscard]]
		Vec2 tangentAt(double t) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	normalAt
		//
		////////////////////////////////////////////////////////////////

		/// @brief パラメータ t における進行方向左手の正規化法線ベクトルを返します。
		/// @param t パラメータ（0.0 ～ 1.0）
		/// @return 法線ベクトル
		[[nodiscard]]
		Vec2 normalAt(double t) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	headingAt
		//
		////////////////////////////////////////////////////////////////

		/// @brief 接線方向の角度（ラジアン）を返します。
		/// @param t パラメータ（0.0 ～ 1.0）
		/// @return 角度（-π～π）
		[[nodiscard]]
		double headingAt(double t) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	curvatureAt
		//
		////////////////////////////////////////////////////////////////

		/// @brief 曲率 κ(t) を返します（符号付き）。
		/// @param t パラメータ（0.0 ～ 1.0）
		/// @return 曲率
		[[nodiscard]]
		double curvatureAt(double t) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	radiusOfCurvatureAt
		//
		////////////////////////////////////////////////////////////////

		/// @brief 曲率半径 ρ(t) = 1/|κ(t)| を返します。
		/// @param t パラメータ（0.0 ～ 1.0）
		/// @return 曲率半径（直線に近い場合は Inf を返すことがあります）
		[[nodiscard]]
		double radiusOfCurvatureAt(double t) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	computeLength
		//
		////////////////////////////////////////////////////////////////

		/// @brief 曲線の弧長を数値積分により計算します。
		/// @return 曲線の弧長の近似値
		[[nodiscard]]
		double computeLength() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	computeTAtDistance
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した長さになる位置のパラメータ t を計算します。
		/// @param distanceFromStart 始点からの長さ
		/// @return パラメータ t（0.0 ～ 1.0）
		[[nodiscard]]
		double computeTAtDistance(double distanceFromStart) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	computePointAtDistance
		//
		////////////////////////////////////////////////////////////////

		/// @brief 始点からの長さが length になる位置の座標を計算します。
		/// @param distanceFromStart 始点からの長さ
		/// @return 座標
		[[nodiscard]]
		position_type computePointAtDistance(double distanceFromStart) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//  computeClosestT
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した点に最も近い曲線上の位置のパラメータ t を返します。
		/// @param targetPoint 対象の点
		/// @return パラメータ t（0.0～1.0）
		[[nodiscard]]
		double computeClosestT(const position_type& targetPoint) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//  computeClosestPoint
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した点に最も近い曲線上の点を返します。
		/// @param targetPoint 対象の点
		/// @return 曲線上の最近傍点
		[[nodiscard]]
		position_type computeClosestPoint(const position_type& targetPoint) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	toLineString
		//
		////////////////////////////////////////////////////////////////

		/// @brief パラメータ t を等分して曲線を LineString で近似します。
		/// @param segments 分割数（最低 1）
		/// @return LineString
		[[nodiscard]]
		LineString toLineString(int32 segments = 24) const;

		////////////////////////////////////////////////////////////////
		//
		//	toLineStringAdaptive
		//
		////////////////////////////////////////////////////////////////

		/// @brief 誤差に応じて適応的に分割した LineString を返します。
		/// @param maxError 許容誤差
		/// @param maxDepth 最大再帰深度
		/// @return LineString
		LineString toLineStringAdaptive(double maxError = 0.48, int32 maxDepth = 5) const;

		////////////////////////////////////////////////////////////////
		//
		//	split
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した t で曲線を 2 つに分割します（De Casteljau 法）。
		/// @param t 分割位置（0.0～1.0）
		/// @return 分割された 2 つの三次ベジェ曲線（first: [0,t], second: [t,1]）
		[[nodiscard]]
		constexpr std::pair<Bezier3, Bezier3> split(double t) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	subcurve
		//
		////////////////////////////////////////////////////////////////

		/// @brief 区間 [t0, t1] に対応する部分曲線を返します。
		/// @param t0 開始パラメータ
		/// @param t1 終了パラメータ
		/// @return 部分曲線（t1 < t0 の場合は反転して返されます）
		[[nodiscard]]
		Bezier3 subcurve(double t0, double t1) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	reverse, reversed
		//
		////////////////////////////////////////////////////////////////

		/// @brief 曲線の向きを反転します（p0 と p3 を入れ替え、制御点順も反転します）。
		/// @return *this
		constexpr Bezier3& reverse() noexcept;

		/// @brief 反転した新しい曲線を返します。
		/// @return 反転後の三次ベジェ曲線
		[[nodiscard]]
		constexpr Bezier3 reversed() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	controlPointsBoundingRect
		//
		////////////////////////////////////////////////////////////////

		/// @brief 制御点を包含する軸平行矩形（AABB）を返します。
		/// @return バウンディングボックス
		[[nodiscard]]
		constexpr RectF controlPointsBoundingRect() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	computeBoundingRect
		//
		////////////////////////////////////////////////////////////////

		/// @brief 曲線を包含する軸平行矩形（AABB）を返します。
		/// @return バウンディングボックス
		[[nodiscard]]
		RectF computeBoundingRect() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	p0p1, p1p2, p2p3, p0p3
		//
		////////////////////////////////////////////////////////////////

		/// @brief p0 から p1 への線分を返します。
		/// @return Line{ p0, p1 }
		[[nodiscard]]
		constexpr Line p0p1() const noexcept;

		/// @brief p1 から p2 への線分を返します。
		/// @return Line{ p1, p2 }
		[[nodiscard]]
		constexpr Line p1p2() const noexcept;

		/// @brief p2 から p3 への線分を返します。
		/// @return Line{ p2, p3 }
		[[nodiscard]]
		constexpr Line p2p3() const noexcept;

		/// @brief p0 から p3 への線分を返します。
		/// @return Line{ p0, p3 }
		[[nodiscard]]
		constexpr Line p0p3() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	p1p0, p2p1, p3p2, p3p0
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr Line p1p0() const noexcept;

		[[nodiscard]]
		constexpr Line p2p1() const noexcept;

		[[nodiscard]]
		constexpr Line p3p2() const noexcept;

		[[nodiscard]]
		constexpr Line p3p0() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	computeInflectionTs
		//
		////////////////////////////////////////////////////////////////

		/// @brief 反曲点（inflection）候補の t を返します。
		/// @remark 0～1 の範囲にある解のみ返します。存在しない場合は空配列。
		[[nodiscard]]
		Array<double> computeInflectionTs() const;

		////////////////////////////////////////////////////////////////
		//
		//	lerp
		//
		////////////////////////////////////////////////////////////////

		/// @brief 別の三次ベジェ曲線との線形補間を計算します。
		/// @param other 別の三次ベジェ曲線
		/// @param f 補間係数
		/// @return 線形補間された三次ベジェ曲線
		[[nodiscard]]
		constexpr Bezier3 lerp(const Bezier3& other, double f) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	approximatedBezier2
		//
		////////////////////////////////////////////////////////////////

		/// @brief この三次ベジェ曲線を近似する二次ベジェ曲線を返します。
		/// @return 近似された二次ベジェ曲線
		[[nodiscard]]
		constexpr Bezier2 approximatedBezier2() const noexcept;

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
		//	intersects
		//
		////////////////////////////////////////////////////////////////

		template <class Shape2DType>
		[[nodiscard]]
		bool intersects(const Shape2DType& other) const;

		////////////////////////////////////////////////////////////////
		//
		//	intersectsAt
		//
		////////////////////////////////////////////////////////////////

		//template <class Shape2DType>
		//[[nodiscard]]
		//Optional<Array<Vec2>> intersectsAt(const Shape2DType& other) const;

		////////////////////////////////////////////////////////////////
		//
		//	paint
		//
		////////////////////////////////////////////////////////////////

		const Bezier3& paint(Image& dst, const Color& color, EnableAntialiasing enableAntialiasing = EnableAntialiasing::Yes) const;

		const Bezier3& paint(Image& dst, double thickness, const Color& color, EnableAntialiasing enableAntialiasing = EnableAntialiasing::Yes) const;

		const Bezier3& paint(Image& dst, LineCap lineCap, double thickness, const Color& color, EnableAntialiasing enableAntialiasing = EnableAntialiasing::Yes) const;

		////////////////////////////////////////////////////////////////
		//
		//	overwrite
		//
		////////////////////////////////////////////////////////////////

		const Bezier3& overwrite(Image& dst, const Color& color, EnableAntialiasing enableAntialiasing = EnableAntialiasing::Yes) const;

		const Bezier3& overwrite(Image& dst, double thickness, const Color& color, EnableAntialiasing enableAntialiasing = EnableAntialiasing::Yes) const;

		const Bezier3& overwrite(Image& dst, LineCap lineCap, double thickness, const Color& color, EnableAntialiasing enableAntialiasing = EnableAntialiasing::Yes) const;

		////////////////////////////////////////////////////////////////
		//
		//	draw
		//
		////////////////////////////////////////////////////////////////

		/// @brief 曲線を描画します。
		/// @param color 色
		/// @param segments 分割数（最低 1）
		/// @return *this
		const Bezier3& draw(const ColorF& color = Palette::White, int32 segments = 24) const;

		/// @brief 曲線を描画します。
		/// @param thickness 太さ
		/// @param color 色
		/// @param segments 分割数（最低 1）
		/// @return *this
		const Bezier3& draw(double thickness, const ColorF& color = Palette::White, int32 segments = 24) const;

		/// @brief 曲線を描画します。
		/// @param lineCap 線端の種類
		/// @param thickness 太さ
		/// @param color 色
		/// @param segments 分割数（最低 1）
		/// @return *this
		const Bezier3& draw(LineCap lineCap, double thickness, const ColorF& color = Palette::White, int32 segments = 24) const;

		const Bezier3& draw(LineCap startCap, LineCap endCap, double thickness, const ColorF& color = Palette::White, int32 segments = 24) const;

		////////////////////////////////////////////////////////////////
		//
		//	drawAdaptive
		//
		////////////////////////////////////////////////////////////////

		/// @brief 曲線を適応分割で描画します。
		/// @param color 色
		/// @param maxError 許容誤差
		/// @param maxDepth 最大再帰深度
		/// @return *this
		const Bezier3& drawAdaptive(const ColorF& color = Palette::White, double maxError = 0.48, int32 maxDepth = 5) const;

		/// @brief 曲線を適応分割で描画します。
		/// @param thickness 太さ
		/// @param color 色
		/// @param maxError 許容誤差
		/// @param maxDepth 最大再帰深度
		/// @return *this
		const Bezier3& drawAdaptive(double thickness, const ColorF& color = Palette::White, double maxError = 0.48, int32 maxDepth = 5) const;

		/// @brief 曲線を適応分割で描画します。
		/// @param lineCap 線端の種類
		/// @param thickness 太さ
		/// @param color 色
		/// @param maxError 許容誤差
		/// @param maxDepth 最大再帰深度
		/// @return *this
		const Bezier3& drawAdaptive(LineCap lineCap, double thickness, const ColorF& color = Palette::White, double maxError = 0.48, int32 maxDepth = 5) const;

		const Bezier3& drawAdaptive(LineCap startCap, LineCap endCap, double thickness, const ColorF& color = Palette::White, double maxError = 0.48, int32 maxDepth = 5) const;

		////////////////////////////////////////////////////////////////
		//
		//	FromHermite
		//
		////////////////////////////////////////////////////////////////

		/// @brief 始点・終点の位置と速度（接線）から曲線を作成します（Hermite → Bezier）。
		/// @param p0 開始点
		/// @param v0 開始点の速度ベクトル
		/// @param p3 終了点
		/// @param v3 終了点の速度ベクトル
		[[nodiscard]]
		static constexpr Bezier3 FromHermite(const position_type& p0, const Vec2& v0, const position_type& p3, const Vec2& v3) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	FromCatmullRom
		//
		////////////////////////////////////////////////////////////////

		/// @brief Catmull-Rom スプラインの 1 区間を計算し、ベジェ曲線に変換します。
		/// @param pPrev 前の制御点（カーブの進入角度に影響します）
		/// @param p0 セグメント開始点（実際の描画開始位置）
		/// @param p3 セグメント終了点（実際の描画終了位置）
		/// @param pNext 次の制御点（カーブの脱出角度に影響します）
		/// @param tension テンション（0.0: 標準。1.0 に近づくほど直線的になり、負の値だと緩く膨らみます）
		[[nodiscard]]
		static constexpr Bezier3 FromCatmullRom(const position_type& pPrev, const position_type& p0, const position_type& p3, const position_type& pNext, double tension = 0.0) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator <<
		//
		////////////////////////////////////////////////////////////////

		/// @brief 出力ストリームに Bezier3 の内容を出力します。
		/// @tparam CharType 出力ストリームの文字型
		/// @param output 出力ストリーム
		/// @param value Bezier3
		/// @return 出力ストリーム
		template <class CharType>
		friend std::basic_ostream<CharType>& operator <<(std::basic_ostream<CharType>& output, const Bezier3& value)
		{
			return output << CharType('(')
				<< value.p0 << CharType(',') << CharType(' ')
				<< value.p1 << CharType(',') << CharType(' ')
				<< value.p2 << CharType(',') << CharType(' ')
				<< value.p3 << CharType(')');
		}

		////////////////////////////////////////////////////////////////
		//
		//	operator >>
		//
		////////////////////////////////////////////////////////////////
		
		/// @brief 入力ストリームから Bezier3 の内容を読み込みます。
		/// @tparam CharType 入力ストリームの文字型
		/// @param input 入力ストリーム
		/// @param value Bezier3 の格納先
		/// @return 入力ストリーム
		template <class CharType>
		friend std::basic_istream<CharType>& operator >>(std::basic_istream<CharType>& input, Bezier3& value)
		{
			CharType unused;
			return input >> unused
				>> value.p0 >> unused
				>> value.p1 >> unused
				>> value.p2 >> unused
				>> value.p3 >> unused;
		}

		////////////////////////////////////////////////////////////////
		//
		//	Formatter
		//
		////////////////////////////////////////////////////////////////

		/// @brief Bezier3 を文字列に変換します。
		/// @param formatData 文字列バッファ
		/// @param value Bezier3
		/// @remark この関数は Format 用の関数です。通常、ユーザーが直接呼び出す必要はありません。
		friend void Formatter(FormatData& formatData, const Bezier3& value);

	private:

		/// @brief パラメータ t における速度（|B'(t)|）を返します。
		double speed(double t) const noexcept;

		/// @brief 区間 [a, b] の速度を数値積分して弧長を求めます。
		double integrateSpeed(double a, double b) const noexcept;

		/// @brief controlPointAtIndex() の範囲外アクセス時に例外を送出します。
		[[noreturn]]
		static void ThrowControlPointAtIndexOutOfRange();
	};
}

////////////////////////////////////////////////////////////////
//
//	fmt
//
////////////////////////////////////////////////////////////////

template <>
struct fmt::formatter<s3d::Bezier3>
{
	std::string tag;

	constexpr auto parse(format_parse_context& ctx)
	{
		return s3d::FmtHelper::GetFormatTag(tag, ctx);
	}

	format_context::iterator format(const s3d::Bezier3& value, format_context& ctx) const;
};

template <>
struct fmt::formatter<s3d::Bezier3, s3d::char32>
{
	std::u32string tag;

	s3d::ParseContext::iterator parse(s3d::ParseContext& ctx);

	s3d::BufferContext::iterator format(const s3d::Bezier3& value, s3d::BufferContext& ctx) const;
};

////////////////////////////////////////////////////////////////
//
//	std::hash
//
////////////////////////////////////////////////////////////////

template <>
struct std::hash<s3d::Bezier3>
{
	[[nodiscard]]
	size_t operator ()(const s3d::Bezier3& value) const noexcept
	{
		return value.hash();
	}
};
