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

# include <Siv3D/CPUInfo.hpp>
# include <Siv3D/Unicode.hpp>

# if SIV3D_CPU(X86_64)

# include <cstring>
# include <ThirdParty/cpu_features/cpuinfo_x86.h>

namespace s3d
{
	static_assert(sizeof(s3d::X86Features) == sizeof(cpu_features::X86Features));

	namespace
	{
		[[nodiscard]]
		static CPUInfo InitCPUInfo() noexcept
		{
			const cpu_features::X86Info info = cpu_features::GetX86Info();

			CPUInfo result;
			std::memcpy(&result.features, &info.features, sizeof(s3d::X86Features));
			result.family	= info.family;
			result.model	= info.model;
			result.stepping	= info.stepping;
			result.vendor	= Unicode::FromUTF8(info.vendor);
			result.brand	= Unicode::FromUTF8(info.brand_string);

			return result;
		}
	}

	static const CPUInfo g_CPUInfo = InitCPUInfo();
	
	namespace Platform::X86_64::System
	{
		////////////////////////////////////////////////////////////////
		//
		//	GetCPUInfo
		//
		////////////////////////////////////////////////////////////////

		const CPUInfo& GetCPUInfo() noexcept
		{
			return g_CPUInfo;
		}
	}
}

# endif

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	SupportsAVX
	//
	////////////////////////////////////////////////////////////////

	bool SupportsAVX() noexcept
	{
	# if SIV3D_CPU(X86_64)

		return g_CPUInfo.features.avx;

	# else

		return false;

	# endif
	}

	////////////////////////////////////////////////////////////////
	//
	//	SupportsAVX2
	//
	////////////////////////////////////////////////////////////////

	bool SupportsAVX2() noexcept
	{
	# if SIV3D_CPU(X86_64)

		return g_CPUInfo.features.avx2;

	# else

		return false;

	# endif
	}
}
