﻿//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2025 Ryo Suzuki
//	Copyright (c) 2016-2025 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include <Siv3D/Scene.hpp>
# include <Siv3D/2DShapes.hpp>
# include "IScene.hpp"
# include "SceneTime.hpp"
# include "SceneStyle.hpp"
# include <Siv3D/Renderer/IRenderer.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>

namespace s3d
{
	namespace Scene
	{
		////////////////////////////////////////////////////////////////
		//
		//	Resize
		//
		////////////////////////////////////////////////////////////////

		void Resize(const s3d::Size size)
		{
			SIV3D_ENGINE(Renderer)->resizeSceneBuffer(size);
		}

		void Resize(const int32 width, const int32 height)
		{
			Resize(s3d::Size{ width, height });
		}

		////////////////////////////////////////////////////////////////
		//
		//	Size
		//
		////////////////////////////////////////////////////////////////

		s3d::Size Size() noexcept
		{
			return SIV3D_ENGINE(Renderer)->getSceneBufferSize();
		}

		////////////////////////////////////////////////////////////////
		//
		//	Width
		//
		////////////////////////////////////////////////////////////////

		int32 Width() noexcept
		{
			return Size().x;
		}

		////////////////////////////////////////////////////////////////
		//
		//	Height
		//
		////////////////////////////////////////////////////////////////

		int32 Height() noexcept
		{
			return Size().y;
		}

		////////////////////////////////////////////////////////////////
		//
		//	Center, CenterF
		//
		////////////////////////////////////////////////////////////////

		Point Center() noexcept
		{
			return (Size() / 2);
		}

		Vec2 CenterF() noexcept
		{
			return (Size() * 0.5);
		}

		////////////////////////////////////////////////////////////////
		//
		//	Rect
		//
		////////////////////////////////////////////////////////////////

		s3d::Rect Rect() noexcept
		{
			return s3d::Rect{ Size() };
		}

		////////////////////////////////////////////////////////////////
		//
		//	SetResizeMode
		//
		////////////////////////////////////////////////////////////////

		void SetResizeMode(const ResizeMode resizeMode)
		{
			return SIV3D_ENGINE(Renderer)->setSceneResizeMode(resizeMode);
		}

		////////////////////////////////////////////////////////////////
		//
		//	GetResizeMode
		//
		////////////////////////////////////////////////////////////////

		ResizeMode GetResizeMode() noexcept
		{
			return SIV3D_ENGINE(Renderer)->getSceneResizeMode();
		}

		////////////////////////////////////////////////////////////////
		//
		//	SetBackground
		//
		////////////////////////////////////////////////////////////////

		void SetBackground(const ColorF& color)
		{
			SIV3D_ENGINE(Renderer)->getSceneStyle().backgroundColor = color;
		}

		////////////////////////////////////////////////////////////////
		//
		//	GetBackground
		//
		////////////////////////////////////////////////////////////////

		const ColorF& GetBackground() noexcept
		{
			return SIV3D_ENGINE(Renderer)->getSceneStyle().backgroundColor;
		}

		////////////////////////////////////////////////////////////////
		//
		//	SetLetterbox
		//
		////////////////////////////////////////////////////////////////

		void SetLetterbox(const ColorF& color)
		{
			SIV3D_ENGINE(Renderer)->getSceneStyle().letterboxColor = color;
		}

		////////////////////////////////////////////////////////////////
		//
		//	GetLetterbox
		//
		////////////////////////////////////////////////////////////////

		const ColorF& GetLetterbox() noexcept
		{
			return SIV3D_ENGINE(Renderer)->getSceneStyle().letterboxColor;
		}

		////////////////////////////////////////////////////////////////
		//
		//	SetTextureFilter
		//
		////////////////////////////////////////////////////////////////

		void SetTextureFilter(const TextureFilter textureFilter)
		{
			SIV3D_ENGINE(Renderer)->getSceneStyle().textureFilter = textureFilter;
		}

		////////////////////////////////////////////////////////////////
		//
		//	GetTextureFilter
		//
		////////////////////////////////////////////////////////////////

		TextureFilter GetTextureFilter() noexcept
		{
			return SIV3D_ENGINE(Renderer)->getSceneStyle().textureFilter;
		}

		////////////////////////////////////////////////////////////////
		//
		//	SetMSAASampleCount
		//
		////////////////////////////////////////////////////////////////

		//void SetMSAASampleCount(const MSAASampleCount sampleCount)
		//{

		//}

		////////////////////////////////////////////////////////////////
		//
		//	GetMSAASampleCount
		//
		////////////////////////////////////////////////////////////////

		//[[nodiscard]]
		//MSAASampleCount GetMSAASampleCount() noexcept
		//{

		//}

		////////////////////////////////////////////////////////////////
		//
		//	SetMaxDeltaTime
		//
		////////////////////////////////////////////////////////////////

		void SetMaxDeltaTime(const double timeSec)
		{
			SIV3D_ENGINE(Scene)->getSceneTime().setMaxDeltaTimeSec(timeSec);
		}

		////////////////////////////////////////////////////////////////
		//
		//	GetMaxDeltaTime
		//
		////////////////////////////////////////////////////////////////

		double GetMaxDeltaTime() noexcept
		{
			return SIV3D_ENGINE(Scene)->getSceneTime().getMaxDeltaTimeSec();
		}

		////////////////////////////////////////////////////////////////
		//
		//	DeltaTime
		//
		////////////////////////////////////////////////////////////////

		double DeltaTime() noexcept
		{
			return SIV3D_ENGINE(Scene)->getSceneTime().getDeltaTimeSec();
		}

		////////////////////////////////////////////////////////////////
		//
		//	Time
		//
		////////////////////////////////////////////////////////////////

		double Time() noexcept
		{
			return SIV3D_ENGINE(Scene)->getSceneTime().getTimeSec();
		}

		////////////////////////////////////////////////////////////////
		//
		//	FrameCount
		//
		////////////////////////////////////////////////////////////////

		int32 FrameCount() noexcept
		{
			return SIV3D_ENGINE(Scene)->getSceneTime().getFrameCount();
		}

		////////////////////////////////////////////////////////////////
		//
		//	ClientToScene
		//
		////////////////////////////////////////////////////////////////

		Vec2 ClientToScene(const Vec2 pos) noexcept
		{
			auto [s, viewRect] = SIV3D_ENGINE(Renderer)->getLetterboxComposition();
			return ((pos - viewRect.pos) / s);
		}
	}
}
