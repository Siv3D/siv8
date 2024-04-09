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
# include "JSON.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	JSONIterator
	//
	////////////////////////////////////////////////////////////////

	class JSONIterator
	{
	public:

		struct JSONItem
		{
			String key;

			JSON value;
		};

		using value_type		= JSONItem;

		using difference_type	= std::ptrdiff_t;

		using iterator_concept	= std::bidirectional_iterator_tag;

		[[nodiscard]]
		JSONIterator() = default;

		[[nodiscard]]
		explicit JSONIterator(JSON::json_base::iterator it, const Optional<difference_type>& index = none);

		[[nodiscard]]
		JSONItem operator *() const;

		[[nodiscard]]
		String key() const;

		[[nodiscard]]
		JSON value() const;

		JSONIterator& operator ++();

		JSONIterator operator ++(int);
		
		JSONIterator& operator --();

		JSONIterator operator --(int);

		[[nodiscard]]
		friend bool operator ==(const JSONIterator& lhs, const JSONIterator& rhs);

	private:

		JSON::json_base::iterator m_iterator;

		difference_type m_index;

		bool m_isArray = false;
	};

	////////////////////////////////////////////////////////////////
	//
	//	JSONConstIterator
	//
	////////////////////////////////////////////////////////////////

	class JSONConstIterator
	{
	public:

		struct JSONItem
		{
			const String key;

			const JSON value;
		};

		using value_type		= JSONItem;

		using difference_type	= std::ptrdiff_t;

		using iterator_concept	= std::bidirectional_iterator_tag;

		[[nodiscard]]
		JSONConstIterator() = default;

		[[nodiscard]]
		explicit JSONConstIterator(JSON::json_base::const_iterator it, const Optional<difference_type>& index = none);

		[[nodiscard]]
		JSONItem operator *() const;

		[[nodiscard]]
		const String key() const;

		[[nodiscard]]
		const JSON value() const;

		JSONConstIterator& operator ++();

		JSONConstIterator operator ++(int);

		JSONConstIterator& operator --();

		JSONConstIterator operator --(int);

		[[nodiscard]]
		friend bool operator ==(const JSONConstIterator& lhs, const JSONConstIterator& rhs);

	private:

		JSON::json_base::const_iterator m_iterator;

		difference_type m_index;

		bool m_isArray = false;
	};
}
