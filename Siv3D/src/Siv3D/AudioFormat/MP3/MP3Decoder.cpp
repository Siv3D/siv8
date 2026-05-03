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

# include <Siv3D/AudioFormat/MP3Decoder.hpp>
# include <Siv3D/IReader.hpp>
# include <Siv3D/EngineLog.hpp>

#define DR_MP3_NO_STDIO
#define DR_MP3_IMPLEMENTATION
#include <ThirdParty/dr_mp3/dr_mp3.h>

namespace s3d
{
	namespace
	{
		struct MP3BasicInfo
		{
			bool isMP3 = false;
			bool sampleCountIsExact = false;
			bool isVBR = false;
			bool isCBR = false;
			uint32 channels = 0;
			uint32 sampleRate = 0;
			uint32 encoderDelay = 0;
			uint32 encoderPadding = 0;
			uint64 pcmFrameCount = 0; // Wave::samples() に対応する「PCM フレーム数」
		};

		class DRMP3ReaderBridge
		{
		public:
			explicit DRMP3ReaderBridge(IReader& reader)
				: m_reader{ std::addressof(reader) }
				, m_basePos{ reader.getPos() }
				, m_endPos{ reader.size() } {}

			[[nodiscard]]
			int64 basePos() const noexcept
			{
				return m_basePos;
			}

			[[nodiscard]]
			int64 logicalSize() const noexcept
			{
				return (m_endPos - m_basePos);
			}

			[[nodiscard]]
			static size_t OnRead(void* pUserData, void* pBufferOut, size_t bytesToRead)
			{
				auto& self = *static_cast<DRMP3ReaderBridge*>(pUserData);
				return static_cast<size_t>(self.m_reader->read(pBufferOut, static_cast<int64>(bytesToRead)));
			}

			[[nodiscard]]
			static drmp3_bool32 OnSeek(void* pUserData, int offset, drmp3_seek_origin origin)
			{
				auto& self = *static_cast<DRMP3ReaderBridge*>(pUserData);

				int64 target = 0;
				switch (origin)
				{
				case DRMP3_SEEK_SET:
					target = (self.m_basePos + static_cast<int64>(offset));
					break;
				case DRMP3_SEEK_CUR:
					target = (self.m_reader->getPos() + static_cast<int64>(offset));
					break;
				case DRMP3_SEEK_END:
					target = (self.m_endPos + static_cast<int64>(offset));
					break;
				default:
					return DRMP3_FALSE;
				}

				if ((target < self.m_basePos) || (self.m_endPos < target))
				{
					return DRMP3_FALSE;
				}

				return (self.m_reader->setPos(target) == target) ? DRMP3_TRUE : DRMP3_FALSE;
			}

			[[nodiscard]]
			static drmp3_bool32 OnTell(void* pUserData, drmp3_int64* pCursor)
			{
				if (pCursor == nullptr)
				{
					return DRMP3_FALSE;
				}

				auto& self = *static_cast<DRMP3ReaderBridge*>(pUserData);
				*pCursor = static_cast<drmp3_int64>(self.m_reader->getPos() - self.m_basePos);
				return DRMP3_TRUE;
			}

		private:
			
			IReader* m_reader = nullptr;
			
			int64 m_basePos = 0;
			
			int64 m_endPos = 0;
		};

		class ReaderPosScope
		{
		public:

			explicit ReaderPosScope(IReader& reader)
				: m_reader{ std::addressof(reader) }
				, m_savedPos{ reader.getPos() } {}

			~ReaderPosScope()
			{
				if (m_reader)
				{
					m_reader->setPos(m_savedPos);
				}
			}

			ReaderPosScope(const ReaderPosScope&) = delete;
			
			ReaderPosScope& operator =(const ReaderPosScope&) = delete;

		private:
			
			IReader* m_reader = nullptr;
			
			int64 m_savedPos = 0;
		};

