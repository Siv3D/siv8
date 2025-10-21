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
# include <Siv3D/Blob.hpp>
# include <Siv3D/Renderer/Metal/Metal.hpp>

namespace s3d
{
	class MetalVertexShader
	{
	public:

		struct Null {};

		[[nodiscard]]
		MetalVertexShader() = default;

		[[nodiscard]]
		MetalVertexShader(Null);

		[[nodiscard]]
		MetalVertexShader(MTL::Library* library, const std::string& name);

		[[nodiscard]]
		MetalVertexShader(MTL::Device* device, const std::string& source, const std::string& entryPoint);

		[[nodiscard]]
		bool isInitialized() const noexcept;

		[[nodiscard]]
		MTL::Function* getShader() const;

	private:

		NS::SharedPtr<MTL::Function> m_shader;

		bool m_initialized = false;
	};
}
