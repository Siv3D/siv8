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

# include "CRegExp.hpp"
# include <Siv3D/EngineLog.hpp>
# include <ThirdParty/Oniguruma/oniguruma.h>

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	(destructor)
	//
	////////////////////////////////////////////////////////////////

	CRegExp::~CRegExp()
	{
		LOG_SCOPED_DEBUG("CRegExp::~CRegExp()");
		
		::onig_end();
	}

	////////////////////////////////////////////////////////////////
	//
	//	init
	//
	////////////////////////////////////////////////////////////////

	void CRegExp::init()
	{
		LOG_SCOPED_DEBUG("CRegExp::init()");

		OnigEncoding encodings = ONIG_ENCODING_UTF32_LE;
		::onig_initialize(&encodings, 1);
	}
}
