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

		/// @brief アルファカバレッジを有効にするか
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
			bool _writeR = true,
			bool _writeG = true,
			bool _writeB = true,
			bool _writeA = true
		) noexcept;

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

		constexpr BlendState& setColorWriteMask(bool r, bool g, bool b, bool a) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	asValue
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr storage_type asValue() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	format
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		String format() const;

		////////////////////////////////////////////////////////////////
		//
		//	operator ==
		//
		////////////////////////////////////////////////////////////////

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
