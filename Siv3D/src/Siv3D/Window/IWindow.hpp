//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2023 Ryo Suzuki
//	Copyright (c) 2016-2023 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once
# include <Siv3D/Common.hpp>

namespace s3d
{
	class String;

	class SIV3D_NOVTABLE ISiv3DWindow
	{
	public:

		static ISiv3DWindow* Create();

		virtual ~ISiv3DWindow() = default;

		virtual void init() = 0;

		virtual void update() = 0;

		virtual void setWindowTitle(const String& title) = 0;

		virtual const String& getWindowTitle() const noexcept = 0;

		virtual void* getHandle() const noexcept = 0;
	};
}
