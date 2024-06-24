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
# include "BasicCamera2D.hpp"
# include "Camera2DControl.hpp"
# include "Scene.hpp"

namespace s3d
{
	/// @brief 2D カメラ
	class Camera2D : public BasicCamera2D
	{
	public:

		[[nodiscard]]
		explicit Camera2D(const Camera2DControl& cameraControl = Camera2DControl::Default);

		[[nodiscard]]
		explicit Camera2D(double scale, const Camera2DControl& cameraControl = Camera2DControl::Default);

		/// @brief 2D カメラを作成します。
		/// @param center カメラが見ている中心座標の初期設定
		/// @param scale カメラのズーム倍率の初期設定
		/// @param cameraControl カメラの操作設定
		[[nodiscard]]
		Camera2D(Vec2 center, double scale, const Camera2DControl& cameraControl = Camera2DControl::Default);

		void setControl(const Camera2DControl& cameraControl);

		/// @brief カメラが見る中心座標の目標を設定します。
		/// @param targetCenter カメラが見る中心座標の目標
		/// @remark カメラは一定の時間をかけて目標の座標に移動します。
		void setTargetCenter(Vec2 targetCenter) noexcept;

		/// @brief 現在の目標中心座標を返します。
		/// @return 現在の目標中心座標
		const Vec2& getTargetCenter() const noexcept;

		/// @brief カメラのズームアップ倍率の目標を設定します。
		/// @param targetScale カメラのズーム倍率の目標
		/// @remark カメラは一定の時間をかけて目標のズーム倍率になります。
		void setTargetScale(double targetScale) noexcept;

		/// @brief 現在の目標ズームアップ倍率を返します。
		/// @return 現在の目標ズームアップ倍率
		double getTargetScale() const noexcept;

		/// @brief 指定した中心座標とズーム倍率を即座に適用します。
		/// @param center カメラが見る中心座標
		/// @param scale カメラのズーム倍率
		void jumpTo(Vec2 center, double scale) noexcept;

		/// @brief 2D カメラの状態を更新します。
		/// @param deltaTime 前回のフレームからの経過時間（秒）
		/// @param sceneSize レンダーターゲットのサイズ（ピクセル）
		void update(double deltaTime = Scene::DeltaTime(), SizeF sceneSize = Graphics2D::GetRenderTargetSize());

		/// @brief 2D カメラの右クリック・ドラッグによる移動の UI を表示します。
		/// @param color UI の色
		void draw(const ColorF& color = Palette::White) const;

	protected:

		double m_targetScale = BasicCamera2D::m_scale;

		double m_scaleChangeVelocity = 0.0;

		Vec2 m_targetCenter = BasicCamera2D::m_center;

		Vec2 m_positionChangeVelocity = Vec2::Zero();

		Optional<Vec2> m_grabPos;

		Optional<std::pair<Point, Vec2>> m_pointedScale;

		Camera2DControl m_cameraControl = Camera2DControl::Default;

		void updateWheel(const SizeF& sceneSize);

		void updateControls(const double deltaTime);

		void updateMouse(const double deltaTime);
	};
}
