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
# include "String.hpp"
# include "Array.hpp"
# include "HashTable.hpp"
# include "Optional.hpp"
# include "IReader.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	INI
	//
	////////////////////////////////////////////////////////////////

	/// @brief INI 形式のデータの読み書き
	class INI
	{
	public:

		////////////////////////////////////////////////////////////////
		//
		//	INI::Item
		//
		////////////////////////////////////////////////////////////////

		struct Item
		{
			/// @brief プロパティの値
			String value;

			/// @brief セクション内でのインデックス（小さいほど上位）
			int32 index = 0;

			////////////////////////////////////////////////////////////////
			//
			//	get
			//
			////////////////////////////////////////////////////////////////

			/// @brief 値を Type 型として取得します。
			/// @tparam Type 値の型
			/// @return 値
			/// @throw Error JSON の値を Type 型に変換できない場合
			template <class Type>
			[[nodiscard]]
			Type get() const;

			////////////////////////////////////////////////////////////////
			//
			//	getOr
			//
			////////////////////////////////////////////////////////////////

			/// @brief 値を Type 型として取得します。取得できなかった場合は defaultValue を返します。
			/// @tparam Type 値の型
			/// @tparam U 取得できなかった場合に返す値の型
			/// @param defaultValue 取得できなかった場合に返す値
			/// @return 値
			template <class Type, class U>
			[[nodiscard]]
			Type getOr(U&& defaultValue) const;

			////////////////////////////////////////////////////////////////
			//
			//	getOpt
			//
			////////////////////////////////////////////////////////////////

			/// @brief 値を Type 型として取得します。
			/// @tparam Type 値の型
			/// @return 値。取得できなかった場合は none
			template <class Type>
			[[nodiscard]]
			Optional<Type> getOpt() const;
		};

		////////////////////////////////////////////////////////////////
		//
		//	INI::Section
		//
		////////////////////////////////////////////////////////////////

		/// @brief INI ファイルのセクション
		struct Section
		{
			/// @brief セクション名
			String name;

			/// @brief プロパティ
			HashTable<String, Item> items;

			[[nodiscard]]
			bool hasProperty(StringView key) const noexcept;

			void addProperty(StringView key, StringView value);

			[[nodiscard]]
			const Item& operator [](StringView key) const;

			[[nodiscard]]
			Item& operator [](StringView key);
		};

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		/// @brief デフォルトコンストラクタ
		[[nodiscard]]
		INI() = default;

		/// @brief INI 形式のデータをファイルからロードして作成します。
		/// @param path ファイルパス
		/// @remark コンストラクタでファイルの内容を読み取り、以降はファイルにはアクセスしません。
		[[nodiscard]]
		explicit INI(FilePathView path);
		
		template <class Reader>
			requires (std::is_base_of_v<IReader, Reader> && (not std::is_lvalue_reference_v<Reader>))
		[[nodiscard]]
		explicit INI(Reader&& reader);

		[[nodiscard]]
		explicit INI(std::unique_ptr<IReader>&& reader);

		////////////////////////////////////////////////////////////////
		//
		//	load
		//
		////////////////////////////////////////////////////////////////

		bool load(FilePathView path);

		template <class Reader>
			requires (std::is_base_of_v<IReader, Reader> && (not std::is_lvalue_reference_v<Reader>))
		bool load(Reader&& reader);

		bool load(std::unique_ptr<IReader>&& reader);

		////////////////////////////////////////////////////////////////
		//
		//	parse
		//
		////////////////////////////////////////////////////////////////
		
		bool parse(std::string_view s);

		bool parse(StringView s);

		////////////////////////////////////////////////////////////////
		//
		//	isEmpty
		//
		////////////////////////////////////////////////////////////////

		/// @brief データが空であるかを返します。
		/// @return データが空である場合 true, それ以外の場合は false
		[[nodiscard]]
		bool isEmpty() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator bool
		//
		////////////////////////////////////////////////////////////////

		/// @brief データが空でないかを返します。
		/// @return データが空でない場合 true, それ以外の場合は false
		[[nodiscard]]
		explicit operator bool() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	clear
		//
		////////////////////////////////////////////////////////////////
		
		/// @brief 保持しているデータを消去します。
		void clear();

		////////////////////////////////////////////////////////////////
		//
		//	sections
		//
		////////////////////////////////////////////////////////////////

		/// @brief セクションの一覧を返します。
		/// @return セクションの一覧
		[[nodiscard]]
		const Array<Section>& sections() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	hasSection
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定したセクションが存在するかを返します。
		/// @param section セクション名。空文字列の場合はグローバル（無名）セクション
		/// @return 指定したセクションが存在する場合 true, それ以外の場合は false
		[[nodiscard]]
		bool hasSection(StringView section) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	getSection
		//
		////////////////////////////////////////////////////////////////

		/// @brief セクションへの参照を返します。
		/// @param section セクション名。空文字列の場合はグローバル（無名）セクション
		/// @return セクションへの参照
		/// @throw Error 指定したセクションが存在しない場合
		[[nodiscard]]
		const Section& getSection(StringView section) const;

		/// @brief セクションへの参照を返します。
		/// @param section セクション名。空文字列の場合はグローバル（無名）セクション
		/// @return セクションへの参照
		/// @throw Error 指定したセクションが存在しない場合
		[[nodiscard]]
		Section& getSection(StringView section);

		////////////////////////////////////////////////////////////////
		//
		//	hasGlobalSection
		//
		////////////////////////////////////////////////////////////////

		/// @brief グローバル（無名）セクションが存在するかを返します。
		/// @return グローバル（無名）セクションが存在する場合 true, それ以外の場合は false
		[[nodiscard]]
		bool hasGlobalSection() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	getGlobalSection
		//
		////////////////////////////////////////////////////////////////

		/// @brief グローバル（無名）セクションへの参照を返します。
		/// @return グローバル（無名）セクションへの参照
		[[nodiscard]]
		const Section& getGlobalSection() const;

		/// @brief グローバル（無名）セクションへの参照を返します。
		/// @return グローバル（無名）セクションへの参照
		[[nodiscard]]
		Section& getGlobalSection();

		////////////////////////////////////////////////////////////////
		//
		//	operator []
		//
		////////////////////////////////////////////////////////////////

		/// @brief セクションへの参照を返します。
		/// @param section セクション名。空文字列の場合はグローバル（無名）セクション
		/// @return セクションへの参照
		/// @throw Error 指定したセクションが存在しない場合
		/// @remark `getSection()` と同じです。
		[[nodiscard]]
		const Section& operator [](StringView section) const;

		/// @brief セクションへの参照を返します。
		/// @param section セクション名。空文字列の場合はグローバル（無名）セクション
		/// @return セクションへの参照
		/// @throw Error 指定したセクションが存在しない場合
		/// @remark `getSection()` と同じです。
		[[nodiscard]]
		Section& operator [](StringView section);

	# ifdef __cpp_multidimensional_subscript

		/// @brief グローバル（無名）セクションへの参照を返します。
		/// @return グローバル（無名）セクションへの参照
		/// @throw Error グローバル（無名）セクションが存在しない場合
		/// @remark `getGlobalSection()` と同じです。
		[[nodiscard]]
		const Section& operator []() const;

		/// @brief グローバル（無名）セクションへの参照を返します。
		/// @return グローバル（無名）セクションへの参照
		/// @throw Error グローバル（無名）セクションが存在しない場合
		/// @remark `getGlobalSection()` と同じです。
		[[nodiscard]]
		Section& operator []();

	# endif

		////////////////////////////////////////////////////////////////
		//
		//	hasProperty
		//
		////////////////////////////////////////////////////////////////

		/// @brief プロパティが存在するかを返します。
		/// @param section セクション名。空文字列の場合はグローバル（無名）セクション
		/// @param key プロパティ名
		/// @return プロパティが存在する場合 true, それ以外の場合は false
		[[nodiscard]]
		bool hasProperty(StringView section, StringView key) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	getProperty
		//
		////////////////////////////////////////////////////////////////

		/// @brief プロパティの値を返します。
		/// @param section セクション名。空文字列の場合はグローバル（無名）セクション
		/// @param key プロパティ名
		/// @return プロパティの値
		/// @throw Error 指定したプロパティが存在しない場合
		[[nodiscard]]
		const String& getProperty(StringView section, StringView key) const;

		////////////////////////////////////////////////////////////////
		//
		//	hasGlobalProperty
		//
		////////////////////////////////////////////////////////////////

		/// @brief グローバル（無名）セクションにプロパティが存在するかを返します。
		/// @param key プロパティ名
		/// @return プロパティが存在する場合 true, それ以外の場合は false
		[[nodiscard]]
		bool hasGlobalProperty(StringView key) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	getGlobalProperty
		//
		////////////////////////////////////////////////////////////////

		/// @brief グローバル（無名）セクションのプロパティの値を返します。
		/// @param key プロパティ名
		/// @return プロパティの値
		/// @throw Error 指定したプロパティが存在しない場合
		[[nodiscard]]
		const String& getGlobalProperty(StringView key) const;

		////////////////////////////////////////////////////////////////
		//
		//	get
		//
		////////////////////////////////////////////////////////////////

		template <class Type>
		[[nodiscard]]
		Type get(StringView section, StringView key) const;

		////////////////////////////////////////////////////////////////
		//
		//	getOr
		//
		////////////////////////////////////////////////////////////////

		template <class Type, class U>
		[[nodiscard]]
		Type getOr(StringView section, StringView key, U&& defaultValue) const;

		////////////////////////////////////////////////////////////////
		//
		//	getOpt
		//
		////////////////////////////////////////////////////////////////

		template <class Type>
		[[nodiscard]]
		Optional<Type> getOpt(StringView section, StringView key) const;

		////////////////////////////////////////////////////////////////
		//
		//	getGlobal
		//
		////////////////////////////////////////////////////////////////

		template <class Type>
		[[nodiscard]]
		Type getGlobal(StringView key) const;

		////////////////////////////////////////////////////////////////
		//
		//	getGlobalOr
		//
		////////////////////////////////////////////////////////////////

		template <class Type, class U>
		[[nodiscard]]
		Type getGlobalOr(StringView key, U&& defaultValue) const;

		////////////////////////////////////////////////////////////////
		//
		//	getGlobalOpt
		//
		////////////////////////////////////////////////////////////////

		template <class Type>
		[[nodiscard]]
		Optional<Type> getGlobalOpt(StringView key) const;

		////////////////////////////////////////////////////////////////
		//
		//	addSection
		//
		////////////////////////////////////////////////////////////////

		/// @brief セクションを追加します。
		/// @param section セクション名。空文字列の場合はグローバル（無名）セクション
		/// @remark 既に存在する場合は何もしません。
		void addSection(StringView section);

		////////////////////////////////////////////////////////////////
		//
		//	removeSection
		//
		////////////////////////////////////////////////////////////////

		/// @brief セクションを削除します。
		/// @param section セクション名。空文字列の場合はグローバル（無名）セクション
		void removeSection(StringView section);

		////////////////////////////////////////////////////////////////
		//
		//	removeGlobalSection
		//
		////////////////////////////////////////////////////////////////

		/// @brief グローバル（無名）セクションを削除します。
		void removeGlobalSection();

		////////////////////////////////////////////////////////////////
		//
		//	addProperty
		//
		////////////////////////////////////////////////////////////////

		void addProperty(StringView section, StringView key, String value);

		////////////////////////////////////////////////////////////////
		//
		//	addGlobalProperty
		//
		////////////////////////////////////////////////////////////////

		void addGlobalProperty(StringView key, String value);

		////////////////////////////////////////////////////////////////
		//
		//	format
		//
		////////////////////////////////////////////////////////////////

		/// @brief INI 形式のデータを文字列に変換します。
		/// @return INI 形式のデータ
		[[nodiscard]]
		String format() const;

		////////////////////////////////////////////////////////////////
		//
		//	formatUTF8
		//
		////////////////////////////////////////////////////////////////

		/// @brief INI 形式のデータを UTF-8 文字列に変換します。
		/// @return INI 形式のデータ
		[[nodiscard]]
		std::string formatUTF8() const;

		////////////////////////////////////////////////////////////////
		//
		//	save
		//
		////////////////////////////////////////////////////////////////

		/// @brief INI 形式のデータをファイルに保存します。
		/// @param path ファイルパス
		/// @return 保存に成功した場合 true, それ以外の場合は false
		bool save(FilePathView path) const;

		////////////////////////////////////////////////////////////////
		//
		//	Load
		//
		////////////////////////////////////////////////////////////////

		static INI Load(FilePathView path);

		template <class Reader>
			requires (std::is_base_of_v<IReader, Reader> && (not std::is_lvalue_reference_v<Reader>))
		static INI Load(Reader&& reader);

		static INI Load(std::unique_ptr<IReader>&& reader);

		////////////////////////////////////////////////////////////////
		//
		//	Parse
		//
		////////////////////////////////////////////////////////////////

		static INI Parse(std::string_view s);

		static INI Parse(StringView s);

	private:

		Array<Section> m_sections;

		HashTable<String, size_t> m_sectionIndex;

		const String* getPropertyValue(StringView section, StringView key) const;
	};
}

# include "detail/INI.ipp"
