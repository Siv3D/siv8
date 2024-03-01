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
	namespace detail
	{
		template <class Type>
		void FormatTupleElement(FormatData& formatData, const Type& value, const bool isFirst)
		{
			if (not isFirst)
			{
				formatData.string.append(U", "_sv);
			}

			DebugFormatter(formatData, value);
		}
	}
		
	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	template <class... Ts>
	void Formatter(FormatData& formatData, const std::tuple<Ts...>& tuple)
	{
		formatData.string.push_back(U'(');
	
		std::apply([&formatData](const auto&... args)
			{
				bool isFirst = true;
				(detail::FormatTupleElement(formatData, args, std::exchange(isFirst, false)), ...);
			}, tuple);

		formatData.string.push_back(U')');
	}

	template <class First, class Second>
	void Formatter(FormatData& formatData, const std::pair<First, Second>& pair)
	{
		Formatter(formatData, std::tie(pair.first, pair.second));
	}
}