		[[nodiscard]]
		static uint64 GetExactPCMFrameCountIfKnown(const drmp3& mp3) noexcept
		{
			if (mp3.totalPCMFrameCount == DRMP3_UINT64_MAX)
			{
				return 0;
			}

			uint64 total = static_cast<uint64>(mp3.totalPCMFrameCount);

			if (total >= mp3.delayInPCMFrames)
			{
				total -= mp3.delayInPCMFrames;
			}
			else
			{
				total = 0;
			}

			if (total >= mp3.paddingInPCMFrames)
			{
				total -= mp3.paddingInPCMFrames;
			}
			else
			{
				total = 0;
			}

			return total;
		}

		static void InterleavedFloatToStereoSamples(const float* pSource, const uint64 frames, const uint32 channels, StereoSampleFloat32* pDst)
		{
			if ((pSource == nullptr) || (pDst == nullptr) || (frames == 0))
			{
				return;
			}

			if (channels <= 1)
			{
				for (uint64 i = 0; i < frames; ++i)
				{
					const float v = pSource[i];
					pDst[i].left = v;
					pDst[i].right = v;
				}
			}
			else
			{
				for (uint64 i = 0; i < frames; ++i)
				{
					pDst[i].left = pSource[(i * channels) + 0];
					pDst[i].right = pSource[(i * channels) + 1];
				}
			}
		}

		[[nodiscard]]
		static MP3BasicInfo MakeBasicInfo(const drmp3& mp3) noexcept
		{
			MP3BasicInfo info;
			info.isMP3 = true;
			info.sampleCountIsExact = (mp3.totalPCMFrameCount != DRMP3_UINT64_MAX);
			info.isVBR = (mp3.isVBR != 0);
			info.isCBR = (mp3.isCBR != 0);
			info.channels = static_cast<uint32>(mp3.channels);
			info.sampleRate = static_cast<uint32>(mp3.sampleRate);
			info.encoderDelay = static_cast<uint32>(mp3.delayInPCMFrames);
			info.encoderPadding = static_cast<uint32>(mp3.paddingInPCMFrames);
			info.pcmFrameCount = GetExactPCMFrameCountIfKnown(mp3);
			return info;
		}

		class MP3StreamDecoder
		{
		public:
			
			MP3StreamDecoder() = default;

			explicit MP3StreamDecoder(std::unique_ptr<IReader> reader)
			{
				open(std::move(reader));
			}

			~MP3StreamDecoder()
			{
				close();
			}

			MP3StreamDecoder(const MP3StreamDecoder&) = delete;
			
			MP3StreamDecoder& operator =(const MP3StreamDecoder&) = delete;
			
			MP3StreamDecoder(MP3StreamDecoder&&) = delete;
			
			MP3StreamDecoder& operator =(MP3StreamDecoder&&) = delete;

			bool open(std::unique_ptr<IReader> reader)
			{
				close();

				if ((not reader) || (not reader->isOpen()))
				{
					return false;
				}

				m_reader = std::move(reader);
				m_bridge = std::make_unique<DRMP3ReaderBridge>(*m_reader);

				if (not drmp3_init(&m_mp3,
					DRMP3ReaderBridge::OnRead,
					DRMP3ReaderBridge::OnSeek,
					DRMP3ReaderBridge::OnTell,
					nullptr,
					m_bridge.get(),
					nullptr))
				{
					m_bridge.reset();
					m_reader.reset();
					return false;
				}

				m_info = MakeBasicInfo(m_mp3);
				m_isOpen = true;
				return true;
			}

			void close()
			{
				if (m_isOpen)
				{
					drmp3_uninit(&m_mp3);
					m_isOpen = false;
				}

				m_bridge.reset();
				m_reader.reset();
				m_info = {};
				m_floatBuffer.clear();
			}

			[[nodiscard]]
			bool isOpen() const noexcept
			{
				return m_isOpen;
			}

			[[nodiscard]]
			explicit operator bool() const noexcept
			{
				return isOpen();
			}

			[[nodiscard]]
			const MP3BasicInfo& info() const noexcept
			{
				return m_info;
			}

