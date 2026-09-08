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

# include "Mesh3DTestHelper.hpp"
# include "Mesh3DAssemblyExamples.hpp"
# include <map>
# include <sstream>

namespace
{
	using Assembly = Mesh3DAssembly;

	std::string Text(const Blob& blob)
	{
		return { reinterpret_cast<const char*>(blob.data()), blob.size() };
	}

	struct Face
	{
		std::string group, material;
		std::array<size_t, 3> indices;
	};

	struct ParsedOBJ
	{
		std::vector<Float3> positions, normals;
		std::vector<Float2> uv;
		std::vector<Face> faces;
	};

	// Independent line-oriented consumer of the exported OBJ subset.
	ParsedOBJ ParseOBJ(const std::string& text)
	{
		ParsedOBJ parsed;
		std::istringstream input{ text };
		std::string line, group, material;
		while (std::getline(input, line))
		{
			std::istringstream row{ line };
			std::string key;
			row >> key;
			if (key == "v" || key == "vn")
			{
				Float3 v;
				row >> v.x >> v.y >> v.z;
				(key == "v" ? parsed.positions : parsed.normals).push_back(v);
			}
			else if (key == "vt")
			{
				Float2 uv;
				row >> uv.x >> uv.y;
				parsed.uv.push_back(uv);
			}
			else if (key == "g") { row >> group; }
			else if (key == "usemtl") { row >> material; }
			else if (key == "f")
			{
				Face face{ group, material, {} };
				for (auto& index : face.indices)
				{
					size_t tex, normal;
					char slash1, slash2;
					row >> index >> slash1 >> tex >> slash2 >> normal;
					REQUIRE(row);
					CHECK_EQ(slash1, '/');
					CHECK_EQ(slash2, '/');
					CHECK_EQ(index, tex);
					CHECK_EQ(index, normal);
					REQUIRE(index > 0);
					--index;
				}
				parsed.faces.push_back(face);
			}
		}
		return parsed;
	}

	Assembly MakeAssembly()
	{
		Assembly a;
		const auto mesh = a.addMesh(Mesh3D::Box()).value();
		const auto red = a.addMaterial(Material{ .name = U"paint", .baseColor = ColorF{ 1, 0.25, 0.125 } });
		const auto blue = a.addMaterial(Material{ .name = U"paint", .baseColor = ColorF{ 0.125, 0.25, 1 } });
		(void)a.addPart({ .name = U"pivot", .placement = Vec3{ 0, 3, 0 } }).value();
		(void)a.addPart({ .name = U"shell", .mesh = mesh, .material = red, .parent = Assembly::PartID{ 0 } }).value();
		(void)a.addPart({ .name = U"shell", .mesh = mesh, .material = blue,
			.placement = Mat4x4::Scale(Float3{ -1, 2, 1 }) * Mat4x4::Translate(2, 0, 0) }).value();
		(void)a.addPart({ .mesh = mesh, .placement = Vec3{ -2, 0, 0 } }).value();
		return a;
	}

	std::map<std::string, ColorF> ParseDiffuseColors(const std::string& text)
	{
		std::map<std::string, ColorF> colors;
		std::istringstream input{ text };
		std::string line, current;
		while (std::getline(input, line))
		{
			std::istringstream row{ line };
			std::string key;
			row >> key;
			if (key == "newmtl")
			{
				row >> current;
				REQUIRE(colors.emplace(current, ColorF{}).second);
			}
			else if (key == "Kd")
			{
				auto& color = colors.at(current);
				row >> color.r >> color.g >> color.b;
				REQUIRE(row);
			}
			else if (key == "d")
			{
				row >> colors.at(current).a;
				REQUIRE(row);
			}
		}
		return colors;
	}

