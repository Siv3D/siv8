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
# include "Blob.hpp"
# include "BoxUVMapping.hpp"
# include "IWriter.hpp"
# include "PredefinedYesNo.hpp"
# include "Vertex3D.hpp"
# include "TriangleIndex32.hpp"
# include "VertexNormalWeighting.hpp"

namespace s3d
{
	struct Mat4x4;
	struct Quaternion;

	////////////////////////////////////////////////////////////////
	//
	//	Mesh3D
	//
	////////////////////////////////////////////////////////////////

	/// @brief 3D メッシュデータ
	struct Mesh3D
	{
		/// @brief 頂点配列
		Array<Vertex3D> vertices;

		/// @brief 三角形インデックス配列
		Array<TriangleIndex32> indices;

		/// @brief 使用可能な最大の頂点インデックス
		/// @remark 32-bit index の最大値 0xFFFFFFFF は、一部のグラフィックス API で
		/// primitive restart / strip cut の特殊値として予約されているため使用しません。
		static constexpr size_t MaxVertexIndex = 4294967294;

		/// @brief Mesh3D が保持できる最大の頂点数
		static constexpr size_t MaxVertexCount = (MaxVertexIndex + 1);

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		/// @brief 空の 3D メッシュを作成します。
		[[nodiscard]]
		Mesh3D() = default;

		/// @brief 指定した数の頂点と三角形を持つ 3D メッシュを作成します。
		/// @param vertexCount 頂点数
		/// @param triangleCount 三角形数
		[[nodiscard]]
		Mesh3D(size_t vertexCount, size_t triangleCount);

		/// @brief 頂点配列と三角形インデックス配列から 3D メッシュを作成します。
		/// @param _vertices 頂点配列
		/// @param _indices 三角形インデックス配列
		[[nodiscard]]
		Mesh3D(Array<Vertex3D> _vertices, Array<TriangleIndex32> _indices);
		
		/// @brief 頂点と三角形インデックスの範囲から 3D メッシュを作成します。
		/// @param _vertices コピーする頂点の範囲
		/// @param _indices コピーする三角形インデックスの範囲
		[[nodiscard]]
		Mesh3D(std::span<const Vertex3D> _vertices, std::span<const TriangleIndex32> _indices);

		////////////////////////////////////////////////////////////////
		//
		//	Box
		//
		////////////////////////////////////////////////////////////////

		/// @brief 原点を中心とする直方体の 3D メッシュを作成します。
		/// @param size 直方体の各軸方向の大きさ
		/// @return 直方体の 3D メッシュ。`size` のいずれかの成分が正の有限値でない場合は空の 3D メッシュ
		/// @remark 各面は独立した頂点を持ち、面ごとに `[0, 1]` の UV 座標が割り当てられます。
		[[nodiscard]]
		static Mesh3D Box(Float3 size = Float3{ 1.0f, 1.0f, 1.0f });

		/// @brief 原点を中心とする直方体の 3D メッシュを作成します。
		/// @param size 直方体の各軸方向の大きさ
		/// @param uvMapping 各面に割り当てる UV 矩形
		/// @return 直方体の 3D メッシュ。`size` または `uvMapping` が不正な場合は空の 3D メッシュ
		/// @remark 各面は独立した頂点を持ち、`uvMapping` の対応する矩形が割り当てられます。
		[[nodiscard]]
		static Mesh3D Box(Float3 size, const BoxUVMapping& uvMapping);

		////////////////////////////////////////////////////////////////
		//
		//	Plane
		//
		////////////////////////////////////////////////////////////////

		/// @brief XZ 平面上に、法線が Y 軸の正方向を向く長方形の 3D メッシュを作成します。
		/// @param sizeXZ X 軸方向および Z 軸方向の大きさ
		/// @param uvScale UV 座標の拡大率
		/// @param uvOffset UV 座標のオフセット
		/// @return 長方形の 3D メッシュ。大きさまたは UV パラメータが不正な場合は空の 3D メッシュ
		/// @remark UV 座標の V 成分は Z 軸の正方向から負方向へ向かって増加します。
		[[nodiscard]]
		static Mesh3D Plane(
			Float2 sizeXZ = Float2{ 1.0f, 1.0f },
			Float2 uvScale = Float2{ 1.0f, 1.0f },
			Float2 uvOffset = Float2{ 0.0f, 0.0f });

