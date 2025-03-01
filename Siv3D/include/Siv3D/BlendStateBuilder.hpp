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
# include "BlendFactor.hpp"
# include "BlendOperation.hpp"
# include "ColorWriteMask.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	BlendStateBuilder
	//
	////////////////////////////////////////////////////////////////

	/// @brief ブレンドステートの設定を構築するクラス
	class BlendStateBuilder
	{
	public:

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr BlendStateBuilder(
			bool _enable,
			BlendFactor _sourceRGB,
			BlendFactor _destinationRGB,
			BlendOperation _rgbOperation,
			BlendFactor _sourceAlpha,
			BlendFactor _destinationAlpha,
			BlendOperation _alphaOperation,
			bool _alphaToCoverageEnable,
			bool _writeR,
			bool _writeG,
			bool _writeB,
			bool _writeA
		) noexcept;

		[[nodiscard]]
		explicit constexpr BlendStateBuilder(
			bool _enable = true,
			BlendFactor _sourceRGB = BlendFactor::One,
			BlendFactor _destinationRGB = BlendFactor::OneMinusSourceAlpha,
			BlendOperation _rgbOperation = BlendOperation::Add,
			BlendFactor _sourceAlpha = BlendFactor::Zero,
			BlendFactor _destinationAlpha = BlendFactor::One,
			BlendOperation _alphaOperation = BlendOperation::Add,
			bool _alphaToCoverageEnable = false,
			ColorWriteMask colorWriteMask = ColorWriteMask::All
		) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	enabled
		//
		////////////////////////////////////////////////////////////////

		/// @brief ブレンドステートを有効にするかを設定します。
		/// @param _enabled ブレンドステートを有効にする場合は true, それ以外の場合は false
		/// @return 新しいブレンドステート設定
		[[nodiscard]]
		constexpr BlendStateBuilder enabled(bool _enabled) const noexcept;

		/// @brief ブレンドステートが有効かを返します。
		/// @return ブレンドステートが有効な場合 true, それ以外の場合は false
		[[nodiscard]]
		constexpr bool enabled() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	sourceRGB
		//
		////////////////////////////////////////////////////////////////

		/// @brief ソール RGB に適用するブレンド係数を設定します。
		/// @param _sourceRGB ソース RGB に適用するブレンド係数
		/// @return 新しいブレンドステート設定
		[[nodiscard]]
		constexpr BlendStateBuilder sourceRGB(BlendFactor _sourceRGB) const noexcept;

		/// @brief ソース RGB に適用するブレンド係数を返します。
		/// @return ソース RGB に適用するブレンド係数
		[[nodiscard]]
		constexpr BlendFactor sourceRGB() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	destinationRGB
		//
		////////////////////////////////////////////////////////////////

		/// @brief 書き込み先 RGB に適用するブレンド係数を設定します。
		/// @param _destinationRGB 書き込み先 RGB に適用するブレンド係数
		/// @return 新しいブレンドステート設定
		[[nodiscard]]
		constexpr BlendStateBuilder destinationRGB(BlendFactor _destinationRGB) const noexcept;

		/// @brief 書き込み先 RGB に適用するブレンド係数を返します。
		/// @return 書き込み先 RGB に適用するブレンド係数
		[[nodiscard]]
		constexpr BlendFactor destinationRGB() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	rgbOperation
		//
		////////////////////////////////////////////////////////////////

		/// @brief RGB に適用するブレンド操作を設定します。
		/// @param _rgbOperation RGB に適用するブレンド操作
		/// @return 新しいブレンドステート設定
		[[nodiscard]]
		constexpr BlendStateBuilder rgbOperation(BlendOperation _rgbOperation) const noexcept;

		/// @brief RGB に適用するブレンド操作を返します。
		/// @return RGB に適用するブレンド操作
		[[nodiscard]]
		constexpr BlendOperation rgbOperation() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	sourceAlpha
		//
		////////////////////////////////////////////////////////////////

		/// @brief ソースアルファに適用するブレンド係数を設定します。
		/// @param _sourceAlpha ソースアルファに適用するブレンド係数
		/// @return 新しいブレンドステート設定
		[[nodiscard]]
		constexpr BlendStateBuilder sourceAlpha(BlendFactor _sourceAlpha) const noexcept;

		/// @brief ソースアルファに適用するブレンド係数を返します。
		/// @return ソースアルファに適用するブレンド係数
		[[nodiscard]]
		constexpr BlendFactor sourceAlpha() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	destinationAlpha
		//
		////////////////////////////////////////////////////////////////

		/// @brief 書き込み先アルファに適用するブレンド係数を設定します。
		/// @param _destinationAlpha 書き込み先アルファに適用するブレンド係数
		/// @return 新しいブレンドステート設定
		[[nodiscard]]
		constexpr BlendStateBuilder destinationAlpha(BlendFactor _destinationAlpha) const noexcept;

		/// @brief 書き込み先アルファに適用するブレンド係数を返します。
		/// @return 書き込み先アルファに適用するブレンド係数
		[[nodiscard]]
		constexpr BlendFactor destinationAlpha() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	alphaOperation
		//
		////////////////////////////////////////////////////////////////

		/// @brief アルファに適用するブレンド操作を設定します。
		/// @param _alphaOperation アルファに適用するブレンド操作
		/// @return 新しいブレンドステート設定
		[[nodiscard]]
		constexpr BlendStateBuilder alphaOperation(BlendOperation _alphaOperation) const noexcept;

		/// @brief アルファに適用するブレンド操作を返します。
		/// @return アルファに適用するブレンド操作
		[[nodiscard]]
		constexpr BlendOperation alphaOperation() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	alphaToCoverageEnabled
		//
		////////////////////////////////////////////////////////////////

		/// @brief Alpha-to-coverage を有効にするかを設定します。
		/// @param _alphaToCoverageEnabled Alpha-to-coverage を有効にする場合は true, それ以外の場合は false
		/// @return 新しいブレンドステート設定
		[[nodiscard]]
		constexpr BlendStateBuilder alphaToCoverageEnabled(bool _alphaToCoverageEnabled) const noexcept;

		/// @brief Alpha-to-coverage が有効かを返します。
		/// @return Alpha-to-coverage が有効な場合 true, それ以外の場合は false
		[[nodiscard]]
		constexpr bool alphaToCoverageEnabled() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	writeR
		//
		////////////////////////////////////////////////////////////////

		/// @brief R チャンネルへの書き込みを有効にするかを設定します。
		/// @param _writeR R チャンネルへの書き込みを有効にする場合は true, それ以外の場合は false
		/// @return 新しいブレンドステート設定
		[[nodiscard]]
		constexpr BlendStateBuilder writeR(bool _writeR) const noexcept;

		/// @brief R チャンネルへの書き込みが有効かを返します。
		/// @return R チャンネルへの書き込みが有効な場合 true, それ以外の場合は false
		[[nodiscard]]
		constexpr bool writeR() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	writeG
		//
		////////////////////////////////////////////////////////////////

		/// @brief G チャンネルへの書き込みを有効にするかを設定します。
		/// @param _writeG G チャンネルへの書き込みを有効にする場合は true, それ以外の場合は false
		/// @return 新しいブレンドステート設定
		[[nodiscard]]
		constexpr BlendStateBuilder writeG(bool _writeG) const noexcept;

		/// @brief G チャンネルへの書き込みが有効かを返します。
		/// @return G チャンネルへの書き込みが有効な場合 true, それ以外の場合は false
		[[nodiscard]]
		constexpr bool writeG() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	writeB
		//
		////////////////////////////////////////////////////////////////

		/// @brief B チャンネルへの書き込みを有効にするかを設定します。
		/// @param _writeB B チャンネルへの書き込みを有効にする場合は true, それ以外の場合は false
		/// @return 新しいブレンドステート設定
		[[nodiscard]]
		constexpr BlendStateBuilder writeB(bool _writeB) const noexcept;

		/// @brief B チャンネルへの書き込みが有効かを返します。
		/// @return B チャンネルへの書き込みが有効な場合 true, それ以外の場合は false
		[[nodiscard]]
		constexpr bool writeB() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	writeA
		//
		////////////////////////////////////////////////////////////////

		/// @brief A チャンネルへの書き込みを有効にするかを設定します。
		/// @param _writeA A チャンネルへの書き込みを有効にする場合は true, それ以外の場合は false
		/// @return 新しいブレンドステート設定
		[[nodiscard]]
		constexpr BlendStateBuilder writeA(bool _writeA) const noexcept;

		/// @brief A チャンネルへの書き込みが有効かを返します。
		/// @return A チャンネルへの書き込みが有効な場合 true, それ以外の場合は false
		[[nodiscard]]
		constexpr bool writeA() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	colorWriteMask
		//
		////////////////////////////////////////////////////////////////

		/// @brief チャンネルへの書き込みマスクを設定します。
		/// @param colorWriteMask チャンネルへの書き込みマスク
		/// @return 新しいブレンドステート設定
		[[nodiscard]]
		constexpr BlendStateBuilder colorWriteMask(ColorWriteMask colorWriteMask) const noexcept;

		/// @brief チャンネルへの書き込みマスクを返します。
		/// @return チャンネルへの書き込みマスク
		[[nodiscard]]
		constexpr ColorWriteMask colorWriteMask() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	Formatter
		//
		////////////////////////////////////////////////////////////////

		/// @brief ブレンドステートの設定を文字列に変換します。
		/// @param formatData 文字列バッファ
		/// @param value ブレンドステートの設定
		/// @remark この関数は Format 用の関数です。通常、ユーザーが直接呼び出す必要はありません。
		friend void Formatter(FormatData& formatData, const BlendStateBuilder& value);

	private:

		bool m_enabled : 1 = true;

		bool m_writeR : 1 = true;

		bool m_writeG : 1 = true;

		BlendFactor m_sourceRGB : 5 = BlendFactor::One;

		BlendFactor m_destinationRGB : 5 = BlendFactor::OneMinusSourceAlpha;

		BlendOperation m_rgbOperation : 3 = BlendOperation::Add;

		bool m_alphaToCoverageEnabled : 1 = false;

		bool m_writeB : 1 = true;

		bool m_writeA : 1 = true;

		BlendFactor m_sourceAlpha : 5 = BlendFactor::Zero;

		BlendFactor m_destinationAlpha : 5 = BlendFactor::One;

		BlendOperation m_alphaOperation : 3 = BlendOperation::Add;
	};
}

# include "detail/BlendStateBuilder.ipp"
