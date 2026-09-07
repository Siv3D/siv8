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
# include <utility>
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

		/// @brief 部分階層の複製結果。形状と材質の ID は複製元と共有します。
		struct ClonedSubtree
		{
			/// @brief 新しい根の部品 ID
			PartID root{};

			/// @brief 複製元・複製先の PartID の組。複製元 ID の昇順で、根を含む複製した部品だけを格納します。
			/// @remark find() を使用する場合は昇順と複製元 ID の一意性を保ってください。Assembly の clear() などによる ID の無効化は検出しません。
			Array<std::pair<PartID, PartID>> parts;

			/// @brief 元の部品 ID に対応する複製先の部品 ID を取得します。
			/// @param source 複製元の部品 ID
			/// @return 対応する ID。複製対象外の場合 none
			/// @remark 複製部品数に対して対数時間で検索し、動的メモリを確保しません。
			[[nodiscard]]
			Optional<PartID> find(PartID source) const noexcept;
		};

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

			/// @brief 部品と材質をまとめて 1 組の OBJ / MTL ファイルへ保存します。
			/// @param path OBJ の保存先。空のベース名と .mtl 拡張子は使用できません。
			/// @return 保存に成功した場合 true。入力不正または書き込み失敗の場合 false
			/// @remark MTL は同じディレクトリへ保存します。ベース名の UTF-8 バイトのうち英数字・`_`・`-`・`.` 以外を `%HH` に変換し、`.mtl` を付けた名前を使います。
			/// @remark 部品・材質・座標の出力規約と入力の検証規約は encodeOBJ() と同じです。入力不正の場合はファイルを開きません。I/O 失敗の場合は途中までのファイルが残ることがあります。
			/// @remark 相対テクスチャパスは MTL のディレクトリを基準にそのまま出力し、テクスチャファイル自体はコピーしません。
			bool saveOBJ(FilePathView path) const;

			/// @brief 部品と材質を OBJ / MTL 形式で 2 つの Writer に書き出します。
			/// @param objWriter OBJ の書き出し先
			/// @param mtlWriter MTL の書き出し先。objWriter とは別の出力先である必要があります。
			/// @param mtlFileName OBJ に記載する MTL ファイル名。空文字列、`.`、`..` は使用できず、ASCII の英数字・`_`・`-`・`.`・`%` のみ使用できます。
			/// @return 書き出しに成功した場合 true。入力不正、Writer が閉じている、同一 Writer、または書き込み失敗の場合 false
			/// @remark 非空の有効な mesh が必要です。parts は頂点・三角形を登録順で隙間なく分割し、各 part.id は配列位置と一致する必要があります。各三角形はその部品の頂点範囲のみを参照します。
			/// @remark 三角形を持つ部品だけを `g part_<ID>_<名前>` として出力します。材質名は `material_<ID>_<名前>` です。名前は UTF-8 バイトを saveOBJ() と同じ `%HH` 規約で変換し、空の名前では末尾の `_` と名前を省略します。
			/// @remark 使用する材質だけを ID 順に MTL へ出力します。材質未指定の部品には `material_default`（Material の既定値）を明示的に割り当てます。
			/// @remark 色・金属度・粗さ・テクスチャなどは Mesh3D::saveOBJ(path, material) と同じ変換規約です。金属度が 0 の場合、baseColor.rgb は MTL の Kd にそのまま出力され、テクスチャなしで色分けできます。
			/// @remark 参照する材質と OBJ の頂点属性を検証します。材質名は上記の変換を行うため、空文字列や制御文字を含む名前も使用できます。未使用の材質は検証しません。
			/// @remark 座標・法線の Z と三角形の巻き順を反転し、UV の V を `1 - V` にして UTF-8（BOM なし）・LF で出力します。部品の worldTransform を再適用しません。階層、共有形状、接線は保存しません。
			/// @remark 入力不正の場合は Writer に書き込みません。書き込み中に失敗した場合は部分的な出力が残ります。失敗理由は Fail レベルのログに出力します。
			bool encodeOBJ(IWriter& objWriter, IWriter& mtlWriter, StringView mtlFileName) const;
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

		/// @brief 根とその全子孫を、形状・材質を共有する独立した部品階層として複製します。
		/// @param root 複製元の根の部品 ID
		/// @param placement 新しい根の親に対する配置。元の根の配置を置き換えます。
		/// @param parent 新しい根の親。未指定の場合は Assembly の座標系に直接配置します。
		/// @return 新しい根と部品 ID の対応。存在しない root / parent は InvalidArgument、部品数の上限超過は SizeLimit
		/// @remark 名前、MeshID、MaterialID、子孫のローカル配置をコピーし、内部の親参照を複製先へ付け替えます。形状・材質の登録や頂点のコピーは行いません。配置の変更は独立し、setMesh() / setMaterial() による共有データの変更は両方へ反映されます。
		/// @remark 呼び出し開始時点の子孫だけを複製します。登録順で他の階層が間にあっても対象を選別します。parent は複製元の根や子孫も指定でき、元の部品の子として複製します。
		/// @remark 新しい部品は元の登録順で末尾へ追加され、既存 ID は変わりません。getPart() で取得したポインタは追加による再確保で無効になる場合があります。world 配置を維持する reparent 操作や、後からの階層編集の同期は行いません。
		/// @code
		/// const auto parent = assembly.addPart({ .placement = Vec3{ 10, 0, 0 } }).value();
		/// const auto root = assembly.addPart({ .parent = parent, .placement = Vec3{ 2, 0, 0 } }).value();
		/// const auto copy = assembly.cloneSubtree(root, Vec3{ 5, 0, 0 }, parent).value();
		/// // 元の根の world 位置は (12, 0, 0)、複製先は (15, 0, 0)。
		/// // parent を省略すると複製先の world 位置は (5, 0, 0)。
		/// @endcode
		/// @remark 複製元の根以降の既存部品数を N、複製する部品数を K とすると、名前のコピーと追加先配列の再確保を除く時間は O(N log(K + 1))、補助ストレージは O(K) です。複製を準備してから一括追加し、失敗時は既存の組立データを変更しません。
		[[nodiscard]]
		Result<ClonedSubtree, Mesh3DError> cloneSubtree(PartID root, const Mesh3DPlacement& placement, Optional<PartID> parent = none);

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
		/// @remark 法線・接線・巻き順の変換は Mesh3D::append(mesh, matrix) と同じです。鏡映を含む部品も表裏を維持します。
		/// @remark 特異な変換では法線・接線を変更しません。空の Assembly、または形状なしの部品だけの場合も成功します。
		[[nodiscard]]
		Result<void, Mesh3DError> bake(BakedMesh& destination, const Mesh3DBakeOptions& options = {}) const;

		/// @brief 全部品を新しい焼き込み結果として取得します。
		/// @param options 出力サイズの上限
		/// @return 焼き込み結果、または SizeLimit のエラー
		/// @remark 出力先を受け取る bake() を使用する便利関数です。
		[[nodiscard]]
		Result<BakedMesh, Mesh3DError> bake(const Mesh3DBakeOptions& options = {}) const;

		/// @brief 全部品を焼き込み、部品と材質をまとめて 1 組の OBJ / MTL ファイルへ保存します。
		/// @param path OBJ の保存先
		/// @param options 焼き込みの出力サイズ上限
		/// @return 焼き込みと保存に成功した場合 true、それ以外の場合 false
		/// @remark bake() と BakedMesh::saveOBJ() を使用する便利関数です。出力規約は BakedMesh::saveOBJ() と同じです。
		bool saveOBJ(FilePathView path, const Mesh3DBakeOptions& options = {}) const;

	private:

		Array<Mesh3D> m_meshes;
		Array<Material> m_materials;
		Array<Part> m_parts;

		[[nodiscard]]
		Result<void, Mesh3DError> checkPart(const Part& part, size_t parentLimit) const;
	};
}
