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
# include "Common.hpp"
# include "Number.hpp"
# include "FormatData.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	AssetID
	//
	////////////////////////////////////////////////////////////////

	/// @brief アセット ID を表現する型
	/// @tparam AssetTag アセットタグ
	template <class AssetTag>
	class AssetID
	{
	public:

		/// @brief アセット ID に使う整数型
		using value_type = uint32;

		/// @brief Null アセットに使うアセット ID
		static constexpr value_type NullID = 0;

		/// @brief Invalid アセットに使うアセット ID
		static constexpr value_type InvalidID = Largest<value_type>;

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		/// @brief デフォルトコンストラクタ 
		[[nodiscard]]
		AssetID() = default;

		/// @brief 指定した数値でアセット ID を作成します。
		/// @param id アセット ID に使う数値
		[[nodiscard]]
		explicit constexpr AssetID(value_type id) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	value
		//
		////////////////////////////////////////////////////////////////

		/// @brief アセット ID の数値を返します。
		/// @return アセット ID の数値
		[[nodiscard]]
		constexpr value_type value() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	isNull
		//
		////////////////////////////////////////////////////////////////

		/// @brief アセット ID が Null であるかを返します。
		/// @return アセット ID が Null である場合 true, それ以外の場合は false
		[[nodiscard]]
		constexpr bool isNull() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	isInvalid
		//
		////////////////////////////////////////////////////////////////

		/// @brief アセット ID が Invalid であるかを返します。
		/// @return アセット ID が Invalid である場合 true, それ以外の場合は false
		[[nodiscard]]
		constexpr bool isInvalid() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator ==, operator <=>
		//
		////////////////////////////////////////////////////////////////

		/// @brief 2 つのアセット ID が等しいかを返します。
		/// @param lhs 一方のアセット ID
		/// @param rhs もう一方のアセット ID
		/// @return 2 つのアセット ID が等しい場合 true, それ以外の場合は false
		[[nodiscard]]
		friend constexpr bool operator ==(const AssetID& lhs, const AssetID& rhs) = default;

		/// @brief 2 つのアセット ID の大小関係を返します。
		/// @param lhs 一方のアセット ID
		/// @param rhs もう一方のアセット ID
		/// @return 2 つのアセット ID の大小関係
		[[nodiscard]]
		friend constexpr auto operator <=>(const AssetID& lhs, const AssetID& rhs) = default;

		////////////////////////////////////////////////////////////////
		//
		//	Null
		//
		////////////////////////////////////////////////////////////////

		/// @brief Null アセット ID を作成します。
		/// @return Null アセット ID
		[[nodiscard]]
		static constexpr AssetID Null() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	Invalid
		//
		////////////////////////////////////////////////////////////////

		/// @brief Invalid アセット ID を作成します。
		/// @return Invalid アセット ID
		[[nodiscard]]
		static constexpr AssetID Invalid() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	Formatter
		//
		////////////////////////////////////////////////////////////////

		friend void Formatter(FormatData& formatData, const AssetID& value)
		{
			Formatter(formatData, value.value());
		}

	private:

		value_type m_value = NullID;
	};
}

////////////////////////////////////////////////////////////////
//
//	std::hash
//
////////////////////////////////////////////////////////////////

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
