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

namespace s3d
{
	namespace SimpleHTTP
	{
		constexpr bool IsInformational(const HTTPStatusCode code) noexcept
		{
			return InRange<uint32>(FromEnum(code), 100u, 199u);
		}

		constexpr bool IsSuccess(const HTTPStatusCode s) noexcept
		{
			return InRange<uint32>(FromEnum(s), 200u, 299u);
		}

		constexpr bool IsRedirect(const HTTPStatusCode s) noexcept
		{
			return InRange<uint32>(FromEnum(s), 300u, 399u);
		}

		constexpr bool IsClientError(const HTTPStatusCode s) noexcept
		{
			return InRange<uint32>(FromEnum(s), 400u, 499u);
		}

		constexpr bool IsServerError(const HTTPStatusCode s) noexcept
		{
			return InRange<uint32>(FromEnum(s), 500u, 599u);
		}
	}
}
