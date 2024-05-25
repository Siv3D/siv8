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
# include <Siv3D/BlendState.hpp>
# include <Siv3D/HashTable.hpp>
# include <Siv3D/PointVector.hpp>
# include "../Device/D3D11Device.hpp"

namespace s3d
{
	class D3D11BlendState
	{
	public:

		void init(const D3D11Device& device);

		void set(const BlendState& state, const Float4& blendColor);
	
	private:

		using StateTable = HashTable<BlendState, ComPtr<ID3D11BlendState>>;

		ID3D11Device* m_device = nullptr;

		ID3D11DeviceContext* m_context = nullptr;

		StateTable m_states;

		std::pair<BlendState, Float4> m_currentState{ BlendState::Invalid(), Float4{ 0, 0, 0, 0} };

		StateTable::iterator create(const BlendState& state);
	};
}
