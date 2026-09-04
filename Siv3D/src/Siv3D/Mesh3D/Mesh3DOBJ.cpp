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
		bool IsFinite(const Float3& value) noexcept
		{
			return (std::isfinite(value.x)
				&& std::isfinite(value.y)
				&& std::isfinite(value.z));
		}

		[[nodiscard]]
		bool IsFinite(const Float2& value) noexcept
		{
			return (std::isfinite(value.x)
				&& std::isfinite(value.y));
		}

		[[nodiscard]]
		bool IsFinite(const ColorF& value) noexcept
		{
			return (std::isfinite(value.r)
				&& std::isfinite(value.g)
				&& std::isfinite(value.b)
				&& std::isfinite(value.a));
		}

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
		bool ValidateForMTL(const Material& material) noexcept
		{
			if (material.name.isEmpty() || (not IsSingleLine(material.name))
				|| (not IsFinite(material.baseColor))
				|| (not std::isfinite(material.metallic))
				|| (not std::isfinite(material.roughness))
				|| (not IsFinite(material.emissive))
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
				if ((not IsFinite(vertex.pos))
					|| (not IsFinite(vertex.tex))
					|| (not IsFinite(vertex.normal)))
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

		bool EncodeValidatedMTL(const Material& material, IWriter& writer)
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
				output.append(Unicode::ToUTF8(material.name));
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
		BinaryFileWriter mtlWriter{ mtlPath };

		if ((not objWriter) || (not mtlWriter))
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
		if (not ValidateForOBJ(*this))
		{
			return GenerationFailed<Blob>("Mesh3D::encodeOBJ(): The mesh is empty, invalid, or contains non-finite OBJ vertex attributes");
		}

		MemoryWriter writer;

		if (not EncodeValidatedOBJ(*this, writer))
		{
			return GenerationFailed<Blob>("Mesh3D::encodeOBJ(): Failed to encode the OBJ data");
		}

		return writer.extractBlob();
	}
}
