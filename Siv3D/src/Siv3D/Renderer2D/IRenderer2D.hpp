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
# include <Siv3D/Common.hpp>
# include <Siv3D/PointVector.hpp>

namespace s3d
{
	struct FloatRect;

	class SIV3D_NOVTABLE ISiv3DRenderer2D
	{
	public:

		[[nodiscard]]
		static ISiv3DRenderer2D* Create();

		virtual ~ISiv3DRenderer2D() = default;

		virtual void init() = 0;

		virtual void addTriangle(const Float2(&points)[3], const Float4& color) = 0;

		virtual void addRect(const FloatRect& rect, const Float4& color) = 0;

		virtual void flush() = 0;
	};
}
