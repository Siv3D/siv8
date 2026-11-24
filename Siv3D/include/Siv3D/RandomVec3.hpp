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

/// @file RandomVec3.hpp
/// @brief ランダムな 3 次元ベクトルや、立体の内部・表面上の位置を生成します。
/// @details
/// 内部では同じ体積の領域が、球面上では同じ面積の領域が、同じ確率で選ばれます。
///
/// 乱数エンジンを省略すると、現在のスレッドの既定の乱数エンジンを使います。
/// 指定した乱数エンジンはコピーせずに使います。
///
/// 座標、長さ、半径、範囲の端点には有限値を指定してください。
/// 長さや球面上の位置には丸め誤差があります。
/// 球や球殻の内部を選ぶ場合も、誤差により境界をわずかに越えることがあります。

# pragma once
# include <utility>
# include "Common.hpp"
# include "PointVector.hpp"
# include "Random.hpp"
# include "MathConstants.hpp"
# include "Box.hpp"
# include "Sphere.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	RandomUnitVec3
	//
	////////////////////////////////////////////////////////////////

	/// @brief ランダムな向きの単位ベクトルを返します。
	/// @return 全方向に均等に分布する、長さ 1 のベクトル
	[[nodiscard]]
	Vec3 RandomUnitVec3();

	/// @brief ランダムな向きの単位ベクトルを返します。
	/// @param urbg 使用する乱数エンジン
	/// @return 全方向に均等に分布する、長さ 1 のベクトル
	[[nodiscard]]
	Vec3 RandomUnitVec3(Concept::UniformRandomBitGenerator auto&& urbg);

	////////////////////////////////////////////////////////////////
	//
	//	RandomVec3
	//
	////////////////////////////////////////////////////////////////

	/// @brief 長さを指定して、ランダムな向きのベクトルを返します。
	/// @param length ベクトルの長さ（0 以上）
	/// @return 全方向に均等に分布する、指定した長さのベクトル。length が 0 の場合はゼロベクトル
	[[nodiscard]]
	Vec3 RandomVec3(double length);

	/// @brief 長さを指定して、ランダムな向きのベクトルを返します。
	/// @param length ベクトルの長さ（0 以上）
	/// @param urbg 使用する乱数エンジン
	/// @return 全方向に均等に分布する、指定した長さのベクトル。length が 0 の場合はゼロベクトル
	[[nodiscard]]
	Vec3 RandomVec3(double length, Concept::UniformRandomBitGenerator auto&& urbg);

	/// @brief 各成分の範囲を指定して、ランダムなベクトルを返します。
	/// @param xMinMax X 成分の { 最小値, 上限値 }
	/// @param yMinMax Y 成分の { 最小値, 上限値 }
	/// @param zMinMax Z 成分の { 最小値, 上限値 }
	/// @pre 各範囲は最小値 <= 上限値とし、その差が double の有限値に収まるように指定してください。
	/// @return 各成分をそれぞれの範囲から独立に、一様に選んだベクトル。
	/// 最小値を含み、上限値は含みません。両者が等しい成分は、その値に固定されます。
	/// @par 使用例
	/// @code
	/// const Vec3 p = RandomVec3({ -1.0, 1.0 }, { 5.0, 5.0 }, { 0.0, 10.0 });
	/// // -1 <= p.x < 1、p.y == 5、0 <= p.z < 10
	/// @endcode
	[[nodiscard]]
	Vec3 RandomVec3(
		const std::pair<double, double>& xMinMax,
		const std::pair<double, double>& yMinMax,
		const std::pair<double, double>& zMinMax);

	/// @brief 各成分の範囲を指定して、ランダムなベクトルを返します。
	/// @param xMinMax X 成分の { 最小値, 上限値 }
	/// @param yMinMax Y 成分の { 最小値, 上限値 }
	/// @param zMinMax Z 成分の { 最小値, 上限値 }
	/// @param urbg 使用する乱数エンジン
	/// @pre 各範囲は最小値 <= 上限値とし、その差が double の有限値に収まるように指定してください。
	/// @return 各成分をそれぞれの範囲から独立に、一様に選んだベクトル。
	/// 最小値を含み、上限値は含みません。両者が等しい成分は、その値に固定されます。
	[[nodiscard]]
	Vec3 RandomVec3(
		const std::pair<double, double>& xMinMax,
		const std::pair<double, double>& yMinMax,
		const std::pair<double, double>& zMinMax, Concept::UniformRandomBitGenerator auto&& urbg);

	/// @brief 各成分の上限を指定して、ランダムなベクトルを返します。
	/// @param xMax X 成分の上限値（0 以上）
	/// @param yMax Y 成分の上限値（0 以上）
	/// @param zMax Z 成分の上限値（0 以上）
	/// @return 各成分を 0 以上、指定した上限未満から独立に、一様に選んだベクトル。上限が 0 の成分は 0
	[[nodiscard]]
	Vec3 RandomVec3(double xMax, double yMax, double zMax);

	/// @brief 各成分の上限を指定して、ランダムなベクトルを返します。
	/// @param xMax X 成分の上限値（0 以上）
	/// @param yMax Y 成分の上限値（0 以上）
	/// @param zMax Z 成分の上限値（0 以上）
	/// @param urbg 使用する乱数エンジン
	/// @return 各成分を 0 以上、指定した上限未満から独立に、一様に選んだベクトル。上限が 0 の成分は 0
	[[nodiscard]]
	Vec3 RandomVec3(double xMax, double yMax, double zMax, Concept::UniformRandomBitGenerator auto&& urbg);

	/// @brief 直方体の内部から、ランダムな位置を返します。
	/// @param box 対象の直方体（各寸法は 0 以上）
	/// @pre 各軸の範囲の端点（center ± size / 2）と幅が double の有限値に収まるように指定してください。
	/// @return 各成分を独立に選んだ、直方体内に均等に分布する位置。
	/// 各軸の最小値を含み、最大値は含みません。寸法が 0 の軸は中心座標に固定されます。
	[[nodiscard]]
	Vec3 RandomVec3(const Box& box);

	/// @brief 直方体の内部から、ランダムな位置を返します。
	/// @param box 対象の直方体（各寸法は 0 以上）
	/// @param urbg 使用する乱数エンジン
	/// @pre 各軸の範囲の端点（center ± size / 2）と幅が double の有限値に収まるように指定してください。
	/// @return 各成分を独立に選んだ、直方体内に均等に分布する位置。
	/// 各軸の最小値を含み、最大値は含みません。寸法が 0 の軸は中心座標に固定されます。
	[[nodiscard]]
	Vec3 RandomVec3(const Box& box, Concept::UniformRandomBitGenerator auto&& urbg);

	/// @brief 球の内部から、ランダムな位置を返します。
	/// @param sphere 対象の球（半径は 0 以上）
	/// @pre 球全体の座標（中心の各成分 ± 半径）が double の有限値に収まるように指定してください。
	/// @return 球の内部に均等に分布する位置。半径が 0 の場合は中心
	[[nodiscard]]
	Vec3 RandomVec3(const Sphere& sphere);

	/// @brief 球の内部から、ランダムな位置を返します。
	/// @param sphere 対象の球（半径は 0 以上）
	/// @param urbg 使用する乱数エンジン
	/// @pre 球全体の座標（中心の各成分 ± 半径）が double の有限値に収まるように指定してください。
	/// @return 球の内部に均等に分布する位置。半径が 0 の場合は中心
	[[nodiscard]]
	Vec3 RandomVec3(const Sphere& sphere, Concept::UniformRandomBitGenerator auto&& urbg);

	////////////////////////////////////////////////////////////////
	//
	//	RandomVec3InsideUnitSphere
	//
	////////////////////////////////////////////////////////////////

	/// @brief 原点を中心とする半径 1 の球の内部から、ランダムな位置を返します。
	/// @return 単位球の内部に均等に分布する位置
	[[nodiscard]]
	Vec3 RandomVec3InsideUnitSphere();

	/// @brief 原点を中心とする半径 1 の球の内部から、ランダムな位置を返します。
	/// @param urbg 使用する乱数エンジン
	/// @return 単位球の内部に均等に分布する位置
	[[nodiscard]]
	Vec3 RandomVec3InsideUnitSphere(Concept::UniformRandomBitGenerator auto&& urbg);

	////////////////////////////////////////////////////////////////
	//
	//	RandomVec3On
	//
	////////////////////////////////////////////////////////////////

	/// @brief 球の表面から、ランダムな位置を返します。
	/// @param sphere 対象の球（半径は 0 以上）
	/// @pre 球全体の座標（中心の各成分 ± 半径）が double の有限値に収まるように指定してください。
	/// @return 球面上に均等に分布する位置。半径が 0 の場合は中心
	[[nodiscard]]
	Vec3 RandomVec3On(const Sphere& sphere);

	/// @brief 球の表面から、ランダムな位置を返します。
	/// @param sphere 対象の球（半径は 0 以上）
	/// @param urbg 使用する乱数エンジン
	/// @pre 球全体の座標（中心の各成分 ± 半径）が double の有限値に収まるように指定してください。
	/// @return 球面上に均等に分布する位置。半径が 0 の場合は中心
	[[nodiscard]]
	Vec3 RandomVec3On(const Sphere& sphere, Concept::UniformRandomBitGenerator auto&& urbg);

	////////////////////////////////////////////////////////////////
	//
	//	RandomVec3InsideSphericalShell
	//
	////////////////////////////////////////////////////////////////

	/// @brief 原点を中心とする 2 つの球の間（球殻）から、ランダムな位置を返します。
	/// @param innerRadius 内側の球の半径（0 以上）
	/// @param outerRadius 外側の球の半径（innerRadius 以上）
	/// @return 球殻の内部に均等に分布する位置。
	/// 両半径が等しい場合はその球面上に均等に分布する位置、両方 0 の場合は原点を返します。
	/// @par 使用例
	/// @code
	/// const Vec3 p = RandomVec3InsideSphericalShell(2.0, 5.0);
	/// // 原点から 2 ～ 5 離れた球殻内の位置
	/// @endcode
	[[nodiscard]]
	Vec3 RandomVec3InsideSphericalShell(double innerRadius, double outerRadius);

	/// @brief 原点を中心とする 2 つの球の間（球殻）から、ランダムな位置を返します。
	/// @param innerRadius 内側の球の半径（0 以上）
	/// @param outerRadius 外側の球の半径（innerRadius 以上）
	/// @param urbg 使用する乱数エンジン
	/// @return 球殻の内部に均等に分布する位置。
	/// 両半径が等しい場合はその球面上に均等に分布する位置、両方 0 の場合は原点を返します。
	[[nodiscard]]
	Vec3 RandomVec3InsideSphericalShell(double innerRadius, double outerRadius, Concept::UniformRandomBitGenerator auto&& urbg);
}

# include "detail/RandomVec3.ipp"
