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

# include <Siv3D/Platform.hpp>

# define WUFFS_IMPLEMENTATION

// Wuffs の全利用をこの translation unit 内に閉じ込める
# define WUFFS_CONFIG__STATIC_FUNCTIONS

// 必要なモジュールのみ
# define WUFFS_CONFIG__MODULES

# define WUFFS_CONFIG__MODULE__BASE
# define WUFFS_CONFIG__MODULE__ADLER32
# define WUFFS_CONFIG__MODULE__CRC32
# define WUFFS_CONFIG__MODULE__DEFLATE
# define WUFFS_CONFIG__MODULE__ZLIB
# define WUFFS_CONFIG__MODULE__GIF
# define WUFFS_CONFIG__MODULE__PNG

// GZIP は実装開始時に追加する
// # define WUFFS_CONFIG__MODULE__GZIP

// アニメーション合成用キャンバスは常に straight alpha の RGBA8
# define WUFFS_CONFIG__DST_PIXEL_FORMAT__ENABLE_ALLOWLIST
# define WUFFS_CONFIG__DST_PIXEL_FORMAT__ALLOW_RGBA_NONPREMUL

// Windows / cl.exe / x64。
// SSE4.2 + POPCNT + PCLMULQDQ の V2 パスを有効化。
// AVX2 の V3 パスは有効化しない。
# if defined(_MSC_VER) && !defined(__clang__) && defined(_M_X64)
#	define WUFFS_CONFIG__ENABLE_MSVC_CPU_ARCH__X86_64_V2
# endif

SIV3D_DISABLE_MSVC_WARNINGS_PUSH(4100)
SIV3D_DISABLE_MSVC_WARNINGS_PUSH(4505)
#include "wuffs-v0.4.h"
SIV3D_DISABLE_MSVC_WARNINGS_POP()
SIV3D_DISABLE_MSVC_WARNINGS_POP()

# include "WuffsCodecs.hpp"
# include <Siv3D/AnimatedImage.hpp>
# include <Siv3D/Array.hpp>
# include <Siv3D/BinaryFileReader.hpp>
# include <Siv3D/IReader.hpp>
# include <algorithm>
# include <array>
# include <limits>

namespace s3d
{
	namespace
	{
		enum class WuffsImageFormat
		{
			GIF,
			APNG,
		};

		inline constexpr size_t InputBufferSize = (64 * 1024);

		[[nodiscard]]
		static uint32 ReadBigEndianUint32(const uint8* const p) noexcept
		{
			return ((static_cast<uint32>(p[0]) << 24)
				| (static_cast<uint32>(p[1]) << 16)
				| (static_cast<uint32>(p[2]) << 8)
				| static_cast<uint32>(p[3]));
		}

		class PNGProbe
		{
		public:

			void update(const uint8* data, size_t size) noexcept
			{
				while ((size != 0) && (m_error == AnimatedImageDecodeError::None)
					&& (not m_isAnimated) && (not m_isStatic))
				{
					if (m_signatureIndex < m_signature.size())
					{
						static constexpr std::array<uint8, 8> PNGSignature =
							{ 0x89, 'P', 'N', 'G', 0x0D, 0x0A, 0x1A, 0x0A };

						if (*data != PNGSignature[m_signatureIndex])
						{
							m_error = AnimatedImageDecodeError::InvalidFormat;
							return;
						}

						++m_signatureIndex;
						++data;
						--size;
						continue;
					}

					if (m_chunkHeaderIndex < m_chunkHeader.size())
					{
						m_chunkHeader[m_chunkHeaderIndex++] = *data++;
						--size;

						if (m_chunkHeaderIndex == m_chunkHeader.size())
						{
							beginChunk();
						}

						continue;
					}

					if (m_chunkDataRemaining != 0)
					{
						const size_t count = static_cast<size_t>(
							std::min<uint64>(m_chunkDataRemaining, size));

						if (m_isIHDR && (m_chunkDataIndex <= 12)
							&& (12 < (m_chunkDataIndex + count)))
						{
							const uint8 interlaceMethod = data[12 - m_chunkDataIndex];

							if (1 < interlaceMethod)
							{
								m_error = AnimatedImageDecodeError::InvalidData;
								return;
							}

							m_isInterlaced = (interlaceMethod == 1);
						}

						m_chunkDataIndex += count;
						m_chunkDataRemaining -= count;
						data += count;
						size -= count;
						continue;
					}

					const size_t count = std::min(m_crcRemaining, size);
					m_crcRemaining -= count;
					data += count;
					size -= count;

					if (m_crcRemaining == 0)
					{
						m_chunkHeaderIndex = 0;
						m_chunkDataIndex = 0;
						m_isIHDR = false;
					}
				}
			}

