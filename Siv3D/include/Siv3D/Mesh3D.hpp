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
# include <array>
# include <span>
# include "Common.hpp"
# include "Array.hpp"
# include "Blob.hpp"
# include "BoxUVMapping.hpp"
# include "Grid.hpp"
# include "IWriter.hpp"
# include "Material.hpp"
# include "PredefinedYesNo.hpp"
# include "Vertex3D.hpp"
# include "TriangleIndex32.hpp"
# include "VertexNormalWeighting.hpp"

namespace s3d
{
	class Polygon;
	struct Mat4x4;
	struct Quaternion;

	////////////////////////////////////////////////////////////////
	//
	//	Mesh3D
	//
	////////////////////////////////////////////////////////////////

	/// @brief 3D メッシュデータ
	struct Mesh3D
	{
		/// @brief 頂点配列
		Array<Vertex3D> vertices;

		/// @brief 三角形インデックス配列
		Array<TriangleIndex32> indices;

		/// @brief 使用可能な最大の頂点インデックス
		/// @remark 32-bit index の最大値 0xFFFFFFFF は、一部のグラフィックス API で
		/// primitive restart / strip cut の特殊値として予約されているため使用しません。
		static constexpr size_t MaxVertexIndex = 4294967294;

		/// @brief Mesh3D が保持できる最大の頂点数
		static constexpr size_t MaxVertexCount = (MaxVertexIndex + 1);

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		/// @brief 空の 3D メッシュを作成します。
		[[nodiscard]]
		Mesh3D() = default;

		/// @brief 指定した数の頂点と三角形を持つ 3D メッシュを作成します。
		/// @param vertexCount 頂点数
		/// @param triangleCount 三角形数
		[[nodiscard]]
		Mesh3D(size_t vertexCount, size_t triangleCount);

		/// @brief 頂点配列と三角形インデックス配列から 3D メッシュを作成します。
		/// @param _vertices 頂点配列
		/// @param _indices 三角形インデックス配列
		[[nodiscard]]
		Mesh3D(Array<Vertex3D> _vertices, Array<TriangleIndex32> _indices);
		
		/// @brief 頂点と三角形インデックスの範囲から 3D メッシュを作成します。
		/// @param _vertices コピーする頂点の範囲
		/// @param _indices コピーする三角形インデックスの範囲
		[[nodiscard]]
		Mesh3D(std::span<const Vertex3D> _vertices, std::span<const TriangleIndex32> _indices);

		////////////////////////////////////////////////////////////////
		//
		//	Box
		//
		////////////////////////////////////////////////////////////////

		/// @brief 原点を中心とする直方体の 3D メッシュを作成します。
		/// @param size 直方体の各軸方向の大きさ
		/// @return 直方体の 3D メッシュ。`size` のいずれかの成分が正の有限値でない場合、または float で表現できない場合は空の 3D メッシュ
		/// @remark 各面は独立した頂点を持ち、面ごとに `[0, 1]` の UV 座標が割り当てられます。
		[[nodiscard]]
		static Mesh3D Box(Vec3 size = Vec3{ 1.0, 1.0, 1.0 });

		/// @brief 原点を中心とする直方体の 3D メッシュを作成します。
		/// @param size 直方体の各軸方向の大きさ
		/// @param uvMapping 各面に割り当てる UV 矩形
		/// @return 直方体の 3D メッシュ。`size` または `uvMapping` が不正な場合は空の 3D メッシュ
		/// @remark 各面は独立した頂点を持ち、`uvMapping` の対応する矩形が割り当てられます。
		[[nodiscard]]
		static Mesh3D Box(Vec3 size, const BoxUVMapping& uvMapping);

		////////////////////////////////////////////////////////////////
		//
		//	RoundedBox
		//
		////////////////////////////////////////////////////////////////

		/// @brief 原点を中心とする角丸直方体の 3D メッシュを作成します。
		/// @param size 角丸直方体の各軸方向の大きさ
		/// @param radius 角の丸みの半径。0 以上、`size` の最小成分の半分以下である必要があります。
		/// @param subdivisions 各面の丸み部分の分割数。1 以上である必要があります。
		/// @return 角丸直方体の 3D メッシュ。引数が不正な場合、または頂点数が上限を超える場合は空の 3D メッシュ
		/// @remark `radius == 0` の場合は `Box(size)` と同じ 3D メッシュを返します。
		/// @remark UV 座標は、形状全体のバウンディングボックスに対する Box と同じ投影で割り当てられます。
		[[nodiscard]]
		static Mesh3D RoundedBox(
			Vec3 size = Vec3{ 1.0, 1.0, 1.0 },
			double radius = 0.1,
			uint32 subdivisions = 4);

		/// @brief 原点を中心とする角丸直方体の 3D メッシュを作成します。
		/// @param size 角丸直方体の各軸方向の大きさ
		/// @param radius 角の丸みの半径。0 以上、`size` の最小成分の半分以下である必要があります。
		/// @param subdivisions 各面の丸み部分の分割数。1 以上である必要があります。
		/// @param uvMapping 各面から形状全体のバウンディングボックスへ投影する UV 矩形
		/// @return 角丸直方体の 3D メッシュ。引数または `uvMapping` が不正な場合、または頂点数が上限を超える場合は空の 3D メッシュ
		/// @remark 丸い辺と角を含め、各面は `BoxUVMapping` の対応する矩形へ投影されます。面の境界は UV の継ぎ目になります。
		[[nodiscard]]
		static Mesh3D RoundedBox(
			Vec3 size,
			double radius,
			uint32 subdivisions,
			const BoxUVMapping& uvMapping);

		////////////////////////////////////////////////////////////////
		//
		//	Wedge
		//
		////////////////////////////////////////////////////////////////

		/// @brief 原点を中心とし、Z 軸の正方向へ上るくさび形の 3D メッシュを作成します。
		/// @param size くさび形の各軸方向の大きさ
		/// @return くさび形の 3D メッシュ。`size` のいずれかの成分が正の有限値でない場合、または float で表現できない場合は空の 3D メッシュ
		/// @remark X 軸方向を幅、Y 軸方向を高さ、Z 軸方向を奥行きとし、斜面は `-Z` 側の下端から `+Z` 側の上端へ伸びます。
		/// @remark UV 座標は、形状全体のバウンディングボックスに対する Box と同じ投影で割り当てられます。斜面には `BoxUVMapping::positiveY` が使用されます。
		[[nodiscard]]
		static Mesh3D Wedge(Vec3 size = Vec3{ 1.0, 1.0, 1.0 });

		/// @brief 原点を中心とし、Z 軸の正方向へ上るくさび形の 3D メッシュを作成します。
		/// @param size くさび形の各軸方向の大きさ
		/// @param uvMapping 形状全体のバウンディングボックスへ投影する各面の UV 矩形
		/// @return くさび形の 3D メッシュ。`size` または `uvMapping` が不正な場合は空の 3D メッシュ
		/// @remark X 軸方向を幅、Y 軸方向を高さ、Z 軸方向を奥行きとし、斜面は `-Z` 側の下端から `+Z` 側の上端へ伸びます。
		/// @remark 斜面には `BoxUVMapping::positiveY`、垂直な背面には `positiveZ`、底面には `negativeY`、左右の側面には `positiveX` と `negativeX` が使用されます。
		[[nodiscard]]
		static Mesh3D Wedge(Vec3 size, const BoxUVMapping& uvMapping);

		////////////////////////////////////////////////////////////////
		//
		//	TriangularPrism
		//
		////////////////////////////////////////////////////////////////

