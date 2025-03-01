//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2025 Ryo Suzuki
//	Copyright (c) 2016-2025 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	Reseed
	//
	////////////////////////////////////////////////////////////////

	template <class SeedSequence>
	void Reseed(SeedSequence&& seedSequence) noexcept
	{
		GetDefaultRNG().seed(std::forward<SeedSequence>(seedSequence));
	}

	////////////////////////////////////////////////////////////////
	//
	//	Random
	//
	////////////////////////////////////////////////////////////////

	double Random(Concept::UniformRandomBitGenerator auto&& urbg) noexcept
	{
		if constexpr (std::is_same_v<std::decay_t<decltype(urbg)>, DefaultRNG>)
		{
			return urbg.generateReal();
		}
		else
		{
			return RandomClosedOpen(0.0, 1.0, std::forward<decltype(urbg)>(urbg));
		}
	}

	template <Concept::Arithmetic Arithmetic>
	Arithmetic Random(const Arithmetic min, const Arithmetic max)
	{
		if constexpr (std::is_integral_v<Arithmetic>)
		{
			return absl::Uniform(absl::IntervalClosed, GetDefaultRNG(), min, max);
		}
		else
		{
			return absl::Uniform(absl::IntervalClosedOpen, GetDefaultRNG(), min, max);
		}
	}

	template <Concept::Arithmetic Arithmetic>
	Arithmetic Random(const Arithmetic min, const Arithmetic max, Concept::UniformRandomBitGenerator auto&& urbg)
	{
		if constexpr (std::is_integral_v<Arithmetic>)
		{
			return absl::Uniform(absl::IntervalClosed, std::forward<decltype(urbg)>(urbg), min, max);
		}
		else
		{
			return absl::Uniform(absl::IntervalClosedOpen, std::forward<decltype(urbg)>(urbg), min, max);
		}
	}

	template <Concept::Arithmetic Arithmetic>
	Arithmetic Random(const Arithmetic max)
	{
		return Random(max, GetDefaultRNG());
	}

	template <Concept::Arithmetic Arithmetic>
	Arithmetic Random(const Arithmetic max, Concept::UniformRandomBitGenerator auto&& urbg)
	{
		return Random<Arithmetic>(0, max, std::forward<decltype(urbg)>(urbg));
	}

	////////////////////////////////////////////////////////////////
	//
	//	RandomOpen
	//
	////////////////////////////////////////////////////////////////

	template <Concept::Arithmetic Arithmetic>
	Arithmetic RandomOpen(const Arithmetic min, const Arithmetic max)
	{
		return absl::Uniform(absl::IntervalOpen, GetDefaultRNG(), min, max);
	}

	template <Concept::Arithmetic Arithmetic>
	Arithmetic RandomOpen(const Arithmetic min, const Arithmetic max, Concept::UniformRandomBitGenerator auto&& urbg)
	{
		return absl::Uniform(absl::IntervalOpen, std::forward<decltype(urbg)>(urbg), min, max);
	}

	////////////////////////////////////////////////////////////////
	//
	//	RandomClosed
	//
	////////////////////////////////////////////////////////////////

	template <Concept::Arithmetic Arithmetic>
	Arithmetic RandomClosed(const Arithmetic min, const Arithmetic max)
	{
		return absl::Uniform(absl::IntervalClosed, GetDefaultRNG(), min, max);
	}

	template <Concept::Arithmetic Arithmetic>
	Arithmetic RandomClosed(const Arithmetic min, const Arithmetic max, Concept::UniformRandomBitGenerator auto&& urbg)
	{
		return absl::Uniform(absl::IntervalClosed, std::forward<decltype(urbg)>(urbg), min, max);
	}

	////////////////////////////////////////////////////////////////
	//
	//	RandomOpenClosed
	//
	////////////////////////////////////////////////////////////////

	template <Concept::Arithmetic Arithmetic>
	Arithmetic RandomOpenClosed(const Arithmetic min, const Arithmetic max)
	{
		return absl::Uniform(absl::IntervalOpenClosed, GetDefaultRNG(), min, max);
	}

	template <Concept::Arithmetic Arithmetic>
	Arithmetic RandomOpenClosed(const Arithmetic min, const Arithmetic max, Concept::UniformRandomBitGenerator auto&& urbg)
	{
		return absl::Uniform(absl::IntervalOpenClosed, std::forward<decltype(urbg)>(urbg), min, max);
	}

	////////////////////////////////////////////////////////////////
	//
	//	RandomClosedOpen
	//
	////////////////////////////////////////////////////////////////

	template <Concept::Arithmetic Arithmetic>
	Arithmetic RandomClosedOpen(const Arithmetic min, const Arithmetic max)
	{
		return absl::Uniform(absl::IntervalClosedOpen, GetDefaultRNG(), min, max);
	}

	template <Concept::Arithmetic Arithmetic>
	Arithmetic RandomClosedOpen(const Arithmetic min, const Arithmetic max, Concept::UniformRandomBitGenerator auto&& urbg)
	{
		return absl::Uniform(absl::IntervalClosedOpen, std::forward<decltype(urbg)>(urbg), min, max);
	}

	////////////////////////////////////////////////////////////////
	//
	//	RandomBool
	//
	////////////////////////////////////////////////////////////////

	inline bool RandomBool(const double p) noexcept
	{
		return absl::Bernoulli(GetDefaultRNG(), p);
	}

	bool RandomBool(const double p, Concept::UniformRandomBitGenerator auto&& urbg) noexcept
	{
		return absl::Bernoulli(std::forward<decltype(urbg)>(urbg), p);
	}

	////////////////////////////////////////////////////////////////
	//
	//	RandomUint8
	//
	////////////////////////////////////////////////////////////////

	inline uint8 RandomUint8()
	{
		return absl::Uniform<uint8>(GetDefaultRNG());
	}

	uint8 RandomUint8(Concept::UniformRandomBitGenerator auto&& urbg)
	{
		return absl::Uniform<uint8>(std::forward<decltype(urbg)>(urbg));
	}

	////////////////////////////////////////////////////////////////
	//
	//	RandomUint16
	//
	////////////////////////////////////////////////////////////////

	inline uint16 RandomUint16()
	{
		return absl::Uniform<uint16>(GetDefaultRNG());
	}

	uint16 RandomUint16(Concept::UniformRandomBitGenerator auto&& urbg)
	{
		return absl::Uniform<uint16>(std::forward<decltype(urbg)>(urbg));
	}

	////////////////////////////////////////////////////////////////
	//
	//	RandomUint32
	//
	////////////////////////////////////////////////////////////////

	inline uint32 RandomUint32()
	{
		return absl::Uniform<uint32>(GetDefaultRNG());
	}

	uint32 RandomUint32(Concept::UniformRandomBitGenerator auto&& urbg)
	{
		return absl::Uniform<uint32>(std::forward<decltype(urbg)>(urbg));
	}

	////////////////////////////////////////////////////////////////
	//
	//	RandomUint64
	//
	////////////////////////////////////////////////////////////////

	inline uint64 RandomUint64()
	{
		return absl::Uniform<uint64>(GetDefaultRNG());
	}

	uint64 RandomUint64(Concept::UniformRandomBitGenerator auto&& urbg)
	{
		return absl::Uniform<uint64>(std::forward<decltype(urbg)>(urbg));
	}

	////////////////////////////////////////////////////////////////
	//
	//	RandomInt8
	//
	////////////////////////////////////////////////////////////////

	inline int8 RandomInt8()
	{
		return RandomInt8(GetDefaultRNG());
	}

	int8 RandomInt8(Concept::UniformRandomBitGenerator auto&& urbg)
	{
		return absl::Uniform(absl::IntervalClosed, std::forward<decltype(urbg)>(urbg), Smallest<int8>, Largest<int8>);
	}

	////////////////////////////////////////////////////////////////
	//
	//	RandomInt16
	//
	////////////////////////////////////////////////////////////////

	inline int16 RandomInt16()
	{
		return RandomInt16(GetDefaultRNG());
	}

	int16 RandomInt16(Concept::UniformRandomBitGenerator auto&& urbg)
	{
		return absl::Uniform(absl::IntervalClosed, std::forward<decltype(urbg)>(urbg), Smallest<int16>, Largest<int16>);
	}

	////////////////////////////////////////////////////////////////
	//
	//	RandomInt32
	//
	////////////////////////////////////////////////////////////////

	inline int32 RandomInt32()
	{
		return RandomInt32(GetDefaultRNG());
	}

	int32 RandomInt32(Concept::UniformRandomBitGenerator auto&& urbg)
	{
		return absl::Uniform(absl::IntervalClosed, std::forward<decltype(urbg)>(urbg), Smallest<int32>, Largest<int32>);
	}

	////////////////////////////////////////////////////////////////
	//
	//	RandomInt64
	//
	////////////////////////////////////////////////////////////////

	inline int64 RandomInt64()
	{
		return RandomInt64(GetDefaultRNG());
	}

	int64 RandomInt64(Concept::UniformRandomBitGenerator auto&& urbg)
	{
		return absl::Uniform(absl::IntervalClosed, std::forward<decltype(urbg)>(urbg), Smallest<int64>, Largest<int64>);
	}
}