	class FailingWriter : public IWriter
	{
	public:
		bool open = true;
		bool isOpen() const noexcept override { return open; }
		int64 size() const override { return 0; }
		int64 getPos() const override { return 0; }
		bool setPos(int64) override { return false; }
		int64 write(const void*, int64 sizeBytes) override { return (sizeBytes - 1); }
	};
}

TEST_CASE("Mesh3DAssembly::OBJ groups materials and round-trip geometry")
{
	const auto baked = MakeAssembly().bake().value();
	MemoryWriter obj, mtl;
	REQUIRE(baked.encodeOBJ(obj, mtl, U"model.mtl"));
	const std::string text = Text(obj.getBlob());
	const std::string materialText = Text(mtl.getBlob());
	CHECK(text.starts_with("mtllib model.mtl\n"));
	CHECK_EQ(text.find('\r'), std::string::npos);
	CHECK_EQ(materialText.find('\r'), std::string::npos);
	CHECK_EQ(text.find("g part_0"), std::string::npos);
	CHECK_NE(materialText.find("newmtl material_0_paint\nKd 1 0.25 0.125\n"), std::string::npos);
	CHECK_NE(materialText.find("newmtl material_1_paint\nKd 0.125 0.25 1\n"), std::string::npos);
	CHECK_NE(materialText.find("newmtl material_default\nKd 1 1 1\n"), std::string::npos);
	CHECK_EQ(materialText.find("map_"), std::string::npos);
	const ParsedOBJ parsed = ParseOBJ(text);
	REQUIRE_EQ(parsed.positions.size(), baked.mesh.vertexCount());
	REQUIRE_EQ(parsed.normals.size(), parsed.positions.size());
	REQUIRE_EQ(parsed.uv.size(), parsed.positions.size());
	REQUIRE_EQ(parsed.faces.size(), baked.mesh.triangleCount());
	for (size_t i = 0; i < parsed.positions.size(); ++i)
	{
		const auto& v = baked.mesh.vertices[i];
		CHECK_EQ(parsed.positions[i], (Float3{ v.pos.x, v.pos.y, -v.pos.z }));
		CHECK_EQ(parsed.normals[i], (Float3{ v.normal.x, v.normal.y, -v.normal.z }));
		CHECK_EQ(parsed.uv[i], (Float2{ v.tex.x, (1.0f - v.tex.y) }));
	}
	const std::array<std::string, 3> groups{ "part_1_shell", "part_2_shell", "part_3" };
	const std::array<std::string, 3> materials{ "material_0_paint", "material_1_paint", "material_default" };
	for (size_t i = 0; i < parsed.faces.size(); ++i)
	{
		const auto& face = parsed.faces[i];
		const auto& t = baked.mesh.indices[i];
		CHECK_EQ(face.group, groups[i / 12]);
		CHECK_EQ(face.material, materials[i / 12]);
		CHECK_EQ(face.indices[0], t.i0);
		CHECK_EQ(face.indices[1], t.i2);
		CHECK_EQ(face.indices[2], t.i1);
		const auto p0 = parsed.positions[face.indices[0]], p1 = parsed.positions[face.indices[1]], p2 = parsed.positions[face.indices[2]];
		CHECK((p1 - p0).cross(p2 - p0).dot(parsed.normals[face.indices[0]]) > 0);
	}
	MemoryWriter againOBJ, againMTL;
	REQUIRE(baked.encodeOBJ(againOBJ, againMTL, U"model.mtl"));
	CHECK_EQ(Text(againOBJ.getBlob()), text);
	CHECK_EQ(Text(againMTL.getBlob()), materialText);
}