		/// @brief 原点を中心とし、Y-Z 平面に平行な二等辺三角形の断面を持つ三角柱の 3D メッシュを作成します。
		/// @param size 三角柱の各軸方向の大きさ
		/// @return 三角柱の 3D メッシュ。`size` のいずれかの成分が正の有限値でない場合、または float で表現できない場合は空の 3D メッシュ
		/// @remark X 軸方向を長さ、Y 軸方向を全高、Z 軸方向を底面幅とします。
		/// @remark Y-Z 断面の頂点は `(0, size.y / 2, 0)`、底辺は `y = -size.y / 2` に配置されます。
		/// @remark UV 座標は、形状全体のバウンディングボックスに対する Box と同じ投影で割り当てられます。2 枚の斜面には `BoxUVMapping::positiveY` が使用されます。
		[[nodiscard]]
		static Mesh3D TriangularPrism(Vec3 size = Vec3{ 1.0, 1.0, 1.0 });

		/// @brief 原点を中心とし、Y-Z 平面に平行な二等辺三角形の断面を持つ三角柱の 3D メッシュを作成します。
		/// @param size 三角柱の各軸方向の大きさ
		/// @param uvMapping 形状全体のバウンディングボックスへ投影する各面の UV 矩形
		/// @return 三角柱の 3D メッシュ。`size` または `uvMapping` が不正な場合は空の 3D メッシュ
		/// @remark X 軸方向を長さ、Y 軸方向を全高、Z 軸方向を底面幅とします。
		/// @remark 2 枚の斜面には `BoxUVMapping::positiveY`、両端には `positiveX` と `negativeX`、底面には `negativeY` が使用されます。
		[[nodiscard]]
		static Mesh3D TriangularPrism(Vec3 size, const BoxUVMapping& uvMapping);

		////////////////////////////////////////////////////////////////
		//
		//	Stairs
		//
		////////////////////////////////////////////////////////////////

		/// @brief 原点を中心とし、Z 軸の正方向へ上る階段の 3D メッシュを作成します。
		/// @param size 階段全体の各軸方向の大きさ
		/// @param steps 段数。1 以上である必要があります。
		/// @return 階段の 3D メッシュ。引数が不正な場合、または頂点数が上限を超える場合は空の 3D メッシュ
		/// @remark X 軸方向を幅、Y 軸方向を全体の高さ、Z 軸方向を全体の奥行きとします。
		/// @remark UV 座標は、階段全体のバウンディングボックスに対する Box と同じ投影で割り当てられます。
		[[nodiscard]]
		static Mesh3D Stairs(Vec3 size, uint32 steps);

		/// @brief 原点を中心とし、Z 軸の正方向へ上る階段の 3D メッシュを作成します。
		/// @param size 階段全体の各軸方向の大きさ
		/// @param steps 段数。1 以上である必要があります。
		/// @param uvMapping 階段全体のバウンディングボックスへ投影する各面の UV 矩形
		/// @return 階段の 3D メッシュ。引数または `uvMapping` が不正な場合、または頂点数が上限を超える場合は空の 3D メッシュ
		/// @remark 踏み面には `BoxUVMapping::positiveY`、蹴上げ面には `negativeZ`、背面には `positiveZ`、底面には `negativeY`、左右の側面には `positiveX` と `negativeX` が使用されます。
		/// @remark 各 UV 矩形は個々の段へ引き伸ばされず、階段全体のバウンディングボックスを基準に投影されます。
		[[nodiscard]]
		static Mesh3D Stairs(Vec3 size, uint32 steps, const BoxUVMapping& uvMapping);

		////////////////////////////////////////////////////////////////
		//
		//	Pyramid
		//
		////////////////////////////////////////////////////////////////

		/// @brief 原点を中心とし、XZ 平面に平行な正方形の底面を持つ四角錐の 3D メッシュを作成します。
		/// @param baseSize 底面の一辺の長さ
		/// @param height 四角錐の高さ
		/// @return 四角錐の 3D メッシュ。`baseSize` または `height` が正の有限値でない場合、または float で表現できない場合は空の 3D メッシュ
		[[nodiscard]]
		static Mesh3D Pyramid(double baseSize, double height);

		/// @brief 原点を中心とし、XZ 平面に平行な長方形の底面を持つ四角錐の 3D メッシュを作成します。
		/// @param baseSizeXZ 底面の X 軸方向および Z 軸方向の大きさ
		/// @param height 四角錐の高さ
		/// @return 四角錐の 3D メッシュ。`baseSizeXZ` または `height` が正の有限値でない場合、または float で表現できない場合は空の 3D メッシュ
		/// @remark 底面は `y = -height / 2`、頂点は `(0, height / 2, 0)` に配置されます。
		/// @remark 各側面と底面は独立した頂点を持ち、それぞれに `[0, 1]` の UV 座標が割り当てられます。
		[[nodiscard]]
		static Mesh3D Pyramid(
			SizeF baseSizeXZ = SizeF{ 1.0, 1.0 },
			double height = 1.0);

		////////////////////////////////////////////////////////////////
		//
		//	RectangularFrustum
		//
		////////////////////////////////////////////////////////////////

		/// @brief 原点を中心とし、XZ 平面に平行な長方形の上下端を持つ角錐台の 3D メッシュを作成します。
		/// @param bottomSizeXZ 底面の X 軸方向および Z 軸方向の大きさ
		/// @param topSizeXZ 上面の X 軸方向および Z 軸方向の大きさ
		/// @param height 角錐台の高さ
		/// @return 角錐台の 3D メッシュ。いずれかの大きさが正の有限値でない場合、または float で表現できない場合は空の 3D メッシュ
		/// @remark 底面は `y = -height / 2`、上面は `y = height / 2` に配置されます。
		/// @remark UV 座標は、形状全体のバウンディングボックスに対する Box と同じ投影で割り当てられます。
		[[nodiscard]]
		static Mesh3D RectangularFrustum(
			SizeF bottomSizeXZ,
			SizeF topSizeXZ,
			double height);

		/// @brief 原点を中心とし、XZ 平面に平行な長方形の上下端を持つ角錐台の 3D メッシュを作成します。
		/// @param bottomSizeXZ 底面の X 軸方向および Z 軸方向の大きさ
		/// @param topSizeXZ 上面の X 軸方向および Z 軸方向の大きさ
		/// @param height 角錐台の高さ
		/// @param uvMapping 形状全体のバウンディングボックスへ投影する各面の UV 矩形
		/// @return 角錐台の 3D メッシュ。大きさまたは `uvMapping` が不正な場合は空の 3D メッシュ
		[[nodiscard]]
		static Mesh3D RectangularFrustum(
			SizeF bottomSizeXZ,
			SizeF topSizeXZ,
			double height,
			const BoxUVMapping& uvMapping);

		////////////////////////////////////////////////////////////////
		//
		//	Extrude
		//
		////////////////////////////////////////////////////////////////

		/// @brief 2D の多角形を Y 軸方向に押し出した 3D メッシュを作成します。
		/// @param polygon 押し出す多角形。穴を含むことができます。
		/// @param height 押し出す高さ
		/// @return 押し出し形状の 3D メッシュ。`polygon` または `height` が不正な場合、または頂点数が上限を超える場合は空の 3D メッシュ
		/// @remark `polygon` の X 座標を X 軸、Y 座標を Z 軸の負方向へ対応させます。多角形の位置は平行移動せず、高さ方向のみ原点を中心として、下面を `y = -height / 2`、上面を `y = height / 2` に配置します。
		/// @remark 上下面は閉じられます。上面の UV 座標は多角形のバウンディングボックスを `[0, 1]` に正規化し、下面は表側から同じ向きに見えるよう V 座標を反転します。
		/// @remark 側面の U 座標は外周および各穴の周長をそれぞれ `[0, 1]` に正規化し、V 座標は上端を 0、下端を 1 とします。多角形の各頂点はハードエッジになります。
		[[nodiscard]]
		static Mesh3D Extrude(const Polygon& polygon, double height);

