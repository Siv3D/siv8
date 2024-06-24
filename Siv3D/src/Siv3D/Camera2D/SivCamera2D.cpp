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

# include <Siv3D/Camera2D.hpp>
# include <Siv3D/Mouse.hpp>
# include <Siv3D/Cursor.hpp>

namespace s3d
{
	Camera2D::Camera2D(const Camera2DControl& cameraControl)
		: Camera2D{ (Graphics2D::GetRenderTargetSize() * 0.5), 1.0, cameraControl } {}

	Camera2D::Camera2D(const double scale, const Camera2DControl& cameraControl)
		: Camera2D{ (Graphics2D::GetRenderTargetSize() * 0.5), scale, cameraControl } {}

	Camera2D::Camera2D(const Vec2 center, const double scale, const Camera2DControl& cameraControl)
		: BasicCamera2D{ center, scale }
		, m_cameraControl{ cameraControl } {}

	void Camera2D::setControl(const Camera2DControl& cameraControl)
	{
		m_cameraControl = cameraControl;
	}

	void Camera2D::setTargetCenter(const Vec2 targetCenter) noexcept
	{
		m_grabPos.reset();
		m_pointedScale.reset();
		m_targetCenter = targetCenter;
	}

	const Vec2& Camera2D::getTargetCenter() const noexcept
	{
		return m_targetCenter;
	}

	void Camera2D::setTargetScale(const double targetScale) noexcept
	{
		m_grabPos.reset();
		m_pointedScale.reset();
		m_targetScale = targetScale;
	}

	double Camera2D::getTargetScale() const noexcept
	{
		return m_targetScale;
	}

	void Camera2D::jumpTo(const Vec2 center, const double scale) noexcept
	{
		m_grabPos.reset();
		m_pointedScale.reset();
		m_targetCenter = m_center = center;
		m_targetScale = m_scale = scale;
		m_positionChangeVelocity = Vec2::Zero();
		m_scaleChangeVelocity = 0.0;
	}

	void Camera2D::update(const double deltaTime, const SizeF sceneSize)
	{
		updateWheel(sceneSize);
		updateControls(deltaTime);
		updateMouse(deltaTime);

		{
			const double oldScale = m_scale;
			m_scale = Math::SmoothDamp(m_scale, m_targetScale, m_scaleChangeVelocity, m_cameraControl.scaleSmoothTime, unspecified, deltaTime);

			if (deltaTime && (m_scale != m_targetScale) && (m_scale == oldScale))
			{
				m_scale = m_targetScale;
			}
		}

		if (m_pointedScale)
		{
			const Vec2 v = m_pointedScale->first - (sceneSize * 0.5);
			m_targetCenter = m_center = (m_pointedScale->second - v / m_scale);
		}
		else
		{
			const Vec2 oldCenter = m_center;
			m_center = Math::SmoothDamp(m_center, m_targetCenter, m_positionChangeVelocity, m_cameraControl.positionSmoothTime, unspecified, deltaTime);

			if (deltaTime && (m_center != m_targetCenter) && (m_center == oldCenter))
			{
				m_center = m_targetCenter;
			}
		}
	}

	//void Camera2D::draw(const ColorF& color) const
	//{
	//	if ((not m_grabPos)
	//		|| m_pointedScale)
	//	{
	//		return;
	//	}

	//	const auto t1 = Transformer2D{ Mat3x2::Identity(), TransformCursor::Yes, Transformer2D::Target::SetLocal };
	//	const auto t2 = Transformer2D{ Mat3x2::Identity(), TransformCursor::Yes, Transformer2D::Target::SetCamera };

	//	const double radius = 12.0;
	//	const Point delta = (Cursor::Pos() - m_grabPos.value());
	//	const double length = delta.length();

	//	Circle{ m_grabPos.value(), radius }.drawFrame(4.0, 2.0, color);

	//	if ((radius * 2) <= length)
	//	{
	//		const Vec2 direction = (delta / length);
	//		const Vec2 normalDirection{ direction.y, -direction.x };
	//		const Vec2 leftOffset = (normalDirection * 10);

