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
# include <Siv3D/BinaryReader.hpp>

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
	//	md5
	//
	////////////////////////////////////////////////////////////////

	MD5Value Blob::md5() const
	{
		return MD5Value::FromBinary(m_data.data(), m_data.size_bytes());
	}
}
