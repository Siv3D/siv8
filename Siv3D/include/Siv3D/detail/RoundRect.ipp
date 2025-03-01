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

namespace s3d
{

	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	constexpr RoundRect::RoundRect(const value_type _x, const value_type _y, const value_type _w, const value_type _h, const value_type _r) noexcept
		: rect{ _x, _y, _w, _h }
		, r{ _r } {}

	constexpr RoundRect::RoundRect(const Concept::Arithmetic auto _x, const Concept::Arithmetic auto _y, const Concept::Arithmetic auto _w, const Concept::Arithmetic auto _h, const Concept::Arithmetic auto _r) noexcept
		: rect{ static_cast<value_type>(_x), static_cast<value_type>(_y), static_cast<value_type>(_w), static_cast<value_type>(_h) }
		, r{ static_cast<value_type>(_r) } {}

	constexpr RoundRect::RoundRect(const position_type pos, const value_type _w, const value_type _h, const value_type _r) noexcept
		: rect{ pos, _w, _h }
		, r{ _r } {}

	constexpr RoundRect::RoundRect(const position_type pos, const Concept::Arithmetic auto _w, const Concept::Arithmetic auto _h, const Concept::Arithmetic auto _r) noexcept
		: rect{ pos, static_cast<value_type>(_w), static_cast<value_type>(_h) }
		, r{ static_cast<value_type>(_r) } {}

	constexpr RoundRect::RoundRect(const value_type _x, const value_type _y, const size_type size, const value_type _r) noexcept
		: rect{ _x, _y, size }
		, r{ _r } {}

	constexpr RoundRect::RoundRect(const Concept::Arithmetic auto _x, const Concept::Arithmetic auto _y, const size_type size, const Concept::Arithmetic auto _r) noexcept
		: rect{ static_cast<value_type>(_x), static_cast<value_type>(_y), size }
		, r{ static_cast<value_type>(_r) } {}

	constexpr RoundRect::RoundRect(const position_type pos, const size_type size, const value_type _r) noexcept
		: rect{ pos, size }
		, r{ _r } {}

	constexpr RoundRect::RoundRect(const position_type pos, const size_type size, const Concept::Arithmetic auto _r) noexcept
		: rect{ pos, size }
		, r{ static_cast<value_type>(_r) } {}

	constexpr RoundRect::RoundRect(Arg::center_<position_type> _center, const value_type _w, const value_type _h, const value_type _r) noexcept
		: rect{ _center, _w, _h }
		, r{ _r } {}

	constexpr RoundRect::RoundRect(Arg::center_<position_type> _center, const Concept::Arithmetic auto _w, const Concept::Arithmetic auto _h, const Concept::Arithmetic auto _r) noexcept
		: rect{ _center, static_cast<value_type>(_w), static_cast<value_type>(_h) }
		, r{ static_cast<value_type>(_r) } {}

	constexpr RoundRect::RoundRect(Arg::center_<position_type> _center, const size_type size, const value_type _r) noexcept
		: rect{ _center, size }
		, r{ _r } {}

	constexpr RoundRect::RoundRect(Arg::center_<position_type> _center, const size_type size, const Concept::Arithmetic auto _r) noexcept
		: rect{ _center, size }
		, r{ static_cast<value_type>(_r) } {}

	constexpr RoundRect::RoundRect(const RectF& _rect, const value_type _r) noexcept
		: rect{ _rect }
		, r{ _r } {}

	constexpr RoundRect::RoundRect(const RectF& _rect, const Concept::Arithmetic auto _r) noexcept
		: rect{ _rect }
		, r{ static_cast<value_type>(_r) } {}

	////////////////////////////////////////////////////////////////
	//
	//	withX
	//
	////////////////////////////////////////////////////////////////

	constexpr RoundRect RoundRect::withX(const value_type _x) const noexcept
	{
		return{ _x, rect.pos.y, rect.size, r };
	}

	////////////////////////////////////////////////////////////////
	//
	//	withY
	//
	////////////////////////////////////////////////////////////////

	constexpr RoundRect RoundRect::withY(const value_type _y) const noexcept
	{
		return{ rect.pos.x, _y, rect.size, r };
	}

	////////////////////////////////////////////////////////////////
	//
	//	withPos
	//
	////////////////////////////////////////////////////////////////

	constexpr RoundRect RoundRect::withPos(const value_type _x, const value_type _y) const noexcept
	{
		return{ _x, _y, rect.size, r };
	}

