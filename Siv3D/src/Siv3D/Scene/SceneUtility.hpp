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

# include <Siv3D/Common.hpp>
# include <Siv3D/PointVector.hpp>
# include <Siv3D/2DShapes.hpp>

namespace s3d
{
	namespace SceneMisc
	{
		[[nodiscard]]
		std::pair<double, RectF> CalculateLetterboxComposition(const SizeF& backBufferSize, const SizeF& sceneSize);
	}
}
