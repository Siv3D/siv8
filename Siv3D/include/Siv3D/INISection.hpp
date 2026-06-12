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
# include "Array.hpp"
# include "INIItem.hpp"
# include "INIWriteOptions.hpp"
# include "HashMap.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	INISection
	//
	////////////////////////////////////////////////////////////////

	/// @brief INI ファイルの 1 つのセクションを表す構造体 | Structure representing one section in an INI file
	/// @remark セクション内の値は INIItem として文字列で保持されます。 | Values in the section are stored as strings in INIItem.
	/// @remark items を直接走査した場合、プロパティの順序は保証されません。ファイル上または追加順の順序で取得するには propertyView() を使います。 | Iteration order of items is not guaranteed. Use propertyView() to access properties in file or insertion order.
	struct INISection
	{
		/// @brief プロパティ用 HashMap のイテレータ型 | Iterator type for the property HashMap
		using iterator = HashMap<String, INIItem>::iterator;

		/// @brief プロパティ用 HashMap の const イテレータ型 | Const iterator type for the property HashMap
		using const_iterator = HashMap<String, INIItem>::const_iterator;

		/// @brief セクション内のプロパティ | Property in a section
		struct Property
		{
			/// @brief キー | Key
			String key;

			/// @brief 値 | Value
			String value;

			/// @brief セクション内でのインデックス（小さいほど上位） | Index in the section. Smaller values appear earlier.
			int32 index = 0;
		};

		/// @brief セクション名 | Section name
		/// @remark 空文字列はグローバル（無名）セクションを表します。 | An empty string represents the global section.
		String name;

		/// @brief プロパティ | Properties
		/// @remark 直接走査した場合、順序は保証されません。ファイル上の順序で取得するには propertyView() を使います。 | Iteration order is not guaranteed. Use propertyView() to access properties in file order.
		/// @remark 直接変更すると、プロパティの順序情報が壊れる可能性があります。通常は addProperty() / removeProperty() を使います。 | Direct modification may break property ordering information. Prefer addProperty() and removeProperty().
		HashMap<String, INIItem> items;

		////////////////////////////////////////////////////////////////
		//
		//	hasProperty
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定したプロパティが存在するかを返します。 | Returns whether the specified property exists.
		/// @param key プロパティのキー | Property key
		/// @return プロパティが存在する場合 true, それ以外の場合は false | True if the property exists, otherwise false
		[[nodiscard]]
		bool hasProperty(StringView key) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	addProperty
		//
		////////////////////////////////////////////////////////////////

		/// @brief プロパティを追加または更新します。 | Adds or updates a property.
		/// @param key プロパティのキー | Property key
		/// @param value プロパティの値 | Property value
		/// @remark value は Format() により文字列化されます。 | value is converted to a string by Format().
		/// @remark 既に同じキーが存在する場合、値を更新します。 | If the same key already exists, its value is updated.
		void addProperty(StringView key, const Concept::Formattable auto& value);

		////////////////////////////////////////////////////////////////
		//
		//	removeProperty
		//
		////////////////////////////////////////////////////////////////

		/// @brief プロパティを削除します。 | Removes a property.
		/// @param key 削除するプロパティのキー | Property key to remove
		/// @remark 指定したプロパティが存在しない場合は何もしません。 | Does nothing if the specified property does not exist.
		void removeProperty(StringView key);

		////////////////////////////////////////////////////////////////
		//
		//	operator []
		//
		////////////////////////////////////////////////////////////////

		/// @brief プロパティへの参照を返します。 | Returns a reference to the property.
		/// @param key プロパティのキー | Property key
		/// @return プロパティへの参照 | Reference to the property
		/// @throw Error 指定したプロパティが存在しない場合 | Throws Error if the specified property does not exist.
		[[nodiscard]]
		const INIItem& operator [](StringView key) const;

		/// @brief プロパティへの参照を返します。 | Returns a reference to the property.
		/// @param key プロパティのキー | Property key
		/// @return プロパティへの参照 | Reference to the property
		/// @throw Error 指定したプロパティが存在しない場合 | Throws Error if the specified property does not exist.
		[[nodiscard]]
		INIItem& operator [](StringView key);

		////////////////////////////////////////////////////////////////
		//
		//	begin, end
		//
		////////////////////////////////////////////////////////////////

		/// @brief プロパティの走査開始位置を返します。 | Returns an iterator to the beginning of properties.
		/// @return プロパティの先頭を指すイテレータ | Iterator to the beginning of properties
		/// @remark 走査順序はファイル上の順序とは限りません。順序付きで取得するには propertyView() を使います。 | Iteration order is not guaranteed to match file order. Use propertyView() for ordered access.
		[[nodiscard]]
		iterator begin() noexcept;

		/// @brief プロパティの走査終了位置を返します。 | Returns an iterator to the end of properties.
		/// @return プロパティの終端を指すイテレータ | Iterator to the end of properties
		[[nodiscard]]
		iterator end() noexcept;

		/// @brief プロパティの走査開始位置を返します。 | Returns an iterator to the beginning of properties.
		/// @return プロパティの先頭を指す const イテレータ | Const iterator to the beginning of properties
		/// @remark 走査順序はファイル上の順序とは限りません。順序付きで取得するには propertyView() を使います。 | Iteration order is not guaranteed to match file order. Use propertyView() for ordered access.
		[[nodiscard]]
		const_iterator begin() const noexcept;

		/// @brief プロパティの走査終了位置を返します。 | Returns an iterator to the end of properties.
		/// @return プロパティの終端を指す const イテレータ | Const iterator to the end of properties
		[[nodiscard]]
		const_iterator end() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	cbegin, cend
		//
		////////////////////////////////////////////////////////////////

		/// @brief プロパティの走査開始位置を返します。 | Returns an iterator to the beginning of properties.
		/// @return プロパティの先頭を指す const イテレータ | Const iterator to the beginning of properties
		/// @remark 走査順序はファイル上の順序とは限りません。順序付きで取得するには propertyView() を使います。 | Iteration order is not guaranteed to match file order. Use propertyView() for ordered access.
		[[nodiscard]]
		const_iterator cbegin() const noexcept;

		/// @brief プロパティの走査終了位置を返します。 | Returns an iterator to the end of properties.
		/// @return プロパティの終端を指す const イテレータ | Const iterator to the end of properties
		[[nodiscard]]
		const_iterator cend() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	get
		//
		////////////////////////////////////////////////////////////////

		/// @brief プロパティ値を指定した型に変換して返します。 | Returns the property value converted to the specified type.
		/// @tparam Type 取得する型 | Type to get
		/// @param key プロパティのキー | Property key
		/// @return 変換された値 | Converted value
		/// @throw Error プロパティが存在しない場合、または値の変換に失敗した場合 | Throws Error if the property does not exist or conversion fails.
		template <class Type>
		[[nodiscard]]
		Type get(StringView key) const;

		////////////////////////////////////////////////////////////////
		//
		//	getOr
		//
		////////////////////////////////////////////////////////////////

		/// @brief プロパティ値を指定した型に変換して返します。失敗した場合はデフォルト値を返します。 | Returns the property value converted to the specified type, or a default value on failure.
		/// @tparam Type 取得する型 | Type to get
		/// @tparam U デフォルト値の型 | Type of the default value
		/// @param key プロパティのキー | Property key
		/// @param defaultValue デフォルト値 | Default value
		/// @return 変換された値。プロパティが存在しない場合、または値の変換に失敗した場合は defaultValue | Converted value, or defaultValue if the property does not exist or conversion fails
		template <class Type, class U>
		[[nodiscard]]
		Type getOr(StringView key, U&& defaultValue) const;

		////////////////////////////////////////////////////////////////
		//
		//	getOpt
		//
		////////////////////////////////////////////////////////////////

		/// @brief プロパティ値を指定した型に変換して返します。 | Returns the property value converted to the specified type.
		/// @tparam Type 取得する型 | Type to get
		/// @param key プロパティのキー | Property key
		/// @return 変換に成功した場合はその値、失敗した場合は none | Converted value if successful, otherwise none
		/// @remark プロパティが存在しない場合も none を返します。 | Returns none if the property does not exist.
		template <class Type>
		[[nodiscard]]
		Optional<Type> getOpt(StringView key) const;

		////////////////////////////////////////////////////////////////
		//
		//	propertyView
		//
		////////////////////////////////////////////////////////////////

		/// @brief セクション内のプロパティを追加順に返します。 | Returns properties in insertion order.
		/// @return プロパティの一覧 | Properties
		/// @remark 返される配列はコピーです。 | The returned array is a copy.
		[[nodiscard]]
		Array<Property> propertyView() const;

		////////////////////////////////////////////////////////////////
		//
		//	format
		//
		////////////////////////////////////////////////////////////////

		/// @brief セクションを INI 形式の文字列に変換します。 | Formats the section as INI text.
		/// @param options 書き出しオプション | Write options
		/// @return INI 形式の文字列 | INI text
		/// @remark セクション名が空の場合、セクションヘッダは出力されません。 | If the section name is empty, the section header is not written.
		[[nodiscard]]
		String format(const INIWriteOptions& options = {}) const;

		////////////////////////////////////////////////////////////////
		//
		//	_addProperty
		//
		////////////////////////////////////////////////////////////////

		/// @brief プロパティを内部表現に追加します。 | Adds a property to the internal representation.
		/// @param key プロパティのキー | Property key
		/// @param value プロパティの値 | Property value
		/// @remark 内部実装用です。ユーザコードでは通常 addProperty() を使います。 | For internal implementation. User code should normally use addProperty().
		void _addProperty(StringView key, String&& value);
	};
}

# include "detail/INISection.ipp"
