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
# include <memory>
# include "Array.hpp"
# include "Common.hpp"
# include "PointVector.hpp"
# include "2DShapes.hpp"
# include "TriangleIndex.hpp"
# include "PolygonFailureType.hpp"
# include "QualityFactor.hpp"
# include "PredefinedYesNo.hpp"
# include "PolygonHolesView.hpp"

namespace s3d
{
	struct PatternParameters;

	////////////////////////////////////////////////////////////////
	//
	//	Polygon
	//
	////////////////////////////////////////////////////////////////

	/// @brief 多角形（穴をもつことも可能）
	class Polygon
	{
	private:

		class PolygonDetail;

	public:

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		/// @brief デフォルトコンストラクタ
		[[nodiscard]]
		Polygon();

		/// @brief 多角形をコピーして作成します。
		/// @param other コピーする多角形
		[[nodiscard]]
		Polygon(const Polygon& other);

		/// @brief 多角形をムーブして作成します。
		/// @param other ムーブする多角形
		[[nodiscard]]
		Polygon(Polygon&& other) noexcept;

		/// @brief 多角形を作成します。
		/// @param outer 外周の頂点配列（時計回り）
		/// @param skipValidation 頂点の位置の妥当性を検証しない場合は `SkipValidation::Yes`
		[[nodiscard]]
		explicit Polygon(std::initializer_list<Vec2> outer, SkipValidation skipValidation = SkipValidation::No);

		/// @brief 多角形を作成します。
		/// @param outer 外周の頂点配列（時計回り）
		/// @param holes 穴を構成する頂点配列（反時計回り）の配列
		/// @param skipValidation 頂点の位置の妥当性を検証しない場合は `SkipValidation::Yes`
		[[nodiscard]]
		explicit Polygon(std::span<const Vec2> outer, SkipValidation skipValidation = SkipValidation::No);

		/// @brief 多角形を作成します。
		/// @param outer 外周の頂点配列（時計回り）
		/// @param boundingRect バウンディングボックス
		/// @param skipValidation 頂点の位置の妥当性を検証しない場合は `SkipValidation::Yes`
		[[nodiscard]]
		Polygon(std::initializer_list<Vec2> outer, const RectF& boundingRect, SkipValidation skipValidation = SkipValidation::No);

		/// @brief 多角形を作成します。
		/// @param outer 外周の頂点配列（時計回り）
		/// @param boundingRect バウンディングボックス
		/// @param skipValidation 頂点の位置の妥当性を検証しない場合は `SkipValidation::Yes`
		[[nodiscard]]
		Polygon(std::span<const Vec2> outer, const RectF& boundingRect, SkipValidation skipValidation = SkipValidation::No);

		/// @brief 多角形を作成します。
		/// @param outer 外周の頂点配列（時計回り）
		/// @param holes 穴を構成する頂点配列（反時計回り）の配列
		/// @param skipValidation 頂点の位置の妥当性を検証しない場合は `SkipValidation::Yes`
		[[nodiscard]]
		Polygon(std::initializer_list<Vec2> outer, Array<Array<Vec2>> holes, SkipValidation skipValidation = SkipValidation::No);

		/// @brief 多角形を作成します。
		/// @param outer 外周の頂点配列（時計回り）
		/// @param holes 穴を構成する頂点配列（反時計回り）の配列
		/// @param skipValidation 頂点の位置の妥当性を検証しない場合は `SkipValidation::Yes`
		[[nodiscard]]
		Polygon(std::span<const Vec2> outer, Array<Array<Vec2>> holes, SkipValidation skipValidation = SkipValidation::No);

		/// @brief 多角形を作成します。
		/// @param outer 外周の頂点配列（時計回り）
		/// @param holes 穴を構成する頂点配列（反時計回り）の配列
		/// @param boundingRect バウンディングボックス
		/// @param skipValidation 頂点の位置の妥当性を検証しない場合は `SkipValidation::Yes`
		[[nodiscard]]
		Polygon(std::initializer_list<Vec2> outer, Array<Array<Vec2>> holes, const RectF& boundingRect, SkipValidation skipValidation = SkipValidation::No);

		/// @brief 多角形を作成します。
		/// @param outer 外周の頂点配列（時計回り）
		/// @param holes 穴を構成する頂点配列（反時計回り）の配列
		/// @param boundingRect バウンディングボックス
		/// @param skipValidation 頂点の位置の妥当性を検証しない場合は `SkipValidation::Yes`
		[[nodiscard]]
		Polygon(std::span<const Vec2> outer, Array<Array<Vec2>> holes, const RectF& boundingRect, SkipValidation skipValidation = SkipValidation::No);

		/// @brief 多角形を作成します。
		/// @param outer 外周の頂点配列（時計回り）
		/// @param indices 三角形分割の頂点インデックス
		/// @param boundingRect バウンディングボックス
		/// @param skipValidation 頂点の位置の妥当性を検証しない場合は `SkipValidation::Yes`
		[[nodiscard]]
		Polygon(std::initializer_list<Vec2> outer, Array<TriangleIndex> indices, const RectF& boundingRect, SkipValidation skipValidation = SkipValidation::No);

		/// @brief 多角形を作成します。
		/// @param outer 外周の頂点配列（時計回り）
		/// @param indices 三角形分割の頂点インデックス
		/// @param boundingRect バウンディングボックス
		/// @param skipValidation 頂点の位置の妥当性を検証しない場合は `SkipValidation::Yes`
		[[nodiscard]]
		Polygon(std::span<const Vec2> outer, Array<TriangleIndex> indices, const RectF& boundingRect, SkipValidation skipValidation = SkipValidation::No);

		/// @brief 多角形を作成します。
		/// @param outer 外周の頂点配列（時計回り）
		/// @param holes 穴を構成する頂点配列（反時計回り）の配列
		/// @param vertices すべての頂点
		/// @param indices 三角形分割の頂点インデックス
		/// @param boundingRect バウンディングボックス
		/// @param skipValidation 頂点の位置の妥当性を検証しない場合は `SkipValidation::Yes`
		[[nodiscard]]
		Polygon(std::initializer_list<Vec2> outer, Array<Array<Vec2>> holes, Array<Float2> vertices, Array<TriangleIndex> indices, const RectF& boundingRect, SkipValidation skipValidation = SkipValidation::No);

