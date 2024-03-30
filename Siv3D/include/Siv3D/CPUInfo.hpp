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
# include "Common.hpp"
# include "String.hpp"

# if SIV3D_CPU(X86_64)

namespace s3d
{
	//
	// cpu_features
	// https://github.com/google/cpu_features
	//

	// Copyright 2017 Google LLC
	// Copyright 2020 Intel Corporation
	//
	// Licensed under the Apache License, Version 2.0 (the "License");
	// you may not use this file except in compliance with the License.
	// You may obtain a copy of the License at
	//
	//    http://www.apache.org/licenses/LICENSE-2.0
	//
	// Unless required by applicable law or agreed to in writing, software
	// distributed under the License is distributed on an "AS IS" BASIS,
	// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
	// See the License for the specific language governing permissions and
	// limitations under the License.

	struct X86Features
	{
		int fpu : 1;
		int tsc : 1;
		int cx8 : 1;
		int clfsh : 1;
		int mmx : 1;
		int aes : 1;
		int erms : 1;
		int f16c : 1;
		int fma4 : 1;
		int fma3 : 1;
		int vaes : 1;
		int vpclmulqdq : 1;
		int bmi1 : 1;
		int hle : 1;
		int bmi2 : 1;
		int rtm : 1;
		int rdseed : 1;
		int clflushopt : 1;
		int clwb : 1;

		int sse : 1;
		int sse2 : 1;
		int sse3 : 1;
		int ssse3 : 1;
		int sse4_1 : 1;
		int sse4_2 : 1;
		int sse4a : 1;

		int avx : 1;
		int avx_vnni : 1;
		int avx2 : 1;

		int avx512f : 1;
		int avx512cd : 1;
		int avx512er : 1;
		int avx512pf : 1;
		int avx512bw : 1;
		int avx512dq : 1;
		int avx512vl : 1;
		int avx512ifma : 1;
		int avx512vbmi : 1;
		int avx512vbmi2 : 1;
		int avx512vnni : 1;
		int avx512bitalg : 1;
		int avx512vpopcntdq : 1;
		int avx512_4vnniw : 1;
		int avx512_4vbmi2 : 1;  // Note: this is an alias to avx512_4fmaps.
		int avx512_second_fma : 1;
		int avx512_4fmaps : 1;
		int avx512_bf16 : 1;
		int avx512_vp2intersect : 1;
		int avx512_fp16 : 1;
		int amx_bf16 : 1;
		int amx_tile : 1;
		int amx_int8 : 1;
		int amx_fp16 : 1;

		int pclmulqdq : 1;
		int smx : 1;
		int sgx : 1;
		int cx16 : 1;  // aka. CMPXCHG16B
		int sha : 1;
		int popcnt : 1;
		int movbe : 1;
		int rdrnd : 1;

		int dca : 1;
		int ss : 1;
		int adx : 1;
		int lzcnt : 1;  // Note: this flag is called ABM for AMD, LZCNT for Intel.
		int gfni : 1;
		int movdiri : 1;
		int movdir64b : 1;
		int fs_rep_mov : 1;          // Fast short REP MOV
		int fz_rep_movsb : 1;        // Fast zero-length REP MOVSB
		int fs_rep_stosb : 1;        // Fast short REP STOSB
		int fs_rep_cmpsb_scasb : 1;  // Fast short REP CMPSB/SCASB

		int lam : 1;  // Intel Linear Address Mask
		int uai : 1;  // AMD Upper Address Ignore
	};

	struct CPUInfo
	{
		/// @brief 対応命令セット
		X86Features features;

		int32 family;

		int32 model;

		int32 stepping;
		
		String vendor;
		
		String brand;
	};

	namespace Platform::X86_64::System
	{
		/// @brief ユーザの CPU の情報を返します。
		/// @return ユーザの CPU の情報
		[[nodiscard]]
		const CPUInfo& GetCPUInfo() noexcept;
	}
}

# endif

namespace s3d
{
	[[nodiscard]]
	bool SupportsAVX() noexcept;

	[[nodiscard]]
	bool SupportsAVX2() noexcept;
}
