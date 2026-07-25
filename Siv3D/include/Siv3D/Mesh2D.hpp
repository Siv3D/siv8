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
# include "Array.hpp"
# include "Vertex2D.hpp"
# include "TriangleIndex.hpp"
# include "PredefinedNamedParameter.hpp"

namespace s3d
{
	class Polygon;
	class Shape2D;
	class Texture;
	struct Mat3x2;

	/// @brief 2D 描画用のメッシュ
	/// @remark 頂点配列と三角形インデックス配列を保持し、低いオーバーヘッドで 2D 描画を行います。
	/// @remark 頂点やインデックスは `vertices`, `indices` を通して直接編集できます。
	struct Mesh2D
	{
		/// @brief 頂点配列
		Array<Vertex2D> vertices;

		/// @brief 三角形インデックス配列
		Array<TriangleIndex> indices;

		/// @brief 使用可能な最大の頂点インデックス
		static constexpr size_t MaxVertexIndex = 65534;

		/// @brief Mesh2D が保持できる最大の頂点数
		static constexpr size_t MaxVertexCount = 65535;

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		/// @brief 空の Mesh2D を作成します。
		[[nodiscard]]
		Mesh2D() = default;

		/// @brief 指定した数の頂点と三角形を持つ Mesh2D を作成します。
		/// @param vertexCount 頂点の個数
		/// @param triangleCount 三角形の個数
		/// @remark `vertexCount` が `MaxVertexCount` を超える場合は空の Mesh2D を作成します。
		[[nodiscard]]
		Mesh2D(size_t vertexCount, size_t triangleCount);

		/// @brief 頂点配列と三角形インデックス配列から Mesh2D を作成します。
		/// @param _vertices 頂点配列
		/// @remark `_vertices` の要素数が `MaxVertexCount` を超える場合は空の Mesh2D を作成します。
		[[nodiscard]]
		Mesh2D(Array<Vertex2D> _vertices, Array<TriangleIndex> _indices);

		/// @brief 頂点と三角形インデックスの範囲から Mesh2D を作成します。
		/// @param _vertices コピーする頂点の範囲
		/// @param _indices コピーする三角形インデックスの範囲
		/// @remark `_vertices` の要素数が `MaxVertexCount` を超える場合は空の Mesh2D を作成します。
		[[nodiscard]]
		Mesh2D(std::span<const Vertex2D> _vertices, std::span<const TriangleIndex> _indices);

		/// @brief Polygon から Mesh2D を作成します。
		/// @param polygon Mesh2D に変換する Polygon
		/// @remark 各頂点の色は白に、UV 座標は `(0, 0)` に設定されます。
		/// @remark `polygon` の頂点数が `MaxVertexCount` を超える場合は空の Mesh2D を作成します。
		[[nodiscard]]
		Mesh2D(const Polygon& polygon);

		/// @brief Polygon から、指定した矩形を UV 空間に対応させた Mesh2D を作成します。
		/// @param polygon Mesh2D に変換する Polygon
		/// @param mappingRect UV 座標 `(0, 0)` から `(1, 1)` に対応させる座標空間上の矩形
		/// @remark `mappingRect` の左上が UV 座標 `(0, 0)`、右下が UV 座標 `(1, 1)` に対応します。
		/// @remark `mappingRect` の外側にある頂点の UV 座標は `[0, 1]` の範囲外になることがあります。
		/// @remark `polygon` の頂点数が `MaxVertexCount` を超える場合、または `mappingRect` の幅または高さが 0 の場合は空の Mesh2D を作成します。
		[[nodiscard]]
		Mesh2D(const Polygon& polygon, const RectF& mappingRect);

		/// @brief Polygon から、指定した変換によって UV 座標を生成した Mesh2D を作成します。
		/// @param polygon Mesh2D に変換する Polygon
		/// @param uvTransform 頂点座標から UV 座標を計算する変換行列
		/// @remark 各頂点の UV 座標は、その頂点座標に `uvTransform` を適用して計算されます。
		/// @remark `polygon` の頂点数が `MaxVertexCount` を超える場合は空の Mesh2D を作成します。
		[[nodiscard]]
		Mesh2D(const Polygon& polygon, const Mat3x2& uvTransform);