		/// @brief 2D の多角形を Y 軸方向に押し出し、側面の法線を角度に応じて補間した 3D メッシュを作成します。
		/// @param polygon 押し出す多角形。穴を含むことができます。
		/// @param height 押し出す高さ
		/// @param smoothingAngle 側面の法線を補間する隣接面間の最大角度（ラジアン）。0 以上 π 以下
		/// @return 押し出し形状の 3D メッシュ。引数が不正な場合、または頂点数が上限を超える場合は空の 3D メッシュ
		/// @remark `smoothingAngle` 以下の角度で接続する側面間では、共有する輪郭頂点の法線と接線を補間します。上下面と側面の境界は補間しません。
		/// @remark 座標および UV 座標の規約は 2 引数版の `Extrude()` と同じです。
		[[nodiscard]]
		static Mesh3D Extrude(const Polygon& polygon, double height, double smoothingAngle);

		////////////////////////////////////////////////////////////////
		//
		//	Revolve
		//
		////////////////////////////////////////////////////////////////

		/// @brief 2D プロファイルを Y 軸の周りに一周回転させた 3D メッシュを作成します。
		/// @param profile 回転させるプロファイル。各要素の X 座標を半径、Y 座標を生成後の Y 座標として使用します。
		/// @param segments 回転方向の分割数。3 以上である必要があります。
		/// @return 回転体の 3D メッシュ。引数が不正な場合、または頂点数が上限を超える場合は空の 3D メッシュ
		/// @remark プロファイルの各線分を個別のハードエッジとして生成します。回転方向の法線は滑らかに接続されます。
		/// @remark 半径 0 の端点は軸上の頂点として閉じられます。半径が正の端点には開口リングが残ります。端面は暗黙には追加されません。
		/// @remark 先頭要素と末尾要素が完全に一致する場合は閉じたプロファイルとして扱います。閉じ目には UV seam のための重複頂点が作成されます。
		/// @remark 外側輪郭を Y 座標の小さい側から大きい側へ並べると外向きの面になります。プロファイルの順序を反転すると面と法線の向きも反転します。
		/// @remark UV 座標の U は `+X` 方向を 0 として `+Z` 方向へ増加し、V はプロファイルの累積距離を `[0, 1]` に正規化した値です。
		/// @remark プロファイルの自己交差および回転後の自己交差は検査しません。
		[[nodiscard]]
		static Mesh3D Revolve(std::span<const Vec2> profile, uint32 segments = 32);

		/// @brief 2D プロファイルを Y 軸の周りに一周回転させ、プロファイル方向の法線を角度に応じて補間した 3D メッシュを作成します。
		/// @param profile 回転させるプロファイル。各要素の X 座標を半径、Y 座標を生成後の Y 座標として使用します。
		/// @param segments 回転方向の分割数。3 以上である必要があります。
		/// @param smoothingAngle プロファイル方向の法線を補間する隣接面間の最大角度（ラジアン）。0 以上 π 以下
		/// @return 回転体の 3D メッシュ。引数が不正な場合、または頂点数が上限を超える場合は空の 3D メッシュ
		/// @remark `smoothingAngle` 以下の角度で接続するプロファイル線分間では、共有するプロファイル頂点の法線を補間します。回転方向の法線は常に滑らかに接続されます。
		/// @remark 座標、開口、および UV 座標の規約は 2 引数版の `Revolve()` と同じです。
		[[nodiscard]]
		static Mesh3D Revolve(std::span<const Vec2> profile, uint32 segments, double smoothingAngle);

		////////////////////////////////////////////////////////////////
		//
		//	Tube
		//
		////////////////////////////////////////////////////////////////

		/// @brief 3D 経路に沿う一定半径のチューブを作成します。
		/// @param path チューブの中心を通る開いた経路。2 点以上である必要があります。
		/// @param radius チューブの半径
		/// @param sides チューブ断面の分割数。3 以上である必要があります。
		/// @param uvScale UV 座標の拡大率
		/// @param uvOffset UV 座標のオフセット
		/// @return 経路に沿うチューブの 3D メッシュ。引数が不正な場合、または頂点数が上限を超える場合は空の 3D メッシュ
		/// @remark parallel-transport frame を使って断面を経路に沿って運びます。折れ点の断面は前後の線分方向を平均した平面上に配置されます。
		/// @remark 始端と終端は閉じられ、側面との境界はハードエッジになります。円周方向および経路の折れ点では法線が滑らかに接続されます。
		/// @remark 側面の U 座標は円周を `[0, 1]` とし、V 座標は経路の始端からの累積距離です。その後に `uvScale` と `uvOffset` が適用されます。
		/// @remark 端面の UV 座標は各円を `[0, 1]` 四方に収める平面投影とし、同じ `uvScale` と `uvOffset` が適用されます。
		/// @remark 閉じた経路、連続する同一点、および 180° 折り返す経路には対応しません。鋭い折れ点における自己交差は検査しません。
		[[nodiscard]]
		static Mesh3D Tube(
			std::span<const Vec3> path,
			double radius,
			uint32 sides = 12,
			Vec2 uvScale = Vec2{ 1.0, 1.0 },
			Vec2 uvOffset = Vec2{ 0.0, 0.0 });

		////////////////////////////////////////////////////////////////
		//
		//	Sweep
		//
		////////////////////////////////////////////////////////////////

		/// @brief 2D 断面を 3D 経路に沿わせた 3D メッシュを作成します。
		/// @param crossSection 経路に沿わせる断面。穴を含むことができます。
		/// @param path 断面の中心を通る開いた経路。2 点以上である必要があります。
		/// @param uvScale UV 座標の拡大率
		/// @param uvOffset UV 座標のオフセット
		/// @return 断面を経路に沿わせた 3D メッシュ。引数が不正な場合、または頂点数が上限を超える場合は空の 3D メッシュ
		/// @remark parallel-transport frame を使って断面を経路に沿って運びます。開始時の断面の向きは、最初の経路方向と最も平行でない座標軸から自動的に決定します。
		/// @remark 断面の X 座標はフレームの第 1 軸、Y 座標は第 2 軸に対応します。断面の位置は平行移動しません。
		/// @remark 始端と終端は閉じられ、端面と側面、および断面の各頂点はハードエッジになります。経路の折れ点では法線が滑らかに接続されます。
		/// @remark 側面の U 座標は外周と各穴の周長をそれぞれ `[0, 1]` とし、V 座標は経路の始端からの累積距離です。その後に `uvScale` と `uvOffset` が適用されます。
		/// @remark 端面の UV 座標は断面全体のバウンディングボックスを `[0, 1]` 四方に正規化し、同じ `uvScale` と `uvOffset` を適用します。
		/// @remark 閉じた経路、連続する同一点、および 180° 折り返す経路には対応しません。鋭い折れ点における自己交差は検査しません。
		[[nodiscard]]
		static Mesh3D Sweep(
			const Polygon& crossSection,
			std::span<const Vec3> path,
			Vec2 uvScale = Vec2{ 1.0, 1.0 },
			Vec2 uvOffset = Vec2{ 0.0, 0.0 });

		/// @brief 開始時の断面方向を指定し、2D 断面を 3D 経路に沿わせた 3D メッシュを作成します。
		/// @param crossSection 経路に沿わせる断面。穴を含むことができます。
		/// @param path 断面の中心を通る開いた経路。2 点以上である必要があります。
		/// @param initialNormal 開始時に断面の X 軸を向ける方向。最初の経路方向に垂直な平面へ投影して使用します。
		/// @param uvScale UV 座標の拡大率
		/// @param uvOffset UV 座標のオフセット
		/// @return 断面を経路に沿わせた 3D メッシュ。引数が不正な場合、または頂点数が上限を超える場合は空の 3D メッシュ
		/// @remark `initialNormal` がゼロベクトル、非有限値、または最初の経路方向と平行な場合は空の 3D メッシュを返します。
		/// @remark 断面、経路、および UV 座標の規約は自動方向版の `Sweep()` と同じです。
		[[nodiscard]]
		static Mesh3D Sweep(
			const Polygon& crossSection,
			std::span<const Vec3> path,
			Vec3 initialNormal,
			Vec2 uvScale = Vec2{ 1.0, 1.0 },
			Vec2 uvOffset = Vec2{ 0.0, 0.0 });

