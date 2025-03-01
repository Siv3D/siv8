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
# include "Common.hpp"
# include "Concepts.hpp"
# include "PCG64.hpp"
# include "Distribution.hpp"
# include "Number.hpp"

namespace s3d
{
	/// @brief デフォルトの乱数エンジンの種類 (PRNG::PCG64)
	using DefaultRNG = PRNG::PCG64;

	////////////////////////////////////////////////////////////////
	//
	//	GetDefaultRNG
	//
	////////////////////////////////////////////////////////////////

	/// @brief 現在のスレッドのグローバル乱数エンジンの参照を返します。
	/// @return 現在のスレッドのグローバル乱数エンジン
	[[nodiscard]]
	DefaultRNG& GetDefaultRNG() noexcept;

	////////////////////////////////////////////////////////////////
	//
	//	Reseed
	//
	////////////////////////////////////////////////////////////////

	/// @brief 現在のスレッドのグローバル乱数エンジンのシード値を設定します。 
	/// @param seed シード値
	void Reseed(uint64 seed) noexcept;

	/// @brief 現在のスレッドのグローバル乱数エンジンのシード値を、シード列から生成して設定します。 
	/// @param seeds シード列
	template <class SeedSequence>
	void Reseed(SeedSequence&& seedSequence) noexcept;

	////////////////////////////////////////////////////////////////
	//
	//	Random
	//
	////////////////////////////////////////////////////////////////

	/// @brief [0.0, 1.0) の範囲の乱数を返します。
	/// @remark 現在のスレッドのグローバルな乱数エンジンを使用します。
	/// @return [0.0, 1.0) の範囲の乱数
	double Random() noexcept;

	/// @brief [0.0, 1.0) の範囲の乱数を返します。
	/// @param urbg 乱数エンジン
	/// @return [0.0, 1.0) の範囲の乱数
	[[nodiscard]]
	double Random(Concept::UniformRandomBitGenerator auto&& urbg) noexcept;

	/// @brief 現在のスレッドの乱数エンジンを用いて、min 以上 max 以下の範囲の乱数を生成して返します。
	/// @tparam Arithmetic 生成する乱数の型
	/// @param min 生成する乱数の最小値
	/// @param max 生成する乱数の最大値
	/// @return 生成された乱数
	template <Concept::Arithmetic Arithmetic>
	[[nodiscard]]
	Arithmetic Random(Arithmetic min, Arithmetic max);

	/// @brief min 以上 max 以下の範囲の乱数を生成して返します。
	/// @tparam Arithmetic 生成する乱数の型
	/// @param min 生成する乱数の最小値
	/// @param max 生成する乱数の最大値
	/// @param urbg 乱数エンジン
	/// @return 生成された乱数
	template <Concept::Arithmetic Arithmetic>
	[[nodiscard]]
	Arithmetic Random(Arithmetic min, Arithmetic max, Concept::UniformRandomBitGenerator auto&& urbg);

	/// @brief 現在のスレッドの乱数エンジンを用いて、0 以上 max 以下の範囲の乱数を生成して返します。
	/// @tparam Arithmetic 生成する乱数の型
	/// @param max 生成する乱数の最大値
	/// @return 生成された乱数
	template <Concept::Arithmetic Arithmetic>
	[[nodiscard]]
	Arithmetic Random(Arithmetic max);

	/// @brief 0 以上 max 以下の範囲の乱数を生成して返します。
	/// @tparam Arithmetic 生成する乱数の型
	/// @param max 生成する乱数の最大値
	/// @param urbg 乱数エンジン
	/// @return 生成された乱数
	template <Concept::Arithmetic Arithmetic>
	[[nodiscard]]
	Arithmetic Random(Arithmetic max, Concept::UniformRandomBitGenerator auto&& urbg);

	////////////////////////////////////////////////////////////////
	//
	//	RandomOpen
	//
	////////////////////////////////////////////////////////////////

	/// @brief 現在のスレッドの乱数エンジンを用いて、min より大きく max 未満の範囲の乱数を生成して返します。
	/// @tparam Arithmetic 生成する乱数の型
	/// @param min 生成する乱数の範囲の基準となる値（これより大きい）
	/// @param max 生成する乱数の範囲の基準となる値（これ未満）
	/// @return 生成された乱数
	template <Concept::Arithmetic Arithmetic>
	[[nodiscard]]
	Arithmetic RandomOpen(Arithmetic min, Arithmetic max);

