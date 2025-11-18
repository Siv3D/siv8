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
# include <array>
# include <cmath>
# include "Common.hpp"
# include "Concepts.hpp"
# include "PointVector.hpp"
# include "Random.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	BasicPerlinNoise
	//
	////////////////////////////////////////////////////////////////

	/// @brief Perlin Noise 生成器
	/// @tparam Float 出力結果の型
	template <Concept::FloatingPoint Float>
	class BasicPerlinNoise
	{
	public:

		using value_type = Float;
		using state_type = std::array<uint8, 256>;

		struct Parameters
		{
			int32 octaves = 1;
			
			value_type persistence = value_type(0.5);
		};

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr BasicPerlinNoise() noexcept;

		[[nodiscard]]
		explicit BasicPerlinNoise(uint64 seed);

		[[nodiscard]]
		explicit BasicPerlinNoise(Concept::UniformRandomBitGenerator auto&& urbg);

		////////////////////////////////////////////////////////////////
		//
		//	reseed
		//
		////////////////////////////////////////////////////////////////

		void reseed(uint64 seed);

		void reseed(Concept::UniformRandomBitGenerator auto&& urbg);

		////////////////////////////////////////////////////////////////
		//
		//	noise
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		value_type noise(value_type x) const noexcept;

		[[nodiscard]]
		value_type noise(value_type x, value_type y) const noexcept;

		[[nodiscard]]
		value_type noise(Vector2D<value_type> xy) const noexcept;

		[[nodiscard]]
		value_type noise(value_type x, value_type y, value_type z) const noexcept;

		[[nodiscard]]
		value_type noise(Vector3D<value_type> xyz) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	noise01
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		value_type noise01(value_type x) const noexcept;

		[[nodiscard]]
		value_type noise01(value_type x, value_type y) const noexcept;

		[[nodiscard]]
		value_type noise01(Vector2D<value_type> xy) const noexcept;

		[[nodiscard]]
		value_type noise01(value_type x, value_type y, value_type z) const noexcept;

		[[nodiscard]]
		value_type noise01(Vector3D<value_type> xyz) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	octaveNoise
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		value_type octaveNoise(value_type x, Parameters parameters) const noexcept;

		[[nodiscard]]
		value_type octaveNoise(value_type x, value_type y, Parameters parameters) const noexcept;

		[[nodiscard]]
		value_type octaveNoise(Vector2D<value_type> xy, Parameters parameters) const noexcept;

		[[nodiscard]]
		value_type octaveNoise(value_type x, value_type y, value_type z, Parameters parameters) const noexcept;

		[[nodiscard]]
		value_type octaveNoise(Vector3D<value_type> xyz, Parameters parameters) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	octaveNoise01
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		value_type octaveNoise01(value_type x, Parameters parameters) const noexcept;

		[[nodiscard]]
		value_type octaveNoise01(value_type x, value_type y, Parameters parameters) const noexcept;

		[[nodiscard]]
		value_type octaveNoise01(Vector2D<value_type> xy, Parameters parameters) const noexcept;

		[[nodiscard]]
		value_type octaveNoise01(value_type x, value_type y, value_type z, Parameters parameters) const noexcept;

		[[nodiscard]]
		value_type octaveNoise01(Vector3D<value_type> xyz, Parameters parameters) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	octaveNoiseNormalized
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		value_type octaveNoiseNormalized(value_type x, Parameters parameters) const noexcept;

		[[nodiscard]]
		value_type octaveNoiseNormalized(value_type x, value_type y, Parameters parameters) const noexcept;

		[[nodiscard]]
		value_type octaveNoiseNormalized(Vector2D<value_type> xy, Parameters parameters) const noexcept;

		[[nodiscard]]
		value_type octaveNoiseNormalized(value_type x, value_type y, value_type z, Parameters parameters) const noexcept;

		[[nodiscard]]
		value_type octaveNoiseNormalized(Vector3D<value_type> xyz, Parameters parameters) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	octaveNoiseNormalized01
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		value_type octaveNoiseNormalized01(value_type x, Parameters parameters) const noexcept;

		[[nodiscard]]
		value_type octaveNoiseNormalized01(value_type x, value_type y, Parameters parameters) const noexcept;

		[[nodiscard]]
		value_type octaveNoiseNormalized01(Vector2D<value_type> xy, Parameters parameters) const noexcept;

		[[nodiscard]]
		value_type octaveNoiseNormalized01(value_type x, value_type y, value_type z, Parameters parameters) const noexcept;

		[[nodiscard]]
		value_type octaveNoiseNormalized01(Vector3D<value_type> xyz, Parameters parameters) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	serialize, deserialize
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr const state_type& serialize() const noexcept;

		constexpr void deserialize(const state_type& state) noexcept;

	private:

		state_type m_perm;

		static constexpr value_type Fade(value_type t) noexcept;

		static constexpr value_type Lerp(value_type a, value_type b, value_type t) noexcept;

		static constexpr value_type Grad(uint8 hash, value_type x, value_type y, value_type z) noexcept;

		static constexpr value_type To01(value_type x) noexcept;

		static constexpr value_type To01Clamped(value_type x) noexcept;

		static constexpr value_type AmplitudeSum(Parameters parameters) noexcept;
	};

	using PerlinNoiseF = BasicPerlinNoise<float>;
	using PerlinNoise = BasicPerlinNoise<double>;
}

# include "detail/PerlinNoise.ipp"