		/// @brief Shape2D から Mesh2D を作成します。
		/// @param shape2D Mesh2D に変換する Shape2D
		/// @remark 各頂点の色は白に、UV 座標は `(0, 0)` に設定されます。
		/// @remark `shape2D` の頂点数が `MaxVertexCount` を超える場合は空の Mesh2D を作成します。
		[[nodiscard]]
		Mesh2D(const Shape2D& shape2D);

		/// @brief Shape2D から、指定した矩形を UV 空間に対応させた Mesh2D を作成します。
		/// @param shape2D Mesh2D に変換する Shape2D
		/// @param mappingRect UV 座標 `(0, 0)` から `(1, 1)` に対応させる座標空間上の矩形
		/// @remark `mappingRect` の左上が UV 座標 `(0, 0)`、右下が UV 座標 `(1, 1)` に対応します。
		/// @remark `mappingRect` の外側にある頂点の UV 座標は `[0, 1]` の範囲外になることがあります。
		/// @remark `shape2D` の頂点数が `MaxVertexCount` を超える場合、または `mappingRect` の幅または高さが 0 の場合は空の Mesh2D を作成します。
		[[nodiscard]]
		Mesh2D(const Shape2D& shape2D, const RectF& mappingRect);

		/// @brief Shape2D から、指定した変換によって UV 座標を生成した Mesh2D を作成します。
		/// @param shape2D Mesh2D に変換する Shape2D
		/// @param uvTransform 頂点座標から UV 座標を計算する変換行列
		/// @remark 各頂点の UV 座標は、その頂点座標に `uvTransform` を適用して計算されます。
		/// @remark `shape2D` の頂点数が `MaxVertexCount` を超える場合は空の Mesh2D を作成します。
		[[nodiscard]]
		Mesh2D(const Shape2D& shape2D, const Mat3x2& uvTransform);

		////////////////////////////////////////////////////////////////
		//
		//	isEmpty
		//
		////////////////////////////////////////////////////////////////

		/// @brief 描画可能な三角形を持たないかを返します。
		/// @return 頂点または三角形が空の場合 true, それ以外の場合は false
		[[nodiscard]]
		bool isEmpty() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	isValid
		//
		////////////////////////////////////////////////////////////////

		/// @brief Mesh2D の頂点数とインデックスの範囲を検証します。
		/// @return 頂点数が `MaxVertexCount` 以下で、すべてのインデックスが頂点配列の範囲内にある場合 true, それ以外の場合は false
		/// @remark 三角形の面積、頂点の並び順、頂点座標や UV 座標の値など、幾何学的な妥当性は検証しません。
		/// @remark 計算量は三角形の個数を n として O(n) です。
		[[nodiscard]]
		bool isValid() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	num_vertices
		//
		////////////////////////////////////////////////////////////////

		/// @brief 頂点の個数を返します。
		/// @return 頂点の個数
		[[nodiscard]]
		size_t num_vertices() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	num_triangles
		//
		////////////////////////////////////////////////////////////////

		/// @brief 三角形の個数を返します。
		/// @return 三角形の個数
		[[nodiscard]]
		size_t num_triangles() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	append
		//
		////////////////////////////////////////////////////////////////

		/// @brief 別の Mesh2D の頂点と三角形を末尾に追加します。
		/// @param mesh 追加する Mesh2D
		/// @return 追加に成功した場合 true, 追加できない場合は false
		/// @remark 追加される三角形のインデックスは、追加先の既存の頂点数に合わせて自動的に調整されます。
		/// @remark 追加後の頂点数が `MaxVertexCount` を超える場合、または調整後のインデックスが `MaxVertexIndex` を超える場合は追加に失敗します。
		/// @remark 追加に失敗した場合、この Mesh2D の内容は変更されません。
		[[nodiscard]]
		bool append(const Mesh2D& mesh);

		////////////////////////////////////////////////////////////////
		//
		//	setColor
		//
		////////////////////////////////////////////////////////////////

