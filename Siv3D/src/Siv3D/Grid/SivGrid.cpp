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

# include <Siv3D/Grid.hpp>

namespace s3d
{
	namespace detail
	{
		void ThrowGridAtOutOfRange()
		{
			throw std::out_of_range{ "Grid::at(): index out of range" };
		}

		void ThrowGridPopBackRowOutOfRange()
		{
			// 高さが 0 であるため、行を削除できません。
			throw std::out_of_range{ "Grid::pop_back_row(): Grid height is 0" };
		}

		void ThrowGridPopBackColumnOutOfRange()
		{
			// 幅が 0 であるため、列を削除できません。
			throw std::out_of_range{ "Grid::pop_back_column(): Grid width is 0" };
		}

		void ThrowGridInsertRowOutOfRange()
		{
			// 指定した行が存在しないため、行を挿入できません。
			throw std::out_of_range{ "Grid::insert_row(): index out of range" };
		}

		void ThrowGridInsertRowsOutOfRange()
		{
			// 指定した行が存在しないため、行を挿入できません。
			throw std::out_of_range{ "Grid::insert_rows(): index out of range" };
		}

		void ThrowGridInsertColumnOutOfRange()
		{
			// 指定した列が存在しないため、列を挿入できません。
			throw std::out_of_range{ "Grid::insert_column(): index out of range" };
		}

		void ThrowGridInsertColumnsOutOfRange()
		{
			// 指定した列が存在しないため、列を挿入できません。
			throw std::out_of_range{ "Grid::insert_columns(): index out of range" };
		}

		void ThrowGridRemoveRowOutOfRange()
		{
			// 指定した行が存在しないため、行を削除できません。
			throw std::out_of_range{ "Grid::remove_row(): index out of range" };
		}

		void ThrowGridRemoveRowsOutOfRange()
		{
			// 指定した行が存在しないため、行を削除できません。
			throw std::out_of_range{ "Grid::remove_rows(): index out of range" };
		}

		void ThrowGridRemoveColumnOutOfRange()
		{
			// 指定した列が存在しないため、列を削除できません。
			throw std::out_of_range{ "Grid::remove_column(): index out of range" };
		}

		void ThrowGridRemoveColumnsOutOfRange()
		{
			// 指定した列が存在しないため、列を削除できません。
			throw std::out_of_range{ "Grid::remove_columns(): index out of range" };
		}
	}
}