	//		const Vec2 p0 = delta;
	//		const Vec2 p1 = direction.withLength(radius * 1.2) - leftOffset;
	//		const Vec2 p2 = direction.withLength(radius * 1.8);
	//		const Vec2 p3 = direction.withLength(radius * 1.2) + leftOffset;

	//		Quad{ p1, p2, p3, p0 }.moveBy(m_grabPos.value()).draw(color);
	//	}
	//}

	void Camera2D::updateWheel(const SizeF& sceneSize)
	{
		const double wheel = Mouse::Wheel();

		if (wheel == 0.0)
		{
			return;
		}

		m_positionChangeVelocity = Vec2::Zero();

		if (wheel < 0.0)
		{
			m_targetScale *= m_cameraControl.wheelScaleFactor;
		}
		else
		{
			m_targetScale /= m_cameraControl.wheelScaleFactor;
		}

		m_targetScale = Clamp(m_targetScale, m_cameraControl.minScale, m_cameraControl.maxScale);

		const auto t1 = Transformer2D{ Mat3x2::Identity(), TransformCursor::Yes, Transformer2D::Target::SetLocal };
		const auto t2 = Transformer2D{ Mat3x2::Identity(), TransformCursor::Yes, Transformer2D::Target::SetCamera };

		const Point cursorPos = Cursor::Pos();
		const Vec2 point = (m_center + (cursorPos - (sceneSize * 0.5)) / m_scale);
		m_pointedScale.emplace(cursorPos, point);
	}

	void Camera2D::updateControls(const double deltaTime)
	{
		{
			Vec2 delta{ 0, 0 };

			if (m_cameraControl.moveToUp
				&& m_cameraControl.moveToUp())
			{
				delta.y -= 1.0;
			}

			if (m_cameraControl.moveToLeft
				&& m_cameraControl.moveToLeft())
			{
				delta.x -= 1.0;
			}

			if (m_cameraControl.moveToDown
				&& m_cameraControl.moveToDown())
			{
				delta.y += 1.0;
			}

			if (m_cameraControl.moveToRight
				&& m_cameraControl.moveToRight())
			{
				delta.x += 1.0;
			}

			if (not delta.isZero())
			{
				m_pointedScale.reset();
				delta.normalize();
				m_targetCenter += (m_cameraControl.controlSpeedFactor * (deltaTime / 1.0) * (delta / m_targetScale));
			}
		}

		{
			if (m_cameraControl.zoomIn
				&& m_cameraControl.zoomIn())
			{
				m_targetScale *= (1.0 + (m_cameraControl.controlScaleFactor - 1.0) * (deltaTime / 1.0));
				m_targetScale = Min(m_targetScale, m_cameraControl.maxScale);
			}

			if (m_cameraControl.zoomOut
				&& m_cameraControl.zoomOut())
			{
				m_targetScale /= (1.0 + (m_cameraControl.controlScaleFactor - 1.0) * (deltaTime / 1.0));
				m_targetScale = Max(m_targetScale, m_cameraControl.minScale);
			}
		}
	}

	void Camera2D::updateMouse(const double deltaTime)
	{
		const auto t1 = Transformer2D{ Mat3x2::Identity(), TransformCursor::Yes, Transformer2D::Target::SetLocal };
		const auto t2 = Transformer2D{ Mat3x2::Identity(), TransformCursor::Yes, Transformer2D::Target::SetCamera };

		if (MouseR.down())
		{
			m_grabPos = Cursor::PosF();
			m_pointedScale.reset();
		}
		else if (m_grabPos)
		{
			const Vec2 delta = (Cursor::PosF() - m_grabPos.value());
			m_targetCenter += (m_cameraControl.grabSpeedFactor * (deltaTime / 1.0) * (delta / m_targetScale));

			if (MouseR.up())
			{
				m_grabPos = none;
			}
		}
	}
}
