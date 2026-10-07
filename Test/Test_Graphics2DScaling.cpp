//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2026 Ryo Suzuki
//	Copyright (c) 2016-2026 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include "Siv3DTest.hpp"
# include <cmath>

TEST_CASE("Graphics2D::GetRMSScaling")
{
	const Mat3x2 previousLocal = Graphics2D::GetLocalTransform();
	const Mat3x2 previousCamera = Graphics2D::GetCameraTransform();
	const float previousScaling = Graphics2D::GetRMSScaling();
	{
		const Transformer2D camera{ Mat3x2::Identity(), Transformer2D::Target::SetCamera };
		const Transformer2D local{ Mat3x2::Identity(), Transformer2D::Target::SetLocal };
		CHECK(Graphics2D::GetRMSScaling() == 1.0f);
		{
			const Transformer2D anisotropic{ Mat3x2::Scale(4.0f, 1.0f), Transformer2D::Target::SetLocal };
			CHECK(std::abs(Graphics2D::GetRMSScaling() - std::sqrt(8.5f)) < 1e-5f);
			CHECK(std::abs(QualityFactor::FromSceneScaling(2.0).value() - 2.0 * std::sqrt(8.5)) < 1e-5);
			CHECK(QualityFactor::FromSceneScaling(-1.0).value() == 0.0);
			{
				// Compose matrices before measuring: these anisotropic scales cancel.
				const Transformer2D inverseCamera{ Mat3x2::Scale(0.25f, 1.0f), Transformer2D::Target::SetCamera };
				CHECK(Graphics2D::GetRMSScaling() == 1.0f);
			}
			CHECK(std::abs(Graphics2D::GetRMSScaling() - std::sqrt(8.5f)) < 1e-5f);
			{
				const Transformer2D rotation{ Mat3x2::Rotate(45_deg), Transformer2D::Target::PushLocal };
				CHECK(std::abs(Graphics2D::GetRMSScaling() - std::sqrt(8.5f)) < 1e-5f);
			}
		}
		CHECK(Graphics2D::GetRMSScaling() == 1.0f);
		{
			const Transformer2D zero{ Mat3x2::Scale(0.0f), Transformer2D::Target::SetLocal };
			CHECK(Graphics2D::GetRMSScaling() == 0.0f);
			CHECK(QualityFactor::FromSceneScaling().value() == 0.0);
		}
		CHECK(Graphics2D::GetRMSScaling() == 1.0f);
	}
	CHECK(Graphics2D::GetLocalTransform() == previousLocal);
	CHECK(Graphics2D::GetCameraTransform() == previousCamera);
	CHECK(Graphics2D::GetRMSScaling() == previousScaling);
}
