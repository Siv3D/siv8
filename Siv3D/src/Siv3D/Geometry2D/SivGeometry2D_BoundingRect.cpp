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

# include <Siv3D/Geometry2D/BoundingRect.hpp>

namespace s3d
{
	namespace Geometry2D
	{
		////////////////////////////////////////////////////////////////
		//
		//	BoundingRect
		//
		////////////////////////////////////////////////////////////////

		Rect BoundingRect(std::span<const Point> points) noexcept
		{
			if (points.empty())
			{
				return Rect::Empty();
			}

			const Point* it = points.data();
			const Point* itEnd = (it + points.size());
			int32 left = it->x;
			int32 top = it->y;
			int32 right = left;
			int32 bottom = top;
			++it;

			while (it != itEnd)
			{
				if (it->x < left)
				{
					left = it->x;
				}
				else if (right < it->x)
				{
					right = it->x;
				}

				if (it->y < top)
				{
					top = it->y;
				}
				else if (bottom < it->y)
				{
					bottom = it->y;
				}

				++it;
			}

			return{ left, top, (right - left), (bottom - top) };
		}

		RectF BoundingRect(std::span<const Float2> points) noexcept
		{
			if (points.empty())
			{
				return RectF::Empty();
			}

			const Float2* it = points.data();
			const Float2* itEnd = (it + points.size());
			float left = it->x;
			float top = it->y;
			float right = left;
			float bottom = top;
			++it;

			while (it != itEnd)
			{
				if (it->x < left)
				{
					left = it->x;
				}
				else if (right < it->x)
				{
					right = it->x;
				}

				if (it->y < top)
				{
					top = it->y;
				}
				else if (bottom < it->y)
				{
					bottom = it->y;
				}

				++it;
			}

			return{ left, top, (right - left), (bottom - top) };
		}

		RectF BoundingRect(std::span<const Vec2> points) noexcept
		{
			if (points.empty())
			{
				return RectF::Empty();
			}

			const Vec2* it = points.data();
			const Vec2* itEnd = (it + points.size());
			double left = it->x;
			double top = it->y;
			double right = left;
			double bottom = top;
			++it;

			while (it != itEnd)
			{
				if (it->x < left)
				{
					left = it->x;
				}
				else if (right < it->x)
				{
					right = it->x;
				}

				if (it->y < top)
				{
					top = it->y;
				}
				else if (bottom < it->y)
				{
					bottom = it->y;
				}

				++it;
			}

			return{ left, top, (right - left), (bottom - top) };
		}
	}
}
