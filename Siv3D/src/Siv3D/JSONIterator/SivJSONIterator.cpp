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

# include <Siv3D/JSONIterator.hpp>

namespace s3d
{
	JSONIterator::JSONIterator(JSON::json_base::iterator it, const Optional<difference_type>& index)
		: m_iterator(it)
		, m_index(index.value_or(0LL))
		, m_isArray{ index.has_value() } {}

	JSONIterator::JSONItem JSONIterator::operator *() const
	{
		return JSONItem{ key(), value() };
	}

	String JSONIterator::key() const
	{
		if (m_isArray)
		{
			return ToString(m_index);
		}

		return Unicode::FromUTF8(m_iterator.key());
	}

	JSON JSONIterator::value() const
	{
		return JSON(std::ref(m_iterator.value()));
	}

	JSONIterator& JSONIterator::operator ++()
	{
		++m_iterator;
		++m_index;
		return *this;
	}

	JSONIterator JSONIterator::operator ++(int)
	{
		JSONIterator old = *this;
		++m_iterator;
		++m_index;
		return old;
	}

	JSONIterator& JSONIterator::operator --()
	{
		--m_iterator;
		--m_index;
		return *this;
	}

	JSONIterator JSONIterator::operator --(int)
	{
		JSONIterator old = *this;
		--m_iterator;
		--m_index;
		return old;
	}

	bool operator ==(const JSONIterator& lhs, const JSONIterator& rhs)
	{
		return (lhs.m_iterator == rhs.m_iterator);
	}

	JSONConstIterator::JSONConstIterator(JSON::json_base::const_iterator it, const Optional<difference_type>& index)
		: m_iterator(it)
		, m_index(index.value_or(0LL))
		, m_isArray{ index.has_value() } {}

	JSONConstIterator::JSONItem JSONConstIterator::operator *() const
	{
		return JSONItem{ key(), value() };
	}

	const String JSONConstIterator::key() const
	{
		if (m_isArray)
		{
			return ToString(m_index);
		}

		return Unicode::FromUTF8(m_iterator.key());
	}

	const JSON JSONConstIterator::value() const
	{
		return JSON(std::cref(m_iterator.value()));
	}

	JSONConstIterator& JSONConstIterator::operator ++()
	{
		++m_iterator;
		++m_index;
		return *this;
	}

	JSONConstIterator JSONConstIterator::operator ++(int)
	{
		JSONConstIterator old = *this;
		++m_iterator;
		++m_index;
		return old;
	}

	JSONConstIterator& JSONConstIterator::operator --()
	{
		--m_iterator;
		--m_index;
		return *this;
	}

	JSONConstIterator JSONConstIterator::operator --(int)
	{
		JSONConstIterator old = *this;
		--m_iterator;
		--m_index;
		return old;
	}

	bool operator ==(const JSONConstIterator& lhs, const JSONConstIterator& rhs)
	{
		return (lhs.m_iterator == rhs.m_iterator);
	}
}
