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

# include "Mesh3DMikkTSpace.hpp"
# include <ThirdParty/MikkTSpace/mikktspace.h>

namespace s3d
{
	namespace
	{
		struct MikkTSpaceUserData
		{
			const Array<Vertex3D>* vertices = nullptr;

			const Array<TriangleIndex32>* indices = nullptr;

			Array<Float4>* cornerTangents = nullptr;
		};

		struct TangentVariant
		{
			Float4 tangent;

			uint32 destinationVertexIndex = 0;

			size_t nextVariantIndex = 0;
		};

		inline constexpr size_t InvalidVariantIndex = std::numeric_limits<size_t>::max();

		[[nodiscard]]
		constexpr uint32 GetVertexIndex(
			const TriangleIndex32& triangle,
			const int cornerIndex) noexcept
		{
			switch (cornerIndex)
			{
			case 0:
				return triangle.i0;
			case 1:
				return triangle.i1;
			default:
				return triangle.i2;
			}
		}

		void SetVertexIndex(
			TriangleIndex32& triangle,
			const int cornerIndex,
			const uint32 vertexIndex) noexcept
		{
			switch (cornerIndex)
			{
			case 0:
				triangle.i0 = vertexIndex;
				break;
			case 1:
				triangle.i1 = vertexIndex;
				break;
			default:
				triangle.i2 = vertexIndex;
				break;
			}
		}

		[[nodiscard]]
		const Vertex3D& GetVertex(
			const MikkTSpaceUserData& data,
			const int faceIndex,
			const int cornerIndex) noexcept
		{
			const TriangleIndex32& triangle
				= (*data.indices)[static_cast<size_t>(faceIndex)];

			return (*data.vertices)[GetVertexIndex(triangle, cornerIndex)];
		}

		[[nodiscard]]
		bool IsFinite(const Float2& value) noexcept
		{
			return (std::isfinite(value.x)
				&& std::isfinite(value.y));
		}

		[[nodiscard]]
		bool IsFinite(const Float3& value) noexcept
		{
			return (std::isfinite(value.x)
				&& std::isfinite(value.y)
				&& std::isfinite(value.z));
		}

		[[nodiscard]]
		bool IsFinite(const Float4& value) noexcept
		{
			return (std::isfinite(value.x)
				&& std::isfinite(value.y)
				&& std::isfinite(value.z)
				&& std::isfinite(value.w));
		}

		[[nodiscard]]
		bool IsZeroVector(const Float3& value) noexcept
		{
			return ((value.x == 0.0f)
				&& (value.y == 0.0f)
				&& (value.z == 0.0f));
		}

		[[nodiscard]]
		bool EqualTangent(
			const Float4& a,
			const Float4& b) noexcept
		{
			// MikkTSpace が同一の接空間として返した値は完全一致する。
			// epsilon 比較で異なる接空間を統合すると、ノーマルマップの
			// シームを破壊する可能性があるため、完全一致で比較する。
			return ((a.x == b.x)
				&& (a.y == b.y)
				&& (a.z == b.z)
				&& (a.w == b.w));
		}

		[[nodiscard]]
		int MikkGetNumFaces(const SMikkTSpaceContext* context)
		{
			const auto& data
				= *static_cast<const MikkTSpaceUserData*>(context->m_pUserData);

			return static_cast<int>(data.indices->size());
		}

		[[nodiscard]]
		int MikkGetNumVerticesOfFace(
			const SMikkTSpaceContext*,
			const int)
		{
			return 3;
		}

		void MikkGetPosition(
			const SMikkTSpaceContext* context,
			float position[3],
			const int faceIndex,
			const int cornerIndex)
		{
			const auto& data
				= *static_cast<const MikkTSpaceUserData*>(context->m_pUserData);

			const Float3& value = GetVertex(data, faceIndex, cornerIndex).pos;
			position[0] = value.x;
			position[1] = value.y;
			position[2] = value.z;
		}

		void MikkGetNormal(
			const SMikkTSpaceContext* context,
			float normal[3],
			const int faceIndex,
			const int cornerIndex)
		{
			const auto& data
				= *static_cast<const MikkTSpaceUserData*>(context->m_pUserData);

			const Float3& value = GetVertex(data, faceIndex, cornerIndex).normal;
			normal[0] = value.x;
			normal[1] = value.y;
			normal[2] = value.z;
		}

		void MikkGetTexCoord(
			const SMikkTSpaceContext* context,
			float texCoord[2],
			const int faceIndex,
			const int cornerIndex)
		{
			const auto& data
				= *static_cast<const MikkTSpaceUserData*>(context->m_pUserData);

			const Float2& value = GetVertex(data, faceIndex, cornerIndex).tex;
			texCoord[0] = value.x;
			texCoord[1] = value.y;
		}

		void MikkSetTSpaceBasic(
			const SMikkTSpaceContext* context,
			const float tangent[3],
			const float sign,
			const int faceIndex,
			const int cornerIndex)
		{
			auto& data
				= *static_cast<MikkTSpaceUserData*>(context->m_pUserData);

			(*data.cornerTangents)[
				(static_cast<size_t>(faceIndex) * 3)
					+ static_cast<size_t>(cornerIndex)] = Float4{
					tangent[0],
					tangent[1],
					tangent[2],
					(sign < 0.0f) ? -1.0f : 1.0f
				};
		}

