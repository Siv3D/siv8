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
# include "../Error.hpp"
# include "../String.hpp"

namespace s3d
{	
	////////////////////////////////////////////////////////////////
	//
	//	InternalEngineError
	//
	////////////////////////////////////////////////////////////////

	/// @brief エンジン内部エラー | Internal engine error
	class InternalEngineError final : public Error
	{
	public:

		using Error::Error;
		
		////////////////////////////////////////////////////////////////
		//
		//	type
		//
		////////////////////////////////////////////////////////////////

		/// @brief エラーの種類を返します。 | Returns the type of the error.
		/// @return U"InternalEngineError"
		[[nodiscard]]
		constexpr String type() const override
		{
			return U"InternalEngineError";
		}
	};
}
