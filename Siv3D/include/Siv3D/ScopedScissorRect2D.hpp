//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2024 Ryo Suzuki
//	Copyright (c) 2016-2024 OpenSiv3D Project
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
	//	ScopedScissorRect2D
	//
	////////////////////////////////////////////////////////////////

	class ScopedScissorRect2D
	{
	public:

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		ScopedScissorRect2D() = default;

		[[nodiscard]]
		explicit ScopedScissorRect2D(None_t);

		[[nodiscard]]
		explicit ScopedScissorRect2D(const Optional<Rect>& scissorRect);

		[[nodiscard]]
		ScopedScissorRect2D(int32 w, int32 h);

		[[nodiscard]]
		ScopedScissorRect2D(Size size);

		[[nodiscard]]
		ScopedScissorRect2D(int32 x, int32 y, int32 w, int32 h);

		[[nodiscard]]
		ScopedScissorRect2D(Point pos, int32 w, int32 h);

		[[nodiscard]]
		ScopedScissorRect2D(int32 x, int32 y, Size size);

		[[nodiscard]]
		ScopedScissorRect2D(Point pos, Size size);

		[[nodiscard]]
		ScopedScissorRect2D(ScopedScissorRect2D&& other) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	(destructor)
		//
		////////////////////////////////////////////////////////////////

		~ScopedScissorRect2D();

		////////////////////////////////////////////////////////////////
		//
		//	(deleted)
		//
		////////////////////////////////////////////////////////////////

		ScopedScissorRect2D(const ScopedScissorRect2D&) = delete;

		ScopedScissorRect2D& operator =(const ScopedScissorRect2D&) = delete;

	private:

		Optional<Optional<Rect>> m_oldScissorRect;

		void clear() noexcept;
	};
}
