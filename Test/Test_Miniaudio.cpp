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
# include "../Siv3D/src/ThirdParty/miniaudio/miniaudio.h"
# include <array>

TEST_CASE("Miniaudio.data_converter_channel_maps")
{
	std::array<ma_channel, 2> inputMap{ MA_CHANNEL_FRONT_RIGHT, MA_CHANNEL_FRONT_LEFT };
	std::array<ma_channel, 2> outputMap{ MA_CHANNEL_FRONT_LEFT, MA_CHANNEL_FRONT_RIGHT };
	auto config = ma_data_converter_config_init(ma_format_f32, ma_format_f32, 2, 2, 48000, 48000);
	config.pChannelMapIn = inputMap.data();
	config.pChannelMapOut = outputMap.data();

	ma_data_converter converter{};
	REQUIRE(ma_data_converter_init(&config, nullptr, &converter) == MA_SUCCESS);
	const ScopeExit cleanup{ [&] { ma_data_converter_uninit(&converter, nullptr); } };

	std::array<ma_channel, 2> actualInputMap{};
	std::array<ma_channel, 2> actualOutputMap{};
	REQUIRE(ma_data_converter_get_input_channel_map(&converter, actualInputMap.data(), static_cast<ma_uint32>(actualInputMap.size())) == MA_SUCCESS);
	REQUIRE(ma_data_converter_get_output_channel_map(&converter, actualOutputMap.data(), static_cast<ma_uint32>(actualOutputMap.size())) == MA_SUCCESS);
	CHECK(actualInputMap == inputMap);
	CHECK(actualOutputMap == outputMap);

	const std::array<float, 4> input{ 0.25f, -0.5f, 0.75f, -1.0f };
	const std::array<float, 4> expected{ -0.5f, 0.25f, -1.0f, 0.75f };
	std::array<float, 4> output{};
	ma_uint64 inputFrames = 2;
	ma_uint64 outputFrames = 2;
	REQUIRE(ma_data_converter_process_pcm_frames(&converter, input.data(), &inputFrames, output.data(), &outputFrames) == MA_SUCCESS);
	CHECK(inputFrames == 2);
	CHECK(outputFrames == 2);
	CHECK(output == expected);
}

TEST_CASE("Miniaudio.gainer_smoothing_boundary")
{
	const std::array<float, 6> expectedGains{ 1.0f, 0.75f, 0.5f, 0.25f, 0.0f, 0.0f };

	for (const ma_uint32 channels : { 2u, 6u })
	{
		for (const ma_uint64 blockFrames : { 3u, 6u })
		{
			CAPTURE(channels, blockFrames);
			const auto config = ma_gainer_config_init(channels, 4);
			ma_gainer gainer{};
			REQUIRE(ma_gainer_init(&config, nullptr, &gainer) == MA_SUCCESS);
			const ScopeExit cleanup{ [&] { ma_gainer_uninit(&gainer, nullptr); } };

			// The initial gain is immediate; subsequent changes use the configured ramp.
			REQUIRE(ma_gainer_set_gain(&gainer, 1.0f) == MA_SUCCESS);
			REQUIRE(ma_gainer_set_gain(&gainer, 0.0f) == MA_SUCCESS);
			std::array<float, 36> input;
			input.fill(1.0f);
			std::array<float, 36> output{};

			for (ma_uint64 frame = 0; frame < expectedGains.size(); frame += blockFrames)
			{
				const size_t sampleOffset = static_cast<size_t>(frame * channels);
				REQUIRE(ma_gainer_process_pcm_frames(&gainer, (output.data() + sampleOffset), (input.data() + sampleOffset), blockFrames) == MA_SUCCESS);
			}

			for (size_t frame = 0; frame < expectedGains.size(); ++frame)
			{
				for (ma_uint32 channel = 0; channel < channels; ++channel)
				{
					CAPTURE(frame, channel);
					CHECK(output[frame * channels + channel] == expectedGains[frame]);
				}
			}
		}
	}
}

TEST_CASE("Miniaudio.null_backend_enumeration")
{
	const ma_backend backend = ma_backend_null;
	ma_context context{};
	REQUIRE(ma_context_init(&backend, 1, nullptr, &context) == MA_SUCCESS);
	const ScopeExit cleanup{ [&] { ma_context_uninit(&context); } };
	CHECK(context.backend == ma_backend_null);

	// The null backend supplies one synthetic device in each direction.
	for (int iteration = 0; iteration < 2; ++iteration)
	{
		CAPTURE(iteration);
		ma_device_info* playbackDevices = nullptr;
		ma_device_info* captureDevices = nullptr;
		ma_uint32 playbackCount = 0;
		ma_uint32 captureCount = 0;
		REQUIRE(ma_context_get_devices(&context, &playbackDevices, &playbackCount, &captureDevices, &captureCount) == MA_SUCCESS);
		REQUIRE(playbackCount == 1);
		REQUIRE(captureCount == 1);
		REQUIRE(playbackDevices != nullptr);
		REQUIRE(captureDevices != nullptr);
		CHECK(playbackDevices[0].isDefault == MA_TRUE);
		CHECK(captureDevices[0].isDefault == MA_TRUE);
		CHECK(playbackDevices[0].name[0] != '\0');
		CHECK(captureDevices[0].name[0] != '\0');
	}
}