		////////////////////////////////////////////////////////////////
		//
		//	Grid
		//
		////////////////////////////////////////////////////////////////

		/// @brief XZ 平面上に、法線が Y 軸の正方向を向く格子状の 3D メッシュを作成します。
		/// @param sizeXZ X 軸方向および Z 軸方向の大きさ
		/// @param segmentsX X 軸方向の分割数
		/// @param segmentsZ Z 軸方向の分割数
		/// @param uvScale UV 座標の拡大率
		/// @param uvOffset UV 座標のオフセット
		/// @return 格子状の 3D メッシュ。大きさ、分割数、UV パラメータが不正な場合、または頂点数が上限を超える場合は空の 3D メッシュ
		/// @remark UV 座標の V 成分は Z 軸の正方向から負方向へ向かって増加します。
		[[nodiscard]]
		static Mesh3D Grid(
			Float2 sizeXZ,
			uint32 segmentsX,
			uint32 segmentsZ,
			Float2 uvScale = Float2{ 1.0f, 1.0f },
			Float2 uvOffset = Float2{ 0.0f, 0.0f });

		////////////////////////////////////////////////////////////////
		//
		//	Torus
		//
		////////////////////////////////////////////////////////////////

		/// @brief Y 軸を回転軸とするトーラスの 3D メッシュを作成します。
		/// @param majorRadius 原点からチューブ断面の中心までの半径
		/// @param tubeRadius チューブ断面の半径。`majorRadius` より小さい必要があります。
		/// @param ringSegments リング方向の分割数。3 以上である必要があります。
		/// @param tubeSegments チューブ断面方向の分割数。3 以上である必要があります。
		/// @return トーラスの 3D メッシュ。引数が不正な場合、または頂点数が上限を超える場合は空の 3D メッシュ
		/// @remark UV 座標および接線空間の不連続を表現するため、リング方向とチューブ断面方向の継ぎ目に頂点が複製されます。
		[[nodiscard]]
		static Mesh3D Torus(
			float majorRadius,
			float tubeRadius,
			uint32 ringSegments = 32,
			uint32 tubeSegments = 16);

		////////////////////////////////////////////////////////////////
		//
		//	Capsule
		//
		////////////////////////////////////////////////////////////////

		/// @brief Y 軸方向に伸びるカプセルの 3D メッシュを作成します。
		/// @param radius カプセルの半径
		/// @param cylinderHeight 2 つの半球の間にある円柱部分の高さ。0 以上である必要があります。
		/// @param slices 円周方向の分割数。3 以上である必要があります。
		/// @param hemisphereStacks 各半球の緯度方向の分割数。1 以上である必要があります。
		/// @return カプセルの 3D メッシュ。引数が不正な場合、または頂点数が上限を超える場合は空の 3D メッシュ
		/// @remark カプセルは原点を中心とし、全体の高さは `cylinderHeight + 2 * radius` です。
		/// @remark `cylinderHeight` が 0 の場合は UV 球を作成します。
		/// @remark UV 座標および接線空間の不連続を表現するため、経度方向の継ぎ目と極に頂点が複製されます。
		[[nodiscard]]
		static Mesh3D Capsule(
			float radius,
			float cylinderHeight,
			uint32 slices = 32,
			uint32 hemisphereStacks = 8);

		////////////////////////////////////////////////////////////////
		//
		//	UVSphere
		//
		////////////////////////////////////////////////////////////////