	constexpr RoundRect RoundRect::withPos(const position_type _pos) const noexcept
	{
		return{ _pos, rect.size, r };
	}

	////////////////////////////////////////////////////////////////
	//
	//	withW
	//
	////////////////////////////////////////////////////////////////

	constexpr RoundRect RoundRect::withW(const value_type _w) const noexcept
	{
		return{ rect.pos, _w, rect.size.y, r };
	}

	////////////////////////////////////////////////////////////////
	//
	//	withH
	//
	////////////////////////////////////////////////////////////////

	constexpr RoundRect RoundRect::withH(const value_type _h) const noexcept
	{
		return{ rect.pos, rect.size.x, _h, r };
	}

	////////////////////////////////////////////////////////////////
	//
	//	withSize
	//
	////////////////////////////////////////////////////////////////

	constexpr RoundRect RoundRect::withSize(const value_type _w, const value_type _h) const noexcept
	{
		return{ rect.pos, _w, _h, r };
	}

	constexpr RoundRect RoundRect::withSize(const size_type _size) const noexcept
	{
		return{ rect.pos, _size, r };
	}

	////////////////////////////////////////////////////////////////
	//
	//	withCenter
	//
	////////////////////////////////////////////////////////////////

	constexpr RoundRect RoundRect::withCenter(const value_type _cx, const value_type _cy) const noexcept
	{
		return{ rect.withCenter(_cx, _cy), r};
	}

	constexpr RoundRect RoundRect::withCenter(const position_type _center) const noexcept
	{
		return{ rect.withCenter(_center), r };
	}

	////////////////////////////////////////////////////////////////
	//
	//	withR
	//
	////////////////////////////////////////////////////////////////

	constexpr RoundRect RoundRect::withR(const value_type _r) const noexcept
	{
		return{ rect, _r };
	}

	////////////////////////////////////////////////////////////////
	//
	//	setPos
	//
	////////////////////////////////////////////////////////////////

	constexpr RoundRect& RoundRect::setPos(const value_type _x, const value_type _y) noexcept
	{
		rect.setPos(_x, _y);
		return *this;
	}

