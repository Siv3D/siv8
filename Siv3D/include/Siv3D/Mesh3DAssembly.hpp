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
# include "Mesh3DBuilder.hpp"

namespace s3d
{
	/// @brief Mesh3DAssembly の焼き込みに許容するメッシュの大きさ
	struct Mesh3DBakeOptions
	{
		/// @brief 出力頂点数の上限。Mesh3D::MaxVertexCount より大きく指定しても、その上限は超えられません。
		size_t maxVertexCount = Mesh3D::MaxVertexCount;

		/// @brief 出力三角形数の上限
		size_t maxTriangleCount = static_cast<size_t>(-1);
	};

	/// @brief 共有する形状、材質、および親子関係を持つ部品を保持する CPU 側の 3D 組立データ
	/// @remark addMesh() で登録した形状を、複数の Part から参照できます。部品の追加や配置の変更では頂点を複製しません。
	/// @remark ID は種類ごとの登録順を表し、追加、差し替え、および bake() で変化しません。別の Assembly の ID は使用できません。
	/// @remark コピーは形状を含む独立したコピーです。コピー元と同じ ID でコピー先の要素を参照できます。
	/// @remark clear()、代入、およびムーブ元からの移動で、以前取得した ID・参照は無効になります。ID に所有者や世代の検査はありません。
	/// @remark 親は子より先に登録します。親の変更も、変更する部品より先に登録した部品に限ります。この規約により循環を作らず登録順で変換を合成できます。
	/// @remark 配置行列と合成後の行列は有限のアフィン変換で、変換後の頂点属性は float で表現できる必要があります。これらの数値的な事前条件は検査しません。
	/// @remark Result で報告する失敗は Fail レベルのログにも出力します。失敗した編集操作は既存の組立データを変更しません。メモリ確保の例外は Result では扱いません。
	/// @code
	/// Mesh3DAssembly assembly;
	/// const auto wheel = assembly.addMesh(Mesh3D::Cylinder(0.4, 0.2)).value();
	/// const auto axle = assembly.addPart({ .name = U"axle", .placement = Vec3{ 0, 0.4, 0 } }).value();
	/// const auto left = assembly.addPart({ .name = U"left wheel", .mesh = wheel, .parent = axle,
	///     .placement = Mesh3DPlacement{ Vec3{ -1, 0, 0 }, Quaternion::RotateZ(90_deg) } }).value();
	/// const auto right = assembly.addPart({ .name = U"right wheel", .mesh = wheel, .parent = axle,
	///     .placement = Mesh3DPlacement{ Vec3{ 1, 0, 0 }, Quaternion::RotateZ(90_deg) } }).value();
	/// auto baked = assembly.bake().value();
	/// @endcode
	class Mesh3DAssembly
	{
	public:

		/// @brief 登録した共有形状の ID
		enum class MeshID : size_t {};

		/// @brief 登録した材質の ID
		enum class MaterialID : size_t {};

		/// @brief 登録した部品の ID。頂点・三角形のオフセットとは独立しています。
		enum class PartID : size_t {};

		/// @brief 部品の形状参照、材質参照、および親に対するローカル配置
		struct Part
		{
			/// @brief 部品名。空文字列や重複する名前も使用でき、同一性は PartID で表します。
			String name;

			/// @brief 共有形状。未指定の場合は、子を配置するための形状を持たない部品です。
			Optional<MeshID> mesh;

			/// @brief この部品の材質。未指定は材質未割り当てを表し、親からの継承は行いません。
			Optional<MaterialID> material;

			/// @brief 親部品。未指定の場合は Assembly の座標系に直接配置します。
			Optional<PartID> parent;

			/// @brief 親に対するローカル変換。ローカル原点が回転中心です。world = local * parentWorld の順で合成します。
			/// @remark 任意のヒンジ位置を使う場合は、その位置に形状なしの親を置き、形状を子として相対配置します。
			Mesh3DPlacement placement{ Mat4x4::Identity() };
		};

