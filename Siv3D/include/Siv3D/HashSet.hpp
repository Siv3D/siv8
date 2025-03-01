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
	//	HashSet
	//
	////////////////////////////////////////////////////////////////

	template <class Key,
		class Hash = std::conditional_t<std::is_same_v<Key, String>, StringHash, phmap::priv::hash_default_hash<Key>>,
		class Eq = std::conditional_t<std::is_same_v<Key, String>, StringCompare, phmap::priv::hash_default_eq<Key>>,
		class Alloc = phmap::priv::Allocator<Key>>
	using HashSet = phmap::flat_hash_set<Key, Hash, Eq, Alloc>;
}

# if SIV3D_INTRINSIC(SSE)
#	undef PHMAP_HAVE_SSE2
#	undef PHMAP_HAVE_SSSE3
# endif
