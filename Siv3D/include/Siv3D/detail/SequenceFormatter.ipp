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

namespace s3d
{		
	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	void Formatter(FormatData& formatData, const std::span<Type> span)
	{
		Formatter(formatData, std::span<const Type>{ span });
	}

	template <class Type>
	void Formatter(FormatData& formatData, const std::span<const Type> span)
	{
		formatData.string.push_back(U'[');

		auto first = span.begin();
		const auto last = span.end();
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

	template <class Type, size_t N>
	void Formatter(FormatData& formatData, const Type(&values)[N])
	{
		Formatter(formatData, std::span<const Type>{ values });
	}

	template <class Type, size_t N>
	void Formatter(FormatData& formatData, const std::array<Type, N>& v)
	{
		Formatter(formatData, std::span<const Type>{ v });
	}

	template <class Type, class Allocator>
	void Formatter(FormatData& formatData, const std::vector<Type, Allocator>& v)
	{
		Formatter(formatData, std::span{ v });
	}

	template <class Type, class Allocator>
	void Formatter(FormatData& formatData, const Array<Type, Allocator>& v)
	{
		Formatter(formatData, std::span{ v });
	}

	template <class Type>
	void Formatter(FormatData& formatData, const std::initializer_list<Type> list)
	{
		Formatter(formatData, std::span{ list });
	}
}