		////////////////////////////////////////////////////////////////
		//
		//	Regular polyhedra
		//
		////////////////////////////////////////////////////////////////

		/// @brief 原点を中心とする正四面体の 3D メッシュを作成します。
		/// @param radius 外接球の半径
		/// @return 正四面体の 3D メッシュ。`radius` が不正な場合は空の 3D メッシュ
		/// @remark 各面には独立した `[0, 1]` の UV 座標が割り当てられます。
		[[nodiscard]]
		static Mesh3D Tetrahedron(double radius = 1.0);

		/// @brief 原点を中心とする正八面体の 3D メッシュを作成します。
		/// @param radius 外接球の半径
		/// @return 正八面体の 3D メッシュ。`radius` が不正な場合は空の 3D メッシュ
		/// @remark 各面には独立した `[0, 1]` の UV 座標が割り当てられます。
		[[nodiscard]]
		static Mesh3D Octahedron(double radius = 1.0);

		/// @brief 原点を中心とする正二十面体の 3D メッシュを作成します。
		/// @param radius 外接球の半径
		/// @return 正二十面体の 3D メッシュ。`radius` が不正な場合は空の 3D メッシュ
		/// @remark 各面には独立した `[0, 1]` の UV 座標が割り当てられます。
		[[nodiscard]]
		static Mesh3D Icosahedron(double radius = 1.0);

		/// @brief 原点を中心とする正十二面体の 3D メッシュを作成します。
		/// @param radius 外接球の半径
		/// @return 正十二面体の 3D メッシュ。`radius` が不正な場合は空の 3D メッシュ
		/// @remark 各面には正五角形を含む独立した `[0, 1]` の UV 座標が割り当てられます。
		[[nodiscard]]
		static Mesh3D Dodecahedron(double radius = 1.0);

		////////////////////////////////////////////////////////////////
		//
		//	Plane
		//
		////////////////////////////////////////////////////////////////

		/// @brief XZ 平面上に、法線が Y 軸の正方向を向く長方形の 3D メッシュを作成します。
		/// @param sizeXZ X 軸方向および Z 軸方向の大きさ
		/// @param uvScale UV 座標の拡大率
		/// @param uvOffset UV 座標のオフセット
		/// @return 長方形の 3D メッシュ。大きさまたは UV パラメータが不正な場合は空の 3D メッシュ
		/// @remark UV 座標の V 成分は Z 軸の正方向から負方向へ向かって増加します。
		[[nodiscard]]
		static Mesh3D Plane(
			SizeF sizeXZ = SizeF{ 1.0, 1.0 },
			Vec2 uvScale = Vec2{ 1.0, 1.0 },
			Vec2 uvOffset = Vec2{ 0.0, 0.0 });

		////////////////////////////////////////////////////////////////
		//
		//	Grid
		//
		////////////////////////////////////////////////////////////////

		/// @brief XZ 平面上に、法線が Y 軸の正方向を向く格子状の 3D メッシュを作成します。
		/// @param sizeXZ X 軸方向および Z 軸方向の大きさ
		/// @param segmentsX X 軸方向の分割数
		/// @param segmentsZ Z 軸方向の分割数
		/// @param uvScale UV 座標の拡大率
		/// @param uvOffset UV 座標のオフセット
		/// @return 格子状の 3D メッシュ。大きさ、分割数、UV パラメータが不正な場合、または頂点数が上限を超える場合は空の 3D メッシュ
		/// @remark UV 座標の V 成分は Z 軸の正方向から負方向へ向かって増加します。
		[[nodiscard]]
		static Mesh3D Grid(
			SizeF sizeXZ,
			uint32 segmentsX,
			uint32 segmentsZ,
			Vec2 uvScale = Vec2{ 1.0, 1.0 },
			Vec2 uvOffset = Vec2{ 0.0, 0.0 });

		////////////////////////////////////////////////////////////////
		//
		//	HeightField
		//
		////////////////////////////////////////////////////////////////

		/// @brief 格子状の高さデータから地形の 3D メッシュを作成します。
		/// @param heights 各頂点の Y 座標を格納した高さデータ。幅と高さがそれぞれ 2 以上である必要があります。
		/// @param sizeXZ X 軸方向および Z 軸方向の大きさ
		/// @param uvScale UV 座標の拡大率
		/// @param uvOffset UV 座標のオフセット
		/// @return 高さデータから作成した 3D メッシュ。引数が不正な場合、または頂点数が上限を超える場合は空の 3D メッシュ
		/// @remark `heights[y][x]` を対応する頂点の Y 座標としてそのまま使用します。Y 方向の平行移動やスケーリングは行いません。
		/// @remark 列 0 を `X = -sizeXZ.x / 2`、最終列を `X = sizeXZ.x / 2`、行 0 を `Z = sizeXZ.y / 2`、最終行を `Z = -sizeXZ.y / 2` に配置します。
		/// @remark UV 座標は左上を `(0, 0)`、右下を `(1, 1)` とし、`uvScale` と `uvOffset` を適用します。
		/// @remark 隣接する高さの差分から、滑らかに接続する頂点法線と接線を計算します。
		[[nodiscard]]
		static Mesh3D HeightField(
			const s3d::Grid<float>& heights,
			SizeF sizeXZ,
			Vec2 uvScale = Vec2{ 1.0, 1.0 },
			Vec2 uvOffset = Vec2{ 0.0, 0.0 });

		////////////////////////////////////////////////////////////////
		//
		//	Loft
		//
		////////////////////////////////////////////////////////////////

		/// @brief 複数の断面を高さ方向に接続した 3D メッシュを作成します。
		/// @tparam SectionCount 断面数。2 以上である必要があります。
		/// @tparam VertexCount 各断面の頂点数。3 以上である必要があります。
		/// @param sections 各断面の頂点配列
		/// @param heights 各断面の Y 座標。厳密な昇順である必要があります。
		/// @param uvScale UV 座標の拡大率
		/// @param uvOffset UV 座標のオフセット
		/// @return 断面を接続した 3D メッシュ。引数が不正な場合、または頂点数が上限を超える場合は空の 3D メッシュ
		/// @remark 各断面の `Vec2` を `(X, -Z)` に対応させます。各断面は先頭頂点を末尾に重複させず、時計回りに指定します。
		/// @remark `sections[i][j]` と `sections[i + 1][j]` を対応する頂点として接続します。始端と終端は閉じます。
		/// @remark 側面の U 座標は最初の断面の周長に沿って `[0, 1]`、V 座標は `heights[0]` からの実距離です。
		/// @remark 輪郭の各頂点はハードエッジとし、断面間では法線と接線を滑らかに接続します。
		/// @remark 中間断面の自己交差、および異なる断面間での側面の自己交差は検査しません。
		/// @remark `uvScale.y` を単位高さあたりの反復数として使用すると、Repeat sampler で高さ方向に一定密度のタイリングができます。
		template <size_t SectionCount, size_t VertexCount>
			requires ((2 <= SectionCount) && (3 <= VertexCount))
		[[nodiscard]]
		static Mesh3D Loft(
			const std::array<std::array<Vec2, VertexCount>, SectionCount>& sections,
			const std::array<double, SectionCount>& heights,
			Vec2 uvScale = Vec2{ 1.0, 1.0 },
			Vec2 uvOffset = Vec2{ 0.0, 0.0 });

		////////////////////////////////////////////////////////////////
		//
		//	Torus
		//
		////////////////////////////////////////////////////////////////