		/// @brief 焼き込み時点の部品情報
		struct BakedPart
		{
			/// @brief 焼き込み元の部品 ID
			PartID id{};

			/// @brief 部品名のコピー
			String name;

			/// @brief 焼き込み元の親 ID
			Optional<PartID> parent;

			/// @brief BakedMesh::materials 内の材質 ID。未指定は材質未割り当てです。
			Optional<MaterialID> material;

			/// @brief この部品自身の頂点・三角形範囲。子の範囲は含みません。形状なしの部品は空範囲になります。
			Mesh3DRange range;

			/// @brief 焼き込み時点の Assembly 座標系への変換
			Mat4x4 worldTransform{ Mat4x4::Identity() };
		};

		/// @brief 焼き込んだメッシュと、それに対応する部品・材質の独立したスナップショット
		/// @remark Assembly を変更・破棄しても有効です。mesh のトポロジーを直接変更した場合、parts の範囲は自動更新されません。
		struct BakedMesh
		{
			/// @brief Assembly の座標系へ変換し、部品の登録順に結合したメッシュ
			Mesh3D mesh;

			/// @brief 形状なしの部品を含む全部品。配列の位置は PartID の整数値と一致します。
			Array<BakedPart> parts;

			/// @brief 未使用の材質も含む全登録材質。配列の位置は MaterialID の整数値と一致します。
			Array<Material> materials;
		};

		/// @brief 空の組立データを作成します。
		Mesh3DAssembly() = default;

		/// @brief 各登録配列の容量を確保します。形状内部の頂点・三角形の容量は変更しません。
		/// @param meshCapacity 共有形状の容量
		/// @param materialCapacity 材質の容量
		/// @param partCapacity 部品の容量
		void reserve(size_t meshCapacity, size_t materialCapacity, size_t partCapacity);

		/// @brief 全登録を削除します。登録配列の容量を保持します。既存 ID は無効になります。
		void clear() noexcept;

		/// @brief 共有形状を所有ストレージへ登録します。
		/// @param mesh 登録する形状。右辺値のストレージは移動し、左辺値はコピーします。
		/// @return 形状 ID。空の形状や不正な index は InvalidGeometry
		/// @remark 登録時に isEmpty() と validate() を検査します。非有限値、縮退面、法線、UV などの検査は行いません。
		[[nodiscard]]
		Result<MeshID, Mesh3DError> addMesh(Mesh3D mesh);

		/// @brief 共有形状を差し替えます。この ID を参照する全部品の次の焼き込みに反映されます。
		/// @param id 差し替える形状 ID
		/// @param mesh 新しい形状。検査規約は addMesh() と同じです。
		/// @return 成功。存在しない ID は InvalidArgument、形状不正は InvalidGeometry
		[[nodiscard]]
		Result<void, Mesh3DError> setMesh(MeshID id, Mesh3D mesh);

		/// @brief 材質を登録します。材質の値はそのまま保持し、出力形式固有の検査は行いません。
		/// @param material 登録する材質
		/// @return 材質 ID
		[[nodiscard]]
		MaterialID addMaterial(Material material);

		/// @brief 材質を差し替えます。この ID を参照する全部品の次の焼き込みに反映されます。
		/// @param id 材質 ID
		/// @param material 新しい材質
		/// @return 成功。存在しない ID は InvalidArgument
		[[nodiscard]]
		Result<void, Mesh3DError> setMaterial(MaterialID id, Material material);

		/// @brief 部品を登録します。形状の頂点を複製しません。
		/// @param part 名前、共有形状、材質、親、および配置。指定した ID は登録済みである必要があります。
		/// @return 部品 ID。存在しない参照先は InvalidArgument
		[[nodiscard]]
		Result<PartID, Mesh3DError> addPart(Part part);