		/// @brief 多角形を作成します。
		/// @param outer 外周の頂点配列（時計回り）
		/// @param holes 穴を構成する頂点配列（反時計回り）の配列
		/// @param vertices すべての頂点
		/// @param indices 三角形分割の頂点インデックス
		/// @param boundingRect バウンディングボックス
		/// @param skipValidation 頂点の位置の妥当性を検証しない場合は `SkipValidation::Yes`
		[[nodiscard]]
		Polygon(std::span<const Vec2> outer, Array<Array<Vec2>> holes, Array<Float2> vertices, Array<TriangleIndex> indices, const RectF& boundingRect, SkipValidation skipValidation = SkipValidation::No);

		/// @brief 多角形を作成します。
		/// @param shape 2D 形状
		[[nodiscard]]
		Polygon(const Shape2D& shape);

		////////////////////////////////////////////////////////////////
		//
		//	(destructor)
		//
		////////////////////////////////////////////////////////////////

		/// @brief デストラクタ
		~Polygon();

		////////////////////////////////////////////////////////////////
		//
		//	operator =
		//
		////////////////////////////////////////////////////////////////

		/// @brief 多角形をコピー代入します。
		/// @param other コピーする多角形
		/// @return *this
		Polygon& operator =(const Polygon& other);

		/// @brief 多角形をムーブ代入します。
		/// @param other ムーブする多角形
		/// @return *this
		Polygon& operator =(Polygon&& other) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	swap
		//
		////////////////////////////////////////////////////////////////

		/// @brief 多角形を別の多角形と交換します。
		/// @param other 交換する多角形
		void swap(Polygon& other) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	isEmpty
		//
		////////////////////////////////////////////////////////////////

		/// @brief 多角形が空であるかを返します。
		/// @return  多角形が空である場合 true, それ以外の場合は false
		[[nodiscard]]
		bool isEmpty() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator bool
		//
		////////////////////////////////////////////////////////////////

		/// @brief 多角形が空ではないかを返します。
		/// @return 多角形が空でない場合 true, それ以外の場合は false
		[[nodiscard]]
		explicit operator bool() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	hasHoles
		//
		////////////////////////////////////////////////////////////////

		/// @brief 多角形が穴を持っているかを返します。
		/// @return 穴を持つ場合 true, それ以外の場合は false
		[[nodiscard]]
		bool hasHoles() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	num_holes
		//
		////////////////////////////////////////////////////////////////

		/// @brief 多角形が持つ穴の個数を返します。
		/// @return 多角形が持つ穴の個数
		[[nodiscard]]
		size_t num_holes() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	outer
		//
		////////////////////////////////////////////////////////////////

		/// @brief 多角形の外周の頂点配列を返します。
		/// @return 多角形の外周の頂点配列
		[[nodiscard]]
		const Array<Vec2>& outer() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	inners
		//
		////////////////////////////////////////////////////////////////

		/// @brief 多角形の穴を構成する頂点配列を返します。
		/// @return 多角形の穴を構成する頂点配列
		[[nodiscard]]
		PolygonHolesView inners() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	vertices
		//
		////////////////////////////////////////////////////////////////

		/// @brief 多角形のすべての頂点を返します。
		/// @return 多角形のすべての頂点
		[[nodiscard]]
		const Array<Float2>& vertices() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	indices
		//
		////////////////////////////////////////////////////////////////

		/// @brief 多角形の三角形分割のすべての頂点インデックスを返します。
		/// @remark `vertices()` の戻り値に対応します。
		/// @return 多角形の三角形分割のすべての頂点インデックス
		[[nodiscard]]
		const Array<TriangleIndex>& indices() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	boundingRect
		//
		////////////////////////////////////////////////////////////////

		/// @brief 多角形のバウンディングボックスを返します。
		/// @remark バウンディングボックスは計算済みであるため、この関数のコストは小さいです。
		/// @return 多角形のバウンディングボックス
		[[nodiscard]]
		const RectF& boundingRect() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	num_triangles
		//
		////////////////////////////////////////////////////////////////

		/// @brief 多角形の三角形分割での三角形の個数を返します。
		/// @remark 三角形分割は計算済みであるため、この関数のコストは小さいです。
		/// @return 三角形の個数
		[[nodiscard]]
		size_t num_triangles() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	triangleAtIndex
		//
		////////////////////////////////////////////////////////////////

		/// @brief 多角形の三角形分割での、個々の三角形を返します。
		/// @param index 三角形のインデックス。0 以上 `(num_triangles() - 1)` 以下
		/// @return 三角形
		[[nodiscard]]
		Triangle triangleAtIndex(size_t index) const;

		////////////////////////////////////////////////////////////////
		//
		//	clear
		//
		////////////////////////////////////////////////////////////////

		/// @brief 多角形を空にします。
		void clear();

		////////////////////////////////////////////////////////////////
		//
		//	addHole
		//
		////////////////////////////////////////////////////////////////

		/// @brief 多角形に穴を追加します。
		/// @param rect 穴の形状
		/// @return 穴の追加に成功したら true, それ以外の場合は false
		bool addHole(const RectF& rect);

		/// @brief 多角形に穴を追加します。
		/// @param triangle 穴の形状
		/// @return 穴の追加に成功したら true, それ以外の場合は false
		bool addHole(const Triangle& triangle);

		/// @brief 多角形に穴を追加します。
		/// @param quad 穴の形状
		/// @return 穴の追加に成功したら true, それ以外の場合は false
		bool addHole(const Quad& quad);

		/// @brief 多角形に円形の穴を追加します。
		/// @param circle 穴の形状
		/// @param pointsPerCircle 円を近似する頂点数
		/// @return 穴の追加に成功したら true, それ以外の場合は false
		bool addHole(const Circle& circle, const PointsPerCircle& pointsPerCircle);

