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
# include "Common.hpp"
# include "CompareFunction.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	DepthStencilStateBuilder
	//
	////////////////////////////////////////////////////////////////

	/// @brief 深度ステンシルステートの設定を構築するクラス
	class DepthStencilStateBuilder
	{
	public:

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		explicit constexpr DepthStencilStateBuilder(
			bool _depthTestEnabled = false,
			bool _depthWriteEnabled = false,
			CompareFunction _depthCompareFunction = CompareFunction::GreaterEqual
		) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	depthTestEnabled
		//
		////////////////////////////////////////////////////////////////

		/// @brief 深度テストを有効にするかを設定します。
		/// @param enabled 深度テストを有効にする場合は true, それ以外の場合は false
		/// @return 新しい深度ステンシルステート設定
		[[nodiscard]]
		constexpr DepthStencilStateBuilder depthTestEnabled(bool enabled) const noexcept;

		/// @brief 深度テストが有効かを返します。
		/// @return 深度テストが有効な場合 true, それ以外の場合は false
		[[nodiscard]]
		constexpr bool depthTestEnabled() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	depthWriteEnabled
		//
		////////////////////////////////////////////////////////////////

		/// @brief 深度の書き込みを有効にするかを設定します。
		/// @param enabled 深度の書き込みを有効にする場合は true, それ以外の場合は false
		/// @return 新しい深度ステンシルステート設定
		[[nodiscard]]
		constexpr DepthStencilStateBuilder depthWriteEnabled(bool enabled) const noexcept;

		/// @brief 深度の書き込みが有効かを返します。
		/// @return 深度の書き込みが有効な場合 true, それ以外の場合は false
		[[nodiscard]]
		constexpr bool depthWriteEnabled() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	depthCompareFunction
		//
		////////////////////////////////////////////////////////////////

		/// @brief 深度の比較関数を設定します。
		/// @param function 深度の比較関数
		/// @return 新しい深度ステンシルステート設定
		[[nodiscard]]
		constexpr DepthStencilStateBuilder depthCompareFunction(CompareFunction function) const noexcept;

		/// @brief 深度の比較関数を返します。
		/// @return 深度の比較関数
		[[nodiscard]]
		constexpr CompareFunction depthCompareFunction() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	Formatter
		//
		////////////////////////////////////////////////////////////////

		friend void Formatter(FormatData& formatData, const DepthStencilStateBuilder& value);

	private:

		bool m_depthTestEnabled : 1 = false;

		bool m_depthWriteEnabled : 1 = false;

		CompareFunction m_depthCompareFunction : 4 = CompareFunction::GreaterEqual;
	};
}

# include "detail/DepthStencilStateBuilder.ipp"
