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

# pragma once
# include <bit>
# include "Common.hpp"
# include "CompareFunction.hpp"
# include "DepthStencilStateBuilder.hpp"
# include "String.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	DepthStencilState
	//
	////////////////////////////////////////////////////////////////

	/// @brief 深度ステンシルステート
	struct DepthStencilState
	{
		/// @brief 深度テストを有効にするか
		bool depthTestEnabled					: 1	= false;

		/// @brief 深度の書き込みを有効にするか
		bool depthWriteEnabled					: 1	= false;

		/// @brief 深度の比較関数
		CompareFunction depthCompareFunction	: 4 = CompareFunction::GreaterEqual;

		uint8 _unused							: 2 = 0;

		using storage_type = uint8;

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		/// @brief 深度ステンシルステートを作成します。
		/// @param _depthTestEnabled 深度テストを有効にするか
		/// @param _depthWriteEnabled 深度書き込みを有効にするか
		/// @param _depthCompareFunction 深度の比較関数
		[[nodiscard]]
		explicit constexpr DepthStencilState(
			bool _depthTestEnabled = false,
			bool _depthWriteEnabled = false,
			CompareFunction _depthCompareFunction = CompareFunction::GreaterEqual
		) noexcept;

		/// @brief 深度ステンシルステートを作成します。
		/// @param builder 深度ステンシルステートの設定
		[[nodiscard]]
		constexpr DepthStencilState(const DepthStencilStateBuilder& builder) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	(constant)
		//
		////////////////////////////////////////////////////////////////

		static constexpr DepthStencilStateBuilder Disabled{ false, false };

		static constexpr DepthStencilStateBuilder DepthTestOnly{ true, false };

		static constexpr DepthStencilStateBuilder DepthWriteOnly{ false, true };

		static constexpr DepthStencilStateBuilder DepthTestWrite{ true, true };

		static constexpr DepthStencilStateBuilder Default2D = Disabled;

		static constexpr DepthStencilStateBuilder Default3D = DepthTestWrite;

		////////////////////////////////////////////////////////////////
		//
		//	asValue
		//
		////////////////////////////////////////////////////////////////

		/// @brief 深度ステンシルステートの設定を整数値に変換します。
		/// @return 深度ステンシルステートの設定を表す整数値
		[[nodiscard]]
		constexpr storage_type asValue() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	format
		//
		////////////////////////////////////////////////////////////////

		/// @brief 深度ステンシルステートの設定を文字列に変換します。
		/// @return 深度ステンシルステートの設定を表す文字列
		[[nodiscard]]
		String format() const;

		////////////////////////////////////////////////////////////////
		//
		//	operator ==
		//
		////////////////////////////////////////////////////////////////

		/// @brief 2 つの深度ステンシルステートが等しいかを返します。
		/// @param lhs 一方の深度ステンシルステート
		/// @param rhs もう一方の深度ステンシルステート
		/// @return 2 つの深度ステンシルステートが等しい場合 true, それ以外の場合は false
		[[nodiscard]]
		friend constexpr bool operator ==(const DepthStencilState& lhs, const DepthStencilState& rhs) noexcept
		{
			return (lhs.asValue() == rhs.asValue());
		}

		////////////////////////////////////////////////////////////////
		//
		//	Invalid
		//
		////////////////////////////////////////////////////////////////

		/// @brief 無効な深度ステンシルステートを返します。
		/// @return 無効な深度ステンシルステート
		[[nodiscard]]
		static constexpr DepthStencilState Invalid();

		////////////////////////////////////////////////////////////////
		//
		//	Formatter
		//
		////////////////////////////////////////////////////////////////

		/// @brief 深度ステンシルステートを文字列に変換します。
		/// @param formatData 文字列バッファ
		/// @param value 深度ステンシルステート
		/// @remark この関数は Format 用の関数です。通常、ユーザーが直接呼び出す必要はありません。
		friend void Formatter(FormatData& formatData, const DepthStencilState& value);
	};

	static_assert(sizeof(DepthStencilState) == sizeof(DepthStencilState::storage_type));
}

////////////////////////////////////////////////////////////////
//
//	std::hash
//
////////////////////////////////////////////////////////////////

template <>
struct std::hash<s3d::DepthStencilState>
{
	[[nodiscard]]
	size_t operator ()(const s3d::DepthStencilState& value) const noexcept
	{
		return hash<s3d::DepthStencilState::storage_type>()(value.asValue());
	}
};

# include "detail/DepthStencilState.ipp"
