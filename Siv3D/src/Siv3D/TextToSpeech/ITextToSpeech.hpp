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
# include <Siv3D/StringView.hpp>

namespace s3d
{
	class Wave;

	class SIV3D_NOVTABLE ISiv3DTextToSpeech
	{
	public:

		[[nodiscard]]
		static ISiv3DTextToSpeech* Create();

		virtual ~ISiv3DTextToSpeech() = default;

		virtual void init() = 0;

		virtual bool synthesizeToWave(StringView text, Wave& wave) = 0;
	};
}