			[[nodiscard]]
			AnimatedImageDecodeError result() const noexcept
			{
				if (m_error != AnimatedImageDecodeError::None)
				{
					return m_error;
				}

				if (m_isAnimated)
				{
					return (m_isInterlaced
						? AnimatedImageDecodeError::UnsupportedFeature
						: AnimatedImageDecodeError::None);
				}

				if (m_isStatic)
				{
					return AnimatedImageDecodeError::NotAnimated;
				}

				return AnimatedImageDecodeError::TruncatedInput;
			}

		private:

			void beginChunk() noexcept
			{
				const uint32 chunkLength = ReadBigEndianUint32(m_chunkHeader.data());
				const uint8* const chunkType = (m_chunkHeader.data() + 4);
				const bool isFirstChunk = (m_chunkCount == 0);
				m_isIHDR = (std::memcmp(chunkType, "IHDR", 4) == 0);

				if ((isFirstChunk && ((not m_isIHDR) || (chunkLength != 13)))
					|| ((not isFirstChunk) && m_isIHDR))
				{
					m_error = AnimatedImageDecodeError::InvalidData;
					return;
				}

				++m_chunkCount;

				if (std::memcmp(chunkType, "acTL", 4) == 0)
				{
					if ((chunkLength != 8) || m_seenIDAT)
					{
						m_error = AnimatedImageDecodeError::InvalidData;
						return;
					}

					m_isAnimated = true;
				}
				else if (std::memcmp(chunkType, "IDAT", 4) == 0)
				{
					m_seenIDAT = true;

					if (not m_isAnimated)
					{
						m_isStatic = true;
					}
				}

				m_chunkDataRemaining = chunkLength;
				m_crcRemaining = 4;
			}

			std::array<uint8, 8> m_signature{};

			std::array<uint8, 8> m_chunkHeader{};

			size_t m_signatureIndex = 0;

			size_t m_chunkHeaderIndex = 0;

			uint64 m_chunkDataRemaining = 0;

			size_t m_chunkDataIndex = 0;

			size_t m_crcRemaining = 0;

			size_t m_chunkCount = 0;

			AnimatedImageDecodeError m_error = AnimatedImageDecodeError::None;

			bool m_isIHDR = false;

			bool m_seenIDAT = false;

			bool m_isAnimated = false;

			bool m_isStatic = false;

			bool m_isInterlaced = false;
		};

		class WuffsInput
		{
		public:

			WuffsInput(std::unique_ptr<IReader> reader, const bool probePNG)
				: m_reader{ std::move(reader) }
				, m_buffer(InputBufferSize)
				, m_source{ wuffs_base__ptr_u8__writer(m_buffer.data(), m_buffer.size()) }
				, m_probePNG{ probePNG }
			{
				if ((not m_reader) || (not *m_reader))
				{
					m_error = AnimatedImageDecodeError::ReadError;
					return;
				}

				const int64 size = m_reader->size();
				const int64 pos = m_reader->getPos();

				if ((size < 0) || (pos < 0) || (size < pos))
				{
					m_error = AnimatedImageDecodeError::ReadError;
					return;
				}

				m_remaining = static_cast<uint64>(size - pos);
				m_source.meta.closed = (m_remaining == 0);
			}

			[[nodiscard]]
			bool prepareHeader(const size_t minimumBytes)
			{
				while ((m_source.reader_length() < minimumBytes)
					&& (not m_source.meta.closed))
				{
					if (not refill())
					{
						return false;
					}
				}

				return (minimumBytes <= m_source.reader_length());
			}

			[[nodiscard]]
			bool refill()
			{
				if (m_error != AnimatedImageDecodeError::None)
				{
					return false;
				}

				m_source.compact();

				if (m_remaining == 0)
				{
					m_source.meta.closed = true;
					return true;
				}

				const size_t requestSize = static_cast<size_t>(
					std::min<uint64>(m_source.writer_length(), m_remaining));

				if (requestSize == 0)
				{
					m_error = AnimatedImageDecodeError::InvalidData;
					return false;
				}

				const int64 readSize = m_reader->read(
					m_source.writer_pointer(),
					static_cast<int64>(requestSize));

				if ((readSize <= 0)
					|| (static_cast<uint64>(requestSize) < static_cast<uint64>(readSize)))
				{
					m_error = AnimatedImageDecodeError::ReadError;
					return false;
				}

				if (m_probePNG)
				{
					m_pngProbe.update(
						m_source.writer_pointer(),
						static_cast<size_t>(readSize));
				}

				m_source.meta.wi += static_cast<size_t>(readSize);
				m_remaining -= static_cast<uint64>(readSize);
				m_source.meta.closed = (m_remaining == 0);
				return true;
			}

