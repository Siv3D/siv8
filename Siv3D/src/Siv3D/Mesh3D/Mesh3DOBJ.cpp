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

# include <array>
# include <charconv>
# include <cmath>
# include <limits>
# include <string>
# include <Siv3D/Mesh3D.hpp>
# include <Siv3D/BinaryFileWriter.hpp>
# include <Siv3D/MemoryWriter.hpp>

namespace s3d
{
	namespace
	{
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
					&& appendFloat(value.z)
					&& finishLine());
			}

			bool writeTexCoord(const Float2& value)
			{
				m_buffer.append("vt ");
				return (appendFloat(value.x)
					&& appendCharacter(' ')
					&& appendFloat(value.y)
					&& finishLine());
			}

			bool writeNormal(const Float3& value)
			{
				m_buffer.append("vn ");
				return (appendFloat(value.x)
					&& appendCharacter(' ')
					&& appendFloat(value.y)
					&& appendCharacter(' ')
					&& appendFloat(value.z)
					&& finishLine());
			}

			bool writeFace(const TriangleIndex32& triangle)
			{
				m_buffer.append("f ");
				return (appendFaceVertex(triangle.i0)
					&& appendCharacter(' ')
					&& appendFaceVertex(triangle.i1)
					&& appendCharacter(' ')
					&& appendFaceVertex(triangle.i2)
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

		bool EncodeValidatedOBJ(const Mesh3D& mesh, IWriter& writer)
		{
			try
			{
				OBJTextWriter objWriter{ writer };

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

		bool EncodeOBJ(const Mesh3D& mesh, IWriter& writer)
		{
			if ((not writer.isOpen()) || (not ValidateForOBJ(mesh)))
			{
				return false;
			}

			return EncodeValidatedOBJ(mesh, writer);
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
			return false;
		}

		BinaryFileWriter writer{ path };

		if (not writer)
		{
			return false;
		}

		return EncodeValidatedOBJ(*this, writer);
	}

	////////////////////////////////////////////////////////////////
	//
	//	encodeOBJ
	//
	////////////////////////////////////////////////////////////////

	bool Mesh3D::encodeOBJ(IWriter& writer) const
	{
		return EncodeOBJ(*this, writer);
	}

	Blob Mesh3D::encodeOBJ() const
	{
		MemoryWriter writer;

		if (not EncodeOBJ(*this, writer))
		{
			return{};
		}

		return writer.extractBlob();
	}
}
