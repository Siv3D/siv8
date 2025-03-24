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

# include <Siv3D/RectanglePack.hpp>
# include <Siv3D/Number.hpp>
# include <ThirdParty/rectpack2D/finders_interface.h>

namespace s3d
{
	namespace
	{
		template <bool AllowRotation>
		[[nodiscard]]
		static RectanglePack PackRectangles(const Array<Rect>& rects, const int32 maxSide)
		{
			using SpacesType	= rectpack2D::empty_spaces<AllowRotation, rectpack2D::default_empty_spaces>;
			using RectType		= rectpack2D::output_rect_t<SpacesType>;

			std::vector<RectType> rectangles(rects.size());
			{
				for (size_t i = 0; i < rects.size(); ++i)
				{
					const auto& rect = rects[i];
					rectangles[i] = rectpack2D::rect_xywh{ 0, 0, rect.w, rect.h };
				}
			}

			constexpr int32 DiscardStep = 1;
			
			constexpr auto ReportSuccessful = [](RectType&) { return rectpack2D::callback_result::CONTINUE_PACKING; };
		
			bool failed = false;
			auto ReportUnsuccessful = [&failed](RectType&)
			{
				failed = true;
				return rectpack2D::callback_result::ABORT_PACKING;
			};

			const rectpack2D::flipping_option flippingOption = (AllowRotation ? rectpack2D::flipping_option::ENABLED : rectpack2D::flipping_option::DISABLED);

			const auto resultSize = rectpack2D::find_best_packing<SpacesType>(
				rectangles,
				make_finder_input(
					maxSide,
					DiscardStep,
					ReportSuccessful,
					ReportUnsuccessful,
					flippingOption));

			if (failed)
			{
				return{};
			}

			Array<Rect> results(rectangles.size());
			{
				for (size_t i = 0; i < rectangles.size(); ++i)
				{
					const auto& r = rectangles[i];
					results[i].set(r.x, r.y, r.w, r.h);
				}
			}

			return{ std::move(results), Size{ resultSize.w, resultSize.h } };
		}
	}

	RectanglePack RectanglePack::Pack(const Array<Rect>& rects, const int32 maxSide) noexcept
	{
		return PackRectangles<false>(rects, maxSide);
	}

	RectanglePack RectanglePack::Pack(const Array<Rect>& rects, const int32 maxSide, const AllowRotation allowRotation) noexcept
	{
		if (allowRotation)
		{
			return PackRectangles<true>(rects, maxSide);
		}
		else
		{
			return PackRectangles<false>(rects, maxSide);
		}
	}
}
