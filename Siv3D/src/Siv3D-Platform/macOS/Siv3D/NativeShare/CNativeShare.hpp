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
# include <Siv3D/String.hpp>
# include <Siv3D/NativeShare/INativeShare.hpp>

namespace s3d
{
	class CNativeShare final : public ISiv3DNativeShare
	{
	public:

		CNativeShare() = default;

		~CNativeShare() override;

		void init() override;

		bool show(const Image& image) override;

	private:

		FilePath m_imagePath;
	};
}