		/// @brief 多角形に円形の穴を追加します。
		/// @param circle 穴の形状
		/// @param qualityFactor 円を近似する品質係数
		/// @return 穴の追加に成功したら true, それ以外の場合は false
		bool addHole(const Circle& circle, const QualityFactor& qualityFactor = QualityFactor{ 1.0 });

		/// @brief 多角形に楕円形の穴を追加します。
		/// @param ellipse 穴の形状
		/// @param pointsPerCircle 楕円を近似する頂点数
		/// @return 穴の追加に成功したら true, それ以外の場合は false
		bool addHole(const Ellipse& ellipse, const PointsPerCircle& pointsPerCircle);

		/// @brief 多角形に楕円形の穴を追加します。
		/// @param ellipse 穴の形状
		/// @param qualityFactor 楕円を近似する品質係数
		/// @return 穴の追加に成功したら true, それ以外の場合は false
		bool addHole(const Ellipse& ellipse, const QualityFactor& qualityFactor = QualityFactor{ 1.0 });

		/// @brief 多角形に角丸長方形の穴を追加します。
		/// @param roundRect 穴の形状
		/// @param pointsPerCircle 角の円弧を近似する頂点数
		/// @return 穴の追加に成功したら true, それ以外の場合は false
		bool addHole(const RoundRect& roundRect, const PointsPerCircle& pointsPerCircle);

		/// @brief 多角形に角丸長方形の穴を追加します。
		/// @param roundRect 穴の形状
		/// @param qualityFactor 角の円弧を近似する品質係数
		/// @return 穴の追加に成功したら true, それ以外の場合は false
		bool addHole(const RoundRect& roundRect, const QualityFactor& qualityFactor = QualityFactor{ 1.0 });

		/// @brief 多角形に穴を追加します。
		/// @param hole 穴を構成する頂点配列
		/// @return 穴の追加に成功したら true, それ以外の場合は false
		bool addHole(Array<Vec2> hole);

		////////////////////////////////////////////////////////////////
		//
		//	addHoles
		//
		////////////////////////////////////////////////////////////////

		/// @brief 多角形に穴を追加します。
		/// @param holes 穴を構成する頂点配列の配列（複数の穴）
		/// @return 穴の追加に成功したら true, それ以外の場合は false
		bool addHoles(Array<Array<Vec2>> holes);

		////////////////////////////////////////////////////////////////
		//
		//	movedBy
		//
		////////////////////////////////////////////////////////////////

		/// @brief 平行移動した多角形を返します。
		/// @param x X 方向の移動量
		/// @param y Y 方向の移動量
		/// @return 平行移動した多角形
		[[nodiscard]]
		Polygon movedBy(double x, double y) const&;

		/// @brief 平行移動した多角形を返します。
		/// @param x X 方向の移動量
		/// @param y Y 方向の移動量
		/// @return 平行移動した多角形
		[[nodiscard]]
		Polygon movedBy(double x, double y) && noexcept;

		/// @brief 平行移動した多角形を返します。
		/// @param v 移動量
		/// @return 平行移動した多角形
		[[nodiscard]]
		Polygon movedBy(Vec2 v) const&;

		/// @brief 平行移動した多角形を返します。
		/// @param v 移動量
		/// @return 平行移動した多角形
		[[nodiscard]]
		Polygon movedBy(Vec2 v) && noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	moveBy
		//
		////////////////////////////////////////////////////////////////

		/// @brief 多角形を平行移動します。
		/// @param x X 方向の移動量
		/// @param y Y 方向の移動量
		/// @return *this
		Polygon& moveBy(double x, double y) noexcept;

		/// @brief 多角形を平行移動します。
		/// @param v 移動量
		/// @return *this
		Polygon& moveBy(Vec2 v) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	withOffset
		//
		////////////////////////////////////////////////////////////////

		/// @brief 平行移動した多角形を返します。
		/// @param x X 方向の移動量
		/// @param y Y 方向の移動量
		/// @return 平行移動した多角形
		Polygon withOffset(double x, double y) const&;

		/// @brief 平行移動した多角形を返します。
		/// @param x X 方向の移動量
		/// @param y Y 方向の移動量
		/// @return 平行移動した多角形
		Polygon withOffset(double x, double y) && noexcept;

		/// @brief 平行移動した多角形を返します。
		/// @param v 移動量
		/// @return 平行移動した多角形
		Polygon withOffset(Vec2 v) const&;

		/// @brief 平行移動した多角形を返します。
		/// @param v 移動量
		/// @return 平行移動した多角形
		Polygon withOffset(Vec2 v) && noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	withOffsetX, withOffsetY
		//
		////////////////////////////////////////////////////////////////

		/// @brief X 方向に平行移動した多角形を返します。
		/// @param x X 方向の移動量
		/// @return X 方向に平行移動した多角形
		Polygon withOffsetX(double x) const&;

		/// @brief X 方向に平行移動した多角形を返します。
		/// @param x X 方向の移動量
		/// @return X 方向に平行移動した多角形
		Polygon withOffsetX(double x) && noexcept;

		/// @brief Y 方向に平行移動した多角形を返します。
		/// @param y Y 方向の移動量
		/// @return Y 方向に平行移動した多角形
		Polygon withOffsetY(double y) const&;

		/// @brief Y 方向に平行移動した多角形を返します。
		/// @param y Y 方向の移動量
		/// @return Y 方向に平行移動した多角形
		Polygon withOffsetY(double y) && noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	rotated
		//
		////////////////////////////////////////////////////////////////

		/// @brief 原点 (0, 0) を中心に回転した多角形を返します。
		/// @param angle 回転角度（ラジアン）
		/// @return 回転した多角形
		[[nodiscard]]
		Polygon rotated(double angle) const&;

		/// @brief 原点 (0, 0) を中心に回転した多角形を返します。
		/// @param angle 回転角度（ラジアン）
		/// @return 回転した多角形
		[[nodiscard]]
		Polygon rotated(double angle) &&;

		////////////////////////////////////////////////////////////////
		//
		//	rotatedAt
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した座標を中心に回転した多角形を返します。
		/// @param pos 回転の中心座標
		/// @param angle 回転角度（ラジアン）
		/// @return 回転した多角形
		[[nodiscard]]
		Polygon rotatedAt(Vec2 pos, double angle) const&;

