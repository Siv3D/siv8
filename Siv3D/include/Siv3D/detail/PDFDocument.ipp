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

namespace s3d
{
	template <ReaderObject Reader>
	PDFDocument::PDFDocument(Reader&& reader, const StringView password)
		: PDFDocument{ std::make_unique<Reader>(std::forward<Reader>(reader)), password } {}
}
