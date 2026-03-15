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

# include "CMediaTranscoder.hpp"
# include <Siv3D/EngineLog.hpp>
# include <Siv3D/Error/InternalEngineError.hpp>
# include <mfapi.h>

namespace s3d
{
	CMediaTranscoder::~CMediaTranscoder()
	{
		LOG_SCOPED_DEBUG("CMediaTranscoder::~CMediaTranscoder()");

		if (FAILED(::MFShutdown()))
		{
			LOG_FAIL("Failed to shutdown Media Foundation.");
		}
	}

	void CMediaTranscoder::init()
	{
		LOG_SCOPED_DEBUG("CMediaTranscoder::init()");

		if (FAILED(::MFStartup(MF_VERSION, MFSTARTUP_LITE)))
		{
			throw InternalEngineError{ "Failed to initialize Media Foundation." };
		}
	}
}
