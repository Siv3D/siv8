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
# include <functional>
# include <bit>
# include "Common.hpp"
# include "BlendFactor.hpp"
# include "BlendOperation.hpp"
# include "ColorWriteMask.hpp"
# include "BlendStateBuilder.hpp"

namespace s3d
{
	class String;

	////////////////////////////////////////////////////////////////
	//
	//	BlendState
	//
	////////////////////////////////////////////////////////////////

	/// @brief ブレンドステート
	struct BlendState
	{
	public:

		/// @brief ブレンドを有効にするか
		bool enabled					: 1 = true;

		/// @brief R チャンネルへの書き込みを有効にするか
		bool writeR						: 1 = true;

		/// @brief G チャンネルへの書き込みを有効にするか
		bool writeG						: 1 = true;

		/// @brief ソース RGB に適用するブレンド係数
		BlendFactor sourceRGB			: 5 = BlendFactor::One;

		/// @brief 書き込み先 RGB に適用するブレンド係数
		BlendFactor destinationRGB		: 5 = BlendFactor::OneMinusSourceAlpha;

		/// @brief RGB に適用するブレンド操作
		BlendOperation rgbOperation		: 3 = BlendOperation::Add;

		/// @brief Alpha-to-coverage を有効にするか
		bool alphaToCoverageEnabled		: 1 = false;

		/// @brief B チャンネルへの書き込みを有効にするか
		bool writeB						: 1 = true;

		/// @brief A チャンネルへの書き込みを有効にするか
		bool writeA						: 1 = true;

		/// @brief ソースアルファに適用するブレンド係数
		BlendFactor sourceAlpha			: 5 = BlendFactor::Zero;

		/// @brief 書き込み先アルファに適用するブレンド係数
		BlendFactor destinationAlpha	: 5 = BlendFactor::One;

		/// @brief アルファに適用するブレンド操作
		BlendOperation alphaOperation	: 3 = BlendOperation::Add;

		using storage_type = uint32;

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		/// @brief ブレンドステートを作成します。
		/// @param _enabled ブレンドを有効にするか
		/// @param _sourceRGB ソース RGB に適用するブレンド係数
		/// @param _destinationRGB 書き込み先 RGB に適用するブレンド係数
		/// @param _rgbOperation RGB に適用するブレンド操作
		/// @param _sourceAlpha ソースアルファに適用するブレンド係数
		/// @param _destinationAlpha 書き込み先アルファに適用するブレンド係数
		/// @param _alphaOperation アルファに適用するブレンド操作
		/// @param _alphaToCoverageEnabled Alpha-to-coverage を有効にするか
		/// @param _writeR R チャンネルへの書き込みを有効にするか
		/// @param _writeG G チャンネルへの書き込みを有効にするか
		/// @param _writeB B チャンネルへの書き込みを有効にするか
		/// @param _writeA A チャンネルへの書き込みを有効にするか
		[[nodiscard]]
		constexpr BlendState(
			bool _enabled,
			BlendFactor _sourceRGB,
			BlendFactor _destinationRGB,
			BlendOperation _rgbOperation,
			BlendFactor _sourceAlpha,
			BlendFactor _destinationAlpha,
			BlendOperation _alphaOperation,
			bool _alphaToCoverageEnabled,
			bool _writeR,
			bool _writeG,
			bool _writeB,
			bool _writeA
		) noexcept;

		/// @brief ブレンドステートを作成します。
		/// @param _enabled ブレンドを有効にするか
		/// @param _sourceRGB ソース RGB に適用するブレンド係数
		/// @param _destinationRGB 書き込み先 RGB に適用するブレンド係数
		/// @param _rgbOperation RGB に適用するブレンド操作
		/// @param _sourceAlpha ソースアルファに適用するブレンド係数
		/// @param _destinationAlpha 書き込み先アルファに適用するブレンド係数
		/// @param _alphaOperation アルファに適用するブレンド操作
		/// @param _alphaToCoverageEnabled Alpha-to-coverage を有効にするか
		/// @param colorWriteMask チャンネルへの書き込みマスク
		[[nodiscard]]
		explicit constexpr BlendState(
			bool _enabled = true,
			BlendFactor _sourceRGB = BlendFactor::One,
			BlendFactor _destinationRGB = BlendFactor::OneMinusSourceAlpha,
			BlendOperation _rgbOperation = BlendOperation::Add,
			BlendFactor _sourceAlpha = BlendFactor::Zero,
			BlendFactor _destinationAlpha = BlendFactor::One,
			BlendOperation _alphaOperation = BlendOperation::Add,
			bool _alphaToCoverageEnabled = false,
			ColorWriteMask colorWriteMask = ColorWriteMask::All
		) noexcept;

