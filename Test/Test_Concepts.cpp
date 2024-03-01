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

# include <random>
# include "Siv3DTest.hpp"

enum EnumT {};
enum class ScopedEnumT {};
struct StructT {};

static constexpr int32 F1(Concept::Integral auto) { return 0; }
static constexpr int32 F1(Concept::SignedIntegral auto) { return 1; }
static constexpr int32 F1(Concept::UnsignedIntegral auto) { return 2; }
static constexpr int32 F1(Concept::FloatingPoint auto) { return 3; }
static constexpr int32 F1(Concept::Scalar auto) { return 4; }
static constexpr int32 F1(Concept::Arithmetic auto) { return 5; }
static constexpr int32 F1(Concept::Enum auto) { return 6; }
static constexpr int32 F1(Concept::ScopedEnum auto) { return 7; }
static constexpr int32 F1(Concept::Pointer auto) { return 8; }
static constexpr int32 F1(auto) { return 9; }

TEST_CASE("Concepts.overload")
{
	constexpr int* p = nullptr;
	static_assert(F1(bool{}) == 2);
	static_assert(F1(uint8{}) == 2);
	static_assert(F1(int32{}) == 1);
	static_assert(F1(float{}) == 3);
	static_assert(F1(EnumT{}) == 6);
	static_assert(F1(ScopedEnumT{}) == 7);
	static_assert(F1(p) == 8);
	static_assert(F1(StructT{}) == 9);
	static_assert(F1(nullptr) == 4);
}

