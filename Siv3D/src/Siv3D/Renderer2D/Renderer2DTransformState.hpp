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

# pragma once
# include <Siv3D/Mat3x2.hpp>
# include <Siv3D/Mat3x3.hpp>

namespace s3d
{
	struct Renderer2DTransformState
	{
		Mat3x2 localTransform = Mat3x2::Identity();

		Mat3x2 cameraTransform = Mat3x2::Identity();

		Mat3x2 affineTransform = Mat3x2::Identity();

		Mat3x3 quadWarpTransform = Mat3x3::Identity();

		Mat3x3 combinedTransform = Mat3x3::Identity();

		float rmsScaling = 1.0f;

		Float2 sceneScale{ 1.0f, 1.0f };

		float rasterScaling = 1.0f;

		// Fold these calculations into the command manager without extra calls.
		SIV3D_ALWAYS_INLINE
		void updateAffineTransform() noexcept
		{
			affineTransform = (localTransform * cameraTransform);
			rmsScaling = affineTransform.rmsScaling();
		}

		SIV3D_ALWAYS_INLINE
		void updateRasterScaling() noexcept
		{
			rasterScaling = ((sceneScale == Float2{ 1.0f, 1.0f })
				? rmsScaling
				: (affineTransform * Mat3x2::Scale(sceneScale)).rmsScaling());
		}

		SIV3D_ALWAYS_INLINE
		void updateCombinedTransform() noexcept
		{
			combinedTransform = (Mat3x3{ affineTransform } * quadWarpTransform);
		}
	};
}
