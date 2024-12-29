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
# include <array>
# include "IAssetMonitor.hpp"

namespace s3d
{
	class CAssetMonitor final : public ISiv3DAssetMonitor
	{
	public:

		bool update() override;

		void setWarningEnabled(bool enabled) override;

		void reportAssetCreation() override;

		void reportAssetRelease() override;

	private:

		static constexpr int32 RepeatedLoadDestroyFramesLimit = 30;

		std::array<uint16, RepeatedLoadDestroyFramesLimit> m_assetCreationCount{};

		std::array<uint16, RepeatedLoadDestroyFramesLimit> m_assetReleaseCount{};

		bool m_assetCreationWarningEnabled = true;
	};
}