TEST_CASE("Mesh3DAssembly::OBJ reversible names and unused materials")
{
	auto baked = MakeAssembly().bake().value();
	baked.parts[1].name = U"外装 #\n%20\\";
	baked.materials[0].name = U"paint\t#\n";
	baked.materials[1].name.clear();
	baked.materials << Material{ .baseColor = ColorF{ std::numeric_limits<double>::quiet_NaN() } };
	MemoryWriter obj, mtl;
	REQUIRE(baked.encodeOBJ(obj, mtl, U"x.mtl"));
	CHECK_NE(Text(obj.getBlob()).find("g part_1_%E5%A4%96%E8%A3%85%20%23%0A%2520%5C\n"), std::string::npos);
	CHECK_NE(Text(mtl.getBlob()).find("newmtl material_0_paint%09%23%0A\n"), std::string::npos);
	CHECK_NE(Text(mtl.getBlob()).find("newmtl material_1\n"), std::string::npos);
	CHECK_EQ(Text(mtl.getBlob()).find("material_2"), std::string::npos);
}

TEST_CASE("Mesh3DAssembly::OBJ rejects stale ranges and invalid input before writing")
{
	const ScopedLogSilencer silence;
	auto baked = MakeAssembly().bake().value();
	SUBCASE("empty mesh") { baked.mesh.clear(); }
	SUBCASE("missing part") { baked.parts.pop_back(); }
	SUBCASE("overlapping triangle range") { --baked.parts[2].range.triangleOffset; }
	SUBCASE("vertex range overflow") { baked.parts[1].range.vertexCount = static_cast<size_t>(-1); }
	SUBCASE("triangle range overflow") { baked.parts[1].range.triangleCount = static_cast<size_t>(-1); }
	SUBCASE("part ID mismatch") { baked.parts[2].id = Assembly::PartID{ 1 }; }
	SUBCASE("cross-part index") { baked.mesh.indices[0].i0 = 24; }
	SUBCASE("invalid index") { baked.mesh.indices[0].i0 = 9999; }
	SUBCASE("missing material") { baked.parts[1].material = Assembly::MaterialID{ 99 }; }
	SUBCASE("invalid material value") { baked.materials[0].roughness = std::numeric_limits<double>::infinity(); }
	SUBCASE("invalid texture UV set") { baked.materials[0].baseColorTexture = MaterialTexture{ U"texture.png", 1 }; }
	SUBCASE("invalid texture path") { baked.materials[0].baseColorTexture = MaterialTexture{ U"a\nnewmtl injected", 0 }; }
	SUBCASE("invalid position") { baked.mesh.vertices[0].pos.x = std::numeric_limits<float>::infinity(); }
	SUBCASE("invalid UV") { baked.mesh.vertices[0].tex.y = std::numeric_limits<float>::quiet_NaN(); }
	SUBCASE("invalid normal") { baked.mesh.vertices[0].normal.z = std::numeric_limits<float>::quiet_NaN(); }
	MemoryWriter obj, mtl;
	REQUIRE_EQ(obj.write("old", 3), 3);
	REQUIRE_EQ(mtl.write("old", 3), 3);
	CHECK_FALSE(baked.encodeOBJ(obj, mtl, U"model.mtl"));
	CHECK_EQ(Text(obj.getBlob()), "old");
	CHECK_EQ(Text(mtl.getBlob()), "old");
}

TEST_CASE("Mesh3DAssembly::OBJ writer and filename failures")
{
	const ScopedLogSilencer silence;
	const auto baked = MakeAssembly().bake().value();
	for (const String name : { U"", U".", U"..", U"two files.mtl", U"a\nb.mtl", U"a#b.mtl", U"dir/a.mtl", U"a\\b.mtl", U"日本.mtl" })
	{
		MemoryWriter obj, mtl;
		CHECK_FALSE(baked.encodeOBJ(obj, mtl, name));
		CHECK_EQ(obj.size(), int64{ 0 });
		CHECK_EQ(mtl.size(), int64{ 0 });
	}
	MemoryWriter writer;
	CHECK_FALSE(baked.encodeOBJ(writer, writer, U"model.mtl"));
	FailingWriter failure;
	CHECK_FALSE(baked.encodeOBJ(writer, failure, U"model.mtl"));
	CHECK_FALSE(baked.encodeOBJ(failure, writer, U"model.mtl"));
	failure.open = false;
	writer.clear();
	CHECK_FALSE(baked.encodeOBJ(failure, writer, U"model.mtl"));
	CHECK_EQ(writer.size(), int64{ 0 });
	CHECK_FALSE(baked.encodeOBJ(writer, failure, U"model.mtl"));
	CHECK_EQ(writer.size(), int64{ 0 });
}

