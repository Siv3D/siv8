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

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	template <ReaderObject Reader>
	CSV::CSV(Reader&& reader, const CSVReadOptions& options)
	{
		*this = Load(std::move(reader), options);
	}

	////////////////////////////////////////////////////////////////
	//
	//	get
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	Type CSV::get(const size_t row, const size_t column) const
	{
		if (const auto opt = getOpt<Type>(row, column))
		{
			return opt.value();
		}

		return Type{};
	}

	////////////////////////////////////////////////////////////////
	//
	//	getOr
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class U>
	Type CSV::getOr(const size_t row, const size_t column, U&& defaultValue) const
	{
		return getOpt<Type>(row, column).value_or(std::forward<U>(defaultValue));
	}

	////////////////////////////////////////////////////////////////
	//
	//	getOpt
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	Optional<Type> CSV::getOpt(const size_t row, const size_t column) const
	{
		if (const auto item = getItem(row, column))
		{
			return ParseOpt<Type>(*item);
		}

		return none;
	}

	////////////////////////////////////////////////////////////////
	//
	//	addRow
	//
	////////////////////////////////////////////////////////////////

	template <class ... Args>
	void CSV::addRow(const Args& ... records)
	{
		Row row;
		(row.push_back(Format(records)), ...);
		addRow(std::move(row));
	}

	////////////////////////////////////////////////////////////////
	//
	//	write
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	void CSV::write(const Type& value)
	{
		write(Format(value));
	}

	template <class ... Args>
	void CSV::write(const Args& ... records)
	{
		_write(records...);
	}

	////////////////////////////////////////////////////////////////
	//
	//	writeRow
	//
	////////////////////////////////////////////////////////////////

	template <class ... Args>
	void CSV::writeRow(const Args& ... records)
	{
		_write(records...);
		newLine();
	}

	////////////////////////////////////////////////////////////////
	//
	//	Load
	//
	////////////////////////////////////////////////////////////////

	template <ReaderObject Reader>
	CSV CSV::Load(Reader&& reader, const CSVReadOptions& options)
	{
		return Load(std::make_unique<Reader>(std::move(reader)), options);
	}

	////////////////////////////////////////////////////////////////
	//
	//	LoadResult
	//
	////////////////////////////////////////////////////////////////

	template <ReaderObject Reader>
	Result<CSV, Array<CSVParseErrorReason>> CSV::LoadResult(Reader&& reader, const CSVReadOptions& options)
	{
		return LoadResult(std::make_unique<Reader>(std::move(reader)), options);
	}

	////////////////////////////////////////////////////////////////
	//
	//	load
	//
	////////////////////////////////////////////////////////////////

	template <ReaderObject Reader>
	bool CSV::load(Reader&& reader, const CSVReadOptions& options)
	{
		return load(std::make_unique<Reader>(std::move(reader)), options);
	}

	////////////////////////////////////////////////////////////////
	//
	//	(private function)
	//
	////////////////////////////////////////////////////////////////

	inline void CSV::_write()
	{
		return;
	}

	template <class Type, class ... Args>
	void CSV::_write(const Type& record, const Args& ... records)
	{
		write(Format(record));
		return _write(records...);
	}
}
