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
# include "Siv3DTest.hpp"

namespace s3d::Mesh3DTest
{
	// Adapt height-based fixtures while keeping ownership in each test.
	template <class Sections, class Heights>
	Array<LoftSection> LoftSections(const Sections& sections, const Heights& heights)
	{
		REQUIRE((sections.size()) == (heights.size()));
		Array<LoftSection> result;
		result.reserve(sections.size());
		for (size_t i = 0; i < sections.size(); ++i)
		{
			result.push_back({ sections[i], Vec3{ 0, heights[i], 0 } });
		}
		return result;
	}

	constexpr float FrameEpsilon = 1e-5f;
	constexpr float TriangleAreaEpsilon = 1e-10f;

	enum class TangentHandedness
	{
		Either,
		Positive,
	};

	inline void CheckVertexFrame(
		const Vertex3D& vertex,
		const TangentHandedness tangentHandedness = TangentHandedness::Either)
	{
		CHECK(std::isfinite(vertex.pos.x));
		CHECK(std::isfinite(vertex.pos.y));
		CHECK(std::isfinite(vertex.pos.z));
		CHECK(std::isfinite(vertex.tex.x));
		CHECK(std::isfinite(vertex.tex.y));
		CHECK(vertex.normal.length() == Test::Approx(1.0f).epsilon(FrameEpsilon));
		CHECK(vertex.tangent.xyz().length() == Test::Approx(1.0f).epsilon(FrameEpsilon));
		CHECK(std::abs(vertex.normal.dot(vertex.tangent.xyz())) < FrameEpsilon);

		if (tangentHandedness == TangentHandedness::Positive)
		{
			CHECK(std::abs(vertex.tangent.w - 1.0f) < FrameEpsilon);
		}
		else
		{
			CHECK(std::abs(std::abs(vertex.tangent.w) - 1.0f) < FrameEpsilon);
		}

		CHECK(vertex.bitangent().length() == Test::Approx(1.0f).epsilon(FrameEpsilon));
	}

	inline void CheckMeshGeometry(
		const Mesh3D& mesh,
		const TangentHandedness tangentHandedness = TangentHandedness::Either)
	{
		REQUIRE_FALSE(mesh.isEmpty());
		REQUIRE(mesh.validate());

		for (const Vertex3D& vertex : mesh.vertices)
		{
			CheckVertexFrame(vertex, tangentHandedness);
		}

		for (const TriangleIndex32& triangle : mesh.indices)
		{
			const Vertex3D& v0 = mesh.vertices[triangle.i0];
			const Vertex3D& v1 = mesh.vertices[triangle.i1];
			const Vertex3D& v2 = mesh.vertices[triangle.i2];
			const Float3 faceNormal = (v1.pos - v0.pos).cross(v2.pos - v0.pos);
			const Float3 vertexNormal = (v0.normal + v1.normal + v2.normal);

			CHECK(faceNormal.lengthSq() > TriangleAreaEpsilon);
			CHECK(faceNormal.dot(vertexNormal) > 0.0f);
		}
	}

	inline void CheckMeshDataEqual(const Mesh3D& actual, const Mesh3D& expected)
	{
		REQUIRE((actual.vertexCount()) == (expected.vertexCount()));
		REQUIRE((actual.triangleCount()) == (expected.triangleCount()));

		for (size_t i = 0; i < actual.vertexCount(); ++i)
		{
			CHECK((actual.vertices[i].pos) == (expected.vertices[i].pos));
			CHECK((actual.vertices[i].normal) == (expected.vertices[i].normal));
			CHECK((actual.vertices[i].tex) == (expected.vertices[i].tex));
			CHECK((actual.vertices[i].tangent) == (expected.vertices[i].tangent));
		}

		for (size_t i = 0; i < actual.triangleCount(); ++i)
		{
			CHECK((actual.indices[i].i0) == (expected.indices[i].i0));
			CHECK((actual.indices[i].i1) == (expected.indices[i].i1));
			CHECK((actual.indices[i].i2) == (expected.indices[i].i2));
		}
	}
}
