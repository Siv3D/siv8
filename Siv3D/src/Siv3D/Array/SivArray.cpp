//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2026 Ryo Suzuki
//	Copyright (c) 2016-2026 OpenSiv3D Project
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
		void ThrowArrayEraseAtIndexOutOfRange()
		{
			throw std::out_of_range{ "Array::erase_at(): index out of range" };
		}

		void ThrowArrayEraseAtUnstableIndexOutOfRange()
		{
			throw std::out_of_range{ "Array::erase_at_unstable(): index out of range" };
		}

		void ThrowArrayWithoutAtIndexOutOfRange()
		{
			throw std::out_of_range{ "Array::without_at(): index out of range" };
		}

		void ThrowArraySliceIndexOutOfRange()
		{
			throw std::out_of_range{ "Array::slice(): index out of range" };
		}

		void ThrowArrayChoiceOutOfRange()
		{
			throw std::out_of_range{ "Array::choice(): Array is empty" };
		}

		void ThrowArrayValuesAtIndexOutOfRange()
		{
			throw std::out_of_range{ "Array::values_at(): index out of range" };
		}

		void ThrowArrayRotateMiddleOutOfRange()
		{
			throw std::out_of_range{ "Array::rotate(): middle index out of range" };
		}
	}
}
