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

# include "../FontData.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	GetTabAdvance
	//
	////////////////////////////////////////////////////////////////

	double GetTabAdvance(const double spaceWidth, const double scale, const double xBegin, const double currentX, const int32 tabSize)
	{
		// タブの基本幅（ピクセル）
		const double baseTabWidth = (spaceWidth * tabSize * scale);

		// 現在何番目のタブ位置か
		const int32 currentIndentLevel = static_cast<int32>((currentX - xBegin) / baseTabWidth);

		// 次のタブ位置は何番目か
		const int32 nextIndentLevel = (currentIndentLevel + 1);

		// 次のタブ位置の X 座標
		const double nextX = (xBegin + (nextIndentLevel * baseTabWidth));

		// 次のタブ位置までの距離を返す
		return (nextX - currentX);
	}

	////////////////////////////////////////////////////////////////
	//
	//	ConsumeControlCharacter
	//
	////////////////////////////////////////////////////////////////

	bool ConsumeControlCharacter(const char32 ch, Vec2& penPos, int32& lineCount, const Vec2& basePos, const double scale, const double lineHeightScale, const FontFaceInfo& info)
	{
		if (ch == U'\t')
		{
			penPos.x += GetTabAdvance(info.spaceXAdvance, scale, basePos.x, penPos.x, info.tabSize);
		}
		else if (ch == U'\n')
		{
			penPos.x = basePos.x;
			penPos.y += (info.height() * scale * lineHeightScale);
			++lineCount;
		}
		
		return true;
	}
}
