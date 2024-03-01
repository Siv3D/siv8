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

namespace s3d
{		
	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class E>
	void Formatter(FormatData& formatData, const Result<Type, E>& value)
	{
		if constexpr (std::is_same<Type, void>::value)
		{
			if (value)
			{
				formatData.string.append(U"Ok()"_sv);
			}
			else
			{
				formatData.string.append(U"Err("_sv);
				DebugFormatter(formatData, value.error());
				formatData.string.append(U")"_sv);
			}
		}
		else
		{
			if (value)
			{
				formatData.string.append(U"Ok("_sv);
				DebugFormatter(formatData, *value);
				formatData.string.append(U")"_sv);
			}
			else
			{
				formatData.string.append(U"Err("_sv);
				DebugFormatter(formatData, value.error());
				formatData.string.append(U")"_sv);
			}
		}
	}
}
