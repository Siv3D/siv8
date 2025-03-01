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
# include "Optional.hpp"
# include "2DShapes.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	ScopedViewport2D
	//
	////////////////////////////////////////////////////////////////

	class ScopedViewport2D
	{
	public:

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		ScopedViewport2D() = default;

		[[nodiscard]]
		explicit ScopedViewport2D(const Optional<Rect>& viewport);

		[[nodiscard]]
		ScopedViewport2D(int32 w, int32 h);

		[[nodiscard]]
		ScopedViewport2D(Size size);

		[[nodiscard]]
		ScopedViewport2D(int32 x, int32 y, int32 w, int32 h);

		[[nodiscard]]
		ScopedViewport2D(Point pos, int32 w, int32 h);

		[[nodiscard]]
		ScopedViewport2D(int32 x, int32 y, Size size);

		[[nodiscard]]
		ScopedViewport2D(Point pos, Size size);

		[[nodiscard]]
		ScopedViewport2D(ScopedViewport2D&& other) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	(destructor)
		//
		////////////////////////////////////////////////////////////////

		~ScopedViewport2D();

		////////////////////////////////////////////////////////////////
		//
		//	(deleted)
		//
		////////////////////////////////////////////////////////////////

		ScopedViewport2D(const ScopedViewport2D&) = delete;

		ScopedViewport2D& operator =(const ScopedViewport2D&) = delete;

	private:

		Optional<Optional<Rect>> m_oldViewport;

		void clear() noexcept;
	};
}
