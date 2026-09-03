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

	private:

		Mesh3D m_mesh;
	};
}

# include "detail/Mesh3DBuilder.ipp"
