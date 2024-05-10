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
# include <Siv3D/Renderer/IRenderer.hpp>

namespace s3d
{
	class CRenderer_D3D11 final : public ISiv3DRenderer
	{
	public:

		~CRenderer_D3D11() override;

		void init() override;

		void clear() override;

		void flush() override;

		bool present() override;

	private:

	};
}
