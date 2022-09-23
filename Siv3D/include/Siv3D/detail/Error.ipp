//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2022 Ryo Suzuki
//	Copyright (c) 2016-2022 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once

namespace s3d
{
	inline StringView Error::type() const noexcept
	{
		return U"Error"_sv;
	}

	inline StringView ParseError::type() const noexcept
	{
		return U"ParseError"_sv;
	}

	inline StringView NotImplementedError::type() const noexcept
	{
		return U"NotImplementedError"_sv;
	}

	inline StringView EngineError::type() const noexcept
	{
		return U"EngineError"_sv;
	}
}
