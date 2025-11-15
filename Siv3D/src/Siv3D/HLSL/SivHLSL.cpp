//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2025 Ryo Suzuki
//	Copyright (c) 2016-2025 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include <Siv3D/HLSL.hpp>
# include <Siv3D/VertexShader.hpp>
# include <Siv3D/PixelShader.hpp>
# include <Siv3D/ShaderGroup.hpp>

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	HLSL::HLSL(FilePath path)
		: m_path{ std::move(path) } {}

	HLSL::HLSL(FilePath path, String entryPoint)
		: m_path{ std::move(path) }
		, m_entryPoint{ std::move(entryPoint) } {}

	////////////////////////////////////////////////////////////////
	//
	//	operator |
	//
	////////////////////////////////////////////////////////////////

	ShaderGroup HLSL::operator |(const MSL& msl) const
	{
		return{ *this, msl };
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator VertexShader
	//
	////////////////////////////////////////////////////////////////

	HLSL::operator VertexShader() const
	{
		if (m_bytecode)
		{
			return VertexShader::HLSL(m_bytecode);
		}

		if (not m_source.empty())
		{
			return VertexShader::HLSL(m_source, (m_entryPoint ? m_entryPoint : U"VS"));
		}

		if (m_entryPoint)
		{
			return VertexShader::HLSL(m_path, m_entryPoint);
		}
		else
		{
			return VertexShader::HLSL(m_path, U"VS");
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator PixelShader
	//
	////////////////////////////////////////////////////////////////

	HLSL::operator PixelShader() const
	{
		if (m_bytecode)
		{
			return PixelShader::HLSL(m_bytecode);
		}

		if (not m_source.empty())
		{
			return PixelShader::HLSL(m_source, (m_entryPoint ? m_entryPoint : U"PS"));
		}

		if (m_entryPoint)
		{
			return PixelShader::HLSL(m_path, m_entryPoint);
		}
		else
		{
			return PixelShader::HLSL(m_path, U"PS");
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	FromSource
	//
	////////////////////////////////////////////////////////////////

	HLSL HLSL::FromSource(std::string source)
	{
		return FromSource(std::move(source), {});
	}

	HLSL HLSL::FromSource(std::string source, const StringView entryPoint)
	{
		HLSL hlsl{};
		hlsl.m_source = std::move(source);
		hlsl.m_entryPoint = entryPoint;
		return hlsl;
	}

	HLSL HLSL::FromSource(const StringView source)
	{
		return FromSource(source.toUTF8(), {});
	}

	HLSL HLSL::FromSource(const StringView source, const StringView entryPoint)
	{
		return FromSource(source.toUTF8(), entryPoint);
	}

	////////////////////////////////////////////////////////////////
	//
	//	FromBytecode
	//
	////////////////////////////////////////////////////////////////

	HLSL HLSL::FromBytecode(Blob bytecode)
	{
		HLSL hlsl{};
		hlsl.m_bytecode = std::move(bytecode);
		return hlsl;
	}
}