		/// @brief Y 軸を回転軸とするトーラスの 3D メッシュを作成します。
		/// @param majorRadius 原点からチューブ断面の中心までの半径
		/// @param tubeRadius チューブ断面の半径。`majorRadius` より小さい必要があります。
		/// @param ringSegments リング方向の分割数。3 以上である必要があります。
		/// @param tubeSegments チューブ断面方向の分割数。3 以上である必要があります。
		/// @return トーラスの 3D メッシュ。引数が不正な場合、または頂点数が上限を超える場合は空の 3D メッシュ
		/// @remark UV 座標および接線空間の不連続を表現するため、リング方向とチューブ断面方向の継ぎ目に頂点が複製されます。
		[[nodiscard]]
		static Mesh3D Torus(
			double majorRadius,
			double tubeRadius,
			uint32 ringSegments = 32,
			uint32 tubeSegments = 16);

		////////////////////////////////////////////////////////////////
		//
		//	Capsule
		//
		////////////////////////////////////////////////////////////////

		/// @brief Y 軸方向に伸びるカプセルの 3D メッシュを作成します。
		/// @param radius カプセルの半径
		/// @param cylinderHeight 2 つの半球の間にある円柱部分の高さ。0 以上である必要があります。
		/// @param slices 円周方向の分割数。3 以上である必要があります。
		/// @param hemisphereStacks 各半球の緯度方向の分割数。1 以上である必要があります。
		/// @return カプセルの 3D メッシュ。引数が不正な場合、または頂点数が上限を超える場合は空の 3D メッシュ
		/// @remark カプセルは原点を中心とし、全体の高さは `cylinderHeight + 2 * radius` です。
		/// @remark `cylinderHeight` が 0 の場合は UV 球を作成します。
		/// @remark U 座標は +X 方向を 0 とし、東向き（+Z 方向）へ増加します。
		/// @remark UV 座標および接線空間の不連続を表現するため、経度方向の継ぎ目と極に頂点が複製されます。
		[[nodiscard]]
		static Mesh3D Capsule(
			double radius,
			double cylinderHeight,
			uint32 slices = 32,
			uint32 hemisphereStacks = 8);

		////////////////////////////////////////////////////////////////
		//
		//	UVSphere
		//
		////////////////////////////////////////////////////////////////

		/// @brief 原点を中心とする UV 球の 3D メッシュを作成します。
		/// @param radius 球の半径
		/// @param slices 経度方向の分割数。3 以上である必要があります。
		/// @param stacks 緯度方向の分割数。2 以上である必要があります。
		/// @return UV 球の 3D メッシュ。引数が不正な場合、または頂点数が上限を超える場合は空の 3D メッシュ
		/// @remark 球の極は Y 軸上に配置されます。
		/// @remark UV 座標は、北極を V = 0、南極を V = 1 とし、U は +X 方向を 0 として東向き（+Z 方向）へ増加します。
		/// @remark UV 座標および接線空間の不連続を表現するため、同じ位置に複数の頂点が作成されます。
		[[nodiscard]]
		static Mesh3D UVSphere(double radius, uint32 slices = 32, uint32 stacks = 16);

		////////////////////////////////////////////////////////////////
		//
		//	Hemisphere
		//
		////////////////////////////////////////////////////////////////

		/// @brief Y 軸の正方向を向く、底面のない半球の 3D メッシュを作成します。
		/// @param radius 半球の半径
		/// @param slices 経度方向の分割数。3 以上である必要があります。
		/// @param stacks 緯度方向の分割数。1 以上である必要があります。
		/// @return 半球の 3D メッシュ。引数が不正な場合、または頂点数が上限を超える場合は空の 3D メッシュ
		/// @remark 半球の中心は原点、赤道は XZ 平面上、極は `(0, radius, 0)` に配置されます。
		/// @remark 曲面の UV 座標は、極から赤道までを V 座標の 0 から 1 に割り当てます。
		/// @remark 曲面の U 座標は +X 方向を 0 とし、東向き（+Z 方向）へ増加します。
		[[nodiscard]]
		static Mesh3D Hemisphere(double radius, uint32 slices = 32, uint32 stacks = 8);

		/// @brief Y 軸の正方向を向く半球の 3D メッシュを作成します。
		/// @param radius 半球の半径
		/// @param closeBottom 底面を閉じる場合は `CloseBottom::Yes`、底面を作成しない場合は `CloseBottom::No`
		/// @param slices 経度方向の分割数。3 以上である必要があります。
		/// @param stacks 緯度方向の分割数。1 以上である必要があります。
		/// @return 半球の 3D メッシュ。引数が不正な場合、または頂点数が上限を超える場合は空の 3D メッシュ
		/// @remark 半球の中心は原点、赤道は XZ 平面上、極は `(0, radius, 0)` に配置されます。
		/// @remark 曲面の UV 座標は、極から赤道までを V 座標の 0 から 1 に割り当てます。
		/// @remark 曲面の U 座標は +X 方向を 0 とし、東向き（+Z 方向）へ増加します。
		/// @remark 底面を閉じる場合、底面には曲面とは独立した頂点を作成し、円全体を UV 座標の 0 から 1 に割り当てます。
		[[nodiscard]]
		static Mesh3D Hemisphere(double radius, CloseBottom closeBottom, uint32 slices = 32, uint32 stacks = 8);

		////////////////////////////////////////////////////////////////
		//
		//	Disc
		//
		////////////////////////////////////////////////////////////////

		/// @brief XZ 平面上に、法線が Y 軸の正方向を向く円盤の 3D メッシュを作成します。
		/// @param radius 円盤の半径
		/// @param segments 円周の分割数。3 以上である必要があります。
		/// @return 円盤の 3D メッシュ。引数が不正な場合、または頂点数が上限を超える場合は空の 3D メッシュ
		[[nodiscard]]
		static Mesh3D Disc(double radius, uint32 segments = 32);

		////////////////////////////////////////////////////////////////
		//
		//	Annulus
		//
		////////////////////////////////////////////////////////////////

		/// @brief XZ 平面上に、法線が Y 軸の正方向を向く円環の 3D メッシュを作成します。
		/// @param innerRadius 円環の内半径。0 以上である必要があります。
		/// @param outerRadius 円環の外半径。`innerRadius` より大きい必要があります。
		/// @param segments 円周の分割数。3 以上である必要があります。
		/// @return 円環の 3D メッシュ。引数が不正な場合、または頂点数が上限を超える場合は空の 3D メッシュ
		/// @remark `innerRadius` が 0 の場合は、中心頂点を共有する円盤を作成します。
		[[nodiscard]]
		static Mesh3D Annulus(double innerRadius, double outerRadius, uint32 segments = 32);

		////////////////////////////////////////////////////////////////
		//
		//	HollowCylinder
		//
		////////////////////////////////////////////////////////////////

		/// @brief Y 軸方向に伸びる、厚みと円環状の端面を持つ中空円柱の 3D メッシュを作成します。
		/// @param innerRadius 内半径。正の有限値である必要があります。
		/// @param outerRadius 外半径。`innerRadius` より大きい必要があります。
		/// @param height 高さ
		/// @param segments 円周の分割数。3 以上である必要があります。
		/// @return 中空円柱の 3D メッシュ。引数が不正な場合、または頂点数が上限を超える場合は空の 3D メッシュ
		/// @remark 中央の穴は上下に貫通し、肉厚は `outerRadius - innerRadius` です。
		/// @remark 外側面の U 座標は +X 方向を 0 として +Z 方向へ増加し、内側面の U 座標は +X 方向を 0 として -Z 方向へ増加します。これにより、テクスチャは外側と内側のそれぞれを表側から見たときに同じ向きで表示されます。
		/// @remark 側面の V 座標は上端を 0、下端を 1 とします。上下の端面は、外径全体を UV 座標の 0 から 1 に割り当てます。
		[[nodiscard]]
		static Mesh3D HollowCylinder(
			double innerRadius,
			double outerRadius,
			double height,
			uint32 segments = 32);

