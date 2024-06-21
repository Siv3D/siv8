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
	class ScopedColorMul2D
	{
	public:

		[[nodiscard]]
		ScopedColorMul2D() = default;

		[[nodiscard]]
		explicit ScopedColorMul2D(const ColorF& color);

		[[nodiscard]]
		explicit ScopedColorMul2D(const Float4& color);

		[[nodiscard]]
		explicit ScopedColorMul2D(const Float3& rgb, double a = 1.0);

		[[nodiscard]]
		explicit ScopedColorMul2D(double rgb, double a = 1.0);

		[[nodiscard]]
		ScopedColorMul2D(double r, double g, double b, double a = 1.0);

		[[nodiscard]]
		ScopedColorMul2D(ScopedColorMul2D&& other) noexcept;

		ScopedColorMul2D(const ScopedColorMul2D&) = delete;

		~ScopedColorMul2D();

		ScopedColorMul2D& operator =(const ScopedColorMul2D&) = delete;

	private:

		Optional<Float4> m_oldColor;

		void clear() noexcept;
	};
}
