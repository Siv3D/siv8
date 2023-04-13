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
	namespace detail
	{
		template <class Tuple, size_t... Is>
		void FormatTupleImpl(FormatData& formatData, const Tuple& tuple, std::index_sequence<Is...>)
		{
			((Formatter(formatData, std::get<Is>(tuple)), formatData.string.append(((Is + 1) == sizeof...(Is)) ? U"" : U", "_sv)), ...);
		}
	}

	template <class... Ts>
	inline void Formatter(FormatData& formatData, const std::tuple<Ts...>& tuple)
	{
		formatData.string.push_back(U'{');
		detail::FormatTupleImpl(formatData, tuple, std::index_sequence_for<Ts...>{});
		formatData.string.push_back(U'}');
	}

	template <class First, class Second>
	inline void Formatter(FormatData& formatData, const std::pair<First, Second>& pair)
	{
		Formatter(formatData, std::tie(pair.first, pair.second));
	}
}
