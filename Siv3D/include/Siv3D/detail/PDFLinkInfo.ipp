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
	inline PDFLinkInfo::operator bool() const noexcept
	{
		return (not isEmpty());
	}

	inline bool PDFLinkInfo::isEmpty() const noexcept
	{
		return std::holds_alternative<std::monostate>(target);
	}

	inline bool PDFLinkInfo::isExternalLink() const noexcept
	{
		return std::holds_alternative<ExternalLink>(target);
	}

	inline bool PDFLinkInfo::isInternalLink() const noexcept
	{
		return std::holds_alternative<InternalLink>(target);
	}

	inline const PDFLinkInfo::ExternalLink* PDFLinkInfo::getIfExternalLink() const noexcept
	{
		return std::get_if<ExternalLink>(&target);
	}

	inline const PDFLinkInfo::InternalLink* PDFLinkInfo::getIfInternalLink() const noexcept
	{
		return std::get_if<InternalLink>(&target);
	}
}
