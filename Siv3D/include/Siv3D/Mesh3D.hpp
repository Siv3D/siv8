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

# include "Common.hpp"
# include "Array.hpp"
# include "Vertex3D.hpp"
# include "TriangleIndex32.hpp"
# include "VertexNormalWeighting.hpp"

namespace s3d
{
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
	};
}

# include "detail/Mesh3D.ipp"
