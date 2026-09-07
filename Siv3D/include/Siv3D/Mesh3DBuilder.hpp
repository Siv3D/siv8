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
# include "Mesh3DPlacement.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	Mesh3DBuilder
	//
	////////////////////////////////////////////////////////////////

	/// @brief 複数の 3D 形状を 1 つの Mesh3D に直接生成するビルダー
	/// @remark 各形状生成関数は一時的な Mesh3D を作成せず、ビルダーが所有するメッシュへ頂点と三角形を追加します。
	/// @remark `addMesh()` は、呼び出し側が用意した Mesh3D の内容をビルダーが所有するメッシュへコピーします。
	/// @remark 各 add 関数は Mesh3DAddResult を返します。成功時は追加された範囲、失敗時は分類済みのエラーを取得できます。
	/// @remark add 関数が失敗した場合、Fail レベルのエンジンログへ理由を出力し、既存のメッシュ内容は変更されません。何も追加しない有効な操作は空範囲として成功します。
	/// @remark 配置付き add 関数は Mesh3D::transform() と同じ規約で変換し、鏡映時は追加範囲の三角形の巻き順も反転して表裏を維持します。
	/// @remark 各 add 関数が生成する形状の座標、位相、UV 座標、法線、および接線の規約は、対応する `Mesh3D` の生成関数と同じです。
	/// @code
	/// Mesh3DBuilder builder;
	/// if (const auto result = builder.addTube(path, 0.25))
	/// {
	///     const Mesh3DRange added = *result;
	///     Mesh3D mesh = std::move(builder).build(); // 所有ストレージを取り出す
	/// }
	/// else
	/// {
	///     const Mesh3DError& error = result.error();
	///     Console << error.message; // 生成に失敗した理由
	/// }
	/// @endcode
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
		/// @remark 所有権を渡すため、名前のある変数からは `std::move(builder).build()` と呼びます。構築中の内容を参照するだけなら getMesh() を使います。
		[[nodiscard]]
		Mesh3D build() && noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	addMesh
		//
		////////////////////////////////////////////////////////////////

		/// @brief 既存の 3D メッシュを追加します。
		/// @param mesh 追加する 3D メッシュ
		/// @return 成功時は追加された範囲、失敗時はエラー
		/// @remark 空または不正なインデックスを持つ `mesh` は InvalidGeometry、追加後の頂点数が `Mesh3D::MaxVertexCount` を超える場合は SizeLimit で失敗します。
		/// @remark 失敗した場合、既存のメッシュ内容は変更されません。
		[[nodiscard]]
		Mesh3DAddResult addMesh(const Mesh3D& mesh);

		/// @brief 配置変換を適用した既存の 3D メッシュを追加します。
		/// @param mesh 追加する 3D メッシュ
		/// @param placement 適用する配置変換。`Vec3` または `Mat4x4` も直接指定できます。
		/// @return 成功時は追加された範囲、失敗時はエラー
		/// @remark 変換時の法線、接線、および巻き順の規約は、行列を受け取る `Mesh3D::append()` と同じです。
		[[nodiscard]]
		Mesh3DAddResult addMesh(const Mesh3D& mesh, const Mesh3DPlacement& placement);

		////////////////////////////////////////////////////////////////
		//
		//	addBox
		//
		////////////////////////////////////////////////////////////////

		/// @brief 原点を中心とする直方体を追加します。
		/// @param size 直方体の各軸方向の大きさ
		/// @param faces 生成する面
		/// @return 成功時は追加された範囲、失敗時はエラー
		/// @remark `faces == BoxFace::None_` の場合、メッシュを変更せず空の範囲を返します。
		[[nodiscard]]
		Mesh3DAddResult addBox(
			Vec3 size = Vec3{ 1.0, 1.0, 1.0 },
			BoxFace faces = BoxFace::All);

		/// @brief 配置変換を適用した直方体を追加します。
		/// @param size 直方体の各軸方向の大きさ
		/// @param placement 適用する配置変換。`Vec3` または `Mat4x4` も直接指定できます。
		/// @param faces 生成する面
		/// @return 成功時は追加された範囲、失敗時はエラー
		[[nodiscard]]
		Mesh3DAddResult addBox(Vec3 size, const Mesh3DPlacement& placement, BoxFace faces = BoxFace::All);

		/// @brief 指定した UV マッピングを持つ、原点を中心とする直方体を追加します。
		/// @param size 直方体の各軸方向の大きさ
		/// @param uvMapping 各面に割り当てる UV 矩形
		/// @param faces 生成する面
		/// @return 成功時は追加された範囲、失敗時はエラー
		/// @remark `uvMapping` は `faces` で選択した面に対応する矩形のみ検証されます。
		/// @remark `faces == BoxFace::None_` の場合、メッシュを変更せず空の範囲を返します。
		[[nodiscard]]
		Mesh3DAddResult addBox(Vec3 size, const BoxUVMapping& uvMapping, BoxFace faces = BoxFace::All);

		/// @brief 指定した UV マッピングを持つ、配置変換を適用した直方体を追加します。
		/// @param size 直方体の各軸方向の大きさ
		/// @param uvMapping 各面に割り当てる UV 矩形
		/// @param placement 適用する配置変換。`Vec3` または `Mat4x4` も直接指定できます。
		/// @param faces 生成する面
		/// @return 成功時は追加された範囲、失敗時はエラー
		[[nodiscard]]
		Mesh3DAddResult addBox(Vec3 size, const BoxUVMapping& uvMapping, const Mesh3DPlacement& placement, BoxFace faces = BoxFace::All);

		////////////////////////////////////////////////////////////////
		//
		//	addBoxShell
		//
		////////////////////////////////////////////////////////////////

		/// @brief 原点を中心とする、均一な厚みを持つ中空直方体を追加します。
		/// @param outerSize 外側の直方体の各軸方向の大きさ
		/// @param thickness 壁の厚み。正の有限値で、`outerSize` の最小成分の半分未満である必要があります。
		/// @param openFaces 壁を生成しない開口面
		/// @return 成功時は追加された範囲、失敗時はエラー
		/// @remark `openFaces == BoxFace::All` の場合、メッシュを変更せず空の範囲を返します。
		[[nodiscard]]
		Mesh3DAddResult addBoxShell(Vec3 outerSize = Vec3{ 1.0, 1.0, 1.0 }, double thickness = 0.1, BoxFace openFaces = BoxFace::None_);

		/// @brief 配置変換を適用した、均一な厚みの中空直方体を追加します。
		/// @param outerSize 外側の直方体の各軸方向の大きさ
		/// @param thickness 壁の厚み
		/// @param placement 適用する配置変換。`Vec3` または `Mat4x4` も直接指定できます。
		/// @param openFaces 壁を生成しない開口面
		/// @return 成功時は追加された範囲、失敗時はエラー
		[[nodiscard]]
		Mesh3DAddResult addBoxShell(Vec3 outerSize, double thickness, const Mesh3DPlacement& placement, BoxFace openFaces = BoxFace::None_);

		/// @brief 原点を中心とする、軸ごとの厚みを持つ中空直方体を追加します。
		/// @param outerSize 外側の直方体の各軸方向の大きさ
		/// @param thickness 各軸に垂直な壁の厚み。各成分は正の有限値で、対応する `outerSize` 成分の半分未満である必要があります。
		/// @param openFaces 壁を生成しない開口面
		/// @return 成功時は追加された範囲、失敗時はエラー
		/// @remark `openFaces == BoxFace::All` の場合、メッシュを変更せず空の範囲を返します。
		[[nodiscard]]
		Mesh3DAddResult addBoxShell(Vec3 outerSize, Vec3 thickness, BoxFace openFaces = BoxFace::None_);

		/// @brief 配置変換を適用した、軸ごとの厚みの中空直方体を追加します。
		/// @param outerSize 外側の直方体の各軸方向の大きさ
		/// @param thickness 各軸に垂直な壁の厚み
		/// @param placement 適用する配置変換。`Vec3` または `Mat4x4` も直接指定できます。
		/// @param openFaces 壁を生成しない開口面
		/// @return 成功時は追加された範囲、失敗時はエラー
		[[nodiscard]]
		Mesh3DAddResult addBoxShell(Vec3 outerSize, Vec3 thickness, const Mesh3DPlacement& placement, BoxFace openFaces = BoxFace::None_);

		/// @brief 指定した UV マッピングを持つ、均一な厚みの中空直方体を追加します。
		/// @param outerSize 外側の直方体の各軸方向の大きさ
		/// @param thickness 壁の厚み
		/// @param uvMapping 外接 Box の各投影面に割り当てる UV 矩形
		/// @param openFaces 壁を生成しない開口面
		/// @return 成功時は追加された範囲、失敗時はエラー
		[[nodiscard]]
		Mesh3DAddResult addBoxShell(Vec3 outerSize, double thickness, const BoxUVMapping& uvMapping, BoxFace openFaces = BoxFace::None_);

		/// @brief 指定した UV マッピングを持つ、配置変換を適用した均一な厚みの中空直方体を追加します。
		/// @param outerSize 外側の直方体の各軸方向の大きさ
		/// @param thickness 壁の厚み
		/// @param uvMapping 外接 Box の各投影面に割り当てる UV 矩形
		/// @param placement 適用する配置変換。`Vec3` または `Mat4x4` も直接指定できます。
		/// @param openFaces 壁を生成しない開口面
		/// @return 成功時は追加された範囲、失敗時はエラー
		[[nodiscard]]
		Mesh3DAddResult addBoxShell(Vec3 outerSize, double thickness, const BoxUVMapping& uvMapping, const Mesh3DPlacement& placement, BoxFace openFaces = BoxFace::None_);

		/// @brief 指定した UV マッピングを持つ、軸ごとの厚みの中空直方体を追加します。
		/// @param outerSize 外側の直方体の各軸方向の大きさ
		/// @param thickness 各軸に垂直な壁の厚み
		/// @param uvMapping 外接 Box の各投影面に割り当てる UV 矩形
		/// @param openFaces 壁を生成しない開口面
		/// @return 成功時は追加された範囲、失敗時はエラー
		[[nodiscard]]
		Mesh3DAddResult addBoxShell(Vec3 outerSize, Vec3 thickness, const BoxUVMapping& uvMapping, BoxFace openFaces = BoxFace::None_);

		/// @brief 指定した UV マッピングを持つ、配置変換を適用した軸ごとの厚みの中空直方体を追加します。
		/// @param outerSize 外側の直方体の各軸方向の大きさ
		/// @param thickness 各軸に垂直な壁の厚み
		/// @param uvMapping 外接 Box の各投影面に割り当てる UV 矩形
		/// @param placement 適用する配置変換。`Vec3` または `Mat4x4` も直接指定できます。
		/// @param openFaces 壁を生成しない開口面
		/// @return 成功時は追加された範囲、失敗時はエラー
		[[nodiscard]]
		Mesh3DAddResult addBoxShell(Vec3 outerSize, Vec3 thickness, const BoxUVMapping& uvMapping, const Mesh3DPlacement& placement, BoxFace openFaces = BoxFace::None_);

		////////////////////////////////////////////////////////////////
		//
		//	addBoxFrame
		//
		////////////////////////////////////////////////////////////////

		/// @brief 原点を中心とする、均一な太さの直方体枠を追加します。
		/// @param size 枠の外側の各軸方向の大きさ
		/// @param thickness 角材の太さ。正の有限値で、`size` の最小成分の半分未満である必要があります。
		/// @return 成功時は追加された範囲、失敗時はエラー
		[[nodiscard]]
		Mesh3DAddResult addBoxFrame(Vec3 size = Vec3{ 1.0, 1.0, 1.0 }, double thickness = 0.1);

		/// @brief 配置変換を適用した、均一な太さの直方体枠を追加します。
		/// @param size 枠の外側の各軸方向の大きさ
		/// @param thickness 角材の太さ
		/// @param placement 適用する配置変換。`Vec3` または `Mat4x4` も直接指定できます。
		/// @return 成功時は追加された範囲、失敗時はエラー
		[[nodiscard]]
		Mesh3DAddResult addBoxFrame(Vec3 size, double thickness, const Mesh3DPlacement& placement);

		/// @brief 原点を中心とする、軸ごとの太さを持つ直方体枠を追加します。
		/// @param size 枠の外側の各軸方向の大きさ
		/// @param beamSize 角材の軸ごとの太さ。各成分は正の有限値で、対応する `size` 成分の半分未満である必要があります。
		/// @return 成功時は追加された範囲、失敗時はエラー
		/// @remark X 方向の角材は `(size.x, beamSize.y, beamSize.z)`、Y/Z 方向の角材も同様の大きさになります。
		[[nodiscard]]
		Mesh3DAddResult addBoxFrame(Vec3 size, Vec3 beamSize);

		/// @brief 配置変換を適用した、軸ごとの太さを持つ直方体枠を追加します。
		/// @param size 枠の外側の各軸方向の大きさ
		/// @param beamSize 角材の軸ごとの太さ
		/// @param placement 適用する配置変換。`Vec3` または `Mat4x4` も直接指定できます。
		/// @return 成功時は追加された範囲、失敗時はエラー
		[[nodiscard]]
		Mesh3DAddResult addBoxFrame(Vec3 size, Vec3 beamSize, const Mesh3DPlacement& placement);

		/// @brief 指定した UV マッピングを持つ、均一な太さの直方体枠を追加します。
		/// @param size 枠の外側の各軸方向の大きさ
		/// @param thickness 角材の太さ
		/// @param uvMapping 外接 Box の各投影面に割り当てる UV 矩形
		/// @return 成功時は追加された範囲、失敗時はエラー
		[[nodiscard]]
		Mesh3DAddResult addBoxFrame(Vec3 size, double thickness, const BoxUVMapping& uvMapping);

		/// @brief 指定した UV マッピングを持つ、配置変換を適用した均一な太さの直方体枠を追加します。
		/// @param size 枠の外側の各軸方向の大きさ
		/// @param thickness 角材の太さ
		/// @param uvMapping 外接 Box の各投影面に割り当てる UV 矩形
		/// @param placement 適用する配置変換。`Vec3` または `Mat4x4` も直接指定できます。
		/// @return 成功時は追加された範囲、失敗時はエラー
		[[nodiscard]]
		Mesh3DAddResult addBoxFrame(Vec3 size, double thickness, const BoxUVMapping& uvMapping, const Mesh3DPlacement& placement);

		/// @brief 指定した UV マッピングを持つ、軸ごとの太さを持つ直方体枠を追加します。
		/// @param size 枠の外側の各軸方向の大きさ
		/// @param beamSize 角材の軸ごとの太さ
		/// @param uvMapping 外接 Box の各投影面に割り当てる UV 矩形
		/// @return 成功時は追加された範囲、失敗時はエラー
		[[nodiscard]]
		Mesh3DAddResult addBoxFrame(Vec3 size, Vec3 beamSize, const BoxUVMapping& uvMapping);

		/// @brief 指定した UV マッピングを持つ、配置変換を適用した軸ごとの太さの直方体枠を追加します。
		/// @param size 枠の外側の各軸方向の大きさ
		/// @param beamSize 角材の軸ごとの太さ
		/// @param uvMapping 外接 Box の各投影面に割り当てる UV 矩形
		/// @param placement 適用する配置変換。`Vec3` または `Mat4x4` も直接指定できます。
		/// @return 成功時は追加された範囲、失敗時はエラー
		[[nodiscard]]
		Mesh3DAddResult addBoxFrame(Vec3 size, Vec3 beamSize, const BoxUVMapping& uvMapping, const Mesh3DPlacement& placement);

		////////////////////////////////////////////////////////////////
		//
		//	addRoundedBox
		//
		////////////////////////////////////////////////////////////////

		/// @brief 原点を中心とする角丸直方体を追加します。
		/// @param size 角丸直方体の各軸方向の大きさ
		/// @param radius 角の丸みの半径。0 以上、`size` の最小成分の半分以下である必要があります。
		/// @param subdivisions 各面の丸み部分の分割数。1 以上である必要があります。
		/// @return 成功時は追加された範囲、失敗時はエラー
		[[nodiscard]]
		Mesh3DAddResult addRoundedBox(
			Vec3 size = Vec3{ 1.0, 1.0, 1.0 },
			double radius = 0.1,
			uint32 subdivisions = 4);

		/// @brief 配置変換を適用した角丸直方体を追加します。
		/// @param size 角丸直方体の各軸方向の大きさ
		/// @param radius 角の丸みの半径
		/// @param subdivisions 各面の丸み部分の分割数
		/// @param placement 適用する配置変換。`Vec3` または `Mat4x4` も直接指定できます。
		/// @return 成功時は追加された範囲、失敗時はエラー
		[[nodiscard]]
		Mesh3DAddResult addRoundedBox(Vec3 size, double radius, uint32 subdivisions, const Mesh3DPlacement& placement);

		/// @brief 指定した UV マッピングを持つ、原点を中心とする角丸直方体を追加します。
		/// @param size 角丸直方体の各軸方向の大きさ
		/// @param radius 角の丸みの半径。0 以上、`size` の最小成分の半分以下である必要があります。
		/// @param subdivisions 各面の丸み部分の分割数。1 以上である必要があります。
		/// @param uvMapping 各面から形状全体のバウンディングボックスへ投影する UV 矩形
		/// @return 成功時は追加された範囲、失敗時はエラー
		[[nodiscard]]
		Mesh3DAddResult addRoundedBox(Vec3 size, double radius, uint32 subdivisions, const BoxUVMapping& uvMapping);

		/// @brief 指定した UV マッピングを持つ、配置変換を適用した角丸直方体を追加します。
		/// @param size 角丸直方体の各軸方向の大きさ
		/// @param radius 角の丸みの半径
		/// @param subdivisions 各面の丸み部分の分割数
		/// @param uvMapping 各面から形状全体のバウンディングボックスへ投影する UV 矩形
		/// @param placement 適用する配置変換。`Vec3` または `Mat4x4` も直接指定できます。
		/// @return 成功時は追加された範囲、失敗時はエラー
		[[nodiscard]]
		Mesh3DAddResult addRoundedBox(Vec3 size, double radius, uint32 subdivisions, const BoxUVMapping& uvMapping, const Mesh3DPlacement& placement);

		////////////////////////////////////////////////////////////////
		//
		//	addChamferedBox
		//
		////////////////////////////////////////////////////////////////

		/// @brief 原点を中心とする面取り直方体を追加します。
		/// @param size 面取り直方体の各軸方向の大きさ
		/// @param chamfer 面取り幅。0 以上、`size` の最小成分の半分未満である必要があります。
		/// @return 成功時は追加された範囲、失敗時はエラー
		[[nodiscard]]
		Mesh3DAddResult addChamferedBox(
			Vec3 size = Vec3{ 1.0, 1.0, 1.0 },
			double chamfer = 0.1);

		/// @brief 配置変換を適用した面取り直方体を追加します。
		/// @param size 面取り直方体の各軸方向の大きさ
		/// @param chamfer 面取り幅
		/// @param placement 適用する配置変換。`Vec3` または `Mat4x4` も直接指定できます。
		/// @return 成功時は追加された範囲、失敗時はエラー
		[[nodiscard]]
		Mesh3DAddResult addChamferedBox(Vec3 size, double chamfer, const Mesh3DPlacement& placement);

		/// @brief 指定した UV マッピングを持つ、原点を中心とする面取り直方体を追加します。
		/// @param size 面取り直方体の各軸方向の大きさ
		/// @param chamfer 面取り幅
		/// @param uvMapping 各投影面に割り当てる UV 矩形
		/// @return 成功時は追加された範囲、失敗時はエラー
		/// @remark 辺面と角面の投影軸が複数同率になる場合は、X 軸、Y 軸、Z 軸の順に優先します。
		[[nodiscard]]
		Mesh3DAddResult addChamferedBox(Vec3 size, double chamfer, const BoxUVMapping& uvMapping);

		/// @brief 指定した UV マッピングを持つ、配置変換を適用した面取り直方体を追加します。
		/// @param size 面取り直方体の各軸方向の大きさ
		/// @param chamfer 面取り幅
		/// @param uvMapping 各投影面に割り当てる UV 矩形
		/// @param placement 適用する配置変換。`Vec3` または `Mat4x4` も直接指定できます。
		/// @return 成功時は追加された範囲、失敗時はエラー
		[[nodiscard]]
		Mesh3DAddResult addChamferedBox(
			Vec3 size,
			double chamfer,
			const BoxUVMapping& uvMapping,
			const Mesh3DPlacement& placement);

		////////////////////////////////////////////////////////////////
		//
		//	addWedge
		//
		////////////////////////////////////////////////////////////////

		/// @brief 原点を中心とし、Z 軸の正方向へ上るくさび形を追加します。
		/// @param size くさび形の各軸方向の大きさ
		/// @return 成功時は追加された範囲、失敗時はエラー
		[[nodiscard]]
		Mesh3DAddResult addWedge(Vec3 size = Vec3{ 1.0, 1.0, 1.0 });

		/// @brief 配置変換を適用したくさび形を追加します。
		/// @param size くさび形の各軸方向の大きさ
		/// @param placement 適用する配置変換。`Vec3` または `Mat4x4` も直接指定できます。
		/// @return 成功時は追加された範囲、失敗時はエラー
		[[nodiscard]]
		Mesh3DAddResult addWedge(Vec3 size, const Mesh3DPlacement& placement);

		/// @brief 指定した UV マッピングを持つ、原点を中心とするくさび形を追加します。
		/// @param size くさび形の各軸方向の大きさ
		/// @param uvMapping 形状全体のバウンディングボックスへ投影する各面の UV 矩形
		/// @return 成功時は追加された範囲、失敗時はエラー
		[[nodiscard]]
		Mesh3DAddResult addWedge(Vec3 size, const BoxUVMapping& uvMapping);

		/// @brief 指定した UV マッピングを持つ、配置変換を適用したくさび形を追加します。
		/// @param size くさび形の各軸方向の大きさ
		/// @param uvMapping 形状全体のバウンディングボックスへ投影する各面の UV 矩形
		/// @param placement 適用する配置変換。`Vec3` または `Mat4x4` も直接指定できます。
		/// @return 成功時は追加された範囲、失敗時はエラー
		[[nodiscard]]
		Mesh3DAddResult addWedge(Vec3 size, const BoxUVMapping& uvMapping, const Mesh3DPlacement& placement);

		////////////////////////////////////////////////////////////////
		//
		//	addTriangularPrism
		//
		////////////////////////////////////////////////////////////////

		/// @brief 原点を中心とする三角柱を追加します。
		/// @param size 三角柱の各軸方向の大きさ
		/// @return 成功時は追加された範囲、失敗時はエラー
		[[nodiscard]]
		Mesh3DAddResult addTriangularPrism(Vec3 size = Vec3{ 1.0, 1.0, 1.0 });

		/// @brief 配置変換を適用した三角柱を追加します。
		/// @param size 三角柱の各軸方向の大きさ
		/// @param placement 適用する配置変換。`Vec3` または `Mat4x4` も直接指定できます。
		/// @return 成功時は追加された範囲、失敗時はエラー
		[[nodiscard]]
		Mesh3DAddResult addTriangularPrism(Vec3 size, const Mesh3DPlacement& placement);

		/// @brief 指定した UV マッピングを持つ、原点を中心とする三角柱を追加します。
		/// @param size 三角柱の各軸方向の大きさ
		/// @param uvMapping 形状全体のバウンディングボックスへ投影する各面の UV 矩形
		/// @return 成功時は追加された範囲、失敗時はエラー
		[[nodiscard]]
		Mesh3DAddResult addTriangularPrism(Vec3 size, const BoxUVMapping& uvMapping);

		/// @brief 指定した UV マッピングを持つ、配置変換を適用した三角柱を追加します。
		/// @param size 三角柱の各軸方向の大きさ
		/// @param uvMapping 形状全体のバウンディングボックスへ投影する各面の UV 矩形
		/// @param placement 適用する配置変換。`Vec3` または `Mat4x4` も直接指定できます。
		/// @return 成功時は追加された範囲、失敗時はエラー
		[[nodiscard]]
		Mesh3DAddResult addTriangularPrism(Vec3 size, const BoxUVMapping& uvMapping, const Mesh3DPlacement& placement);

		////////////////////////////////////////////////////////////////
		//
		//	addStairs
		//
		////////////////////////////////////////////////////////////////

		/// @brief 原点を中心とし、Z 軸の正方向へ上る階段を追加します。
		/// @param size 階段全体の各軸方向の大きさ
		/// @param steps 段数。1 以上である必要があります。
		/// @return 成功時は追加された範囲、失敗時はエラー
		[[nodiscard]]
		Mesh3DAddResult addStairs(Vec3 size, uint32 steps);

		/// @brief 配置変換を適用した階段を追加します。
		/// @param size 階段全体の各軸方向の大きさ
		/// @param steps 段数
		/// @param placement 適用する配置変換。`Vec3` または `Mat4x4` も直接指定できます。
		/// @return 成功時は追加された範囲、失敗時はエラー
		[[nodiscard]]
		Mesh3DAddResult addStairs(Vec3 size, uint32 steps, const Mesh3DPlacement& placement);

		/// @brief 指定した UV マッピングを持つ、原点を中心とする階段を追加します。
		/// @param size 階段全体の各軸方向の大きさ
		/// @param steps 段数。1 以上である必要があります。
		/// @param uvMapping 階段全体のバウンディングボックスへ投影する各面の UV 矩形
		/// @return 成功時は追加された範囲、失敗時はエラー
		[[nodiscard]]
		Mesh3DAddResult addStairs(Vec3 size, uint32 steps, const BoxUVMapping& uvMapping);

		/// @brief 指定した UV マッピングを持つ、配置変換を適用した階段を追加します。
		/// @param size 階段全体の各軸方向の大きさ
		/// @param steps 段数
		/// @param uvMapping 階段全体のバウンディングボックスへ投影する各面の UV 矩形
		/// @param placement 適用する配置変換。`Vec3` または `Mat4x4` も直接指定できます。
		/// @return 成功時は追加された範囲、失敗時はエラー
		[[nodiscard]]
		Mesh3DAddResult addStairs(Vec3 size, uint32 steps, const BoxUVMapping& uvMapping, const Mesh3DPlacement& placement);

		////////////////////////////////////////////////////////////////
		//
		//	addPyramid
		//
		////////////////////////////////////////////////////////////////

		/// @brief 原点を中心とする正方形底面の四角錐を追加します。
		/// @param baseSize 底面の一辺の長さ
		/// @param height 四角錐の高さ
		/// @return 成功時は追加された範囲、失敗時はエラー
		[[nodiscard]]
		Mesh3DAddResult addPyramid(double baseSize, double height);

		/// @brief 配置変換を適用した正方形底面の四角錐を追加します。
		/// @param baseSize 底面の一辺の長さ
		/// @param height 四角錐の高さ
		/// @param placement 適用する配置変換。`Vec3` または `Mat4x4` も直接指定できます。
		/// @return 成功時は追加された範囲、失敗時はエラー
		[[nodiscard]]
		Mesh3DAddResult addPyramid(double baseSize, double height, const Mesh3DPlacement& placement);

		/// @brief 原点を中心とする長方形底面の四角錐を追加します。
		/// @param baseSizeXZ 底面の X 軸方向および Z 軸方向の大きさ
		/// @param height 四角錐の高さ
		/// @return 成功時は追加された範囲、失敗時はエラー
		[[nodiscard]]
		Mesh3DAddResult addPyramid(SizeF baseSizeXZ = SizeF{ 1.0, 1.0 }, double height = 1.0);

		/// @brief 配置変換を適用した長方形底面の四角錐を追加します。
		/// @param baseSizeXZ 底面の X 軸方向および Z 軸方向の大きさ
		/// @param height 四角錐の高さ
		/// @param placement 適用する配置変換。`Vec3` または `Mat4x4` も直接指定できます。
		/// @return 成功時は追加された範囲、失敗時はエラー
		[[nodiscard]]
		Mesh3DAddResult addPyramid(SizeF baseSizeXZ, double height, const Mesh3DPlacement& placement);

		////////////////////////////////////////////////////////////////
		//
		//	addRectangularFrustum
		//
		////////////////////////////////////////////////////////////////

		/// @brief 原点を中心とする角錐台を追加します。
		/// @param bottomSizeXZ 底面の X 軸方向および Z 軸方向の大きさ
		/// @param topSizeXZ 上面の X 軸方向および Z 軸方向の大きさ
		/// @param height 角錐台の高さ
		/// @return 成功時は追加された範囲、失敗時はエラー
		[[nodiscard]]
		Mesh3DAddResult addRectangularFrustum(SizeF bottomSizeXZ, SizeF topSizeXZ, double height);

		/// @brief 配置変換を適用した角錐台を追加します。
		/// @param bottomSizeXZ 底面の X 軸方向および Z 軸方向の大きさ
		/// @param topSizeXZ 上面の X 軸方向および Z 軸方向の大きさ
		/// @param height 角錐台の高さ
		/// @param placement 適用する配置変換。`Vec3` または `Mat4x4` も直接指定できます。
		/// @return 成功時は追加された範囲、失敗時はエラー
		[[nodiscard]]
		Mesh3DAddResult addRectangularFrustum(SizeF bottomSizeXZ, SizeF topSizeXZ, double height, const Mesh3DPlacement& placement);

		/// @brief 指定した UV マッピングを持つ、原点を中心とする角錐台を追加します。
		/// @param bottomSizeXZ 底面の X 軸方向および Z 軸方向の大きさ
		/// @param topSizeXZ 上面の X 軸方向および Z 軸方向の大きさ
		/// @param height 角錐台の高さ
		/// @param uvMapping 形状全体のバウンディングボックスへ投影する各面の UV 矩形
		/// @return 成功時は追加された範囲、失敗時はエラー
		[[nodiscard]]
		Mesh3DAddResult addRectangularFrustum(SizeF bottomSizeXZ, SizeF topSizeXZ, double height, const BoxUVMapping& uvMapping);

		/// @brief 指定した UV マッピングを持つ、配置変換を適用した角錐台を追加します。
		/// @param bottomSizeXZ 底面の X 軸方向および Z 軸方向の大きさ
		/// @param topSizeXZ 上面の X 軸方向および Z 軸方向の大きさ
		/// @param height 角錐台の高さ
		/// @param uvMapping 形状全体のバウンディングボックスへ投影する各面の UV 矩形
		/// @param placement 適用する配置変換。`Vec3` または `Mat4x4` も直接指定できます。
		/// @return 成功時は追加された範囲、失敗時はエラー
		[[nodiscard]]
		Mesh3DAddResult addRectangularFrustum(SizeF bottomSizeXZ, SizeF topSizeXZ, double height, const BoxUVMapping& uvMapping, const Mesh3DPlacement& placement);

		////////////////////////////////////////////////////////////////
		//
		//	addExtrude
		//
		////////////////////////////////////////////////////////////////

		/// @brief 2D の多角形を Y 軸方向に押し出した形状を追加します。
		/// @param polygon 押し出す多角形。穴を含むことができます。
		/// @param height 押し出す高さ
		/// @param options 側面の法線補間の設定
		/// @return 成功時は追加された範囲、失敗時はエラー
		/// @remark 座標、UV 座標、および法線の規約は Mesh3D::Extrude() と同じです。
		[[nodiscard]]
		Mesh3DAddResult addExtrude(const Polygon& polygon, double height, const ExtrudeOptions& options = {});

		/// @brief 配置変換を適用した押し出し形状を追加します。
		/// @param polygon 押し出す多角形。穴を含むことができます。
		/// @param height 押し出す高さ
		/// @param placement 適用する配置変換
		/// @param options 側面の法線補間の設定
		/// @return 成功時は追加された範囲、失敗時はエラー
		/// @remark 座標、UV 座標、および法線の規約は Mesh3D::Extrude() と同じです。
		[[nodiscard]]
		Mesh3DAddResult addExtrude(
			const Polygon& polygon,
			double height,
			const Mesh3DPlacement& placement,
			const ExtrudeOptions& options = {});

		////////////////////////////////////////////////////////////////
		//
		//	addRevolve
		//
		////////////////////////////////////////////////////////////////

		/// @brief 2D プロファイルを Y 軸の周りに回転させた形状を追加します。
		/// @param profile 回転させるプロファイル
		/// @param options 回転範囲、分割数、法線補間、UV 変換、および回転方向の端面設定
		/// @return 成功時は追加された範囲、失敗時はエラー
		/// @remark プロファイル、法線、角度、端面、および UV 座標の規約は `Mesh3D::Revolve()` と同じです。
		[[nodiscard]]
		Mesh3DAddResult addRevolve(
			std::span<const Vec2> profile,
			const RevolveOptions& options = {});

		/// @brief 初期化子リストで指定した 2D プロファイルを Y 軸の周りに回転させた形状を追加します。
		/// @param profile 回転させるプロファイル
		/// @param options 生成設定
		/// @return 成功時は追加された範囲、失敗時はエラー
		/// @remark プロファイル、法線、角度、端面、および UV 座標の規約は `std::span` を受け取るオーバーロードと同じです。
		[[nodiscard]]
		Mesh3DAddResult addRevolve(
			std::initializer_list<Vec2> profile,
			const RevolveOptions& options = {});

		/// @brief 配置変換を適用した回転体を追加します。
		/// @param profile 回転させるプロファイル
		/// @param placement 適用する配置変換。`Vec3` または `Mat4x4` も直接指定できます。
		/// @param options 生成設定
		/// @return 成功時は追加された範囲、失敗時はエラー
		[[nodiscard]]
		Mesh3DAddResult addRevolve(
			std::span<const Vec2> profile,
			const Mesh3DPlacement& placement,
			const RevolveOptions& options = {});

		////////////////////////////////////////////////////////////////
		//
		//	addTube
		//
		////////////////////////////////////////////////////////////////

		/// @brief 生成設定を指定し、一定半径のチューブを追加します。
		/// @param path チューブの中心を通る経路の頂点
		/// @param radius チューブの半径
		/// @param options 断面分割数、UV 変換、経路の閉鎖方法、および端面設定
		/// @return 成功時は追加された範囲、失敗時はエラー
		/// @remark 端面設定の規約は `TubeOptions` を受け取る `Mesh3D::Tube()` と同じです。
		/// @remark 2 点間を結ぶ円柱状の柱・梁は `builder.addTube({ from, to }, radius)` で追加できます。両端面を既定で生成し、断面分割数は `TubeOptions::sides` で指定できます。
		[[nodiscard]]
		Mesh3DAddResult addTube(std::span<const Vec3> path, double radius, const TubeOptions& options = {});

		/// @brief 初期化子リストと生成設定を指定し、一定半径のチューブを追加します。
		/// @param path チューブの中心を通る経路の頂点
		/// @param radius チューブの半径
		/// @param options 生成設定
		/// @return 成功時は追加された範囲、失敗時はエラー
		[[nodiscard]]
		Mesh3DAddResult addTube(std::initializer_list<Vec3> path, double radius, const TubeOptions& options = {});

		/// @brief 生成設定を指定し、経路点ごとに半径が異なるチューブを追加します。
		/// @param path チューブの中心を通る経路の頂点
		/// @param radii 各経路点における半径
		/// @param options 生成設定
		/// @return 成功時は追加された範囲、失敗時はエラー
		[[nodiscard]]
		Mesh3DAddResult addTube(std::span<const Vec3> path, std::span<const double> radii, const TubeOptions& options = {});

		/// @brief 初期化子リストと生成設定を指定し、経路点ごとに半径が異なるチューブを追加します。
		/// @param path チューブの中心を通る経路の頂点
		/// @param radii 各経路点における半径
		/// @param options 生成設定
		/// @return 成功時は追加された範囲、失敗時はエラー
		[[nodiscard]]
		Mesh3DAddResult addTube(std::initializer_list<Vec3> path, std::initializer_list<double> radii, const TubeOptions& options = {});

		/// @brief 配置変換と生成設定を指定し、一定半径のチューブを追加します。
		/// @param path チューブの中心を通る経路の頂点
		/// @param radius チューブの半径
		/// @param placement 適用する配置変換。`Vec3` または `Mat4x4` も直接指定できます。
		/// @param options 生成設定
		/// @return 成功時は追加された範囲、失敗時はエラー
		[[nodiscard]]
		Mesh3DAddResult addTube(std::span<const Vec3> path, double radius, const Mesh3DPlacement& placement, const TubeOptions& options = {});

		/// @brief 配置変換と生成設定を指定し、経路点ごとに半径が異なるチューブを追加します。
		/// @param path チューブの中心を通る経路の頂点
		/// @param radii 各経路点における半径
		/// @param placement 適用する配置変換。`Vec3` または `Mat4x4` も直接指定できます。
		/// @param options 生成設定
		/// @return 成功時は追加された範囲、失敗時はエラー
		[[nodiscard]]
		Mesh3DAddResult addTube(std::span<const Vec3> path, std::span<const double> radii, const Mesh3DPlacement& placement, const TubeOptions& options = {});

		////////////////////////////////////////////////////////////////
		//
		//	addSweep
		//
		////////////////////////////////////////////////////////////////

		/// @brief 生成設定を指定し、一定断面の Sweep 形状を追加します。
		/// @param crossSection 経路に沿わせる断面。穴を含むことができます。
		/// @param path 断面の中心を通る経路の頂点
		/// @param options 初期断面方向、UV 変換、経路の閉鎖方法、および端面設定
		/// @return 成功時は追加された範囲、失敗時はエラー
		/// @remark 端面設定の規約は `SweepOptions` を受け取る `Mesh3D::Sweep()` と同じです。
		[[nodiscard]]
		Mesh3DAddResult addSweep(const Polygon& crossSection, std::span<const Vec3> path, const SweepOptions& options = {});

		/// @brief 初期化子リストと生成設定を指定し、一定断面の Sweep 形状を追加します。
		/// @param crossSection 経路に沿わせる断面。穴を含むことができます。
		/// @param path 断面の中心を通る経路の頂点
		/// @param options 生成設定
		/// @return 成功時は追加された範囲、失敗時はエラー
		[[nodiscard]]
		Mesh3DAddResult addSweep(const Polygon& crossSection, std::initializer_list<Vec3> path, const SweepOptions& options = {});

		/// @brief 配置変換と生成設定を指定し、一定断面の Sweep 形状を追加します。
		/// @param crossSection 経路に沿わせる断面
		/// @param path 断面の中心を通る経路の頂点
		/// @param placement 適用する配置変換。`Vec3` または `Mat4x4` も直接指定できます。
		/// @param options 生成設定
		/// @return 成功時は追加された範囲、失敗時はエラー
		[[nodiscard]]
		Mesh3DAddResult addSweep(const Polygon& crossSection, std::span<const Vec3> path, const Mesh3DPlacement& placement, const SweepOptions& options = {});

		/// @brief 経路点ごとに断面の拡大率と twist を指定した Sweep 形状を追加します。
		/// @param crossSection 経路に沿わせる断面。穴を含むことができます。
		/// @param path 断面の中心を通る経路の頂点
		/// @param sectionTransforms 各経路点における断面変換。要素数は `path.size()` と等しい必要があります。
		/// @param options 初期断面方向、UV 変換、経路の閉鎖方法、および端面設定
		/// @return 成功時は追加された範囲、失敗時はエラー
		/// @remark 断面変換、経路、端面、UV 座標、および頂点属性の規約は、経路点別変換を受け取る `Mesh3D::Sweep()` と同じです。
		[[nodiscard]]
		Mesh3DAddResult addSweep(
			const Polygon& crossSection,
			std::span<const Vec3> path,
			std::span<const SweepSectionTransform> sectionTransforms,
			const SweepOptions& options = {});

		/// @brief 初期化子リストで経路点ごとの断面変換を指定した Sweep 形状を追加します。
		/// @param crossSection 経路に沿わせる断面。穴を含むことができます。
		/// @param path 断面の中心を通る経路の頂点
		/// @param sectionTransforms 各経路点における断面変換
		/// @param options 初期断面方向、UV 変換、経路の閉鎖方法、および端面設定
		/// @return 成功時は追加された範囲、失敗時はエラー
		[[nodiscard]]
		Mesh3DAddResult addSweep(
			const Polygon& crossSection,
			std::initializer_list<Vec3> path,
			std::initializer_list<SweepSectionTransform> sectionTransforms,
			const SweepOptions& options = {});

		/// @brief 配置変換を適用し、経路点ごとに断面変換を指定した Sweep 形状を追加します。
		/// @param crossSection 経路に沿わせる断面
		/// @param path 断面の中心を通る経路の頂点
		/// @param sectionTransforms 各経路点における断面変換
		/// @param placement 適用する配置変換。`Vec3` または `Mat4x4` も直接指定できます。
		/// @param options 初期断面方向、UV 変換、および経路の閉鎖方法
		/// @return 成功時は追加された範囲、失敗時はエラー
		[[nodiscard]]
		Mesh3DAddResult addSweep(
			const Polygon& crossSection,
			std::span<const Vec3> path,
			std::span<const SweepSectionTransform> sectionTransforms,
			const Mesh3DPlacement& placement,
			const SweepOptions& options = {});

		////////////////////////////////////////////////////////////////
		//
		//	addTetrahedron
		//
		////////////////////////////////////////////////////////////////

		/// @brief 原点を中心とする正四面体を追加します。
		/// @param radius 外接球の半径
		/// @return 成功時は追加された範囲、失敗時はエラー
		[[nodiscard]]
		Mesh3DAddResult addTetrahedron(double radius = 1.0);

		/// @brief 配置変換を適用した正四面体を追加します。
		/// @param radius 外接球の半径
		/// @param placement 適用する配置変換。`Vec3` または `Mat4x4` も直接指定できます。
		/// @return 成功時は追加された範囲、失敗時はエラー
		[[nodiscard]]
		Mesh3DAddResult addTetrahedron(double radius, const Mesh3DPlacement& placement);

		////////////////////////////////////////////////////////////////
		//
		//	addOctahedron
		//
		////////////////////////////////////////////////////////////////

		/// @brief 原点を中心とする正八面体を追加します。
		/// @param radius 外接球の半径
		/// @return 成功時は追加された範囲、失敗時はエラー
		[[nodiscard]]
		Mesh3DAddResult addOctahedron(double radius = 1.0);

		/// @brief 配置変換を適用した正八面体を追加します。
		/// @param radius 外接球の半径
		/// @param placement 適用する配置変換。`Vec3` または `Mat4x4` も直接指定できます。
		/// @return 成功時は追加された範囲、失敗時はエラー
		[[nodiscard]]
		Mesh3DAddResult addOctahedron(double radius, const Mesh3DPlacement& placement);

		////////////////////////////////////////////////////////////////
		//
		//	addIcosahedron
		//
		////////////////////////////////////////////////////////////////

		/// @brief 原点を中心とする正二十面体を追加します。
		/// @param radius 外接球の半径
		/// @return 成功時は追加された範囲、失敗時はエラー
		[[nodiscard]]
		Mesh3DAddResult addIcosahedron(double radius = 1.0);

		/// @brief 配置変換を適用した正二十面体を追加します。
		/// @param radius 外接球の半径
		/// @param placement 適用する配置変換。`Vec3` または `Mat4x4` も直接指定できます。
		/// @return 成功時は追加された範囲、失敗時はエラー
		[[nodiscard]]
		Mesh3DAddResult addIcosahedron(double radius, const Mesh3DPlacement& placement);

		////////////////////////////////////////////////////////////////
		//
		//	addIcoSphere
		//
		////////////////////////////////////////////////////////////////

		/// @brief 正二十面体を細分化した、原点を中心とする球を追加します。
		/// @param radius 球の半径
		/// @param subdivisions 細分化回数。0 以上 8 以下である必要があります。
		/// @return 成功時は追加された範囲、失敗時はエラー
		/// @remark 頂点の共有、法線、UV 座標、および接線の規約は `Mesh3D::IcoSphere()` と同じです。
		[[nodiscard]]
		Mesh3DAddResult addIcoSphere(
			double radius = 1.0,
			uint32 subdivisions = 2);

		/// @brief 配置変換を適用した IcoSphere を追加します。
		/// @param radius 球の半径
		/// @param subdivisions 細分化回数
		/// @param placement 適用する配置変換。`Vec3` または `Mat4x4` も直接指定できます。
		/// @return 成功時は追加された範囲、失敗時はエラー
		[[nodiscard]]
		Mesh3DAddResult addIcoSphere(
			double radius,
			uint32 subdivisions,
			const Mesh3DPlacement& placement);

		////////////////////////////////////////////////////////////////
		//
		//	addDodecahedron
		//
		////////////////////////////////////////////////////////////////

		/// @brief 原点を中心とする正十二面体を追加します。
		/// @param radius 外接球の半径
		/// @return 成功時は追加された範囲、失敗時はエラー
		[[nodiscard]]
		Mesh3DAddResult addDodecahedron(double radius = 1.0);

		/// @brief 配置変換を適用した正十二面体を追加します。
		/// @param radius 外接球の半径
		/// @param placement 適用する配置変換。`Vec3` または `Mat4x4` も直接指定できます。
		/// @return 成功時は追加された範囲、失敗時はエラー
		[[nodiscard]]
		Mesh3DAddResult addDodecahedron(double radius, const Mesh3DPlacement& placement);

		////////////////////////////////////////////////////////////////
		//
		//	addPlane
		//
		////////////////////////////////////////////////////////////////

		/// @brief XZ 平面上に、法線が Y 軸の正方向を向く長方形を追加します。
		/// @param sizeXZ X 軸方向および Z 軸方向の大きさ
		/// @param uvScale UV 座標の拡大率
		/// @param uvOffset UV 座標のオフセット
		/// @return 成功時は追加された範囲、失敗時はエラー
		[[nodiscard]]
		Mesh3DAddResult addPlane(
			SizeF sizeXZ = SizeF{ 1.0, 1.0 },
			Vec2 uvScale = Vec2{ 1.0, 1.0 },
			Vec2 uvOffset = Vec2{ 0.0, 0.0 });

		/// @brief 配置変換を適用した長方形を追加します。
		/// @param sizeXZ X 軸方向および Z 軸方向の大きさ
		/// @param placement 適用する配置変換。`Vec3` または `Mat4x4` も直接指定できます。
		/// @return 成功時は追加された範囲、失敗時はエラー
		[[nodiscard]]
		Mesh3DAddResult addPlane(SizeF sizeXZ, const Mesh3DPlacement& placement);

		/// @brief UV 変換と配置変換を適用した長方形を追加します。
		/// @param sizeXZ X 軸方向および Z 軸方向の大きさ
		/// @param uvScale UV 座標の拡大率
		/// @param uvOffset UV 座標のオフセット
		/// @param placement 適用する配置変換。`Vec3` または `Mat4x4` も直接指定できます。
		/// @return 成功時は追加された範囲、失敗時はエラー
		[[nodiscard]]
		Mesh3DAddResult addPlane(SizeF sizeXZ, Vec2 uvScale, Vec2 uvOffset, const Mesh3DPlacement& placement);

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
		/// @return 成功時は追加された範囲、失敗時はエラー
		[[nodiscard]]
		Mesh3DAddResult addGrid(
			SizeF sizeXZ,
			uint32 segmentsX,
			uint32 segmentsZ,
			Vec2 uvScale = Vec2{ 1.0, 1.0 },
			Vec2 uvOffset = Vec2{ 0.0, 0.0 });

		/// @brief 配置変換を適用した格子を追加します。
		/// @param sizeXZ X 軸方向および Z 軸方向の大きさ
		/// @param segmentsX X 軸方向の分割数
		/// @param segmentsZ Z 軸方向の分割数
		/// @param placement 適用する配置変換。`Vec3` または `Mat4x4` も直接指定できます。
		/// @return 成功時は追加された範囲、失敗時はエラー
		[[nodiscard]]
		Mesh3DAddResult addGrid(SizeF sizeXZ, uint32 segmentsX, uint32 segmentsZ, const Mesh3DPlacement& placement);

		/// @brief UV 変換と配置変換を適用した格子を追加します。
		/// @param sizeXZ X 軸方向および Z 軸方向の大きさ
		/// @param segmentsX X 軸方向の分割数
		/// @param segmentsZ Z 軸方向の分割数
		/// @param uvScale UV 座標の拡大率
		/// @param uvOffset UV 座標のオフセット
		/// @param placement 適用する配置変換。`Vec3` または `Mat4x4` も直接指定できます。
		/// @return 成功時は追加された範囲、失敗時はエラー
		[[nodiscard]]
		Mesh3DAddResult addGrid(SizeF sizeXZ, uint32 segmentsX, uint32 segmentsZ, Vec2 uvScale, Vec2 uvOffset, const Mesh3DPlacement& placement);

		////////////////////////////////////////////////////////////////
		//
		//	addHeightField
		//
		////////////////////////////////////////////////////////////////

		/// @brief 格子状の高さデータから地形を追加します。
		/// @param heights 各頂点の Y 座標を格納した高さデータ。幅と高さがそれぞれ 2 以上である必要があります。
		/// @param sizeXZ X 軸方向および Z 軸方向の大きさ
		/// @param options UV 座標の生成設定
		/// @return 成功時は追加された範囲、失敗時はエラー
		/// @remark 座標、UV 座標、法線、および接線の規約は `Mesh3D::HeightField()` と同じです。
		[[nodiscard]]
		Mesh3DAddResult addHeightField(
			const Grid<float>& heights,
			SizeF sizeXZ,
			const HeightFieldOptions& options = {});

		/// @brief 配置変換を適用した高さフィールドを追加します。
		/// @param heights 各頂点の Y 座標を格納した高さデータ
		/// @param sizeXZ X 軸方向および Z 軸方向の大きさ
		/// @param placement 適用する配置変換。`Vec3` または `Mat4x4` も直接指定できます。
		/// @param options UV 座標の生成設定
		/// @return 成功時は追加された範囲、失敗時はエラー
		[[nodiscard]]
		Mesh3DAddResult addHeightField(
			const Grid<float>& heights,
			SizeF sizeXZ,
			const Mesh3DPlacement& placement,
			const HeightFieldOptions& options = {});

		/// @brief callable で生成した格子状の高さから地形を追加します。
		/// @param gridSize X 方向および Z 方向の頂点数
		/// @param sizeXZ X 軸方向および Z 軸方向の大きさ
		/// @param heightFunction 格子点の列と行を表す `Point` を受け取り、その頂点の Y 座標を返す callable
		/// @param options UV 座標の生成設定
		/// @return 成功時は追加された範囲、失敗時はエラー
		/// @remark callable の呼び出し、および生成される頂点属性の規約は `Mesh3D::HeightField()` と同じです。
		[[nodiscard]]
		Mesh3DAddResult addHeightField(
			Size gridSize,
			SizeF sizeXZ,
			FunctionRef<double(Point)> heightFunction,
			const HeightFieldOptions& options = {});

		/// @brief callable で生成し、配置変換を適用した高さフィールドを追加します。
		/// @param gridSize X 方向および Z 方向の頂点数
		/// @param sizeXZ X 軸方向および Z 軸方向の大きさ
		/// @param heightFunction 各格子点の Y 座標を返す callable
		/// @param placement 適用する配置変換。`Vec3` または `Mat4x4` も直接指定できます。
		/// @param options UV 座標の生成設定
		/// @return 成功時は追加された範囲、失敗時はエラー
		[[nodiscard]]
		Mesh3DAddResult addHeightField(
			Size gridSize,
			SizeF sizeXZ,
			FunctionRef<double(Point)> heightFunction,
			const Mesh3DPlacement& placement,
			const HeightFieldOptions& options = {});

		////////////////////////////////////////////////////////////////
		//
		//	addLoft
		//
		////////////////////////////////////////////////////////////////

		/// @brief 輪郭と配置で指定する断面を接続し、直接追加します。
		/// @param sections 呼び出し終了まで有効な輪郭を参照する断面列
		/// @param options 端面、輪郭方向の平滑化、および UV 変換
		/// @return 追加範囲、または失敗理由。失敗時は既存のメッシュを変更しません。
		/// @remark 入力、配置、頂点対応、法線・接線、および検証の規約は Mesh3D::Loft() と同じです。
		[[nodiscard]]
		Mesh3DAddResult addLoft(std::span<const LoftSection> sections, const LoftOptions& options = {});

		/// @brief 初期化子リストの断面列を接続して追加します。
		/// @param sections 断面列。各 points の参照先は呼び出し終了まで有効である必要があります。
		/// @param options 生成設定
		/// @return 追加範囲、または失敗理由
		[[nodiscard]]
		Mesh3DAddResult addLoft(std::initializer_list<LoftSection> sections, const LoftOptions& options = {});

		/// @brief 全体の配置を適用した Loft を追加します。
		/// @param sections 輪郭と配置を持つ断面列
		/// @param placement 全断面の frame 適用後に行う全体配置
		/// @param options 生成設定
		/// @return 追加範囲、または失敗理由
		[[nodiscard]]
		Mesh3DAddResult addLoft(std::span<const LoftSection> sections, const Mesh3DPlacement& placement, const LoftOptions& options = {});

		////////////////////////////////////////////////////////////////
		//
		//	addDisc
		//
		////////////////////////////////////////////////////////////////

		/// @brief XZ 平面上に、法線が Y 軸の正方向を向く円盤を追加します。
		/// @param radius 円盤の半径
		/// @param segments 円周の分割数。3 以上である必要があります。
		/// @return 成功時は追加された範囲、失敗時はエラー
		[[nodiscard]]
		Mesh3DAddResult addDisc(double radius, uint32 segments = 32);

		/// @brief 配置変換を適用した円盤を追加します。
		/// @param radius 円盤の半径
		/// @param segments 円周の分割数
		/// @param placement 適用する配置変換。`Vec3` または `Mat4x4` も直接指定できます。
		/// @return 成功時は追加された範囲、失敗時はエラー
		[[nodiscard]]
		Mesh3DAddResult addDisc(double radius, uint32 segments, const Mesh3DPlacement& placement);

		////////////////////////////////////////////////////////////////
		//
		//	addAnnulus
		//
		////////////////////////////////////////////////////////////////

		/// @brief XZ 平面上に、法線が Y 軸の正方向を向く円環を追加します。
		/// @param innerRadius 円環の内半径。0 以上である必要があります。
		/// @param outerRadius 円環の外半径。`innerRadius` より大きい必要があります。
		/// @param segments 円周の分割数。3 以上である必要があります。
		/// @return 成功時は追加された範囲、失敗時はエラー
		[[nodiscard]]
		Mesh3DAddResult addAnnulus(double innerRadius, double outerRadius, uint32 segments = 32);

		/// @brief 配置変換を適用した円環を追加します。
		/// @param innerRadius 円環の内半径
		/// @param outerRadius 円環の外半径
		/// @param segments 円周の分割数
		/// @param placement 適用する配置変換。`Vec3` または `Mat4x4` も直接指定できます。
		/// @return 成功時は追加された範囲、失敗時はエラー
		[[nodiscard]]
		Mesh3DAddResult addAnnulus(double innerRadius, double outerRadius, uint32 segments, const Mesh3DPlacement& placement);

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
		/// @return 成功時は追加された範囲、失敗時はエラー
		[[nodiscard]]
		Mesh3DAddResult addHollowCylinder(double innerRadius, double outerRadius, double height, uint32 segments = 32);

		/// @brief 配置変換を適用した中空円柱を追加します。
		/// @param innerRadius 内半径
		/// @param outerRadius 外半径
		/// @param height 高さ
		/// @param segments 円周の分割数
		/// @param placement 適用する配置変換。`Vec3` または `Mat4x4` も直接指定できます。
		/// @return 成功時は追加された範囲、失敗時はエラー
		[[nodiscard]]
		Mesh3DAddResult addHollowCylinder(double innerRadius, double outerRadius, double height, uint32 segments, const Mesh3DPlacement& placement);

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
		/// @return 成功時は追加された範囲、失敗時はエラー
		[[nodiscard]]
		Mesh3DAddResult addConicalFrustum(double bottomRadius, double topRadius, double height, uint32 segments = 32);

		/// @brief 配置変換を適用した円錐台を追加します。
		/// @param bottomRadius 底面の半径
		/// @param topRadius 上面の半径
		/// @param height 高さ
		/// @param segments 円周の分割数
		/// @param placement 適用する配置変換。`Vec3` または `Mat4x4` も直接指定できます。
		/// @return 成功時は追加された範囲、失敗時はエラー
		[[nodiscard]]
		Mesh3DAddResult addConicalFrustum(double bottomRadius, double topRadius, double height, uint32 segments, const Mesh3DPlacement& placement);

		////////////////////////////////////////////////////////////////
		//
		//	addCylinder
		//
		////////////////////////////////////////////////////////////////

		/// @brief Y 軸方向に伸びる、両端が閉じた円柱を追加します。
		/// @param radius 円柱の半径
		/// @param height 円柱の高さ
		/// @param segments 円周の分割数。3 以上である必要があります。
		/// @return 成功時は追加された範囲、失敗時はエラー
		[[nodiscard]]
		Mesh3DAddResult addCylinder(double radius, double height, uint32 segments = 32);

		/// @brief 配置変換を適用した円柱を追加します。
		/// @param radius 円柱の半径
		/// @param height 円柱の高さ
		/// @param segments 円周の分割数
		/// @param placement 適用する配置変換。`Vec3` または `Mat4x4` も直接指定できます。
		/// @return 成功時は追加された範囲、失敗時はエラー
		[[nodiscard]]
		Mesh3DAddResult addCylinder(double radius, double height, uint32 segments, const Mesh3DPlacement& placement);

		////////////////////////////////////////////////////////////////
		//
		//	addCone
		//
		////////////////////////////////////////////////////////////////

		/// @brief Y 軸方向に伸びる、底面が閉じた円錐を追加します。
		/// @param radius 底面の半径
		/// @param height 円錐の高さ
		/// @param segments 円周の分割数。3 以上である必要があります。
		/// @return 成功時は追加された範囲、失敗時はエラー
		[[nodiscard]]
		Mesh3DAddResult addCone(double radius, double height, uint32 segments = 32);

		/// @brief 配置変換を適用した円錐を追加します。
		/// @param radius 底面の半径
		/// @param height 円錐の高さ
		/// @param segments 円周の分割数
		/// @param placement 適用する配置変換。`Vec3` または `Mat4x4` も直接指定できます。
		/// @return 成功時は追加された範囲、失敗時はエラー
		[[nodiscard]]
		Mesh3DAddResult addCone(double radius, double height, uint32 segments, const Mesh3DPlacement& placement);

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
		/// @return 成功時は追加された範囲、失敗時はエラー
		[[nodiscard]]
		Mesh3DAddResult addTorus(double majorRadius, double tubeRadius, uint32 ringSegments = 32, uint32 tubeSegments = 16);

		/// @brief 配置変換を適用したトーラスを追加します。
		/// @param majorRadius 原点からチューブ断面の中心までの半径
		/// @param tubeRadius チューブ断面の半径
		/// @param ringSegments リング方向の分割数
		/// @param tubeSegments チューブ断面方向の分割数
		/// @param placement 適用する配置変換。`Vec3` または `Mat4x4` も直接指定できます。
		/// @return 成功時は追加された範囲、失敗時はエラー
		[[nodiscard]]
		Mesh3DAddResult addTorus(double majorRadius, double tubeRadius, uint32 ringSegments, uint32 tubeSegments, const Mesh3DPlacement& placement);

		////////////////////////////////////////////////////////////////
		//
		//	addSphere
		//
		////////////////////////////////////////////////////////////////

		/// @brief 原点を中心とする UV 球を追加します。
		/// @param radius 球の半径
		/// @param slices 経度方向の分割数。3 以上である必要があります。
		/// @param stacks 緯度方向の分割数。2 以上である必要があります。
		/// @return 成功時は追加された範囲、失敗時はエラー
		[[nodiscard]]
		Mesh3DAddResult addSphere(double radius, uint32 slices = 32, uint32 stacks = 16);

		/// @brief 配置変換を適用した UV 球を追加します。
		/// @param radius 球の半径
		/// @param slices 経度方向の分割数
		/// @param stacks 緯度方向の分割数
		/// @param placement 適用する配置変換。`Vec3` または `Mat4x4` も直接指定できます。
		/// @return 成功時は追加された範囲、失敗時はエラー
		[[nodiscard]]
		Mesh3DAddResult addSphere(double radius, uint32 slices, uint32 stacks, const Mesh3DPlacement& placement);

		////////////////////////////////////////////////////////////////
		//
		//	addHemisphere
		//
		////////////////////////////////////////////////////////////////

		/// @brief Y 軸の正方向を向く、底面のない半球を追加します。
		/// @param radius 半球の半径
		/// @param slices 経度方向の分割数。3 以上である必要があります。
		/// @param stacks 緯度方向の分割数。1 以上である必要があります。
		/// @return 成功時は追加された範囲、失敗時はエラー
		[[nodiscard]]
		Mesh3DAddResult addHemisphere(double radius, uint32 slices = 32, uint32 stacks = 8);

		/// @brief 配置変換を適用した、底面のない半球を追加します。
		/// @param radius 半球の半径
		/// @param slices 経度方向の分割数
		/// @param stacks 緯度方向の分割数
		/// @param placement 適用する配置変換。`Vec3` または `Mat4x4` も直接指定できます。
		/// @return 成功時は追加された範囲、失敗時はエラー
		[[nodiscard]]
		Mesh3DAddResult addHemisphere(double radius, uint32 slices, uint32 stacks, const Mesh3DPlacement& placement);

		/// @brief Y 軸の正方向を向く半球を追加します。
		/// @param radius 半球の半径
		/// @param closeBottom 底面を閉じる場合は `CloseBottom::Yes`、底面を作成しない場合は `CloseBottom::No`
		/// @param slices 経度方向の分割数。3 以上である必要があります。
		/// @param stacks 緯度方向の分割数。1 以上である必要があります。
		/// @return 成功時は追加された範囲、失敗時はエラー
		[[nodiscard]]
		Mesh3DAddResult addHemisphere(double radius, CloseBottom closeBottom, uint32 slices = 32, uint32 stacks = 8);

		/// @brief 配置変換を適用した半球を追加します。
		/// @param radius 半球の半径
		/// @param closeBottom 底面を閉じるか
		/// @param slices 経度方向の分割数
		/// @param stacks 緯度方向の分割数
		/// @param placement 適用する配置変換。`Vec3` または `Mat4x4` も直接指定できます。
		/// @return 成功時は追加された範囲、失敗時はエラー
		[[nodiscard]]
		Mesh3DAddResult addHemisphere(double radius, CloseBottom closeBottom, uint32 slices, uint32 stacks, const Mesh3DPlacement& placement);

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
		/// @return 成功時は追加された範囲、失敗時はエラー
		[[nodiscard]]
		Mesh3DAddResult addCapsule(double radius, double cylinderHeight, uint32 slices = 32, uint32 hemisphereStacks = 8);

		/// @brief 配置変換を適用したカプセルを追加します。
		/// @param radius カプセルの半径
		/// @param cylinderHeight 円柱部分の高さ
		/// @param slices 円周方向の分割数
		/// @param hemisphereStacks 各半球の緯度方向の分割数
		/// @param placement 適用する配置変換。`Vec3` または `Mat4x4` も直接指定できます。
		/// @return 成功時は追加された範囲、失敗時はエラー
		[[nodiscard]]
		Mesh3DAddResult addCapsule(double radius, double cylinderHeight, uint32 slices, uint32 hemisphereStacks, const Mesh3DPlacement& placement);

	private:

		Mesh3D m_mesh;
	};
}

# include "detail/Mesh3DBuilder.ipp"