		/// @brief 部品情報を差し替えます。部品 ID と、その子からの参照は維持します。
		/// @param id 差し替える部品 ID
		/// @param part 新しい部品情報。親はこの部品より先に登録された部品に限ります。
		/// @return 成功。存在しない参照先、不正な親、または存在しない id は InvalidArgument
		[[nodiscard]]
		Result<void, Mesh3DError> setPart(PartID id, Part part);

		/// @brief 部品のローカル配置だけを変更します。頂点の変換・再生成は行いません。
		/// @param id 配置を変更する部品 ID
		/// @param placement 親に対する配置。数値的な事前条件はクラスの説明を参照してください。
		/// @return 成功。存在しない ID は InvalidArgument
		[[nodiscard]]
		Result<void, Mesh3DError> setPlacement(PartID id, const Mesh3DPlacement& placement);

		/// @brief 共有形状を参照します。
		/// @param id 形状 ID
		/// @return 形状へのポインタ。存在しない場合 nullptr。形状の追加・差し替えで無効になる場合があります。
		[[nodiscard]]
		const Mesh3D* getMesh(MeshID id) const noexcept;

		/// @brief 材質を参照します。
		/// @param id 材質 ID
		/// @return 材質へのポインタ。存在しない場合 nullptr。材質の追加・差し替えで無効になる場合があります。
		[[nodiscard]]
		const Material* getMaterial(MaterialID id) const noexcept;

		/// @brief 部品を参照します。
		/// @param id 部品 ID
		/// @return 部品へのポインタ。存在しない場合 nullptr。部品の追加・編集で無効になる場合があります。
		[[nodiscard]]
		const Part* getPart(PartID id) const noexcept;

		/// @brief 登録した共有形状数を返します。
		[[nodiscard]]
		size_t meshCount() const noexcept;

		/// @brief 登録した材質数を返します。
		[[nodiscard]]
		size_t materialCount() const noexcept;

		/// @brief 形状なしの部品を含む登録部品数を返します。
		[[nodiscard]]
		size_t partCount() const noexcept;

		/// @brief 部品のローカル座標から Assembly の座標系への変換を計算します。
		/// @param id 部品 ID
		/// @return 合成した変換。存在しない ID は InvalidArgument
		/// @remark 親の深さに比例した時間で、再帰と動的メモリ確保を使わずに計算します。
		[[nodiscard]]
		Result<Mat4x4, Mesh3DError> computeWorldTransform(PartID id) const;

		/// @brief 全部品を Assembly の座標系へ焼き込みます。
		/// @param destination 出力先。既存のメッシュ・部品・材質配列の容量を再利用します。
		/// @param options 出力サイズの上限。未使用の共有形状は出力サイズに含めません。
		/// @return 成功。サイズ上限を超える場合は SizeLimit で失敗し、destination は変更しません。
		/// @remark 形状の index は登録時に検査済みのため、部品ごとの再検査を行いません。計算量は全部品数と出力の頂点・三角形数に比例します。
		/// @remark 法線・接線の変換は Mesh3D::append(mesh, matrix) と同じです。鏡映を含む部品では追加で三角形の巻き順を反転し、表裏を維持します。
		/// @remark 特異な変換では法線・接線を変更しません。空の Assembly、または形状なしの部品だけの場合も成功します。
		[[nodiscard]]
		Result<void, Mesh3DError> bake(BakedMesh& destination, const Mesh3DBakeOptions& options = {}) const;

		/// @brief 全部品を新しい焼き込み結果として取得します。
		/// @param options 出力サイズの上限
		/// @return 焼き込み結果、または SizeLimit のエラー
		/// @remark 出力先を受け取る bake() を使用する便利関数です。
		[[nodiscard]]
		Result<BakedMesh, Mesh3DError> bake(const Mesh3DBakeOptions& options = {}) const;

	private:

		Array<Mesh3D> m_meshes;
		Array<Material> m_materials;
		Array<Part> m_parts;

		[[nodiscard]]
		Result<void, Mesh3DError> checkPart(const Part& part, size_t parentLimit) const;
	};
}
