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
# include "LineString.hpp"
# include "ListUtility.hpp"

namespace s3d
{
	/// @brief 二次ベジェ曲線
	/// 3 点（開始点 p0、制御点 p1、終了点 p2）で定義される二次ベジェ曲線です。
	/// パラメータ t（0.0～1.0）に対して曲線上の座標・導関数（接線 / 法線）・長さ・分割などを提供します。
	struct Bezier2
	{
		/// @brief 座標型
		using position_type = Vec2;

		/// @brief 要素型
		using value_type = position_type::value_type;

		/// @brief 開始点
		position_type p0;

		/// @brief 制御点
		position_type p1;

		/// @brief 終了点
		position_type p2;

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		/// @brief デフォルトコンストラクタ
		[[nodiscard]]
		Bezier2() = default;

		/// @brief 3 点から二次ベジェ曲線を作成します。
		/// @param _p0 開始点
		/// @param _p1 制御点
		/// @param _p2 終了点
		[[nodiscard]]
		constexpr Bezier2(const position_type& _p0, const position_type& _p1, const position_type& _p2) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	withP0
		//
		////////////////////////////////////////////////////////////////

		/// @brief 開始点 p0 を変更した新しい曲線を返します。
		/// @param _x 開始点の X 座標
		/// @param _y 開始点の Y 座標
		/// @return p0 を変更した二次ベジェ曲線
		[[nodiscard]]
		constexpr Bezier2 withP0(value_type _x, value_type _y) const noexcept;

		/// @brief 開始点 p0 を変更した新しい曲線を返します。
		/// @param _p0 開始点
		/// @return p0 を変更した二次ベジェ曲線
		[[nodiscard]]
		constexpr Bezier2 withP0(position_type _p0) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	withP1
		//
		////////////////////////////////////////////////////////////////

		/// @brief 制御点 p1 を変更した新しい曲線を返します。
		/// @param _x 制御点の X 座標
		/// @param _y 制御点の Y 座標
		/// @return p1 を変更した二次ベジェ曲線
		[[nodiscard]]
		constexpr Bezier2 withP1(value_type _x, value_type _y) const noexcept;
		
		/// @brief 制御点 p1 を変更した新しい曲線を返します。
		/// @param _p1 制御点
		/// @return p1 を変更した二次ベジェ曲線
		[[nodiscard]]
		constexpr Bezier2 withP1(position_type _p1) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	withP2
		//
		////////////////////////////////////////////////////////////////

		/// @brief 終了点 p2 を変更した新しい曲線を返します。
		/// @param _x 終了点の X 座標
		/// @param _y 終了点の Y 座標
		/// @return p2 を変更した二次ベジェ曲線
		[[nodiscard]]
		constexpr Bezier2 withP2(value_type _x, value_type _y) const noexcept;

		/// @brief 終了点 p2 を変更した新しい曲線を返します。
		/// @param _p2 終了点
		/// @return p2 を変更した二次ベジェ曲線
		[[nodiscard]]
		constexpr Bezier2 withP2(position_type _p2) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	withOffset
		//
		////////////////////////////////////////////////////////////////

		/// @brief 3 点すべてにオフセットを加えた新しい曲線を返します。
		/// @param x X 方向のオフセット
		/// @param y Y 方向のオフセット
		/// @return オフセット後の二次ベジェ曲線
		[[nodiscard]]
		constexpr Bezier2 withOffset(value_type x, value_type y) const noexcept;

		/// @brief 3 点すべてにオフセットを加えた新しい曲線を返します。
		/// @param v オフセット
		/// @return オフセット後の二次ベジェ曲線
		[[nodiscard]]
		constexpr Bezier2 withOffset(position_type v) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	withOffsetX, withOffsetY
		//
		////////////////////////////////////////////////////////////////

		/// @brief 3 点すべてに X 方向のオフセットを加えた新しい曲線を返します。
		/// @param x X 方向のオフセット
		/// @return オフセット後の二次ベジェ曲線
		[[nodiscard]]
		constexpr Bezier2 withOffsetX(value_type x) const noexcept;