		/// @brief 原点を中心とする UV 球の 3D メッシュを作成します。
		/// @param radius 球の半径
		/// @param slices 経度方向の分割数。3 以上である必要があります。
		/// @param stacks 緯度方向の分割数。2 以上である必要があります。
		/// @return UV 球の 3D メッシュ。引数が不正な場合、または頂点数が上限を超える場合は空の 3D メッシュ
		/// @remark 球の極は Y 軸上に配置されます。
		/// @remark UV 座標および接線空間の不連続を表現するため、同じ位置に複数の頂点が作成されます。
		[[nodiscard]]
		static Mesh3D UVSphere(float radius, uint32 slices = 32, uint32 stacks = 16);

		////////////////////////////////////////////////////////////////
		//
		//	Hemisphere
		//
		////////////////////////////////////////////////////////////////

		/// @brief Y 軸の正方向を向く、底面のない半球の 3D メッシュを作成します。
		/// @param radius 半球の半径
		/// @param slices 経度方向の分割数。3 以上である必要があります。
		/// @param stacks 緯度方向の分割数。1 以上である必要があります。
		/// @return 半球の 3D メッシュ。引数が不正な場合、または頂点数が上限を超える場合は空の 3D メッシュ
		/// @remark 半球の中心は原点、赤道は XZ 平面上、極は `(0, radius, 0)` に配置されます。
		/// @remark 曲面の UV 座標は、極から赤道までを V 座標の 0 から 1 に割り当てます。
		[[nodiscard]]
		static Mesh3D Hemisphere(float radius, uint32 slices = 32, uint32 stacks = 8);

		/// @brief Y 軸の正方向を向く半球の 3D メッシュを作成します。
		/// @param radius 半球の半径
		/// @param closeBottom 底面を閉じる場合は `CloseBottom::Yes`、底面を作成しない場合は `CloseBottom::No`
		/// @param slices 経度方向の分割数。3 以上である必要があります。
		/// @param stacks 緯度方向の分割数。1 以上である必要があります。
		/// @return 半球の 3D メッシュ。引数が不正な場合、または頂点数が上限を超える場合は空の 3D メッシュ
		/// @remark 半球の中心は原点、赤道は XZ 平面上、極は `(0, radius, 0)` に配置されます。
		/// @remark 曲面の UV 座標は、極から赤道までを V 座標の 0 から 1 に割り当てます。
		/// @remark 底面を閉じる場合、底面には曲面とは独立した頂点を作成し、円全体を UV 座標の 0 から 1 に割り当てます。
		[[nodiscard]]
		static Mesh3D Hemisphere(float radius, CloseBottom closeBottom, uint32 slices = 32, uint32 stacks = 8);

		////////////////////////////////////////////////////////////////
		//
		//	Disc
		//
		////////////////////////////////////////////////////////////////

		/// @brief XZ 平面上に、法線が Y 軸の正方向を向く円盤の 3D メッシュを作成します。
		/// @param radius 円盤の半径
		/// @param segments 円周の分割数。3 以上である必要があります。
		/// @return 円盤の 3D メッシュ。引数が不正な場合、または頂点数が上限を超える場合は空の 3D メッシュ
		[[nodiscard]]
		static Mesh3D Disc(float radius, uint32 segments = 32);

		////////////////////////////////////////////////////////////////
		//
		//	Annulus
		//
		////////////////////////////////////////////////////////////////

		/// @brief XZ 平面上に、法線が Y 軸の正方向を向く円環の 3D メッシュを作成します。
		/// @param innerRadius 円環の内半径。0 以上である必要があります。
		/// @param outerRadius 円環の外半径。`innerRadius` より大きい必要があります。
		/// @param segments 円周の分割数。3 以上である必要があります。
		/// @return 円環の 3D メッシュ。引数が不正な場合、または頂点数が上限を超える場合は空の 3D メッシュ
		/// @remark `innerRadius` が 0 の場合は、中心頂点を共有する円盤を作成します。
		[[nodiscard]]
		static Mesh3D Annulus(float innerRadius, float outerRadius, uint32 segments = 32);

		////////////////////////////////////////////////////////////////
		//
		//	Frustum
		//
		////////////////////////////////////////////////////////////////

