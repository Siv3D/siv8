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
# include <algorithm>
# include <numeric>
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

		using value_type			= Float;
		using state_type			= std::array<uint8, 256>;
		using internal_state_type	= std::array<uint8, 512>;

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
		//	batchNoise
		//
		////////////////////////////////////////////////////////////////

		template <class OutputIterator, class Converter>
		void batchNoise(OutputIterator first, int32 width, int32 height,
			value_type baseX, value_type baseY, value_type xStep, value_type yStep, value_type z, Converter convert) const;

		template <class OutputIterator, class Converter>
		void batchNoise(OutputIterator first, Size size,
			value_type baseX, value_type baseY, value_type xStep, value_type yStep, value_type z, Converter convert) const;

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
		//	batchNoise01
		//
		////////////////////////////////////////////////////////////////

		template <class OutputIterator, class Converter>
		void batchNoise01(OutputIterator first, int32 width, int32 height,
			value_type baseX, value_type baseY, value_type xStep, value_type yStep, value_type z, Converter convert) const;

		template <class OutputIterator, class Converter>
		void batchNoise01(OutputIterator first, Size size,
			value_type baseX, value_type baseY, value_type xStep, value_type yStep, value_type z, Converter convert) const;

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
		//	batchOctaveNoise
		//
		////////////////////////////////////////////////////////////////

		template <class OutputIterator, class Converter>
		void batchOctaveNoise(OutputIterator first, int32 width, int32 height,
			value_type baseX, value_type baseY, value_type xStep, value_type yStep, value_type z, Parameters parameters, Converter convert) const;

		template <class OutputIterator, class Converter>
		void batchOctaveNoise(OutputIterator first, Size size,
			value_type baseX, value_type baseY, value_type xStep, value_type yStep, value_type z, Parameters parameters, Converter convert) const;

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
		//	batchOctaveNoise01
		//
		////////////////////////////////////////////////////////////////

		template <class OutputIterator, class Converter>
		void batchOctaveNoise01(OutputIterator first, int32 width, int32 height,
			value_type baseX, value_type baseY, value_type xStep, value_type yStep, value_type z, Parameters parameters, Converter convert) const;

		template <class OutputIterator, class Converter>
		void batchOctaveNoise01(OutputIterator first, Size size,
			value_type baseX, value_type baseY, value_type xStep, value_type yStep, value_type z, Parameters parameters, Converter convert) const;

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
		//	batchOctaveNoiseNormalized
		//
		////////////////////////////////////////////////////////////////

		template <class OutputIterator, class Converter>
		void batchOctaveNoiseNormalized(OutputIterator first, int32 width, int32 height,
			value_type baseX, const value_type baseY, value_type xStep, value_type yStep, value_type z, Parameters parameters, Converter convert) const;

		template <class OutputIterator, class Converter>
		void batchOctaveNoiseNormalized(OutputIterator first, Size size,
			value_type baseX, value_type baseY, value_type xStep, value_type yStep, value_type z, Parameters parameters, Converter convert) const;

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
		//	batchOctaveNoiseNormalized01
		//
		////////////////////////////////////////////////////////////////

		template <class OutputIterator, class Converter>
		void batchOctaveNoiseNormalized01(OutputIterator first, int32 width, int32 height,
			value_type baseX, value_type baseY, value_type xStep, value_type yStep, value_type z, Parameters parameters, Converter convert) const;

		template <class OutputIterator, class Converter>
		void batchOctaveNoiseNormalized01(OutputIterator first, Size size,
			value_type baseX, value_type baseY, value_type xStep, value_type yStep, value_type z, Parameters parameters, Converter convert) const;

		////////////////////////////////////////////////////////////////
		//
		//	serialize, deserialize
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr const state_type& serialize() const noexcept;

		constexpr void deserialize(const state_type& state) noexcept;

	private:

		static constexpr value_type kGradients[16][3] = {
			{ 1, 1, 0}, {-1, 1, 0}, { 1,-1, 0}, {-1,-1, 0},
			{ 1, 0, 1}, {-1, 0, 1}, { 1, 0,-1}, {-1, 0,-1},
			{ 0, 1, 1}, { 0,-1, 1}, { 0, 1,-1}, { 0,-1,-1},
			{ 1, 1, 0}, { 0,-1, 1}, {-1, 1, 0}, { 0,-1,-1}
		};

		internal_state_type m_perm;

		static constexpr value_type Fade(value_type t) noexcept;

		static constexpr value_type Lerp(value_type a, value_type b, value_type t) noexcept;

		static constexpr value_type Grad(uint8 hash, value_type x, value_type y, value_type z) noexcept;

		static constexpr value_type GradFast(uint8 hash, value_type x, value_type y, value_type z) noexcept;

		static constexpr value_type To01(value_type x) noexcept;

		static constexpr value_type To01Clamped(value_type x) noexcept;

		static constexpr value_type AmplitudeSum(Parameters parameters) noexcept;
	};

	using PerlinNoiseF = BasicPerlinNoise<float>;
	using PerlinNoise = BasicPerlinNoise<double>;
}

# include "detail/PerlinNoise.ipp"
