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

# include <Siv3D/WaveProcessing.hpp>
# include <Siv3D/Error.hpp>
# include <ThirdParty/r8brain-free-src/CDSPResampler.h>

namespace s3d
{
	namespace WaveProcessing
	{
		Wave Resample(const Wave& input, const uint32 newSampleRate)
		{
			const double inSampleRate = static_cast<double>(input.sampleRate());
			const double outSampleRate = static_cast<double>(newSampleRate);

			if (input.isEmpty() || (inSampleRate <= 0.0) || (outSampleRate <= 0.0))
			{
				return{};
			}

			if (not InRange(newSampleRate, Wave::MinSampleRate, Wave::MaxSampleRate))
			{
				throw Error{ U"WaveProcessing::Resample(): newSampleRate is out of range." };
			}

			if (inSampleRate == outSampleRate)
			{
				return input;
			}

			constexpr int32 BlockFrames = 8192;
			const int32 inFrames = static_cast<int32>(input.size());

			r8b::CDSPResampler24 leftResampler{ inSampleRate, outSampleRate, BlockFrames };
			r8b::CDSPResampler24 rightResampler{ inSampleRate, outSampleRate, BlockFrames };

			Array<double> leftIn(BlockFrames);
			Array<double> rightIn(BlockFrames);

			const size_t estimatedFrames = static_cast<size_t>(std::ceil(static_cast<double>(inFrames) * outSampleRate / inSampleRate));
			Wave output{ Arg::sampleRate = newSampleRate };
			output.reserve(estimatedFrames + 16);

			for (int32 pos = 0; pos < inFrames; pos += BlockFrames)
			{
				const int32 blockLen = Min(BlockFrames, (inFrames - pos));

				for (int32 i = 0; i < blockLen; ++i)
				{
					leftIn[i] = input[pos + i].left;
					rightIn[i] = input[pos + i].right;
				}

				double* leftOutPtr = nullptr;
				double* rightOutPtr = nullptr;

				const int32 leftOutLen = leftResampler.process(leftIn.data(), blockLen, leftOutPtr);
				const int32 rightOutLen = rightResampler.process(rightIn.data(), blockLen, rightOutPtr);

				if (leftOutLen != rightOutLen)
				{
					throw Error{ U"WaveProcessing::Resample(): channel output length mismatch." };
				}

				const size_t oldSize = output.size();
				output.resize(oldSize + leftOutLen);

				for (int32 i = 0; i < leftOutLen; ++i)
				{
					output[oldSize + i].left = static_cast<float>(leftOutPtr[i]);
					output[oldSize + i].right = static_cast<float>(rightOutPtr[i]);
				}
			}

			return output;
		}
	}
}
