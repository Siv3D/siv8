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
# include "../Error.hpp"

namespace s3d
{		
	////////////////////////////////////////////////////////////////
	//
	//	ParseError
	//
	////////////////////////////////////////////////////////////////

	/// @brief パースエラー | Parse error
	class ParseError final : public Error
	{
	public:

		using Error::Error;
		
		////////////////////////////////////////////////////////////////
		//
		//	type
		//
		////////////////////////////////////////////////////////////////

		/// @brief エラーの種類を返します。 | Returns the type of the error.
		/// @return U"ParseError"
		[[nodiscard]]
		constexpr String type() const override
		{
			return U"ParseError";
		}
	};
}
