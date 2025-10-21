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

# include <Siv3D/MSL.hpp>
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

	MSL::MSL(String entryPoint)
		: m_entryPoint{ std::move(entryPoint) } {}

	MSL::MSL(FilePath path, String entryPoint)
		: m_path{ std::move(path) }
		, m_entryPoint{ std::move(entryPoint) } {}

	////////////////////////////////////////////////////////////////
	//
	//	operator |
	//
	////////////////////////////////////////////////////////////////

	ShaderGroup MSL::operator |(const HLSL& hlsl) const
	{
		return{ hlsl, *this };
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator VertexShader
	//
	////////////////////////////////////////////////////////////////

	MSL::operator VertexShader() const
	{
		if (not m_source.empty())
		{
			return VertexShader::MSL(m_source, (m_entryPoint ? m_entryPoint : U"PS"));
		}

		if (m_path)
		{
			return VertexShader::MSL(m_path, m_entryPoint);
		}
		else
		{
			return VertexShader::MSL(m_entryPoint);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator PixelShader
	//
	////////////////////////////////////////////////////////////////

	MSL::operator PixelShader() const
	{
		if (not m_source.empty())
		{
			return PixelShader::MSL(m_source, (m_entryPoint ? m_entryPoint : U"PS"));
		}

		if (m_path)
		{
			return PixelShader::MSL(m_path, m_entryPoint);
		}
		else
		{
			return PixelShader::MSL(m_entryPoint);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	FromSource
	//
	////////////////////////////////////////////////////////////////

	MSL MSL::FromSource(std::string source)
	{
		return FromSource(std::move(source), {});
	}

	MSL MSL::FromSource(std::string source, const StringView entryPoint)
	{
		MSL msl{};
		msl.m_source = std::move(source);
		msl.m_entryPoint = entryPoint;
		return msl;
	}

	MSL MSL::FromSource(const StringView source)
	{
		return FromSource(source.toUTF8(), {});
	}

	MSL MSL::FromSource(const StringView source, const StringView entryPoint)
	{
		return FromSource(source.toUTF8(), entryPoint);
	}
}
