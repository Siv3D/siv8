//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2023 Ryo Suzuki
//	Copyright (c) 2016-2023 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once
# include "BTreeSet.hpp"
# include "BTreeMap.hpp"
# include "HashSet.hpp"
# include "HashMap.hpp"
# include "Formatter.hpp"
# include "EscapedFormatter.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	template <class Key, class Compare, class Alloc>
	void Formatter(FormatData& formatData, const BTreeSet<Key, Compare, Alloc>& set);

	template <class Key, class Value, class Compare, class Alloc>
	void Formatter(FormatData& formatData, const BTreeMap<Key, Value, Compare, Alloc>& table);

	template <class Key, class Hash, class Eq, class Alloc>
	void Formatter(FormatData& formatData, const HashSet<Key, Hash, Eq, Alloc>& set);

	template <class Key, class Value, class Hash, class Eq, class Alloc>
	void Formatter(FormatData& formatData, const HashMap<Key, Value, Hash, Eq, Alloc>& table);
}

# include "detail/MapFormatter.ipp"
