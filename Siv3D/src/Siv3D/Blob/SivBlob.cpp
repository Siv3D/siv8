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

	Blob::Blob(IReader& reader)
		: m_data(reader.size())
	{
		const int64 readSize = reader.read(m_data.data(), m_data.size_bytes());

		if (m_data.size() != static_cast<size_type>(readSize))
		{
			m_data.clear();
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

		if (not reader)
		{
			return false;
		}

		m_data.resize(reader.size());

		const int64 readSize = reader.read(m_data.data(), m_data.size_bytes());

		if (m_data.size() != static_cast<size_type>(readSize))
		{
			m_data.clear();
			return false;
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

		const bool result = (writeSize == writer.write(m_data.data(), writeSize));

		return result;
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