		/// @brief 指定した座標を中心に回転した多角形を返します。
		/// @param pos 回転の中心座標
		/// @param angle 回転角度（ラジアン）
		/// @return 回転した多角形
		[[nodiscard]]
		Polygon rotatedAt(Vec2 pos, double angle) &&;

		////////////////////////////////////////////////////////////////
		//
		//	rotate
		//
		////////////////////////////////////////////////////////////////

		/// @brief 原点 (0, 0) を中心に多角形を回転します。
		/// @param angle 回転角度（ラジアン）
		/// @return *this
		Polygon& rotate(double angle);

		////////////////////////////////////////////////////////////////
		//
		//	rotateAt
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した座標を中心に多角形を回転します。
		/// @param pos 回転の中心座標
		/// @param angle 回転角度（ラジアン）
		/// @return *this
		Polygon& rotateAt(Vec2 pos, double angle);

		////////////////////////////////////////////////////////////////
		//
		//	transformed
		//
		////////////////////////////////////////////////////////////////

		/// @brief 回転と平行移動を適用した多角形を返します。
		/// @param s 回転角度の sin
		/// @param c 回転角度の cos
		/// @param pos 平行移動量
		/// @return 変換後の多角形
		[[nodiscard]]
		Polygon transformed(double s, double c, const Vec2& pos) const&;

		/// @brief 回転と平行移動を適用した多角形を返します。
		/// @param s 回転角度の sin
		/// @param c 回転角度の cos
		/// @param pos 平行移動量
		/// @return 変換後の多角形
		[[nodiscard]]
		Polygon transformed(double s, double c, const Vec2& pos) &&;

		////////////////////////////////////////////////////////////////
		//
		//	transform
		//
		////////////////////////////////////////////////////////////////

		/// @brief 多角形に回転と平行移動を適用します。
		/// @param s 回転角度の sin
		/// @param c 回転角度の cos
		/// @param pos 平行移動量
		/// @return *this
		Polygon& transform(double s, double c, const Vec2& pos);

		////////////////////////////////////////////////////////////////
		//
		//	scaledFromOrigin
		//
		////////////////////////////////////////////////////////////////

		/// @brief 原点 (0, 0) を中心に拡大・縮小した多角形を返します。
		/// @param s 拡大率
		/// @return 拡大・縮小した多角形
		[[nodiscard]]
		Polygon scaledFromOrigin(double s) const&;

		/// @brief 原点 (0, 0) を中心に拡大・縮小した多角形を返します。
		/// @param s 拡大率
		/// @return 拡大・縮小した多角形
		[[nodiscard]]
		Polygon scaledFromOrigin(double s) &&;

		/// @brief 原点 (0, 0) を中心に拡大・縮小した多角形を返します。
		/// @param sx X 方向の拡大率
		/// @param sy Y 方向の拡大率
		/// @return 拡大・縮小した多角形
		[[nodiscard]]
		Polygon scaledFromOrigin(double sx, double sy) const&;

		/// @brief 原点 (0, 0) を中心に拡大・縮小した多角形を返します。
		/// @param sx X 方向の拡大率
		/// @param sy Y 方向の拡大率
		/// @return 拡大・縮小した多角形
		[[nodiscard]]
		Polygon scaledFromOrigin(double sx, double sy) &&;

		/// @brief 原点 (0, 0) を中心に拡大・縮小した多角形を返します。
		/// @param s 拡大率
		/// @return 拡大・縮小した多角形
		[[nodiscard]]
		Polygon scaledFromOrigin(Vec2 s) const&;

		/// @brief 原点 (0, 0) を中心に拡大・縮小した多角形を返します。
		/// @param s 拡大率
		/// @return 拡大・縮小した多角形
		[[nodiscard]]
		Polygon scaledFromOrigin(Vec2 s) &&;

		////////////////////////////////////////////////////////////////
		//
		//	scaleFromOrigin
		//
		////////////////////////////////////////////////////////////////

		/// @brief 原点 (0, 0) を中心に拡大・縮小します。
		/// @param s 拡大率
		/// @return *this
		Polygon& scaleFromOrigin(double s);

		/// @brief 原点 (0, 0) を中心に拡大・縮小します。
		/// @param sx X 方向の拡大率
		/// @param sy Y 方向の拡大率
		/// @return *this
		Polygon& scaleFromOrigin(double sx, double sy);

		/// @brief 原点 (0, 0) を中心に拡大・縮小します。
		/// @param s 拡大率
		/// @return *this
		Polygon& scaleFromOrigin(Vec2 s);

		////////////////////////////////////////////////////////////////
		//
		//	scaledFrom
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した位置を中心に拡大・縮小した多角形を返します。
		/// @param pos 拡大・縮小の中心位置
		/// @param s 拡大率
		/// @return 拡大・縮小した多角形
		[[nodiscard]]
		Polygon scaledFrom(Vec2 pos, double s) const&;

		/// @brief 指定した位置を中心に拡大・縮小した多角形を返します。
		/// @param pos 拡大・縮小の中心位置
		/// @param s 拡大率
		/// @return 拡大・縮小した多角形
		[[nodiscard]]
		Polygon scaledFrom(Vec2 pos, double s) &&;

		/// @brief 指定した位置を中心に拡大・縮小した多角形を返します。
		/// @param pos 拡大・縮小の中心位置
		/// @param sx X 方向の拡大率
		/// @param sy Y 方向の拡大率
		/// @return 拡大・縮小した多角形
		[[nodiscard]]
		Polygon scaledFrom(Vec2 pos, double sx, double sy) const&;

		/// @brief 指定した位置を中心に拡大・縮小した多角形を返します。
		/// @param pos 拡大・縮小の中心位置
		/// @param sx X 方向の拡大率
		/// @param sy Y 方向の拡大率
		/// @return 拡大・縮小した多角形
		[[nodiscard]]
		Polygon scaledFrom(Vec2 pos, double sx, double sy) &&;

