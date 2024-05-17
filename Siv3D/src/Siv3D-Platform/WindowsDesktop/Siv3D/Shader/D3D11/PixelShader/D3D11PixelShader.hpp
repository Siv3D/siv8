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
# include <Siv3D/Blob.hpp>
# include <Siv3D/Renderer/D3D11/D3D11.hpp>

namespace s3d
{
	class D3D11PixelShader
	{
	public:

		struct Null {};

		[[nodiscard]]
		D3D11PixelShader() = default;

		[[nodiscard]]
		D3D11PixelShader(Null);

		[[nodiscard]]
		D3D11PixelShader(Blob&& binary, ID3D11Device* device);

		[[nodiscard]]
		bool isInitialized() const noexcept;

		[[nodiscard]]
		ID3D11PixelShader* getShader() const;

		[[nodiscard]]
		const Blob& getBinary() const noexcept;

	private:

		Blob m_binary;

		ComPtr<ID3D11PixelShader> m_shader;

		bool m_initialized = false;
	};
}
