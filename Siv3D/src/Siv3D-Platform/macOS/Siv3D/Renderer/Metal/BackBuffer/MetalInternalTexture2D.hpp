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

# pragma once
# include <Siv3D/Common.hpp>
# include <Siv3D/PointVector.hpp>
# include <Siv3D/Renderer/Metal/Metal.hpp>

namespace s3d
{
	class MetalInternalTexture2D
	{
	public:

		void reset();

		[[nodiscard]]
		bool isEmpty() const noexcept
		{
			return m_size.isZero();
		}

		[[nodiscard]]
		operator bool() const noexcept
		{
			return (not isEmpty());
		}

		[[nodiscard]]
		const Size& size() const noexcept
		{
			return m_size;
		}
		
		[[nodiscard]]
        const MTL::Texture* getTexture() const noexcept
        {
            return m_texture.get();
        }
		
		[[nodiscard]]
		static MetalInternalTexture2D CreateRenderTexture(MTL::Device* device, const Size& size);

		[[nodiscard]]
		static MetalInternalTexture2D CreateMSRenderTexture(const Size& size, uint32 sampleCount);

	private:

		NS::SharedPtr<MTL::Texture> m_texture;

		Size m_size{ 0, 0 };

		uint32 m_sampleCount = 1;
	};
}
