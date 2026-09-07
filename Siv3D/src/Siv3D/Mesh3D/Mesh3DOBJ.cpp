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

# include <algorithm>
# include <array>
# include <charconv>
# include <cmath>
# include <limits>
# include <string>
# include <Siv3D/Mesh3D.hpp>
# include <Siv3D/Mesh3DAssembly.hpp>
# include <Siv3D/BinaryFileWriter.hpp>
# include <Siv3D/FileSystem.hpp>
# include <Siv3D/MemoryWriter.hpp>
# include <Siv3D/Unicode.hpp>
# include "Mesh3DCommon.hpp"

namespace s3d
{
	namespace
	{
		using Mesh3DDetail::GenerationFailed;

		constexpr size_t OBJBufferFlushThreshold = (64 * 1024);

		[[nodiscard]]
		bool IsSingleLine(const StringView value) noexcept
		{
			for (const char32 ch : value)
			{
				if ((ch < U' ') || (ch == U'\x7F'))
				{
					return false;
				}
			}

			return true;
		}

		[[nodiscard]]
		bool ValidateMaterialTexture(const Optional<MaterialTexture>& texture) noexcept
		{
			return (not texture)
				|| ((texture->uvIndex == 0)
					&& (not texture->path.isEmpty())
					&& IsSingleLine(texture->path));
		}

		[[nodiscard]]
		bool ValidateForMTL(const Material& material, const bool validateName = true) noexcept
		{
			if ((validateName && (material.name.isEmpty() || (not IsSingleLine(material.name))))
				|| (not material.baseColor.isFinite())
				|| (not std::isfinite(material.metallic))
				|| (not std::isfinite(material.roughness))
				|| (not material.emissive.isFinite())
				|| (not std::isfinite(material.alphaCutoff))
				|| (not std::isfinite(material.normalScale))
				|| (not std::isfinite(material.occlusionStrength)))
			{
				return false;
			}

			switch (material.alphaMode)
			{
			case MaterialAlphaMode::Opaque:
			case MaterialAlphaMode::Mask:
			case MaterialAlphaMode::Blend:
				break;
			default:
				return false;
			}

			return (ValidateMaterialTexture(material.baseColorTexture)
				&& ValidateMaterialTexture(material.metallicRoughnessTexture)
				&& ValidateMaterialTexture(material.normalTexture)
				&& ValidateMaterialTexture(material.occlusionTexture)
				&& ValidateMaterialTexture(material.emissiveTexture));
		}

		[[nodiscard]]
		bool ValidateForOBJ(const Mesh3D& mesh) noexcept
		{
			if (mesh.isEmpty() || (not mesh.validate()))
			{
				return false;
			}

			for (const auto& vertex : mesh.vertices)
			{
				if ((not vertex.pos.isFinite())
					|| (not vertex.tex.isFinite())
					|| (not vertex.normal.isFinite()))
				{
					return false;
				}
			}

			return true;
		}

		class OBJTextWriter
		{
		public:

			explicit OBJTextWriter(IWriter& writer)
				: m_writer{ writer }
			{
				m_buffer.reserve(OBJBufferFlushThreshold);
			}

			bool writePosition(const Float3& value)
			{
				m_buffer.append("v ");
				return (appendFloat(value.x)
					&& appendCharacter(' ')
					&& appendFloat(value.y)
					&& appendCharacter(' ')
					&& appendNegatedFloat(value.z)
					&& finishLine());
			}

			bool writeMTLLibrary(const std::string_view fileName)
			{
				m_buffer.append("mtllib ");
				m_buffer.append(fileName);
				return finishLine();
			}

			bool writeUseMaterial(const std::string_view materialName)
			{
				m_buffer.append("usemtl ");
				m_buffer.append(materialName);
				return finishLine();
			}

			bool writeGroup(const std::string_view name)
			{
				m_buffer.append("g ");
				m_buffer.append(name);
				return finishLine();
			}

			bool writeTexCoord(const Float2& value)
			{
				m_buffer.append("vt ");
				return (appendFloat(value.x)
					&& appendCharacter(' ')
					&& appendFlippedV(value.y)
					&& finishLine());
			}

