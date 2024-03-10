﻿//-----------------------------------------------
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

# if SIV3D_CPU(X86_64)

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

# elif SIV3D_CPU(ARM64)

	struct ARM64Features
	{
		int fp : 1;          // Floating-point.
		int asimd : 1;       // Advanced SIMD.
		int evtstrm : 1;     // Generic timer generated events.
		int aes : 1;         // Hardware-accelerated Advanced Encryption Standard.
		int pmull : 1;       // Polynomial multiply long.
		int sha1 : 1;        // Hardware-accelerated SHA1.
		int sha2 : 1;        // Hardware-accelerated SHA2-256.
		int crc32 : 1;       // Hardware-accelerated CRC-32.
		int atomics : 1;     // Armv8.1 atomic instructions.
		int fphp : 1;        // Half-precision floating point support.
		int asimdhp : 1;     // Advanced SIMD half-precision support.
		int cpuid : 1;       // Access to certain ID registers.
		int asimdrdm : 1;    // Rounding Double Multiply Accumulate/Subtract.
		int jscvt : 1;       // Support for JavaScript conversion.
		int fcma : 1;        // Floating point complex numbers.
		int lrcpc : 1;       // Support for weaker release consistency.
		int dcpop : 1;       // Data persistence writeback.
		int sha3 : 1;        // Hardware-accelerated SHA3.
		int sm3 : 1;         // Hardware-accelerated SM3.
		int sm4 : 1;         // Hardware-accelerated SM4.
		int asimddp : 1;     // Dot product instruction.
		int sha512 : 1;      // Hardware-accelerated SHA512.
		int sve : 1;         // Scalable Vector Extension.
		int asimdfhm : 1;    // Additional half-precision instructions.
		int dit : 1;         // Data independent timing.
		int uscat : 1;       // Unaligned atomics support.
		int ilrcpc : 1;      // Additional support for weaker release consistency.
		int flagm : 1;       // Flag manipulation instructions.
		int ssbs : 1;        // Speculative Store Bypass Safe PSTATE bit.
		int sb : 1;          // Speculation barrier.
		int paca : 1;        // Address authentication.
		int pacg : 1;        // Generic authentication.
		int dcpodp : 1;      // Data cache clean to point of persistence.
		int sve2 : 1;        // Scalable Vector Extension (version 2).
		int sveaes : 1;      // SVE AES instructions.
		int svepmull : 1;    // SVE polynomial multiply long instructions.
		int svebitperm : 1;  // SVE bit permute instructions.
		int svesha3 : 1;     // SVE SHA3 instructions.
		int svesm4 : 1;      // SVE SM4 instructions.
		int flagm2 : 1;      // Additional flag manipulation instructions.
		int frint : 1;       // Floating point to integer rounding.
		int svei8mm : 1;     // SVE Int8 matrix multiplication instructions.
		int svef32mm : 1;    // SVE FP32 matrix multiplication instruction.
		int svef64mm : 1;    // SVE FP64 matrix multiplication instructions.
		int svebf16 : 1;     // SVE BFloat16 instructions.
		int i8mm : 1;        // Int8 matrix multiplication instructions.
		int bf16 : 1;        // BFloat16 instructions.
		int dgh : 1;         // Data Gathering Hint instruction.
		int rng : 1;         // True random number generator support.
		int bti : 1;         // Branch target identification.
		int mte : 1;         // Memory tagging extension.
		int ecv : 1;         // Enhanced counter virtualization.
		int afp : 1;         // Alternate floating-point behaviour.
		int rpres : 1;       // 12-bit reciprocal (square root) estimate precision.
		int mte3 : 1;        // MTE asymmetric fault handling.
		int sme : 1;         // Scalable Matrix Extension.
		int smei16i64 : 1;   // 16-bit to 64-bit integer widening outer product.
		int smef64f64 : 1;   // FP64 to FP64 outer product.
		int smei8i32 : 1;    // 8-bit to 32-bit integer widening outer product.
		int smef16f32 : 1;   // FP16 to FP32 outer product.
		int smeb16f32 : 1;   // BFloat16 to FP32 outper product.
		int smef32f32 : 1;   // FP32 to FP32 outer product.
		int smefa64 : 1;     // Full A64 support for SME in streaming mode.
		int wfxt : 1;        // WFE and WFI with timeout.
		int ebf16 : 1;       // Extended BFloat16 instructions.
		int sveebf16 : 1;    // SVE BFloat16 instructions.
		int cssc : 1;        // Common short sequence compression instructions.
		int rprfm : 1;       // Range Prefetch Memory hint instruction.
		int sve2p1 : 1;      // Scalable Vector Extension (version 2.1).
		int sme2 : 1;        // Scalable Matrix Extension (version 2).
		int sme2p1 : 1;      // Scalable Matrix Extension (version 2.1).
		int smei16i32 : 1;   // 16-bit to 64-bit integer widening outer product.
		int smebi32i32 : 1;  // 1-bit binary to 32-bit integer outer product.
		int smeb16b16 : 1;   // SME2.1 BFloat16 instructions.
		int smef16f16 : 1;   // FP16 to FP16 outer product.
		int mops : 1;        // Standardized memory operations.
		int hbc : 1;         // Hinted conditional branches.
	};

	struct CPUInfo
	{
		/// @brief 対応命令セット
		ARM64Features features;
		
		int32 implementer;
		
		int32 variant;
		
		int32 part;
		
		int32 revision;
	};

# endif

	namespace System
	{
		/// @brief ユーザの CPU の情報を返します。
		/// @return ユーザの CPU の情報
		[[nodiscard]]
		const CPUInfo& GetCPUInfo() noexcept;
	}
}
