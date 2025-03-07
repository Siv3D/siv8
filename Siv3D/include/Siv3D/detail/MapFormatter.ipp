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

namespace s3d
{		
	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	template <class Key, class Compare, class Alloc>
	void Formatter(FormatData& formatData, const BTreeSet<Key, Compare, Alloc>& set)
	{
		formatData.string.push_back(U'[');

		auto first = set.begin();
		const auto last = set.end();
		bool isFirst = true;

		while (first != last)
		{
			if (isFirst)
			{
				isFirst = false;
			}
			else
			{
				formatData.string.append(U", "_sv);
			}

			DebugFormatter(formatData, *first);

			++first;
		}

		formatData.string.push_back(U']');
	}

	template <class Key, class Value, class Compare, class Alloc>
	void Formatter(FormatData& formatData, const BTreeMap<Key, Value, Compare, Alloc>& table)
	{
		formatData.string.push_back(U'{');

		auto first = table.begin();
		const auto last = table.end();
		bool isFirst = true;

		while (first != last)
		{
			if (isFirst)
			{
				isFirst = false;
			}
			else
			{
				formatData.string.append(U", "_sv);
			}

			DebugFormatter(formatData, first->first);
			formatData.string.append(U": "_sv);
			DebugFormatter(formatData, first->second);

			++first;
		}

		formatData.string.push_back(U'}');
	}

	template <class Key, class Hash, class Eq, class Alloc>
	void Formatter(FormatData& formatData, const HashSet<Key, Hash, Eq, Alloc>& set)
	{
		formatData.string.push_back(U'[');

		auto first = set.begin();
		const auto last = set.end();
		bool isFirst = true;

		while (first != last)
		{
			if (isFirst)
			{
				isFirst = false;
			}
			else
			{
				formatData.string.append(U", "_sv);
			}

			DebugFormatter(formatData, *first);

			++first;
		}

		formatData.string.push_back(U']');
	}

	template <class Key, class Value, class Hash, class Eq, class Alloc>
	void Formatter(FormatData& formatData, const HashTable<Key, Value, Hash, Eq, Alloc>& table)
	{
		formatData.string.push_back(U'{');

		auto first = table.begin();
		const auto last = table.end();
		bool isFirst = true;

		while (first != last)
		{
			if (isFirst)
			{
				isFirst = false;
			}
			else
			{
				formatData.string.append(U", "_sv);
			}

			DebugFormatter(formatData, first->first);
			formatData.string.append(U": "_sv);
			DebugFormatter(formatData, first->second);

			++first;
		}

		formatData.string.push_back(U'}');
	}
}
