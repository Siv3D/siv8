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

# include <Siv3D/ImageFormat/GIFDecoder.hpp>
# include <Siv3D/IReader.hpp>
# include "../../../ThirdParty/wuffs/WuffsCodecs.hpp"

namespace s3d
{
	namespace
	{
		class GIFLookAheadStream
		{
		public:

			explicit GIFLookAheadStream(const IReader& reader)
				: m_reader{ reader }
				, m_pos{ reader.getPos() }
				, m_endPos{ reader.size() }
				, m_bufferStartPos{ m_pos } {}

			[[nodiscard]]
			bool read(uint8* dst, size_t size)
			{
				if ((m_pos < 0) || (m_endPos < m_pos))
				{
					return false;
				}

				while (size != 0)
				{
					if ((m_pos < m_bufferStartPos)
						|| ((m_bufferStartPos + static_cast<int64>(m_bufferSize)) <= m_pos))
					{
						if (not refill())
						{
							return false;
						}
					}

					const size_t bufferOffset = static_cast<size_t>(m_pos - m_bufferStartPos);
					const size_t count = std::min((m_bufferSize - bufferOffset), size);
					std::memcpy(dst, (m_buffer.data() + bufferOffset), count);
					dst += count;
					size -= count;
					m_pos += static_cast<int64>(count);
				}

				return true;
			}

			[[nodiscard]]
			bool skip(const size_t size) noexcept
			{
				if ((m_pos < 0) || (m_endPos < m_pos)
					|| (static_cast<uint64>(m_endPos - m_pos) < size))
				{
					return false;
				}

				m_pos += static_cast<int64>(size);
				return true;
			}

		private:

			[[nodiscard]]
			bool refill()
			{
				if ((m_pos < 0) || (m_endPos <= m_pos))
				{
					return false;
				}

				const int64 requestSize = std::min<int64>(
					static_cast<int64>(m_buffer.size()),
					(m_endPos - m_pos));
				const int64 readSize = m_reader.lookahead(
					m_buffer.data(),
					m_pos,
					requestSize);

				if ((readSize <= 0) || (requestSize < readSize))
				{
					return false;
				}

				m_bufferStartPos = m_pos;
				m_bufferSize = static_cast<size_t>(readSize);
				return true;
			}

			static constexpr size_t BufferSize = 4096;

			const IReader& m_reader;

			std::array<uint8, BufferSize> m_buffer{};

			int64 m_pos = 0;

			int64 m_endPos = 0;

			int64 m_bufferStartPos = 0;

			size_t m_bufferSize = 0;
		};

		[[nodiscard]]
		static bool IsGIFSignature(const uint8* const bytes) noexcept
		{
			return ((std::memcmp(bytes, "GIF87a", 6) == 0)
				|| (std::memcmp(bytes, "GIF89a", 6) == 0));
		}

		[[nodiscard]]
		static bool SkipGIFSubBlocks(GIFLookAheadStream& stream)
		{
			for (;;)
			{
				uint8 blockSize = 0;

				if (not stream.read(&blockSize, 1))
				{
					return false;
				}

				if (blockSize == 0)
				{
					return true;
				}

				if (not stream.skip(blockSize))
				{
					return false;
				}
			}
		}

		[[nodiscard]]
		static bool HasMultipleGIFFrames(const IReader& reader)
		{
			GIFLookAheadStream stream{ reader };
			std::array<uint8, 13> header;

			if ((not stream.read(header.data(), header.size()))
				|| (not IsGIFSignature(header.data())))
			{
				return false;
			}

			const uint8 logicalScreenPackedFields = header[10];

			if (logicalScreenPackedFields & 0x80)
			{
				const size_t colorTableSize =
					(3u << ((logicalScreenPackedFields & 0x07) + 1));

				if (not stream.skip(colorTableSize))
				{
					return false;
				}
			}

			size_t frameCount = 0;

			for (;;)
			{
				uint8 blockType = 0;

				if (not stream.read(&blockType, 1))
				{
					return false;
				}

				if (blockType == 0x2C)
				{
					std::array<uint8, 9> descriptor;

					if (not stream.read(descriptor.data(), descriptor.size()))
					{
						return false;
					}

					const uint32 frameWidth = (static_cast<uint32>(descriptor[4])
						| (static_cast<uint32>(descriptor[5]) << 8));
					const uint32 frameHeight = (static_cast<uint32>(descriptor[6])
						| (static_cast<uint32>(descriptor[7]) << 8));

					if ((frameWidth == 0) || (frameHeight == 0))
					{
						return false;
					}

					++frameCount;

					if (1 < frameCount)
					{
						return true;
					}

					const uint8 imagePackedFields = descriptor[8];

					if (imagePackedFields & 0x80)
					{
						const size_t colorTableSize =
							(3u << ((imagePackedFields & 0x07) + 1));

						if (not stream.skip(colorTableSize))
						{
							return false;
						}
					}

					uint8 minimumCodeSize = 0;

					if ((not stream.read(&minimumCodeSize, 1))
						|| (not SkipGIFSubBlocks(stream)))
					{
						return false;
					}
				}
				else if (blockType == 0x21)
				{
					uint8 extensionLabel = 0;

					if ((not stream.read(&extensionLabel, 1))
						|| (not SkipGIFSubBlocks(stream)))
					{
						return false;
					}
				}
				else if (blockType == 0x3B)
				{
					return false;
				}
				else if (blockType != 0)
				{
					return false;
				}
			}
		}
	}

	StringView GIFDecoder::name() const noexcept
	{
		return U"GIF"_sv;
	}

	ImageFormat GIFDecoder::imageFormat() const noexcept
	{
		return ImageFormat::GIF;
	}

	bool GIFDecoder::isHeader(const uint8(&bytes)[RequiredHeaderBytes]) const noexcept
	{
		return IsGIFSignature(bytes);
	}

	std::span<const StringView> GIFDecoder::possibleExtensions() const noexcept
	{
		static constexpr std::array<StringView, 1> Extensions = { U"gif" };
		return Extensions;
	}

	Optional<ImageInfo> GIFDecoder::getImageInfo(const FilePathView path) const
	{
		return IImageDecoder::getImageInfo(path);
	}

	Optional<ImageInfo> GIFDecoder::getImageInfo(const IReader& reader, const FilePathView) const
	{
		uint8 header[10];

		if ((reader.lookahead(header, sizeof(header)) != sizeof(header))
			|| (not IsGIFSignature(header)))
		{
			return{};
		}

		const uint32 width = (static_cast<uint32>(header[6])
			| (static_cast<uint32>(header[7]) << 8));
		const uint32 height = (static_cast<uint32>(header[8])
			| (static_cast<uint32>(header[9]) << 8));

		if ((width == 0) || (height == 0)
			|| (Image::MaxWidth < width) || (Image::MaxHeight < height))
		{
			return{};
		}

		return ImageInfo{
			Size{ static_cast<int32>(width), static_cast<int32>(height) },
			ImageFormat::GIF,
			ImagePixelFormat::R8G8B8A8,
			HasMultipleGIFFrames(reader)
		};
	}

	Image GIFDecoder::decode(const FilePathView path, const PremultiplyAlpha premultiplyAlpha) const
	{
		return IImageDecoder::decode(path, premultiplyAlpha);
	}

	Image GIFDecoder::decode(std::unique_ptr<IReader> reader, const FilePathView, const PremultiplyAlpha premultiplyAlpha) const
	{
		return detail::DecodeFirstGIFFrame(std::move(reader), premultiplyAlpha);
	}
}
