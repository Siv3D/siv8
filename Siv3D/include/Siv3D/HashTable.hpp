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
# if SIV3D_INTRINSIC(SSE)
#	define PHMAP_HAVE_SSE2 1
#	define PHMAP_HAVE_SSSE3 1
# endif
# include <ThirdParty/parallel_hashmap/phmap.h>
# include "HeterogeneousLookupHelper.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	HashTable
	//
	////////////////////////////////////////////////////////////////

	/// @brief ハッシュテーブル
	/// @tparam Key キーの型
	/// @tparam Value 値の型
	/// @tparam Hash ハッシュ関数の型
	/// @tparam Eq キーの比較関数の型
	/// @tparam Alloc アロケータの型
	template <class Key, class Value,
		class Hash = std::conditional_t<std::is_same_v<Key, String>, StringHash, phmap::priv::hash_default_hash<Key>>,
		class Eq = std::conditional_t<std::is_same_v<Key, String>, StringCompare, phmap::priv::hash_default_eq<Key>>,
		class Alloc = phmap::priv::Allocator<phmap::priv::Pair<const Key, Value>>>
	using HashTable = phmap::flat_hash_map<Key, Value, Hash, Eq, Alloc>;
}

# if SIV3D_INTRINSIC(SSE)
#	undef PHMAP_HAVE_SSE2
#	undef PHMAP_HAVE_SSSE3
# endif
