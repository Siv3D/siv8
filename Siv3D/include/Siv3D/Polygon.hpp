//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2024 Ryo Suzuki
//	Copyright (c) 2016-2024 OpenSiv3D Project
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
		const Array<Array<Vec2>>& inners() const noexcept;

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
		//	addHole
		//
		////////////////////////////////////////////////////////////////

		/// @brief 多角形に穴を追加します。
		/// @param rect 穴の形状
		/// @return 穴の追加に成功したら true, それ以外の場合は false
		bool addHole(const RectF& rect);

		/// @brief 多角形に穴を追加します。
		/// @param triangle 穴の形状
		/// @return  穴の追加に成功したら true, それ以外の場合は false
		bool addHole(const Triangle& triangle);

		/// @brief 多角形に穴を追加します。
		/// @param quad 穴の形状
		/// @return  穴の追加に成功したら true, それ以外の場合は false
		bool addHole(const Quad& quad);

		bool addHole(const Circle& circle, const PointsPerCircle& pointsPerCircle);

		bool addHole(const Circle& circle, const QualityFactor& qualityFactor);

		bool addHole(const Ellipse& ellipse, const PointsPerCircle& pointsPerCircle);

		bool addHole(const Ellipse& ellipse, const QualityFactor& qualityFactor);

		bool addHole(const RoundRect& roundRect, const PointsPerCircle& pointsPerCircle);

		bool addHole(const RoundRect& roundRect, const QualityFactor& qualityFactor);

		/// @brief 多角形に穴を追加します。
		/// @param hole 穴を構成する頂点配列
		/// @return 穴の追加に成功したら true, それ以外の場合は false
		bool addHole(Array<Vec2> hole);

		////////////////////////////////////////////////////////////////
		//
		//	addHoles
		//
		////////////////////////////////////////////////////////////////

		///// @brief 多角形に穴を追加します。
		///// @param holes 穴を構成する頂点配列の配列（複数の穴）
		///// @return 穴の追加に成功したら true, それ以外の場合は false
		//bool addHoles(Array<Array<Vec2>> holes);

		////////////////////////////////////////////////////////////////
		//
		//	movedBy
		//
		////////////////////////////////////////////////////////////////

		///// @brief 平行移動した多角形を返します。
		///// @param x X 方向の移動量
		///// @param y Y 方向の移動量
		///// @return 平行移動した多角形
		//[[nodiscard]]
		//Polygon movedBy(double x, double y) const&;

		///// @brief 平行移動した多角形を返します。
		///// @param v 移動量
		///// @return 平行移動した多角形
		//[[nodiscard]]
		//Polygon movedBy(Vec2 v) const&;

		///// @brief 平行移動した多角形を返します。
		///// @param x X 方向の移動量
		///// @param y Y 方向の移動量
		///// @return 平行移動した多角形
		//[[nodiscard]]
		//Polygon movedBy(double x, double y) && noexcept;

		///// @brief 平行移動した多角形を返します。
		///// @param v 移動量
		///// @return 平行移動した多角形
		//[[nodiscard]]
		//Polygon movedBy(Vec2 v) && noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	movedBy
		//
		////////////////////////////////////////////////////////////////

		///// @brief 多角形を平行移動します。
		///// @param x X 方向の移動量
		///// @param y Y 方向の移動量
		///// @return *this
		//Polygon& moveBy(double x, double y) noexcept;

		///// @brief 多角形を平行移動します。
		///// @param v 移動量
		///// @return *this
		//Polygon& moveBy(Vec2 v) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	rotated
		//
		////////////////////////////////////////////////////////////////

		//[[nodiscard]]
		//Polygon rotated(double angle) const&;

		//[[nodiscard]]
		//Polygon rotated(double angle) &&;

		////////////////////////////////////////////////////////////////
		//
		//	rotatedAt
		//
		////////////////////////////////////////////////////////////////

		//[[nodiscard]]
		//Polygon rotatedAt(double x, double y, double angle) const&;

		//[[nodiscard]]
		//Polygon rotatedAt(double x, double y, double angle) &&;

		//[[nodiscard]]
		//Polygon rotatedAt(Vec2 pos, double angle) const&;

		//[[nodiscard]]
		//Polygon rotatedAt(Vec2 pos, double angle) &&;

		////////////////////////////////////////////////////////////////
		//
		//	rotate
		//
		////////////////////////////////////////////////////////////////

		//Polygon& rotate(double angle);

		////////////////////////////////////////////////////////////////
		//
		//	rotateAt
		//
		////////////////////////////////////////////////////////////////

		//Polygon& rotateAt(double x, double y, double angle);

		//Polygon& rotateAt(Vec2 pos, double angle);

		////////////////////////////////////////////////////////////////
		//
		//	transformed
		//
		////////////////////////////////////////////////////////////////

		//[[nodiscard]]
		//Polygon transformed(double s, double c, const Vec2& pos) const&;

		//[[nodiscard]]
		//Polygon transformed(double s, double c, const Vec2& pos) &&;

		////////////////////////////////////////////////////////////////
		//
		//	transform
		//
		////////////////////////////////////////////////////////////////

		//Polygon& transform(double s, double c, const Vec2& pos);

		////////////////////////////////////////////////////////////////
		//
		//	scaled
		//
		////////////////////////////////////////////////////////////////

		///// @brief 原点 (0, 0) を中心に拡大・縮小した多角形を返します。
		///// @param s 拡大率
		///// @return 拡大・縮小した多角形
		//[[nodiscard]]
		//Polygon scaled(double s) const&;

		///// @brief 原点 (0, 0) を中心に拡大・縮小した多角形を返します。
		///// @param s 拡大率
		///// @return 拡大・縮小した多角形
		//[[nodiscard]]
		//Polygon scaled(double s) &&;

		///// @brief 原点 (0, 0) を中心に拡大・縮小した多角形を返します。
		///// @param sx X 方向の拡大率
		///// @param sy Y 方向の拡大率
		///// @return 拡大・縮小した多角形
		//[[nodiscard]]
		//Polygon scaled(double sx, double sy) const&;

		///// @brief 原点 (0, 0) を中心に拡大・縮小した多角形を返します。
		///// @param sx X 方向の拡大率
		///// @param sy Y 方向の拡大率
		///// @return 拡大・縮小した多角形
		//[[nodiscard]]
		//Polygon scaled(double sx, double sy) &&;

		///// @brief 原点 (0, 0) を中心に拡大・縮小した多角形を返します。
		///// @param s 拡大率
		///// @return 拡大・縮小した多角形
		//[[nodiscard]]
		//Polygon scaled(Vec2 s) const&;

		///// @brief 原点 (0, 0) を中心に拡大・縮小した多角形を返します。
		///// @param s 拡大率
		///// @return 拡大・縮小した多角形
		//[[nodiscard]]
		//Polygon scaled(Vec2 s) &&;

		////////////////////////////////////////////////////////////////
		//
		//	scale
		//
		////////////////////////////////////////////////////////////////

		///// @brief 原点 (0, 0) を中心に拡大・縮小します。
		///// @param s 拡大率
		///// @return *this
		//Polygon& scale(double s);

		///// @brief 原点 (0, 0) を中心に拡大・縮小します。
		///// @param sx X 方向の拡大率
		///// @param sy Y 方向の拡大率
		///// @return *this
		//Polygon& scale(double sx, double sy);

		///// @brief 原点 (0, 0) を中心に拡大・縮小します。
		///// @param s 拡大率
		///// @return *this
		//Polygon& scale(Vec2 s);

		////////////////////////////////////////////////////////////////
		//
		//	scaledAt
		//
		////////////////////////////////////////////////////////////////

		///// @brief 指定した位置を中心に拡大・縮小した多角形を返します。
		///// @param pos 拡大・縮小の中心位置
		///// @param s 拡大率
		///// @return 拡大・縮小した多角形
		//[[nodiscard]]
		//Polygon scaledAt(Vec2 pos, double s) const&;

		///// @brief 指定した位置を中心に拡大・縮小した多角形を返します。
		///// @param pos 拡大・縮小の中心位置
		///// @param s 拡大率
		///// @return 拡大・縮小した多角形
		//[[nodiscard]]
		//Polygon scaledAt(Vec2 pos, double s) &&;

		///// @brief 指定した位置を中心に拡大・縮小した多角形を返します。
		///// @param pos 拡大・縮小の中心位置
		///// @param sx X 方向の拡大率
		///// @param sy Y 方向の拡大率
		///// @return 拡大・縮小した多角形
		//[[nodiscard]]
		//Polygon scaledAt(Vec2 pos, double sx, double sy) const&;

		///// @brief 指定した位置を中心に拡大・縮小した多角形を返します。
		///// @param pos 拡大・縮小の中心位置
		///// @param sx X 方向の拡大率
		///// @param sy Y 方向の拡大率
		///// @return 拡大・縮小した多角形
		//[[nodiscard]]
		//Polygon scaledAt(Vec2 pos, double sx, double sy) &&;

		///// @brief 指定した位置を中心に拡大・縮小した多角形を返します。
		///// @param pos 拡大・縮小の中心位置
		///// @param s 拡大率
		///// @return 拡大・縮小した多角形
		//[[nodiscard]]
		//Polygon scaledAt(Vec2 pos, Vec2 s) const&;

		///// @brief 指定した位置を中心に拡大・縮小した多角形を返します。
		///// @param pos 拡大・縮小の中心位置
		///// @param s 拡大率
		///// @return 拡大・縮小した多角形
		//[[nodiscard]]
		//Polygon scaledAt(Vec2 pos, Vec2 s) &&;

		////////////////////////////////////////////////////////////////
		//
		//	scaleAt
		//
		////////////////////////////////////////////////////////////////

		///// @brief 指定した位置を中心に拡大・縮小します。
		///// @param pos 拡大・縮小の中心位置
		///// @param s 拡大率
		///// @return *this
		//Polygon& scaleAt(Vec2 pos, double s);

		///// @brief 指定した位置を中心に拡大・縮小します。
		///// @param pos 拡大・縮小の中心位置
		///// @param sx X 方向の拡大率
		///// @param sy Y 方向の拡大率
		///// @return *this
		//Polygon& scaleAt(Vec2 pos, double sx, double sy);

		///// @brief 指定した位置を中心に拡大・縮小します。
		///// @param pos 拡大・縮小の中心位置
		///// @param s 拡大率
		///// @return *this
		//Polygon& scaleAt(Vec2 pos, Vec2 s);

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
		//	calculateBuffer
		//
		////////////////////////////////////////////////////////////////

		/// @brief 多角形を太らせた、新しい多角形を返します。
		/// @param distance 太らせる距離。負の場合は細らせます。
		/// @return 新しい多角形
		[[nodiscard]]
		Polygon calculateBuffer(double distance) const;

		////////////////////////////////////////////////////////////////
		//
		//	calculateRoundBuffer
		//
		////////////////////////////////////////////////////////////////

		/// @brief 多角形を丸く太らせた、新しい多角形を返します。分割数は半径に応じて自動的に決定されます。
		/// @param distance 太らせる距離。負の場合は細らせます。
		/// @param qualityFactor 品質係数。大きいほど分割数が増えます。
		/// @return 新しい多角形
		[[nodiscard]]
		Polygon calculateRoundBuffer(double distance, const QualityFactor& qualityFactor = QualityFactor{ 1.0 }) const;

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

		///// @brief 多角形の外周を LineString で返します。
		///// @param closeRing 始点と終点を一致させる場合は `CloseRing::Yes`, それ以外の場合は `CloseRing::No`
		///// @return 多角形の外周
		//[[nodiscard]]
		//LineString outline(CloseRing closeRing = CloseRing::No) const;

		///// @brief 多角形の外周の一部を LineString で返します。
		///// @param distanceFromOrigin 取得の開始位置（Polygon 外周の最初の頂点からの距離）
		///// @param length 取得する LineString の長さ
		///// @return 取得した多角形の外周の一部
		//[[nodiscard]]
		//LineString outline(double distanceFromOrigin, double length) const;

		////////////////////////////////////////////////////////////////
		//
		//	append
		//
		////////////////////////////////////////////////////////////////

		///// @brief 多角形に新しい領域を追加します。新しい領域は既存の多角形と接続していなければなりません。
		///// @param other 新しい領域
		///// @return 新しい領域の追加に成功した場合 true, それ以外の場合は false
		//bool append(const RectF& other);

		///// @brief 多角形に新しい領域を追加します。新しい領域は既存の多角形と接続していなければなりません。
		///// @param other 新しい領域
		///// @return 新しい領域の追加に成功した場合 true, それ以外の場合は false
		//bool append(const Polygon& other);

		////////////////////////////////////////////////////////////////
		//
		//	intersects
		//
		////////////////////////////////////////////////////////////////

		template <class Shape2DType>
		[[nodiscard]]
		bool intersects(const Shape2DType& other) const;

		[[nodiscard]]
		bool intersects(const Line& other) const;

		[[nodiscard]]
		bool intersects(const Rect& other) const;

		[[nodiscard]]
		bool intersects(const RectF& other) const;

		[[nodiscard]]
		bool intersects(const Polygon& other) const;

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

		//template <class Shape2DType>
		//[[nodiscard]]
		//bool contains(const Shape2DType& other) const;

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

		//const Polygon& paint(Image& dst, const Color& color) const;

		//const Polygon& paint(Image& dst, double x, double y, const Color& color) const;

		//const Polygon& paint(Image& dst, const Vec2& pos, const Color& color) const;

		////////////////////////////////////////////////////////////////
		//
		//	overwrite
		//
		////////////////////////////////////////////////////////////////

		//const Polygon& overwrite(Image& dst, const Color& color, Antialiased antialiased = Antialiased::Yes) const;

		//const Polygon& overwrite(Image& dst, double x, double y, const Color& color, Antialiased antialiased = Antialiased::Yes) const;

		//const Polygon& overwrite(Image& dst, const Vec2& pos, const Color& color, Antialiased antialiased = Antialiased::Yes) const;

		////////////////////////////////////////////////////////////////
		//
		//	draw
		//
		////////////////////////////////////////////////////////////////

		/// @brief 多角形を描画します。
		/// @param color 色
		/// @return *this
		const Polygon& draw(const ColorF& color = Palette::White) const;

		/// @brief 多角形を移動して描画します。
		/// @param x X 方向の移動量
		/// @param y Y 方向の移動量
		/// @param color 色
		void draw(double x, double y, const ColorF& color = Palette::White) const;

		/// @brief 多角形を移動して描画します。
		/// @param pos 移動量
		/// @param color 色
		void draw(const Vec2& pos, const ColorF& color = Palette::White) const;

		/// @brief 多角形を描画します。
		/// @param pattern 塗りつぶしパターン
		/// @return *this
		const Polygon& draw(const PatternParameters& pattern) const;

		/// @brief 多角形を移動して描画します。
		/// @param x X 方向の移動量
		/// @param y Y 方向の移動量
		/// @param pattern 塗りつぶしパターン
		void draw(double x, double y, const PatternParameters& pattern) const;

		/// @brief 多角形を移動して描画します。
		/// @param pos 移動量
		/// @param pattern 塗りつぶしパターン
		void draw(const Vec2& pos, const PatternParameters& pattern) const;

		////////////////////////////////////////////////////////////////
		//
		//	drawTransformed
		//
		////////////////////////////////////////////////////////////////

		///// @brief 多角形を回転 + 移動して描画します。
		///// @param angle 回転角度（ラジアン）
		///// @param pos 位置
		///// @param color 色
		//void drawTransformed(double angle, const Vec2& pos, const ColorF& color = Palette::White) const;

		///// @brief 多角形を回転 + 移動して描画します。
		///// @param s 回転角度のサイン
		///// @param c 回転角度のコサイン
		///// @param pos 位置
		///// @param color 色
		//void drawTransformed(double s, double c, const Vec2& pos, const ColorF& color = Palette::White) const;

		////////////////////////////////////////////////////////////////
		//
		//	drawFrame
		//
		////////////////////////////////////////////////////////////////

		///// @brief 多角形の枠を描画します。
		///// @param thickness 枠の太さ（ピクセル）
		///// @param color 色
		///// @return *this
		//const Polygon& drawFrame(double thickness = 1.0, const ColorF& color = Palette::White) const;

		//void drawFrame(double x, double y, double thickness = 1.0, const ColorF& color = Palette::White) const;

		//void drawFrame(const Vec2& pos, double thickness = 1.0, const ColorF& color = Palette::White) const;

		////////////////////////////////////////////////////////////////
		//
		//	drawWireframe
		//
		////////////////////////////////////////////////////////////////

		///// @brief 多角形をワイヤフレーム表示で描画します。
		///// @param thickness ワイヤフレームの太さ（ピクセル）
		///// @param color 色
		///// @return *this
		//const Polygon& drawWireframe(double thickness = 1.0, const ColorF& color = Palette::White) const;

		///// @brief 多角形をワイヤフレーム表示で描画します。
		///// @param x X 座標のオフセット
		///// @param y Y 座標のオフセット
		///// @param thickness ワイヤフレームの太さ（ピクセル）
		///// @param color 色
		//void drawWireframe(double x, double y, double thickness = 1.0, const ColorF& color = Palette::White) const;

		///// @brief 多角形をワイヤフレーム表示で描画します。
		///// @param pos 座標のオフセット
		///// @param thickness ワイヤフレームの太さ（ピクセル）
		///// @param color 色
		//void drawWireframe(const Vec2& pos, double thickness = 1.0, const ColorF& color = Palette::White) const;

		////////////////////////////////////////////////////////////////
		//
		//	toBuffer2D
		//
		////////////////////////////////////////////////////////////////

		//[[nodiscard]]
		//Buffer2D toBuffer2D(const Vec2& uvOrigin, const Vec2& uvScale) const;

		//[[nodiscard]]
		//Buffer2D toBuffer2D(Arg::center_<Vec2> uvCenter, const Vec2& uvScale) const;

		//[[nodiscard]]
		//Buffer2D toBuffer2D(Arg::center_<Vec2> uvCenter, const Vec2& uvScale, double uvRotation) const;

		//[[nodiscard]]
		//Buffer2D toBuffer2D(const Mat3x2& uvMat) const;

		////////////////////////////////////////////////////////////////
		//
		//	Validate
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		static PolygonFailureType Validate(std::span<const Vec2> outer, const Array<Array<Vec2>>& holes = {});

		////////////////////////////////////////////////////////////////
		//
		//	Correct
		//
		////////////////////////////////////////////////////////////////

		/// @brief 頂点配列から多角形を生成します。
		/// @param vertices 頂点配列
		/// @param holes 多角形の穴
		/// @return 頂点配列から生成した多角形の配列
		[[nodiscard]]
		static Array<Polygon> Correct(std::span<const Vec2> outer, const Array<Array<Vec2>>& holes = {});

		////////////////////////////////////////////////////////////////
		//
		//	CorrectOne
		//
		////////////////////////////////////////////////////////////////

		/// @brief 頂点配列から多角形を生成し、最も面積の大きい多角形を返します。
		/// @param vertices 頂点配列
		/// @param holes 多角形の穴
		/// @return 頂点配列から生成した多角形のうち、最も面積の大きい多角形
		[[nodiscard]]
		static Polygon CorrectOne(std::span<const Vec2> outer, const Array<Array<Vec2>>& holes = {});

		////////////////////////////////////////////////////////////////
		//
		//	_detail
		//
		////////////////////////////////////////////////////////////////

		//[[nodiscard]]
		//const PolygonDetail* _detail() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator <<
		//
		////////////////////////////////////////////////////////////////

		//template <class CharType>
		//friend std::basic_ostream<CharType>& operator <<(std::basic_ostream<CharType>& output, const Polygon& value)
		//{
		//	output << CharType('(');

		//	output << CharType('(');

		//	bool b = false;

		//	for (const auto& point : value.outer())
		//	{
		//		if (std::exchange(b, true))
		//		{
		//			output << CharType(',');
		//		}

		//		output << point;
		//	}

		//	output << CharType(')');

		//	if (value.inners())
		//	{
		//		output << CharType(',');

		//		output << CharType('(');

		//		b = false;

		//		for (const auto& hole : value.inners())
		//		{
		//			if (std::exchange(b, true))
		//			{
		//				output << CharType(',');

		//				output << CharType('(');
		//			}

		//			bool b2 = false;

		//			for (const auto& point : hole)
		//			{
		//				if (std::exchange(b2, true))
		//				{
		//					output << CharType(',');
		//				}

		//				output << point;
		//			}

		//			output << CharType(')');
		//		}
		//	}

		//	return output << CharType(')');
		//}

		////////////////////////////////////////////////////////////////
		//
		//	swap
		//
		////////////////////////////////////////////////////////////////

		friend void swap(Polygon& lhs, Polygon& rhs) noexcept
		{
			lhs.swap(rhs);
		}

		////////////////////////////////////////////////////////////////
		//
		//	Formatter
		//
		////////////////////////////////////////////////////////////////

		friend void Formatter(FormatData& formatData, const Polygon& value);

	private:

		[[noreturn]]
		static void ThrowTriangleAtIndexOutOfRange();

		std::unique_ptr<PolygonDetail> pImpl;
	};
}

# include "detail/Polygon.ipp"
