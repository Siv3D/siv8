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
# include <Siv3D/Renderer2D/IRenderer2D.hpp>

namespace s3d
{
	class CRenderer2D_D3D11 final : public ISiv3DRenderer2D
	{
	public:

		~CRenderer2D_D3D11() override;

		void init() override;

		void beginFrame() override;

	private:

	};
}
