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

# include <Siv3D/QualityFactor.hpp>
# include <Siv3D/Graphics2D.hpp>

namespace s3d
{
	QualityFactor QualityFactor::FromSceneScaling(const double s) noexcept
	{
		return QualityFactor{ Graphics2D::GetMaxScaling() * s };
	}
}