		/// @brief 3 点すべてに Y 方向のオフセットを加えた新しい曲線を返します。
		/// @param y Y 方向のオフセット
		/// @return オフセット後の二次ベジェ曲線
		[[nodiscard]]
		constexpr Bezier2 withOffsetY(value_type y) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	movedBy
		//
		////////////////////////////////////////////////////////////////

		/// @brief 平行移動した新しい曲線を返します。
		/// @param x X 方向の移動量
		/// @param y Y 方向の移動量
		/// @return 移動後の二次ベジェ曲線
		[[nodiscard]]
		constexpr Bezier2 movedBy(value_type x, value_type y) const noexcept;

		/// @brief 平行移動した新しい曲線を返します。
		/// @param v 移動量
		/// @return 移動後の二次ベジェ曲線
		[[nodiscard]]
		constexpr Bezier2 movedBy(position_type v) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	moveBy
		//
		////////////////////////////////////////////////////////////////

		/// @brief 曲線を平行移動します。
		/// @param x X 方向の移動量
		/// @param y Y 方向の移動量
		/// @return *this
		constexpr Bezier2& moveBy(value_type x, value_type y) noexcept;

		/// @brief 曲線を平行移動します。
		/// @param v 移動量
		/// @return *this
		constexpr Bezier2& moveBy(position_type v) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	pointAtIndex
		//
		////////////////////////////////////////////////////////////////

		/// @brief インデックスで制御点を取得します。
		/// @param index 0: p0, 1: p1, 2: p2
		/// @return 指定した点への参照
		/// @throw std::out_of_range index が 0～2 の範囲外の場合
		[[nodiscard]]
		position_type& pointAtIndex(size_t index);

		/// @brief インデックスで制御点を取得します。
		/// @param index 0: p0, 1: p1, 2: p2
		/// @return 指定した点への参照
		/// @throw std::out_of_range index が 0～2 の範囲外の場合
		[[nodiscard]]
		const position_type& pointAtIndex(size_t index) const;

		////////////////////////////////////////////////////////////////
		//
		//	getPos
		//
		////////////////////////////////////////////////////////////////

		/// @brief パラメータ t に対応する曲線上の座標を返します。
		/// @param t パラメータ（0.0～1.0）
		/// @return 曲線上の座標
		[[nodiscard]]
		constexpr position_type getPos(const double t) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	getDerivative
		//
		////////////////////////////////////////////////////////////////

		/// @brief パラメータ t における 1 階導関数（速度ベクトル）を返します。
		/// @param t パラメータ（0.0～1.0）
		/// @return 1 階導関数ベクトル
		[[nodiscard]]
		constexpr Vec2 getDerivative(double t) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	getSecondDerivative
		//
		////////////////////////////////////////////////////////////////

		/// @brief 2 階導関数（加速度ベクトル）を返します。
		/// @return 2 階導関数ベクトル
		[[nodiscard]]
		constexpr Vec2 getSecondDerivative() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	getTangent
		//
		////////////////////////////////////////////////////////////////

		/// @brief 正規化された接線ベクトルを返します。
		/// @param t パラメータ（0.0 ～ 1.0）
		/// @return 接線ベクトル
		[[nodiscard]]
		Vec2 getTangent(double t) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	getNormal
		//
		////////////////////////////////////////////////////////////////

		/// @brief 正規化された法線ベクトルを返します。
		/// @param t パラメータ（0.0 ～ 1.0）
		/// @return 法線ベクトル
		[[nodiscard]]
		Vec2 getNormal(double t) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	getHeading
		//
		////////////////////////////////////////////////////////////////

		/// @brief 曲線上の進行方向（接線方向）の角度を返します。
		/// @param t パラメータ（0.0 ～ 1.0）
		/// @return 角度（ラジアン）
		[[nodiscard]]
		double getHeading(double t) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	getCurvature
		//
		////////////////////////////////////////////////////////////////

