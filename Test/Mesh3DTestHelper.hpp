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
		CHECK(vertex.normal.length() == doctest::Approx(1.0f).epsilon(FrameEpsilon));
		CHECK(vertex.tangent.xyz().length() == doctest::Approx(1.0f).epsilon(FrameEpsilon));
		CHECK(std::abs(vertex.normal.dot(vertex.tangent.xyz())) < FrameEpsilon);

		if (tangentHandedness == TangentHandedness::Positive)
		{
			CHECK(std::abs(vertex.tangent.w - 1.0f) < FrameEpsilon);
		}
		else
		{
			CHECK(std::abs(std::abs(vertex.tangent.w) - 1.0f) < FrameEpsilon);
		}

		CHECK(vertex.bitangent().length() == doctest::Approx(1.0f).epsilon(FrameEpsilon));
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
}
