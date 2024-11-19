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
# include "Vertex2D.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	TriangleIndex
	//
	////////////////////////////////////////////////////////////////

	/// @brief 三角形を構成する頂点インデックス
	struct alignas(alignof(Vertex2D::IndexType)) TriangleIndex
	{
		/// @brief インデックスの値の型
		using value_type = Vertex2D::IndexType;

		/// @brief 頂点 0
		value_type i0;

		/// @brief 頂点 1
		value_type i1;

		/// @brief 頂点 2
		value_type i2;

		////////////////////////////////////////////////////////////////
		//
		//	flip
		//
		////////////////////////////////////////////////////////////////

		/// @brief 三角形の頂点の回転方向を反転させます。
		/// @remark { i0, i1, i2 } が { i0, i2, i1 } に変換されます。
		constexpr void flip() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	Zero
		//
		////////////////////////////////////////////////////////////////

		/// @brief すべてのインデックス値が 0 の TriangleIndex を返します。
		/// @return すべてのインデックス値が 0 の TriangleIndex
		[[nodiscard]]
		static constexpr TriangleIndex Zero() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	Formatter
		//
		////////////////////////////////////////////////////////////////

		/// @brief 三角形を構成する頂点インデックスを文字列に変換します。
		/// @param formatData 文字列バッファ
		/// @param value 三角形を構成する頂点インデックス
		/// @remark この関数は Format 用の関数です。通常、ユーザーが直接呼び出す必要はありません。
		friend void Formatter(FormatData& formatData, const TriangleIndex& value);
	};
}

# include "detail/TriangleIndex.ipp"