			[[nodiscard]]
			uint64 readFrames(StereoSampleFloat32* dst, uint64 framesToRead)
			{
				if ((not m_isOpen) || (dst == nullptr) || (framesToRead == 0))
				{
					return 0;
				}

				const uint32 channels = std::max<uint32>(1, m_info.channels);
				const size_t requiredFloatCount = static_cast<size_t>(framesToRead) * channels;
				if (m_floatBuffer.size() < requiredFloatCount)
				{
					m_floatBuffer.resize(requiredFloatCount);
				}

				const uint64 framesRead = static_cast<uint64>(drmp3_read_pcm_frames_f32(&m_mp3, static_cast<drmp3_uint64>(framesToRead), m_floatBuffer.data()));

				InterleavedFloatToStereoSamples(m_floatBuffer.data(), framesRead, channels, dst);
				return framesRead;
			}

			[[nodiscard]]
			bool seekToFrame(uint64 pcmFrameIndex)
			{
				if (not m_isOpen)
				{
					return false;
				}

				return (drmp3_seek_to_pcm_frame(&m_mp3, static_cast<drmp3_uint64>(pcmFrameIndex)) != 0);
			}

			bool rewind()
			{
				return seekToFrame(0);
			}

			[[nodiscard]]
			Wave readAll()
			{
				Wave wave;

				if (not m_isOpen)
				{
					return wave;
				}

				wave.setSampleRate(m_info.sampleRate);

				constexpr uint64 ChunkFrames = 4096;
				Array<StereoSampleFloat32> chunk(static_cast<size_t>(ChunkFrames));

				if (m_info.sampleCountIsExact)
				{
					wave.resize(static_cast<Wave::size_type>(m_info.pcmFrameCount));
					uint64 written = 0;

					while (written < m_info.pcmFrameCount)
					{
						const uint64 requestFrames = std::min<uint64>(ChunkFrames, (m_info.pcmFrameCount - written));
						const uint64 framesRead = readFrames((wave.data() + written), requestFrames);
						if (framesRead == 0)
						{
							break;
						}

						written += framesRead;
					}

					if (written != m_info.pcmFrameCount)
					{
						wave.resize(static_cast<Wave::size_type>(written));
					}
				}
				else
				{
					Array<StereoSampleFloat32> samples;
					samples.reserve(static_cast<size_t>(ChunkFrames * 4));

					for (;;)
					{
						const uint64 framesRead = readFrames(chunk.data(), ChunkFrames);
						if (framesRead == 0)
						{
							break;
						}

						samples.insert(samples.end(), chunk.begin(), (chunk.begin() + static_cast<std::ptrdiff_t>(framesRead)));
					}

					wave.resize(static_cast<Wave::size_type>(samples.size()));
					if (not samples.empty())
					{
						std::memcpy(wave.data(), samples.data(), (samples.size() * sizeof(StereoSampleFloat32)));
					}
				}

				return wave;
			}

		private:
			
			std::unique_ptr<IReader> m_reader;
			
			std::unique_ptr<DRMP3ReaderBridge> m_bridge;
			
			drmp3 m_mp3{};
			
			MP3BasicInfo m_info;
			
			Array<float> m_floatBuffer;
			
			bool m_isOpen = false;
		};
	}

	////////////////////////////////////////////////////////////////
	//
	//	name
	//
	////////////////////////////////////////////////////////////////

	StringView MP3Decoder::name() const noexcept
	{
		return U"MP3"_sv;
	}

	////////////////////////////////////////////////////////////////
	//
	//	audioFormat
	//
	////////////////////////////////////////////////////////////////

	AudioFormat MP3Decoder::audioFormat() const noexcept
	{
		return AudioFormat::MP3;
	}

	////////////////////////////////////////////////////////////////
	//
	//	isHeader
	//
	////////////////////////////////////////////////////////////////