TEST_CASE("Mesh3DAssembly::OBJ save pair and preserve files on validation failure")
{
	const ScopedLogSilencer silence;
	const auto a = MakeAssembly();
	const FilePath directory = FileSystem::UniqueFilePath(Test::OutputPath(U"mesh3dassembly/"));
	REQUIRE(FileSystem::CreateDirectories(directory));
	const FilePath objPath = (directory + U"/model name.obj");
	const FilePath mtlPath = (directory + U"/model%20name.mtl");
	REQUIRE(a.saveOBJ(objPath));
	const auto obj = Text(Blob{ objPath }), mtl = Text(Blob{ mtlPath });
	CHECK(obj.starts_with("mtllib model%20name.mtl\n"));
	MemoryWriter expectedOBJ, expectedMTL;
	REQUIRE(a.bake().value().encodeOBJ(expectedOBJ, expectedMTL, U"model%20name.mtl"));
	CHECK_EQ(obj, Text(expectedOBJ.getBlob()));
	CHECK_EQ(mtl, Text(expectedMTL.getBlob()));
	auto invalid = a.bake().value();
	invalid.parts[1].range.triangleCount = 999;
	CHECK_FALSE(invalid.saveOBJ(objPath));
	CHECK_FALSE(a.saveOBJ(objPath, { .maxVertexCount = 1 }));
	CHECK_EQ(Text(Blob{ objPath }), obj);
	CHECK_EQ(Text(Blob{ mtlPath }), mtl);
	CHECK_FALSE(a.saveOBJ(directory + U"/collision.MTL"));
	CHECK_FALSE(FileSystem::Exists(directory + U"/collision.MTL"));
	CHECK_FALSE(a.saveOBJ(directory)); // A directory cannot be opened as an OBJ file.
	FileSystem::Remove(directory);
}

TEST_CASE("Mesh3DAssembly::OBJ texture-free robot and palette variants")
{
	for (const ColorF color : { ColorF{ 0.95, 0.38, 0.08 }, ColorF{ 0.16, 0.48, 0.72 } })
	{
		const Assembly robot = Mesh3DAssemblyExamples::MakeRobot(color);
		const auto baked = robot.bake().value();
		CHECK_GT(robot.partCount(), robot.meshCount());
		CHECK_EQ(robot.materialCount(), size_t{ 8 });
		Mesh3DTest::CheckMeshGeometry(baked.mesh);
		MemoryWriter obj, mtl;
		REQUIRE(baked.encodeOBJ(obj, mtl, U"robot.mtl"));
		CHECK_EQ(Text(mtl.getBlob()).find("map_"), std::string::npos);
		const auto parsed = ParseOBJ(Text(obj.getBlob()));
		const auto colors = ParseDiffuseColors(Text(mtl.getBlob()));
		CHECK_EQ(parsed.faces.size(), baked.mesh.triangleCount());
		CHECK_EQ(colors.size(), baked.materials.size());
		for (const auto& part : baked.parts)
		{
			for (size_t t = part.range.triangleOffset; t < (part.range.triangleOffset + part.range.triangleCount); ++t)
			{
				REQUIRE(part.material);
				const ColorF expected = baked.materials[static_cast<size_t>(*part.material)].baseColor;
				CHECK_EQ(colors.at(parsed.faces[t].material), expected);
			}
		}
		for (const auto& material : baked.materials)
		{
			CHECK_EQ(material.metallic, 0.0);
			CHECK_FALSE(material.baseColorTexture);
		}
	}
}