		/// @brief 曲線の曲率 κ(t)（どれだけ曲がっているか）を返します（符号付き）。
		/// @param t パラメータ（0.0 ～ 1.0）
		/// @return 曲率 κ（単位: 1 / 長さ）
		/// @remark
		/// - 曲率は「半径 R の円」と同じ曲がり具合なら `κ = 1 / R` になります。
		/// - 値が大きいほど急カーブ、小さいほど直線に近いことを意味します。
		/// - 符号は 2D の回転方向を表し、Siv3D では
		///   - 負: 左回り（反時計回り）に曲がる
		///   - 正: 右回り（時計回り）に曲がる
		///   - 0: 直線
		///   を意味します。
		/// - ゲーム用途では「カーブがきついほど速度を落とす」「操舵量を決める」などに使えます
		[[nodiscard]]
		double getCurvature(double t) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	getCurvatureRadius
		//
		////////////////////////////////////////////////////////////////

		/// @brief 曲率半径 ρ(t)（その場で接する円の半径）を返します。
		/// @param t パラメータ（0.0 ～ 1.0）
		/// @return 曲率半径 ρ（単位: 長さ）
		[[nodiscard]]
		double getCurvatureRadius(double t) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	computeLength
		//
		////////////////////////////////////////////////////////////////

		/// @brief 曲線の長さを計算します。
		/// @return 曲線の長さ
		[[nodiscard]]
		double computeLength() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	tAtLength
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した長さになる位置のパラメータ t を計算します。
		/// @param length 長さ
		/// @return パラメータ t（0.0 ～ 1.0）
		[[nodiscard]]
		double tAtLength(double length) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	getPosAtLength
		//
		////////////////////////////////////////////////////////////////

		/// @brief 始点からの長さが length になる位置の座標を計算します。
		/// @param length 長さ
		/// @return 座標
		[[nodiscard]]
		position_type getPosAtLength(double length) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//  closestT
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した点に最も近い曲線上の位置のパラメータ t を返します。
		/// @param point 点
		/// @return パラメータ t（0.0～1.0）
		[[nodiscard]]
		double closestT(const position_type& point) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//  closestPoint
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した点に最も近い曲線上の点を返します。
		/// @param point 点
		/// @return 曲線上の最近傍点
		[[nodiscard]]
		position_type closestPoint(const position_type& point) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	getLineString
		//
		////////////////////////////////////////////////////////////////

		/// @brief 曲線を等間隔の t で分割した LineString を返します。
		/// @param segments 分割数（最低 1）
		/// @return LineString
		[[nodiscard]]
		LineString getLineString(int32 segments = 24) const;

		////////////////////////////////////////////////////////////////
		//
		//	getLineStringAdaptive
		//
		////////////////////////////////////////////////////////////////

		/// @brief 誤差に応じて適応的に分割した LineString を返します。
		/// @param maxError 許容誤差
		/// @param maxDepth 最大再帰深度
		/// @return LineString
		LineString getLineStringAdaptive(double maxError = 0.48, int32 maxDepth = 5) const;

		////////////////////////////////////////////////////////////////
		//
		//	split
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した t で曲線を 2 つに分割します（De Casteljau 法）。
		/// @param t 分割位置（0.0～1.0）
		/// @return 分割された 2 つの二次ベジェ曲線（first: [0,t], second: [t,1]）
		[[nodiscard]]
		constexpr std::pair<Bezier2, Bezier2> split(double t) const noexcept;

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
		Bezier2 subcurve(double t0, double t1) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	reverse, reversed
		//
		////////////////////////////////////////////////////////////////

		/// @brief 曲線の向きを反転します（p0 と p2 を入れ替えます）。
		/// @return *this
		constexpr Bezier2& reverse() noexcept;

