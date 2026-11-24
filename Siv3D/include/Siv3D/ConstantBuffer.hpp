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

# pragma once
# include <memory>
# include "Common.hpp"
# include "Memory.hpp"
# include "Utility.hpp"
# include "IConstantBuffer.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	ConstantBuffer
	//
	////////////////////////////////////////////////////////////////

	/// @brief 定数バッファ | Constant buffer
	/// @tparam Type データの型 | Data type
	/// @remark Type は trivially copyable で、サイズは 64 KiB 以下である必要があります。
	/// @remark メンバの配置はシェーダの宣言に合わせてください。サイズの 16 バイト単位への切り上げは、メンバ間や配列要素間のパディングを調整しません。
	template <class Type>
	class ConstantBuffer
	{
	public:

		static_assert(sizeof(Type) <= (16 * 4096)); // <= 64KB

		static_assert(std::is_trivially_copyable_v<Type>);

		static constexpr size_t _alignment = Max<size_t>(alignof(Type), 16);

	SIV3D_DISABLE_MSVC_WARNINGS_PUSH(4324)

		struct alignas(_alignment) WrapperType
		{
			Type data;
		};

	SIV3D_DISABLE_MSVC_WARNINGS_POP()

		/// @brief 定数バッファのサイズ（バイト） | Size of the constant buffer in bytes
		static constexpr size_t Size		= sizeof(WrapperType);

		/// @brief 定数バッファのベクトル数 | Number of vectors in the constant buffer
		static constexpr size_t NumVectors	= (Size / 16);

		/// @brief 定数バッファのアライメント | Alignment of the constant buffer
		static constexpr size_t Alignment	= alignof(WrapperType);

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		/// @brief デフォルトコンストラクタ | Default constructor
		[[nodiscard]]
		ConstantBuffer();

		/// @brief データを指定して定数バッファを作成します。 | Creates a constant buffer with the specified data.
		/// @param data データ | Data
		[[nodiscard]]
		ConstantBuffer(const Type& data);

		/// @brief データを指定して定数バッファを作成します。 | Creates a constant buffer with the specified data.
		/// @tparam ...Args データのコンストラクタ引数の型 | Types of the constructor arguments of the data
		/// @param ...args データのコンストラクタ引数 | Constructor arguments of the data
		template <class... Args>
			requires std::is_constructible_v<Type, Args...>
		[[nodiscard]]
		explicit ConstantBuffer(Args&&... args);

		////////////////////////////////////////////////////////////////
		//
		//	operator =
		//
		////////////////////////////////////////////////////////////////

		/// @brief 有効サイズを保ったままデータを代入します。
		/// @param data 代入するデータ
		/// @return *this
		ConstantBuffer& operator =(const Type& data);

		////////////////////////////////////////////////////////////////
		//
		//	size
		//
		////////////////////////////////////////////////////////////////

		/// @brief 有効サイズによらない格納容量（バイト）を返します。
		/// @return Size
		[[nodiscard]]
		constexpr size_t size() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	setActiveSize, activeSize
		//
		////////////////////////////////////////////////////////////////

		/// @brief シェーダに渡す先頭領域の有効サイズを設定します。
		/// @param size バイト数。16 以上、Size 以下の 16 の倍数。
		/// @remark 初期値は Size です。サイズが変わるとダーティになります。格納容量と CPU 側の値は変化しません。
		/// @remark シェーダの宣言は有効領域に適合させ、領域外を参照してはいけません。GPU 側の領域外の値は保証されません。
		/// @throw Error size が条件を満たさない場合。値と有効サイズ、ダーティフラグは変化しません。
		void setActiveSize(size_t size);

		/// @brief シェーダに渡す先頭領域の有効サイズ（バイト）を返します。
		/// @return 有効サイズ（バイト）
		[[nodiscard]]
		size_t activeSize() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	data
		//
		////////////////////////////////////////////////////////////////

		/// @brief データの先頭ポインタを返します。
		/// @return データの先頭ポインタ
		[[nodiscard]]
		const float* data() const noexcept;

		/// @brief 内部用。末尾のパディングを含む Size バイトの格納領域を返します。
		[[nodiscard]]
		const void* _data() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	setDirty
		//
		////////////////////////////////////////////////////////////////

		/// @brief データのダーティフラグを設定します。
		/// @param dirty データをダーティにする場合 true, ダーティを解除する場合は false
		void setDirty(bool dirty) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	isDirty
		//
		////////////////////////////////////////////////////////////////

		/// @brief 変更されたデータがあるかを返します。
		/// @return 変更されたデータがある場合 true, それ以外の場合は false
		[[nodiscard]]
		bool isDirty() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator *
		//
		////////////////////////////////////////////////////////////////

		/// @brief データへの参照を返します。
		/// @return データへの参照
		[[nodiscard]]
		Type& operator *() noexcept;

		/// @brief データへの参照を返します。
		/// @return データへの参照
		[[nodiscard]]
		const Type& operator *() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator ->
		//
		////////////////////////////////////////////////////////////////

		/// @brief データへのポインタを返します。
		/// @return データへのポインタ
		[[nodiscard]]
		Type* operator ->() noexcept;

		/// @brief データへのポインタを返します。
		/// @return データへのポインタ
		[[nodiscard]]
		const Type* operator ->() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	_base
		//
		////////////////////////////////////////////////////////////////

		/// @brief 内部用 | For internal use
		/// @return 内部用の情報 | Internal information
		[[nodiscard]]
		IConstantBuffer* _base() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	_update_if_dirty
		//
		////////////////////////////////////////////////////////////////

		/// @brief 内部用 | For internal use
		/// @return 内部用の情報 | Internal information
		bool _update_if_dirty();

	private:

		struct Deleter
		{
			void operator()(WrapperType* p) const
			{
				AlignedDelete<WrapperType>(p, Alignment);
			}
		};

		std::unique_ptr<WrapperType, Deleter> m_wrapper{ AlignedNew<WrapperType>() };

		std::unique_ptr<IConstantBuffer> m_base;

		size_t m_activeSize = Size;

		bool m_isDirty = true;
	};
}

# include "detail/ConstantBuffer.ipp"
