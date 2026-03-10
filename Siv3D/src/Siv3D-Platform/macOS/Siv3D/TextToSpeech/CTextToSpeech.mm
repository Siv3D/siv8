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

# include "CTextToSpeech.hpp"
# include <Siv3D/Wave.hpp>
# include <Siv3D/EngineLog.hpp>

namespace s3d
{
	CTextToSpeech::~CTextToSpeech() // 必ず owner thread で呼び出される
	{
		LOG_SCOPED_DEBUG("CTextToSpeech::~CTextToSpeech()");
	}

	void CTextToSpeech::init()
	{
		LOG_SCOPED_DEBUG("CTextToSpeech::init()");
		
		m_ownerThreadID = std::this_thread::get_id();	
	}

	bool CTextToSpeech::synthesizeToWave(const StringView text, Wave& wave)
	{
		wave.clear();

		if (text.isEmpty())
		{
			return false;
		}

		return(false);
	}
}