			[[nodiscard]]
			wuffs_base__io_buffer& source() noexcept
			{
				return m_source;
			}

			[[nodiscard]]
			AnimatedImageDecodeError error() const noexcept
			{
				return m_error;
			}

			[[nodiscard]]
			AnimatedImageDecodeError pngProbeResult() const noexcept
			{
				return m_pngProbe.result();
			}

		private:

			std::unique_ptr<IReader> m_reader;

			Array<uint8> m_buffer;

			wuffs_base__io_buffer m_source = wuffs_base__empty_io_buffer();

			PNGProbe m_pngProbe;

			uint64 m_remaining = 0;

			AnimatedImageDecodeError m_error = AnimatedImageDecodeError::None;

			bool m_probePNG = false;
		};

		struct PumpResult
		{
			wuffs_base__status status = wuffs_base__make_status(nullptr);
			AnimatedImageDecodeError error = AnimatedImageDecodeError::None;
		};

		template <class Function>
		[[nodiscard]]
		static PumpResult Pump(WuffsInput& input, Function&& function)
		{
			for (;;)
			{
				const wuffs_base__status status = function(input.source());

				if (status.repr != wuffs_base__suspension__short_read)
				{
					return{ status, AnimatedImageDecodeError::None };
				}

				if (input.source().meta.closed)
				{
					return{ status, AnimatedImageDecodeError::None };
				}

				if (not input.refill())
				{
					return{ status, input.error() };
				}
			}
		}

		struct PreviousFrame
		{
			wuffs_base__rect_ie_u32 bounds = wuffs_base__empty_rect_ie_u32();
			wuffs_base__animation_disposal disposal = WUFFS_BASE__ANIMATION_DISPOSAL__NONE;
			wuffs_base__color_u32_argb_premul backgroundColor = 0;
			Array<Color> restorePixels;
		};

		[[nodiscard]]
		static AnimatedImageDecodeError ToDecodeError(
			const wuffs_base__status status,
			const WuffsImageFormat format) noexcept
		{
			if (status.repr == nullptr)
			{
				return AnimatedImageDecodeError::None;
			}

			if (((format == WuffsImageFormat::GIF)
					&& (status.repr == wuffs_gif__error__truncated_input))
				|| ((format == WuffsImageFormat::APNG)
					&& (status.repr == wuffs_png__error__truncated_input))
				|| (status.repr == wuffs_base__suspension__short_read))
			{
				return AnimatedImageDecodeError::TruncatedInput;
			}

			if ((status.repr == wuffs_base__error__unsupported_option)
				|| ((format == WuffsImageFormat::APNG)
					&& ((status.repr == wuffs_png__error__unsupported_png_compression_method)
						|| (status.repr == wuffs_png__error__unsupported_png_file)
						|| (status.repr == wuffs_png__error__unsupported_cgbi_extension))))
			{
				return AnimatedImageDecodeError::UnsupportedFeature;
			}

			return AnimatedImageDecodeError::InvalidData;
		}

		[[nodiscard]]
		static bool AddExceeds(const uint64 a, const uint64 b, const uint64 limit) noexcept
		{
			return ((limit < a) || ((limit - a) < b));
		}

		static void RestorePreviousFrame(
			Image& canvas,
			wuffs_base__pixel_buffer& pixelBuffer,
			const PreviousFrame& previous)
		{
			if (previous.disposal == WUFFS_BASE__ANIMATION_DISPOSAL__RESTORE_BACKGROUND)
			{
				pixelBuffer.set_color_u32_fill_rect(
					previous.bounds,
					previous.backgroundColor);
			}
			else if (previous.disposal == WUFFS_BASE__ANIMATION_DISPOSAL__RESTORE_PREVIOUS)
			{
				const uint32 width = previous.bounds.width();
				const uint32 height = previous.bounds.height();
				size_t sourceIndex = 0;

				for (uint32 y = 0; y < height; ++y)
				{
					Color* const dst = (canvas.data()
						+ (static_cast<size_t>(previous.bounds.min_incl_y + y) * canvas.width())
						+ previous.bounds.min_incl_x);
					std::memcpy(
						dst,
						(previous.restorePixels.data() + sourceIndex),
						(static_cast<size_t>(width) * sizeof(Color)));
					sourceIndex += width;
				}
			}
		}

