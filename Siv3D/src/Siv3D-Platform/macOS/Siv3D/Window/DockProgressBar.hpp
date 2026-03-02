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
# include <memory>
# include <Siv3D/Common.hpp>

namespace s3d
{
	class DockProgressBar
	{
	public:

		DockProgressBar();
		
		~DockProgressBar();

		void setProgress(double progress0_1);
		
		void clear();

	private:
		
		class Impl;
		
		std::unique_ptr<Impl> m_pImpl;
	};
}