		////////////////////////////////////////////////////////////////
		//
		//	ConicalFrustum
		//
		////////////////////////////////////////////////////////////////

		/// @brief Y 軸方向に伸びる、両端が閉じた円錐台の 3D メッシュを作成します。
		/// @param bottomRadius 底面の半径。正の有限値である必要があります。
		/// @param topRadius 上面の半径。0 以上の有限値である必要があります。0 の場合は円錐を作成します。
		/// @param height 高さ
		/// @param segments 円周の分割数。3 以上である必要があります。
		/// @return 円錐台の 3D メッシュ。引数が不正な場合、または頂点数が上限を超える場合は空の 3D メッシュ
		/// @remark 底面の中心は `(0, -height / 2, 0)`、上面の中心は `(0, height / 2, 0)` に配置されます。
		/// @remark `topRadius` が 0 の場合、側面の U 座標は円周方向を 0 から 1、V 座標は頂点を 0、底面側を 1 とする矩形領域に割り当てられます。
		[[nodiscard]]
		static Mesh3D ConicalFrustum(double bottomRadius, double topRadius, double height, uint32 segments = 32);

		////////////////////////////////////////////////////////////////
		//
		//	Cylinder
		//
		////////////////////////////////////////////////////////////////

		/// @brief Y 軸方向に伸びる、両端が閉じた円柱の 3D メッシュを作成します。
		/// @param radius 円柱の半径
		/// @param height 円柱の高さ
		/// @param segments 円周の分割数。3 以上である必要があります。
		/// @return 円柱の 3D メッシュ。引数が不正な場合、または頂点数が上限を超える場合は空の 3D メッシュ
		[[nodiscard]]
		static Mesh3D Cylinder(double radius, double height, uint32 segments = 32);

		////////////////////////////////////////////////////////////////
		//
		//	Cone
		//
		////////////////////////////////////////////////////////////////

		/// @brief Y 軸方向に伸びる、底面が閉じた円錐の 3D メッシュを作成します。
		/// @param radius 円錐の底面の半径
		/// @param height 円錐の高さ
		/// @param segments 円周の分割数。3 以上である必要があります。
		/// @return 円錐の 3D メッシュ。引数が不正な場合、または頂点数が上限を超える場合は空の 3D メッシュ
		/// @remark 側面の U 座標は円周方向を 0 から 1、V 座標は頂点を 0、底面側を 1 とする矩形領域に割り当てられます。
		[[nodiscard]]
		static Mesh3D Cone(double radius, double height, uint32 segments = 32);

		////////////////////////////////////////////////////////////////
		//
		//	isEmpty
		//
		////////////////////////////////////////////////////////////////

		/// @brief 描画可能な三角形を持たないかを返します。
		/// @return 頂点または三角形が空の場合 true, それ以外の場合は false
		[[nodiscard]]
		bool isEmpty() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	validate
		//
		////////////////////////////////////////////////////////////////

		/// @brief 3D メッシュの頂点数とインデックスの範囲を検証します。
		/// @return 頂点数が `MaxVertexCount` 以下で、すべてのインデックスが頂点配列の範囲内にある場合 true, それ以外の場合は false
		/// @remark 三角形の面積、頂点の並び順、頂点座標や UV 座標の値など、幾何学的な妥当性は検証しません。
		/// @remark 計算量は三角形の個数を n として O(n) です。
		[[nodiscard]]
		bool validate() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	vertexCount
		//
		////////////////////////////////////////////////////////////////

		/// @brief 頂点の個数を返します。
		/// @return 頂点の個数
		[[nodiscard]]
		size_t vertexCount() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	triangleCount
		//
		////////////////////////////////////////////////////////////////

		/// @brief 三角形の個数を返します。
		/// @return 三角形の個数
		[[nodiscard]]
		size_t triangleCount() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	saveOBJ
		//
		////////////////////////////////////////////////////////////////

		/// @brief 3D メッシュを Wavefront OBJ 形式でファイルに保存します。
		/// @param path 保存するファイルのパス
		/// @return 保存に成功した場合 true, それ以外の場合は false
		/// @remark Siv3D の左手座標系から OBJ の右手座標系へ変換するため、頂点座標と法線の Z 成分は符号を反転し、三角形の巻き順は反転して保存されます。
		/// @remark Siv3D の上端を V = 0 とする UV 座標から OBJ の下端を V = 0 とする UV 座標へ変換するため、V 成分は `1 - V` として保存されます。U 成分は変更されません。
		/// @remark 各 `Vertex3D` には、位置、UV、法線で共通の 1 始まりの OBJ インデックスが割り当てられます。
		/// @remark 接線、材質、オブジェクト名、およびグループは保存されません。
		/// @remark UTF-8（BOM なし）、LF 改行で保存されます。
		/// @remark 空のメッシュ、インデックスが不正なメッシュ、または出力対象の頂点属性に非有限値が含まれるメッシュは保存できません。
		bool saveOBJ(FilePathView path) const;

		/// @brief 3D メッシュとマテリアルを Wavefront OBJ / MTL 形式でファイルに保存します。
		/// @param path 保存する OBJ ファイルのパス
		/// @param material 保存するマテリアル
		/// @return OBJ と MTL の保存に成功した場合 true, それ以外の場合は false
		/// @remark MTL ファイルは OBJ ファイルと同じディレクトリに、同じベース名と `.mtl` 拡張子で保存されます。
		/// @remark OBJ ファイルには `mtllib` と `usemtl` が書き込まれます。
		/// @remark Siv3D の左手座標系から OBJ の右手座標系へ変換するため、頂点座標と法線の Z 成分は符号を反転し、三角形の巻き順は反転して保存されます。
		/// @remark Siv3D の上端を V = 0 とする UV 座標から OBJ の下端を V = 0 とする UV 座標へ変換するため、V 成分は `1 - V` として保存されます。U 成分は変更されません。
		/// @remark ベースカラー、金属度、粗さ、エミッシブカラー、アルファ値、および対応するテクスチャは、MTL で表現可能な値へ変換されます。
		/// @remark 金属度と粗さは、従来の MTL パラメータへの変換に加えて `Pm` と `Pr` でも保存されます。
		/// @remark MTL で直接表現できないアルファマスクのしきい値、両面描画、metallic-roughness テクスチャ、およびアンビエントオクルージョンは保存されません。
		/// @remark テクスチャファイル自体はコピーされません。
		/// @remark 空のマテリアル名、制御文字を含む名前やテクスチャパス、非有限値、または UV セット 0 以外を参照するマテリアルは保存できません。
		bool saveOBJ(FilePathView path, const Material& material) const;

		////////////////////////////////////////////////////////////////
		//
		//	encodeOBJ
		//
		////////////////////////////////////////////////////////////////

		/// @brief 3D メッシュを Wavefront OBJ 形式で Writer に書き出します。
		/// @param writer 書き出し先の Writer
		/// @return 書き出しに成功した場合 true, それ以外の場合は false
		/// @remark Siv3D の左手座標系から OBJ の右手座標系へ変換するため、頂点座標と法線の Z 成分は符号を反転し、三角形の巻き順は反転して書き出されます。
		/// @remark Siv3D の上端を V = 0 とする UV 座標から OBJ の下端を V = 0 とする UV 座標へ変換するため、V 成分は `1 - V` として書き出されます。U 成分は変更されません。
		/// @remark 各 `Vertex3D` には、位置、UV、法線で共通の 1 始まりの OBJ インデックスが割り当てられます。
		/// @remark 接線、材質、オブジェクト名、およびグループは書き出されません。
		/// @remark UTF-8（BOM なし）、LF 改行で書き出されます。
		/// @remark 空のメッシュ、インデックスが不正なメッシュ、または出力対象の頂点属性に非有限値が含まれるメッシュは書き出せません。
		/// @remark 書き出し中に失敗した場合、Writer に途中までのデータが残ることがあります。
		bool encodeOBJ(IWriter& writer) const;