			bool writeNormal(const Float3& value)
			{
				m_buffer.append("vn ");
				return (appendFloat(value.x)
					&& appendCharacter(' ')
					&& appendFloat(value.y)
					&& appendCharacter(' ')
					&& appendNegatedFloat(value.z)
					&& finishLine());
			}

			bool writeFace(const TriangleIndex32& triangle)
			{
				m_buffer.append("f ");
				return (appendFaceVertex(triangle.i0)
					&& appendCharacter(' ')
					&& appendFaceVertex(triangle.i2)
					&& appendCharacter(' ')
					&& appendFaceVertex(triangle.i1)
					&& finishLine());
			}

			bool flush()
			{
				if (m_buffer.empty())
				{
					return true;
				}

				const int64 sizeBytes = static_cast<int64>(m_buffer.size());

				if (m_writer.write(m_buffer.data(), sizeBytes) != sizeBytes)
				{
					return false;
				}

				m_buffer.clear();
				return true;
			}

		private:

			IWriter& m_writer;

			std::string m_buffer;

			bool appendFloat(const float value)
			{
				std::array<char, 64> buffer;
				const auto [end, error] = std::to_chars(
					buffer.data(),
					(buffer.data() + buffer.size()),
					value,
					std::chars_format::general,
					std::numeric_limits<float>::max_digits10);

				if (error != std::errc{})
				{
					return false;
				}

				m_buffer.append(buffer.data(), end);
				return true;
			}

			bool appendNegatedFloat(const float value)
			{
				return appendFloat((value == 0.0f) ? 0.0f : -value);
			}

			bool appendFlippedV(const float value)
			{
				const double flippedV = (1.0 - static_cast<double>(value));
				std::array<char, 64> buffer;
				const auto [end, error] = std::to_chars(
					buffer.data(),
					(buffer.data() + buffer.size()),
					flippedV,
					std::chars_format::general,
					std::numeric_limits<float>::max_digits10);

				if (error != std::errc{})
				{
					return false;
				}

				m_buffer.append(buffer.data(), end);
				return true;
			}

			bool appendIndex(const uint64 index)
			{
				std::array<char, 32> buffer;
				const auto [end, error] = std::to_chars(
					buffer.data(),
					(buffer.data() + buffer.size()),
					index);

				if (error != std::errc{})
				{
					return false;
				}

				m_buffer.append(buffer.data(), end);
				return true;
			}

			bool appendCharacter(const char ch)
			{
				m_buffer.push_back(ch);
				return true;
			}

			bool appendFaceVertex(const uint32 index)
			{
				const uint64 objIndex = (static_cast<uint64>(index) + 1);

				return (appendIndex(objIndex)
					&& appendCharacter('/')
					&& appendIndex(objIndex)
					&& appendCharacter('/')
					&& appendIndex(objIndex));
			}

			bool finishLine()
			{
				m_buffer.push_back('\n');

				if (OBJBufferFlushThreshold <= m_buffer.size())
				{
					return flush();
				}

				return true;
			}
		};

		bool WriteOBJVertices(const Mesh3D& mesh, OBJTextWriter& objWriter)
		{
			for (const auto& vertex : mesh.vertices)
			{
				if (not objWriter.writePosition(vertex.pos))
				{
					return false;
				}
			}

			for (const auto& vertex : mesh.vertices)
			{
				if (not objWriter.writeTexCoord(vertex.tex))
				{
					return false;
				}
			}

			for (const auto& vertex : mesh.vertices)
			{
				if (not objWriter.writeNormal(vertex.normal))
				{
					return false;
				}
			}
			return true;
		}

		bool EncodeValidatedOBJ(
			const Mesh3D& mesh,
			IWriter& writer,
			const std::string_view mtlFileName = {},
			const std::string_view materialName = {})
		{
			try
			{
				OBJTextWriter objWriter{ writer };

				if ((not mtlFileName.empty())
					&& (not objWriter.writeMTLLibrary(mtlFileName)))
				{
					return false;
				}

				if (not WriteOBJVertices(mesh, objWriter))
				{
					return false;
				}

				if ((not materialName.empty())
					&& (not objWriter.writeUseMaterial(materialName)))
				{
					return false;
				}

				for (const auto& triangle : mesh.indices)
				{
					if (not objWriter.writeFace(triangle))
					{
						return false;
					}
				}

				return objWriter.flush();
			}
			catch (const std::bad_alloc&)
			{
				return false;
			}
		}

