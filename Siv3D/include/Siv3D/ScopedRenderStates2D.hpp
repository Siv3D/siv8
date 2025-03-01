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
# include "Common.hpp"
# include "Optional.hpp"
# include "Array.hpp"
# include "BlendState.hpp"
# include "RasterizerState.hpp"
# include "SamplerState.hpp"
# include "ShaderStage.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	ScopedRenderStates2D
	//
	////////////////////////////////////////////////////////////////

	/// @brief 2D 描画レンダーステート設定スコープオブジェクト
	/// @remark このオブジェクトが存在するスコープでは、2D 描画に、指定したレンダーステートが適用されます。
	class ScopedRenderStates2D
	{
	public:

		struct SamplerStateInfo
		{
			ShaderStage shaderStage;

			uint32 slot;

			SamplerState state;
		};

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		ScopedRenderStates2D() = default;

		[[nodiscard]]
		explicit ScopedRenderStates2D(const BlendState& blendState);

		[[nodiscard]]
		explicit ScopedRenderStates2D(const RasterizerState& rasterizerState);

		[[nodiscard]]
		explicit ScopedRenderStates2D(const SamplerState& samplerState);

		[[nodiscard]]
		explicit ScopedRenderStates2D(const SamplerStateInfo& samplerStateInfo);

		[[nodiscard]]
		explicit ScopedRenderStates2D(const Array<SamplerStateInfo>& samplerStateInfos);


		[[nodiscard]]
		ScopedRenderStates2D(const BlendState& blendState, const RasterizerState& rasterizerState);

		[[nodiscard]]
		ScopedRenderStates2D(const BlendState& blendState, const SamplerState& samplerState);

		[[nodiscard]]
		ScopedRenderStates2D(const BlendState& blendState, const SamplerStateInfo& samplerStateInfo);

		[[nodiscard]]
		ScopedRenderStates2D(const BlendState& blendState, const Array<SamplerStateInfo>& samplerStateInfos);


		[[nodiscard]]
		ScopedRenderStates2D(const RasterizerState& rasterizerState, const BlendState& blendState);

		[[nodiscard]]
		ScopedRenderStates2D(const RasterizerState& rasterizerState, const SamplerState& samplerState);

		[[nodiscard]]
		ScopedRenderStates2D(const RasterizerState& rasterizerState, const SamplerStateInfo& samplerStateInfo);

		[[nodiscard]]
		ScopedRenderStates2D(const RasterizerState& rasterizerState, const Array<SamplerStateInfo>& samplerStateInfos);


		[[nodiscard]]
		ScopedRenderStates2D(const SamplerState& samplerState, const BlendState& blendState);

		[[nodiscard]]
		ScopedRenderStates2D(const SamplerState& samplerState, const RasterizerState& rasterizerState);


		[[nodiscard]]
		ScopedRenderStates2D(const SamplerStateInfo& samplerStateInfo, const BlendState& blendState);

		[[nodiscard]]
		ScopedRenderStates2D(const SamplerStateInfo& samplerStateInfo, const RasterizerState& rasterizerState);


		[[nodiscard]]
		ScopedRenderStates2D(const Array<SamplerStateInfo>& samplerStateInfos, const BlendState& blendState);

		[[nodiscard]]
		ScopedRenderStates2D(const Array<SamplerStateInfo>& samplerStateInfos, const RasterizerState& rasterizerState);


		[[nodiscard]]
		ScopedRenderStates2D(const BlendState& blendState, const RasterizerState& rasterizerState, const SamplerState& samplerState);

		[[nodiscard]]
		ScopedRenderStates2D(const BlendState& blendState, const RasterizerState& rasterizerState, const SamplerStateInfo& samplerStateInfo);

		[[nodiscard]]
		ScopedRenderStates2D(const BlendState& blendState, const RasterizerState& rasterizerState, const Array<SamplerStateInfo>& samplerStateInfos);

		[[nodiscard]]
		ScopedRenderStates2D(const BlendState& blendState, const SamplerState& samplerState, const RasterizerState& rasterizerState);

		[[nodiscard]]
		ScopedRenderStates2D(const BlendState& blendState, const SamplerStateInfo& samplerStateInfo, const RasterizerState& rasterizerState);

		[[nodiscard]]
		ScopedRenderStates2D(const BlendState& blendState, const Array<SamplerStateInfo>& samplerStateInfos, const RasterizerState& rasterizerState);


		[[nodiscard]]
		ScopedRenderStates2D(const RasterizerState& rasterizerState, const BlendState& blendState, const SamplerState& samplerState);

		[[nodiscard]]
		ScopedRenderStates2D(const RasterizerState& rasterizerState, const BlendState& blendState, const SamplerStateInfo& samplerStateInfo);

		[[nodiscard]]
		ScopedRenderStates2D(const RasterizerState& rasterizerState, const BlendState& blendState, const Array<SamplerStateInfo>& samplerStateInfos);

		[[nodiscard]]
		ScopedRenderStates2D(const RasterizerState& rasterizerState, const SamplerState& samplerState, const BlendState& blendState);

		[[nodiscard]]
		ScopedRenderStates2D(const RasterizerState& rasterizerState, const SamplerStateInfo& samplerStateInfo, const BlendState& blendState);

		[[nodiscard]]
		ScopedRenderStates2D(const RasterizerState& rasterizerState, const Array<SamplerStateInfo>& samplerStateInfos, const BlendState& blendState);


		[[nodiscard]]
		ScopedRenderStates2D(const SamplerState& samplerState, const BlendState& blendState, const RasterizerState& rasterizerState);

		[[nodiscard]]
		ScopedRenderStates2D(const SamplerState& samplerState, const RasterizerState& rasterizerState, const BlendState& blendState);


		[[nodiscard]]
		ScopedRenderStates2D(const SamplerStateInfo& samplerStateInfo, const BlendState& blendState, const RasterizerState& rasterizerState);

		[[nodiscard]]
		ScopedRenderStates2D(const SamplerStateInfo& samplerStateInfo, const RasterizerState& rasterizerState, const BlendState& blendState);


		[[nodiscard]]
		ScopedRenderStates2D(const Array<SamplerStateInfo>& samplerStateInfos, const BlendState& blendState, const RasterizerState& rasterizerState);

		[[nodiscard]]
		ScopedRenderStates2D(const Array<SamplerStateInfo>& samplerStateInfos, const RasterizerState& rasterizerState, const BlendState& blendState);


		[[nodiscard]]
		ScopedRenderStates2D(ScopedRenderStates2D&& other) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	(destructor)
		//
		////////////////////////////////////////////////////////////////

		~ScopedRenderStates2D();

		////////////////////////////////////////////////////////////////
		//
		//	(deleted)
		//
		////////////////////////////////////////////////////////////////

		ScopedRenderStates2D(const ScopedRenderStates2D&) = delete;

		ScopedRenderStates2D& operator =(const ScopedRenderStates2D&) = delete;

	private:

		Optional<BlendState> m_oldBlendState;
		
		Optional<RasterizerState> m_oldRasterizerState;
		
		Array<SamplerStateInfo> m_oldSamplerStateInfos;

		void clear() noexcept;
	};
}