		/// @brief 3D メッシュを Wavefront OBJ 形式でエンコードします。
		/// @return エンコードされたデータ。エンコードに失敗した場合は空の Blob
		/// @remark Siv3D の左手座標系から OBJ の右手座標系へ変換するため、頂点座標と法線の Z 成分は符号を反転し、三角形の巻き順は反転してエンコードされます。
		/// @remark Siv3D の上端を V = 0 とする UV 座標から OBJ の下端を V = 0 とする UV 座標へ変換するため、V 成分は `1 - V` としてエンコードされます。U 成分は変更されません。
		/// @remark 各 `Vertex3D` には、位置、UV、法線で共通の 1 始まりの OBJ インデックスが割り当てられます。
		/// @remark 接線、材質、オブジェクト名、およびグループはエンコードされません。
		/// @remark UTF-8（BOM なし）、LF 改行でエンコードされます。
		/// @remark 空のメッシュ、インデックスが不正なメッシュ、または出力対象の頂点属性に非有限値が含まれるメッシュはエンコードできません。
		[[nodiscard]]
		Blob encodeOBJ() const;

		////////////////////////////////////////////////////////////////
		//
		//	append
		//
		////////////////////////////////////////////////////////////////

		/// @brief 別の 3D メッシュの頂点と三角形を末尾に追加します。
		/// @param mesh 追加する 3D メッシュ
		/// @return 追加に成功した場合 true, 追加できない場合は false
		/// @remark 追加される三角形のインデックスは、追加先の既存の頂点数に合わせて自動的に調整されます。
		/// @remark `mesh` が不正な 3D メッシュである場合、または追加後の頂点数が `MaxVertexCount` を超える場合は追加に失敗します。
		/// @remark この関数が false を返した場合、この 3D メッシュの内容は変更されません。
		[[nodiscard]]
		bool append(const Mesh3D& mesh);

		/// @brief アフィン変換を適用した別の 3D メッシュの頂点と三角形を末尾に追加します。
		/// @param mesh 追加する Mesh3D
		/// @param matrix 追加する頂点に適用するアフィン変換行列
		/// @return 追加に成功した場合 true, 追加できない場合は false
		/// @remark `matrix` はアフィン変換行列である必要があります。
		/// @remark `append(mesh.transformed(matrix))` と同じ結果を、一時 Mesh3D を作成せずに生成します。
		/// @remark `mesh` が不正な Mesh3D である場合、または追加後の頂点数が `MaxVertexCount` を超える場合は追加に失敗します。
		/// @remark この関数が false を返した場合、この Mesh3D の内容は変更されません。
		/// @remark 変換の線形部分が特異である場合、追加する頂点の法線および接線は変更されません。
		/// @remark 変換の線形部分の行列式が負の場合、追加する頂点の接線の `w` 成分が反転しますが、三角形の巻き順は変更されません。
		[[nodiscard]]
		bool append(const Mesh3D& mesh, const Mat4x4& matrix);

		////////////////////////////////////////////////////////////////
		//
		//	computeNormals
		//
		////////////////////////////////////////////////////////////////

		/// @brief 頂点法線を計算します。
		/// @param weighting 各三角形の法線を合成するときの重み付け方式
		/// @remark 同じ頂点インデックスを共有する三角形の法線が合成されます。
		/// @remark ハードエッジを保持するには、その境界で頂点が分割されている必要があります。
		/// @return *this
		/// @throw Error 法線の計算に失敗した場合
		Mesh3D& computeNormals(VertexNormalWeighting weighting = VertexNormalWeighting::Angle);

		////////////////////////////////////////////////////////////////
		//
		//	computeTangents
		//
		////////////////////////////////////////////////////////////////

		/// @brief MikkTSpace を使用して頂点の接線を計算します。
		/// @remark 頂点法線および UV 座標が設定済みである必要があります。
		/// @remark 接線空間が不連続になる箇所では頂点が複製されるため、頂点数およびインデックス配列が変更されることがあります。
		/// @return *this
		/// @throw Error 接線の計算に失敗した場合
		Mesh3D& computeTangents();

		////////////////////////////////////////////////////////////////
		//
		//	reverseWinding
		//
		////////////////////////////////////////////////////////////////

		/// @brief すべての三角形の頂点の巻き順を反転させます。
		/// @return *this
		/// @remark 各三角形のインデックスが `{ i0, i1, i2 }` から `{ i0, i2, i1 }` に変換されます。
		/// @remark 頂点の法線と接線は変更されません。表裏をまとめて反転させるには `invert()` を使用します。
		Mesh3D& reverseWinding() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	invertNormals
		//
		////////////////////////////////////////////////////////////////

		/// @brief すべての頂点の法線を反転させます。
		/// @return *this
		/// @remark 各頂点の法線ベクトルの符号が反転し、あわせて接線の `w` 成分（従接線の向きを表す符号）も反転します。
		/// @remark これにより従接線 `Vertex3D::bitangent()` は反転前と同じ向きに保たれ、法線マッピングとの整合性が維持されます。
		/// @remark 三角形の巻き順は変更されません。表裏をまとめて反転させるには `invert()` を使用します。
		Mesh3D& invertNormals() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	invert
		//
		////////////////////////////////////////////////////////////////

		/// @brief 3D メッシュの表裏を反転させます。
		/// @return *this
		/// @remark `reverseWinding()` と `invertNormals()` を同時に適用するのと同じ結果になります。
		/// @remark 三角形の巻き順、頂点の法線、および接線の `w` 成分が反転します。
		/// @remark 頂点座標、UV 座標、接線ベクトルの `xyz` 成分は変更されません。
		Mesh3D& invert() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	transformed
		//
		////////////////////////////////////////////////////////////////

		/// @brief アフィン変換を適用した 3D メッシュを返します。
		/// @param matrix 適用するアフィン変換行列
		/// @return アフィン変換を適用した 3D メッシュ
		/// @remark `matrix` はアフィン変換行列である必要があります。
		/// @remark 変換の線形部分が非特異である場合、法線および接線ベクトルは変換後に正規化されます。
		/// @remark 変換の線形部分が特異である場合、法線および接線は変更されません。
		/// @remark UV 座標および三角形インデックスは変更されません。
		/// @remark 変換の線形部分の行列式が負の場合、接線の `w` 成分が反転しますが、三角形の巻き順は変更されません。必要に応じて `reverseWinding()` を使用してください。
		[[nodiscard]]
		Mesh3D transformed(const Mat4x4& matrix) const&;

		/// @brief アフィン変換を適用した 3D メッシュを返します。
		/// @param matrix 適用するアフィン変換行列
		/// @return アフィン変換を適用した 3D メッシュ
		/// @remark このオーバーロードは自身のストレージを再利用します。
		/// @remark `matrix` はアフィン変換行列である必要があります。
		/// @remark 変換の線形部分が非特異である場合、法線および接線ベクトルは変換後に正規化されます。
		/// @remark 変換の線形部分が特異である場合、法線および接線は変更されません。
		/// @remark UV 座標および三角形インデックスは変更されません。
		/// @remark 変換の線形部分の行列式が負の場合、接線の `w` 成分が反転しますが、三角形の巻き順は変更されません。必要に応じて `reverseWinding()` を使用してください。
		[[nodiscard]]
		Mesh3D transformed(const Mat4x4& matrix) && noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	transform
		//
		////////////////////////////////////////////////////////////////

