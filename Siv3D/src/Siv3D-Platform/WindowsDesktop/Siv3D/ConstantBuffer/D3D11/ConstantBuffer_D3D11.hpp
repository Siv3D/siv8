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
# include <Siv3D/IConstantBuffer.hpp>
# include <Siv3D/Renderer/D3D11/D3D11.hpp>

namespace s3d
{
	class ConstantBuffer_D3D11 final : public IConstantBuffer
	{
	public:

		explicit ConstantBuffer_D3D11(size_t size);

		bool update(const void* data, size_t size) override;

	private:

		ID3D11DeviceContext* m_context = nullptr;

		ComPtr<ID3D11Buffer> m_buffer;

		uint32 m_bufferSize = 0;

		bool init();
	};
}
