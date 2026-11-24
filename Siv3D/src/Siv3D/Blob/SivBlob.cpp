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

# include <Siv3D/Blob.hpp>
# include <Siv3D/MD5Value.hpp>
# include <Siv3D/Base64Value.hpp>
# include <Siv3D/BinaryFileReader.hpp>
# include <Siv3D/BinaryFileWriter.hpp>

namespace s3d
{		
	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	Blob::Blob(const FilePathView path)
	{
		createFromFile(path);
	}

	Blob::Blob(std::unique_ptr<IReader> reader)
	{
		if (reader)
		{
			createFromReader(*reader);
		}
	}
		
	////////////////////////////////////////////////////////////////
	//
	//	createFromFile
	//
	////////////////////////////////////////////////////////////////

	bool Blob::createFromFile(const FilePathView path)
	{
		BinaryFileReader reader{ path };
		return createFromReader(reader);
	}

	////////////////////////////////////////////////////////////////
	//
	//	createFromReader
	//
	////////////////////////////////////////////////////////////////

	bool Blob::createFromReader(IReader& reader)
	{
		if (not reader.isOpen())
		{
			m_data.clear();
			return false;
		}

		const int64 size = reader.size();
		const int64 pos = reader.getPos();

		if ((pos < 0) || (size < pos))
		{
			m_data.clear();
			return false;
		}

		int64 remaining = (size - pos);
		m_data.resize(static_cast<size_type>(remaining));
		size_type offset = 0;

		while (remaining > 0)
		{
			const int64 readSize = reader.read((m_data.data() + offset), remaining);

			if ((readSize <= 0) || (remaining < readSize))
			{
				m_data.clear();
				return false;
			}

			offset += static_cast<size_type>(readSize);
			remaining -= readSize;
		}

		return true;
	}

	////////////////////////////////////////////////////////////////
	//
	//	save
	//
	////////////////////////////////////////////////////////////////

	bool Blob::save(const FilePathView path) const
	{
		BinaryFileWriter writer{ path };

		if (not writer)
		{
			return false;
		}

		// 書き込むサイズ
		const int64 writeSize = static_cast<int64>(m_data.size_bytes());

		const bool written = (writeSize == writer.write(m_data.data(), writeSize));
		const bool closed = writer.close();
		return (written && closed);
	}

	////////////////////////////////////////////////////////////////
	//
	//	md5
	//
	////////////////////////////////////////////////////////////////

	MD5Value Blob::md5() const
	{
		return MD5Value::FromBlob(*this);
	}

	////////////////////////////////////////////////////////////////
	//
	//	base64
	//
	////////////////////////////////////////////////////////////////

	Base64Value Blob::base64() const
	{
		return Base64Value::EncodeFromBlob(*this);
	}

	void Blob::base64(Base64Value& dst) const
	{
		dst.encodeFromBlob(*this);
	}
}
