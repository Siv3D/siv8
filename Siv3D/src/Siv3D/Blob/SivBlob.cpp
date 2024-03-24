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

# include <Siv3D/Blob.hpp>
# include <Siv3D/MD5Value.hpp>
# include <Siv3D/Base64Value.hpp>
# include <Siv3D/BinaryReader.hpp>
# include <Siv3D/BinaryWriter.hpp>

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
		reader.read(m_data.data(), m_data.size_bytes());
	}
		
	////////////////////////////////////////////////////////////////
	//
	//	createFromFile
	//
	////////////////////////////////////////////////////////////////

	bool Blob::createFromFile(const FilePathView path)
	{
		BinaryReader reader{ path };

		if (not reader)
		{
			return false;
		}

		m_data.resize(reader.size());

		return (static_cast<int64>(m_data.size_bytes()) == reader.read(m_data.data(), m_data.size_bytes()));
	}

	////////////////////////////////////////////////////////////////
	//
	//	save
	//
	////////////////////////////////////////////////////////////////

	bool Blob::save(const FilePathView path) const
	{
		BinaryWriter writer{ path };

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
