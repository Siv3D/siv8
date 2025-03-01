//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2025 Ryo Suzuki
//	Copyright (c) 2016-2025 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once

namespace s3d
{		
	////////////////////////////////////////////////////////////////
	//
	//	ToBinary
	//
	////////////////////////////////////////////////////////////////

	String ToBinary(const Concept::Integral auto value)
	{
		return ToBaseN(value, 2);
	}
		
	////////////////////////////////////////////////////////////////
	//
	//	ToOctal
	//
	////////////////////////////////////////////////////////////////

	String ToOctal(const Concept::Integral auto value)
	{
		return ToBaseN(value, 8);
	}
		
	////////////////////////////////////////////////////////////////
	//
	//	ToHex
	//
	////////////////////////////////////////////////////////////////

	String ToHex(const Concept::Integral auto value, const LetterCase letterCase)
	{
		return ToBaseN(value, 16, letterCase);
	}
}