		bool AppendDouble(std::string& output, const double value)
		{
			std::array<char, 64> buffer;
			const auto [end, error] = std::to_chars(
				buffer.data(),
				(buffer.data() + buffer.size()),
				value,
				std::chars_format::general);

			if (error != std::errc{})
			{
				return false;
			}

			output.append(buffer.data(), end);
			return true;
		}

		bool AppendScalarLine(std::string& output, const std::string_view key, const double value)
		{
			output.append(key);
			output.push_back(' ');

			if (not AppendDouble(output, value))
			{
				return false;
			}

			output.push_back('\n');
			return true;
		}

		bool AppendColorLine(
			std::string& output,
			const std::string_view key,
			const double r,
			const double g,
			const double b)
		{
			output.append(key);
			output.push_back(' ');

			if (not AppendDouble(output, r))
			{
				return false;
			}

			output.push_back(' ');

			if (not AppendDouble(output, g))
			{
				return false;
			}

			output.push_back(' ');

			if (not AppendDouble(output, b))
			{
				return false;
			}

			output.push_back('\n');
			return true;
		}

		void AppendTextureLine(
			std::string& output,
			const std::string_view key,
			const Optional<MaterialTexture>& texture)
		{
			if (not texture)
			{
				return;
			}

			output.append(key);
			output.push_back(' ');
			output.append(Unicode::ToUTF8(texture->path));
			output.push_back('\n');
		}

		bool EncodeValidatedMTL(const Material& material, IWriter& writer, const std::string_view exportedName = {})
		{
			try
			{
				const double metallic = std::clamp(material.metallic, 0.0, 1.0);
				const double roughness = std::clamp(material.roughness, 0.0, 1.0);
				const double diffuseFactor = (1.0 - metallic);
				const double specularFactor = (1.0 - metallic);
				const double specularR = (0.04 * specularFactor + material.baseColor.r * metallic);
				const double specularG = (0.04 * specularFactor + material.baseColor.g * metallic);
				const double specularB = (0.04 * specularFactor + material.baseColor.b * metallic);
				const double specularExponent = (roughness == 0.0)
					? 1000.0
					: std::clamp((2.0 / (roughness * roughness) - 2.0), 0.0, 1000.0);
				const double alpha = (material.alphaMode == MaterialAlphaMode::Opaque)
					? 1.0
					: std::clamp(material.baseColor.a, 0.0, 1.0);

				std::string output;
				output.reserve(512);
				output.append("newmtl ");
				output.append(exportedName.empty() ? Unicode::ToUTF8(material.name) : exportedName);
				output.push_back('\n');

				if ((not AppendColorLine(output, "Kd",
						(material.baseColor.r * diffuseFactor),
						(material.baseColor.g * diffuseFactor),
						(material.baseColor.b * diffuseFactor)))
					|| (not AppendColorLine(output, "Ks", specularR, specularG, specularB))
					|| (not AppendColorLine(output, "Ke", material.emissive.r, material.emissive.g, material.emissive.b))
					|| (not AppendScalarLine(output, "Ns", specularExponent))
					|| (not AppendScalarLine(output, "d", alpha))
					|| (not AppendScalarLine(output, "Pm", metallic))
					|| (not AppendScalarLine(output, "Pr", roughness)))
				{
					return false;
				}

				output.append("illum 2\n");
				AppendTextureLine(output, "map_Kd", material.baseColorTexture);
				AppendTextureLine(output, "norm", material.normalTexture);
				AppendTextureLine(output, "map_Ke", material.emissiveTexture);

				const int64 sizeBytes = static_cast<int64>(output.size());
				return (writer.write(output.data(), sizeBytes) == sizeBytes);
			}
			catch (const std::bad_alloc&)
			{
				return false;
			}
		}

