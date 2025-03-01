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
# include <ThirdParty/parallel_hashmap/btree.h>

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	OrderedSet
	//
	////////////////////////////////////////////////////////////////

	/// @brief テーブル
	/// @tparam Key キーの型
	/// @tparam Compare キーの比較関数の型
	/// @tparam Alloc アロケータの型
	template <class Key,
		class Compare = phmap::Less<Key>,
		class Alloc = phmap::priv::Allocator<Key>>
	using OrderedSet = phmap::btree_set<Key, Compare, Alloc>;
}

# if SIV3D_INTRINSIC(SSE)
#	undef PHMAP_HAVE_SSE2
#	undef PHMAP_HAVE_SSSE3
# endif
