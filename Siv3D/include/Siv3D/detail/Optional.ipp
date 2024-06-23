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

	template <class Type>
	void Formatter(FormatData& formatData, const Optional<Type>& value)
	{
		if (value)
		{
			formatData.string.append(U"(Optional)"_sv);
			Formatter(formatData, *value);
		}
		else
		{
			Formatter(formatData, none);
		}
	}
}