		// Identifiers use ASCII tokens; percent escaping is our reversible naming
		// convention, not an OBJ escape syntax. ID prefixes prevent collisions.
		bool IsNameByte(const unsigned char ch) noexcept
		{
			return (('a' <= ch && ch <= 'z') || ('A' <= ch && ch <= 'Z')
				|| ('0' <= ch && ch <= '9') || ch == '_' || ch == '-' || ch == '.');
		}

		std::string EscapeOBJName(const StringView name)
		{
			const std::string utf8 = Unicode::ToUTF8(name);
			constexpr char Hex[] = "0123456789ABCDEF";
			std::string result;
			result.reserve(utf8.size());
			for (const unsigned char ch : utf8)
			{
				if (IsNameByte(ch))
				{
					result.push_back(static_cast<char>(ch));
				}
				else
				{
					result.push_back('%');
					result.push_back(Hex[ch >> 4]);
					result.push_back(Hex[ch & 15]);
				}
			}
			return result;
		}

		std::string ExportedName(const char* prefix, const size_t id, const StringView name)
		{
			std::string result = (prefix + std::to_string(id));
			if (not name.isEmpty())
			{
				result += '_';
				result += EscapeOBJName(name);
			}
			return result;
		}

		bool IsMTLFileName(const StringView name) noexcept
		{
			if (name.isEmpty() || name == U"." || name == U"..")
			{
				return false;
			}
			for (const char32 ch : name)
			{
				if ((127 < ch) || ((ch != U'%') && (not IsNameByte(static_cast<unsigned char>(ch)))))
				{
					return false;
				}
			}
			return true;
		}

		struct AssemblyOBJNames
		{
			Array<std::string> parts;
			Array<std::string> materials;
			bool defaultMaterial = false;
		};

		bool PrepareAssemblyOBJ(const Mesh3DAssembly::BakedMesh& baked, AssemblyOBJNames& names)
		{
			if (not ValidateForOBJ(baked.mesh))
			{
				return false;
			}
			names.parts.resize(baked.parts.size());
			names.materials.resize(baked.materials.size());
			size_t vertices = 0;
			size_t triangles = 0;
			for (size_t i = 0; i < baked.parts.size(); ++i)
			{
				const auto& part = baked.parts[i];
				const auto& range = part.range;
				if ((static_cast<size_t>(part.id) != i)
					|| (range.vertexOffset != vertices) || (range.triangleOffset != triangles)
					|| ((baked.mesh.vertexCount() - vertices) < range.vertexCount)
					|| ((baked.mesh.triangleCount() - triangles) < range.triangleCount))
				{
					return false;
				}
				vertices += range.vertexCount;
				triangles += range.triangleCount;
				for (size_t t = range.triangleOffset; t < triangles; ++t)
				{
					const auto& tri = baked.mesh.indices[t];
					if ((tri.i0 < range.vertexOffset) || (vertices <= tri.i0)
						|| (tri.i1 < range.vertexOffset) || (vertices <= tri.i1)
						|| (tri.i2 < range.vertexOffset) || (vertices <= tri.i2))
					{
						return false;
					}
				}
				if (range.triangleCount == 0)
				{
					continue;
				}
				names.parts[i] = ExportedName("part_", i, part.name);
				if (part.material)
				{
					const size_t id = static_cast<size_t>(*part.material);
					if (baked.materials.size() <= id)
					{
						return false;
					}
					if (names.materials[id].empty())
					{
						const auto& material = baked.materials[id];
						if (not ValidateForMTL(material, false))
						{
							return false;
						}
						names.materials[id] = ExportedName("material_", id, material.name);
					}
				}
				else
				{
					names.defaultMaterial = true;
				}
			}
			return ((vertices == baked.mesh.vertexCount()) && (triangles == baked.mesh.triangleCount()));
		}

