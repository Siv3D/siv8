//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2026 Ryo Suzuki
//	Copyright (c) 2016-2026 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once
# include <Siv3D/MediaTranscoder/IMediaTranscoder.hpp>

namespace s3d
{
	class CMediaTranscoder final : public ISiv3DMediaTranscoder
	{
	public:

		~CMediaTranscoder() override;

		void init() override;

	private:

	};
}