		/// @brief Y 軸方向に伸びる、両端が閉じた円錐台の 3D メッシュを作成します。
		/// @param bottomRadius 底面の半径。正の有限値である必要があります。
		/// @param topRadius 上面の半径。0 以上の有限値である必要があります。0 の場合は円錐を作成します。
		/// @param height 高さ
		/// @param segments 円周の分割数。3 以上である必要があります。
		/// @return 円錐台の 3D メッシュ。引数が不正な場合、または頂点数が上限を超える場合は空の 3D メッシュ
		/// @remark 底面の中心は `(0, -height / 2, 0)`、上面の中心は `(0, height / 2, 0)` に配置されます。
		[[nodiscard]]
		static Mesh3D Frustum(float bottomRadius, float topRadius, float height, uint32 segments = 32);

		////////////////////////////////////////////////////////////////
		//
		//	Cylinder
		//
		////////////////////////////////////////////////////////////////

		/// @brief Y 軸方向に伸びる、両端が閉じた円柱の 3D メッシュを作成します。
		/// @param radius 円柱の半径
		/// @param height 円柱の高さ
		/// @param segments 円周の分割数。3 以上である必要があります。
		/// @return 円柱の 3D メッシュ。引数が不正な場合、または頂点数が上限を超える場合は空の 3D メッシュ
		[[nodiscard]]
		static Mesh3D Cylinder(float radius, float height, uint32 segments = 32);

		////////////////////////////////////////////////////////////////
		//
		//	Cone
		//
		////////////////////////////////////////////////////////////////

		/// @brief Y 軸方向に伸びる、底面が閉じた円錐の 3D メッシュを作成します。
		/// @param radius 円錐の底面の半径
		/// @param height 円錐の高さ
		/// @param segments 円周の分割数。3 以上である必要があります。
		/// @return 円錐の 3D メッシュ。引数が不正な場合、または頂点数が上限を超える場合は空の 3D メッシュ
		[[nodiscard]]
		static Mesh3D Cone(float radius, float height, uint32 segments = 32);

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
		//	validate
		//
		////////////////////////////////////////////////////////////////

		/// @brief 3D メッシュの頂点数とインデックスの範囲を検証します。
		/// @return 頂点数が `MaxVertexCount` 以下で、すべてのインデックスが頂点配列の範囲内にある場合 true, それ以外の場合は false
		/// @remark 三角形の面積、頂点の並び順、頂点座標や UV 座標の値など、幾何学的な妥当性は検証しません。
		/// @remark 計算量は三角形の個数を n として O(n) です。
		[[nodiscard]]
		bool validate() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	vertexCount
		//
		////////////////////////////////////////////////////////////////

		/// @brief 頂点の個数を返します。
		/// @return 頂点の個数
		[[nodiscard]]
		size_t vertexCount() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	triangleCount
		//
		////////////////////////////////////////////////////////////////

		/// @brief 三角形の個数を返します。
		/// @return 三角形の個数
		[[nodiscard]]
		size_t triangleCount() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	saveOBJ
		//
		////////////////////////////////////////////////////////////////

		/// @brief 3D メッシュを Wavefront OBJ 形式でファイルに保存します。
		/// @param path 保存するファイルのパス
		/// @return 保存に成功した場合 true, それ以外の場合は false
		/// @remark 頂点座標、UV 座標、法線、および三角形の巻き順は変換せずに保存されます。
		/// @remark 各 `Vertex3D` には、位置、UV、法線で共通の 1 始まりの OBJ インデックスが割り当てられます。
		/// @remark 接線、材質、オブジェクト名、およびグループは保存されません。
		/// @remark UTF-8（BOM なし）、LF 改行で保存されます。
		/// @remark 空のメッシュ、インデックスが不正なメッシュ、または出力対象の頂点属性に非有限値が含まれるメッシュは保存できません。
		bool saveOBJ(FilePathView path) const;

		////////////////////////////////////////////////////////////////
		//
		//	encodeOBJ
		//
		////////////////////////////////////////////////////////////////