		[[nodiscard]]
		bool ValidateInput(
			const Array<Vertex3D>& vertices,
			const Array<TriangleIndex32>& indices) noexcept
		{
			if (vertices.isEmpty())
			{
				return false;
			}

			if (indices.size() > static_cast<size_t>(std::numeric_limits<int>::max()))
			{
				return false;
			}

			// TriangleIndex32 で新しい頂点を参照できる余地を残す。
			if (vertices.size() >= static_cast<size_t>(std::numeric_limits<uint32>::max()))
			{
				return false;
			}

			for (const TriangleIndex32& triangle : indices)
			{
				for (int cornerIndex = 0; cornerIndex < 3; ++cornerIndex)
				{
					const uint32 vertexIndex = GetVertexIndex(triangle, cornerIndex);

					if (vertices.size() <= vertexIndex)
					{
						return false;
					}

					const Vertex3D& vertex = vertices[vertexIndex];

					if ((not IsFinite(vertex.pos))
						|| (not IsFinite(vertex.normal))
						|| (not IsFinite(vertex.tex))
						|| IsZeroVector(vertex.normal))
					{
						return false;
					}
				}
			}

			return true;
		}

		[[nodiscard]]
		bool GenerateCornerTangents(
			const Array<Vertex3D>& vertices,
			const Array<TriangleIndex32>& indices,
			Array<Float4>& cornerTangents)
		{
			if (indices.size() > (std::numeric_limits<size_t>::max() / 3))
			{
				return false;
			}

			Array<Float4> result(indices.size() * 3);

			MikkTSpaceUserData userData;
			userData.vertices = &vertices;
			userData.indices = &indices;
			userData.cornerTangents = &result;

			SMikkTSpaceInterface interface{};
			interface.m_getNumFaces = MikkGetNumFaces;
			interface.m_getNumVerticesOfFace = MikkGetNumVerticesOfFace;
			interface.m_getPosition = MikkGetPosition;
			interface.m_getNormal = MikkGetNormal;
			interface.m_getTexCoord = MikkGetTexCoord;
			interface.m_setTSpaceBasic = MikkSetTSpaceBasic;
			interface.m_setTSpace = nullptr;

			SMikkTSpaceContext context{};
			context.m_pInterface = &interface;
			context.m_pUserData = &userData;

			if (not genTangSpaceDefault(&context))
			{
				return false;
			}

			for (const Float4& tangent : result)
			{
				if (not IsFinite(tangent))
				{
					return false;
				}
			}

			cornerTangents = std::move(result);
			return true;
		}

		[[nodiscard]]
		bool RebuildIndexedMesh(
			const Array<Vertex3D>& sourceVertices,
			const Array<TriangleIndex32>& sourceIndices,
			const Array<Float4>& cornerTangents,
			Array<Vertex3D>& destinationVertices,
			Array<TriangleIndex32>& destinationIndices)
		{
			Array<Vertex3D> newVertices = sourceVertices;
			Array<TriangleIndex32> newIndices = sourceIndices;

			Array<size_t> firstVariantIndices(
				sourceVertices.size(),
				InvalidVariantIndex);

			Array<TangentVariant> variants;
			variants.reserve(cornerTangents.size());

			for (size_t faceIndex = 0; faceIndex < sourceIndices.size(); ++faceIndex)
			{
				const TriangleIndex32& sourceTriangle = sourceIndices[faceIndex];
				TriangleIndex32& destinationTriangle = newIndices[faceIndex];

				for (int cornerIndex = 0; cornerIndex < 3; ++cornerIndex)
				{
					const uint32 sourceVertexIndex
						= GetVertexIndex(sourceTriangle, cornerIndex);

					const Float4& tangent
						= cornerTangents[(faceIndex * 3) + static_cast<size_t>(cornerIndex)];

					size_t variantIndex = firstVariantIndices[sourceVertexIndex];
					uint32 destinationVertexIndex = 0;
					bool found = false;

					while (variantIndex != InvalidVariantIndex)
					{
						const TangentVariant& variant = variants[variantIndex];

						if (EqualTangent(variant.tangent, tangent))
						{
							destinationVertexIndex = variant.destinationVertexIndex;
							found = true;
							break;
						}

						variantIndex = variant.nextVariantIndex;
					}

					if (not found)
					{
						const size_t oldFirstVariantIndex
							= firstVariantIndices[sourceVertexIndex];

						if (oldFirstVariantIndex == InvalidVariantIndex)
						{
							destinationVertexIndex = sourceVertexIndex;
							newVertices[destinationVertexIndex].tangent = tangent;
						}
						else
						{
							if (newVertices.size()
								>= static_cast<size_t>(std::numeric_limits<uint32>::max()))
							{
								return false;
							}

							destinationVertexIndex
								= static_cast<uint32>(newVertices.size());

							Vertex3D vertex = sourceVertices[sourceVertexIndex];
							vertex.tangent = tangent;
							newVertices.push_back(vertex);
						}

						firstVariantIndices[sourceVertexIndex] = variants.size();
						variants.push_back(TangentVariant{
							tangent,
							destinationVertexIndex,
							oldFirstVariantIndex
							});
					}

					SetVertexIndex(destinationTriangle, cornerIndex, destinationVertexIndex);
				}
			}

			destinationVertices = std::move(newVertices);
			destinationIndices = std::move(newIndices);
			return true;
		}
	}

	bool GenerateMikkTSpaceTangents(Array<Vertex3D>& vertices, Array<TriangleIndex32>& indices)
	{
		if (indices.isEmpty())
		{
			return true;
		}

		if (not ValidateInput(vertices, indices))
		{
			return false;
		}

		Array<Float4> cornerTangents;

		if (not GenerateCornerTangents(vertices, indices, cornerTangents))
		{
			return false;
		}

		Array<Vertex3D> newVertices;
		Array<TriangleIndex32> newIndices;

		if (not RebuildIndexedMesh(
			vertices,
			indices,
			cornerTangents,
			newVertices,
			newIndices))
		{
			return false;
		}

		vertices = std::move(newVertices);
		indices = std::move(newIndices);
		return true;
	}
}