		[[nodiscard]]
		static Array<Color> SaveFrameRectangle(
			const Image& canvas,
			const wuffs_base__rect_ie_u32 bounds)
		{
			const uint32 width = bounds.width();
			const uint32 height = bounds.height();
			Array<Color> result(static_cast<size_t>(width) * height);
			size_t destinationIndex = 0;

			for (uint32 y = 0; y < height; ++y)
			{
				const Color* const src = (canvas.data()
					+ (static_cast<size_t>(bounds.min_incl_y + y) * canvas.width())
					+ bounds.min_incl_x);
				std::memcpy(
					(result.data() + destinationIndex),
					src,
					(static_cast<size_t>(width) * sizeof(Color)));
				destinationIndex += width;
			}

			return result;
		}

		[[nodiscard]]
		static AnimatedImageDecodeResult DecodeWuffs(
			std::unique_ptr<IReader> reader,
			const WuffsImageFormat format,
			const AnimatedImageDecodeOptions& options,
			const bool firstFrameOnly)
		{
			AnimatedImageDecodeResult result;

			if (options.maxWorkingMemoryBytes < InputBufferSize)
			{
				result.error = AnimatedImageDecodeError::WorkingMemoryLimitExceeded;
				return result;
			}

			WuffsInput input{
				std::move(reader),
				(format == WuffsImageFormat::APNG)
			};

			if (input.error() != AnimatedImageDecodeError::None)
			{
				result.error = input.error();
				return result;
			}

			const size_t requiredHeaderBytes =
				(format == WuffsImageFormat::GIF) ? 6 : 8;

			if (not input.prepareHeader(requiredHeaderBytes))
			{
				result.error = (input.error() == AnimatedImageDecodeError::None)
					? AnimatedImageDecodeError::InvalidFormat
					: input.error();
				return result;
			}

			if ((format == WuffsImageFormat::GIF)
				&& (std::memcmp(input.source().reader_pointer(), "GIF87a", 6) != 0)
				&& (std::memcmp(input.source().reader_pointer(), "GIF89a", 6) != 0))
			{
				result.error = AnimatedImageDecodeError::InvalidFormat;
				return result;
			}

			wuffs_base__image_decoder::unique_ptr decoder{ nullptr };

			if (format == WuffsImageFormat::GIF)
			{
				decoder = wuffs_gif__decoder::alloc_as__wuffs_base__image_decoder();
			}
			else
			{
				decoder = wuffs_png__decoder::alloc_as__wuffs_base__image_decoder();
			}

			if (not decoder)
			{
				result.error = AnimatedImageDecodeError::OutOfMemory;
				return result;
			}

			wuffs_base__image_config imageConfig = wuffs_base__null_image_config();
			const PumpResult imageConfigResult = Pump(
				input,
				[&](wuffs_base__io_buffer& source)
				{
					return decoder->decode_image_config(&imageConfig, &source);
				});

			if (imageConfigResult.error != AnimatedImageDecodeError::None)
			{
				result.error = imageConfigResult.error;
				return result;
			}

			if (imageConfigResult.status.repr != nullptr)
			{
				result.error = ToDecodeError(imageConfigResult.status, format);
				return result;
			}

			if (format == WuffsImageFormat::APNG)
			{
				result.error = input.pngProbeResult();

				if (result.error != AnimatedImageDecodeError::None)
				{
					return result;
				}
			}

			const uint32 width = imageConfig.pixcfg.width();
			const uint32 height = imageConfig.pixcfg.height();

			if ((width == 0) || (height == 0)
				|| (Image::MaxWidth < width) || (Image::MaxHeight < height))
			{
				result.error = AnimatedImageDecodeError::InvalidData;
				return result;
			}

			const uint64 imageBytes = (static_cast<uint64>(width)
				* static_cast<uint64>(height) * sizeof(Color));

			if (options.maxTotalDecodedBytes < imageBytes)
			{
				result.error = AnimatedImageDecodeError::DecodedBytesLimitExceeded;
				return result;
			}

			wuffs_base__pixel_config pixelConfig = wuffs_base__null_pixel_config();
			pixelConfig.set(
				WUFFS_BASE__PIXEL_FORMAT__RGBA_NONPREMUL,
				WUFFS_BASE__PIXEL_SUBSAMPLING__NONE,
				width,
				height);

			const wuffs_base__range_ii_u64 workBufferRange = decoder->workbuf_len();
			const uint64 workBufferLength = workBufferRange.min_incl;

			if (static_cast<uint64>(std::numeric_limits<size_t>::max()) < workBufferLength)
			{
				result.error = AnimatedImageDecodeError::OutOfMemory;
				return result;
			}

			uint64 baseWorkingMemory = InputBufferSize;

			if (AddExceeds(baseWorkingMemory, imageBytes, options.maxWorkingMemoryBytes))
			{
				result.error = AnimatedImageDecodeError::WorkingMemoryLimitExceeded;
				return result;
			}

			baseWorkingMemory += imageBytes;

			if (AddExceeds(baseWorkingMemory, imageBytes, options.maxWorkingMemoryBytes))
			{
				result.error = AnimatedImageDecodeError::WorkingMemoryLimitExceeded;
				return result;
			}

			baseWorkingMemory += imageBytes;

			if (AddExceeds(baseWorkingMemory, workBufferLength, options.maxWorkingMemoryBytes))
			{
				result.error = AnimatedImageDecodeError::WorkingMemoryLimitExceeded;
				return result;
			}

			baseWorkingMemory += workBufferLength;
			Image canvas(width, height, Color{ 0, 0 });
			wuffs_base__pixel_buffer pixelBuffer = wuffs_base__null_pixel_buffer();
			const wuffs_base__status pixelBufferStatus = pixelBuffer.set_from_slice(
				&pixelConfig,
				wuffs_base__make_slice_u8(canvas.dataAsUint8(), canvas.size_bytes()));

			if (pixelBufferStatus.repr != nullptr)
			{
				result.error = AnimatedImageDecodeError::InvalidData;
				return result;
			}

			Array<uint8> workBuffer(static_cast<size_t>(workBufferLength));
			const wuffs_base__slice_u8 workBufferSlice =
				wuffs_base__make_slice_u8(workBuffer.data(), workBuffer.size());
			PreviousFrame previous;
			uint64 decodedBytes = 0;

			for (;;)
			{
				wuffs_base__frame_config frameConfig = wuffs_base__null_frame_config();
				const PumpResult frameConfigResult = Pump(
					input,
					[&](wuffs_base__io_buffer& source)
					{
						return decoder->decode_frame_config(&frameConfig, &source);
					});

				if (frameConfigResult.error != AnimatedImageDecodeError::None)
				{
					result.image = {};
					result.error = frameConfigResult.error;
					return result;
				}

				if (frameConfigResult.status.repr == wuffs_base__note__end_of_data)
				{
					break;
				}

				if (frameConfigResult.status.repr != nullptr)
				{
					result.image = {};
					result.error = ToDecodeError(frameConfigResult.status, format);
					return result;
				}

				if ((not firstFrameOnly)
					&& (options.maxFrames <= result.image.frames.size()))
				{
					result.image = {};
					result.error = AnimatedImageDecodeError::TooManyFrames;
					return result;
				}

				if (AddExceeds(decodedBytes, imageBytes, options.maxTotalDecodedBytes))
				{
					result.image = {};
					result.error = AnimatedImageDecodeError::DecodedBytesLimitExceeded;
					return result;
				}

				const wuffs_base__rect_ie_u32 bounds = frameConfig.bounds();
				const wuffs_base__rect_ie_u32 canvasBounds = pixelConfig.bounds();

				if (bounds.is_empty() || (not canvasBounds.contains_rect(bounds)))
				{
					result.image = {};
					result.error = AnimatedImageDecodeError::InvalidData;
					return result;
				}

				if (not result.image.frames.empty())
				{
					RestorePreviousFrame(canvas, pixelBuffer, previous);
				}
				else
				{
					pixelBuffer.set_color_u32_fill_rect(
						canvasBounds,
						frameConfig.background_color());
				}

				PreviousFrame current;
				current.bounds = bounds;
				current.disposal = frameConfig.disposal();
				current.backgroundColor = frameConfig.background_color();

				if ((current.disposal == WUFFS_BASE__ANIMATION_DISPOSAL__RESTORE_PREVIOUS)
					&& result.image.frames.empty())
				{
					current.disposal = WUFFS_BASE__ANIMATION_DISPOSAL__RESTORE_BACKGROUND;
				}
				else if (current.disposal == WUFFS_BASE__ANIMATION_DISPOSAL__RESTORE_PREVIOUS)
				{
					const uint64 restoreBytes = (static_cast<uint64>(bounds.width())
						* static_cast<uint64>(bounds.height()) * sizeof(Color));

					if (AddExceeds(baseWorkingMemory, restoreBytes, options.maxWorkingMemoryBytes))
					{
						result.image = {};
						result.error = AnimatedImageDecodeError::WorkingMemoryLimitExceeded;
						return result;
					}

					current.restorePixels = SaveFrameRectangle(canvas, bounds);
				}

				const wuffs_base__pixel_blend blend =
					(frameConfig.overwrite_instead_of_blend()
						? WUFFS_BASE__PIXEL_BLEND__SRC
						: WUFFS_BASE__PIXEL_BLEND__SRC_OVER);
				const PumpResult frameResult = Pump(
					input,
					[&](wuffs_base__io_buffer& source)
					{
						return decoder->decode_frame(
							&pixelBuffer,
							&source,
							blend,
							workBufferSlice,
							nullptr);
					});

				if (frameResult.error != AnimatedImageDecodeError::None)
				{
					result.image = {};
					result.error = frameResult.error;
					return result;
				}

				if (frameResult.status.repr != nullptr)
				{
					result.image = {};
					result.error = ToDecodeError(frameResult.status, format);
					return result;
				}

				Image snapshot = canvas;

				if (options.premultiplyAlpha == PremultiplyAlpha::Yes)
				{
					snapshot.premultiplyAlpha();
				}

				result.image.frames.push_back(AnimatedImageFrame{
					std::move(snapshot),
					Duration{ (static_cast<double>(frameConfig.duration())
						/ static_cast<double>(WUFFS_BASE__FLICKS_PER_SECOND)) }
				});
				decodedBytes += imageBytes;
				previous = std::move(current);

				if (firstFrameOnly)
				{
					break;
				}
			}

			if (result.image.frames.empty())
			{
				result.error = AnimatedImageDecodeError::InvalidData;
				return result;
			}

			result.image.playCount = decoder->num_animation_loops();

			if (result.image.frames.size() == 1)
			{
				result.image.playCount = 1;
			}

			result.error = AnimatedImageDecodeError::None;
			return result;
		}

