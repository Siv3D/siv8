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
# include <Siv3D/Cursor/ICursor.hpp>
# include <Siv3D/Windows/MinWindows.hpp>

namespace s3d
{
	class CCursor final : public ISiv3DCursor
	{
	public:

		CCursor() = default;

		~CCursor() override;

		void init() override;

		bool update() override;

		void updateHighTemporalResolutionCursorPos(Point rawClientPos) override;

		const CursorState& getState() const override;

		Array<std::pair<int64, Point>> getHighTemporalResolutionCursorPos() const override;

	private:

		HWND m_hWnd = nullptr;

		////////////////////////////////////////////////////////////////
		//
		class ClientPosBuffer
		{
		public:

			/// @brief 補正前カーソル座標を追加します。
			/// @param pos 補正前カーソル座標
			void add(Point rawClientPos);

			/// @brief 指定したタイムポイント以前のデータを削除します。
			/// @param timePoint タイムポイント
			void remove(int64 timePoint);

			[[nodiscard]]
			Array<std::pair<int64, Point>> get() const;

		private:

			mutable std::mutex m_mutex;

			/// @brief タイムポイントと補正前カーソル座標のペアの配列
			Array<std::pair<int64, Point>> m_buffer;

		} m_clientPosBuffer;
		//
		////////////////////////////////////////////////////////////////

		CursorState m_state;
	};
}
