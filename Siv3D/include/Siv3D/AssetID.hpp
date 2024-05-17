﻿//-----------------------------------------------
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
# include "Number.hpp"

namespace s3d
{
	/// @brief アセット ID を表現する型
	/// @tparam AssetTag アセットタグ
	template <class AssetTag>
	class AssetID
	{
	public:

		/// @brief アセット ID に使う整数型
		using value_type = size_t;

		/// @brief Null アセットに使うアセット ID
		static constexpr value_type NullID = 0;

		/// @brief Invalid アセットに使うアセット ID
		static constexpr value_type InvalidID = Largest<value_type>;

		/// @brief デフォルトコンストラクタ 
		[[nodiscard]]
		AssetID() = default;

		/// @brief 指定した数値でアセット ID を作成します。
		/// @param id アセット ID に使う数値
		[[nodiscard]]
		explicit constexpr AssetID(value_type id) noexcept;

		/// @brief 別のアセット ID と等しいかを返します。
		/// @return 別のアセット ID と等しい場合 true, それ以外の場合は false
		[[nodiscard]]
		constexpr bool operator ==(const AssetID&) const = default;

		/// @brief 別のアセット ID の数値との大小関係を返します。
		/// @return 別のアセット ID の数値との大小関係
		[[nodiscard]]
		constexpr auto operator <=>(const AssetID&) const = default;

		/// @brief アセット ID の数値を返します。
		/// @return アセット ID の数値
		[[nodiscard]]
		constexpr value_type value() const noexcept;

		/// @brief アセット ID が Null であるかを返します。
		/// @return アセット ID が Null である場合 true, それ以外の場合は false
		[[nodiscard]]
		constexpr bool isNull() const noexcept;

		/// @brief アセット ID が Invalid であるかを返します。
		/// @return アセット ID が Invalid である場合 true, それ以外の場合は false
		[[nodiscard]]
		constexpr bool isInvalid() const noexcept;

		/// @brief Null アセット ID を作成します。
		/// @return Null アセット ID
		[[nodiscard]]
		static constexpr AssetID Null() noexcept;

		/// @brief Invalid アセット ID を作成します。
		/// @return Invalid アセット ID
		[[nodiscard]]
		static constexpr AssetID Invalid() noexcept;

	private:

		value_type m_value = NullID;
	};
}

template <class AssetTag>
struct std::hash<s3d::AssetID<AssetTag>>
{
	[[nodiscard]]
	size_t operator ()(const s3d::AssetID<AssetTag>& value) const noexcept
	{
		return std::hash<typename s3d::AssetID<AssetTag>::value_type>{}(value.value());
	}
};

# include "detail/AssetID.ipp"