		bool EncodeAssemblyOBJ(const Mesh3DAssembly::BakedMesh& baked, const AssemblyOBJNames& names,
			IWriter& objOutput, IWriter& mtlOutput, const std::string_view mtlFileName)
		{
			for (size_t i = 0; i < baked.materials.size(); ++i)
			{
				if ((not names.materials[i].empty())
					&& (not EncodeValidatedMTL(baked.materials[i], mtlOutput, names.materials[i])))
				{
					return false;
				}
			}
			if (names.defaultMaterial && (not EncodeValidatedMTL(Material{}, mtlOutput, "material_default")))
			{
				return false;
			}
			OBJTextWriter writer{ objOutput };
			if ((not writer.writeMTLLibrary(mtlFileName)) || (not WriteOBJVertices(baked.mesh, writer)))
			{
				return false;
			}
			for (size_t i = 0; i < baked.parts.size(); ++i)
			{
				const auto& part = baked.parts[i];
				if (part.range.triangleCount == 0)
				{
					continue;
				}
				const std::string_view material = (part.material
					? std::string_view{ names.materials[static_cast<size_t>(*part.material)] } : "material_default");
				if ((not writer.writeGroup(names.parts[i])) || (not writer.writeUseMaterial(material)))
				{
					return false;
				}
				const size_t end = (part.range.triangleOffset + part.range.triangleCount);
				for (size_t t = part.range.triangleOffset; t < end; ++t)
				{
					if (not writer.writeFace(baked.mesh.indices[t]))
					{
						return false;
					}
				}
			}
			return writer.flush();
		}

	}

	////////////////////////////////////////////////////////////////
	//
	//	saveOBJ
	//
	////////////////////////////////////////////////////////////////

	bool Mesh3D::saveOBJ(const FilePathView path) const
	{
		if (not ValidateForOBJ(*this))
		{
			return GenerationFailed<bool>("Mesh3D::saveOBJ(): The mesh is empty, invalid, or contains non-finite OBJ vertex attributes");
		}

		BinaryFileWriter writer{ path };

		if (not writer)
		{
			return GenerationFailed<bool>("Mesh3D::saveOBJ(): Failed to open the OBJ file");
		}

		if (not EncodeValidatedOBJ(*this, writer))
		{
			return GenerationFailed<bool>("Mesh3D::saveOBJ(): Failed to encode or write the OBJ data");
		}

		return true;
	}

	bool Mesh3D::saveOBJ(const FilePathView path, const Material& material) const
	{
		if (not ValidateForOBJ(*this))
		{
			return GenerationFailed<bool>("Mesh3D::saveOBJ(): The mesh is empty, invalid, or contains non-finite OBJ vertex attributes");
		}

		if (not ValidateForMTL(material))
		{
			return GenerationFailed<bool>("Mesh3D::saveOBJ(): The material is invalid or cannot be represented in MTL");
		}

		const String baseName = FileSystem::BaseName(path);

		if (baseName.isEmpty()
			|| (FileSystem::Extension(path) == U"mtl"))
		{
			return GenerationFailed<bool>("Mesh3D::saveOBJ(): The path must have a non-empty base name and must not use the .mtl extension");
		}

		FilePath objFullPath;
		const FilePath parentPath = FileSystem::ParentPath(path, 0, objFullPath);
		const String mtlFileName = (baseName + U".mtl");
		const FilePath mtlPath = FileSystem::PathAppend(parentPath, mtlFileName);

		if ((not IsSingleLine(mtlFileName))
			|| (objFullPath == mtlPath))
		{
			return GenerationFailed<bool>("Mesh3D::saveOBJ(): The derived MTL path or file name is invalid");
		}

		BinaryFileWriter objWriter{ path };
		if (not objWriter)
		{
			return GenerationFailed<bool>("Mesh3D::saveOBJ(): Failed to open the OBJ file");
		}
		BinaryFileWriter mtlWriter{ mtlPath };
		if (not mtlWriter)
		{
			return GenerationFailed<bool>("Mesh3D::saveOBJ(): Failed to open the OBJ or MTL file");
		}

		const std::string mtlFileNameUTF8 = Unicode::ToUTF8(mtlFileName);
		const std::string materialNameUTF8 = Unicode::ToUTF8(material.name);

		if (not EncodeValidatedMTL(material, mtlWriter))
		{
			return GenerationFailed<bool>("Mesh3D::saveOBJ(): Failed to encode or write the MTL data");
		}

		if (not EncodeValidatedOBJ(*this, objWriter, mtlFileNameUTF8, materialNameUTF8))
		{
			return GenerationFailed<bool>("Mesh3D::saveOBJ(): Failed to encode or write the OBJ data");
		}

		return true;
	}