		/// @brief 3D メッシュにアフィン変換を適用します。
		/// @param matrix 適用するアフィン変換行列
		/// @return *this
		/// @remark `matrix` はアフィン変換行列である必要があります。
		/// @remark 変換の線形部分が非特異である場合、法線および接線ベクトルは変換後に正規化されます。
		/// @remark 変換の線形部分が特異である場合、法線および接線は変更されません。
		/// @remark UV 座標および三角形インデックスは変更されません。
		/// @remark 変換の線形部分の行列式が負の場合、接線の `w` 成分が反転しますが、三角形の巻き順は変更されません。必要に応じて `reverseWinding()` を使用してください。
		Mesh3D& transform(const Mat4x4& matrix) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	translated
		//
		////////////////////////////////////////////////////////////////

		/// @brief 平行移動した 3D メッシュを返します。
		/// @param offset 平行移動量
		/// @return 平行移動した 3D メッシュ
		/// @remark 頂点座標のみが変更されます。法線、接線、UV 座標、および三角形インデックスは変更されません。
		[[nodiscard]]
		Mesh3D translated(Float3 offset) const&;

		/// @brief 平行移動した 3D メッシュを返します。
		/// @param offset 平行移動量
		/// @return 平行移動した 3D メッシュ
		/// @remark このオーバーロードは自身のストレージを再利用します。
		/// @remark 頂点座標のみが変更されます。法線、接線、UV 座標、および三角形インデックスは変更されません。
		[[nodiscard]]
		Mesh3D translated(Float3 offset) && noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	translate
		//
		////////////////////////////////////////////////////////////////

		/// @brief 3D メッシュを平行移動します。
		/// @param offset 平行移動量
		/// @return *this
		/// @remark 頂点座標のみが変更されます。法線、接線、UV 座標、および三角形インデックスは変更されません。
		Mesh3D& translate(Float3 offset) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	rotated
		//
		////////////////////////////////////////////////////////////////

		/// @brief 原点を中心に回転させた 3D メッシュを返します。
		/// @param rotation 適用する回転を表す単位クォータニオン
		/// @return 回転させた 3D メッシュ
		/// @remark `rotation` は正規化されている必要があります。
		/// @remark 頂点座標、法線、および接線ベクトルが回転します。接線の `w` 成分、UV 座標、および三角形インデックスは変更されません。
		[[nodiscard]]
		Mesh3D rotated(const Quaternion& rotation) const&;

		/// @brief 原点を中心に回転させた 3D メッシュを返します。
		/// @param rotation 適用する回転を表す単位クォータニオン
		/// @return 回転させた 3D メッシュ
		/// @remark このオーバーロードは自身のストレージを再利用します。
		/// @remark `rotation` は正規化されている必要があります。
		/// @remark 頂点座標、法線、および接線ベクトルが回転します。接線の `w` 成分、UV 座標、および三角形インデックスは変更されません。
		[[nodiscard]]
		Mesh3D rotated(const Quaternion& rotation) && noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	rotate
		//
		////////////////////////////////////////////////////////////////

		/// @brief 3D メッシュを原点を中心に回転させます。
		/// @param rotation 適用する回転を表す単位クォータニオン
		/// @return *this
		/// @remark `rotation` は正規化されている必要があります。
		/// @remark 頂点座標、法線、および接線ベクトルが回転します。接線の `w` 成分、UV 座標、および三角形インデックスは変更されません。
		Mesh3D& rotate(const Quaternion& rotation) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	scaled
		//
		////////////////////////////////////////////////////////////////

		/// @brief 原点を中心に一様に拡大・縮小した 3D メッシュを返します。
		/// @param scale 拡大率
		/// @return 拡大・縮小した 3D メッシュ
		/// @remark `scale` が正の場合、法線および接線は変更されません。
		/// @remark `scale` が負の場合、法線、接線ベクトル、および接線の `w` 成分が反転します。
		/// @remark `scale` が 0 の場合、法線および接線は変更されません。
		/// @remark UV 座標および三角形インデックスは変更されません。
		/// @remark `scale` が負の場合でも三角形の巻き順は変更されません。必要に応じて `reverseWinding()` を使用してください。
		[[nodiscard]]
		Mesh3D scaled(float scale) const&;

		/// @brief 原点を中心に一様に拡大・縮小した 3D メッシュを返します。
		/// @param scale 拡大率
		/// @return 拡大・縮小した 3D メッシュ
		/// @remark このオーバーロードは自身のストレージを再利用します。
		/// @remark `scale` が正の場合、法線および接線は変更されません。
		/// @remark `scale` が負の場合、法線、接線ベクトル、および接線の `w` 成分が反転します。
		/// @remark `scale` が 0 の場合、法線および接線は変更されません。
		/// @remark UV 座標および三角形インデックスは変更されません。
		/// @remark `scale` が負の場合でも三角形の巻き順は変更されません。必要に応じて `reverseWinding()` を使用してください。
		[[nodiscard]]
		Mesh3D scaled(float scale) && noexcept;

		/// @brief 原点を中心に各軸方向へ拡大・縮小した 3D メッシュを返します。
		/// @param scale 各軸方向の拡大率
		/// @return 拡大・縮小した 3D メッシュ
		/// @remark `scale` のすべての成分が非ゼロの場合、法線および接線ベクトルは変換後に正規化されます。
		/// @remark `scale` のいずれかの成分が 0 の場合、法線および接線は変更されません。
		/// @remark UV 座標および三角形インデックスは変更されません。
		/// @remark 拡大率の積が負の場合、接線の `w` 成分が反転しますが、三角形の巻き順は変更されません。必要に応じて `reverseWinding()` を使用してください。
		[[nodiscard]]
		Mesh3D scaled(Float3 scale) const&;

		/// @brief 原点を中心に各軸方向へ拡大・縮小した 3D メッシュを返します。
		/// @param scale 各軸方向の拡大率
		/// @return 拡大・縮小した 3D メッシュ
		/// @remark このオーバーロードは自身のストレージを再利用します。
		/// @remark `scale` のすべての成分が非ゼロの場合、法線および接線ベクトルは変換後に正規化されます。
		/// @remark `scale` のいずれかの成分が 0 の場合、法線および接線は変更されません。
		/// @remark UV 座標および三角形インデックスは変更されません。
		/// @remark 拡大率の積が負の場合、接線の `w` 成分が反転しますが、三角形の巻き順は変更されません。必要に応じて `reverseWinding()` を使用してください。
		[[nodiscard]]
		Mesh3D scaled(Float3 scale) && noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	scale
		//
		////////////////////////////////////////////////////////////////

		/// @brief 3D メッシュを原点を中心に一様に拡大・縮小します。
		/// @param scale 拡大率
		/// @return *this
		/// @remark `scale` が正の場合、法線および接線は変更されません。
		/// @remark `scale` が負の場合、法線、接線ベクトル、および接線の `w` 成分が反転します。
		/// @remark `scale` が 0 の場合、法線および接線は変更されません。
		/// @remark UV 座標および三角形インデックスは変更されません。
		/// @remark `scale` が負の場合でも三角形の巻き順は変更されません。必要に応じて `reverseWinding()` を使用してください。
		Mesh3D& scale(float scale) noexcept;

		/// @brief 3D メッシュを原点を中心に各軸方向へ拡大・縮小します。
		/// @param scale 各軸方向の拡大率
		/// @return *this
		/// @remark `scale` のすべての成分が非ゼロの場合、法線および接線ベクトルは変換後に正規化されます。
		/// @remark `scale` のいずれかの成分が 0 の場合、法線および接線は変更されません。
		/// @remark UV 座標および三角形インデックスは変更されません。
		/// @remark 拡大率の積が負の場合、接線の `w` 成分が反転しますが、三角形の巻き順は変更されません。必要に応じて `reverseWinding()` を使用してください。
		Mesh3D& scale(Float3 scale) noexcept;

	private:

		[[nodiscard]]
		static Mesh3D LoftImpl(
			std::span<const std::span<const Vec2>> sections,
			std::span<const double> heights,
			Vec2 uvScale,
			Vec2 uvOffset);

	};
}

# include "detail/Mesh3D.ipp"