		/// @brief 反転した新しい曲線を返します。
		/// @return 反転後の二次ベジェ曲線
		[[nodiscard]]
		constexpr Bezier2 reversed() const noexcept;

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
		//	boundingRect
		//
		////////////////////////////////////////////////////////////////

		/// @brief 曲線を包含する軸平行矩形（AABB）を返します。
		/// @return バウンディングボックス
		[[nodiscard]]
		RectF boundingRect() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	p0p1, p1p2, p0p2
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

		/// @brief p0 から p2 への線分を返します。
		/// @return Line{ p0, p2 }
		[[nodiscard]]
		constexpr Line p0p2() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	p1p0, p2p1, p2p0
		//
		////////////////////////////////////////////////////////////////

		/// @brief p1 から p0 への線分を返します。
		/// @return Line{ p1, p0 }
		[[nodiscard]]
		constexpr Line p1p0() const noexcept;

		/// @brief p2 から p1 への線分を返します。
		/// @return Line{ p2, p1 }
		[[nodiscard]]
		constexpr Line p2p1() const noexcept;

		/// @brief p2 から p0 への線分を返します。
		/// @return Line{ p2, p0 }
		[[nodiscard]]
		constexpr Line p2p0() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	lerp
		//
		////////////////////////////////////////////////////////////////

		/// @brief 別の二次ベジェ曲線との線形補間を計算します。
		/// @param other 別の二次ベジェ曲線
		/// @param f 補間係数
		/// @return 線形補間された二次ベジェ曲線
		[[nodiscard]]
		constexpr Bezier2 lerp(const Bezier2& other, double f) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	toBezier3
		//
		////////////////////////////////////////////////////////////////

		/// @brief 二次ベジェ曲線を三次ベジェ曲線に変換します。
		/// @return 三次ベジェ曲線
		[[nodiscard]]
		constexpr Bezier3 toBezier3() const noexcept;

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
		//	draw
		//
		////////////////////////////////////////////////////////////////

		/// @brief 曲線を描画します。
		/// @param color 色
		/// @param segments 分割数（最低 1）
		/// @return *this
		const Bezier2& draw(const ColorF& color = Palette::White, int32 segments = 24) const;

		/// @brief 曲線を描画します。
		/// @param thickness 太さ
		/// @param color 色
		/// @param segments 分割数（最低 1）
		/// @return *this
		const Bezier2& draw(double thickness, const ColorF& color = Palette::White, int32 segments = 24) const;

		/// @brief 曲線を描画します。
		/// @param lineCap 線端の種類
		/// @param thickness 太さ
		/// @param color 色
		/// @param segments 分割数（最低 1）
		/// @return *this
		const Bezier2& draw(LineCap lineCap, double thickness, const ColorF& color = Palette::White, int32 segments = 24) const;

		const Bezier2& draw(LineCap startCap, LineCap endCap, double thickness, const ColorF& color = Palette::White, int32 segments = 24) const;

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
		const Bezier2& drawAdaptive(const ColorF& color = Palette::White, double maxError = 0.48, int32 maxDepth = 5) const;

		/// @brief 曲線を適応分割で描画します。
		/// @param thickness 太さ
		/// @param color 色
		/// @param maxError 許容誤差
		/// @param maxDepth 最大再帰深度
		/// @return *this
		const Bezier2& drawAdaptive(double thickness, const ColorF& color = Palette::White, double maxError = 0.48, int32 maxDepth = 5) const;

		/// @brief 曲線を適応分割で描画します。
		/// @param lineCap 線端の種類
		/// @param thickness 太さ
		/// @param color 色
		/// @param maxError 許容誤差
		/// @param maxDepth 最大再帰深度
		/// @return *this
		const Bezier2& drawAdaptive(LineCap lineCap, double thickness, const ColorF& color = Palette::White, double maxError = 0.48, int32 maxDepth = 5) const;

		const Bezier2& drawAdaptive(LineCap startCap, LineCap endCap, double thickness, const ColorF& color = Palette::White, double maxError = 0.48, int32 maxDepth = 5) const;

