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

# pragma once
# include <Siv3D/Common.hpp>
# include <Siv3D/Image.hpp>
# include "../Device/D3D11Device.hpp"

namespace s3d
{
	class D3D11ScreenCapture
	{
	public:

		////////////////////////////////////////////////////////////////
		//
		//	init
		//
		////////////////////////////////////////////////////////////////

		void init(const D3D11Device& device);

		////////////////////////////////////////////////////////////////
		//
		//	capture
		//
		////////////////////////////////////////////////////////////////

		bool capture(ID3D11Texture2D* texture, const Size& size);

		////////////////////////////////////////////////////////////////
		//
		//	getImage
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		const Image& getImage() const noexcept;

	private:

		ID3D11Device* m_device = nullptr;

		ID3D11DeviceContext* m_context = nullptr;

		ComPtr<ID3D11Texture2D> m_stagingTexture;
	
		Image m_image;
	};
}