	/// @brief min より大きく max 未満の範囲の乱数を生成して返します。
	/// @tparam Arithmetic 生成する乱数の型
	/// @param min 生成する乱数の範囲の基準となる値（これより大きい）
	/// @param max 生成する乱数の範囲の基準となる値（これ未満）
	/// @param urbg 乱数エンジン
	/// @return 生成された乱数
	template <Concept::Arithmetic Arithmetic>
	[[nodiscard]]
	Arithmetic RandomOpen(Arithmetic min, Arithmetic max, Concept::UniformRandomBitGenerator auto&& urbg);

	////////////////////////////////////////////////////////////////
	//
	//	RandomClosed
	//
	////////////////////////////////////////////////////////////////

	/// @brief 現在のスレッドの乱数エンジンを用いて、min 以上 max 以下の範囲の乱数を生成して返します。
	/// @tparam Arithmetic 生成する乱数の型
	/// @param min 生成する乱数の最小値
	/// @param max 生成する乱数の最大値
	/// @return 生成された乱数
	/// @remark Random(min, max) と同じです。
	template <Concept::Arithmetic Arithmetic>
	[[nodiscard]]
	Arithmetic RandomClosed(Arithmetic min, Arithmetic max);

	/// @brief min 以上 max 以下の範囲の乱数を生成して返します。
	/// @tparam Arithmetic 生成する乱数の型
	/// @param min 生成する乱数の最小値
	/// @param max 生成する乱数の最大値
	/// @param urbg 乱数エンジン
	/// @return 生成された乱数
	/// @remark Random(min, max, urbg) と同じです。
	template <Concept::Arithmetic Arithmetic>
	[[nodiscard]]
	Arithmetic RandomClosed(Arithmetic min, Arithmetic max, Concept::UniformRandomBitGenerator auto&& urbg);

	////////////////////////////////////////////////////////////////
	//
	//	RandomOpenClosed
	//
	////////////////////////////////////////////////////////////////

	/// @brief 現在のスレッドの乱数エンジンを用いて、min より大きく max 以下の範囲の乱数を生成して返します。
	/// @tparam Arithmetic 生成する乱数の型
	/// @param min 生成する乱数の範囲の基準となる値（これより大きい）
	/// @param max 生成する乱数の最大値
	/// @return 生成された乱数
	template <Concept::Arithmetic Arithmetic>
	[[nodiscard]]
	Arithmetic RandomOpenClosed(Arithmetic min, Arithmetic max);

	/// @brief min より大きく max 以下の範囲の乱数を生成して返します。
	/// @tparam Arithmetic 生成する乱数の型
	/// @param min 生成する乱数の範囲の基準となる値（これより大きい）
	/// @param max 生成する乱数の最大値
	/// @param urbg 乱数エンジン
	/// @return 生成された乱数
	template <Concept::Arithmetic Arithmetic>
	Arithmetic RandomOpenClosed(Arithmetic min, Arithmetic max, Concept::UniformRandomBitGenerator auto&& urbg);

	////////////////////////////////////////////////////////////////
	//
	//	RandomClosedOpen
	//
	////////////////////////////////////////////////////////////////

	/// @brief 現在のスレッドの乱数エンジンを用いて、min 以上 max 未満の範囲の乱数を生成して返します。
	/// @tparam Arithmetic 生成する乱数の型
	/// @param min 生成する乱数の最小値
	/// @param max 生成する乱数の範囲の基準となる値（これ未満）
	/// @return 生成された乱数
	template <Concept::Arithmetic Arithmetic>
	Arithmetic RandomClosedOpen(Arithmetic min, Arithmetic max);

	/// @brief min 以上 max 未満の範囲の乱数を生成して返します。
	/// @tparam Arithmetic 生成する乱数の型
	/// @param min 生成する乱数の最小値
	/// @param max 生成する乱数の範囲の基準となる値（これ未満）
	/// @param urbg 乱数エンジン
	/// @return 生成された乱数
	template <Concept::Arithmetic Arithmetic>
	Arithmetic RandomClosedOpen(Arithmetic min, Arithmetic max, Concept::UniformRandomBitGenerator auto&& urbg);

	////////////////////////////////////////////////////////////////
	//
	//	RandomBool
	//
	////////////////////////////////////////////////////////////////

	/// @brief 現在のスレッドの乱数エンジンを用いて、指定した確率で true を返します。
	/// @param p 確率
	/// @return p で指定した確率に基づき、true または false
	[[nodiscard]]
	bool RandomBool(double p = 0.5) noexcept;

	/// @brief 指定した確率で true を返します。
	/// @param p 確率
	/// @param urbg 使用する乱数エンジン
	/// @return p で指定した確率に基づき、true または false
	[[nodiscard]]
	bool RandomBool(double p, Concept::UniformRandomBitGenerator auto&& urbg) noexcept;

	////////////////////////////////////////////////////////////////
	//
	//	RandomUint8
	//
	////////////////////////////////////////////////////////////////

