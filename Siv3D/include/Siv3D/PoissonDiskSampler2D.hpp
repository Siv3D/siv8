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
# include "Common.hpp"
# include "PointVector.hpp"
# include "Array.hpp"
# include "Grid.hpp"
# include "Optional.hpp"
# include "PredefinedYesNo.hpp"
# include "RandomVec2.hpp"

namespace s3d
{
	class PoissonDiskSampler2D
	{
	public:

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		PoissonDiskSampler2D() = default;

		[[nodiscard]]
		explicit PoissonDiskSampler2D(const Size& size, double r, Precompute precompute = Precompute::Yes);

		[[nodiscard]]
		explicit PoissonDiskSampler2D(const Size& size, double r, Concept::UniformRandomBitGenerator auto&& urbg, Precompute precompute = Precompute::Yes);

		[[nodiscard]]
		explicit PoissonDiskSampler2D(const Size& size, double r, const Vec2& initialPos, Precompute precompute = Precompute::Yes);

		[[nodiscard]]
		explicit PoissonDiskSampler2D(const Size& size, double r, const Vec2& initialPos, Concept::UniformRandomBitGenerator auto&& urbg, Precompute precompute = Precompute::Yes);

		[[nodiscard]]
		explicit PoissonDiskSampler2D(const Rect& rect, double r, Precompute precompute = Precompute::Yes);

		[[nodiscard]]
		explicit PoissonDiskSampler2D(const Rect& rect, double r, Concept::UniformRandomBitGenerator auto&& urbg, Precompute precompute = Precompute::Yes);

		[[nodiscard]]
		explicit PoissonDiskSampler2D(const Rect& rect, double r, const Vec2& initialPos, Precompute precompute = Precompute::Yes);

		[[nodiscard]]
		explicit PoissonDiskSampler2D(const Rect& rect, double r, const Vec2& initialPos, Concept::UniformRandomBitGenerator auto&& urbg, Precompute precompute = Precompute::Yes);

		////////////////////////////////////////////////////////////////
		//
		//	isComplete
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		bool isComplete() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	update
		//
		////////////////////////////////////////////////////////////////

		size_t update(size_t iterations = 100);

		size_t update(Concept::UniformRandomBitGenerator auto&& urbg, size_t iterations = 100);

		////////////////////////////////////////////////////////////////
		//
		//	getPoints
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		const Array<Vec2>& getPoints() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	getPoints
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Rect region() const noexcept;

	private:

		static constexpr size_t k = 30;

		Size m_size{ 0, 0 };

		Point m_offset{ 0, 0 };

		double m_r = 0.0;

		double m_cellSize = 0.0;

		Array<Vec2> m_activeList;

		Grid<Optional<Vec2>> m_grid;

		Array<Vec2> m_results;
	};
}

# include "detail/PoissonDiskSampler2D.ipp"
