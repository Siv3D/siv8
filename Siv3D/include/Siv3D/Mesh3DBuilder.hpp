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
		/// @param faces 生成する面
		/// @return 追加に成功した場合 true, それ以外の場合は false
		/// @remark `faces == BoxFace::None_` の場合、メッシュを変更せず true を返します。
		bool addBox(
			Vec3 size = Vec3{ 1.0, 1.0, 1.0 },
			BoxFace faces = BoxFace::All);

		/// @brief 平行移動した直方体を追加します。
		/// @param size 直方体の各軸方向の大きさ
		/// @param offset 平行移動量
		/// @param faces 生成する面
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addBox(Vec3 size, Vec3 offset, BoxFace faces = BoxFace::All);

		/// @brief 回転および平行移動した直方体を追加します。
		/// @param size 直方体の各軸方向の大きさ
		/// @param offset 平行移動量
		/// @param rotation 原点を中心とする回転を表す単位クォータニオン
		/// @param faces 生成する面
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addBox(Vec3 size, Vec3 offset, const Quaternion& rotation, BoxFace faces = BoxFace::All);

		/// @brief アフィン変換を適用した直方体を追加します。
		/// @param size 直方体の各軸方向の大きさ
		/// @param transform 適用するアフィン変換行列
		/// @param faces 生成する面
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addBox(Vec3 size, const Mat4x4& transform, BoxFace faces = BoxFace::All);

		/// @brief 指定した UV マッピングを持つ、原点を中心とする直方体を追加します。
		/// @param size 直方体の各軸方向の大きさ
		/// @param uvMapping 各面に割り当てる UV 矩形
		/// @param faces 生成する面
		/// @return 追加に成功した場合 true, それ以外の場合は false
		/// @remark `uvMapping` は `faces` で選択した面に対応する矩形のみ検証されます。
		/// @remark `faces == BoxFace::None_` の場合、メッシュを変更せず true を返します。
		bool addBox(Vec3 size, const BoxUVMapping& uvMapping, BoxFace faces = BoxFace::All);

		/// @brief 指定した UV マッピングを持つ、平行移動した直方体を追加します。
		/// @param size 直方体の各軸方向の大きさ
		/// @param uvMapping 各面に割り当てる UV 矩形
		/// @param offset 平行移動量
		/// @param faces 生成する面
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addBox(Vec3 size, const BoxUVMapping& uvMapping, Vec3 offset, BoxFace faces = BoxFace::All);

		/// @brief 指定した UV マッピングを持つ、回転および平行移動した直方体を追加します。
		/// @param size 直方体の各軸方向の大きさ
		/// @param uvMapping 各面に割り当てる UV 矩形
		/// @param offset 平行移動量
		/// @param rotation 原点を中心とする回転を表す単位クォータニオン
		/// @param faces 生成する面
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addBox(Vec3 size, const BoxUVMapping& uvMapping, Vec3 offset, const Quaternion& rotation, BoxFace faces = BoxFace::All);

		/// @brief 指定した UV マッピングを持つ、アフィン変換を適用した直方体を追加します。
		/// @param size 直方体の各軸方向の大きさ
		/// @param uvMapping 各面に割り当てる UV 矩形
		/// @param transform 適用するアフィン変換行列
		/// @param faces 生成する面
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addBox(Vec3 size, const BoxUVMapping& uvMapping, const Mat4x4& transform, BoxFace faces = BoxFace::All);

		////////////////////////////////////////////////////////////////
		//
		//	addBoxShell
		//
		////////////////////////////////////////////////////////////////

		/// @brief 原点を中心とする、均一な厚みを持つ中空直方体を追加します。
		/// @param outerSize 外側の直方体の各軸方向の大きさ
		/// @param thickness 壁の厚み。正の有限値で、`outerSize` の最小成分の半分未満である必要があります。
		/// @param openFaces 壁を生成しない開口面
		/// @return 追加に成功した場合 true, それ以外の場合は false
		/// @remark `openFaces == BoxFace::All` の場合、メッシュを変更せず true を返します。
		bool addBoxShell(Vec3 outerSize = Vec3{ 1.0, 1.0, 1.0 }, double thickness = 0.1, BoxFace openFaces = BoxFace::None_);

		/// @brief 平行移動した、均一な厚みの中空直方体を追加します。
		/// @param outerSize 外側の直方体の各軸方向の大きさ
		/// @param thickness 壁の厚み
		/// @param offset 平行移動量
		/// @param openFaces 壁を生成しない開口面
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addBoxShell(Vec3 outerSize, double thickness, Vec3 offset, BoxFace openFaces = BoxFace::None_);

		/// @brief 回転および平行移動した、均一な厚みの中空直方体を追加します。
		/// @param outerSize 外側の直方体の各軸方向の大きさ
		/// @param thickness 壁の厚み
		/// @param offset 平行移動量
		/// @param rotation 原点を中心とする回転を表す単位クォータニオン
		/// @param openFaces 壁を生成しない開口面
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addBoxShell(Vec3 outerSize, double thickness, Vec3 offset, const Quaternion& rotation, BoxFace openFaces = BoxFace::None_);

		/// @brief アフィン変換を適用した、均一な厚みの中空直方体を追加します。
		/// @param outerSize 外側の直方体の各軸方向の大きさ
		/// @param thickness 壁の厚み
		/// @param transform 適用するアフィン変換行列
		/// @param openFaces 壁を生成しない開口面
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addBoxShell(Vec3 outerSize, double thickness, const Mat4x4& transform, BoxFace openFaces = BoxFace::None_);

		/// @brief 原点を中心とする、軸ごとの厚みを持つ中空直方体を追加します。
		/// @param outerSize 外側の直方体の各軸方向の大きさ
		/// @param thickness 各軸に垂直な壁の厚み。各成分は正の有限値で、対応する `outerSize` 成分の半分未満である必要があります。
		/// @param openFaces 壁を生成しない開口面
		/// @return 追加に成功した場合 true, それ以外の場合は false
		/// @remark `openFaces == BoxFace::All` の場合、メッシュを変更せず true を返します。
		bool addBoxShell(Vec3 outerSize, Vec3 thickness, BoxFace openFaces = BoxFace::None_);

		/// @brief 平行移動した、軸ごとの厚みの中空直方体を追加します。
		/// @param outerSize 外側の直方体の各軸方向の大きさ
		/// @param thickness 各軸に垂直な壁の厚み
		/// @param offset 平行移動量
		/// @param openFaces 壁を生成しない開口面
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addBoxShell(Vec3 outerSize, Vec3 thickness, Vec3 offset, BoxFace openFaces = BoxFace::None_);

		/// @brief 回転および平行移動した、軸ごとの厚みの中空直方体を追加します。
		/// @param outerSize 外側の直方体の各軸方向の大きさ
		/// @param thickness 各軸に垂直な壁の厚み
		/// @param offset 平行移動量
		/// @param rotation 原点を中心とする回転を表す単位クォータニオン
		/// @param openFaces 壁を生成しない開口面
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addBoxShell(Vec3 outerSize, Vec3 thickness, Vec3 offset, const Quaternion& rotation, BoxFace openFaces = BoxFace::None_);

		/// @brief アフィン変換を適用した、軸ごとの厚みの中空直方体を追加します。
		/// @param outerSize 外側の直方体の各軸方向の大きさ
		/// @param thickness 各軸に垂直な壁の厚み
		/// @param transform 適用するアフィン変換行列
		/// @param openFaces 壁を生成しない開口面
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addBoxShell(Vec3 outerSize, Vec3 thickness, const Mat4x4& transform, BoxFace openFaces = BoxFace::None_);

		/// @brief 指定した UV マッピングを持つ、均一な厚みの中空直方体を追加します。
		/// @param outerSize 外側の直方体の各軸方向の大きさ
		/// @param thickness 壁の厚み
		/// @param uvMapping 外接 Box の各投影面に割り当てる UV 矩形
		/// @param openFaces 壁を生成しない開口面
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addBoxShell(Vec3 outerSize, double thickness, const BoxUVMapping& uvMapping, BoxFace openFaces = BoxFace::None_);

		/// @brief 指定した UV マッピングを持つ、平行移動した均一な厚みの中空直方体を追加します。
		/// @param outerSize 外側の直方体の各軸方向の大きさ
		/// @param thickness 壁の厚み
		/// @param uvMapping 外接 Box の各投影面に割り当てる UV 矩形
		/// @param offset 平行移動量
		/// @param openFaces 壁を生成しない開口面
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addBoxShell(Vec3 outerSize, double thickness, const BoxUVMapping& uvMapping, Vec3 offset, BoxFace openFaces = BoxFace::None_);

		/// @brief 指定した UV マッピングを持つ、回転および平行移動した均一な厚みの中空直方体を追加します。
		/// @param outerSize 外側の直方体の各軸方向の大きさ
		/// @param thickness 壁の厚み
		/// @param uvMapping 外接 Box の各投影面に割り当てる UV 矩形
		/// @param offset 平行移動量
		/// @param rotation 原点を中心とする回転を表す単位クォータニオン
		/// @param openFaces 壁を生成しない開口面
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addBoxShell(Vec3 outerSize, double thickness, const BoxUVMapping& uvMapping, Vec3 offset, const Quaternion& rotation, BoxFace openFaces = BoxFace::None_);

		/// @brief 指定した UV マッピングを持つ、アフィン変換を適用した均一な厚みの中空直方体を追加します。
		/// @param outerSize 外側の直方体の各軸方向の大きさ
		/// @param thickness 壁の厚み
		/// @param uvMapping 外接 Box の各投影面に割り当てる UV 矩形
		/// @param transform 適用するアフィン変換行列
		/// @param openFaces 壁を生成しない開口面
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addBoxShell(Vec3 outerSize, double thickness, const BoxUVMapping& uvMapping, const Mat4x4& transform, BoxFace openFaces = BoxFace::None_);

		/// @brief 指定した UV マッピングを持つ、軸ごとの厚みの中空直方体を追加します。
		/// @param outerSize 外側の直方体の各軸方向の大きさ
		/// @param thickness 各軸に垂直な壁の厚み
		/// @param uvMapping 外接 Box の各投影面に割り当てる UV 矩形
		/// @param openFaces 壁を生成しない開口面
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addBoxShell(Vec3 outerSize, Vec3 thickness, const BoxUVMapping& uvMapping, BoxFace openFaces = BoxFace::None_);

		/// @brief 指定した UV マッピングを持つ、平行移動した軸ごとの厚みの中空直方体を追加します。
		/// @param outerSize 外側の直方体の各軸方向の大きさ
		/// @param thickness 各軸に垂直な壁の厚み
		/// @param uvMapping 外接 Box の各投影面に割り当てる UV 矩形
		/// @param offset 平行移動量
		/// @param openFaces 壁を生成しない開口面
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addBoxShell(Vec3 outerSize, Vec3 thickness, const BoxUVMapping& uvMapping, Vec3 offset, BoxFace openFaces = BoxFace::None_);

		/// @brief 指定した UV マッピングを持つ、回転および平行移動した軸ごとの厚みの中空直方体を追加します。
		/// @param outerSize 外側の直方体の各軸方向の大きさ
		/// @param thickness 各軸に垂直な壁の厚み
		/// @param uvMapping 外接 Box の各投影面に割り当てる UV 矩形
		/// @param offset 平行移動量
		/// @param rotation 原点を中心とする回転を表す単位クォータニオン
		/// @param openFaces 壁を生成しない開口面
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addBoxShell(Vec3 outerSize, Vec3 thickness, const BoxUVMapping& uvMapping, Vec3 offset, const Quaternion& rotation, BoxFace openFaces = BoxFace::None_);

		/// @brief 指定した UV マッピングを持つ、アフィン変換を適用した軸ごとの厚みの中空直方体を追加します。
		/// @param outerSize 外側の直方体の各軸方向の大きさ
		/// @param thickness 各軸に垂直な壁の厚み
		/// @param uvMapping 外接 Box の各投影面に割り当てる UV 矩形
		/// @param transform 適用するアフィン変換行列
		/// @param openFaces 壁を生成しない開口面
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addBoxShell(Vec3 outerSize, Vec3 thickness, const BoxUVMapping& uvMapping, const Mat4x4& transform, BoxFace openFaces = BoxFace::None_);

		////////////////////////////////////////////////////////////////
		//
		//	addBoxFrame
		//
		////////////////////////////////////////////////////////////////

		/// @brief 原点を中心とする、均一な太さの直方体枠を追加します。
		/// @param size 枠の外側の各軸方向の大きさ
		/// @param thickness 角材の太さ。正の有限値で、`size` の最小成分の半分未満である必要があります。
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addBoxFrame(Vec3 size = Vec3{ 1.0, 1.0, 1.0 }, double thickness = 0.1);

		/// @brief 平行移動した、均一な太さの直方体枠を追加します。
		/// @param size 枠の外側の各軸方向の大きさ
		/// @param thickness 角材の太さ
		/// @param offset 平行移動量
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addBoxFrame(Vec3 size, double thickness, Vec3 offset);

		/// @brief 回転および平行移動した、均一な太さの直方体枠を追加します。
		/// @param size 枠の外側の各軸方向の大きさ
		/// @param thickness 角材の太さ
		/// @param offset 平行移動量
		/// @param rotation 原点を中心とする回転を表す単位クォータニオン
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addBoxFrame(Vec3 size, double thickness, Vec3 offset, const Quaternion& rotation);

		/// @brief アフィン変換を適用した、均一な太さの直方体枠を追加します。
		/// @param size 枠の外側の各軸方向の大きさ
		/// @param thickness 角材の太さ
		/// @param transform 適用するアフィン変換行列
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addBoxFrame(Vec3 size, double thickness, const Mat4x4& transform);

		/// @brief 原点を中心とする、軸ごとの太さを持つ直方体枠を追加します。
		/// @param size 枠の外側の各軸方向の大きさ
		/// @param beamSize 角材の軸ごとの太さ。各成分は正の有限値で、対応する `size` 成分の半分未満である必要があります。
		/// @return 追加に成功した場合 true, それ以外の場合は false
		/// @remark X 方向の角材は `(size.x, beamSize.y, beamSize.z)`、Y/Z 方向の角材も同様の大きさになります。
		bool addBoxFrame(Vec3 size, Vec3 beamSize);

		/// @brief 平行移動した、軸ごとの太さを持つ直方体枠を追加します。
		/// @param size 枠の外側の各軸方向の大きさ
		/// @param beamSize 角材の軸ごとの太さ
		/// @param offset 平行移動量
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addBoxFrame(Vec3 size, Vec3 beamSize, Vec3 offset);

		/// @brief 回転および平行移動した、軸ごとの太さを持つ直方体枠を追加します。
		/// @param size 枠の外側の各軸方向の大きさ
		/// @param beamSize 角材の軸ごとの太さ
		/// @param offset 平行移動量
		/// @param rotation 原点を中心とする回転を表す単位クォータニオン
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addBoxFrame(Vec3 size, Vec3 beamSize, Vec3 offset, const Quaternion& rotation);

		/// @brief アフィン変換を適用した、軸ごとの太さを持つ直方体枠を追加します。
		/// @param size 枠の外側の各軸方向の大きさ
		/// @param beamSize 角材の軸ごとの太さ
		/// @param transform 適用するアフィン変換行列
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addBoxFrame(Vec3 size, Vec3 beamSize, const Mat4x4& transform);

		/// @brief 指定した UV マッピングを持つ、均一な太さの直方体枠を追加します。
		/// @param size 枠の外側の各軸方向の大きさ
		/// @param thickness 角材の太さ
		/// @param uvMapping 外接 Box の各投影面に割り当てる UV 矩形
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addBoxFrame(Vec3 size, double thickness, const BoxUVMapping& uvMapping);

		/// @brief 指定した UV マッピングを持つ、平行移動した均一な太さの直方体枠を追加します。
		/// @param size 枠の外側の各軸方向の大きさ
		/// @param thickness 角材の太さ
		/// @param uvMapping 外接 Box の各投影面に割り当てる UV 矩形
		/// @param offset 平行移動量
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addBoxFrame(Vec3 size, double thickness, const BoxUVMapping& uvMapping, Vec3 offset);

		/// @brief 指定した UV マッピングを持つ、回転および平行移動した均一な太さの直方体枠を追加します。
		/// @param size 枠の外側の各軸方向の大きさ
		/// @param thickness 角材の太さ
		/// @param uvMapping 外接 Box の各投影面に割り当てる UV 矩形
		/// @param offset 平行移動量
		/// @param rotation 原点を中心とする回転を表す単位クォータニオン
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addBoxFrame(Vec3 size, double thickness, const BoxUVMapping& uvMapping, Vec3 offset, const Quaternion& rotation);

		/// @brief 指定した UV マッピングを持つ、アフィン変換を適用した均一な太さの直方体枠を追加します。
		/// @param size 枠の外側の各軸方向の大きさ
		/// @param thickness 角材の太さ
		/// @param uvMapping 外接 Box の各投影面に割り当てる UV 矩形
		/// @param transform 適用するアフィン変換行列
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addBoxFrame(Vec3 size, double thickness, const BoxUVMapping& uvMapping, const Mat4x4& transform);

		/// @brief 指定した UV マッピングを持つ、軸ごとの太さを持つ直方体枠を追加します。
		/// @param size 枠の外側の各軸方向の大きさ
		/// @param beamSize 角材の軸ごとの太さ
		/// @param uvMapping 外接 Box の各投影面に割り当てる UV 矩形
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addBoxFrame(Vec3 size, Vec3 beamSize, const BoxUVMapping& uvMapping);

		/// @brief 指定した UV マッピングを持つ、平行移動した軸ごとの太さの直方体枠を追加します。
		/// @param size 枠の外側の各軸方向の大きさ
		/// @param beamSize 角材の軸ごとの太さ
		/// @param uvMapping 外接 Box の各投影面に割り当てる UV 矩形
		/// @param offset 平行移動量
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addBoxFrame(Vec3 size, Vec3 beamSize, const BoxUVMapping& uvMapping, Vec3 offset);

		/// @brief 指定した UV マッピングを持つ、回転および平行移動した軸ごとの太さの直方体枠を追加します。
		/// @param size 枠の外側の各軸方向の大きさ
		/// @param beamSize 角材の軸ごとの太さ
		/// @param uvMapping 外接 Box の各投影面に割り当てる UV 矩形
		/// @param offset 平行移動量
		/// @param rotation 原点を中心とする回転を表す単位クォータニオン
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addBoxFrame(Vec3 size, Vec3 beamSize, const BoxUVMapping& uvMapping, Vec3 offset, const Quaternion& rotation);

		/// @brief 指定した UV マッピングを持つ、アフィン変換を適用した軸ごとの太さの直方体枠を追加します。
		/// @param size 枠の外側の各軸方向の大きさ
		/// @param beamSize 角材の軸ごとの太さ
		/// @param uvMapping 外接 Box の各投影面に割り当てる UV 矩形
		/// @param transform 適用するアフィン変換行列
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addBoxFrame(Vec3 size, Vec3 beamSize, const BoxUVMapping& uvMapping, const Mat4x4& transform);

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
		//	addChamferedBox
		//
		////////////////////////////////////////////////////////////////

		/// @brief 原点を中心とする面取り直方体を追加します。
		/// @param size 面取り直方体の各軸方向の大きさ
		/// @param chamfer 面取り幅。0 以上、`size` の最小成分の半分未満である必要があります。
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addChamferedBox(
			Vec3 size = Vec3{ 1.0, 1.0, 1.0 },
			double chamfer = 0.1);

		/// @brief 平行移動した面取り直方体を追加します。
		/// @param size 面取り直方体の各軸方向の大きさ
		/// @param chamfer 面取り幅
		/// @param offset 平行移動量
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addChamferedBox(Vec3 size, double chamfer, Vec3 offset);

		/// @brief 回転および平行移動した面取り直方体を追加します。
		/// @param size 面取り直方体の各軸方向の大きさ
		/// @param chamfer 面取り幅
		/// @param offset 平行移動量
		/// @param rotation 原点を中心とする回転を表す単位クォータニオン
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addChamferedBox(Vec3 size, double chamfer, Vec3 offset, const Quaternion& rotation);

		/// @brief アフィン変換を適用した面取り直方体を追加します。
		/// @param size 面取り直方体の各軸方向の大きさ
		/// @param chamfer 面取り幅
		/// @param transform 適用するアフィン変換行列
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addChamferedBox(Vec3 size, double chamfer, const Mat4x4& transform);

		/// @brief 指定した UV マッピングを持つ、原点を中心とする面取り直方体を追加します。
		/// @param size 面取り直方体の各軸方向の大きさ
		/// @param chamfer 面取り幅
		/// @param uvMapping 各投影面に割り当てる UV 矩形
		/// @return 追加に成功した場合 true, それ以外の場合は false
		/// @remark 辺面と角面の投影軸が複数同率になる場合は、X 軸、Y 軸、Z 軸の順に優先します。
		bool addChamferedBox(Vec3 size, double chamfer, const BoxUVMapping& uvMapping);

		/// @brief 指定した UV マッピングを持つ、平行移動した面取り直方体を追加します。
		/// @param size 面取り直方体の各軸方向の大きさ
		/// @param chamfer 面取り幅
		/// @param uvMapping 各投影面に割り当てる UV 矩形
		/// @param offset 平行移動量
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addChamferedBox(Vec3 size, double chamfer, const BoxUVMapping& uvMapping, Vec3 offset);

		/// @brief 指定した UV マッピングを持つ、回転および平行移動した面取り直方体を追加します。
		/// @param size 面取り直方体の各軸方向の大きさ
		/// @param chamfer 面取り幅
		/// @param uvMapping 各投影面に割り当てる UV 矩形
		/// @param offset 平行移動量
		/// @param rotation 原点を中心とする回転を表す単位クォータニオン
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addChamferedBox(
			Vec3 size,
			double chamfer,
			const BoxUVMapping& uvMapping,
			Vec3 offset,
			const Quaternion& rotation);

		/// @brief 指定した UV マッピングを持つ、アフィン変換を適用した面取り直方体を追加します。
		/// @param size 面取り直方体の各軸方向の大きさ
		/// @param chamfer 面取り幅
		/// @param uvMapping 各投影面に割り当てる UV 矩形
		/// @param transform 適用するアフィン変換行列
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addChamferedBox(
			Vec3 size,
			double chamfer,
			const BoxUVMapping& uvMapping,
			const Mat4x4& transform);

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
		//	addExtrude
		//
		////////////////////////////////////////////////////////////////

		/// @brief 2D の多角形を Y 軸方向に押し出した形状を追加します。
		/// @param polygon 押し出す多角形。穴を含むことができます。
		/// @param height 押し出す高さ
		/// @return 追加に成功した場合 true, それ以外の場合は false
		/// @remark 座標、UV 座標、および法線の規約は `Mesh3D::Extrude()` と同じです。
		bool addExtrude(const Polygon& polygon, double height);

		/// @brief 平行移動した押し出し形状を追加します。
		/// @param polygon 押し出す多角形。穴を含むことができます。
		/// @param height 押し出す高さ
		/// @param offset 平行移動量
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addExtrude(const Polygon& polygon, double height, Vec3 offset);

		/// @brief 回転および平行移動した押し出し形状を追加します。
		/// @param polygon 押し出す多角形。穴を含むことができます。
		/// @param height 押し出す高さ
		/// @param offset 平行移動量
		/// @param rotation 原点を中心とする回転を表す単位クォータニオン
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addExtrude(const Polygon& polygon, double height, Vec3 offset, const Quaternion& rotation);

		/// @brief アフィン変換を適用した押し出し形状を追加します。
		/// @param polygon 押し出す多角形。穴を含むことができます。
		/// @param height 押し出す高さ
		/// @param transform 適用するアフィン変換行列
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addExtrude(const Polygon& polygon, double height, const Mat4x4& transform);

		/// @brief 2D の多角形を Y 軸方向に押し出し、側面の法線を角度に応じて補間した形状を追加します。
		/// @param polygon 押し出す多角形。穴を含むことができます。
		/// @param height 押し出す高さ
		/// @param smoothingAngle 側面の法線を補間する隣接面間の最大角度（ラジアン）。0 以上 π 以下
		/// @return 追加に成功した場合 true, それ以外の場合は false
		/// @remark 座標、UV 座標、および法線の規約は `Mesh3D::Extrude()` と同じです。
		bool addExtrude(const Polygon& polygon, double height, double smoothingAngle);

		/// @brief 平行移動し、側面の法線を角度に応じて補間した押し出し形状を追加します。
		/// @param polygon 押し出す多角形。穴を含むことができます。
		/// @param height 押し出す高さ
		/// @param smoothingAngle 側面の法線を補間する隣接面間の最大角度（ラジアン）。0 以上 π 以下
		/// @param offset 平行移動量
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addExtrude(const Polygon& polygon, double height, double smoothingAngle, Vec3 offset);

		/// @brief 回転および平行移動し、側面の法線を角度に応じて補間した押し出し形状を追加します。
		/// @param polygon 押し出す多角形。穴を含むことができます。
		/// @param height 押し出す高さ
		/// @param smoothingAngle 側面の法線を補間する隣接面間の最大角度（ラジアン）。0 以上 π 以下
		/// @param offset 平行移動量
		/// @param rotation 原点を中心とする回転を表す単位クォータニオン
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addExtrude(
			const Polygon& polygon,
			double height,
			double smoothingAngle,
			Vec3 offset,
			const Quaternion& rotation);

		/// @brief アフィン変換を適用し、側面の法線を角度に応じて補間した押し出し形状を追加します。
		/// @param polygon 押し出す多角形。穴を含むことができます。
		/// @param height 押し出す高さ
		/// @param smoothingAngle 側面の法線を補間する隣接面間の最大角度（ラジアン）。0 以上 π 以下
		/// @param transform 適用するアフィン変換行列
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addExtrude(
			const Polygon& polygon,
			double height,
			double smoothingAngle,
			const Mat4x4& transform);

		////////////////////////////////////////////////////////////////
		//
		//	addRevolve
		//
		////////////////////////////////////////////////////////////////

		/// @brief 2D プロファイルを Y 軸の周りに一周回転させた形状を追加します。
		/// @param profile 回転させるプロファイル
		/// @param segments 回転方向の分割数。3 以上である必要があります。
		/// @return 追加に成功した場合 true, それ以外の場合は false
		/// @remark 座標、開口、UV 座標、および法線の規約は `Mesh3D::Revolve()` と同じです。
		bool addRevolve(std::span<const Vec2> profile, uint32 segments = 32);

		/// @brief 平行移動した回転体を追加します。
		/// @param profile 回転させるプロファイル
		/// @param segments 回転方向の分割数
		/// @param offset 平行移動量
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addRevolve(std::span<const Vec2> profile, uint32 segments, Vec3 offset);

		/// @brief 回転および平行移動した回転体を追加します。
		/// @param profile 回転させるプロファイル
		/// @param segments 回転方向の分割数
		/// @param offset 平行移動量
		/// @param rotation 原点を中心とする回転を表す単位クォータニオン
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addRevolve(
			std::span<const Vec2> profile,
			uint32 segments,
			Vec3 offset,
			const Quaternion& rotation);

		/// @brief アフィン変換を適用した回転体を追加します。
		/// @param profile 回転させるプロファイル
		/// @param segments 回転方向の分割数
		/// @param transform 適用するアフィン変換行列
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addRevolve(
			std::span<const Vec2> profile,
			uint32 segments,
			const Mat4x4& transform);

		/// @brief 2D プロファイルを Y 軸の周りに一周回転させ、プロファイル方向の法線を角度に応じて補間した形状を追加します。
		/// @param profile 回転させるプロファイル
		/// @param segments 回転方向の分割数。3 以上である必要があります。
		/// @param smoothingAngle プロファイル方向の法線を補間する隣接面間の最大角度（ラジアン）。0 以上 π 以下
		/// @return 追加に成功した場合 true, それ以外の場合は false
		/// @remark 座標、開口、UV 座標、および法線の規約は `Mesh3D::Revolve()` と同じです。
		bool addRevolve(
			std::span<const Vec2> profile,
			uint32 segments,
			double smoothingAngle);

		/// @brief 平行移動し、プロファイル方向の法線を角度に応じて補間した回転体を追加します。
		/// @param profile 回転させるプロファイル
		/// @param segments 回転方向の分割数
		/// @param smoothingAngle プロファイル方向の法線を補間する隣接面間の最大角度（ラジアン）。0 以上 π 以下
		/// @param offset 平行移動量
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addRevolve(
			std::span<const Vec2> profile,
			uint32 segments,
			double smoothingAngle,
			Vec3 offset);

		/// @brief 回転および平行移動し、プロファイル方向の法線を角度に応じて補間した回転体を追加します。
		/// @param profile 回転させるプロファイル
		/// @param segments 回転方向の分割数
		/// @param smoothingAngle プロファイル方向の法線を補間する隣接面間の最大角度（ラジアン）。0 以上 π 以下
		/// @param offset 平行移動量
		/// @param rotation 原点を中心とする回転を表す単位クォータニオン
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addRevolve(
			std::span<const Vec2> profile,
			uint32 segments,
			double smoothingAngle,
			Vec3 offset,
			const Quaternion& rotation);

		/// @brief アフィン変換を適用し、プロファイル方向の法線を角度に応じて補間した回転体を追加します。
		/// @param profile 回転させるプロファイル
		/// @param segments 回転方向の分割数
		/// @param smoothingAngle プロファイル方向の法線を補間する隣接面間の最大角度（ラジアン）。0 以上 π 以下
		/// @param transform 適用するアフィン変換行列
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addRevolve(
			std::span<const Vec2> profile,
			uint32 segments,
			double smoothingAngle,
			const Mat4x4& transform);

		/// @brief 2D プロファイルを Y 軸の周りに指定した角度だけ回転させた形状を追加します。
		/// @param profile 回転させるプロファイル
		/// @param startAngle 回転を開始する角度（ラジアン）。0 は `+X` 方向です。
		/// @param sweepAngle `+Z` 方向へ回転する角度（ラジアン）。0 より大きく 2π 以下である必要があります。
		/// @param segments 回転方向の分割数。部分回転では 1 以上、完全な一周では 3 以上である必要があります。
		/// @param closeEnds 回転方向の始端と終端を閉じるかどうか
		/// @return 追加に成功した場合 true, それ以外の場合は false
		/// @remark 角度、端面、UV 座標、および法線の規約は角度を指定する `Mesh3D::Revolve()` と同じです。
		bool addRevolve(
			std::span<const Vec2> profile,
			double startAngle,
			double sweepAngle,
			uint32 segments,
			CloseEnds closeEnds = CloseEnds::No);

		/// @brief 平行移動した部分回転体を追加します。
		/// @param profile 回転させるプロファイル
		/// @param startAngle 回転を開始する角度（ラジアン）
		/// @param sweepAngle 回転する角度（ラジアン）
		/// @param segments 回転方向の分割数
		/// @param offset 平行移動量
		/// @param closeEnds 回転方向の始端と終端を閉じるかどうか
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addRevolve(
			std::span<const Vec2> profile,
			double startAngle,
			double sweepAngle,
			uint32 segments,
			Vec3 offset,
			CloseEnds closeEnds = CloseEnds::No);

		/// @brief 回転および平行移動した部分回転体を追加します。
		/// @param profile 回転させるプロファイル
		/// @param startAngle 回転を開始する角度（ラジアン）
		/// @param sweepAngle 回転する角度（ラジアン）
		/// @param segments 回転方向の分割数
		/// @param offset 平行移動量
		/// @param rotation 原点を中心とする回転を表す単位クォータニオン
		/// @param closeEnds 回転方向の始端と終端を閉じるかどうか
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addRevolve(
			std::span<const Vec2> profile,
			double startAngle,
			double sweepAngle,
			uint32 segments,
			Vec3 offset,
			const Quaternion& rotation,
			CloseEnds closeEnds = CloseEnds::No);

		/// @brief アフィン変換を適用した部分回転体を追加します。
		/// @param profile 回転させるプロファイル
		/// @param startAngle 回転を開始する角度（ラジアン）
		/// @param sweepAngle 回転する角度（ラジアン）
		/// @param segments 回転方向の分割数
		/// @param transform 適用するアフィン変換行列
		/// @param closeEnds 回転方向の始端と終端を閉じるかどうか
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addRevolve(
			std::span<const Vec2> profile,
			double startAngle,
			double sweepAngle,
			uint32 segments,
			const Mat4x4& transform,
			CloseEnds closeEnds = CloseEnds::No);

		/// @brief プロファイル方向の法線を補間した部分回転体を追加します。
		/// @param profile 回転させるプロファイル
		/// @param startAngle 回転を開始する角度（ラジアン）
		/// @param sweepAngle 回転する角度（ラジアン）
		/// @param segments 回転方向の分割数
		/// @param smoothingAngle プロファイル方向の法線を補間する隣接面間の最大角度（ラジアン）。0 以上 π 以下
		/// @param closeEnds 回転方向の始端と終端を閉じるかどうか
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addRevolve(
			std::span<const Vec2> profile,
			double startAngle,
			double sweepAngle,
			uint32 segments,
			double smoothingAngle,
			CloseEnds closeEnds = CloseEnds::No);

		/// @brief 平行移動し、プロファイル方向の法線を補間した部分回転体を追加します。
		/// @param profile 回転させるプロファイル
		/// @param startAngle 回転を開始する角度（ラジアン）
		/// @param sweepAngle 回転する角度（ラジアン）
		/// @param segments 回転方向の分割数
		/// @param smoothingAngle プロファイル方向の法線を補間する隣接面間の最大角度（ラジアン）
		/// @param offset 平行移動量
		/// @param closeEnds 回転方向の始端と終端を閉じるかどうか
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addRevolve(
			std::span<const Vec2> profile,
			double startAngle,
			double sweepAngle,
			uint32 segments,
			double smoothingAngle,
			Vec3 offset,
			CloseEnds closeEnds = CloseEnds::No);

		/// @brief 回転および平行移動し、プロファイル方向の法線を補間した部分回転体を追加します。
		/// @param profile 回転させるプロファイル
		/// @param startAngle 回転を開始する角度（ラジアン）
		/// @param sweepAngle 回転する角度（ラジアン）
		/// @param segments 回転方向の分割数
		/// @param smoothingAngle プロファイル方向の法線を補間する隣接面間の最大角度（ラジアン）
		/// @param offset 平行移動量
		/// @param rotation 原点を中心とする回転を表す単位クォータニオン
		/// @param closeEnds 回転方向の始端と終端を閉じるかどうか
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addRevolve(
			std::span<const Vec2> profile,
			double startAngle,
			double sweepAngle,
			uint32 segments,
			double smoothingAngle,
			Vec3 offset,
			const Quaternion& rotation,
			CloseEnds closeEnds = CloseEnds::No);

		/// @brief アフィン変換を適用し、プロファイル方向の法線を補間した部分回転体を追加します。
		/// @param profile 回転させるプロファイル
		/// @param startAngle 回転を開始する角度（ラジアン）
		/// @param sweepAngle 回転する角度（ラジアン）
		/// @param segments 回転方向の分割数
		/// @param smoothingAngle プロファイル方向の法線を補間する隣接面間の最大角度（ラジアン）
		/// @param transform 適用するアフィン変換行列
		/// @param closeEnds 回転方向の始端と終端を閉じるかどうか
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addRevolve(
			std::span<const Vec2> profile,
			double startAngle,
			double sweepAngle,
			uint32 segments,
			double smoothingAngle,
			const Mat4x4& transform,
			CloseEnds closeEnds = CloseEnds::No);

		////////////////////////////////////////////////////////////////
		//
		//	addTube
		//
		////////////////////////////////////////////////////////////////

		/// @brief 3D 経路に沿う一定半径のチューブを追加します。
		/// @param path チューブの中心を通る開いた経路。2 点以上である必要があります。
		/// @param radius チューブの半径
		/// @param sides チューブ断面の分割数。3 以上である必要があります。
		/// @param uvScale UV 座標の拡大率
		/// @param uvOffset UV 座標のオフセット
		/// @return 追加に成功した場合 true, それ以外の場合は false
		/// @remark 経路、端面、UV 座標、および法線の規約は `Mesh3D::Tube()` と同じです。
		bool addTube(
			std::span<const Vec3> path,
			double radius,
			uint32 sides = 12,
			Vec2 uvScale = Vec2{ 1.0, 1.0 },
			Vec2 uvOffset = Vec2{ 0.0, 0.0 });

		/// @brief 平行移動したチューブを追加します。
		/// @param path チューブの中心を通る開いた経路
		/// @param radius チューブの半径
		/// @param sides チューブ断面の分割数
		/// @param offset 平行移動量
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addTube(std::span<const Vec3> path, double radius, uint32 sides, Vec3 offset);

		/// @brief 回転および平行移動したチューブを追加します。
		/// @param path チューブの中心を通る開いた経路
		/// @param radius チューブの半径
		/// @param sides チューブ断面の分割数
		/// @param offset 平行移動量
		/// @param rotation 原点を中心とする回転を表す単位クォータニオン
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addTube(
			std::span<const Vec3> path,
			double radius,
			uint32 sides,
			Vec3 offset,
			const Quaternion& rotation);

		/// @brief アフィン変換を適用したチューブを追加します。
		/// @param path チューブの中心を通る開いた経路
		/// @param radius チューブの半径
		/// @param sides チューブ断面の分割数
		/// @param transform 適用するアフィン変換行列
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addTube(
			std::span<const Vec3> path,
			double radius,
			uint32 sides,
			const Mat4x4& transform);

		/// @brief UV 変換と平行移動を適用したチューブを追加します。
		/// @param path チューブの中心を通る開いた経路
		/// @param radius チューブの半径
		/// @param sides チューブ断面の分割数
		/// @param uvScale UV 座標の拡大率
		/// @param uvOffset UV 座標のオフセット
		/// @param offset 平行移動量
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addTube(
			std::span<const Vec3> path,
			double radius,
			uint32 sides,
			Vec2 uvScale,
			Vec2 uvOffset,
			Vec3 offset);

		/// @brief UV 変換、回転、および平行移動を適用したチューブを追加します。
		/// @param path チューブの中心を通る開いた経路
		/// @param radius チューブの半径
		/// @param sides チューブ断面の分割数
		/// @param uvScale UV 座標の拡大率
		/// @param uvOffset UV 座標のオフセット
		/// @param offset 平行移動量
		/// @param rotation 原点を中心とする回転を表す単位クォータニオン
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addTube(
			std::span<const Vec3> path,
			double radius,
			uint32 sides,
			Vec2 uvScale,
			Vec2 uvOffset,
			Vec3 offset,
			const Quaternion& rotation);

		/// @brief UV 変換とアフィン変換を適用したチューブを追加します。
		/// @param path チューブの中心を通る開いた経路
		/// @param radius チューブの半径
		/// @param sides チューブ断面の分割数
		/// @param uvScale UV 座標の拡大率
		/// @param uvOffset UV 座標のオフセット
		/// @param transform 適用するアフィン変換行列
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addTube(
			std::span<const Vec3> path,
			double radius,
			uint32 sides,
			Vec2 uvScale,
			Vec2 uvOffset,
			const Mat4x4& transform);

		////////////////////////////////////////////////////////////////
		//
		//	addSweep
		//
		////////////////////////////////////////////////////////////////

		/// @brief 2D 断面を 3D 経路に沿わせた形状を追加します。
		/// @param crossSection 経路に沿わせる断面。穴を含むことができます。
		/// @param path 断面の中心を通る開いた経路。2 点以上である必要があります。
		/// @param uvScale UV 座標の拡大率
		/// @param uvOffset UV 座標のオフセット
		/// @return 追加に成功した場合 true, それ以外の場合は false
		/// @remark 断面、経路、端面、UV 座標、および法線の規約は `Mesh3D::Sweep()` と同じです。
		bool addSweep(
			const Polygon& crossSection,
			std::span<const Vec3> path,
			Vec2 uvScale = Vec2{ 1.0, 1.0 },
			Vec2 uvOffset = Vec2{ 0.0, 0.0 });

		/// @brief 平行移動した Sweep 形状を追加します。
		/// @param crossSection 経路に沿わせる断面
		/// @param path 断面の中心を通る開いた経路
		/// @param offset 平行移動量
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addSweep(const Polygon& crossSection, std::span<const Vec3> path, Vec3 offset);

		/// @brief 回転および平行移動した Sweep 形状を追加します。
		/// @param crossSection 経路に沿わせる断面
		/// @param path 断面の中心を通る開いた経路
		/// @param offset 平行移動量
		/// @param rotation 原点を中心とする回転を表す単位クォータニオン
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addSweep(
			const Polygon& crossSection,
			std::span<const Vec3> path,
			Vec3 offset,
			const Quaternion& rotation);

		/// @brief アフィン変換を適用した Sweep 形状を追加します。
		/// @param crossSection 経路に沿わせる断面
		/// @param path 断面の中心を通る開いた経路
		/// @param transform 適用するアフィン変換行列
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addSweep(
			const Polygon& crossSection,
			std::span<const Vec3> path,
			const Mat4x4& transform);

		/// @brief UV 変換と平行移動を適用した Sweep 形状を追加します。
		/// @param crossSection 経路に沿わせる断面
		/// @param path 断面の中心を通る開いた経路
		/// @param uvScale UV 座標の拡大率
		/// @param uvOffset UV 座標のオフセット
		/// @param offset 平行移動量
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addSweep(
			const Polygon& crossSection,
			std::span<const Vec3> path,
			Vec2 uvScale,
			Vec2 uvOffset,
			Vec3 offset);

		/// @brief UV 変換、回転、および平行移動を適用した Sweep 形状を追加します。
		/// @param crossSection 経路に沿わせる断面
		/// @param path 断面の中心を通る開いた経路
		/// @param uvScale UV 座標の拡大率
		/// @param uvOffset UV 座標のオフセット
		/// @param offset 平行移動量
		/// @param rotation 原点を中心とする回転を表す単位クォータニオン
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addSweep(
			const Polygon& crossSection,
			std::span<const Vec3> path,
			Vec2 uvScale,
			Vec2 uvOffset,
			Vec3 offset,
			const Quaternion& rotation);

		/// @brief UV 変換とアフィン変換を適用した Sweep 形状を追加します。
		/// @param crossSection 経路に沿わせる断面
		/// @param path 断面の中心を通る開いた経路
		/// @param uvScale UV 座標の拡大率
		/// @param uvOffset UV 座標のオフセット
		/// @param transform 適用するアフィン変換行列
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addSweep(
			const Polygon& crossSection,
			std::span<const Vec3> path,
			Vec2 uvScale,
			Vec2 uvOffset,
			const Mat4x4& transform);

		/// @brief 開始時の断面の X 軸方向を指定して Sweep 形状を追加します。
		/// @param crossSection 経路に沿わせる断面。穴を含むことができます。
		/// @param path 断面の中心を通る開いた経路。2 点以上である必要があります。
		/// @param initialXAxis 開始時に断面の X 軸を向ける方向
		/// @param uvScale UV 座標の拡大率
		/// @param uvOffset UV 座標のオフセット
		/// @return 追加に成功した場合 true, それ以外の場合は false
		/// @remark `initialXAxis` は最初の経路方向に垂直な平面へ投影して使用します。
		bool addSweep(
			const Polygon& crossSection,
			std::span<const Vec3> path,
			Arg::initialXAxis_<Vec3> initialXAxis,
			Vec2 uvScale = Vec2{ 1.0, 1.0 },
			Vec2 uvOffset = Vec2{ 0.0, 0.0 });

		/// @brief 初期 X 軸方向を指定し、平行移動した Sweep 形状を追加します。
		/// @param crossSection 経路に沿わせる断面
		/// @param path 断面の中心を通る開いた経路
		/// @param initialXAxis 開始時に断面の X 軸を向ける方向
		/// @param offset 平行移動量
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addSweep(
			const Polygon& crossSection,
			std::span<const Vec3> path,
			Arg::initialXAxis_<Vec3> initialXAxis,
			Vec3 offset);

		/// @brief 初期 X 軸方向を指定し、回転および平行移動した Sweep 形状を追加します。
		/// @param crossSection 経路に沿わせる断面
		/// @param path 断面の中心を通る開いた経路
		/// @param initialXAxis 開始時に断面の X 軸を向ける方向
		/// @param offset 平行移動量
		/// @param rotation 原点を中心とする回転を表す単位クォータニオン
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addSweep(
			const Polygon& crossSection,
			std::span<const Vec3> path,
			Arg::initialXAxis_<Vec3> initialXAxis,
			Vec3 offset,
			const Quaternion& rotation);

		/// @brief 初期 X 軸方向を指定し、アフィン変換を適用した Sweep 形状を追加します。
		/// @param crossSection 経路に沿わせる断面
		/// @param path 断面の中心を通る開いた経路
		/// @param initialXAxis 開始時に断面の X 軸を向ける方向
		/// @param transform 適用するアフィン変換行列
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addSweep(
			const Polygon& crossSection,
			std::span<const Vec3> path,
			Arg::initialXAxis_<Vec3> initialXAxis,
			const Mat4x4& transform);

		/// @brief 初期 X 軸方向、UV 変換、および平行移動を指定して Sweep 形状を追加します。
		/// @param crossSection 経路に沿わせる断面
		/// @param path 断面の中心を通る開いた経路
		/// @param initialXAxis 開始時に断面の X 軸を向ける方向
		/// @param uvScale UV 座標の拡大率
		/// @param uvOffset UV 座標のオフセット
		/// @param offset 平行移動量
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addSweep(
			const Polygon& crossSection,
			std::span<const Vec3> path,
			Arg::initialXAxis_<Vec3> initialXAxis,
			Vec2 uvScale,
			Vec2 uvOffset,
			Vec3 offset);

		/// @brief 初期 X 軸方向、UV 変換、回転、および平行移動を指定して Sweep 形状を追加します。
		/// @param crossSection 経路に沿わせる断面
		/// @param path 断面の中心を通る開いた経路
		/// @param initialXAxis 開始時に断面の X 軸を向ける方向
		/// @param uvScale UV 座標の拡大率
		/// @param uvOffset UV 座標のオフセット
		/// @param offset 平行移動量
		/// @param rotation 原点を中心とする回転を表す単位クォータニオン
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addSweep(
			const Polygon& crossSection,
			std::span<const Vec3> path,
			Arg::initialXAxis_<Vec3> initialXAxis,
			Vec2 uvScale,
			Vec2 uvOffset,
			Vec3 offset,
			const Quaternion& rotation);

		/// @brief 初期 X 軸方向、UV 変換、およびアフィン変換を指定して Sweep 形状を追加します。
		/// @param crossSection 経路に沿わせる断面
		/// @param path 断面の中心を通る開いた経路
		/// @param initialXAxis 開始時に断面の X 軸を向ける方向
		/// @param uvScale UV 座標の拡大率
		/// @param uvOffset UV 座標のオフセット
		/// @param transform 適用するアフィン変換行列
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addSweep(
			const Polygon& crossSection,
			std::span<const Vec3> path,
			Arg::initialXAxis_<Vec3> initialXAxis,
			Vec2 uvScale,
			Vec2 uvOffset,
			const Mat4x4& transform);

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

		////////////////////////////////////////////////////////////////
		//
		//	addHollowCylinder
		//
		////////////////////////////////////////////////////////////////

		/// @brief Y 軸方向に伸びる、両端が閉じた中空円柱を追加します。
		/// @param innerRadius 内半径
		/// @param outerRadius 外半径。`innerRadius` より大きい必要があります。
		/// @param height 高さ
		/// @param segments 円周の分割数。3 以上である必要があります。
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addHollowCylinder(double innerRadius, double outerRadius, double height, uint32 segments = 32);

		/// @brief 平行移動した中空円柱を追加します。
		/// @param innerRadius 内半径
		/// @param outerRadius 外半径
		/// @param height 高さ
		/// @param segments 円周の分割数
		/// @param offset 平行移動量
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addHollowCylinder(double innerRadius, double outerRadius, double height, uint32 segments, Vec3 offset);

		/// @brief 回転および平行移動した中空円柱を追加します。
		/// @param innerRadius 内半径
		/// @param outerRadius 外半径
		/// @param height 高さ
		/// @param segments 円周の分割数
		/// @param offset 平行移動量
		/// @param rotation 原点を中心とする回転を表す単位クォータニオン
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addHollowCylinder(double innerRadius, double outerRadius, double height, uint32 segments, Vec3 offset, const Quaternion& rotation);

		/// @brief アフィン変換を適用した中空円柱を追加します。
		/// @param innerRadius 内半径
		/// @param outerRadius 外半径
		/// @param height 高さ
		/// @param segments 円周の分割数
		/// @param transform 適用するアフィン変換行列
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addHollowCylinder(double innerRadius, double outerRadius, double height, uint32 segments, const Mat4x4& transform);

		////////////////////////////////////////////////////////////////
		//
		//	addConicalFrustum
		//
		////////////////////////////////////////////////////////////////

		/// @brief Y 軸方向に伸びる、両端が閉じた円錐台を追加します。
		/// @param bottomRadius 底面の半径
		/// @param topRadius 上面の半径。0 の場合は円錐を生成します。
		/// @param height 高さ
		/// @param segments 円周の分割数。3 以上である必要があります。
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addConicalFrustum(double bottomRadius, double topRadius, double height, uint32 segments = 32);

		/// @brief 平行移動した円錐台を追加します。
		/// @param bottomRadius 底面の半径
		/// @param topRadius 上面の半径
		/// @param height 高さ
		/// @param segments 円周の分割数
		/// @param offset 平行移動量
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addConicalFrustum(double bottomRadius, double topRadius, double height, uint32 segments, Vec3 offset);

		/// @brief 回転および平行移動した円錐台を追加します。
		/// @param bottomRadius 底面の半径
		/// @param topRadius 上面の半径
		/// @param height 高さ
		/// @param segments 円周の分割数
		/// @param offset 平行移動量
		/// @param rotation 原点を中心とする回転を表す単位クォータニオン
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addConicalFrustum(double bottomRadius, double topRadius, double height, uint32 segments, Vec3 offset, const Quaternion& rotation);

		/// @brief アフィン変換を適用した円錐台を追加します。
		/// @param bottomRadius 底面の半径
		/// @param topRadius 上面の半径
		/// @param height 高さ
		/// @param segments 円周の分割数
		/// @param transform 適用するアフィン変換行列
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addConicalFrustum(double bottomRadius, double topRadius, double height, uint32 segments, const Mat4x4& transform);

		////////////////////////////////////////////////////////////////
		//
		//	addCylinder
		//
		////////////////////////////////////////////////////////////////

		/// @brief Y 軸方向に伸びる、両端が閉じた円柱を追加します。
		/// @param radius 円柱の半径
		/// @param height 円柱の高さ
		/// @param segments 円周の分割数。3 以上である必要があります。
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addCylinder(double radius, double height, uint32 segments = 32);

		/// @brief 平行移動した円柱を追加します。
		/// @param radius 円柱の半径
		/// @param height 円柱の高さ
		/// @param segments 円周の分割数
		/// @param offset 平行移動量
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addCylinder(double radius, double height, uint32 segments, Vec3 offset);

		/// @brief 回転および平行移動した円柱を追加します。
		/// @param radius 円柱の半径
		/// @param height 円柱の高さ
		/// @param segments 円周の分割数
		/// @param offset 平行移動量
		/// @param rotation 原点を中心とする回転を表す単位クォータニオン
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addCylinder(double radius, double height, uint32 segments, Vec3 offset, const Quaternion& rotation);

		/// @brief アフィン変換を適用した円柱を追加します。
		/// @param radius 円柱の半径
		/// @param height 円柱の高さ
		/// @param segments 円周の分割数
		/// @param transform 適用するアフィン変換行列
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addCylinder(double radius, double height, uint32 segments, const Mat4x4& transform);

		////////////////////////////////////////////////////////////////
		//
		//	addCone
		//
		////////////////////////////////////////////////////////////////

		/// @brief Y 軸方向に伸びる、底面が閉じた円錐を追加します。
		/// @param radius 底面の半径
		/// @param height 円錐の高さ
		/// @param segments 円周の分割数。3 以上である必要があります。
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addCone(double radius, double height, uint32 segments = 32);

		/// @brief 平行移動した円錐を追加します。
		/// @param radius 底面の半径
		/// @param height 円錐の高さ
		/// @param segments 円周の分割数
		/// @param offset 平行移動量
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addCone(double radius, double height, uint32 segments, Vec3 offset);

		/// @brief 回転および平行移動した円錐を追加します。
		/// @param radius 底面の半径
		/// @param height 円錐の高さ
		/// @param segments 円周の分割数
		/// @param offset 平行移動量
		/// @param rotation 原点を中心とする回転を表す単位クォータニオン
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addCone(double radius, double height, uint32 segments, Vec3 offset, const Quaternion& rotation);

		/// @brief アフィン変換を適用した円錐を追加します。
		/// @param radius 底面の半径
		/// @param height 円錐の高さ
		/// @param segments 円周の分割数
		/// @param transform 適用するアフィン変換行列
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addCone(double radius, double height, uint32 segments, const Mat4x4& transform);

		////////////////////////////////////////////////////////////////
		//
		//	addTorus
		//
		////////////////////////////////////////////////////////////////

		/// @brief Y 軸を回転軸とするトーラスを追加します。
		/// @param majorRadius 原点からチューブ断面の中心までの半径
		/// @param tubeRadius チューブ断面の半径。`majorRadius` より小さい必要があります。
		/// @param ringSegments リング方向の分割数。3 以上である必要があります。
		/// @param tubeSegments チューブ断面方向の分割数。3 以上である必要があります。
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addTorus(double majorRadius, double tubeRadius, uint32 ringSegments = 32, uint32 tubeSegments = 16);

		/// @brief 平行移動したトーラスを追加します。
		/// @param majorRadius 原点からチューブ断面の中心までの半径
		/// @param tubeRadius チューブ断面の半径
		/// @param ringSegments リング方向の分割数
		/// @param tubeSegments チューブ断面方向の分割数
		/// @param offset 平行移動量
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addTorus(double majorRadius, double tubeRadius, uint32 ringSegments, uint32 tubeSegments, Vec3 offset);

		/// @brief 回転および平行移動したトーラスを追加します。
		/// @param majorRadius 原点からチューブ断面の中心までの半径
		/// @param tubeRadius チューブ断面の半径
		/// @param ringSegments リング方向の分割数
		/// @param tubeSegments チューブ断面方向の分割数
		/// @param offset 平行移動量
		/// @param rotation 原点を中心とする回転を表す単位クォータニオン
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addTorus(double majorRadius, double tubeRadius, uint32 ringSegments, uint32 tubeSegments, Vec3 offset, const Quaternion& rotation);

		/// @brief アフィン変換を適用したトーラスを追加します。
		/// @param majorRadius 原点からチューブ断面の中心までの半径
		/// @param tubeRadius チューブ断面の半径
		/// @param ringSegments リング方向の分割数
		/// @param tubeSegments チューブ断面方向の分割数
		/// @param transform 適用するアフィン変換行列
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addTorus(double majorRadius, double tubeRadius, uint32 ringSegments, uint32 tubeSegments, const Mat4x4& transform);

		////////////////////////////////////////////////////////////////
		//
		//	addSphere
		//
		////////////////////////////////////////////////////////////////

		/// @brief 原点を中心とする UV 球を追加します。
		/// @param radius 球の半径
		/// @param slices 経度方向の分割数。3 以上である必要があります。
		/// @param stacks 緯度方向の分割数。2 以上である必要があります。
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addSphere(double radius, uint32 slices = 32, uint32 stacks = 16);

		/// @brief 平行移動した UV 球を追加します。
		/// @param radius 球の半径
		/// @param slices 経度方向の分割数
		/// @param stacks 緯度方向の分割数
		/// @param offset 平行移動量
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addSphere(double radius, uint32 slices, uint32 stacks, Vec3 offset);

		/// @brief 回転および平行移動した UV 球を追加します。
		/// @param radius 球の半径
		/// @param slices 経度方向の分割数
		/// @param stacks 緯度方向の分割数
		/// @param offset 平行移動量
		/// @param rotation 原点を中心とする回転を表す単位クォータニオン
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addSphere(double radius, uint32 slices, uint32 stacks, Vec3 offset, const Quaternion& rotation);

		/// @brief アフィン変換を適用した UV 球を追加します。
		/// @param radius 球の半径
		/// @param slices 経度方向の分割数
		/// @param stacks 緯度方向の分割数
		/// @param transform 適用するアフィン変換行列
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addSphere(double radius, uint32 slices, uint32 stacks, const Mat4x4& transform);

		////////////////////////////////////////////////////////////////
		//
		//	addHemisphere
		//
		////////////////////////////////////////////////////////////////

		/// @brief Y 軸の正方向を向く、底面のない半球を追加します。
		/// @param radius 半球の半径
		/// @param slices 経度方向の分割数。3 以上である必要があります。
		/// @param stacks 緯度方向の分割数。1 以上である必要があります。
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addHemisphere(double radius, uint32 slices = 32, uint32 stacks = 8);

		/// @brief 平行移動した、底面のない半球を追加します。
		/// @param radius 半球の半径
		/// @param slices 経度方向の分割数
		/// @param stacks 緯度方向の分割数
		/// @param offset 平行移動量
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addHemisphere(double radius, uint32 slices, uint32 stacks, Vec3 offset);

		/// @brief 回転および平行移動した、底面のない半球を追加します。
		/// @param radius 半球の半径
		/// @param slices 経度方向の分割数
		/// @param stacks 緯度方向の分割数
		/// @param offset 平行移動量
		/// @param rotation 原点を中心とする回転を表す単位クォータニオン
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addHemisphere(double radius, uint32 slices, uint32 stacks, Vec3 offset, const Quaternion& rotation);

		/// @brief アフィン変換を適用した、底面のない半球を追加します。
		/// @param radius 半球の半径
		/// @param slices 経度方向の分割数
		/// @param stacks 緯度方向の分割数
		/// @param transform 適用するアフィン変換行列
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addHemisphere(double radius, uint32 slices, uint32 stacks, const Mat4x4& transform);

		/// @brief Y 軸の正方向を向く半球を追加します。
		/// @param radius 半球の半径
		/// @param closeBottom 底面を閉じる場合は `CloseBottom::Yes`、底面を作成しない場合は `CloseBottom::No`
		/// @param slices 経度方向の分割数。3 以上である必要があります。
		/// @param stacks 緯度方向の分割数。1 以上である必要があります。
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addHemisphere(double radius, CloseBottom closeBottom, uint32 slices = 32, uint32 stacks = 8);

		/// @brief 平行移動した半球を追加します。
		/// @param radius 半球の半径
		/// @param closeBottom 底面を閉じるか
		/// @param slices 経度方向の分割数
		/// @param stacks 緯度方向の分割数
		/// @param offset 平行移動量
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addHemisphere(double radius, CloseBottom closeBottom, uint32 slices, uint32 stacks, Vec3 offset);

		/// @brief 回転および平行移動した半球を追加します。
		/// @param radius 半球の半径
		/// @param closeBottom 底面を閉じるか
		/// @param slices 経度方向の分割数
		/// @param stacks 緯度方向の分割数
		/// @param offset 平行移動量
		/// @param rotation 原点を中心とする回転を表す単位クォータニオン
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addHemisphere(double radius, CloseBottom closeBottom, uint32 slices, uint32 stacks, Vec3 offset, const Quaternion& rotation);

		/// @brief アフィン変換を適用した半球を追加します。
		/// @param radius 半球の半径
		/// @param closeBottom 底面を閉じるか
		/// @param slices 経度方向の分割数
		/// @param stacks 緯度方向の分割数
		/// @param transform 適用するアフィン変換行列
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addHemisphere(double radius, CloseBottom closeBottom, uint32 slices, uint32 stacks, const Mat4x4& transform);

		////////////////////////////////////////////////////////////////
		//
		//	addCapsule
		//
		////////////////////////////////////////////////////////////////

		/// @brief Y 軸方向に伸びるカプセルを追加します。
		/// @param radius カプセルの半径
		/// @param cylinderHeight 2 つの半球の間にある円柱部分の高さ。0 以上である必要があります。
		/// @param slices 円周方向の分割数。3 以上である必要があります。
		/// @param hemisphereStacks 各半球の緯度方向の分割数。1 以上である必要があります。
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addCapsule(double radius, double cylinderHeight, uint32 slices = 32, uint32 hemisphereStacks = 8);

		/// @brief 平行移動したカプセルを追加します。
		/// @param radius カプセルの半径
		/// @param cylinderHeight 円柱部分の高さ
		/// @param slices 円周方向の分割数
		/// @param hemisphereStacks 各半球の緯度方向の分割数
		/// @param offset 平行移動量
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addCapsule(double radius, double cylinderHeight, uint32 slices, uint32 hemisphereStacks, Vec3 offset);

		/// @brief 回転および平行移動したカプセルを追加します。
		/// @param radius カプセルの半径
		/// @param cylinderHeight 円柱部分の高さ
		/// @param slices 円周方向の分割数
		/// @param hemisphereStacks 各半球の緯度方向の分割数
		/// @param offset 平行移動量
		/// @param rotation 原点を中心とする回転を表す単位クォータニオン
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addCapsule(double radius, double cylinderHeight, uint32 slices, uint32 hemisphereStacks, Vec3 offset, const Quaternion& rotation);

		/// @brief アフィン変換を適用したカプセルを追加します。
		/// @param radius カプセルの半径
		/// @param cylinderHeight 円柱部分の高さ
		/// @param slices 円周方向の分割数
		/// @param hemisphereStacks 各半球の緯度方向の分割数
		/// @param transform 適用するアフィン変換行列
		/// @return 追加に成功した場合 true, それ以外の場合は false
		bool addCapsule(double radius, double cylinderHeight, uint32 slices, uint32 hemisphereStacks, const Mat4x4& transform);

	private:

		Mesh3D m_mesh;
	};
}

# include "detail/Mesh3DBuilder.ipp"
