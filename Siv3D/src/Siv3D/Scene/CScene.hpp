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
# include "IScene.hpp"
# include "SceneTime.hpp"

namespace s3d
{
	class CScene final : public ISiv3DScene
	{
	public:

		void update() override;

		SceneTime& getSceneTime() noexcept override;

	private:

		SceneTime m_sceneTime;
	};
}