	bool MP3Decoder::isHeader(const uint8(&bytes)[RequiredHeaderBytes]) const noexcept
	{
		static_assert(3 <= RequiredHeaderBytes);

		auto ReadSynchsafe32 = [](const uint8* p) -> uint32_t
		{
			return (uint32_t(p[0]) << 21)
				| (uint32_t(p[1]) << 14)
				| (uint32_t(p[2]) << 7)
				| uint32_t(p[3]);
		};

		auto HasValidMp3FrameHeaderAt = [&](size_t pos) -> bool
		{
			if (pos + 4 > RequiredHeaderBytes)
				return false;

			const uint8 b0 = bytes[pos + 0];
			const uint8 b1 = bytes[pos + 1];
			const uint8 b2 = bytes[pos + 2];
			const uint8 b3 = bytes[pos + 3];

			// sync
			if (b0 != 0xFF || (b1 & 0xE0) != 0xE0)
				return false;

			const uint8 versionBits = (b1 >> 3) & 0x03;
			const uint8 layerBits = (b1 >> 1) & 0x03;
			const uint8 bitrateIndex = (b2 >> 4) & 0x0F;
			const uint8 sampleRateBits = (b2 >> 2) & 0x03;
			const uint8 emphasisBits = b3 & 0x03;

			// ここでは precision 寄りに MPEG-1/2 Layer III だけ許可
			// versionBits: 3=MPEG1, 2=MPEG2, 1=reserved, 0=MPEG2.5(ここでは不許可)
			if (versionBits != 0x03 && versionBits != 0x02)
				return false;

			// Layer III
			if (layerBits != 0x01)
				return false;

			// invalid / reserved
			if (bitrateIndex == 0x00 || bitrateIndex == 0x0F)
				return false;
			if (sampleRateBits == 0x03)
				return false;
			if (emphasisBits == 0x02)
				return false;

			return true;
		};

		auto HasValidID3v2Header = [](const uint8* p) -> bool
		{
			// "ID3"
			if (p[0] != 0x49 || p[1] != 0x44 || p[2] != 0x33)
				return false;

			const uint8 ver = p[3];
			const uint8 rev = p[4];
			const uint8 flags = p[5];

			// detection pattern 的に version/revision は 0xFF ではない
			if (ver == 0xFF || rev == 0xFF)
				return false;

			// ここでは v2.3 / v2.4 に限定
			if (ver == 0x03)
			{
				// v2.3: 上位3bit以外は未使用
				if ((flags & 0x1F) != 0)
					return false;
			}
			else if (ver == 0x04)
			{
				// v2.4: 上位4bit以外は未使用
				if ((flags & 0x0F) != 0)
					return false;
			}
			else
			{
				return false;
			}

			// synchsafe integer
			if ((p[6] & 0x80) || (p[7] & 0x80) || (p[8] & 0x80) || (p[9] & 0x80))
				return false;

			return true;
		};

		// 1) 先頭がいきなり MP3 フレーム
		if (HasValidMp3FrameHeaderAt(0))
		{
			return true;
		}

		// 2) 先頭が ID3v2
		if (not HasValidID3v2Header(bytes))
		{
			return false;
		}

		const uint8 ver = bytes[3];
		const uint8 flags = bytes[5];
		const bool hasFooter = (ver == 0x04) && ((flags & 0x10) != 0);

		// ID3 size は header(10 bytes) を含まない
		const size_t tagSize = ReadSynchsafe32(bytes + 6);
		const size_t firstAudioPos = 10 + tagSize + (hasFooter ? 10 : 0);

		// 64 バイト内に最初の audio frame が見えるなら、追加で確定判定
		if (firstAudioPos + 4 <= RequiredHeaderBytes)
		{
			return HasValidMp3FrameHeaderAt(firstAudioPos);
		}

		// 分からない場合は true
		return true;
	}

	////////////////////////////////////////////////////////////////
	//
	//	possibleExtensions
	//
	////////////////////////////////////////////////////////////////

	std::span<const StringView> MP3Decoder::possibleExtensions() const noexcept
	{
		static constexpr std::array<StringView, 1> Extensions = { U"mp3" };
		return Extensions;
	}

	////////////////////////////////////////////////////////////////
	//
	//	decode
	//
	////////////////////////////////////////////////////////////////

	Wave MP3Decoder::decode(const FilePathView path) const
	{
		return IAudioDecoder::decode(path);
	}

	Wave MP3Decoder::decode(std::unique_ptr<IReader> reader, [[maybe_unused]] const FilePathView pathHint) const
	{
		if ((not reader) || (not reader->isOpen()))
		{
			return{};
		}

		std::unique_ptr<MP3StreamDecoder> decoder = std::make_unique<MP3StreamDecoder>(std::move(reader));

		return decoder->readAll();
	}
}
