//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2023 Ryo Suzuki
//	Copyright (c) 2016-2023 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once
# include "StringView.hpp"
# include "Utility.hpp"

namespace s3d
{
	class String
	{
	public:

		using string_type				= std::u32string;
		using traits_type				= typename string_type::traits_type;
		using allocator_type			= typename string_type::allocator_type;
		using value_type				= typename string_type::value_type;
		using size_type					= typename string_type::size_type;
		using difference_type			= typename string_type::difference_type;
		using pointer					= typename string_type::pointer;
		using const_pointer				= typename string_type::const_pointer;
		using reference					= typename string_type::reference;
		using const_reference			= typename string_type::const_reference;
		using iterator					= typename string_type::iterator;
		using const_iterator			= typename string_type::const_iterator;
		using reverse_iterator			= typename string_type::reverse_iterator;
		using const_reverse_iterator	= typename string_type::const_reverse_iterator;

		static constexpr auto npos{ string_type::npos };

		/// @brief デフォルトコンストラクタ | default constructor
		[[nodiscard]]
		String() noexcept;

		/// @brief 文字列をコピーして新しい文字列を作成します。 | Creates a new string by copying the given string.
		/// @param other コピーする文字列 | The string to copy
		[[nodiscard]]
		String(const String& other);

		/// @brief 文字列をコピーして新しい文字列を作成します。 | Creates a new string by copying the given string.
		/// @param s コピーする文字列 | The string to copy
		[[nodiscard]]
		String(const string_type& s);

		[[nodiscard]]
		String(const String& other, size_type pos);

		[[nodiscard]]
		String(const string_type& other, size_type pos);

		[[nodiscard]]
		String(const String& other, size_type pos, size_type count);

		[[nodiscard]]
		String(const string_type& other, size_type pos, size_type count);

		[[nodiscard]]
		String(const value_type* s);

		[[nodiscard]]
		String(const value_type* s, size_type count);

		[[nodiscard]]
		String(std::initializer_list<value_type> ilist);

		[[nodiscard]]
		String(size_t count, value_type ch);

		template <class Iterator>
		[[nodiscard]]
		String(Iterator first, Iterator last);

		[[nodiscard]]
		String(String&& other) noexcept;

		[[nodiscard]]
		String(string_type&& s) noexcept;

		[[nodiscard]]
		explicit String(const StringViewIsh auto& s);

		[[nodiscard]]
		String(StringViewIsh auto s, size_type pos, size_type count);

		String(std::nullptr_t) = delete;

		[[nodiscard]]
		operator StringView() const noexcept;


		String& operator =(const String& other);

		String& operator =(const string_type& s);

		String& operator =(String&& other) noexcept;

		String& operator =(string_type&& s) noexcept;

		String& operator =(const value_type* s);

		String& operator =(std::initializer_list<value_type> ilist);

		String& operator =(const StringViewIsh auto& s);

		String& operator =(std::nullptr_t) = delete;


		String& assign(size_t count, value_type ch);

		String& assign(const String& s);

		String& assign(const string_type& s);

		String& assign(String&& s) noexcept;

		String& assign(string_type&& s) noexcept;

		String& assign(const String& s, size_type pos, size_type count = npos);

		String& assign(const string_type& s, size_type pos, size_type count = npos);

		String& assign(const value_type* s);

		String& assign(const value_type* s, size_type count);

		template <class Iterator>
		String& assign(Iterator first, Iterator last);

		String& assign(std::initializer_list<value_type> ilist);

		String& assign(const StringViewIsh auto& s);

		String& assign(const StringViewIsh auto& s, size_type pos, size_type count = npos);


		[[nodiscard]]
		allocator_type get_allocator() const noexcept;


		String& operator <<(value_type ch);

		String& operator +=(const String& s);

		String& operator +=(const string_type& s);

		String& operator +=(value_type ch);

		String& operator +=(const value_type* s);

		String& operator +=(std::initializer_list<value_type> ilist);

		String& operator +=(const StringViewIsh auto& s);
		


		String& append(size_t count, value_type ch);

		String& append(const String& s);

		String& append(const string_type& s);

		String& append(const String& s, size_type pos, size_type count = npos);

		String& append(const string_type& s, size_type pos, size_type count = npos);

		String& append(value_type ch);

		String& append(const value_type* s);

		String& append(const value_type* s, size_t count);

		template <class Iterator>
		String& append(Iterator first, Iterator last);

		String& append(std::initializer_list<value_type> ilist);

		String& append(const StringViewIsh auto& s);

		String& append(const StringViewIsh auto& s, size_type pos, size_type count = npos);



		String& insert(size_t offset, size_t count, value_type ch);

