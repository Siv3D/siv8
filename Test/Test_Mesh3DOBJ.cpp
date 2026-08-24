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

# include "Siv3DTest.hpp"

namespace
{
	[[nodiscard]]
	static std::string BlobToString(const Blob& blob)
	{
		return std::string{
			reinterpret_cast<const char*>(blob.data()),
			blob.size()
		};
	}

	[[nodiscard]]
	static Mesh3D MakeTriangleMesh()
	{
		return Mesh3D{
			{
				Vertex3D{
					.pos = Float3{ 1.5f, -2.0f, 3.25f },
					.normal = Float3{ 0.0f, 1.0f, 0.0f },
					.tex = Float2{ 0.25f, 0.75f },
					.tangent = Float4{ 1.0f, 0.0f, 0.0f, 1.0f }
				},
				Vertex3D{
					.pos = Float3{ 0.0f, 0.0f, 0.0f },
					.normal = Float3{ 0.0f, 0.0f, 1.0f },
					.tex = Float2{ 0.0f, 1.0f },
					.tangent = Float4{ 1.0f, 0.0f, 0.0f, -1.0f }
				},
				Vertex3D{
					.pos = Float3{ -1.0f, 2.0f, 4.0f },
					.normal = Float3{ -1.0f, 0.0f, 0.0f },
					.tex = Float2{ 1.0f, 0.0f },
					.tangent = Float4{ 0.0f, 0.0f, 1.0f, 1.0f }
				}
			},
			{ TriangleIndex32{ 0, 1, 2 } }
		};
	}

	class ShortWriter : public IWriter
	{
	public:

		[[nodiscard]]
		bool isOpen() const noexcept override
		{
			return true;
		}

		[[nodiscard]]
		int64 size() const override
		{
			return 0;
		}

		[[nodiscard]]
		int64 getPos() const override
		{
			return 0;
		}

		bool setPos(int64) override
		{
			return false;
		}

		int64 write(const void*, const int64 sizeBytes) override
		{
			return (sizeBytes - 1);
		}
	};

	constexpr std::string_view ExpectedOBJ =
		"v 1.5 -2 3.25\n"
		"v 0 0 0\n"
		"v -1 2 4\n"
		"vt 0.25 0.75\n"
		"vt 0 1\n"
		"vt 1 0\n"
		"vn 0 1 0\n"
		"vn 0 0 1\n"
		"vn -1 0 0\n"
		"f 1/1/1 2/2/2 3/3/3\n";
}

TEST_CASE("Mesh3D::encodeOBJ")
{
	const Mesh3D mesh = MakeTriangleMesh();

	MemoryWriter writer;
	REQUIRE(mesh.encodeOBJ(writer));
	CHECK_EQ(BlobToString(writer.getBlob()), ExpectedOBJ);

	const Blob blob = mesh.encodeOBJ();
	CHECK_EQ(BlobToString(blob), ExpectedOBJ);
}

TEST_CASE("Mesh3D::saveOBJ")
{
	const FilePath path{ U"../../Test/output/mesh3d.obj" };
	const Mesh3D mesh = MakeTriangleMesh();

	FileSystem::Remove(path);
	REQUIRE(mesh.saveOBJ(path));
	CHECK_EQ(BlobToString(Blob{ path }), ExpectedOBJ);
	FileSystem::Remove(path);
}

TEST_CASE("Mesh3D::encodeOBJ preserves winding and vertex attributes")
{
	Mesh3D mesh = MakeTriangleMesh();
	mesh.indices.push_back(TriangleIndex32{ 2, 1, 0 });
	mesh.vertices[0].tangent = Float4{
		std::numeric_limits<float>::quiet_NaN(),
		std::numeric_limits<float>::quiet_NaN(),
		std::numeric_limits<float>::quiet_NaN(),
		std::numeric_limits<float>::quiet_NaN()
	};

	const std::string obj = BlobToString(mesh.encodeOBJ());
	CHECK(obj.ends_with("f 1/1/1 2/2/2 3/3/3\nf 3/3/3 2/2/2 1/1/1\n"));
}

TEST_CASE("Mesh3D::encodeOBJ accepts finite float boundary values")
{
	Mesh3D mesh = MakeTriangleMesh();
	mesh.vertices[0].pos = Float3{
		std::numeric_limits<float>::max(),
		std::numeric_limits<float>::lowest(),
		std::numeric_limits<float>::denorm_min()
	};
	mesh.vertices[0].tex = Float2{
		std::numeric_limits<float>::max(),
		std::numeric_limits<float>::lowest()
	};
	mesh.vertices[0].normal = Float3{
		std::numeric_limits<float>::denorm_min(),
		-std::numeric_limits<float>::denorm_min(),
		std::numeric_limits<float>::max()
	};

	const Blob blob = mesh.encodeOBJ();
	CHECK_FALSE(blob.isEmpty());

	const std::string obj = BlobToString(blob);
	CHECK_EQ(obj.find("inf"), std::string::npos);
	CHECK_EQ(obj.find("nan"), std::string::npos);
}

TEST_CASE("Mesh3D::encodeOBJ rejects invalid input")
{
	SUBCASE("empty mesh")
	{
		MemoryWriter writer;
		CHECK_FALSE(Mesh3D{}.encodeOBJ(writer));
		CHECK(writer.getBlob().isEmpty());
		CHECK(Mesh3D{}.encodeOBJ().isEmpty());
	}

	SUBCASE("vertices without triangles")
	{
		Mesh3D mesh = MakeTriangleMesh();
		mesh.indices.clear();
		CHECK(mesh.encodeOBJ().isEmpty());
	}

	SUBCASE("out-of-range index")
	{
		Mesh3D mesh = MakeTriangleMesh();
		mesh.indices[0].i2 = 3;
		CHECK(mesh.encodeOBJ().isEmpty());
	}

	SUBCASE("non-finite position")
	{
		Mesh3D mesh = MakeTriangleMesh();
		mesh.vertices[0].pos.x = std::numeric_limits<float>::infinity();
		CHECK(mesh.encodeOBJ().isEmpty());
	}

	SUBCASE("non-finite texture coordinate")
	{
		Mesh3D mesh = MakeTriangleMesh();
		mesh.vertices[0].tex.y = std::numeric_limits<float>::quiet_NaN();
		CHECK(mesh.encodeOBJ().isEmpty());
	}

	SUBCASE("non-finite normal")
	{
		Mesh3D mesh = MakeTriangleMesh();
		mesh.vertices[0].normal.z = -std::numeric_limits<float>::infinity();
		CHECK(mesh.encodeOBJ().isEmpty());
	}
}

TEST_CASE("Mesh3D::encodeOBJ reports writer failure")
{
	const Mesh3D mesh = MakeTriangleMesh();

	BinaryFileWriter closedWriter;
	CHECK_FALSE(mesh.encodeOBJ(closedWriter));

	ShortWriter shortWriter;
	CHECK_FALSE(mesh.encodeOBJ(shortWriter));
}
