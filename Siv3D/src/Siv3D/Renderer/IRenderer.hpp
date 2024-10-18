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
# include <Siv3D/Common.hpp>
# include <Siv3D/PointVector.hpp>

namespace s3d
{
	struct SceneStyle;
	enum class ResizeMode : uint8;

	class SIV3D_NOVTABLE ISiv3DRenderer
	{
	public:

		[[nodiscard]]
		static ISiv3DRenderer* Create();

		virtual ~ISiv3DRenderer() = default;

		virtual StringView getName() const = 0;

		virtual void init() = 0;

		virtual void beginFrame() = 0;

		virtual void flush() = 0;

		virtual bool present() = 0;

		virtual SceneStyle& getSceneStyle() noexcept = 0;

		virtual void setSceneResizeMode(ResizeMode resizeMode) = 0;

		virtual ResizeMode getSceneResizeMode() const noexcept = 0;

		virtual void resizeSceneBuffer(Size size) = 0;

		virtual const Size& getSceneBufferSize() const noexcept = 0;

		virtual std::pair<double, RectF> getLetterboxComposition() const noexcept = 0;

		virtual void updateSceneSize() = 0;

		virtual void setVSyncEnabled(bool enabled) = 0;

		virtual bool isVSyncEnabled() const = 0;

		virtual void setLowLatencyMode(bool enabled) = 0;

		virtual bool isLowLatencyMode() const = 0;

		virtual const Image& getScreenCapture() const = 0;
	};
}
