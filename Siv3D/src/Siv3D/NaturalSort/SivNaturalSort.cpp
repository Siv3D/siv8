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

# include <Siv3D/NaturalSort.hpp>
# include <Siv3D/Char.hpp>

namespace s3d
{
	int32 NaturalSortCompare(const StringView left, const StringView right) noexcept
	{
		size_t leftIndex = 0;
		size_t rightIndex = 0;
		const size_t leftLength = left.size();
		const size_t rightLength = right.size();

		while ((leftIndex < leftLength) && (rightIndex < rightLength))
		{
			const char32 leftChar = left[leftIndex];
			const char32 rightChar = right[rightIndex];

			// 両方が数字なら数値ブロックとして比較
			if (IsDigit(leftChar) && IsDigit(rightChar))
			{
				// 先頭の 0 をスキップ
				size_t leftNonZeroIndex = leftIndex;
				while ((leftNonZeroIndex < leftLength) && (left[leftNonZeroIndex] == U'0'))
				{
					++leftNonZeroIndex;
				}

				size_t rightNonZeroIndex = rightIndex;
				while ((rightNonZeroIndex < rightLength) && (right[rightNonZeroIndex] == U'0'))
				{
					++rightNonZeroIndex;
				}

				// 実質的な数字部分の終端を探す
				size_t leftDigitBlockEnd = leftNonZeroIndex;
				while ((leftDigitBlockEnd < leftLength) && IsDigit(left[leftDigitBlockEnd]))
				{
					++leftDigitBlockEnd;
				}

				size_t rightDigitBlockEnd = rightNonZeroIndex;
				while ((rightDigitBlockEnd < rightLength) && IsDigit(right[rightDigitBlockEnd]))
				{
					++rightDigitBlockEnd;
				}

				const size_t significantDigitsLeft = (leftDigitBlockEnd - leftNonZeroIndex);
				const size_t significantDigitsRight = (rightDigitBlockEnd - rightNonZeroIndex);

				// 有効桁数が違えば、桁数の多い方が大きい
				if (significantDigitsLeft != significantDigitsRight)
				{
					return (significantDigitsLeft < significantDigitsRight) ? -1 : 1;
				}

				// 有効桁数が 0（全部 0）の場合は、数値としては同じ 0 とみなす
				if (significantDigitsLeft > 0)
				{
					// 桁数が同じなら、数字部分を文字列として比較
					const int numericCompareResult = std::char_traits<char32>::compare(
						(left.data() + leftNonZeroIndex),
						(right.data() + rightNonZeroIndex),
						significantDigitsLeft
					);

					if (numericCompareResult != 0)
					{
						return (numericCompareResult < 0) ? -1 : 1;
					}
				}

				// 数値として完全に同じなら、この数字ブロックは同じとみなして続きへ
				// ※ leading zero も含めてブロック全体をスキップ
				while ((leftIndex < leftLength) && IsDigit(left[leftIndex]))
				{
					++leftIndex;
				}
				while ((rightIndex < rightLength) && IsDigit(right[rightIndex]))
				{
					++rightIndex;
				}
				continue;
			}

			// 少なくとも片方が数字でない場合は普通の文字として比較
			if (leftChar != rightChar)
			{
				return (leftChar < rightChar) ? -1 : 1;
			}

			++leftIndex;
			++rightIndex;
		}

		// ここまで一緒なら、長さで決着
		if (leftLength == rightLength)
		{
			return 0;
		}
		return (leftLength < rightLength) ? -1 : 1;
	}

	bool NaturalSortLess::operator()(const StringView left, const StringView right) noexcept
	{
		return (NaturalSortCompare(left, right) < 0);
	}
}