		String& insert(size_t offset, const value_type* s);

		String& insert(size_t offset, const value_type* s, size_t count);
		
		String& insert(size_t offset, const String& s);

		String& insert(size_t offset, const string_type& s);

		String& insert(size_t offset, const String& s, size_type pos, size_type count = npos);

		String& insert(size_t offset, const string_type& s, size_type pos, size_type count = npos);

		iterator insert(const_iterator where, value_type ch);

		iterator insert(const_iterator where, size_t count, value_type ch);

		template <class Iterator>
		iterator insert(const_iterator where, Iterator first, Iterator last);

		iterator insert(const_iterator where, std::initializer_list<value_type> ilist);

		String& insert(size_t offset, const StringViewIsh auto& s);

		String& insert(size_t offset, const StringViewIsh auto& s, size_type pos, size_type count = npos);


		/// @brief 文字列の先頭に文字を追加します。 | Appends a character to the beginning of the string.
		/// @param ch 追加する文字 | The character to append
		void push_front(value_type ch);

		/// @brief 文字列の末尾に文字を追加します。 | Appends a character to the end of the string.
		/// @param ch 追加する文字 | The character to append
		void push_back(value_type ch);

		/// @brief 文字列から先頭の文字を削除します。 | Removes the first character from the string.
		/// @remark 文字列が空の時に呼び出してはいけません。 | You must not call this function when the string is empty.
		void pop_front();

		/// @brief 文字列から先頭の n 文字を削除します。 | Removes the first n characters from the string.
		/// @remark n が現在の文字数より大きい場合は空の文字列にします。 | If n is greater than the current number of characters, the string will be empty.
		/// @param n 削除する文字数 | The number of characters to remove
		void pop_front_N(size_t n);

		/// @brief 文字列の末尾の文字を削除します。 | Removes the last character from the string.
		/// @remark 文字列が空の時に呼び出してはいけません。 | You must not call this function when the string is empty.
		void pop_back() noexcept;

		/// @brief 文字列から末尾の n 文字を削除します。 | Removes the last n characters from the string.
		/// @remark n が現在の文字数より大きい場合は空の文字列にします。 | If n is greater than the current number of characters, the string will be empty.
		/// @param n 削除する文字数 | The number of characters to remove
		void pop_back_N(size_t n) noexcept;


		String& erase(size_t offset = 0, size_t count = npos);

		iterator erase(const_iterator where) noexcept;

		iterator erase(const_iterator first, const_iterator last) noexcept;
		

		/// @brief 格納している文字列を消去し、空の文字列にします。 | Erases the stored string and makes it an empty string.
		void clear() noexcept;


		[[nodiscard]]
		iterator begin() noexcept;

		[[nodiscard]]
		iterator end() noexcept;

		[[nodiscard]]
		const_iterator begin() const noexcept;

		[[nodiscard]]
		const_iterator end() const noexcept;

		[[nodiscard]]
		const_iterator cbegin() const noexcept;

		[[nodiscard]]
		const_iterator cend() const noexcept;

		[[nodiscard]]
		reverse_iterator rbegin() noexcept;

		[[nodiscard]]
		reverse_iterator rend() noexcept;

		[[nodiscard]]
		const_reverse_iterator rbegin() const noexcept;

		[[nodiscard]]
		const_reverse_iterator rend() const noexcept;

		[[nodiscard]]
		const_reverse_iterator crbegin() const noexcept;

		[[nodiscard]]
		const_reverse_iterator crend() const noexcept;


		void shrink_to_fit();


		void release();

		[[nodiscard]]
		value_type& at(size_t offset)&;

		[[nodiscard]]
		const value_type& at(size_t offset) const&;

		[[nodiscard]]
		value_type at(size_t offset)&&;

		[[nodiscard]]
		value_type& operator [](size_t offset) & noexcept;

		[[nodiscard]]
		const value_type& operator [](size_t offset) const& noexcept;

		[[nodiscard]]
		value_type operator [](size_t offset) && noexcept;

		/// @brief 先頭の文字への参照を返します。
		/// @remark 空の文字列に対しては使えません。
		/// @return 先頭の文字への参照
		[[nodiscard]]
		value_type& front() noexcept;

		/// @brief 先頭の文字への参照を返します。
		/// @remark 空の文字列に対しては使えません。
		/// @return 先頭の文字への参照
		[[nodiscard]]
		const value_type& front() const noexcept;

		/// @brief 末尾の文字への参照を返します。
		/// @remark 空の文字列に対しては使えません。
		/// @return 末尾の文字への参照
		[[nodiscard]]
		value_type& back() noexcept;

