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

# include <stdexcept>
# include <Siv3D/Array.hpp>

namespace s3d
{
	namespace detail
	{
		void ThrowArrayRemoveAtIndexOutOfRange()
		{
			throw std::out_of_range{ "Array::remove_at(): index out of range" };
		}

		void ThrowArrayRemovedAtIndexOutOfRange()
		{
			throw std::out_of_range{ "Array::removed_at(): index out of range" };
		}

		void ThrowArrayChoiceOutOfRange()
		{
			throw std::out_of_range{ "Array::choice(): Array is empty" };
		}

		void ThrowArrayValuesAtIndexOutOfRange()
		{
			throw std::out_of_range{ "Array::values_at(): index out of range" };
		}
	}
}