TEST_CASE("Concepts")
{
	static_assert(Concept::Integral<bool> == true);
	static_assert(Concept::Integral<uint8> == true);
	static_assert(Concept::Integral<int32> == true);
	static_assert(Concept::Integral<float> == false);
	static_assert(Concept::Integral<std::string> == false);
	static_assert(Concept::Integral<EnumT> == false);
	static_assert(Concept::Integral<ScopedEnumT> == false);
	static_assert(Concept::Integral<StructT> == false);

	static_assert(Concept::SignedIntegral<bool> == false);
	static_assert(Concept::SignedIntegral<uint8> == false);
	static_assert(Concept::SignedIntegral<int32> == true);
	static_assert(Concept::SignedIntegral<float> == false);
	static_assert(Concept::SignedIntegral<std::string> == false);
	static_assert(Concept::SignedIntegral<EnumT> == false);
	static_assert(Concept::SignedIntegral<ScopedEnumT> == false);
	static_assert(Concept::SignedIntegral<StructT> == false);

	static_assert(Concept::UnsignedIntegral<bool> == true);
	static_assert(Concept::UnsignedIntegral<uint8> == true);
	static_assert(Concept::UnsignedIntegral<int32> == false);
	static_assert(Concept::UnsignedIntegral<float> == false);
	static_assert(Concept::UnsignedIntegral<std::string> == false);
	static_assert(Concept::UnsignedIntegral<EnumT> == false);
	static_assert(Concept::UnsignedIntegral<ScopedEnumT> == false);
	static_assert(Concept::UnsignedIntegral<StructT> == false);

	static_assert(Concept::FloatingPoint<bool> == false);
	static_assert(Concept::FloatingPoint<uint8> == false);
	static_assert(Concept::FloatingPoint<int32> == false);
	static_assert(Concept::FloatingPoint<float> == true);
	static_assert(Concept::FloatingPoint<std::string> == false);
	static_assert(Concept::FloatingPoint<EnumT> == false);
	static_assert(Concept::FloatingPoint<ScopedEnumT> == false);
	static_assert(Concept::FloatingPoint<StructT> == false);

	static_assert(Concept::Signed<bool> == false);
	static_assert(Concept::Signed<uint8> == false);
	static_assert(Concept::Signed<int32> == true);
	static_assert(Concept::Signed<float> == true);
	static_assert(Concept::Signed<std::string> == false);
	static_assert(Concept::Signed<EnumT> == false);
	static_assert(Concept::Signed<ScopedEnumT> == false);
	static_assert(Concept::Signed<StructT> == false);

	static_assert(Concept::Arithmetic<bool> == true);
	static_assert(Concept::Arithmetic<uint8> == true);
	static_assert(Concept::Arithmetic<int32> == true);
	static_assert(Concept::Arithmetic<float> == true);
	static_assert(Concept::Arithmetic<std::string> == false);
	static_assert(Concept::Arithmetic<EnumT> == false);
	static_assert(Concept::Arithmetic<ScopedEnumT> == false);
	static_assert(Concept::Arithmetic<StructT> == false);

	static_assert(Concept::Scalar<bool> == true);
	static_assert(Concept::Scalar<uint8> == true);
	static_assert(Concept::Scalar<int32> == true);
	static_assert(Concept::Scalar<float> == true);
	static_assert(Concept::Scalar<std::string> == false);
	static_assert(Concept::Scalar<EnumT> == true);
	static_assert(Concept::Scalar<ScopedEnumT> == true);
	static_assert(Concept::Scalar<StructT> == false);

	static_assert(Concept::Enum<bool> == false);
	static_assert(Concept::Enum<uint8> == false);
	static_assert(Concept::Enum<int32> == false);
	static_assert(Concept::Enum<float> == false);
	static_assert(Concept::Enum<std::string> == false);
	static_assert(Concept::Enum<EnumT> == true);
	static_assert(Concept::Enum<ScopedEnumT> == true);
	static_assert(Concept::Enum<StructT> == false);

	static_assert(Concept::ScopedEnum<bool> == false);
	static_assert(Concept::ScopedEnum<uint8> == false);
	static_assert(Concept::ScopedEnum<int32> == false);
	static_assert(Concept::ScopedEnum<float> == false);
	static_assert(Concept::ScopedEnum<std::string> == false);
	static_assert(Concept::ScopedEnum<EnumT> == false);
	static_assert(Concept::ScopedEnum<ScopedEnumT> == true);
	static_assert(Concept::ScopedEnum<StructT> == false);

	static_assert(Concept::TriviallyCopyable<bool> == true);
	static_assert(Concept::TriviallyCopyable<uint8> == true);
	static_assert(Concept::TriviallyCopyable<int32> == true);
	static_assert(Concept::TriviallyCopyable<float> == true);
	static_assert(Concept::TriviallyCopyable<std::string> == false);
	static_assert(Concept::TriviallyCopyable<EnumT> == true);
	static_assert(Concept::TriviallyCopyable<ScopedEnumT> == true);
	static_assert(Concept::TriviallyCopyable<StructT> == true);

	static_assert(Concept::UniformRandomBitGenerator<bool> == false);
	static_assert(Concept::UniformRandomBitGenerator<uint8> == false);
	static_assert(Concept::UniformRandomBitGenerator<int32> == false);
	static_assert(Concept::UniformRandomBitGenerator<float> == false);
	static_assert(Concept::UniformRandomBitGenerator<std::string> == false);
	static_assert(Concept::UniformRandomBitGenerator<EnumT> == false);
	static_assert(Concept::UniformRandomBitGenerator<ScopedEnumT> == false);
	static_assert(Concept::UniformRandomBitGenerator<StructT> == false);
	static_assert(Concept::UniformRandomBitGenerator<std::mt19937> == true);
	static_assert(Concept::UniformRandomBitGenerator<std::random_device> == true);

	static_assert(Concept::LessThanComparable<bool> == true);
	static_assert(Concept::LessThanComparable<uint8> == true);
	static_assert(Concept::LessThanComparable<int32> == true);
	static_assert(Concept::LessThanComparable<float> == true);
	static_assert(Concept::LessThanComparable<std::string> == true);
	static_assert(Concept::LessThanComparable<EnumT> == true);
	static_assert(Concept::LessThanComparable<ScopedEnumT> == true);
	static_assert(Concept::LessThanComparable<StructT> == false);

	static_assert(Concept::Addable<bool> == true);
	static_assert(Concept::Addable<uint8> == true);
	static_assert(Concept::Addable<int32> == true);
	static_assert(Concept::Addable<float> == true);
	static_assert(Concept::Addable<std::string> == true);
	static_assert(Concept::Addable<EnumT> == false);
	static_assert(Concept::Addable<ScopedEnumT> == false);
	static_assert(Concept::Addable<StructT> == false);

	static_assert(Concept::ContainerCompatibleRange<bool, bool> == false);
	static_assert(Concept::ContainerCompatibleRange<uint8, uint8> == false);
	static_assert(Concept::ContainerCompatibleRange<int32, int32> == false);
	static_assert(Concept::ContainerCompatibleRange<float, float> == false);
	static_assert(Concept::ContainerCompatibleRange<std::string, std::string> == false);
	static_assert(Concept::ContainerCompatibleRange<EnumT, EnumT> == false);
	static_assert(Concept::ContainerCompatibleRange<ScopedEnumT, ScopedEnumT> == false);
	static_assert(Concept::ContainerCompatibleRange<StructT, StructT> == false);
	static_assert(Concept::ContainerCompatibleRange<std::vector<int32>, int32> == true);
	static_assert(Concept::ContainerCompatibleRange<Array<int32>, int32> == true);
	static_assert(Concept::ContainerCompatibleRange<std::string_view, char> == true);
	static_assert(Concept::ContainerCompatibleRange<String, char32> == true);
	static_assert(Concept::ContainerCompatibleRange<StringView, char32> == true);
	static_assert(Concept::ContainerCompatibleRange<std::array<int32, 3>, int32> == true);
}
