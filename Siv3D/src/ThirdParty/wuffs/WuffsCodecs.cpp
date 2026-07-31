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
# include <Siv3D/AnimatedImageDecoder.hpp>
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
					&& (not m_isDone))
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

						if (m_isACTL)
						{
							std::memcpy(
								(m_animationControlData.data() + m_chunkDataIndex),
								data,
								count);
						}

						m_chunkDataIndex += count;
						m_chunkDataRemaining -= count;
						data += count;
						size -= count;

						if (m_isACTL && (m_chunkDataRemaining == 0))
						{
							m_declaredFrameCount = ReadBigEndianUint32(
								m_animationControlData.data());

							if (m_declaredFrameCount == 0)
							{
								m_error = AnimatedImageDecodeError::InvalidData;
								return;
							}

							m_isAnimated = true;
							m_isDone = true;
						}

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
						m_isACTL = false;
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

			[[nodiscard]]
			uint32 declaredFrameCount() const noexcept
			{
				return m_declaredFrameCount;
			}

			[[nodiscard]]
			bool isComplete() const noexcept
			{
				return (m_isDone
					|| (m_error != AnimatedImageDecodeError::None));
			}

		private:

			void beginChunk() noexcept
			{
				const uint32 chunkLength = ReadBigEndianUint32(m_chunkHeader.data());
				const uint8* const chunkType = (m_chunkHeader.data() + 4);
				const bool isFirstChunk = (m_chunkCount == 0);
				m_isIHDR = (std::memcmp(chunkType, "IHDR", 4) == 0);
				m_isACTL = (std::memcmp(chunkType, "acTL", 4) == 0);

				if ((isFirstChunk && ((not m_isIHDR) || (chunkLength != 13)))
					|| ((not isFirstChunk) && m_isIHDR))
				{
					m_error = AnimatedImageDecodeError::InvalidData;
					return;
				}

				++m_chunkCount;

				if (m_isACTL)
				{
					if ((chunkLength != 8) || m_seenIDAT)
					{
						m_error = AnimatedImageDecodeError::InvalidData;
						return;
					}
				}
				else if (std::memcmp(chunkType, "IDAT", 4) == 0)
				{
					m_seenIDAT = true;

					if (not m_isAnimated)
					{
						m_isStatic = true;
						m_isDone = true;
					}
				}

				m_chunkDataRemaining = chunkLength;
				m_crcRemaining = 4;
			}

			std::array<uint8, 8> m_signature{};

			std::array<uint8, 8> m_chunkHeader{};

			std::array<uint8, 8> m_animationControlData{};

			size_t m_signatureIndex = 0;

			size_t m_chunkHeaderIndex = 0;

			uint64 m_chunkDataRemaining = 0;

			size_t m_chunkDataIndex = 0;

			size_t m_crcRemaining = 0;

			size_t m_chunkCount = 0;

			AnimatedImageDecodeError m_error = AnimatedImageDecodeError::None;

			uint32 m_declaredFrameCount = 0;

			bool m_isIHDR = false;

			bool m_isACTL = false;

			bool m_seenIDAT = false;

			bool m_isAnimated = false;

			bool m_isStatic = false;

			bool m_isInterlaced = false;

			bool m_isDone = false;
		};

		class WuffsInput
		{
		public:

			WuffsInput(std::unique_ptr<IReader> reader, const bool probePNG)
				: m_reader{ std::move(reader) }
				, m_probePNG{ probePNG } {}

			[[nodiscard]]
			bool initialize()
			{
				if ((not m_reader) || (not *m_reader))
				{
					m_error = AnimatedImageDecodeError::ReadError;
					return false;
				}

				const int64 size = m_reader->size();
				const int64 pos = m_reader->getPos();

				if ((size < 0) || (pos < 0) || (size < pos))
				{
					m_error = AnimatedImageDecodeError::ReadError;
					return false;
				}

				m_buffer.resize(InputBufferSize);
				m_source = wuffs_base__ptr_u8__writer(
					m_buffer.data(),
					m_buffer.size());
				m_remaining = static_cast<uint64>(size - pos);
				m_source.meta.closed = (m_remaining == 0);
				return true;
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

			[[nodiscard]]
			uint32 declaredFrameCount() const noexcept
			{
				return m_pngProbe.declaredFrameCount();
			}

			[[nodiscard]]
			bool pngProbeIsComplete() const noexcept
			{
				return m_pngProbe.isComplete();
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
			const PreviousFrame& previous,
			const Array<Color>& restorePixels)
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
						(restorePixels.data() + sourceIndex),
						(static_cast<size_t>(width) * sizeof(Color)));
					sourceIndex += width;
				}
			}
		}

		static void SaveFrameRectangle(
			const Image& canvas,
			const wuffs_base__rect_ie_u32 bounds,
			Array<Color>& restorePixels)
		{
			const uint32 width = bounds.width();
			const uint32 height = bounds.height();
			restorePixels.resize(static_cast<size_t>(width) * height);
			size_t destinationIndex = 0;

			for (uint32 y = 0; y < height; ++y)
			{
				const Color* const src = (canvas.data()
					+ (static_cast<size_t>(bounds.min_incl_y + y) * canvas.width())
					+ bounds.min_incl_x);
				std::memcpy(
					(restorePixels.data() + destinationIndex),
					src,
					(static_cast<size_t>(width) * sizeof(Color)));
				destinationIndex += width;
			}
		}

		enum class FrameReadStatus
		{
			Frame,
			EndOfData,
			Error,
		};

		class WuffsAnimatedImageDecoder
		{
		public:

			WuffsAnimatedImageDecoder(
				std::unique_ptr<IReader> reader,
				const WuffsImageFormat format,
				const AnimatedImageDecodeOptions& options)
				: m_format{ format }
				, m_options{ options }
				, m_input{
					std::move(reader),
					(format == WuffsImageFormat::APNG)
				}
			{
				try
				{
					initialize();
				}
				catch (const std::bad_alloc&)
				{
					m_error = AnimatedImageDecodeError::OutOfMemory;
				}
			}

			[[nodiscard]]
			FrameReadStatus readFrame(AnimatedImageFrame& frame)
			{
				if (m_error != AnimatedImageDecodeError::None)
				{
					return FrameReadStatus::Error;
				}

				if (m_endOfData)
				{
					return FrameReadStatus::EndOfData;
				}

				try
				{
					return readFrameImpl(frame);
				}
				catch (const std::bad_alloc&)
				{
					return fail(AnimatedImageDecodeError::OutOfMemory);
				}
			}

			[[nodiscard]]
			AnimatedImageDecodeError error() const noexcept
			{
				return m_error;
			}

			[[nodiscard]]
			uint32 reserveFrameCount() const noexcept
			{
				if ((m_format != WuffsImageFormat::APNG)
					|| (not m_pngProbeValidated))
				{
					return 0;
				}

				const uint32 frameCount = m_input.declaredFrameCount();

				if ((m_options.maxFrames < frameCount)
					|| (m_imageBytes
						&& (m_options.maxTotalDecodedBytes / m_imageBytes < frameCount)))
				{
					return 0;
				}

				return frameCount;
			}

			[[nodiscard]]
			uint32 playCount() const noexcept
			{
				return m_decoder
					? m_decoder->num_animation_loops()
					: 0;
			}

		private:

			void initialize()
			{
				if (m_options.maxWorkingMemoryBytes < InputBufferSize)
				{
					m_error = AnimatedImageDecodeError::WorkingMemoryLimitExceeded;
					return;
				}

				if (not m_input.initialize())
				{
					m_error = m_input.error();
					return;
				}

				const size_t requiredHeaderBytes =
					(m_format == WuffsImageFormat::GIF) ? 6 : 8;

				if (not m_input.prepareHeader(requiredHeaderBytes))
				{
					m_error = (m_input.error() == AnimatedImageDecodeError::None)
						? AnimatedImageDecodeError::InvalidFormat
						: m_input.error();
					return;
				}

				if ((m_format == WuffsImageFormat::GIF)
					&& (std::memcmp(m_input.source().reader_pointer(), "GIF87a", 6) != 0)
					&& (std::memcmp(m_input.source().reader_pointer(), "GIF89a", 6) != 0))
				{
					m_error = AnimatedImageDecodeError::InvalidFormat;
					return;
				}

				if (m_format == WuffsImageFormat::GIF)
				{
					m_decoder = wuffs_gif__decoder::alloc_as__wuffs_base__image_decoder();
				}
				else
				{
					m_decoder = wuffs_png__decoder::alloc_as__wuffs_base__image_decoder();
				}

				if (not m_decoder)
				{
					m_error = AnimatedImageDecodeError::OutOfMemory;
					return;
				}

				wuffs_base__image_config imageConfig = wuffs_base__null_image_config();
				const PumpResult imageConfigResult = Pump(
					m_input,
					[&](wuffs_base__io_buffer& source)
					{
						return m_decoder->decode_image_config(&imageConfig, &source);
					});

				if (imageConfigResult.error != AnimatedImageDecodeError::None)
				{
					m_error = imageConfigResult.error;
					return;
				}

				if (imageConfigResult.status.repr != nullptr)
				{
					m_error = ToDecodeError(imageConfigResult.status, m_format);
					return;
				}

				if ((m_format == WuffsImageFormat::APNG)
					&& m_input.pngProbeIsComplete())
				{
					m_error = m_input.pngProbeResult();

					if (m_error != AnimatedImageDecodeError::None)
					{
						return;
					}

					m_pngProbeValidated = true;
				}

				const uint32 width = imageConfig.pixcfg.width();
				const uint32 height = imageConfig.pixcfg.height();

				if ((width == 0) || (height == 0)
					|| (Image::MaxWidth < width) || (Image::MaxHeight < height))
				{
					m_error = AnimatedImageDecodeError::InvalidData;
					return;
				}

				m_imageBytes = (static_cast<uint64>(width)
					* static_cast<uint64>(height) * sizeof(Color));

				if (m_options.maxTotalDecodedBytes < m_imageBytes)
				{
					m_error = AnimatedImageDecodeError::DecodedBytesLimitExceeded;
					return;
				}

				m_pixelConfig.set(
					WUFFS_BASE__PIXEL_FORMAT__RGBA_NONPREMUL,
					WUFFS_BASE__PIXEL_SUBSAMPLING__NONE,
					width,
					height);

				const wuffs_base__range_ii_u64 workBufferRange =
					m_decoder->workbuf_len();
				const uint64 workBufferLength = workBufferRange.min_incl;

				if (static_cast<uint64>(std::numeric_limits<size_t>::max())
					< workBufferLength)
				{
					m_error = AnimatedImageDecodeError::OutOfMemory;
					return;
				}

				m_baseWorkingMemory = InputBufferSize;

				if (AddExceeds(
					m_baseWorkingMemory,
					m_imageBytes,
					m_options.maxWorkingMemoryBytes))
				{
					m_error = AnimatedImageDecodeError::WorkingMemoryLimitExceeded;
					return;
				}

				m_baseWorkingMemory += m_imageBytes;

				if (AddExceeds(
					m_baseWorkingMemory,
					m_imageBytes,
					m_options.maxWorkingMemoryBytes))
				{
					m_error = AnimatedImageDecodeError::WorkingMemoryLimitExceeded;
					return;
				}

				m_baseWorkingMemory += m_imageBytes;

				if (AddExceeds(
					m_baseWorkingMemory,
					workBufferLength,
					m_options.maxWorkingMemoryBytes))
				{
					m_error = AnimatedImageDecodeError::WorkingMemoryLimitExceeded;
					return;
				}

				m_baseWorkingMemory += workBufferLength;
				m_canvas = Image{ width, height, Color{ 0, 0 } };
				const wuffs_base__status pixelBufferStatus =
					m_pixelBuffer.set_from_slice(
						&m_pixelConfig,
						wuffs_base__make_slice_u8(
							m_canvas.dataAsUint8(),
							m_canvas.size_bytes()));

				if (pixelBufferStatus.repr != nullptr)
				{
					m_error = AnimatedImageDecodeError::InvalidData;
					return;
				}

				m_workBuffer.resize(static_cast<size_t>(workBufferLength));
				m_workBufferSlice = wuffs_base__make_slice_u8(
					m_workBuffer.data(),
					m_workBuffer.size());
			}

			[[nodiscard]]
			FrameReadStatus readFrameImpl(AnimatedImageFrame& frame)
			{
				wuffs_base__frame_config frameConfig =
					wuffs_base__null_frame_config();
				const PumpResult frameConfigResult = Pump(
					m_input,
					[&](wuffs_base__io_buffer& source)
					{
						return m_decoder->decode_frame_config(&frameConfig, &source);
					});

				if (frameConfigResult.error != AnimatedImageDecodeError::None)
				{
					return fail(frameConfigResult.error);
				}

				if (frameConfigResult.status.repr == wuffs_base__note__end_of_data)
				{
					if (m_frameCount == 0)
					{
						return fail(AnimatedImageDecodeError::InvalidData);
					}

					m_endOfData = true;
					return FrameReadStatus::EndOfData;
				}

				if (frameConfigResult.status.repr != nullptr)
				{
					return fail(ToDecodeError(frameConfigResult.status, m_format));
				}

				if ((m_format == WuffsImageFormat::APNG)
					&& (not m_pngProbeValidated))
				{
					const AnimatedImageDecodeError probeError =
						m_input.pngProbeResult();

					if (probeError != AnimatedImageDecodeError::None)
					{
						return fail(probeError);
					}

					m_pngProbeValidated = true;
				}

				if (m_options.maxFrames <= m_frameCount)
				{
					return fail(AnimatedImageDecodeError::TooManyFrames);
				}

				if (AddExceeds(
					m_decodedBytes,
					m_imageBytes,
					m_options.maxTotalDecodedBytes))
				{
					return fail(AnimatedImageDecodeError::DecodedBytesLimitExceeded);
				}

				const wuffs_base__rect_ie_u32 bounds = frameConfig.bounds();
				const wuffs_base__rect_ie_u32 canvasBounds =
					m_pixelConfig.bounds();

				if (bounds.is_empty() || (not canvasBounds.contains_rect(bounds)))
				{
					return fail(AnimatedImageDecodeError::InvalidData);
				}

				if (m_frameCount != 0)
				{
					RestorePreviousFrame(
						m_canvas,
						m_pixelBuffer,
						m_previous,
						m_restorePixels);
				}
				else
				{
					m_pixelBuffer.set_color_u32_fill_rect(
						canvasBounds,
						frameConfig.background_color());
				}

				PreviousFrame current;
				current.bounds = bounds;
				current.disposal = frameConfig.disposal();
				current.backgroundColor = frameConfig.background_color();

				if ((current.disposal
						== WUFFS_BASE__ANIMATION_DISPOSAL__RESTORE_PREVIOUS)
					&& (m_frameCount == 0))
				{
					current.disposal =
						WUFFS_BASE__ANIMATION_DISPOSAL__RESTORE_BACKGROUND;
				}
				else if (current.disposal
					== WUFFS_BASE__ANIMATION_DISPOSAL__RESTORE_PREVIOUS)
				{
					const uint64 restoreBytes =
						(static_cast<uint64>(bounds.width())
							* static_cast<uint64>(bounds.height())
							* sizeof(Color));
					const uint64 peakRestoreBytes =
						std::max(m_restoreBytes, restoreBytes);

					if (AddExceeds(
						m_baseWorkingMemory,
						peakRestoreBytes,
						m_options.maxWorkingMemoryBytes))
					{
						return fail(
							AnimatedImageDecodeError::WorkingMemoryLimitExceeded);
					}

					SaveFrameRectangle(m_canvas, bounds, m_restorePixels);
					m_restoreBytes = peakRestoreBytes;
				}

				const wuffs_base__pixel_blend blend =
					(frameConfig.overwrite_instead_of_blend()
						? WUFFS_BASE__PIXEL_BLEND__SRC
						: WUFFS_BASE__PIXEL_BLEND__SRC_OVER);
				const PumpResult frameResult = Pump(
					m_input,
					[&](wuffs_base__io_buffer& source)
					{
						return m_decoder->decode_frame(
							&m_pixelBuffer,
							&source,
							blend,
							m_workBufferSlice,
							nullptr);
					});

				if (frameResult.error != AnimatedImageDecodeError::None)
				{
					return fail(frameResult.error);
				}

				if (frameResult.status.repr != nullptr)
				{
					return fail(ToDecodeError(frameResult.status, m_format));
				}

				frame.image = m_canvas;

				if (m_options.premultiplyAlpha == PremultiplyAlpha::Yes)
				{
					frame.image.premultiplyAlpha();
				}

				frame.duration = Duration{
					(static_cast<double>(frameConfig.duration())
						/ static_cast<double>(WUFFS_BASE__FLICKS_PER_SECOND))
				};
				m_decodedBytes += m_imageBytes;
				++m_frameCount;
				m_previous = current;
				return FrameReadStatus::Frame;
			}

			[[nodiscard]]
			FrameReadStatus fail(
				const AnimatedImageDecodeError error) noexcept
			{
				m_error = error;
				return FrameReadStatus::Error;
			}

			WuffsImageFormat m_format;

			AnimatedImageDecodeOptions m_options;

			WuffsInput m_input;

			wuffs_base__image_decoder::unique_ptr m_decoder{ nullptr };

			wuffs_base__pixel_config m_pixelConfig =
				wuffs_base__null_pixel_config();

			Image m_canvas;

			wuffs_base__pixel_buffer m_pixelBuffer =
				wuffs_base__null_pixel_buffer();

			Array<uint8> m_workBuffer;

			wuffs_base__slice_u8 m_workBufferSlice =
				wuffs_base__empty_slice_u8();

			PreviousFrame m_previous;

			Array<Color> m_restorePixels;

			uint64 m_imageBytes = 0;

			uint64 m_baseWorkingMemory = 0;

			uint64 m_restoreBytes = 0;

			uint64 m_decodedBytes = 0;

			uint32 m_frameCount = 0;

			AnimatedImageDecodeError m_error = AnimatedImageDecodeError::None;

			bool m_endOfData = false;

			bool m_pngProbeValidated = false;
		};

		[[nodiscard]]
		static AnimatedImageDecodeResult DecodeWuffs(
			std::unique_ptr<IReader> reader,
			const WuffsImageFormat format,
			const AnimatedImageDecodeOptions& options)
		{
			AnimatedImageDecodeResult result;
			WuffsAnimatedImageDecoder decoder{
				std::move(reader),
				format,
				options
			};

			if (decoder.error() != AnimatedImageDecodeError::None)
			{
				result.error = decoder.error();
				return result;
			}

			if (const uint32 frameCount = decoder.reserveFrameCount())
			{
				result.image.frames.reserve(frameCount);
			}

			for (;;)
			{
				result.image.frames.emplace_back();
				const FrameReadStatus status =
					decoder.readFrame(result.image.frames.back());

				if (status == FrameReadStatus::Frame)
				{
					continue;
				}

				result.image.frames.pop_back();

				if (status == FrameReadStatus::EndOfData)
				{
					break;
				}

				result.image = {};
				result.error = decoder.error();
				return result;
			}

			result.image.playCount = (result.image.frames.size() == 1)
				? 1
				: decoder.playCount();
			return result;
		}

		[[nodiscard]]
		static AnimatedImageDecodeResult Decode(
			std::unique_ptr<IReader> reader,
			const WuffsImageFormat format,
			const AnimatedImageDecodeOptions& options)
		{
			try
			{
				return DecodeWuffs(
					std::move(reader),
					format,
					options);
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
			options);
	}

	AnimatedImageDecodeResult DecodeAnimatedGIF(
		std::unique_ptr<IReader> reader,
		const AnimatedImageDecodeOptions& options)
	{
		return Decode(
			std::move(reader),
			WuffsImageFormat::GIF,
			options);
	}

	AnimatedImageDecodeResult DecodeAPNG(
		const FilePathView path,
		const AnimatedImageDecodeOptions& options)
	{
		return Decode(
			std::make_unique<BinaryFileReader>(path),
			WuffsImageFormat::APNG,
			options);
	}

	AnimatedImageDecodeResult DecodeAPNG(
		std::unique_ptr<IReader> reader,
		const AnimatedImageDecodeOptions& options)
	{
		return Decode(
			std::move(reader),
			WuffsImageFormat::APNG,
			options);
	}

	namespace detail
	{
		Image DecodeFirstGIFFrame(
			std::unique_ptr<IReader> reader,
			const PremultiplyAlpha premultiplyAlpha)
		{
			AnimatedImageDecodeOptions options;
			options.premultiplyAlpha = premultiplyAlpha;
			WuffsAnimatedImageDecoder decoder{
				std::move(reader),
				WuffsImageFormat::GIF,
				options
			};
			AnimatedImageFrame frame;

			if ((decoder.error() != AnimatedImageDecodeError::None)
				|| (decoder.readFrame(frame) != FrameReadStatus::Frame))
			{
				return{};
			}

			return std::move(frame.image);
		}
	}
}
