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

# include <Siv3D/Scene.hpp>
# include "IScene.hpp"
# include "SceneTime.hpp"
# include "SceneStyle.hpp"
# include <Siv3D/Renderer/IRenderer.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>

namespace s3d
{
	namespace Scene
	{
		//void Resize(const s3d::Size size)
		//{
		//	SIV3D_ENGINE(Renderer)->setSceneBufferSize(size);
		//}

		//void Resize(const int32 width, const int32 height)
		//{
		//	Resize(s3d::Size{ width, height });
		//}

		//s3d::Size Size() noexcept
		//{
		//	return SIV3D_ENGINE(Renderer)->getSceneBufferSize();
		//}

		//int32 Width() noexcept
		//{
		//	return Size().x;
		//}

		//int32 Height() noexcept
		//{
		//	return Size().y;
		//}

		//Point Center() noexcept
		//{
		//	return (Size() / 2);
		//}

		//Vec2 CenterF() noexcept
		//{
		//	return (Size() * 0.5);
		//}

		//s3d::Rect Rect() noexcept
		//{
		//	return s3d::Rect{ Size() };
		//}

		//void SetResizeMode(const ResizeMode resizeMode)
		//{
		//	return SIV3D_ENGINE(Renderer)->setSceneResizeMode(resizeMode);
		//}

		//ResizeMode GetResizeMode() noexcept
		//{
		//	return SIV3D_ENGINE(Renderer)->getSceneResizeMode();
		//}

		void SetBackground(const ColorF& color)
		{
			SIV3D_ENGINE(Renderer)->getSceneStyle().backgroundColor = color;
		}

		const ColorF& GetBackground() noexcept
		{
			return SIV3D_ENGINE(Renderer)->getSceneStyle().backgroundColor;
		}

		void SetLetterbox(const ColorF& color)
		{
			SIV3D_ENGINE(Renderer)->getSceneStyle().letterboxColor = color;
		}

		const ColorF& GetLetterbox() noexcept
		{
			return SIV3D_ENGINE(Renderer)->getSceneStyle().letterboxColor;
		}

		void SetTextureFilter(const TextureFilter textureFilter)
		{
			SIV3D_ENGINE(Renderer)->getSceneStyle().textureFilter = textureFilter;
		}

		TextureFilter GetTextureFilter() noexcept
		{
			return SIV3D_ENGINE(Renderer)->getSceneStyle().textureFilter;
		}

		void SetMaxDeltaTime(const double timeSec)
		{
			SIV3D_ENGINE(Scene)->getSceneTime().setMaxDeltaTimeSec(timeSec);
		}

		double GetMaxDeltaTime() noexcept
		{
			return SIV3D_ENGINE(Scene)->getSceneTime().getMaxDeltaTimeSec();
		}

		double DeltaTime() noexcept
		{
			return SIV3D_ENGINE(Scene)->getSceneTime().getDeltaTimeSec();
		}

		double Time() noexcept
		{
			return SIV3D_ENGINE(Scene)->getSceneTime().getTimeSec();
		}

		int32 FrameCount() noexcept
		{
			return SIV3D_ENGINE(Scene)->getSceneTime().getFrameCount();
		}

		//Vec2 ClientToScene(const Vec2 pos) noexcept
		//{
		//	auto [s, viewRect] = SIV3D_ENGINE(Renderer)->getLetterboxComposition();
		//	return (pos - viewRect.pos) / s;
		//}
	}
}
