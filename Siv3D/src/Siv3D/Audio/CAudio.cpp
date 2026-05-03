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

# include "CAudio.hpp"
# include <Siv3D/EngineLog.hpp>
# include <Siv3D/Error/InternalEngineError.hpp>
# include <ThirdParty/miniaudio/miniaudio.h>

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	CAudio::CAudio() = default;

	////////////////////////////////////////////////////////////////
	//
	//	(destructor)
	//
	////////////////////////////////////////////////////////////////

	CAudio::~CAudio()
	{
		LOG_SCOPED_DEBUG("CAudio::~CAudio()");

		::ma_context_uninit(m_context.get());
	}

	////////////////////////////////////////////////////////////////
	//
	//	init
	//
	////////////////////////////////////////////////////////////////

	void CAudio::init()
	{
		LOG_SCOPED_DEBUG("CAudio::init()");

		m_context = std::make_unique<ma_context>();

		if (::ma_result result = ::ma_context_init(nullptr, 0, nullptr, m_context.get());
			result != MA_SUCCESS)
		{
			throw InternalEngineError(fmt::format("ma_context_init() failed with code: {}", FromEnum(result)));
		}

		LOG_INFO(fmt::format("ℹ️ Audio backend: {} ({})", ma_get_backend_name(m_context->backend), FromEnum(m_context->backend)));
	}




	////////////////////////////////////////////////////////////////
	//
	//	getContext
	//
	////////////////////////////////////////////////////////////////

	ma_context* CAudio::getContext() const noexcept
	{
		return m_context.get();
	}
}