		////////////////////////////////////////////////////////////////
		//
		//	FromMidpoint
		//
		////////////////////////////////////////////////////////////////

		/// @brief t = 0.5 で指定した中間点を通る二次ベジェ曲線を作成します。
		/// @param p0 開始点
		/// @param midpoint t = 0.5 の中間点
		/// @param p2 終了点
		/// @return 二次ベジェ曲線
		[[nodiscard]]
		static constexpr Bezier2 FromMidpoint(const position_type& p0, const position_type& midpoint, const position_type& p2) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator <<
		//
		////////////////////////////////////////////////////////////////

		/// @brief 出力ストリームに Bezier2 の内容を出力します。
		/// @tparam CharType 出力ストリームの文字型
		/// @param output 出力ストリーム
		/// @param value Bezier2
		/// @return 出力ストリーム
		template <class CharType>
		friend std::basic_ostream<CharType>& operator <<(std::basic_ostream<CharType>& output, const Bezier2& value)
		{
			return output << CharType('(')
				<< value.p0 << CharType(',') << CharType(' ')
				<< value.p1 << CharType(',') << CharType(' ')
				<< value.p2 << CharType(')');
		}

		////////////////////////////////////////////////////////////////
		//
		//	operator >>
		//
		////////////////////////////////////////////////////////////////

		/// @brief 入力ストリームから Bezier2 の内容を読み込みます。
		/// @tparam CharType 入力ストリームの文字型
		/// @param input 入力ストリーム
		/// @param value Bezier2 の格納先
		/// @return 入力ストリーム
		template <class CharType>
		friend std::basic_istream<CharType>& operator >>(std::basic_istream<CharType>& input, Bezier2& value)
		{
			CharType unused;
			return input >> unused
				>> value.p0 >> unused
				>> value.p1 >> unused
				>> value.p2 >> unused;
		}

		////////////////////////////////////////////////////////////////
		//
		//	Formatter
		//
		////////////////////////////////////////////////////////////////

		/// @brief Bezier2 を文字列に変換します。
		/// @param formatData 文字列バッファ
		/// @param value Bezier2
		/// @remark この関数は Format 用の関数です。通常、ユーザーが直接呼び出す必要はありません。
		friend void Formatter(FormatData& formatData, const Bezier2& value);

	private:

		/// @brief パラメータ t における速度（|B'(t)|）を返します。
		/// @param t パラメータ
		/// @return 速度
		double speed(double t) const noexcept;

		/// @brief 区間 [a, b] の速度を数値積分して弧長を求めます。
		/// @param a 区間開始（0.0～1.0）
		/// @param b 区間終了（0.0～1.0）
		/// @return 弧長
		double integrateSpeed(double a, double b) const noexcept;

		/// @brief pointAtIndex() の範囲外アクセス時に例外を送出します。
		[[noreturn]]
		static void ThrowPointAtIndexOutOfRange();
	};
}

////////////////////////////////////////////////////////////////
//
//	fmt
//
////////////////////////////////////////////////////////////////

template <>
struct fmt::formatter<s3d::Bezier2>
{
	std::string tag;

	constexpr auto parse(format_parse_context& ctx)
	{
		return s3d::FmtHelper::GetFormatTag(tag, ctx);
	}

	format_context::iterator format(const s3d::Bezier2& value, format_context& ctx);
};

template <>
struct fmt::formatter<s3d::Bezier2, s3d::char32>
{
	std::u32string tag;

	s3d::ParseContext::iterator parse(s3d::ParseContext& ctx);

	s3d::BufferContext::iterator format(const s3d::Bezier2& value, s3d::BufferContext& ctx);
};

////////////////////////////////////////////////////////////////
//
//	std::hash
//
////////////////////////////////////////////////////////////////

template <>
struct std::hash<s3d::Bezier2>
{
	[[nodiscard]]
	size_t operator ()(const s3d::Bezier2& value) const noexcept
	{
		return value.hash();
	}
};