	/// @brief 現在のスレッドの乱数エンジンを用いて、uint8 型のランダムな値を返します。
	/// @return uint8 型のランダムな値
	[[nodiscard]]
	uint8 RandomUint8();

	/// @brief uint8 型のランダムな値を返します。
	/// @param urbg 使用する乱数エンジン
	/// @return uint8 型のランダムな値
	[[nodiscard]]
	uint8 RandomUint8(Concept::UniformRandomBitGenerator auto&& urbg);

	////////////////////////////////////////////////////////////////
	//
	//	RandomUint16
	//
	////////////////////////////////////////////////////////////////

	/// @brief 現在のスレッドの乱数エンジンを用いて、uint16 型のランダムな値を返します。
	/// @return uint16 型のランダムな値
	[[nodiscard]]
	uint16 RandomUint16();

	/// @brief uint16 型のランダムな値を返します。
	/// @param urbg 使用する乱数エンジン
	/// @return uint16 型のランダムな値
	[[nodiscard]]
	uint16 RandomUint16(Concept::UniformRandomBitGenerator auto&& urbg);

	////////////////////////////////////////////////////////////////
	//
	//	RandomUint32
	//
	////////////////////////////////////////////////////////////////

	/// @brief 現在のスレッドの乱数エンジンを用いて、uint32 型のランダムな値を返します。
	/// @return uint32 型のランダムな値
	[[nodiscard]]
	uint32 RandomUint32();

	/// @brief uint32 型のランダムな値を返します。
	/// @param urbg 使用する乱数エンジン
	/// @return uint32 型のランダムな値
	[[nodiscard]]
	uint32 RandomUint32(Concept::UniformRandomBitGenerator auto&& urbg);

	////////////////////////////////////////////////////////////////
	//
	//	RandomUint64
	//
	////////////////////////////////////////////////////////////////

	/// @brief 現在のスレッドの乱数エンジンを用いて、uint64 型のランダムな値を返します。
	/// @return uint64 型のランダムな値
	[[nodiscard]]
	uint64 RandomUint64();

	/// @brief uint64 型のランダムな値を返します。
	/// @param urbg 使用する乱数エンジン
	/// @return uint64 型のランダムな値
	[[nodiscard]]
	uint64 RandomUint64(Concept::UniformRandomBitGenerator auto&& urbg);

	////////////////////////////////////////////////////////////////
	//
	//	RandomInt8
	//
	////////////////////////////////////////////////////////////////

	/// @brief 現在のスレッドの乱数エンジンを用いて、int8 型のランダムな値を返します。
	/// @return int8 型のランダムな値
	[[nodiscard]]
	int8 RandomInt8();

	/// @brief int8 型のランダムな値を返します。
	/// @param urbg 使用する乱数エンジン
	/// @return int8 型のランダムな値
	[[nodiscard]]
	int8 RandomInt8(Concept::UniformRandomBitGenerator auto&& urbg);

	////////////////////////////////////////////////////////////////
	//
	//	RandomInt16
	//
	////////////////////////////////////////////////////////////////

	/// @brief 現在のスレッドの乱数エンジンを用いて、int16 型のランダムな値を返します。
	/// @return int16 型のランダムな値
	[[nodiscard]]
	int16 RandomInt16();

	/// @brief int16 型のランダムな値を返します。
	/// @param urbg 使用する乱数エンジン
	/// @return int16 型のランダムな値
	[[nodiscard]]
	int16 RandomInt16(Concept::UniformRandomBitGenerator auto&& urbg);

	////////////////////////////////////////////////////////////////
	//
	//	RandomInt32
	//
	////////////////////////////////////////////////////////////////

	/// @brief 現在のスレッドの乱数エンジンを用いて、int32 型のランダムな値を返します。
	/// @return int32 型のランダムな値
	[[nodiscard]]
	int32 RandomInt32();

	/// @brief int32 型のランダムな値を返します。
	/// @param urbg 使用する乱数エンジン
	/// @return int32 型のランダムな値
	[[nodiscard]]
	int32 RandomInt32(Concept::UniformRandomBitGenerator auto&& urbg);

	////////////////////////////////////////////////////////////////
	//
	//	RandomInt64
	//
	////////////////////////////////////////////////////////////////

	/// @brief 現在のスレッドの乱数エンジンを用いて、int64 型のランダムな値を返します。
	/// @return int64 型のランダムな値
	[[nodiscard]]
	int64 RandomInt64();

	/// @brief int64 型のランダムな値を返します。
	/// @param urbg 使用する乱数エンジン
	/// @return int64 型のランダムな値
	[[nodiscard]]
	int64 RandomInt64(Concept::UniformRandomBitGenerator auto&& urbg);
}

# include "detail/Random.ipp"