		/// @brief すべての頂点の色を設定します。
		/// @param color 設定する色
		/// @return *this
		Mesh2D& setColor(const ColorF& color) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	transformUV
		//
		////////////////////////////////////////////////////////////////

		/// @brief すべての頂点の UV 座標を変換します。
		/// @param transform UV 座標に適用する変換行列
		/// @return *this
		/// @remark 各頂点の現在の UV 座標に `transform` が適用されます。
		Mesh2D& transformUV(const Mat3x2& transform) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	draw
		//
		////////////////////////////////////////////////////////////////

		/// @brief Mesh2D を描画します。
		/// @remark 各頂点の色が描画色として使用されます。
		void draw() const;

		/// @brief テクスチャを使用して Mesh2D を描画します。
		/// @param texture 描画に使用するテクスチャ
		/// @remark 各頂点の UV 座標がテクスチャ座標として使用されます。
		/// @remark 各頂点の色がテクスチャの色に乗算されます。
		void draw(const Texture& texture) const;

		////////////////////////////////////////////////////////////////
		//
		//	drawSubset
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した範囲の三角形を描画します。
		/// @param startTriangle 描画を開始する三角形のインデックス
		/// @param triangleCount 描画する三角形の個数
		/// @remark `startTriangle + triangleCount` が三角形配列の末尾を超える場合、存在する範囲のみ描画されます。
		/// @remark `startTriangle` が三角形配列の範囲外の場合は何も描画されません。
		void drawSubset(size_t startTriangle, size_t triangleCount) const;

		/// @brief テクスチャを使用して、指定した範囲の三角形を描画します。
		/// @param startTriangle 描画を開始する三角形のインデックス
		/// @param triangleCount 描画する三角形の個数
		/// @param texture 描画に使用するテクスチャ
		/// @remark `startTriangle + triangleCount` が三角形配列の末尾を超える場合、存在する範囲のみ描画されます。
		/// @remark `startTriangle` が三角形配列の範囲外の場合は何も描画されません。
		/// @remark 各頂点の UV 座標がテクスチャ座標として使用されます。
		void drawSubset(size_t startTriangle, size_t triangleCount, const Texture& texture) const;

		////////////////////////////////////////////////////////////////
		//
		//	Grid
		//
		////////////////////////////////////////////////////////////////

		/// @brief 長方形を格子状に分割した Mesh2D を作成します。
		/// @param rect Mesh2D を作成する長方形
		/// @param divisions 横方向および縦方向の分割数
		/// @return 作成した Mesh2D
		/// @remark `divisions` が `{ x, y }` の場合、頂点数は `(x + 1) * (y + 1)`、三角形数は `2 * x * y` になります。
		/// @remark 長方形の左上から右下にかけて、UV 座標 `(0, 0)` から `(1, 1)` が割り当てられます。
		/// @remark 各頂点の色は白に設定されます。
		/// @remark 分割数が正でない場合、または必要な頂点数が `MaxVertexCount` を超える場合は空の Mesh2D を返します。
		[[nodiscard]]
		static Mesh2D Grid(const RectF& rect, const Size& divisions);

		/// @brief 四角形を格子状に分割した Mesh2D を作成します。
		/// @param quad Mesh2D を作成する四角形
		/// @param divisions 横方向および縦方向の分割数
		/// @return 作成した Mesh2D
		/// @remark `divisions` が `{ x, y }` の場合、頂点数は `(x + 1) * (y + 1)`、三角形数は `2 * x * y` になります。
		/// @remark 四角形の各辺を補間して格子状の頂点を生成します。
		/// @remark 四角形の左上側から右下側にかけて、UV 座標 `(0, 0)` から `(1, 1)` が割り当てられます。
		/// @remark 各頂点の色は白に設定されます。
		/// @remark 分割数が正でない場合、または必要な頂点数が `MaxVertexCount` を超える場合は空の Mesh2D を返します。
		[[nodiscard]]
		static Mesh2D Grid(const Quad& quad, const Size& divisions);
	};
}

# include "detail/Mesh2D.ipp"
