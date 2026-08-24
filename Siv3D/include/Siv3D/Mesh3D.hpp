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
