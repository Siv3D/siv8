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
# include "PointVector.hpp"
# include "ColorHSV.hpp"

namespace s3d
{
	class ScopedColorAdd2D
	{
	public:

		[[nodiscard]]
		ScopedColorAdd2D() = default;

		[[nodiscard]]
		explicit ScopedColorAdd2D(const ColorF& rgb);

		[[nodiscard]]
		explicit ScopedColorAdd2D(const Float4& rgb);

		[[nodiscard]]
		explicit ScopedColorAdd2D(const Float3& rgb);

		[[nodiscard]]
		explicit ScopedColorAdd2D(double rgb);

		[[nodiscard]]
		ScopedColorAdd2D(double r, double g, double b);

		[[nodiscard]]
		ScopedColorAdd2D(ScopedColorAdd2D&& other) noexcept;

		ScopedColorAdd2D(const ScopedColorAdd2D&) = delete;

		~ScopedColorAdd2D();

		ScopedColorAdd2D& operator =(const ScopedColorAdd2D&) = delete;

	private:

		Optional<Float3> m_oldColor;

		void clear() noexcept;
	};
}
