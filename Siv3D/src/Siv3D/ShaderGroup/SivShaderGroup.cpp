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

# include <Siv3D/ShaderGroup.hpp>
# include <Siv3D/VertexShader.hpp>
# include <Siv3D/PixelShader.hpp>
# include <Siv3D/System.hpp>

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	ShaderGroup::ShaderGroup(const Optional<HLSL>& hlsl, const Optional<MSL>& msl)
		: m_hlsl{ hlsl }
		, m_msl{ msl } {}

	////////////////////////////////////////////////////////////////
	//
	//	operator VertexShader
	//
	////////////////////////////////////////////////////////////////

	ShaderGroup::operator VertexShader() const
	{
		if (const EngineOption::Renderer renderer = System::GetRendererType();
			renderer == EngineOption::Renderer::Direct3D11)
		{
			assert(m_hlsl);
			return *m_hlsl;
		}
		else if (renderer == EngineOption::Renderer::Metal)
		{
			assert(m_msl);
			return *m_msl;
		}

		return{};
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator PixelShader
	//
	////////////////////////////////////////////////////////////////

	ShaderGroup::operator PixelShader() const
	{
		if (const EngineOption::Renderer renderer = System::GetRendererType();
			renderer == EngineOption::Renderer::Direct3D11)
		{
			assert(m_hlsl);
			return *m_hlsl;
		}
		else if (renderer == EngineOption::Renderer::Metal)
		{
			assert(m_msl);
			return *m_msl;
		}

		return{};
	}
}
