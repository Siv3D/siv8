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

# include <Siv3D/Grid.hpp>

namespace s3d
{
	namespace detail
	{
		void ThrowGridAtOutOfRange()
		{
			throw std::out_of_range{ "Grid::at(): index out of range" };
		}
	}
}