		/// @brief 末尾の文字への参照を返します。
		/// @remark 空の文字列に対しては使えません。
		/// @return 末尾の文字への参照
		[[nodiscard]]
		const value_type& back() const noexcept;

		[[nodiscard]]
		const value_type* c_str() const noexcept;

		[[nodiscard]]
		const value_type* data() const noexcept;

		[[nodiscard]]
		value_type* data() noexcept;

		[[nodiscard]]
		string_type& str() noexcept;

		[[nodiscard]]
		const string_type& str() const noexcept;

		/// @brief 文字列の長さ（要素数）を返します。
		/// @return 文字列の長さ（要素数）
		[[nodiscard]]
		size_t length() const noexcept;

		/// @brief 文字列の長さ（要素数）を返します。
		/// @remark `.length()` と同じです。
		/// @return 文字列の長さ（要素数）
		[[nodiscard]]
		size_t size() const noexcept;

		/// @brief 文字列のデータサイズ（バイト）を返します。
		/// @remark `sizeof(value_type) * length()` です。
		/// @return 文字列のデータサイズ（バイト）
		[[nodiscard]]
		size_t size_bytes() const noexcept;

		/// @brief 文字列が空であるかを返します。
		/// @return 文字列が空である場合 true, それ以外の場合は false
		[[nodiscard]]
		bool empty() const noexcept;

		/// @brief 文字列が空であるかを返します。
		/// @remark `empty()` と同じです。
		/// @return 文字列が空である場合 true, それ以外の場合は false
		[[nodiscard]]
		bool isEmpty() const noexcept;

		/// @brief 文字列が空でないかを返します。
		/// @return 文字列が空でない場合 true, それ以外の場合は false
		[[nodiscard]]
		explicit operator bool() const noexcept;

		[[nodiscard]]
		size_t max_size() const noexcept;

		[[nodiscard]]
		size_t capacity() const noexcept;

		void resize(size_t newSize);

		void resize(size_t newSize, value_type ch);

	# if defined(__cpp_lib_string_resize_and_overwrite)

		template <class Operation>
		void resize_and_overwrite(size_type count, Operation op);

	# endif

		void reserve(size_t newCapacity);

		void swap(String& other) noexcept;

		/// @brief 文字列が指定した文字から始まるかを返します。
		/// @param ch 検索する文字
		/// @return 指定した文字から始まる場合 true, それ以外の場合は false	
		[[nodiscard]]
		bool starts_with(value_type ch) const noexcept;

		/// @brief 文字列が指定した文字列から始まるかを返します。
		/// @param s 検索する文字列
		/// @return 指定した文字列から始まる場合 true, それ以外の場合は false
		[[nodiscard]]
		bool starts_with(const value_type* s) const noexcept;

		/// @brief 文字列が指定した文字列から始まるかを返します。
		/// @param s 検索する文字列
		/// @return 指定した文字列から始まる場合 true, それ以外の場合は false
		[[nodiscard]]
		bool starts_with(StringView s) const noexcept;

		/// @brief 文字列が指定した文字で終わるかを返します。
		/// @param ch 検索する文字
		/// @return 指定した文字で終わる場合 true, それ以外の場合は false
		[[nodiscard]]
		bool ends_with(value_type ch) const noexcept;

		/// @brief 文字列が指定した文字列で終わるかを返します。
		/// @param s 検索する文字列
		/// @return 指定した文字列で終わる場合 true, それ以外の場合は false
		[[nodiscard]]
		bool ends_with(const value_type* s) const noexcept;

		/// @brief 文字列が指定した文字列で終わるかを返します。
		/// @param s 検索する文字列
		/// @return 指定した文字列で終わる場合 true, それ以外の場合は false
		[[nodiscard]]
		bool ends_with(StringView s) const noexcept;

		/// @brief 文字列が指定した文字を含むかを返します。
		/// @param ch 検索する文字
		/// @return 指定した文字を含む場合 true, それ以外の場合は false
		[[nodiscard]]
		bool contains(value_type ch) const noexcept;

		/// @brief 文字列が指定した文字を含むかを返します。
		/// @param s 検索する文字列
		/// @return 指定した文字列を含む場合 true, それ以外の場合は false
		[[nodiscard]]
		bool contains(const value_type* s) const noexcept;

		/// @brief 文字列が指定した文字列を含むかを返します。
		/// @param s 検索する文字列
		/// @return 指定した文字列を含む場合 true, それ以外の場合は false
		[[nodiscard]]
		bool contains(StringView s) const noexcept;

		[[deprecated("Use String::contains(value_type) instead")]]
		bool includes(value_type ch) const noexcept;