		/// @brief 3D メッシュを Wavefront OBJ 形式で Writer に書き出します。
		/// @param writer 書き出し先の Writer
		/// @return 書き出しに成功した場合 true, それ以外の場合は false
		/// @remark 頂点座標、UV 座標、法線、および三角形の巻き順は変換せずに書き出されます。
		/// @remark 各 `Vertex3D` には、位置、UV、法線で共通の 1 始まりの OBJ インデックスが割り当てられます。
		/// @remark 接線、材質、オブジェクト名、およびグループは書き出されません。
		/// @remark UTF-8（BOM なし）、LF 改行で書き出されます。
		/// @remark 空のメッシュ、インデックスが不正なメッシュ、または出力対象の頂点属性に非有限値が含まれるメッシュは書き出せません。
		/// @remark 書き出し中に失敗した場合、Writer に途中までのデータが残ることがあります。
		bool encodeOBJ(IWriter& writer) const;

		/// @brief 3D メッシュを Wavefront OBJ 形式でエンコードします。
		/// @return エンコードされたデータ。エンコードに失敗した場合は空の Blob
		/// @remark 頂点座標、UV 座標、法線、および三角形の巻き順は変換せずにエンコードされます。
		/// @remark 各 `Vertex3D` には、位置、UV、法線で共通の 1 始まりの OBJ インデックスが割り当てられます。
		/// @remark 接線、材質、オブジェクト名、およびグループはエンコードされません。
		/// @remark UTF-8（BOM なし）、LF 改行でエンコードされます。
		/// @remark 空のメッシュ、インデックスが不正なメッシュ、または出力対象の頂点属性に非有限値が含まれるメッシュはエンコードできません。
		[[nodiscard]]
		Blob encodeOBJ() const;

		////////////////////////////////////////////////////////////////
		//
		//	append
		//
		////////////////////////////////////////////////////////////////

		/// @brief 別の 3D メッシュの頂点と三角形を末尾に追加します。
		/// @param mesh 追加する 3D メッシュ
		/// @return 追加に成功した場合 true, 追加できない場合は false
		/// @remark 追加される三角形のインデックスは、追加先の既存の頂点数に合わせて自動的に調整されます。
		/// @remark `mesh` が不正な 3D メッシュである場合、または追加後の頂点数が `MaxVertexCount` を超える場合は追加に失敗します。
		/// @remark この関数が false を返した場合、この 3D メッシュの内容は変更されません。
		[[nodiscard]]
		bool append(const Mesh3D& mesh);

		/// @brief アフィン変換を適用した別の 3D メッシュの頂点と三角形を末尾に追加します。
		/// @param mesh 追加する Mesh3D
		/// @param matrix 追加する頂点に適用するアフィン変換行列
		/// @return 追加に成功した場合 true, 追加できない場合は false
		/// @remark `matrix` はアフィン変換行列である必要があります。
		/// @remark `append(mesh.transformed(matrix))` と同じ結果を、一時 Mesh3D を作成せずに生成します。
		/// @remark `mesh` が不正な Mesh3D である場合、または追加後の頂点数が `MaxVertexCount` を超える場合は追加に失敗します。
		/// @remark この関数が false を返した場合、この Mesh3D の内容は変更されません。
		/// @remark 変換の線形部分が特異である場合、追加する頂点の法線および接線は変更されません。
		/// @remark 変換の線形部分の行列式が負の場合、追加する頂点の接線の `w` 成分が反転しますが、三角形の巻き順は変更されません。
		[[nodiscard]]
		bool append(const Mesh3D& mesh, const Mat4x4& matrix);

		////////////////////////////////////////////////////////////////
		//
		//	computeNormals
		//
		////////////////////////////////////////////////////////////////

		/// @brief 頂点法線を計算します。
		/// @param weighting 各三角形の法線を合成するときの重み付け方式
		/// @remark 同じ頂点インデックスを共有する三角形の法線が合成されます。
		/// @remark ハードエッジを保持するには、その境界で頂点が分割されている必要があります。
		/// @return *this
		/// @throw Error 法線の計算に失敗した場合
		Mesh3D& computeNormals(VertexNormalWeighting weighting = VertexNormalWeighting::Angle);

