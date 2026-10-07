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

# include <Siv3D/ScopedConstantBuffer2D.hpp>
# include <Siv3D/Graphics2D.hpp>
# include <utility>

namespace s3d
{
	template <ShaderStage Stage>
	ScopedConstantBuffer2D<Stage>::ScopedConstantBuffer2D(const uint32 slot, const void* data, const size_t size)
		: m_slot{ slot }
		, m_previous{ Graphics2D::Internal::BeginConstantBufferScope(Stage, slot, data, size) }
		, m_active{ true } {}

	template <ShaderStage Stage>
	ScopedConstantBuffer2D<Stage>::ScopedConstantBuffer2D(ScopedConstantBuffer2D&& other) noexcept
		: m_slot{ other.m_slot }
		, m_previous{ other.m_previous }
		, m_active{ std::exchange(other.m_active, false) } {}

	template <ShaderStage Stage>
	ScopedConstantBuffer2D<Stage>::~ScopedConstantBuffer2D()
	{
		if (m_active)
		{
			Graphics2D::Internal::EndConstantBufferScope(Stage, m_slot, m_previous);
		}
	}

	template class ScopedConstantBuffer2D<ShaderStage::Vertex>;
	template class ScopedConstantBuffer2D<ShaderStage::Pixel>;
}
