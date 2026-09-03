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
# include "Mesh3D.hpp"
# include "Mat4x4.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	Mesh3DBuilder
	//
	////////////////////////////////////////////////////////////////

	/// @brief 複数の 3D 形状を 1 つの Mesh3D に直接生成するビルダー
	/// @remark 各 add 関数は一時的な Mesh3D を作成せず、ビルダーが所有するメッシュへ頂点と三角形を追加します。
	/// @remark add 関数が失敗した場合、Fail レベルのエンジンログへ理由を出力し、既存のメッシュ内容は変更されません。
	class Mesh3DBuilder
	{
	public:

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		/// @brief 空の Mesh3DBuilder を作成します。
		[[nodiscard]]
		Mesh3DBuilder() = default;

		////////////////////////////////////////////////////////////////
		//
		//	reserve
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した要素数まで再確保なしで格納できるよう、頂点配列と三角形インデックス配列の容量を確保します。
		/// @param vertexCapacity 確保する頂点容量
		/// @param triangleCapacity 確保する三角形容量
		/// @remark `vertexCapacity` が `Mesh3D::MaxVertexCount` を超える場合、容量は変更されません。
		void reserve(size_t vertexCapacity, size_t triangleCapacity);

		////////////////////////////////////////////////////////////////
		//
		//	clear
		//
		////////////////////////////////////////////////////////////////

		/// @brief 生成済みのすべての頂点と三角形を削除します。
		/// @remark 頂点配列と三角形インデックス配列の容量は変更されません。
		void clear() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	getMesh
		//
		////////////////////////////////////////////////////////////////

		/// @brief 現在の 3D メッシュを参照します。
		/// @return 現在の 3D メッシュへの参照
		[[nodiscard]]
		const Mesh3D& getMesh() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	build
		//
		////////////////////////////////////////////////////////////////

		/// @brief 構築した 3D メッシュを取得します。
		/// @return 構築した 3D メッシュ
		/// @remark ビルダーが所有するストレージを再利用します。この関数を呼んだ後のビルダーの状態は規定されません。
		[[nodiscard]]
		Mesh3D build() && noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	addBox
		//
		////////////////////////////////////////////////////////////////

		/// @brief 原点を中心とする直方体を追加します。
		/// @param size 直方体の各軸方向の大きさ
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addBox(Vec3 size = Vec3{ 1.0, 1.0, 1.0 });

		/// @brief 平行移動した直方体を追加します。
		/// @param size 直方体の各軸方向の大きさ
		/// @param offset 平行移動量
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addBox(Vec3 size, Vec3 offset);

		/// @brief 回転および平行移動した直方体を追加します。
		/// @param size 直方体の各軸方向の大きさ
		/// @param offset 平行移動量
		/// @param rotation 原点を中心とする回転を表す単位クォータニオン
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addBox(Vec3 size, Vec3 offset, const Quaternion& rotation);

		/// @brief アフィン変換を適用した直方体を追加します。
		/// @param size 直方体の各軸方向の大きさ
		/// @param transform 適用するアフィン変換行列
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addBox(Vec3 size, const Mat4x4& transform);

		/// @brief 指定した UV マッピングを持つ、原点を中心とする直方体を追加します。
		/// @param size 直方体の各軸方向の大きさ
		/// @param uvMapping 各面に割り当てる UV 矩形
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addBox(Vec3 size, const BoxUVMapping& uvMapping);

		/// @brief 指定した UV マッピングを持つ、平行移動した直方体を追加します。
		/// @param size 直方体の各軸方向の大きさ
		/// @param uvMapping 各面に割り当てる UV 矩形
		/// @param offset 平行移動量
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addBox(Vec3 size, const BoxUVMapping& uvMapping, Vec3 offset);

		/// @brief 指定した UV マッピングを持つ、回転および平行移動した直方体を追加します。
		/// @param size 直方体の各軸方向の大きさ
		/// @param uvMapping 各面に割り当てる UV 矩形
		/// @param offset 平行移動量
		/// @param rotation 原点を中心とする回転を表す単位クォータニオン
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addBox(Vec3 size, const BoxUVMapping& uvMapping, Vec3 offset, const Quaternion& rotation);

		/// @brief 指定した UV マッピングを持つ、アフィン変換を適用した直方体を追加します。
		/// @param size 直方体の各軸方向の大きさ
		/// @param uvMapping 各面に割り当てる UV 矩形
		/// @param transform 適用するアフィン変換行列
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addBox(Vec3 size, const BoxUVMapping& uvMapping, const Mat4x4& transform);

		////////////////////////////////////////////////////////////////
		//
		//	addRoundedBox
		//
		////////////////////////////////////////////////////////////////

		/// @brief 原点を中心とする角丸直方体を追加します。
		/// @param size 角丸直方体の各軸方向の大きさ
		/// @param radius 角の丸みの半径。0 以上、`size` の最小成分の半分以下である必要があります。
		/// @param subdivisions 各面の丸み部分の分割数。1 以上である必要があります。
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addRoundedBox(
			Vec3 size = Vec3{ 1.0, 1.0, 1.0 },
			double radius = 0.1,
			uint32 subdivisions = 4);

		/// @brief 平行移動した角丸直方体を追加します。
		/// @param size 角丸直方体の各軸方向の大きさ
		/// @param radius 角の丸みの半径
		/// @param subdivisions 各面の丸み部分の分割数
		/// @param offset 平行移動量
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addRoundedBox(Vec3 size, double radius, uint32 subdivisions, Vec3 offset);

		/// @brief 回転および平行移動した角丸直方体を追加します。
		/// @param size 角丸直方体の各軸方向の大きさ
		/// @param radius 角の丸みの半径
		/// @param subdivisions 各面の丸み部分の分割数
		/// @param offset 平行移動量
		/// @param rotation 原点を中心とする回転を表す単位クォータニオン
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addRoundedBox(Vec3 size, double radius, uint32 subdivisions, Vec3 offset, const Quaternion& rotation);

		/// @brief アフィン変換を適用した角丸直方体を追加します。
		/// @param size 角丸直方体の各軸方向の大きさ
		/// @param radius 角の丸みの半径
		/// @param subdivisions 各面の丸み部分の分割数
		/// @param transform 適用するアフィン変換行列
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addRoundedBox(Vec3 size, double radius, uint32 subdivisions, const Mat4x4& transform);

		/// @brief 指定した UV マッピングを持つ、原点を中心とする角丸直方体を追加します。
		/// @param size 角丸直方体の各軸方向の大きさ
		/// @param radius 角の丸みの半径。0 以上、`size` の最小成分の半分以下である必要があります。
		/// @param subdivisions 各面の丸み部分の分割数。1 以上である必要があります。
		/// @param uvMapping 各面から形状全体のバウンディングボックスへ投影する UV 矩形
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addRoundedBox(Vec3 size, double radius, uint32 subdivisions, const BoxUVMapping& uvMapping);

		/// @brief 指定した UV マッピングを持つ、平行移動した角丸直方体を追加します。
		/// @param size 角丸直方体の各軸方向の大きさ
		/// @param radius 角の丸みの半径
		/// @param subdivisions 各面の丸み部分の分割数
		/// @param uvMapping 各面から形状全体のバウンディングボックスへ投影する UV 矩形
		/// @param offset 平行移動量
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addRoundedBox(Vec3 size, double radius, uint32 subdivisions, const BoxUVMapping& uvMapping, Vec3 offset);

		/// @brief 指定した UV マッピングを持つ、回転および平行移動した角丸直方体を追加します。
		/// @param size 角丸直方体の各軸方向の大きさ
		/// @param radius 角の丸みの半径
		/// @param subdivisions 各面の丸み部分の分割数
		/// @param uvMapping 各面から形状全体のバウンディングボックスへ投影する UV 矩形
		/// @param offset 平行移動量
		/// @param rotation 原点を中心とする回転を表す単位クォータニオン
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addRoundedBox(Vec3 size, double radius, uint32 subdivisions, const BoxUVMapping& uvMapping, Vec3 offset, const Quaternion& rotation);

		/// @brief 指定した UV マッピングを持つ、アフィン変換を適用した角丸直方体を追加します。
		/// @param size 角丸直方体の各軸方向の大きさ
		/// @param radius 角の丸みの半径
		/// @param subdivisions 各面の丸み部分の分割数
		/// @param uvMapping 各面から形状全体のバウンディングボックスへ投影する UV 矩形
		/// @param transform 適用するアフィン変換行列
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addRoundedBox(Vec3 size, double radius, uint32 subdivisions, const BoxUVMapping& uvMapping, const Mat4x4& transform);

		////////////////////////////////////////////////////////////////
		//
		//	addWedge
		//
		////////////////////////////////////////////////////////////////

		/// @brief 原点を中心とし、Z 軸の正方向へ上るくさび形を追加します。
		/// @param size くさび形の各軸方向の大きさ
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addWedge(Vec3 size = Vec3{ 1.0, 1.0, 1.0 });

		/// @brief 平行移動したくさび形を追加します。
		/// @param size くさび形の各軸方向の大きさ
		/// @param offset 平行移動量
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addWedge(Vec3 size, Vec3 offset);

		/// @brief 回転および平行移動したくさび形を追加します。
		/// @param size くさび形の各軸方向の大きさ
		/// @param offset 平行移動量
		/// @param rotation 原点を中心とする回転を表す単位クォータニオン
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addWedge(Vec3 size, Vec3 offset, const Quaternion& rotation);

		/// @brief アフィン変換を適用したくさび形を追加します。
		/// @param size くさび形の各軸方向の大きさ
		/// @param transform 適用するアフィン変換行列
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addWedge(Vec3 size, const Mat4x4& transform);

		/// @brief 指定した UV マッピングを持つ、原点を中心とするくさび形を追加します。
		/// @param size くさび形の各軸方向の大きさ
		/// @param uvMapping 形状全体のバウンディングボックスへ投影する各面の UV 矩形
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addWedge(Vec3 size, const BoxUVMapping& uvMapping);

		/// @brief 指定した UV マッピングを持つ、平行移動したくさび形を追加します。
		/// @param size くさび形の各軸方向の大きさ
		/// @param uvMapping 形状全体のバウンディングボックスへ投影する各面の UV 矩形
		/// @param offset 平行移動量
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addWedge(Vec3 size, const BoxUVMapping& uvMapping, Vec3 offset);

		/// @brief 指定した UV マッピングを持つ、回転および平行移動したくさび形を追加します。
		/// @param size くさび形の各軸方向の大きさ
		/// @param uvMapping 形状全体のバウンディングボックスへ投影する各面の UV 矩形
		/// @param offset 平行移動量
		/// @param rotation 原点を中心とする回転を表す単位クォータニオン
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addWedge(Vec3 size, const BoxUVMapping& uvMapping, Vec3 offset, const Quaternion& rotation);

		/// @brief 指定した UV マッピングを持つ、アフィン変換を適用したくさび形を追加します。
		/// @param size くさび形の各軸方向の大きさ
		/// @param uvMapping 形状全体のバウンディングボックスへ投影する各面の UV 矩形
		/// @param transform 適用するアフィン変換行列
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addWedge(Vec3 size, const BoxUVMapping& uvMapping, const Mat4x4& transform);

		////////////////////////////////////////////////////////////////
		//
		//	addTriangularPrism
		//
		////////////////////////////////////////////////////////////////

		/// @brief 原点を中心とする三角柱を追加します。
		/// @param size 三角柱の各軸方向の大きさ
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addTriangularPrism(Vec3 size = Vec3{ 1.0, 1.0, 1.0 });

		/// @brief 平行移動した三角柱を追加します。
		/// @param size 三角柱の各軸方向の大きさ
		/// @param offset 平行移動量
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addTriangularPrism(Vec3 size, Vec3 offset);

		/// @brief 回転および平行移動した三角柱を追加します。
		/// @param size 三角柱の各軸方向の大きさ
		/// @param offset 平行移動量
		/// @param rotation 原点を中心とする回転を表す単位クォータニオン
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addTriangularPrism(Vec3 size, Vec3 offset, const Quaternion& rotation);

		/// @brief アフィン変換を適用した三角柱を追加します。
		/// @param size 三角柱の各軸方向の大きさ
		/// @param transform 適用するアフィン変換行列
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addTriangularPrism(Vec3 size, const Mat4x4& transform);

		/// @brief 指定した UV マッピングを持つ、原点を中心とする三角柱を追加します。
		/// @param size 三角柱の各軸方向の大きさ
		/// @param uvMapping 形状全体のバウンディングボックスへ投影する各面の UV 矩形
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addTriangularPrism(Vec3 size, const BoxUVMapping& uvMapping);

		/// @brief 指定した UV マッピングを持つ、平行移動した三角柱を追加します。
		/// @param size 三角柱の各軸方向の大きさ
		/// @param uvMapping 形状全体のバウンディングボックスへ投影する各面の UV 矩形
		/// @param offset 平行移動量
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addTriangularPrism(Vec3 size, const BoxUVMapping& uvMapping, Vec3 offset);

		/// @brief 指定した UV マッピングを持つ、回転および平行移動した三角柱を追加します。
		/// @param size 三角柱の各軸方向の大きさ
		/// @param uvMapping 形状全体のバウンディングボックスへ投影する各面の UV 矩形
		/// @param offset 平行移動量
		/// @param rotation 原点を中心とする回転を表す単位クォータニオン
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addTriangularPrism(Vec3 size, const BoxUVMapping& uvMapping, Vec3 offset, const Quaternion& rotation);

		/// @brief 指定した UV マッピングを持つ、アフィン変換を適用した三角柱を追加します。
		/// @param size 三角柱の各軸方向の大きさ
		/// @param uvMapping 形状全体のバウンディングボックスへ投影する各面の UV 矩形
		/// @param transform 適用するアフィン変換行列
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addTriangularPrism(Vec3 size, const BoxUVMapping& uvMapping, const Mat4x4& transform);

		////////////////////////////////////////////////////////////////
		//
		//	addStairs
		//
		////////////////////////////////////////////////////////////////

		/// @brief 原点を中心とし、Z 軸の正方向へ上る階段を追加します。
		/// @param size 階段全体の各軸方向の大きさ
		/// @param steps 段数。1 以上である必要があります。
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addStairs(Vec3 size, uint32 steps);

		/// @brief 平行移動した階段を追加します。
		/// @param size 階段全体の各軸方向の大きさ
		/// @param steps 段数
		/// @param offset 平行移動量
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addStairs(Vec3 size, uint32 steps, Vec3 offset);

		/// @brief 回転および平行移動した階段を追加します。
		/// @param size 階段全体の各軸方向の大きさ
		/// @param steps 段数
		/// @param offset 平行移動量
		/// @param rotation 原点を中心とする回転を表す単位クォータニオン
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addStairs(Vec3 size, uint32 steps, Vec3 offset, const Quaternion& rotation);

		/// @brief アフィン変換を適用した階段を追加します。
		/// @param size 階段全体の各軸方向の大きさ
		/// @param steps 段数
		/// @param transform 適用するアフィン変換行列
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addStairs(Vec3 size, uint32 steps, const Mat4x4& transform);

		/// @brief 指定した UV マッピングを持つ、原点を中心とする階段を追加します。
		/// @param size 階段全体の各軸方向の大きさ
		/// @param steps 段数。1 以上である必要があります。
		/// @param uvMapping 階段全体のバウンディングボックスへ投影する各面の UV 矩形
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addStairs(Vec3 size, uint32 steps, const BoxUVMapping& uvMapping);

		/// @brief 指定した UV マッピングを持つ、平行移動した階段を追加します。
		/// @param size 階段全体の各軸方向の大きさ
		/// @param steps 段数
		/// @param uvMapping 階段全体のバウンディングボックスへ投影する各面の UV 矩形
		/// @param offset 平行移動量
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addStairs(Vec3 size, uint32 steps, const BoxUVMapping& uvMapping, Vec3 offset);

		/// @brief 指定した UV マッピングを持つ、回転および平行移動した階段を追加します。
		/// @param size 階段全体の各軸方向の大きさ
		/// @param steps 段数
		/// @param uvMapping 階段全体のバウンディングボックスへ投影する各面の UV 矩形
		/// @param offset 平行移動量
		/// @param rotation 原点を中心とする回転を表す単位クォータニオン
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addStairs(Vec3 size, uint32 steps, const BoxUVMapping& uvMapping, Vec3 offset, const Quaternion& rotation);

		/// @brief 指定した UV マッピングを持つ、アフィン変換を適用した階段を追加します。
		/// @param size 階段全体の各軸方向の大きさ
		/// @param steps 段数
		/// @param uvMapping 階段全体のバウンディングボックスへ投影する各面の UV 矩形
		/// @param transform 適用するアフィン変換行列
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addStairs(Vec3 size, uint32 steps, const BoxUVMapping& uvMapping, const Mat4x4& transform);

		////////////////////////////////////////////////////////////////
		//
		//	addPyramid
		//
		////////////////////////////////////////////////////////////////

		/// @brief 原点を中心とする正方形底面の四角錐を追加します。
		/// @param baseSize 底面の一辺の長さ
		/// @param height 四角錐の高さ
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addPyramid(double baseSize, double height);

		/// @brief 平行移動した正方形底面の四角錐を追加します。
		/// @param baseSize 底面の一辺の長さ
		/// @param height 四角錐の高さ
		/// @param offset 平行移動量
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addPyramid(double baseSize, double height, Vec3 offset);

		/// @brief 回転および平行移動した正方形底面の四角錐を追加します。
		/// @param baseSize 底面の一辺の長さ
		/// @param height 四角錐の高さ
		/// @param offset 平行移動量
		/// @param rotation 原点を中心とする回転を表す単位クォータニオン
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addPyramid(double baseSize, double height, Vec3 offset, const Quaternion& rotation);

		/// @brief アフィン変換を適用した正方形底面の四角錐を追加します。
		/// @param baseSize 底面の一辺の長さ
		/// @param height 四角錐の高さ
		/// @param transform 適用するアフィン変換行列
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addPyramid(double baseSize, double height, const Mat4x4& transform);

		/// @brief 原点を中心とする長方形底面の四角錐を追加します。
		/// @param baseSizeXZ 底面の X 軸方向および Z 軸方向の大きさ
		/// @param height 四角錐の高さ
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addPyramid(SizeF baseSizeXZ = SizeF{ 1.0, 1.0 }, double height = 1.0);

		/// @brief 平行移動した長方形底面の四角錐を追加します。
		/// @param baseSizeXZ 底面の X 軸方向および Z 軸方向の大きさ
		/// @param height 四角錐の高さ
		/// @param offset 平行移動量
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addPyramid(SizeF baseSizeXZ, double height, Vec3 offset);

		/// @brief 回転および平行移動した長方形底面の四角錐を追加します。
		/// @param baseSizeXZ 底面の X 軸方向および Z 軸方向の大きさ
		/// @param height 四角錐の高さ
		/// @param offset 平行移動量
		/// @param rotation 原点を中心とする回転を表す単位クォータニオン
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addPyramid(SizeF baseSizeXZ, double height, Vec3 offset, const Quaternion& rotation);

		/// @brief アフィン変換を適用した長方形底面の四角錐を追加します。
		/// @param baseSizeXZ 底面の X 軸方向および Z 軸方向の大きさ
		/// @param height 四角錐の高さ
		/// @param transform 適用するアフィン変換行列
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addPyramid(SizeF baseSizeXZ, double height, const Mat4x4& transform);

		////////////////////////////////////////////////////////////////
		//
		//	addRectangularFrustum
		//
		////////////////////////////////////////////////////////////////

		/// @brief 原点を中心とする角錐台を追加します。
		/// @param bottomSizeXZ 底面の X 軸方向および Z 軸方向の大きさ
		/// @param topSizeXZ 上面の X 軸方向および Z 軸方向の大きさ
		/// @param height 角錐台の高さ
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addRectangularFrustum(SizeF bottomSizeXZ, SizeF topSizeXZ, double height);

		/// @brief 平行移動した角錐台を追加します。
		/// @param bottomSizeXZ 底面の X 軸方向および Z 軸方向の大きさ
		/// @param topSizeXZ 上面の X 軸方向および Z 軸方向の大きさ
		/// @param height 角錐台の高さ
		/// @param offset 平行移動量
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addRectangularFrustum(SizeF bottomSizeXZ, SizeF topSizeXZ, double height, Vec3 offset);

		/// @brief 回転および平行移動した角錐台を追加します。
		/// @param bottomSizeXZ 底面の X 軸方向および Z 軸方向の大きさ
		/// @param topSizeXZ 上面の X 軸方向および Z 軸方向の大きさ
		/// @param height 角錐台の高さ
		/// @param offset 平行移動量
		/// @param rotation 原点を中心とする回転を表す単位クォータニオン
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addRectangularFrustum(SizeF bottomSizeXZ, SizeF topSizeXZ, double height, Vec3 offset, const Quaternion& rotation);

		/// @brief アフィン変換を適用した角錐台を追加します。
		/// @param bottomSizeXZ 底面の X 軸方向および Z 軸方向の大きさ
		/// @param topSizeXZ 上面の X 軸方向および Z 軸方向の大きさ
		/// @param height 角錐台の高さ
		/// @param transform 適用するアフィン変換行列
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addRectangularFrustum(SizeF bottomSizeXZ, SizeF topSizeXZ, double height, const Mat4x4& transform);

		/// @brief 指定した UV マッピングを持つ、原点を中心とする角錐台を追加します。
		/// @param bottomSizeXZ 底面の X 軸方向および Z 軸方向の大きさ
		/// @param topSizeXZ 上面の X 軸方向および Z 軸方向の大きさ
		/// @param height 角錐台の高さ
		/// @param uvMapping 形状全体のバウンディングボックスへ投影する各面の UV 矩形
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addRectangularFrustum(SizeF bottomSizeXZ, SizeF topSizeXZ, double height, const BoxUVMapping& uvMapping);

		/// @brief 指定した UV マッピングを持つ、平行移動した角錐台を追加します。
		/// @param bottomSizeXZ 底面の X 軸方向および Z 軸方向の大きさ
		/// @param topSizeXZ 上面の X 軸方向および Z 軸方向の大きさ
		/// @param height 角錐台の高さ
		/// @param uvMapping 形状全体のバウンディングボックスへ投影する各面の UV 矩形
		/// @param offset 平行移動量
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addRectangularFrustum(SizeF bottomSizeXZ, SizeF topSizeXZ, double height, const BoxUVMapping& uvMapping, Vec3 offset);

		/// @brief 指定した UV マッピングを持つ、回転および平行移動した角錐台を追加します。
		/// @param bottomSizeXZ 底面の X 軸方向および Z 軸方向の大きさ
		/// @param topSizeXZ 上面の X 軸方向および Z 軸方向の大きさ
		/// @param height 角錐台の高さ
		/// @param uvMapping 形状全体のバウンディングボックスへ投影する各面の UV 矩形
		/// @param offset 平行移動量
		/// @param rotation 原点を中心とする回転を表す単位クォータニオン
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addRectangularFrustum(SizeF bottomSizeXZ, SizeF topSizeXZ, double height, const BoxUVMapping& uvMapping, Vec3 offset, const Quaternion& rotation);

		/// @brief 指定した UV マッピングを持つ、アフィン変換を適用した角錐台を追加します。
		/// @param bottomSizeXZ 底面の X 軸方向および Z 軸方向の大きさ
		/// @param topSizeXZ 上面の X 軸方向および Z 軸方向の大きさ
		/// @param height 角錐台の高さ
		/// @param uvMapping 形状全体のバウンディングボックスへ投影する各面の UV 矩形
		/// @param transform 適用するアフィン変換行列
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addRectangularFrustum(SizeF bottomSizeXZ, SizeF topSizeXZ, double height, const BoxUVMapping& uvMapping, const Mat4x4& transform);

		////////////////////////////////////////////////////////////////
		//
		//	addTetrahedron
		//
		////////////////////////////////////////////////////////////////

		/// @brief 原点を中心とする正四面体を追加します。
		/// @param radius 外接球の半径
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addTetrahedron(double radius = 1.0);

		/// @brief 平行移動した正四面体を追加します。
		/// @param radius 外接球の半径
		/// @param offset 平行移動量
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addTetrahedron(double radius, Vec3 offset);

		/// @brief 回転および平行移動した正四面体を追加します。
		/// @param radius 外接球の半径
		/// @param offset 平行移動量
		/// @param rotation 原点を中心とする回転を表す単位クォータニオン
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addTetrahedron(double radius, Vec3 offset, const Quaternion& rotation);

		/// @brief アフィン変換を適用した正四面体を追加します。
		/// @param radius 外接球の半径
		/// @param transform 適用するアフィン変換行列
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addTetrahedron(double radius, const Mat4x4& transform);

		////////////////////////////////////////////////////////////////
		//
		//	addOctahedron
		//
		////////////////////////////////////////////////////////////////

		/// @brief 原点を中心とする正八面体を追加します。
		/// @param radius 外接球の半径
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addOctahedron(double radius = 1.0);

		/// @brief 平行移動した正八面体を追加します。
		/// @param radius 外接球の半径
		/// @param offset 平行移動量
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addOctahedron(double radius, Vec3 offset);

		/// @brief 回転および平行移動した正八面体を追加します。
		/// @param radius 外接球の半径
		/// @param offset 平行移動量
		/// @param rotation 原点を中心とする回転を表す単位クォータニオン
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addOctahedron(double radius, Vec3 offset, const Quaternion& rotation);

		/// @brief アフィン変換を適用した正八面体を追加します。
		/// @param radius 外接球の半径
		/// @param transform 適用するアフィン変換行列
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addOctahedron(double radius, const Mat4x4& transform);

		////////////////////////////////////////////////////////////////
		//
		//	addIcosahedron
		//
		////////////////////////////////////////////////////////////////

		/// @brief 原点を中心とする正二十面体を追加します。
		/// @param radius 外接球の半径
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addIcosahedron(double radius = 1.0);

		/// @brief 平行移動した正二十面体を追加します。
		/// @param radius 外接球の半径
		/// @param offset 平行移動量
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addIcosahedron(double radius, Vec3 offset);

		/// @brief 回転および平行移動した正二十面体を追加します。
		/// @param radius 外接球の半径
		/// @param offset 平行移動量
		/// @param rotation 原点を中心とする回転を表す単位クォータニオン
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addIcosahedron(double radius, Vec3 offset, const Quaternion& rotation);

		/// @brief アフィン変換を適用した正二十面体を追加します。
		/// @param radius 外接球の半径
		/// @param transform 適用するアフィン変換行列
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addIcosahedron(double radius, const Mat4x4& transform);

		////////////////////////////////////////////////////////////////
		//
		//	addDodecahedron
		//
		////////////////////////////////////////////////////////////////

		/// @brief 原点を中心とする正十二面体を追加します。
		/// @param radius 外接球の半径
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addDodecahedron(double radius = 1.0);

		/// @brief 平行移動した正十二面体を追加します。
		/// @param radius 外接球の半径
		/// @param offset 平行移動量
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addDodecahedron(double radius, Vec3 offset);

		/// @brief 回転および平行移動した正十二面体を追加します。
		/// @param radius 外接球の半径
		/// @param offset 平行移動量
		/// @param rotation 原点を中心とする回転を表す単位クォータニオン
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addDodecahedron(double radius, Vec3 offset, const Quaternion& rotation);

		/// @brief アフィン変換を適用した正十二面体を追加します。
		/// @param radius 外接球の半径
		/// @param transform 適用するアフィン変換行列
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addDodecahedron(double radius, const Mat4x4& transform);

		////////////////////////////////////////////////////////////////
		//
		//	addPlane
		//
		////////////////////////////////////////////////////////////////

		/// @brief XZ 平面上に、法線が Y 軸の正方向を向く長方形を追加します。
		/// @param sizeXZ X 軸方向および Z 軸方向の大きさ
		/// @param uvScale UV 座標の拡大率
		/// @param uvOffset UV 座標のオフセット
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addPlane(
			SizeF sizeXZ = SizeF{ 1.0, 1.0 },
			Vec2 uvScale = Vec2{ 1.0, 1.0 },
			Vec2 uvOffset = Vec2{ 0.0, 0.0 });

		/// @brief 平行移動した長方形を追加します。
		/// @param sizeXZ X 軸方向および Z 軸方向の大きさ
		/// @param offset 平行移動量
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addPlane(SizeF sizeXZ, Vec3 offset);

		/// @brief 回転および平行移動した長方形を追加します。
		/// @param sizeXZ X 軸方向および Z 軸方向の大きさ
		/// @param offset 平行移動量
		/// @param rotation 原点を中心とする回転を表す単位クォータニオン
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addPlane(SizeF sizeXZ, Vec3 offset, const Quaternion& rotation);

		/// @brief アフィン変換を適用した長方形を追加します。
		/// @param sizeXZ X 軸方向および Z 軸方向の大きさ
		/// @param transform 適用するアフィン変換行列
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addPlane(SizeF sizeXZ, const Mat4x4& transform);

		/// @brief UV 変換と平行移動を適用した長方形を追加します。
		/// @param sizeXZ X 軸方向および Z 軸方向の大きさ
		/// @param uvScale UV 座標の拡大率
		/// @param uvOffset UV 座標のオフセット
		/// @param offset 平行移動量
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addPlane(SizeF sizeXZ, Vec2 uvScale, Vec2 uvOffset, Vec3 offset);

		/// @brief UV 変換、回転、および平行移動を適用した長方形を追加します。
		/// @param sizeXZ X 軸方向および Z 軸方向の大きさ
		/// @param uvScale UV 座標の拡大率
		/// @param uvOffset UV 座標のオフセット
		/// @param offset 平行移動量
		/// @param rotation 原点を中心とする回転を表す単位クォータニオン
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addPlane(SizeF sizeXZ, Vec2 uvScale, Vec2 uvOffset, Vec3 offset, const Quaternion& rotation);

		/// @brief UV 変換とアフィン変換を適用した長方形を追加します。
		/// @param sizeXZ X 軸方向および Z 軸方向の大きさ
		/// @param uvScale UV 座標の拡大率
		/// @param uvOffset UV 座標のオフセット
		/// @param transform 適用するアフィン変換行列
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addPlane(SizeF sizeXZ, Vec2 uvScale, Vec2 uvOffset, const Mat4x4& transform);

		////////////////////////////////////////////////////////////////
		//
		//	addGrid
		//
		////////////////////////////////////////////////////////////////

		/// @brief XZ 平面上に、法線が Y 軸の正方向を向く格子を追加します。
		/// @param sizeXZ X 軸方向および Z 軸方向の大きさ
		/// @param segmentsX X 軸方向の分割数
		/// @param segmentsZ Z 軸方向の分割数
		/// @param uvScale UV 座標の拡大率
		/// @param uvOffset UV 座標のオフセット
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addGrid(
			SizeF sizeXZ,
			uint32 segmentsX,
			uint32 segmentsZ,
			Vec2 uvScale = Vec2{ 1.0, 1.0 },
			Vec2 uvOffset = Vec2{ 0.0, 0.0 });

		/// @brief 平行移動した格子を追加します。
		/// @param sizeXZ X 軸方向および Z 軸方向の大きさ
		/// @param segmentsX X 軸方向の分割数
		/// @param segmentsZ Z 軸方向の分割数
		/// @param offset 平行移動量
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addGrid(SizeF sizeXZ, uint32 segmentsX, uint32 segmentsZ, Vec3 offset);

		/// @brief 回転および平行移動した格子を追加します。
		/// @param sizeXZ X 軸方向および Z 軸方向の大きさ
		/// @param segmentsX X 軸方向の分割数
		/// @param segmentsZ Z 軸方向の分割数
		/// @param offset 平行移動量
		/// @param rotation 原点を中心とする回転を表す単位クォータニオン
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addGrid(SizeF sizeXZ, uint32 segmentsX, uint32 segmentsZ, Vec3 offset, const Quaternion& rotation);

		/// @brief アフィン変換を適用した格子を追加します。
		/// @param sizeXZ X 軸方向および Z 軸方向の大きさ
		/// @param segmentsX X 軸方向の分割数
		/// @param segmentsZ Z 軸方向の分割数
		/// @param transform 適用するアフィン変換行列
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addGrid(SizeF sizeXZ, uint32 segmentsX, uint32 segmentsZ, const Mat4x4& transform);

		/// @brief UV 変換と平行移動を適用した格子を追加します。
		/// @param sizeXZ X 軸方向および Z 軸方向の大きさ
		/// @param segmentsX X 軸方向の分割数
		/// @param segmentsZ Z 軸方向の分割数
		/// @param uvScale UV 座標の拡大率
		/// @param uvOffset UV 座標のオフセット
		/// @param offset 平行移動量
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addGrid(SizeF sizeXZ, uint32 segmentsX, uint32 segmentsZ, Vec2 uvScale, Vec2 uvOffset, Vec3 offset);

		/// @brief UV 変換、回転、および平行移動を適用した格子を追加します。
		/// @param sizeXZ X 軸方向および Z 軸方向の大きさ
		/// @param segmentsX X 軸方向の分割数
		/// @param segmentsZ Z 軸方向の分割数
		/// @param uvScale UV 座標の拡大率
		/// @param uvOffset UV 座標のオフセット
		/// @param offset 平行移動量
		/// @param rotation 原点を中心とする回転を表す単位クォータニオン
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addGrid(SizeF sizeXZ, uint32 segmentsX, uint32 segmentsZ, Vec2 uvScale, Vec2 uvOffset, Vec3 offset, const Quaternion& rotation);

		/// @brief UV 変換とアフィン変換を適用した格子を追加します。
		/// @param sizeXZ X 軸方向および Z 軸方向の大きさ
		/// @param segmentsX X 軸方向の分割数
		/// @param segmentsZ Z 軸方向の分割数
		/// @param uvScale UV 座標の拡大率
		/// @param uvOffset UV 座標のオフセット
		/// @param transform 適用するアフィン変換行列
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addGrid(SizeF sizeXZ, uint32 segmentsX, uint32 segmentsZ, Vec2 uvScale, Vec2 uvOffset, const Mat4x4& transform);

		////////////////////////////////////////////////////////////////
		//
		//	addDisc
		//
		////////////////////////////////////////////////////////////////

		/// @brief XZ 平面上に、法線が Y 軸の正方向を向く円盤を追加します。
		/// @param radius 円盤の半径
		/// @param segments 円周の分割数。3 以上である必要があります。
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addDisc(double radius, uint32 segments = 32);

		/// @brief 平行移動した円盤を追加します。
		/// @param radius 円盤の半径
		/// @param segments 円周の分割数
		/// @param offset 平行移動量
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addDisc(double radius, uint32 segments, Vec3 offset);

		/// @brief 回転および平行移動した円盤を追加します。
		/// @param radius 円盤の半径
		/// @param segments 円周の分割数
		/// @param offset 平行移動量
		/// @param rotation 原点を中心とする回転を表す単位クォータニオン
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addDisc(double radius, uint32 segments, Vec3 offset, const Quaternion& rotation);

		/// @brief アフィン変換を適用した円盤を追加します。
		/// @param radius 円盤の半径
		/// @param segments 円周の分割数
		/// @param transform 適用するアフィン変換行列
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addDisc(double radius, uint32 segments, const Mat4x4& transform);

		////////////////////////////////////////////////////////////////
		//
		//	addAnnulus
		//
		////////////////////////////////////////////////////////////////

		/// @brief XZ 平面上に、法線が Y 軸の正方向を向く円環を追加します。
		/// @param innerRadius 円環の内半径。0 以上である必要があります。
		/// @param outerRadius 円環の外半径。`innerRadius` より大きい必要があります。
		/// @param segments 円周の分割数。3 以上である必要があります。
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addAnnulus(double innerRadius, double outerRadius, uint32 segments = 32);

		/// @brief 平行移動した円環を追加します。
		/// @param innerRadius 円環の内半径
		/// @param outerRadius 円環の外半径
		/// @param segments 円周の分割数
		/// @param offset 平行移動量
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addAnnulus(double innerRadius, double outerRadius, uint32 segments, Vec3 offset);

		/// @brief 回転および平行移動した円環を追加します。
		/// @param innerRadius 円環の内半径
		/// @param outerRadius 円環の外半径
		/// @param segments 円周の分割数
		/// @param offset 平行移動量
		/// @param rotation 原点を中心とする回転を表す単位クォータニオン
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addAnnulus(double innerRadius, double outerRadius, uint32 segments, Vec3 offset, const Quaternion& rotation);

		/// @brief アフィン変換を適用した円環を追加します。
		/// @param innerRadius 円環の内半径
		/// @param outerRadius 円環の外半径
		/// @param segments 円周の分割数
		/// @param transform 適用するアフィン変換行列
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addAnnulus(double innerRadius, double outerRadius, uint32 segments, const Mat4x4& transform);

	private:

		Mesh3D m_mesh;
	};
}

# include "detail/Mesh3DBuilder.ipp"