		////////////////////////////////////////////////////////////////
		//
		//	computeTangents
		//
		////////////////////////////////////////////////////////////////

		/// @brief MikkTSpace を使用して頂点の接線を計算します。
		/// @remark 頂点法線および UV 座標が設定済みである必要があります。
		/// @remark 接線空間が不連続になる箇所では頂点が複製されるため、頂点数およびインデックス配列が変更されることがあります。
		/// @return *this
		/// @throw Error 接線の計算に失敗した場合
		Mesh3D& computeTangents();

		////////////////////////////////////////////////////////////////
		//
		//	reverseWinding
		//
		////////////////////////////////////////////////////////////////

		/// @brief すべての三角形の頂点の巻き順を反転させます。
		/// @return *this
		/// @remark 各三角形のインデックスが `{ i0, i1, i2 }` から `{ i0, i2, i1 }` に変換されます。
		/// @remark 頂点の法線と接線は変更されません。表裏をまとめて反転させるには `invert()` を使用します。
		Mesh3D& reverseWinding() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	invertNormals
		//
		////////////////////////////////////////////////////////////////

		/// @brief すべての頂点の法線を反転させます。
		/// @return *this
		/// @remark 各頂点の法線ベクトルの符号が反転し、あわせて接線の `w` 成分（従接線の向きを表す符号）も反転します。
		/// @remark これにより従接線 `Vertex3D::bitangent()` は反転前と同じ向きに保たれ、法線マッピングとの整合性が維持されます。
		/// @remark 三角形の巻き順は変更されません。表裏をまとめて反転させるには `invert()` を使用します。
		Mesh3D& invertNormals() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	invert
		//
		////////////////////////////////////////////////////////////////

		/// @brief 3D メッシュの表裏を反転させます。
		/// @return *this
		/// @remark `reverseWinding()` と `invertNormals()` を同時に適用するのと同じ結果になります。
		/// @remark 三角形の巻き順、頂点の法線、および接線の `w` 成分が反転します。
		/// @remark 頂点座標、UV 座標、接線ベクトルの `xyz` 成分は変更されません。
		Mesh3D& invert() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	transformed
		//
		////////////////////////////////////////////////////////////////

		/// @brief アフィン変換を適用した 3D メッシュを返します。
		/// @param matrix 適用するアフィン変換行列
		/// @return アフィン変換を適用した 3D メッシュ
		/// @remark `matrix` はアフィン変換行列である必要があります。
		/// @remark 変換の線形部分が非特異である場合、法線および接線ベクトルは変換後に正規化されます。
		/// @remark 変換の線形部分が特異である場合、法線および接線は変更されません。
		/// @remark UV 座標および三角形インデックスは変更されません。
		/// @remark 変換の線形部分の行列式が負の場合、接線の `w` 成分が反転しますが、三角形の巻き順は変更されません。必要に応じて `reverseWinding()` を使用してください。
		[[nodiscard]]
		Mesh3D transformed(const Mat4x4& matrix) const&;

		/// @brief アフィン変換を適用した 3D メッシュを返します。
		/// @param matrix 適用するアフィン変換行列
		/// @return アフィン変換を適用した 3D メッシュ
		/// @remark このオーバーロードは自身のストレージを再利用します。
		/// @remark `matrix` はアフィン変換行列である必要があります。
		/// @remark 変換の線形部分が非特異である場合、法線および接線ベクトルは変換後に正規化されます。
		/// @remark 変換の線形部分が特異である場合、法線および接線は変更されません。
		/// @remark UV 座標および三角形インデックスは変更されません。
		/// @remark 変換の線形部分の行列式が負の場合、接線の `w` 成分が反転しますが、三角形の巻き順は変更されません。必要に応じて `reverseWinding()` を使用してください。
		[[nodiscard]]
		Mesh3D transformed(const Mat4x4& matrix) && noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	transform
		//
		////////////////////////////////////////////////////////////////