		/// @brief 指定した位置を中心に拡大・縮小した多角形を返します。
		/// @param pos 拡大・縮小の中心位置
		/// @param s 拡大率
		/// @return 拡大・縮小した多角形
		[[nodiscard]]
		Polygon scaledFrom(Vec2 pos, Vec2 s) const&;

		/// @brief 指定した位置を中心に拡大・縮小した多角形を返します。
		/// @param pos 拡大・縮小の中心位置
		/// @param s 拡大率
		/// @return 拡大・縮小した多角形
		[[nodiscard]]
		Polygon scaledFrom(Vec2 pos, Vec2 s) &&;

		////////////////////////////////////////////////////////////////
		//
		//	scaleFrom
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した位置を中心に拡大・縮小します。
		/// @param pos 拡大・縮小の中心位置
		/// @param s 拡大率
		/// @return *this
		Polygon& scaleFrom(Vec2 pos, double s);

		/// @brief 指定した位置を中心に拡大・縮小します。
		/// @param pos 拡大・縮小の中心位置
		/// @param sx X 方向の拡大率
		/// @param sy Y 方向の拡大率
		/// @return *this
		Polygon& scaleFrom(Vec2 pos, double sx, double sy);

		/// @brief 指定した位置を中心に拡大・縮小します。
		/// @param pos 拡大・縮小の中心位置
		/// @param s 拡大率
		/// @return *this
		Polygon& scaleFrom(Vec2 pos, Vec2 s);

		////////////////////////////////////////////////////////////////
		//
		//	area
		//
		////////////////////////////////////////////////////////////////

		/// @brief 多角形の面積を返します。
		/// @return 多角形の面積
		[[nodiscard]]
		double area() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	perimeter
		//
		////////////////////////////////////////////////////////////////

		/// @brief 多角形の、穴を含めた輪郭の長さを返します。
		/// @return 多角形の、穴を含めた輪郭の長さ
		[[nodiscard]]
		double perimeter() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	centroid
		//
		////////////////////////////////////////////////////////////////

		/// @brief 多角形の重心の座標を返します。
		/// @return 多角形の重心の座標、多角形が空の場合は `Vec2{ 0, 0 }`
		[[nodiscard]]
		Vec2 centroid() const;

		////////////////////////////////////////////////////////////////
		//
		//	computeConvexHull
		//
		////////////////////////////////////////////////////////////////

		/// @brief 多角形の凸包を計算して返します。
		/// @return 多角形の凸包
		[[nodiscard]]
		Polygon computeConvexHull() const;

		////////////////////////////////////////////////////////////////
		//
		//	computeMiterBufferPolygon
		//
		////////////////////////////////////////////////////////////////

		/// @brief 多角形を太らせた、新しい多角形を返します。
		/// @param distance 太らせる距離。負の場合は細らせます。
		/// @return 新しい多角形
		[[nodiscard]]
		Polygon computeMiterBufferPolygon(double distance) const;

		////////////////////////////////////////////////////////////////
		//
		//	computeRoundBufferPolygon
		//
		////////////////////////////////////////////////////////////////

		/// @brief 多角形を丸く太らせた、新しい多角形を返します。分割数は半径に応じて自動的に決定されます。
		/// @param distance 太らせる距離。負の場合は細らせます。
		/// @param qualityFactor 品質係数。大きいほど分割数が増えます。
		/// @return 新しい多角形
		[[nodiscard]]
		Polygon computeRoundBufferPolygon(double distance, const QualityFactor& qualityFactor = QualityFactor{ 1.0 }) const;

		////////////////////////////////////////////////////////////////
		//
		//	simplified
		//
		////////////////////////////////////////////////////////////////

		/// @brief 多角形を単純化した、新しい多角形を返します。
		/// @param maxDistance 単純化に使う距離。大きいほど単純化されます。
		/// @return 新しい多角形
		[[nodiscard]]
		Polygon simplified(double maxDistance = 2.0) const;

		////////////////////////////////////////////////////////////////
		//
		//	outline
		//
		////////////////////////////////////////////////////////////////

		/// @brief 多角形の外周を LineString で返します。
		/// @param closeRing 始点と終点を一致させる場合は `CloseRing::Yes`, それ以外の場合は `CloseRing::No`
		/// @return 多角形の外周
		[[nodiscard]]
		LineString outline(CloseRing closeRing = CloseRing::No) const;

		/// @brief 多角形の外周の一部を LineString で返します。
		/// @param distanceFromOrigin 取得の開始位置（Polygon 外周の最初の頂点からの距離）
		/// @param length 取得する LineString の長さ
		/// @return 取得した多角形の外周の一部
		[[nodiscard]]
		LineString outline(double distanceFromOrigin, double length) const;

		////////////////////////////////////////////////////////////////
		//
		//	append
		//
		////////////////////////////////////////////////////////////////

		/// @brief 多角形に新しい領域を追加します。新しい領域は既存の多角形と接続していなければなりません。
		/// @param other 新しい領域
		/// @return 新しい領域の追加に成功した場合 true, それ以外の場合は false
		bool append(const RectF& other);

		/// @brief 多角形に新しい領域を追加します。新しい領域は既存の多角形と接続していなければなりません。
		/// @param other 新しい領域
		/// @return 新しい領域の追加に成功した場合 true, それ以外の場合は false
		bool append(const Polygon& other);

		////////////////////////////////////////////////////////////////
		//
		//	intersects
		//
		////////////////////////////////////////////////////////////////

		/// @brief 別の図形と交差しているかを返します。
		/// @tparam Shape2DType 別の図形の型
		/// @param other 別の図形
		/// @return 別の図形と交差している場合 true, それ以外の場合は false
		template <class Shape2DType>
		[[nodiscard]]
		bool intersects(const Shape2DType& other) const;

		///// @brief 点と交差しているかを返します。
		///// @param other 点
		///// @return 点と交差している場合 true, それ以外の場合は false
		//[[nodiscard]]
		//bool intersects(const Vec2& other) const;

		///// @brief 線分と交差しているかを返します。
		///// @param other 線分
		///// @return 線分と交差している場合 true, それ以外の場合は false
		//[[nodiscard]]
		//bool intersects(const Line& other) const;

