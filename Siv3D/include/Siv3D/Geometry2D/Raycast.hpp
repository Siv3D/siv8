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

# pragma once
# include "../Common.hpp"
# include "../Optional.hpp"
# include "../Number.hpp"
# include "../2DShapes.hpp"
# include "../Ray2D.hpp"
# include "../RaycastHit2D.hpp"

namespace s3d
{
	class Polygon;
	class MultiPolygon;

	namespace Geometry2D
	{
		/// @defgroup geometry2d_raycast レイと境界の最初のヒット
		/// @brief Raycast() の共通契約。
		/// @pre 図形は @ref geometry2d_queries の入力条件に従い、ray は有限の始点と単位方向ベクトルを持つ必要があります。
		/// @pre maxDistance は有限値または Math::Inf を指定します。
		/// @return 最初のヒット情報。範囲内にヒットがない、対象が面積を持たない、または maxDistance が負の場合は none。
		///
		/// 検索範囲は始点から [0, maxDistance] です（両端を含む）。MultiPolygon では面積を持つ要素だけを対象にします。
		/// 内部から出発するレイは最初に境界へ達する位置を返します。始点が境界上なら、向きによらず距離 0、startsInside は false です。
		/// 接するだけの場合もヒットし、境界の辺に沿う場合は共有区間の最初の位置を返します。
		/// Polygon の穴の輪郭も対象で、穴の内部からのレイは外部からのレイとして扱います。
		/// 法線は図形の外側へ向き、穴では穴の内部へ向きます。角・尖点など法線が一意でない箇所での選び方は未規定です。
		///
		/// @par 数値計算
		/// SuperEllipse のヒット位置は数値近似です。接触や検索範囲の端のごく近傍もヒットとみなす場合があります。
		/// 返す距離は [0, maxDistance] に収まります。境界近傍の始点は startsInside が false になる場合があります。
		/// @{

		template <class Shape>
		Optional<RaycastHit2D> Raycast(const Ray2D&, const Shape&, double) = delete;

		[[nodiscard]]
		Optional<RaycastHit2D> Raycast(const Ray2D& ray, const Rect& shape, double maxDistance = Math::Inf);

		[[nodiscard]]
		Optional<RaycastHit2D> Raycast(const Ray2D& ray, const RectF& shape, double maxDistance = Math::Inf);

		[[nodiscard]]
		Optional<RaycastHit2D> Raycast(const Ray2D& ray, const Circle& shape, double maxDistance = Math::Inf);

		[[nodiscard]]
		Optional<RaycastHit2D> Raycast(const Ray2D& ray, const Ellipse& shape, double maxDistance = Math::Inf);

		[[nodiscard]]
		Optional<RaycastHit2D> Raycast(const Ray2D& ray, const SuperEllipse& shape, double maxDistance = Math::Inf);

		[[nodiscard]]
		Optional<RaycastHit2D> Raycast(const Ray2D& ray, const Triangle& shape, double maxDistance = Math::Inf);

		[[nodiscard]]
		Optional<RaycastHit2D> Raycast(const Ray2D& ray, const Quad& shape, double maxDistance = Math::Inf);

		[[nodiscard]]
		Optional<RaycastHit2D> Raycast(const Ray2D& ray, const RoundRect& shape, double maxDistance = Math::Inf);

		[[nodiscard]]
		Optional<RaycastHit2D> Raycast(const Ray2D& ray, const Polygon& shape, double maxDistance = Math::Inf);

		[[nodiscard]]
		Optional<RaycastHit2D> Raycast(const Ray2D& ray, const MultiPolygon& shape, double maxDistance = Math::Inf);

		/// @}
	}
}
