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
# include <mutex>
# include <Siv3D/Common.hpp>
# include <Siv3D/PointVector.hpp>
# include <Siv3D/Array.hpp>
# include <Siv3D/Duration.hpp>

namespace s3d
{
	class HighTemporalResolutionCursor
	{
	public:

		static constexpr Duration DefaultLifeTime = SecondsF{ 1.0 };

		/// @brief 補正前カーソル座標を追加します。
		/// @param pos 補正前カーソル座標
		void add(Point rawClientPos);

		/// @brief 指定した時間よりも古いデータを削除します。
		/// @param deltaTime 時間
		void update(const Duration& deltaTime = DefaultLifeTime);

		[[nodiscard]]
		Array<std::pair<int64, Point>> get() const;

	private:

		mutable std::mutex m_mutex;

		/// @brief タイムポイントと補正前カーソル座標のペアの配列
		Array<std::pair<int64, Point>> m_buffer;
	};
}