		/// @brief 3D メッシュにアフィン変換を適用します。
		/// @param matrix 適用するアフィン変換行列
		/// @return *this
		/// @remark `matrix` はアフィン変換行列である必要があります。
		/// @remark 変換の線形部分が非特異である場合、法線および接線ベクトルは変換後に正規化されます。
		/// @remark 変換の線形部分が特異である場合、法線および接線は変更されません。
		/// @remark UV 座標および三角形インデックスは変更されません。
		/// @remark 変換の線形部分の行列式が負の場合、接線の `w` 成分が反転しますが、三角形の巻き順は変更されません。必要に応じて `reverseWinding()` を使用してください。
		Mesh3D& transform(const Mat4x4& matrix) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	translated
		//
		////////////////////////////////////////////////////////////////

		/// @brief 平行移動した 3D メッシュを返します。
		/// @param offset 平行移動量
		/// @return 平行移動した 3D メッシュ
		/// @remark 頂点座標のみが変更されます。法線、接線、UV 座標、および三角形インデックスは変更されません。
		[[nodiscard]]
		Mesh3D translated(Float3 offset) const&;

		/// @brief 平行移動した 3D メッシュを返します。
		/// @param offset 平行移動量
		/// @return 平行移動した 3D メッシュ
		/// @remark このオーバーロードは自身のストレージを再利用します。
		/// @remark 頂点座標のみが変更されます。法線、接線、UV 座標、および三角形インデックスは変更されません。
		[[nodiscard]]
		Mesh3D translated(Float3 offset) && noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	translate
		//
		////////////////////////////////////////////////////////////////

		/// @brief 3D メッシュを平行移動します。
		/// @param offset 平行移動量
		/// @return *this
		/// @remark 頂点座標のみが変更されます。法線、接線、UV 座標、および三角形インデックスは変更されません。
		Mesh3D& translate(Float3 offset) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	rotated
		//
		////////////////////////////////////////////////////////////////

		/// @brief 原点を中心に回転させた 3D メッシュを返します。
		/// @param rotation 適用する回転を表す単位クォータニオン
		/// @return 回転させた 3D メッシュ
		/// @remark `rotation` は正規化されている必要があります。
		/// @remark 頂点座標、法線、および接線ベクトルが回転します。接線の `w` 成分、UV 座標、および三角形インデックスは変更されません。
		[[nodiscard]]
		Mesh3D rotated(const Quaternion& rotation) const&;

		/// @brief 原点を中心に回転させた 3D メッシュを返します。
		/// @param rotation 適用する回転を表す単位クォータニオン
		/// @return 回転させた 3D メッシュ
		/// @remark このオーバーロードは自身のストレージを再利用します。
		/// @remark `rotation` は正規化されている必要があります。
		/// @remark 頂点座標、法線、および接線ベクトルが回転します。接線の `w` 成分、UV 座標、および三角形インデックスは変更されません。
		[[nodiscard]]
		Mesh3D rotated(const Quaternion& rotation) && noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	rotate
		//
		////////////////////////////////////////////////////////////////

		/// @brief 3D メッシュを原点を中心に回転させます。
		/// @param rotation 適用する回転を表す単位クォータニオン
		/// @return *this
		/// @remark `rotation` は正規化されている必要があります。
		/// @remark 頂点座標、法線、および接線ベクトルが回転します。接線の `w` 成分、UV 座標、および三角形インデックスは変更されません。
		Mesh3D& rotate(const Quaternion& rotation) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	scaled
		//
		////////////////////////////////////////////////////////////////

		/// @brief 原点を中心に一様に拡大・縮小した 3D メッシュを返します。
		/// @param scale 拡大率
		/// @return 拡大・縮小した 3D メッシュ
		/// @remark `scale` が正の場合、法線および接線は変更されません。
		/// @remark `scale` が負の場合、法線、接線ベクトル、および接線の `w` 成分が反転します。
		/// @remark `scale` が 0 の場合、法線および接線は変更されません。
		/// @remark UV 座標および三角形インデックスは変更されません。
		/// @remark `scale` が負の場合でも三角形の巻き順は変更されません。必要に応じて `reverseWinding()` を使用してください。
		[[nodiscard]]
		Mesh3D scaled(float scale) const&;