		[[nodiscard]]
		static AnimatedImageDecodeResult Decode(
			std::unique_ptr<IReader> reader,
			const WuffsImageFormat format,
			const AnimatedImageDecodeOptions& options,
			const bool firstFrameOnly)
		{
			try
			{
				return DecodeWuffs(
					std::move(reader),
					format,
					options,
					firstFrameOnly);
			}
			catch (const std::bad_alloc&)
			{
				return{ {}, AnimatedImageDecodeError::OutOfMemory };
			}
		}
	}

	AnimatedImageDecodeResult DecodeAnimatedGIF(
		const FilePathView path,
		const AnimatedImageDecodeOptions& options)
	{
		return Decode(
			std::make_unique<BinaryFileReader>(path),
			WuffsImageFormat::GIF,
			options,
			false);
	}

	AnimatedImageDecodeResult DecodeAnimatedGIF(
		std::unique_ptr<IReader> reader,
		const AnimatedImageDecodeOptions& options)
	{
		return Decode(
			std::move(reader),
			WuffsImageFormat::GIF,
			options,
			false);
	}

	AnimatedImageDecodeResult DecodeAPNG(
		const FilePathView path,
		const AnimatedImageDecodeOptions& options)
	{
		return Decode(
			std::make_unique<BinaryFileReader>(path),
			WuffsImageFormat::APNG,
			options,
			false);
	}

	AnimatedImageDecodeResult DecodeAPNG(
		std::unique_ptr<IReader> reader,
		const AnimatedImageDecodeOptions& options)
	{
		return Decode(
			std::move(reader),
			WuffsImageFormat::APNG,
			options,
			false);
	}

	namespace detail
	{
		Image DecodeFirstGIFFrame(
			std::unique_ptr<IReader> reader,
			const PremultiplyAlpha premultiplyAlpha)
		{
			AnimatedImageDecodeOptions options;
			options.premultiplyAlpha = premultiplyAlpha;
			AnimatedImageDecodeResult result = Decode(
				std::move(reader),
				WuffsImageFormat::GIF,
				options,
				true);

			if (result.image.frames.empty())
			{
				return{};
			}

			return std::move(result.image.frames.front().image);
		}
	}
}
