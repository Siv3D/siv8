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
# include "Common.hpp"

namespace s3d
{
	struct FormatData;

	////////////////////////////////////////////////////////////////
	//
	//	TOMLValueType
	//
	////////////////////////////////////////////////////////////////

	enum class TOMLValueType : uint8
	{
		Invalid,
		
		Bool,
		
		Int,
		
		Float,
		
		String,
		
		OffsetDateTime,
		
		LocalDateTime,
		
		LocalDate,
		
		LocalTime,
		
		Array,
		
		Table,
	};

	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	void Formatter(FormatData& formatData, TOMLValueType value);
}