		[[deprecated("Use String::contains(StringView) instead")]]
		bool includes(StringView s) const noexcept;

		/// @brief 部分文字列を取得します。
		/// @param offset 開始インデックス
		/// @param count 取得する文字数。末尾までの場合 npos
		/// @return 部分文字列
		[[nodiscard]]
		String substr(size_t offset = 0, size_t count = npos) const;

		/// @brief 部分文字列へのビューを取得します。
		/// @param offset 開始インデックス
		/// @param count 取得する文字数。末尾までの場合 npos
		/// @return 部分文字列へのビュー
		[[nodiscard]]
		StringView substrView(size_t offset = 0, size_t count = npos) const&;

		[[nodiscard]]
		StringView substrView(size_t offset = 0, size_t count = npos) && = delete;

		[[deprecated("Use String::toUTF8() instead")]]
		std::string narrow() const;

		/// @brief 文字列を UTF-8 文字列に変換します。
		/// @return 変換された文字列
		[[nodiscard]]
		std::string toUTF8() const;

		/// @brief 文字列を std::wstring に変換します。
		/// @return 変換された文字列
		[[nodiscard]]
		std::wstring toWstr() const;

		/// @brief 文字列を UTF-16 文字列に変換します。
		/// @return 変換された文字列
		[[nodiscard]]
		std::u16string toUTF16() const;

		/// @brief 文字列を UTF-32 文字列に変換します。
		/// @return 変換された文字列
		[[nodiscard]]
		const std::u32string& toUTF32() const noexcept;

		uint64 hash() const noexcept;



		/// @brief 文字列の末尾にある空白文字を削除します。
		/// @return *this
		String& rtrim();

		/// @brief 文字列の末尾にある空白文字を削除した新しい文字列を返します。
		/// @return 新しい文字列
		[[nodiscard]]
		String rtrimmed() const&;

		/// @brief 文字列の末尾にある空白文字を削除した新しい文字列を返します。
		/// @return 新しい文字列
		[[nodiscard]]
		String rtrimmed()&&;






		bool operator ==(const String&) const noexcept = default;

		std::strong_ordering operator <=>(const String& rhs) const noexcept = default;

		friend bool operator ==(const String& lhs, const value_type* rhs);

		friend std::strong_ordering operator <=>(const String& lhs, const value_type* rhs);


		friend String operator +(const value_type lhs, const String& rhs);

		friend String operator +(const value_type lhs, String&& rhs);

		friend String operator +(const value_type* lhs, const String& rhs);

		friend String operator +(const value_type* lhs, String&& rhs);

		friend String operator +(StringView lhs, const String& rhs);

		friend String operator +(StringView lhs, String&& rhs);

		friend String operator +(const String& lhs, const value_type rhs);

		friend String operator +(const String& lhs, const value_type* rhs);

		friend String operator +(const String& lhs, StringView rhs);

		friend String operator +(const String& lhs, const String& rhs);

		friend String operator +(const String& lhs, String&& rhs);

		friend String operator +(String&& lhs, const value_type rhs);

		friend String operator +(String&& lhs, const value_type* rhs);

		friend String operator +(String&& lhs, StringView rhs);

		friend String operator +(String&& lhs, const String& rhs);

		friend String operator +(String&& lhs, String&& rhs);



		friend std::ostream& operator <<(std::ostream& os, const String& value);

		friend std::wostream& operator <<(std::wostream& os, const String& value);

		friend std::basic_ostream<char32>& operator <<(std::basic_ostream<char32>& os, const String& value);

		friend std::istream& operator >>(std::istream& is, String& value);

		friend std::wistream& operator >>(std::wistream& is, String& value);


		friend void Formatter(FormatData& formatData, const String& s);

	private:

		string_type m_string;
	};

	[[nodiscard]]
	String operator +(const String::value_type lhs, StringView rhs);

	[[nodiscard]]
	String operator +(const String::value_type* lhs, StringView rhs);

	[[nodiscard]]
	String operator +(StringView lhs, const String::value_type* rhs);

	[[nodiscard]]
	String operator +(StringView lhs, StringView rhs);

	[[nodiscard]]
	String operator +(StringView lhs, const String::value_type rhs);

	void swap(String& a, String& b) noexcept;

	inline namespace Literals
	{
		inline namespace StringLiterals
		{
			[[nodiscard]]
			String operator ""_s(const char32_t* s, size_t length);
		}
	}

	using FilePath = String;
}

template <>
struct std::hash<s3d::String>
{
	[[nodiscard]]
	size_t operator ()(const s3d::String& value) const noexcept
	{
		return value.hash();
	}
};

# include "detail/String.ipp"
