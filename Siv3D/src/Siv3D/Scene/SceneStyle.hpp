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
# include <Siv3D/Common.hpp>
# include <Siv3D/Scene.hpp>

namespace s3d
{
	struct SceneStyle
	{
		ColorF letterboxColor		= Scene::DefaultLetterboxColor;

		ColorF backgroundColor		= Scene::DefaultBackgroundColor;

		void beginFrame() noexcept
		{
			m_frameBackgroundColor = backgroundColor;
		}

		[[nodiscard]]
		const ColorF& getFrameBackgroundColor() const noexcept
		{
			return m_frameBackgroundColor;
		}

	private:

		// サイズ・MSAA の変更で再クリアするときも、フレーム開始時の色を使う。
		ColorF m_frameBackgroundColor = Scene::DefaultBackgroundColor;
	};
}
