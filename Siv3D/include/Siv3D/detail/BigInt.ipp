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
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	inline BigInt::BigInt(const Concept::SignedIntegral auto i)
		: BigInt{ static_cast<int64>(i) } {}

	inline BigInt::BigInt(const Concept::UnsignedIntegral auto i)
		: BigInt{ static_cast<uint64>(i) } {}
}
