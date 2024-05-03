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
	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	JSONIterator::JSONIterator(JSON::json_base::iterator it, const Optional<difference_type>& index)
		: m_iterator(it)
		, m_index(index.value_or(0LL))
		, m_isArray{ index.has_value() } {}

	////////////////////////////////////////////////////////////////
	//
	//	operator *
	//
	////////////////////////////////////////////////////////////////

	JSONIterator::JSONItem JSONIterator::operator *() const
	{
		return JSONItem{ key(), value() };
	}

	////////////////////////////////////////////////////////////////
	//
	//	key
	//
	////////////////////////////////////////////////////////////////

	String JSONIterator::key() const
	{
		if (m_isArray)
		{
			return ToString(m_index);
		}

		return Unicode::FromUTF8(m_iterator.key());
	}

	////////////////////////////////////////////////////////////////
	//
	//	value
	//
	////////////////////////////////////////////////////////////////

	JSON JSONIterator::value() const
	{
		return JSON(std::ref(m_iterator.value()));
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator ++
	//
	////////////////////////////////////////////////////////////////

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

	////////////////////////////////////////////////////////////////
	//
	//	operator --
	//
	////////////////////////////////////////////////////////////////

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

	////////////////////////////////////////////////////////////////
	//
	//	operator ==
	//
	////////////////////////////////////////////////////////////////

	bool operator ==(const JSONIterator& lhs, const JSONIterator& rhs)
	{
		return (lhs.m_iterator == rhs.m_iterator);
	}

	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	JSONConstIterator::JSONConstIterator(JSON::json_base::const_iterator it, const Optional<difference_type>& index)
		: m_iterator(it)
		, m_index(index.value_or(0LL))
		, m_isArray{ index.has_value() } {}

	////////////////////////////////////////////////////////////////
	//
	//	operator *
	//
	////////////////////////////////////////////////////////////////

	JSONConstIterator::JSONItem JSONConstIterator::operator *() const
	{
		return JSONItem{ key(), value() };
	}

	////////////////////////////////////////////////////////////////
	//
	//	key
	//
	////////////////////////////////////////////////////////////////

	const String JSONConstIterator::key() const
	{
		if (m_isArray)
		{
			return ToString(m_index);
		}

		return Unicode::FromUTF8(m_iterator.key());
	}

	////////////////////////////////////////////////////////////////
	//
	//	value
	//
	////////////////////////////////////////////////////////////////

	const JSON JSONConstIterator::value() const
	{
		return JSON(std::cref(m_iterator.value()));
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator ++
	//
	////////////////////////////////////////////////////////////////

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

	////////////////////////////////////////////////////////////////
	//
	//	operator --
	//
	////////////////////////////////////////////////////////////////

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

	////////////////////////////////////////////////////////////////
	//
	//	operator ==
	//
	////////////////////////////////////////////////////////////////

	bool operator ==(const JSONConstIterator& lhs, const JSONConstIterator& rhs)
	{
		return (lhs.m_iterator == rhs.m_iterator);
	}
}