		///// @brief 長方形と交差しているかを返します。
		///// @param other 長方形
		///// @return 長方形と交差している場合 true, それ以外の場合は false
		//[[nodiscard]]
		//bool intersects(const Rect& other) const;

		///// @brief 長方形と交差しているかを返します。
		///// @param other 長方形
		///// @return 長方形と交差している場合 true, それ以外の場合は false
		//[[nodiscard]]
		//bool intersects(const RectF& other) const;

		///// @brief 円と交差しているかを返します。
		///// @param other 円
		///// @return 円と交差している場合 true, それ以外の場合は false
		//[[nodiscard]]
		//bool intersects(const Circle& other) const;

		///// @brief 楕円と交差しているかを返します。
		///// @param other 楕円
		///// @return 楕円と交差している場合 true, それ以外の場合は false
		//[[nodiscard]]
		//bool intersects(const Ellipse& other) const;

		///// @brief 三角形と交差しているかを返します。
		///// @param other 三角形
		///// @return 三角形と交差している場合 true, それ以外の場合は false
		//[[nodiscard]]
		//bool intersects(const Triangle& other) const;

		///// @brief 四角形と交差しているかを返します。
		///// @param other 四角形
		///// @return 四角形と交差している場合 true, それ以外の場合は false
		//[[nodiscard]]
		//bool intersects(const Quad& other) const;

		///// @brief 多角形と交差しているかを返します。
		///// @param other 多角形
		///// @return 多角形と交差している場合 true, それ以外の場合は false
		//[[nodiscard]]
		//bool intersects(const Polygon& other) const;

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
		//	contains
		//
		////////////////////////////////////////////////////////////////

		/// @brief 別の図形を完全に含んでいるかを返します。
		/// @tparam Shape2DType 別の図形の型
		/// @param other 別の図形
		/// @return 別の図形を完全に含んでいる場合 true, それ以外の場合は false
		template <class Shape2DType>
		[[nodiscard]]
		bool contains(const Shape2DType& other) const;

		////////////////////////////////////////////////////////////////
		//
		//	leftClicked, leftPressed, leftReleased
		//
		////////////////////////////////////////////////////////////////

		/// @brief 多角形が現在のフレームで左クリックされ始めたかを返します。
		/// @return 多角形が現在のフレームで左クリックされ始めた場合 true, それ以外の場合は false
		[[nodiscard]]
		bool leftClicked() const noexcept;

		/// @brief 多角形が左クリックされているかを返します。
		/// @return 多角形が左クリックされている場合 true, それ以外の場合は false
		[[nodiscard]]
		bool leftPressed() const noexcept;

		/// @brief 現在のフレームで多角形への左クリックが離されたかを返します。
		/// @return 現在のフレームで多角形への左クリックが離された場合 true, それ以外の場合は false
		[[nodiscard]]
		bool leftReleased() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	rightClicked, rightPressed, rightReleased
		//
		////////////////////////////////////////////////////////////////

		/// @brief 多角形が現在のフレームで右クリックされ始めたかを返します。
		/// @return 多角形が現在のフレームで右クリックされ始めた場合 true, それ以外の場合は false
		[[nodiscard]]
		bool rightClicked() const noexcept;

		/// @brief 多角形が右クリックされているかを返します。
		/// @return 多角形が右クリックされている場合 true, それ以外の場合は false
		[[nodiscard]]
		bool rightPressed() const noexcept;

		/// @brief 現在のフレームで多角形への右クリックが離されたかを返します。
		/// @return 現在のフレームで多角形への右クリックが離された場合 true, それ以外の場合は false
		[[nodiscard]]
		bool rightReleased() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	mouseOver
		//
		////////////////////////////////////////////////////////////////

		/// @brief 多角形上にマウスカーソルがあるかを返します。
		/// @return 多角形上にマウスカーソルがある場合 true, それ以外の場合は false
		[[nodiscard]]
		bool mouseOver() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	paint
		//
		////////////////////////////////////////////////////////////////

		/// @brief 多角形を Image に描き込みます。
		/// @param dst 描き込み先の Image
		/// @param color 色
		/// @param enableAntialiasing アンチエイリアスを有効にするか
		/// @return *this
		const Polygon& paint(Image& dst, const Color& color, EnableAntialiasing enableAntialiasing = EnableAntialiasing::Yes) const;

		/// @brief 多角形を移動させた位置で Image に描き込みます。
		/// @param dst 描き込み先の Image
		/// @param offset 座標のオフセット
		/// @param color 色
		/// @param enableAntialiasing アンチエイリアスを有効にするか
		/// @return *this
		const Polygon& paint(Image& dst, const Vec2& offset, const Color& color, EnableAntialiasing enableAntialiasing = EnableAntialiasing::Yes) const;

		////////////////////////////////////////////////////////////////
		//
		//	overwrite
		//
		////////////////////////////////////////////////////////////////

		/// @brief 多角形を Image に上書きします。
		/// @param dst 上書き先の Image
		/// @param color 色
		/// @param enableAntialiasing アンチエイリアスを有効にするか
		/// @return *this
		const Polygon& overwrite(Image& dst, const Color& color, EnableAntialiasing enableAntialiasing = EnableAntialiasing::Yes) const;

		/// @brief 多角形を移動させた位置で Image に上書きします。
		/// @param dst 上書き先の Image
		/// @param offset 座標のオフセット
		/// @param color 色
		/// @param enableAntialiasing アンチエイリアスを有効にするか
		/// @return *this
		const Polygon& overwrite(Image& dst, const Vec2& offset, const Color& color, EnableAntialiasing enableAntialiasing = EnableAntialiasing::Yes) const;

		////////////////////////////////////////////////////////////////
		//
		//	paintFrame
		//
		////////////////////////////////////////////////////////////////

		/// @brief 多角形の枠を Image に描き込みます。
		/// @param dst 描き込み先の Image
		/// @param color 色
		/// @param enableAntialiasing アンチエイリアスを有効にするか
		/// @return *this
		const Polygon& paintFrame(Image& dst, const Color& color, EnableAntialiasing enableAntialiasing = EnableAntialiasing::Yes) const;

