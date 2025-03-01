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

# pragma once
# include <Siv3D/Common.hpp>

namespace s3d
{
	class SceneTime;

	class SIV3D_NOVTABLE ISiv3DScene
	{
	public:

		[[nodiscard]]
		static ISiv3DScene* Create();

		virtual ~ISiv3DScene() = default;

		virtual void update() = 0;

		[[nodiscard]]
		virtual SceneTime& getSceneTime() noexcept = 0;
	};
}
