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

# include <Siv3D/MediaTranscoder/CMediaTranscoder.hpp>

namespace s3d
{
	ISiv3DMediaTranscoder* ISiv3DMediaTranscoder::Create()
	{
		return new CMediaTranscoder;
	}
}