	constexpr RoundRect& RoundRect::setPos(const position_type _pos) noexcept
	{
		rect.setPos(_pos);
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	setCenter
	//
	////////////////////////////////////////////////////////////////

	constexpr RoundRect& RoundRect::setCenter(const value_type _x, const value_type _y) noexcept
	{
		rect.setCenter(_x, _y);
		return *this;
	}

	constexpr RoundRect& RoundRect::setCenter(const position_type _center) noexcept
	{
		rect.setCenter(_center);
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	setSize
	//
	////////////////////////////////////////////////////////////////

	constexpr RoundRect& RoundRect::setSize(const value_type _size) noexcept
	{
		rect.setSize(_size);
		return *this;
	}

	constexpr RoundRect& RoundRect::setSize(const value_type _w, const value_type _h) noexcept
	{
		rect.setSize(_w, _h);
		return *this;
	}

	constexpr RoundRect& RoundRect::setSize(const size_type _size) noexcept
	{
		rect.setSize(_size);
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	setR
	//
	////////////////////////////////////////////////////////////////

	constexpr RoundRect& RoundRect::setR(const value_type _r) noexcept
	{
		r = _r;
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	set
	//
	////////////////////////////////////////////////////////////////

	constexpr RoundRect& RoundRect::set(const value_type _x, const value_type _y, const value_type _w, const value_type _h, const value_type _r) noexcept
	{
		rect.set(_x, _y, _w, _h);
		r = _r;
		return *this;
	}

	constexpr RoundRect& RoundRect::set(const position_type pos, const value_type _w, const value_type _h, const value_type _r) noexcept
	{
		rect.set(pos, _w, _h);
		r = _r;
		return *this;
	}

	constexpr RoundRect& RoundRect::set(const value_type _x, const value_type _y, const size_type size, const value_type _r) noexcept
	{
		rect.set(_x, _y, size);
		r = _r;
		return *this;
	}

	constexpr RoundRect& RoundRect::set(const position_type pos, const size_type size, const value_type _r) noexcept
	{
		rect.set(pos, size);
		r = _r;
		return *this;
	}

	constexpr RoundRect& RoundRect::set(const RectF& _rect, const value_type _r) noexcept
	{
		rect = _rect;
		r = _r;
		return *this;
	}

	constexpr RoundRect& RoundRect::set(const RoundRect& roundRect) noexcept
	{
		rect = roundRect.rect;
		r = roundRect.r;
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	movedBy
	//
	////////////////////////////////////////////////////////////////

	constexpr RoundRect RoundRect::movedBy(const value_type _x, const value_type _y) const noexcept
	{
		return{ rect.movedBy(_x, _y), r };
	}

	constexpr RoundRect RoundRect::movedBy(const size_type v) const noexcept
	{
		return{ rect.movedBy(v), r };
	}

	////////////////////////////////////////////////////////////////
	//
	//	moveBy
	//
	////////////////////////////////////////////////////////////////

	constexpr RoundRect& RoundRect::moveBy(const value_type _x, const value_type _y) noexcept
	{
		rect.moveBy(_x, _y);
		return *this;
	}

	constexpr RoundRect& RoundRect::moveBy(const size_type v) noexcept
	{
		rect.moveBy(v);
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	stretched
	//
	////////////////////////////////////////////////////////////////

	constexpr RoundRect RoundRect::stretched(const value_type xy) const noexcept
	{
		return{ rect.stretched(xy), r };
	}

	constexpr RoundRect RoundRect::stretched(const value_type _x, const value_type _y) const noexcept
	{
		return{ rect.stretched(_x, _y), r };
	}

	constexpr RoundRect RoundRect::stretched(const size_type xy) const noexcept
	{
		return{ rect.stretched(xy), r };
	}

	constexpr RoundRect RoundRect::stretched(const value_type top, const value_type right, const value_type bottom, const value_type left) const noexcept
	{
		return{ rect.stretched(top, right, bottom, left), r };
	}

	////////////////////////////////////////////////////////////////
	//
	//	scaledFromCenter
	//
	////////////////////////////////////////////////////////////////

	constexpr RoundRect RoundRect::scaledFromCenter(const double s) const noexcept
	{
		return{ rect.scaledFromCenter(s), r };
	}

	constexpr RoundRect RoundRect::scaledFromCenter(const double sx, const double sy) const noexcept
	{
		return{ rect.scaledFromCenter(sx, sy), r };
	}

	constexpr RoundRect RoundRect::scaledFromCenter(const Vec2 s) const noexcept
	{
		return{ rect.scaledFromCenter(s), r };
	}

	////////////////////////////////////////////////////////////////
	//
	//	scaledFromTopLeft
	//
	////////////////////////////////////////////////////////////////

	constexpr RoundRect RoundRect::scaledFromTopLeft(const double s) const noexcept
	{
		return{ rect.scaledFromTopLeft(s), r };
	}

	constexpr RoundRect RoundRect::scaledFromTopLeft(const double sx, const double sy) const noexcept
	{
		return{ rect.scaledFromTopLeft(sx, sy), r };
	}

	constexpr RoundRect RoundRect::scaledFromTopLeft(const Vec2 s) const noexcept
	{
		return{ rect.scaledFromTopLeft(s), r };
	}

	////////////////////////////////////////////////////////////////
	//
	//	scaledFrom
	//
	////////////////////////////////////////////////////////////////

	constexpr RoundRect RoundRect::scaledFrom(const Vec2 _pos, const double s) const noexcept
	{
		return{ rect.scaledFrom(_pos, s), r };
	}

	constexpr RoundRect RoundRect::scaledFrom(const Vec2 _pos, const double sx, const double sy) const noexcept
	{
		return{ rect.scaledFrom(_pos, sx, sy), r };
	}

	constexpr RoundRect RoundRect::scaledFrom(const Vec2 _pos, const Vec2 s) const noexcept
	{
		return{ rect.scaledFrom(_pos, s), r };
	}

	////////////////////////////////////////////////////////////////
	//
	//	hasArea
	//
	////////////////////////////////////////////////////////////////

	constexpr bool RoundRect::hasArea() const noexcept
	{
		return rect.hasArea();
	}

	////////////////////////////////////////////////////////////////
	//
	//	leftX, rightX, topY, bottomY
	//
	////////////////////////////////////////////////////////////////

	constexpr RoundRect::value_type RoundRect::leftX() const noexcept
	{
		return rect.leftX();
	}

	constexpr RoundRect::value_type RoundRect::rightX() const noexcept
	{
		return rect.rightX();
	}

	constexpr RoundRect::value_type RoundRect::topY() const noexcept
	{
		return rect.topY();
	}

	constexpr RoundRect::value_type RoundRect::bottomY() const noexcept
	{
		return rect.bottomY();
	}

	////////////////////////////////////////////////////////////////
	//
	//	centerX, centerY
	//
	////////////////////////////////////////////////////////////////

	constexpr double RoundRect::centerX() const noexcept
	{
		return rect.centerX();
	}

	constexpr double RoundRect::centerY() const noexcept
	{
		return rect.centerY();
	}

	////////////////////////////////////////////////////////////////
	//
	//	tl, tr, br, bl
	//
	////////////////////////////////////////////////////////////////

	constexpr RoundRect::position_type RoundRect::tl() const noexcept
	{
		return rect.tl();
	}

	constexpr RoundRect::position_type RoundRect::tr() const noexcept
	{
		return rect.tr();
	}

	constexpr RoundRect::position_type RoundRect::br() const noexcept
	{
		return rect.br();
	}

	constexpr RoundRect::position_type RoundRect::bl() const noexcept
	{
		return rect.bl();
	}

	////////////////////////////////////////////////////////////////
	//
	//	cornerTl, cornerTr, cornerBr, cornerBl
	//
	////////////////////////////////////////////////////////////////

	constexpr Vec2 RoundRect::cornerTl() const noexcept
	{
		const double t = (r * (1.0 - Math::InvSqrt2));
		return rect.tl().movedBy(t, t);
	}

	constexpr Vec2 RoundRect::cornerTr() const noexcept
	{
		const double t = (r * (1.0 - Math::InvSqrt2));
		return rect.tr().movedBy(-t, t);
	}

	constexpr Vec2 RoundRect::cornerBr() const noexcept
	{
		const double t = (r * (1.0 - Math::InvSqrt2));
		return rect.br().movedBy(-t, -t);
	}

	constexpr Vec2 RoundRect::cornerBl() const noexcept
	{
		const double t = (r * (1.0 - Math::InvSqrt2));
		return rect.bl().movedBy(t, -t);
	}

	////////////////////////////////////////////////////////////////
	//
	//	tlCenter, trCenter, brCenter, blCenter
	//
	////////////////////////////////////////////////////////////////

	constexpr Vec2 RoundRect::tlCenter() const noexcept
	{
		return rect.tl().movedBy(r, r);
	}

	constexpr Vec2 RoundRect::trCenter() const noexcept
	{
		return rect.tr().movedBy(-r, r);
	}

	constexpr Vec2 RoundRect::brCenter() const noexcept
	{
		return rect.br().movedBy(-r, -r);
	}

	constexpr Vec2 RoundRect::blCenter() const noexcept
	{
		return rect.bl().movedBy(r, -r);
	}

	////////////////////////////////////////////////////////////////
	//
	//	tlCircle, trCircle, brCircle, blCircle
	//
	////////////////////////////////////////////////////////////////

	constexpr Circle RoundRect::tlCircle() const noexcept
	{
		return{ tlCenter(), r };
	}

	constexpr Circle RoundRect::trCircle() const noexcept
	{
		return{ trCenter(), r };
	}

	constexpr Circle RoundRect::brCircle() const noexcept
	{
		return{ brCenter(), r };
	}

	constexpr Circle RoundRect::blCircle() const noexcept
	{
		return{ blCenter(), r };
	}

	////////////////////////////////////////////////////////////////
	//
	//	topCenter, rightCenter, bottomCenter, leftCenter
	//
	////////////////////////////////////////////////////////////////

	constexpr Vec2 RoundRect::topCenter() const noexcept
	{
		return rect.topCenter();
	}

	constexpr Vec2 RoundRect::rightCenter() const noexcept
	{
		return rect.rightCenter();
	}

	constexpr Vec2 RoundRect::bottomCenter() const noexcept
	{
		return rect.bottomCenter();
	}

	constexpr Vec2 RoundRect::leftCenter() const noexcept
	{
		return rect.leftCenter();
	}

	////////////////////////////////////////////////////////////////
	//
	//	center
	//
	////////////////////////////////////////////////////////////////

	constexpr Vec2 RoundRect::center() const noexcept
	{
		return rect.center();
	}

	////////////////////////////////////////////////////////////////
	//
	//	area
	//
	////////////////////////////////////////////////////////////////

	constexpr RoundRect::value_type RoundRect::area() const noexcept
	{
		return (rect.area() - (4 - Math::Pi) * r * r);
	}

	////////////////////////////////////////////////////////////////
	//
	//	perimeter
	//
	////////////////////////////////////////////////////////////////

	constexpr RoundRect::value_type RoundRect::perimeter() const noexcept
	{
		const double xl = (rect.w - 2 * r);
		const double yl = (rect.h - 2 * r);
		return ((xl * 2 + yl * 2) + (Math::TwoPi * r));
	}
}