		/// @brief 多角形の枠を Image に描き込みます。
		/// @param dst 描き込み先の Image
		/// @param thickness 枠の太さ
		/// @param color 色
		/// @param enableAntialiasing アンチエイリアスを有効にするか
		/// @return *this
		const Polygon& paintFrame(Image& dst, double thickness, const Color& color, EnableAntialiasing enableAntialiasing = EnableAntialiasing::Yes) const;

		/// @brief 多角形の枠を移動させた位置で Image に描き込みます。
		/// @param dst 描き込み先の Image
		/// @param offset 座標のオフセット
		/// @param color 色
		/// @param enableAntialiasing アンチエイリアスを有効にするか
		void paintFrame(Image& dst, const Vec2& offset, const Color& color, EnableAntialiasing enableAntialiasing = EnableAntialiasing::Yes) const;

		/// @brief 多角形の枠を移動させた位置で Image に描き込みます。
		/// @param dst 描き込み先の Image
		/// @param offset 座標のオフセット
		/// @param thickness 枠の太さ
		/// @param color 色
		/// @param enableAntialiasing アンチエイリアスを有効にするか
		void paintFrame(Image& dst, const Vec2& offset, double thickness, const Color& color, EnableAntialiasing enableAntialiasing = EnableAntialiasing::Yes) const;

		////////////////////////////////////////////////////////////////
		//
		//	overwriteFrame
		//
		////////////////////////////////////////////////////////////////

		/// @brief 多角形の枠を Image に上書きします。
		/// @param dst 上書き先の Image
		/// @param color 色
		/// @param enableAntialiasing アンチエイリアスを有効にするか
		/// @return *this
		const Polygon& overwriteFrame(Image& dst, const Color& color, EnableAntialiasing enableAntialiasing = EnableAntialiasing::Yes) const;

		/// @brief 多角形の枠を Image に上書きします。
		/// @param dst 上書き先の Image
		/// @param thickness 枠の太さ
		/// @param color 色
		/// @param enableAntialiasing アンチエイリアスを有効にするか
		/// @return *this
		const Polygon& overwriteFrame(Image& dst, double thickness, const Color& color, EnableAntialiasing enableAntialiasing = EnableAntialiasing::Yes) const;

		/// @brief 多角形の枠を移動させた位置で Image に上書きします。
		/// @param dst 上書き先の Image
		/// @param offset 座標のオフセット
		/// @param color 色
		/// @param enableAntialiasing アンチエイリアスを有効にするか
		void overwriteFrame(Image& dst, const Vec2& offset, const Color& color, EnableAntialiasing enableAntialiasing = EnableAntialiasing::Yes) const;

		/// @brief 多角形の枠を移動させた位置で Image に上書きします。
		/// @param dst 上書き先の Image
		/// @param offset 座標のオフセット
		/// @param thickness 枠の太さ
		/// @param color 色
		/// @param enableAntialiasing アンチエイリアスを有効にするか
		void overwriteFrame(Image& dst, const Vec2& offset, double thickness, const Color& color, EnableAntialiasing enableAntialiasing = EnableAntialiasing::Yes) const;

		////////////////////////////////////////////////////////////////
		//
		//	draw
		//
		////////////////////////////////////////////////////////////////

		/// @brief 多角形を描画します。
		/// @param color 色
		/// @return *this
		const Polygon& draw(const ColorF& color = Palette::White) const;

		/// @brief 移動させた位置に多角形を描画します。
		/// @param offset 座標のオフセット
		/// @param color 色
		void draw(const Vec2& offset, const ColorF& color = Palette::White) const;

		/// @brief 多角形を描画します。
		/// @param pattern 塗りつぶしパターン
		/// @return *this
		const Polygon& draw(const PatternParameters& pattern) const;

		/// @brief 移動させた位置に多角形を描画します。
		/// @param offset 座標のオフセット
		/// @param pattern 塗りつぶしパターン
		void draw(const Vec2& offset, const PatternParameters& pattern) const;

		////////////////////////////////////////////////////////////////
		//
		//	drawTransformed
		//
		////////////////////////////////////////////////////////////////

		/// @brief 多角形を回転 + 移動して描画します。
		/// @param angle 回転角度（ラジアン）
		/// @param pos 位置
		/// @param color 色
		void drawTransformed(double angle, const Vec2& pos, const ColorF& color = Palette::White) const;

		/// @brief 多角形を回転 + 移動して描画します。
		/// @param s 回転角度のサイン
		/// @param c 回転角度のコサイン
		/// @param pos 位置
		/// @param color 色
		void drawTransformed(double s, double c, const Vec2& pos, const ColorF& color = Palette::White) const;

		/// @brief 多角形を回転 + 移動して描画します。
		/// @param angle 回転角度（ラジアン）
		/// @param pos 位置
		/// @param pattern 塗りつぶしパターン 
		void drawTransformed(double angle, const Vec2& pos, const PatternParameters& pattern) const;

		/// @brief 多角形を回転 + 移動して描画します。
		/// @param s 回転角度のサイン
		/// @param c 回転角度のコサイン
		/// @param pos 位置
		/// @param pattern 塗りつぶしパターン
		void drawTransformed(double s, double c, const Vec2& pos, const PatternParameters& pattern) const;

		////////////////////////////////////////////////////////////////
		//
		//	drawFrame
		//
		////////////////////////////////////////////////////////////////

		/// @brief 多角形の枠を描画します。
		/// @param thickness 枠の太さ（ピクセル）
		/// @param color 色
		/// @return *this
		const Polygon& drawFrame(double thickness = 1.0, const ColorF& color = Palette::White) const;

		/// @brief 移動させた位置に多角形の枠を描画します。
		/// @param offset 座標のオフセット
		/// @param thickness 枠の太さ（ピクセル）
		/// @param color 色
		void drawFrame(const Vec2& offset, double thickness = 1.0, const ColorF& color = Palette::White) const;

		/// @brief 多角形の枠を描画します。
		/// @param thickness 枠の太さ（ピクセル）
		/// @param pattern 塗りつぶしパターン
		/// @return *this
		const Polygon& drawFrame(double thickness, const PatternParameters& pattern) const;

