//-----------------------------------------------
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//-----------------------------------------------
# include "Siv3DTest.hpp"

TEST_CASE("Wave.array_like_contract")
{
	const Wave source{ Array<StereoSampleFloat32>{ { 0.1f, 0.2f }, { 0.3f, 0.4f }, { 0.5f, 0.6f } }, Arg::sampleRate = 48000 };
	Wave wave = source;
	CHECK_EQ(wave.get_if(1), &wave[1]);
	CHECK_EQ(std::as_const(wave).get_if(1), &wave[1]);
	CHECK_EQ(wave.get_if(3), nullptr);
	const auto tail = wave.drop(1);
	CHECK_EQ(tail.size(), size_t{ 2 });
	CHECK_EQ(tail.sampleRate(), uint32{ 48000 });
	CHECK_EQ(tail[0].left, 0.3f);
	CHECK_EQ(tail[0].right, 0.4f);
	CHECK_EQ(wave.take(1).sampleRate(), uint32{ 48000 });
	CHECK_EQ(wave.take(100).size(), size_t{ 3 });
	CHECK_EQ(wave.slice(1).size(), size_t{ 2 });
	CHECK_EQ(wave.slice(1, 1).size(), size_t{ 1 });
	CHECK_EQ(wave.slice(1, 1)[0].right, 0.4f);
	CHECK_EQ(wave.slice(3).sampleRate(), uint32{ 48000 });
	CHECK_EQ(wave.drop(100).sampleRate(), uint32{ 48000 });
	CHECK(wave.drop(100).isEmpty());
	CHECK_THROWS_AS((void) wave.slice(4), std::out_of_range);
	CHECK_THROWS_AS((void) wave.slice(2, 2), std::out_of_range);
	const auto storage = wave.data();
	auto result = std::move(wave).drop(1).take(1);
	CHECK_EQ(result.data(), storage);
	CHECK_EQ(result.sampleRate(), uint32{ 48000 });
	CHECK_EQ(result.size(), size_t{ 1 });
	CHECK_EQ(result[0].left, 0.3f);
	CHECK_EQ(result[0].right, 0.4f);
}