		/// @brief ブレンドステートを作成します。
		/// @param builder ブレンドステートの設定
		[[nodiscard]]
		constexpr BlendState(const BlendStateBuilder& builder) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	(constant)
		//
		////////////////////////////////////////////////////////////////

		static constexpr BlendStateBuilder Default2D{};

		static constexpr BlendStateBuilder Opaque{ false };

		////////////////////////////////////////////////////////////////
		//
		//	setColorWriteMask
		//
		////////////////////////////////////////////////////////////////

		/// @brief チャンネルへの書き込みを設定します。
		/// @param r R チャンネルへの書き込みを有効にするか
		/// @param g G チャンネルへの書き込みを有効にするか
		/// @param b B チャンネルへの書き込みを有効にするか
		/// @param a A チャンネルへの書き込みを有効にするか
		/// @return *this
		constexpr BlendState& setColorWriteMask(bool r, bool g, bool b, bool a) noexcept;

		/// @brief チャンネルへの書き込みを設定します。
		/// @param mask チャンネルへの書き込みを設定するマスク
		/// @return *this
		constexpr BlendState& setColorWriteMask(ColorWriteMask mask) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	getColorWriteMask
		//
		////////////////////////////////////////////////////////////////

		/// @brief チャンネルへの書き込みマスクを返します。
		/// @return チャンネルへの書き込みマスク
		[[nodiscard]]
		constexpr ColorWriteMask getColorWriteMask() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	asValue
		//
		////////////////////////////////////////////////////////////////

		/// @brief ブレンドステートの設定を 32 ビット整数に変換します。
		/// @return ブレンドステートの設定を表す 32 ビット整数
		[[nodiscard]]
		constexpr storage_type asValue() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	format
		//
		////////////////////////////////////////////////////////////////

		/// @brief ブレンドステートの設定を文字列に変換します。
		/// @return ブレンドステートの設定を表す文字列
		[[nodiscard]]
		String format() const;

		////////////////////////////////////////////////////////////////
		//
		//	operator ==
		//
		////////////////////////////////////////////////////////////////

		/// @brief 2 つのブレンドステートが等しいかを返します。
		/// @param lhs 一方のブレンドステート
		/// @param rhs もう一方のブレンドステート
		/// @return 2 つのブレンドステートが等しい場合 true, それ以外の場合は false
		[[nodiscard]]
		friend constexpr bool operator ==(const BlendState& lhs, const BlendState& rhs) noexcept
		{
			return (lhs.asValue() == rhs.asValue());
		}

		////////////////////////////////////////////////////////////////
		//
		//	Invalid
		//
		////////////////////////////////////////////////////////////////

		/// @brief 無効なブレンドステートを返します。
		/// @return 無効なブレンドステート
		[[nodiscard]]
		static constexpr BlendState Invalid();

		////////////////////////////////////////////////////////////////
		//
		//	Formatter
		//
		////////////////////////////////////////////////////////////////

		friend void Formatter(FormatData& formatData, const BlendState& value);
	};

	static_assert(sizeof(BlendState) == sizeof(BlendState::storage_type));
}

////////////////////////////////////////////////////////////////
//
//	std::hash
//
////////////////////////////////////////////////////////////////

template <>
struct std::hash<s3d::BlendState>
{
	[[nodiscard]]
	size_t operator ()(const s3d::BlendState& value) const noexcept
	{
		return hash<s3d::BlendState::storage_type>()(value.asValue());
	}
};

# include "detail/BlendState.ipp"
