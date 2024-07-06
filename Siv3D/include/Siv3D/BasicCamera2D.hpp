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
# include "Common.hpp"
# include "PointVector.hpp"
# include "2DShapes.hpp"
# include "Mat3x2.hpp"
# include "Transformer2D.hpp"
# include "Graphics2D.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	BasicCamera2D
	//
	////////////////////////////////////////////////////////////////

	/// @brief 2D カメラの基本クラス
	class BasicCamera2D
	{
	public:

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		/// @brief デフォルトコンストラクタ
		[[nodiscard]]
		BasicCamera2D() = default;

		/// @brief 指定した設定で 2D カメラを作成します。
		/// @param center カメラが見ている中心座標の初期設定
		/// @param scale カメラのズーム倍率の初期設定
		[[nodiscard]]
		explicit constexpr BasicCamera2D(Vec2 center, double scale = 1.0) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	getCenter, setCenter
		//
		////////////////////////////////////////////////////////////////

		/// @brief カメラが見ている中心座標を返します。
		/// @return カメラが見ている中心座標
		[[nodiscard]]
		constexpr const Vec2& getCenter() const noexcept;

		/// @brief カメラが見る中心座標を設定します。
		/// @param center カメラが見る中心座標
		constexpr void setCenter(Vec2 center) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	getScale, setScale
		//
		////////////////////////////////////////////////////////////////

		/// @brief カメラのズームアップ倍率を返します。
		/// @return カメラのズーム倍率
		[[nodiscard]]
		constexpr double getScale() const noexcept;

		/// @brief カメラのズームアップ倍率を設定します。
		/// @param scale カメラのズーム倍率
		constexpr void setScale(double scale) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	getRegion
		//
		////////////////////////////////////////////////////////////////

		/// @brief カメラに映っている領域を返します。
		/// @param renderTargetSize 現在のレンダーターゲットのサイズ（ピクセル）
		/// @return カメラに映っている領域
		[[nodiscard]]
		constexpr RectF getRegion(Size renderTargetSize = Graphics2D::GetRenderTargetSize()) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	getMat3x2
		//
		////////////////////////////////////////////////////////////////

		/// @brief 現在のカメラの設定から座標変換行列を作成します。
		/// @param renderTargetSize 現在のレンダーターゲットのサイズ（ピクセル）
		/// @return 現在のカメラの設定から作成された座標変換行列
		[[nodiscard]]
		constexpr Mat3x2 getMat3x2(Size renderTargetSize = Graphics2D::GetRenderTargetSize()) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	createTransformer
		//
		////////////////////////////////////////////////////////////////

		/// @brief 現在のカメラの設定から Transformer2D を作成します。
		/// @return 現在のカメラの設定から作成された Transformer2D
		[[nodiscard]]
		Transformer2D createTransformer() const;

	protected:

		/// @brief カメラが見ている中心座標
		Vec2 m_center{ 0, 0 };

		/// @brief カメラのズーム倍率
		double m_scale = 1.0;
	};
}

# include "detail/BasicCamera2D.ipp"