		/// @brief 原点を中心に一様に拡大・縮小した 3D メッシュを返します。
		/// @param scale 拡大率
		/// @return 拡大・縮小した 3D メッシュ
		/// @remark このオーバーロードは自身のストレージを再利用します。
		/// @remark `scale` が正の場合、法線および接線は変更されません。
		/// @remark `scale` が負の場合、法線、接線ベクトル、および接線の `w` 成分が反転します。
		/// @remark `scale` が 0 の場合、法線および接線は変更されません。
		/// @remark UV 座標および三角形インデックスは変更されません。
		/// @remark `scale` が負の場合でも三角形の巻き順は変更されません。必要に応じて `reverseWinding()` を使用してください。
		[[nodiscard]]
		Mesh3D scaled(float scale) && noexcept;

		/// @brief 原点を中心に各軸方向へ拡大・縮小した 3D メッシュを返します。
		/// @param scale 各軸方向の拡大率
		/// @return 拡大・縮小した 3D メッシュ
		/// @remark `scale` のすべての成分が非ゼロの場合、法線および接線ベクトルは変換後に正規化されます。
		/// @remark `scale` のいずれかの成分が 0 の場合、法線および接線は変更されません。
		/// @remark UV 座標および三角形インデックスは変更されません。
		/// @remark 拡大率の積が負の場合、接線の `w` 成分が反転しますが、三角形の巻き順は変更されません。必要に応じて `reverseWinding()` を使用してください。
		[[nodiscard]]
		Mesh3D scaled(Float3 scale) const&;

		/// @brief 原点を中心に各軸方向へ拡大・縮小した 3D メッシュを返します。
		/// @param scale 各軸方向の拡大率
		/// @return 拡大・縮小した 3D メッシュ
		/// @remark このオーバーロードは自身のストレージを再利用します。
		/// @remark `scale` のすべての成分が非ゼロの場合、法線および接線ベクトルは変換後に正規化されます。
		/// @remark `scale` のいずれかの成分が 0 の場合、法線および接線は変更されません。
		/// @remark UV 座標および三角形インデックスは変更されません。
		/// @remark 拡大率の積が負の場合、接線の `w` 成分が反転しますが、三角形の巻き順は変更されません。必要に応じて `reverseWinding()` を使用してください。
		[[nodiscard]]
		Mesh3D scaled(Float3 scale) && noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	scale
		//
		////////////////////////////////////////////////////////////////

		/// @brief 3D メッシュを原点を中心に一様に拡大・縮小します。
		/// @param scale 拡大率
		/// @return *this
		/// @remark `scale` が正の場合、法線および接線は変更されません。
		/// @remark `scale` が負の場合、法線、接線ベクトル、および接線の `w` 成分が反転します。
		/// @remark `scale` が 0 の場合、法線および接線は変更されません。
		/// @remark UV 座標および三角形インデックスは変更されません。
		/// @remark `scale` が負の場合でも三角形の巻き順は変更されません。必要に応じて `reverseWinding()` を使用してください。
		Mesh3D& scale(float scale) noexcept;

		/// @brief 3D メッシュを原点を中心に各軸方向へ拡大・縮小します。
		/// @param scale 各軸方向の拡大率
		/// @return *this
		/// @remark `scale` のすべての成分が非ゼロの場合、法線および接線ベクトルは変換後に正規化されます。
		/// @remark `scale` のいずれかの成分が 0 の場合、法線および接線は変更されません。
		/// @remark UV 座標および三角形インデックスは変更されません。
		/// @remark 拡大率の積が負の場合、接線の `w` 成分が反転しますが、三角形の巻き順は変更されません。必要に応じて `reverseWinding()` を使用してください。
		Mesh3D& scale(Float3 scale) noexcept;

	};
}

# include "detail/Mesh3D.ipp"
