﻿//-----------------------------------------------
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
# include <Siv3D/Vertex2D.hpp>
# include <Siv3D/FloatRect.hpp>
# include <Siv3D/FunctionRef.hpp>
# include "Vertex2DBufferPointer.hpp"
# include "RectFrameColorType.hpp"

namespace s3d
{
	using BufferCreatorFunc = FunctionRef<Vertex2DBufferPointer(Vertex2D::IndexType, Vertex2D::IndexType)>;
	struct LineStyle;
	struct FloatQuad;

	namespace Vertex2DBuilder
	{
		[[nodiscard]]
		Vertex2D::IndexType BuildLine(const BufferCreatorFunc& bufferCreator, const LineStyle& style, const Float2& start, const Float2& end, float thickness, const Float4(&colors)[2], float scale);

		[[nodiscard]]
		Vertex2D::IndexType BuildCappedLine(const BufferCreatorFunc& bufferCreator, const Float2& start, const Float2& end, float thickness, const Float4(&colors)[2]);

		[[nodiscard]]
		Vertex2D::IndexType BuildUncappedLine(const BufferCreatorFunc& bufferCreator, const Float2& start, const Float2& end, float thickness, const Float4(&colors)[2]);

		[[nodiscard]]
		Vertex2D::IndexType BuildTriangle(const BufferCreatorFunc& bufferCreator, const Float2(&points)[3], const Float4& color);

		[[nodiscard]]
		Vertex2D::IndexType BuildTriangle(const BufferCreatorFunc& bufferCreator, const Float2(&points)[3], const Float4(&colors)[3]);

		[[nodiscard]]
		Vertex2D::IndexType BuildRect(const BufferCreatorFunc& bufferCreator, const FloatRect& rect, const Float4& color);

		[[nodiscard]]
		Vertex2D::IndexType BuildRect(const BufferCreatorFunc& bufferCreator, const FloatRect& rect, const Float4(&colors)[4]);

		[[nodiscard]]
		Vertex2D::IndexType BuildRectFrame(const BufferCreatorFunc& bufferCreator, const FloatRect& innerRect, float thickness, RectFrameColorType colorType, const Float4& color0, const Float4& color1);

		[[nodiscard]]
		Vertex2D::IndexType BuildCircle(const BufferCreatorFunc& bufferCreator, const Float2& center, float r, const Float4& innerColor, const Float4& outerColor, float scale);

		[[nodiscard]]
		Vertex2D::IndexType BuildCircleFrame(const BufferCreatorFunc& bufferCreator, const Float2& center, float rInner, float thickness, const Float4& innerColor, const Float4& outerColor, float scale);

		[[nodiscard]]
		Vertex2D::IndexType BuildQuad(const BufferCreatorFunc& bufferCreator, const FloatQuad& quad, const Float4 color);

		[[nodiscard]]
		Vertex2D::IndexType BuildQuad(const BufferCreatorFunc& bufferCreator, const FloatQuad& quad, const Float4(&colors)[4]);
	}
}
