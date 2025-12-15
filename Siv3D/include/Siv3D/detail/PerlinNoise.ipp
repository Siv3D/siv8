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
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr BasicPerlinNoise<Float>::BasicPerlinNoise() noexcept
		: m_perm{ 151,160,137,91,90,15,
				131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
				190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
				88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
				77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
				102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
				135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
				5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
				223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
				129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
				251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
				49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
				138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180 } {}

	template <Concept::FloatingPoint Float>
	BasicPerlinNoise<Float>::BasicPerlinNoise(const uint64 seed)
	{
		reseed(seed);
	}

	template <Concept::FloatingPoint Float>
	BasicPerlinNoise<Float>::BasicPerlinNoise(Concept::UniformRandomBitGenerator auto&& urbg)
	{
		reseed(std::forward<decltype(urbg)>(urbg));
	}

	////////////////////////////////////////////////////////////////
	//
	//	reseed
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	void BasicPerlinNoise<Float>::reseed(const uint64 seed)
	{
		reseed(DefaultRNG{ seed });
	}

	template <Concept::FloatingPoint Float>
	void BasicPerlinNoise<Float>::reseed(Concept::UniformRandomBitGenerator auto&& urbg)
	{
		std::iota(m_perm.begin(), m_perm.end(), static_cast<uint8>(0));
		Shuffle(m_perm.begin(), m_perm.end(), std::forward<decltype(urbg)>(urbg));
	}

	////////////////////////////////////////////////////////////////
	//
	//	noise
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	BasicPerlinNoise<Float>::value_type BasicPerlinNoise<Float>::noise(const value_type x) const noexcept
	{
		return noise(x,
			static_cast<value_type>(0.34567890123456789012),
			static_cast<value_type>(0.12345678901234567890)); // arbitrary number for entropy
	}

	template <Concept::FloatingPoint Float>
	BasicPerlinNoise<Float>::value_type BasicPerlinNoise<Float>::noise(const value_type x, const value_type y) const noexcept
	{
		return noise(x,
			y,
			static_cast<value_type>(0.12345678901234567890)); // arbitrary number for entropy
	}

	template <Concept::FloatingPoint Float>
	BasicPerlinNoise<Float>::value_type BasicPerlinNoise<Float>::noise(const Vector2D<value_type> xy) const noexcept
	{
		return noise(xy.x, xy.y);
	}

	template <Concept::FloatingPoint Float>
	BasicPerlinNoise<Float>::value_type BasicPerlinNoise<Float>::noise(const value_type x, const value_type y, const value_type z) const noexcept
	{
		const value_type _x = std::floor(x);
		const value_type _y = std::floor(y);
		const value_type _z = std::floor(z);

		const int32 ix = static_cast<int32>(_x) & 255;
		const int32 iy = static_cast<int32>(_y) & 255;
		const int32 iz = static_cast<int32>(_z) & 255;

		const value_type fx = (x - _x);
		const value_type fy = (y - _y);
		const value_type fz = (z - _z);

		const value_type u = Fade(fx);
		const value_type v = Fade(fy);
		const value_type w = Fade(fz);

		const uint8 A = (m_perm[ix] + iy) & 255;
		const uint8 B = (m_perm[(ix + 1) & 255] + iy) & 255;

		const uint8 AA = (m_perm[A] + iz) & 255;
		const uint8 AB = (m_perm[(A + 1) & 255] + iz) & 255;

		const uint8 BA = (m_perm[B] + iz) & 255;
		const uint8 BB = (m_perm[(B + 1) & 255] + iz) & 255;

		const value_type p0 = Grad(m_perm[AA], fx, fy, fz);
		const value_type p1 = Grad(m_perm[BA], (fx - 1), fy, fz);
		const value_type p2 = Grad(m_perm[AB], fx, (fy - 1), fz);
		const value_type p3 = Grad(m_perm[BB], (fx - 1), (fy - 1), fz);
		const value_type p4 = Grad(m_perm[(AA + 1) & 255], fx, fy, (fz - 1));
		const value_type p5 = Grad(m_perm[(BA + 1) & 255], (fx - 1), fy, (fz - 1));
		const value_type p6 = Grad(m_perm[(AB + 1) & 255], fx, (fy - 1), (fz - 1));
		const value_type p7 = Grad(m_perm[(BB + 1) & 255], (fx - 1), (fy - 1), (fz - 1));

		const value_type q0 = Lerp(p0, p1, u);
		const value_type q1 = Lerp(p2, p3, u);
		const value_type q2 = Lerp(p4, p5, u);
		const value_type q3 = Lerp(p6, p7, u);

		const value_type r0 = Lerp(q0, q1, v);
		const value_type r1 = Lerp(q2, q3, v);

		return Lerp(r0, r1, w);
	}

	template <Concept::FloatingPoint Float>
	BasicPerlinNoise<Float>::value_type BasicPerlinNoise<Float>::noise(const Vector3D<value_type> xyz) const noexcept
	{
		return noise3D(xyz.x, xyz.y, xyz.z);
	}

	////////////////////////////////////////////////////////////////
	//
	//	noise01
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	BasicPerlinNoise<Float>::value_type BasicPerlinNoise<Float>::noise01(const value_type x) const noexcept
	{
		return To01(noise(x));
	}

	template <Concept::FloatingPoint Float>
	BasicPerlinNoise<Float>::value_type BasicPerlinNoise<Float>::noise01(const value_type x, const value_type y) const noexcept
	{
		return To01(noise(x, y));
	}

	template <Concept::FloatingPoint Float>
	BasicPerlinNoise<Float>::value_type BasicPerlinNoise<Float>::noise01(const Vector2D<value_type> xy) const noexcept
	{
		return noise01(xy.x, xy.y);
	}

	template <Concept::FloatingPoint Float>
	BasicPerlinNoise<Float>::value_type BasicPerlinNoise<Float>::noise01(const value_type x, const value_type y, const value_type z) const noexcept
	{
		return To01(noise(x, y, z));
	}

	template <Concept::FloatingPoint Float>
	BasicPerlinNoise<Float>::value_type BasicPerlinNoise<Float>::noise01(const Vector3D<value_type> xyz) const noexcept
	{
		return noise01(xyz.x, xyz.y, xyz.z);
	}

	////////////////////////////////////////////////////////////////
	//
	//	octaveNoise
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	BasicPerlinNoise<Float>::value_type BasicPerlinNoise<Float>::octaveNoise(value_type x, const Parameters parameters) const noexcept
	{
		value_type result = 0;
		value_type amplitude = 1;

		for (int32 i = 0; i < parameters.octaves; ++i)
		{
			result += (noise(x) * amplitude);
			x *= 2;
			amplitude *= parameters.persistence;
		}

		return result;
	}

	template <Concept::FloatingPoint Float>
	BasicPerlinNoise<Float>::value_type BasicPerlinNoise<Float>::octaveNoise(value_type x, value_type y, const Parameters parameters) const noexcept
	{
		value_type result = 0;
		value_type amplitude = 1;
		
		for (int32 i = 0; i < parameters.octaves; ++i)
		{
			result += (noise(x, y) * amplitude);
			x *= 2;
			y *= 2;
			amplitude *= parameters.persistence;
		}
		
		return result;
	}

	template <Concept::FloatingPoint Float>
	BasicPerlinNoise<Float>::value_type BasicPerlinNoise<Float>::octaveNoise(const Vector2D<value_type> xy, const Parameters parameters) const noexcept
	{
		return octaveNoise(xy.x, xy.y, parameters);
	}

	template <Concept::FloatingPoint Float>
	BasicPerlinNoise<Float>::value_type BasicPerlinNoise<Float>::octaveNoise(value_type x, value_type y, value_type z, const Parameters parameters) const noexcept
	{
		value_type result = 0;
		value_type amplitude = 1;
		
		for (int32 i = 0; i < parameters.octaves; ++i)
		{
			result += (noise(x, y, z) * amplitude);
			x *= 2;
			y *= 2;
			z *= 2;
			amplitude *= parameters.persistence;
		}
		
		return result;
	}

	template <Concept::FloatingPoint Float>
	BasicPerlinNoise<Float>::value_type BasicPerlinNoise<Float>::octaveNoise(const Vector3D<value_type> xyz, const Parameters parameters) const noexcept
	{
		return octaveNoise(xyz.x, xyz.y, xyz.z, parameters);
	}

	////////////////////////////////////////////////////////////////
	//
	//	octaveNoise01
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	BasicPerlinNoise<Float>::value_type BasicPerlinNoise<Float>::octaveNoise01(const value_type x, const Parameters parameters) const noexcept
	{
		return To01Clamped(octaveNoise(x, parameters));
	}

	template <Concept::FloatingPoint Float>
	BasicPerlinNoise<Float>::value_type BasicPerlinNoise<Float>::octaveNoise01(const value_type x, const value_type y, const Parameters parameters) const noexcept
	{
		return To01Clamped(octaveNoise(x, y, parameters));
	}

	template <Concept::FloatingPoint Float>
	BasicPerlinNoise<Float>::value_type BasicPerlinNoise<Float>::octaveNoise01(const Vector2D<value_type> xy, const Parameters parameters) const noexcept
	{
		return To01Clamped(octaveNoise(xy.x, xy.y, parameters));
	}

	template <Concept::FloatingPoint Float>
	BasicPerlinNoise<Float>::value_type BasicPerlinNoise<Float>::octaveNoise01(const value_type x, const value_type y, const value_type z, const Parameters parameters) const noexcept
	{
		return To01Clamped(octaveNoise(x, y, z, parameters));
	}

	template <Concept::FloatingPoint Float>
	BasicPerlinNoise<Float>::value_type BasicPerlinNoise<Float>::octaveNoise01(const Vector3D<value_type> xyz, const Parameters parameters) const noexcept
	{
		return To01Clamped(octaveNoise(xyz.x, xyz.y, xyz.z, parameters));
	}

	////////////////////////////////////////////////////////////////
	//
	//	octaveNoiseNormalized
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	BasicPerlinNoise<Float>::value_type BasicPerlinNoise<Float>::octaveNoiseNormalized(const value_type x, const Parameters parameters) const noexcept
	{
		if (parameters.octaves <= 0)
		{
			return 0;
		}

		return (octaveNoise(x, parameters) / AmplitudeSum(parameters));
	}

	template <Concept::FloatingPoint Float>
	BasicPerlinNoise<Float>::value_type BasicPerlinNoise<Float>::octaveNoiseNormalized(const value_type x, const value_type y, const Parameters parameters) const noexcept
	{
		if (parameters.octaves <= 0)
		{
			return 0;
		}
		
		return (octaveNoise(x, y, parameters) / AmplitudeSum(parameters));
	}

	template <Concept::FloatingPoint Float>
	BasicPerlinNoise<Float>::value_type BasicPerlinNoise<Float>::octaveNoiseNormalized(const Vector2D<value_type> xy, const Parameters parameters) const noexcept
	{
		return octaveNoiseNormalized(xy.x, xy.y, parameters);
	}

	template <Concept::FloatingPoint Float>
	BasicPerlinNoise<Float>::value_type BasicPerlinNoise<Float>::octaveNoiseNormalized(const value_type x, const value_type y, const value_type z, const Parameters parameters) const noexcept
	{
		if (parameters.octaves <= 0)
		{
			return 0;
		}
		
		return (octaveNoise(x, y, z, parameters) / AmplitudeSum(parameters));
	}

	template <Concept::FloatingPoint Float>
	BasicPerlinNoise<Float>::value_type BasicPerlinNoise<Float>::octaveNoiseNormalized(const Vector3D<value_type> xyz, const Parameters parameters) const noexcept
	{
		return octaveNoiseNormalized(xyz.x, xyz.y, xyz.z, parameters);
	}

	////////////////////////////////////////////////////////////////
	//
	//	octaveNoiseNormalized01
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	BasicPerlinNoise<Float>::value_type BasicPerlinNoise<Float>::octaveNoiseNormalized01(const value_type x, const Parameters parameters) const noexcept
	{
		return To01(octaveNoiseNormalized(x, parameters));
	}

	template <Concept::FloatingPoint Float>
	BasicPerlinNoise<Float>::value_type BasicPerlinNoise<Float>::octaveNoiseNormalized01(const value_type x, const value_type y, const Parameters parameters) const noexcept
	{
		return To01(octaveNoiseNormalized(x, y, parameters));
	}

	template <Concept::FloatingPoint Float>
	BasicPerlinNoise<Float>::value_type BasicPerlinNoise<Float>::octaveNoiseNormalized01(const Vector2D<value_type> xy, const Parameters parameters) const noexcept
	{
		return To01(octaveNoiseNormalized(xy.x, xy.y, parameters));
	}

	template <Concept::FloatingPoint Float>
	BasicPerlinNoise<Float>::value_type BasicPerlinNoise<Float>::octaveNoiseNormalized01(const value_type x, const value_type y, const value_type z, const Parameters parameters) const noexcept
	{
		return To01(octaveNoiseNormalized(x, y, z, parameters));
	}

	template <Concept::FloatingPoint Float>
	BasicPerlinNoise<Float>::value_type BasicPerlinNoise<Float>::octaveNoiseNormalized01(const Vector3D<value_type> xyz, const Parameters parameters) const noexcept
	{
		return To01(octaveNoiseNormalized(xyz.x, xyz.y, xyz.z, parameters));
	}

	////////////////////////////////////////////////////////////////
	//
	//	serialize, deserialize
	//
	////////////////////////////////////////////////////////////////
	
	template <Concept::FloatingPoint Float>
	constexpr const typename BasicPerlinNoise<Float>::state_type& BasicPerlinNoise<Float>::serialize() const noexcept
	{
		return m_perm;
	}

	template <Concept::FloatingPoint Float>
	constexpr void BasicPerlinNoise<Float>::deserialize(const state_type& state) noexcept
	{
		m_perm = state;
	}

	////////////////////////////////////////////////////////////////
	//
	//	(private function)
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr BasicPerlinNoise<Float>::value_type BasicPerlinNoise<Float>::Fade(const value_type t) noexcept
	{
		return (t * t * t * (t * (t * 6 - 15) + 10));
	}

	template <Concept::FloatingPoint Float>
	constexpr BasicPerlinNoise<Float>::value_type BasicPerlinNoise<Float>::Lerp(const value_type a, const value_type b, const value_type t) noexcept
	{
		return (a + (b - a) * t);
	}

	template <Concept::FloatingPoint Float>
	constexpr BasicPerlinNoise<Float>::value_type BasicPerlinNoise<Float>::Grad(const uint8 hash, const value_type x, const value_type y, const value_type z) noexcept
	{
		const uint8 h = (hash & 15);
		const value_type u = (h < 8 ? x : y);
		const value_type v = (h < 4 ? y : h == 12 || h == 14 ? x : z);
		return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
	}

	template <Concept::FloatingPoint Float>
	constexpr BasicPerlinNoise<Float>::value_type BasicPerlinNoise<Float>::To01(const value_type x) noexcept
	{
		return (x * value_type(0.5) + value_type(0.5));
	}

	template <Concept::FloatingPoint Float>
	constexpr BasicPerlinNoise<Float>::value_type BasicPerlinNoise<Float>::To01Clamped(const value_type x) noexcept
	{
		if (x <= value_type(-1.0))
		{
			return value_type(0.0);
		}
		else if (value_type(1.0) <= x)
		{
			return value_type(1.0);
		}

		return To01(x);
	}

	template <Concept::FloatingPoint Float>
	constexpr BasicPerlinNoise<Float>::value_type BasicPerlinNoise<Float>::AmplitudeSum(const Parameters parameters) noexcept
	{
		value_type sum = 0;
		value_type amplitude = 1;

		for (int32 i = 0; i < parameters.octaves; ++i)
		{
			sum += amplitude;
			amplitude *= parameters.persistence;
		}

		return sum;
	}
}