	////////////////////////////////////////////////////////////////
	//
	//	encodeOBJ
	//
	////////////////////////////////////////////////////////////////

	bool Mesh3D::encodeOBJ(IWriter& writer) const
	{
		if (not writer.isOpen())
		{
			return GenerationFailed<bool>("Mesh3D::encodeOBJ(): writer must be open");
		}

		if (not ValidateForOBJ(*this))
		{
			return GenerationFailed<bool>("Mesh3D::encodeOBJ(): The mesh is empty, invalid, or contains non-finite OBJ vertex attributes");
		}

		if (not EncodeValidatedOBJ(*this, writer))
		{
			return GenerationFailed<bool>("Mesh3D::encodeOBJ(): Failed to encode or write the OBJ data");
		}

		return true;
	}

	Blob Mesh3D::encodeOBJ() const
	{
		MemoryWriter writer;
		if (not encodeOBJ(writer))
		{
			return{};
		}
		return writer.extractBlob();
	}

	bool Mesh3DAssembly::BakedMesh::encodeOBJ(IWriter& objWriter, IWriter& mtlWriter, const StringView mtlFileName) const
	{
		try
		{
			AssemblyOBJNames names;
			if ((&objWriter == &mtlWriter) || (not objWriter.isOpen()) || (not mtlWriter.isOpen())
				|| (not IsMTLFileName(mtlFileName)) || (not PrepareAssemblyOBJ(*this, names)))
			{
				return GenerationFailed<bool>("Mesh3DAssembly::encodeOBJ(): Invalid writers, MTL filename, mesh, part ranges, or material");
			}
			if (not EncodeAssemblyOBJ(*this, names, objWriter, mtlWriter, Unicode::ToUTF8(mtlFileName)))
			{
				return GenerationFailed<bool>("Mesh3DAssembly::encodeOBJ(): Failed to write OBJ or MTL data");
			}
			return true;
		}
		catch (const std::bad_alloc&)
		{
			return GenerationFailed<bool>("Mesh3DAssembly::encodeOBJ(): Allocation failed");
		}
	}

	bool Mesh3DAssembly::BakedMesh::saveOBJ(const FilePathView path) const
	{
		try
		{
			AssemblyOBJNames names;
			const String baseName = FileSystem::BaseName(path);
			if (baseName.isEmpty() || (FileSystem::Extension(path) == U"mtl") || (not PrepareAssemblyOBJ(*this, names)))
			{
				return GenerationFailed<bool>("Mesh3DAssembly::saveOBJ(): Invalid path, mesh, part ranges, or material");
			}
			const std::string mtlFileName = (EscapeOBJName(baseName) + ".mtl");
			FilePath objFullPath;
			const FilePath parent = FileSystem::ParentPath(path, 0, objFullPath);
			const FilePath mtlPath = FileSystem::PathAppend(parent, Unicode::FromUTF8(mtlFileName));
			if (objFullPath == mtlPath)
			{
				return GenerationFailed<bool>("Mesh3DAssembly::saveOBJ(): OBJ and MTL paths must differ");
			}
			BinaryFileWriter objWriter{ path };
			if (not objWriter)
			{
				return GenerationFailed<bool>("Mesh3DAssembly::saveOBJ(): Failed to open OBJ file");
			}
			BinaryFileWriter mtlWriter{ mtlPath };
			if ((not mtlWriter)
				|| (not EncodeAssemblyOBJ(*this, names, objWriter, mtlWriter, mtlFileName)))
			{
				return GenerationFailed<bool>("Mesh3DAssembly::saveOBJ(): Failed to open or write OBJ / MTL files");
			}
			return true;
		}
		catch (const std::bad_alloc&)
		{
			return GenerationFailed<bool>("Mesh3DAssembly::saveOBJ(): Allocation failed");
		}
	}

	bool Mesh3DAssembly::saveOBJ(const FilePathView path, const Mesh3DBakeOptions& options) const
	{
		const auto baked = bake(options);
		return (baked && baked->saveOBJ(path));
	}
}