		/// @brief 移動させた位置に多角形の枠を描画します。
		/// @param offset 座標のオフセット
		/// @param thickness 枠の太さ（ピクセル）
		/// @param pattern 塗りつぶしパターン
		void drawFrame(const Vec2& offset, double thickness, const PatternParameters& pattern) const;

		////////////////////////////////////////////////////////////////
		//
		//	drawWireframe
		//
		////////////////////////////////////////////////////////////////

		/// @brief 多角形をワイヤフレーム表示で描画します。
		/// @param thickness ワイヤフレームの太さ（ピクセル）
		/// @param color 色
		/// @return *this
		const Polygon& drawWireframe(double thickness = 1.0, const ColorF& color = Palette::White) const;

		/// @brief 移動させた位置に多角形をワイヤフレーム表示で描画します。
		/// @param offset 座標のオフセット
		/// @param thickness ワイヤフレームの太さ（ピクセル）
		/// @param color 色
		void drawWireframe(const Vec2& offset, double thickness = 1.0, const ColorF& color = Palette::White) const;

		/// @brief 多角形をワイヤフレーム表示で描画します。
		/// @param thickness ワイヤフレームの太さ（ピクセル）
		/// @param pattern 塗りつぶしパターン
		/// @return *this
		const Polygon& drawWireframe(double thickness, const PatternParameters& pattern) const;

		/// @brief 移動させた位置に多角形をワイヤフレーム表示で描画します。
		/// @param offset 座標のオフセット
		/// @param thickness ワイヤフレームの太さ（ピクセル）
		/// @param pattern 塗りつぶしパターン
		void drawWireframe(const Vec2& offset, double thickness, const PatternParameters& pattern) const;

		////////////////////////////////////////////////////////////////
		//
		//	toMesh2D
		//
		////////////////////////////////////////////////////////////////

		//[[nodiscard]]
		//Mesh2D toMesh2D(const Vec2& uvOrigin, const Vec2& uvScale) const;

		//[[nodiscard]]
		//Mesh2D toMesh2D(Arg::center_<Vec2> uvCenter, const Vec2& uvScale) const;

		//[[nodiscard]]
		//Mesh2D toMesh2D(Arg::center_<Vec2> uvCenter, const Vec2& uvScale, double uvRotation) const;

		//[[nodiscard]]
		//Mesh2D toMesh2D(const Mat3x2& uvMat) const;

		////////////////////////////////////////////////////////////////
		//
		//	_detail
		//
		////////////////////////////////////////////////////////////////

		/// @brief 内部実装へのポインタを返します。
		/// @return 内部実装へのポインタ
		[[nodiscard]]
		const PolygonDetail* _detail() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	Validate
		//
		////////////////////////////////////////////////////////////////

		/// @brief 頂点配列が多角形として有効かを検証します。
		/// @param outer 外周の頂点配列（時計回り）
		/// @param holes 多角形の穴の頂点配列（反時計回り）
		/// @return 多角形として有効であれば `PolygonFailureType::OK`, それ以外の場合はエラーの種類
		[[nodiscard]]
		static PolygonFailureType Validate(std::span<const Vec2> outer, const Array<Array<Vec2>>& holes = {});

		////////////////////////////////////////////////////////////////
		//
		//	Correct
		//
		////////////////////////////////////////////////////////////////

		/// @brief 頂点配列を修正して多角形を生成します。
		/// @param outer 外周の頂点配列
		/// @param holes 多角形の穴
		/// @return 頂点配列から生成した多角形の配列
		[[nodiscard]]
		static Array<Polygon> Correct(std::span<const Vec2> outer, const Array<Array<Vec2>>& holes = {});

		////////////////////////////////////////////////////////////////
		//
		//	CorrectOne
		//
		////////////////////////////////////////////////////////////////

		/// @brief 頂点配列を修正して多角形を生成し、最も面積の大きい多角形を返します。
		/// @param outer 外周の頂点配列
		/// @param holes 多角形の穴
		/// @return 頂点配列から生成した多角形のうち、最も面積の大きい多角形
		[[nodiscard]]
		static Polygon CorrectOne(std::span<const Vec2> outer, const Array<Array<Vec2>>& holes = {});

		////////////////////////////////////////////////////////////////
		//
		//	operator <<
		//
		////////////////////////////////////////////////////////////////

		/// @brief 多角形を出力します。
		/// @tparam CharType 出力ストリームの文字型
		/// @param output 出力ストリーム
		/// @param value 多角形
		/// @return 出力ストリーム
		template <class CharType>
		friend std::basic_ostream<CharType>& operator <<(std::basic_ostream<CharType>& output, const Polygon& value)
		{
			return output << Format(value);
		}

		////////////////////////////////////////////////////////////////
		//
		//	swap
		//
		////////////////////////////////////////////////////////////////

		/// @brief 2 つの多角形を入れ替えます。
		/// @param lhs 一方の多角形
		/// @param rhs もう一方の多角形
		friend void swap(Polygon& lhs, Polygon& rhs) noexcept
		{
			lhs.swap(rhs);
		}

		////////////////////////////////////////////////////////////////
		//
		//	Formatter
		//
		////////////////////////////////////////////////////////////////

		/// @brief 多角形を文字列に変換します。
		/// @param formatData 文字列バッファ
		/// @param value 多角形
		/// @remark この関数は Format 用の関数です。通常、ユーザーが直接呼び出す必要はありません。
		friend void Formatter(FormatData& formatData, const Polygon& value);

	private:

		[[noreturn]]
		static void ThrowTriangleAtIndexOutOfRange();

		std::unique_ptr<PolygonDetail> pImpl;
	};
}

////////////////////////////////////////////////////////////////
//
//	fmt
//
////////////////////////////////////////////////////////////////

template <>
struct fmt::formatter<s3d::Polygon, s3d::char32>
{
	std::u32string tag;

	s3d::ParseContext::iterator parse(s3d::ParseContext& ctx);

	s3d::BufferContext::iterator format(const s3d::Polygon& value, s3d::BufferContext& ctx) const;
};

# include "detail/Polygon.ipp"
