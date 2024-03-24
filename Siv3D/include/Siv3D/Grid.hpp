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
# include "Common.hpp"
# include "Array.hpp"
# include "PointVector.hpp"

namespace s3d
{
	template <class Type, class Allocator>
	class Grid;

	/// @brief 二次元配列クラス
	/// @tparam Type 要素の型
	/// @tparam Allocator アロケータの型
	template <class Type, class Allocator = std::allocator<Type>>
	class Grid
	{
	public:

		using container_type			= Array<Type, Allocator>;
		using value_type				= typename container_type::value_type;
		using pointer					= typename container_type::pointer;
		using const_pointer				= typename container_type::const_pointer;
		using reference					= typename container_type::reference;
		using const_reference			= typename container_type::const_reference;
		using iterator					= typename container_type::iterator;
		using const_iterator			= typename container_type::const_iterator;
		using reverse_iterator			= typename container_type::reverse_iterator;
		using const_reverse_iterator	= typename container_type::const_reverse_iterator;
		using size_type					= typename container_type::size_type;
		using difference_type			= typename container_type::difference_type;
		using allocator_type			= typename container_type::allocator_type;

	private:

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Grid() = default;

		[[nodiscard]]
		Grid(const Grid&) = default;

		[[nodiscard]]
		Grid(Grid&&) = default;






		////////////////////////////////////////////////////////////////
		//
		//	operator =
		//
		////////////////////////////////////////////////////////////////

		/// @brief コピー代入演算子
		/// @param other コピーする配列
		/// @return *this
		Grid& operator =(const Grid&) = default;

		/// @brief ムーブ代入演算子
		/// @param other ムーブする配列
		/// @return *this
		Grid& operator =(Grid&&) = default;

	private:

		container_type m_container;

		size_type m_width = 0;

		size_type m_height = 0;
	};
}

# include "detail/Grid.ipp"
