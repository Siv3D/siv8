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

struct ma_context;

namespace s3d
{
	class SIV3D_NOVTABLE ISiv3DAudio
	{
	public:

		[[nodiscard]]
		static ISiv3DAudio* Create();

		virtual ~ISiv3DAudio() = default;

		virtual void init() = 0;



		////////////////////////////////////////////////////////////////
		//
		//	getContext
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		virtual ma_context* getContext() const noexcept = 0;
	};
}
