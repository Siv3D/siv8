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
# include <variant>
# include "Common.hpp"
# include "String.hpp"
# include "2DShapes.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	PDFLinkInfo
	//
	////////////////////////////////////////////////////////////////

	struct PDFLinkInfo
	{
		using ExternalLink = String;

		using InternalLink = int32;

		std::variant<std::monostate, ExternalLink, InternalLink> target;

		RectF region = RectF::Empty();

		////////////////////////////////////////////////////////////////
		//
		//	operator bool
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		explicit operator bool() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	isEmpty
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		bool isEmpty() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	isExternalLink, isInternalLink
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		bool isExternalLink() const noexcept;

		[[nodiscard]]
		bool isInternalLink() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	getIfExternalLink, getIfInternalLink
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		const ExternalLink* getIfExternalLink() const noexcept;

		[[nodiscard]]
		const InternalLink* getIfInternalLink() const noexcept;
	};
}

# include "detail/PDFLinkInfo.ipp"
