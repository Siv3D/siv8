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
# include <initializer_list>
# include <span>
# include "Common.hpp"
# include "Array.hpp"
# include "Blob.hpp"
# include "BoxFace.hpp"
# include "BoxUVMapping.hpp"
# include "Grid.hpp"
# include "IWriter.hpp"
# include "Material.hpp"
# include "Optional.hpp"
# include "PredefinedNamedParameter.hpp"
# include "PredefinedYesNo.hpp"
# include "Result.hpp"
# include "String.hpp"
# include "Vertex3D.hpp"
# include "TriangleIndex32.hpp"
# include "VertexNormalWeighting.hpp"

namespace s3d
{
	class Polygon;
	struct Mat3x2;
	struct Mat4x4;
	struct Quaternion;

	////////////////////////////////////////////////////////////////
	//
	//	Mesh3DRange
	//
	////////////////////////////////////////////////////////////////

	/// @brief 3D メッシュ内の連続した頂点範囲と三角形範囲
	/// @remark 三角形範囲内のインデックスは Mesh3D 全体の頂点インデックスであり、vertexOffset を基準とする相対値ではありません。
	struct Mesh3DRange
	{
		/// @brief 頂点範囲の先頭オフセット
		size_t vertexOffset = 0;

		/// @brief 頂点数
		size_t vertexCount = 0;

		/// @brief 三角形範囲の先頭オフセット
		size_t triangleOffset = 0;

		/// @brief 三角形数
		size_t triangleCount = 0;

		/// @brief 頂点と三角形を持たない範囲であるかを返します。
		/// @return 頂点数と三角形数がともに 0 の場合 true, それ以外の場合は false
		[[nodiscard]]
		constexpr bool isEmpty() const noexcept
		{
			return ((vertexCount == 0) && (triangleCount == 0));
		}
	};

	////////////////////////////////////////////////////////////////
	//
	//	Mesh3DErrorCode
	//
	////////////////////////////////////////////////////////////////

	/// @brief 3D メッシュ生成エラーの分類
	enum class Mesh3DErrorCode : uint8
	{
		/// @brief 引数が不正
		InvalidArgument,

		/// @brief 入力された幾何形状から安定したメッシュを生成できない
		InvalidGeometry,

		/// @brief 入力値が非有限、または入力値・生成値を float で表現できない
		NumericRange,

		/// @brief 生成後のメッシュがサポートされるサイズを超える
		SizeLimit,
	};

	////////////////////////////////////////////////////////////////
	//
	//	Mesh3DError
	//
	////////////////////////////////////////////////////////////////

	/// @brief 3D メッシュ生成エラー
	struct Mesh3DError
	{
		/// @brief エラーの分類
		Mesh3DErrorCode code = Mesh3DErrorCode::InvalidArgument;

		/// @brief 診断用のエラー詳細。エラーの判定には code を使用します。
		String message;
	};

	/// @brief Mesh3DBuilder の add 関数による追加結果
	/// @remark 成功時は追加された範囲、失敗時はエラーを保持します。
	/// @remark `has_value()` または bool への変換で、追加に成功したかを確認できます。
	using Mesh3DAddResult = Result<Mesh3DRange, Mesh3DError>;

	////////////////////////////////////////////////////////////////
	//
	//	SweepSectionTransform
	//
	////////////////////////////////////////////////////////////////

	/// @brief Sweep の経路点における断面変換
	struct SweepSectionTransform
	{
		/// @brief 断面の X 軸方向および Y 軸方向の拡大率
		Vec2 scale = Vec2{ 1.0, 1.0 };

		/// @brief 経路の接線を軸とする断面の回転角（ラジアン）
		double twist = 0.0;
	};

	////////////////////////////////////////////////////////////////
	//
	//	SweepOptions
	//
	////////////////////////////////////////////////////////////////

	/// @brief 経路点ごとの断面変換を使う Sweep の設定
	struct SweepOptions
	{
		/// @brief 開始時に断面の X 軸を向ける方向。未指定の場合は経路から自動的に決定します。
		Optional<Vec3> initialXAxis;

		/// @brief UV 座標の拡大率
		Vec2 uvScale = Vec2{ 1.0, 1.0 };

		/// @brief UV 座標のオフセット
		Vec2 uvOffset = Vec2{ 0.0, 0.0 };

		/// @brief 経路の末尾と先頭を接続するか
		CloseRing closeRing = CloseRing::No;
	};

	////////////////////////////////////////////////////////////////
	//
	//	Mesh3D
	//
	////////////////////////////////////////////////////////////////

	/// @brief 3D メッシュデータ
	/// @remark 生成関数の `uvScale` と `uvOffset` は、各生成関数が定める基礎 UV 座標に対して、成分ごとに `uv = (baseUV * uvScale) + uvOffset` として適用されます。
	/// @remark 基礎 UV 座標の範囲は形状ごとに異なります。通常は `[0, 1]` に正規化されますが、`Tube()`、`Sweep()`、`Loft()` の経路方向または高さ方向には実距離が使われます。
	/// @remark 生成関数が不正な引数やサイズ上限などにより生成できない場合、理由を Fail レベルのエンジンログへ出力し、空の 3D メッシュを返します。
	/// @par 座標と配置
	/// - 3D 座標は左手系の Y-up です。`Polygon` や断面の 2D 座標 `(x, y)` は、水平面では原則として `(X, -Z)` に対応します。
	/// - Y 軸周りの角度は `Quaternion::RotateY()` と同じ規約を使い、0 は `+X` 方向、正の角度は `+X` から `-Z` へ進みます。
	/// - 基本プリミティブ、`Extrude()`、`Plane()`、`Grid()` は、各関数で明記された軸について原点を中心に生成します。
	/// - `Revolve()` のプロファイルの Y 座標、`Loft()` の `heights`、`HeightField()` の各高さ、および `Tube()` / `Sweep()` の経路座標は、平行移動せず生成後の座標として使用します。
	/// - `Mesh3DBuilder` の offset と rotation を受け取る overload は、原点を中心に回転してから offset を加えます。
	/// @par 2D 輪郭の頂点順序
	/// - `Polygon` の外周、および `Loft()` の各断面は、末尾から先頭へ戻る辺を含む `Σ(x[i] * y[i+1] - x[i+1] * y[i])` が正になる順序で指定します。画面座標では時計回りに見える順序です。
	/// - `Polygon` の穴は同じ式の値が負になる順序で指定します。画面座標では反時計回りに見える順序です。各輪郭では先頭頂点を末尾に重複させません。
	/// @par UV 座標と頂点属性
	/// - UV 座標は画像の上端を V = 0、下端を V = 1 とします。
	/// - 生成関数は位置、UV 座標、法線、および接線を設定します。法線と接線は単位長で互いに直交し、`tangent.w` は `bitangent() = Math::Cross(normal, tangent.xyz()) * tangent.w` の向きを表します。
	/// @par 断面とフレーム
	/// - `Sweep()` の各断面点 `(x, y)` は、経路上の位置を P、フレームの第 1 軸を N、経路の接線を T として、`P + N * x + (N.cross(T)) * y` に配置されます。
	/// - UV seam、ハードエッジ、および端面との境界では、位置が同じ頂点を属性ごとに複製することがあります。
	/// @par 複数形状の合成
	/// - 独立して生成した閉じた形状の同じ向きの面を同一平面上で重ねると、z-fighting が発生することがあります。不要な面を生成しないか、一方をわずかに交差させて配置してください。
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
		/// @remark `vertexCount` が `MaxVertexCount` を超える場合は空の 3D メッシュを作成します。
		[[nodiscard]]
		Mesh3D(size_t vertexCount, size_t triangleCount);

		/// @brief 頂点配列と三角形インデックス配列から 3D メッシュを作成します。
		/// @param _vertices 頂点配列
		/// @param _indices 三角形インデックス配列
		/// @remark `_vertices` の要素数が `MaxVertexCount` を超える場合は空の 3D メッシュを作成します。
		[[nodiscard]]
		Mesh3D(Array<Vertex3D> _vertices, Array<TriangleIndex32> _indices);
		
		/// @brief 頂点と三角形インデックスの範囲から 3D メッシュを作成します。
		/// @param _vertices コピーする頂点の範囲
		/// @param _indices コピーする三角形インデックスの範囲
		/// @remark `_vertices` の要素数が `MaxVertexCount` を超える場合は空の 3D メッシュを作成します。
		[[nodiscard]]
		Mesh3D(std::span<const Vertex3D> _vertices, std::span<const TriangleIndex32> _indices);

		////////////////////////////////////////////////////////////////
		//
		//	Box
		//
		////////////////////////////////////////////////////////////////

		/// @brief 原点を中心とする直方体の 3D メッシュを作成します。
		/// @param size 直方体の各軸方向の大きさ
		/// @param faces 生成する面
		/// @return 直方体の 3D メッシュ。`size` のいずれかの成分が正の有限値でない場合、または float で表現できない場合は空の 3D メッシュ
		/// @remark 各面は独立した頂点を持ち、面ごとに `[0, 1]` の UV 座標が割り当てられます。
		/// @remark `faces == BoxFace::None_` の場合は空の 3D メッシュを返します。
		[[nodiscard]]
		static Mesh3D Box(
			Vec3 size = Vec3{ 1.0, 1.0, 1.0 },
			BoxFace faces = BoxFace::All);

		/// @brief 原点を中心とする直方体の 3D メッシュを作成します。
		/// @param size 直方体の各軸方向の大きさ
		/// @param uvMapping 各面に割り当てる UV 矩形
		/// @param faces 生成する面
		/// @return 直方体の 3D メッシュ。`size` または `uvMapping` が不正な場合は空の 3D メッシュ
		/// @remark 各面は独立した頂点を持ち、`uvMapping` の対応する矩形が割り当てられます。
		/// @remark `uvMapping` は `faces` で選択した面に対応する矩形のみ検証されます。
		/// @remark `faces == BoxFace::None_` の場合は空の 3D メッシュを返します。
		[[nodiscard]]
		static Mesh3D Box(
			Vec3 size,
			const BoxUVMapping& uvMapping,
			BoxFace faces = BoxFace::All);

		////////////////////////////////////////////////////////////////
		//
		//	BoxShell
		//
		////////////////////////////////////////////////////////////////

		/// @brief 原点を中心とする、均一な厚みを持つ中空直方体の 3D メッシュを作成します。
		/// @param outerSize 外側の直方体の各軸方向の大きさ
		/// @param thickness 壁の厚み。正の有限値で、`outerSize` の最小成分の半分未満である必要があります。
		/// @param openFaces 壁を生成しない開口面
		/// @return 中空直方体の 3D メッシュ。引数が不正な場合は空の 3D メッシュ
		/// @remark 閉じた面には外面と内面を生成し、開口部には残る壁の厚みが見える縁面を生成します。内部面や重複面は生成しません。
		/// @remark UV 座標は、形状全体の外接 Box に対する平面投影で割り当てられます。
		/// @remark `openFaces == BoxFace::All` の場合は空の 3D メッシュを返します。
		[[nodiscard]]
		static Mesh3D BoxShell(
			Vec3 outerSize = Vec3{ 1.0, 1.0, 1.0 },
			double thickness = 0.1,
			BoxFace openFaces = BoxFace::None_);

		/// @brief 原点を中心とする、軸ごとの厚みを持つ中空直方体の 3D メッシュを作成します。
		/// @param outerSize 外側の直方体の各軸方向の大きさ
		/// @param thickness 各軸に垂直な壁の厚み。各成分は正の有限値で、対応する `outerSize` 成分の半分未満である必要があります。
		/// @param openFaces 壁を生成しない開口面
		/// @return 中空直方体の 3D メッシュ。引数が不正な場合は空の 3D メッシュ
		/// @remark 閉じた面には外面と内面を生成し、開口部には残る壁の厚みが見える縁面を生成します。内部面や重複面は生成しません。
		/// @remark UV 座標は、形状全体の外接 Box に対する平面投影で割り当てられます。
		/// @remark `openFaces == BoxFace::All` の場合は空の 3D メッシュを返します。
		[[nodiscard]]
		static Mesh3D BoxShell(
			Vec3 outerSize,
			Vec3 thickness,
			BoxFace openFaces = BoxFace::None_);

		/// @brief 指定した UV マッピングを持つ、均一な厚みの中空直方体の 3D メッシュを作成します。
		/// @param outerSize 外側の直方体の各軸方向の大きさ
		/// @param thickness 壁の厚み
		/// @param uvMapping 外接 Box の各投影面に割り当てる UV 矩形
		/// @param openFaces 壁を生成しない開口面
		/// @return 中空直方体の 3D メッシュ。引数または使用する UV 矩形が不正な場合は空の 3D メッシュ
		[[nodiscard]]
		static Mesh3D BoxShell(
			Vec3 outerSize,
			double thickness,
			const BoxUVMapping& uvMapping,
			BoxFace openFaces = BoxFace::None_);

		/// @brief 指定した UV マッピングを持つ、軸ごとの厚みの中空直方体の 3D メッシュを作成します。
		/// @param outerSize 外側の直方体の各軸方向の大きさ
		/// @param thickness 各軸に垂直な壁の厚み
		/// @param uvMapping 外接 Box の各投影面に割り当てる UV 矩形
		/// @param openFaces 壁を生成しない開口面
		/// @return 中空直方体の 3D メッシュ。引数または使用する UV 矩形が不正な場合は空の 3D メッシュ
		[[nodiscard]]
		static Mesh3D BoxShell(
			Vec3 outerSize,
			Vec3 thickness,
			const BoxUVMapping& uvMapping,
			BoxFace openFaces = BoxFace::None_);

		////////////////////////////////////////////////////////////////
		//
		//	BoxFrame
		//
		////////////////////////////////////////////////////////////////

		/// @brief 原点を中心とする直方体の 12 辺を、均一な太さの角材で構成した枠の 3D メッシュを作成します。
		/// @param size 枠の外側の各軸方向の大きさ
		/// @param thickness 角材の太さ。正の有限値で、`size` の最小成分の半分未満である必要があります。
		/// @return 直方体枠の 3D メッシュ。引数が不正な場合は空の 3D メッシュ
		/// @remark 角材同士の接合部にある内部面や重複面は生成しません。
		/// @remark UV 座標は、形状全体の外接 Box に対する平面投影で割り当てられます。
		[[nodiscard]]
		static Mesh3D BoxFrame(
			Vec3 size = Vec3{ 1.0, 1.0, 1.0 },
			double thickness = 0.1);

		/// @brief 原点を中心とする直方体の 12 辺を、軸ごとの太さを持つ角材で構成した枠の 3D メッシュを作成します。
		/// @param size 枠の外側の各軸方向の大きさ
		/// @param beamSize 角材の軸ごとの太さ。各成分は正の有限値で、対応する `size` 成分の半分未満である必要があります。
		/// @return 直方体枠の 3D メッシュ。引数が不正な場合は空の 3D メッシュ
		/// @remark X 方向の角材は `(size.x, beamSize.y, beamSize.z)`、Y/Z 方向の角材も同様の大きさになります。
		/// @remark 角材同士の接合部にある内部面や重複面は生成しません。
		/// @remark UV 座標は、形状全体の外接 Box に対する平面投影で割り当てられます。
		[[nodiscard]]
		static Mesh3D BoxFrame(Vec3 size, Vec3 beamSize);

		/// @brief 指定した UV マッピングを持つ、均一な太さの直方体枠の 3D メッシュを作成します。
		/// @param size 枠の外側の各軸方向の大きさ
		/// @param thickness 角材の太さ
		/// @param uvMapping 外接 Box の各投影面に割り当てる UV 矩形
		/// @return 直方体枠の 3D メッシュ。引数または `uvMapping` が不正な場合は空の 3D メッシュ
		[[nodiscard]]
		static Mesh3D BoxFrame(
			Vec3 size,
			double thickness,
			const BoxUVMapping& uvMapping);

		/// @brief 指定した UV マッピングを持つ、軸ごとの太さを持つ直方体枠の 3D メッシュを作成します。
		/// @param size 枠の外側の各軸方向の大きさ
		/// @param beamSize 角材の軸ごとの太さ
		/// @param uvMapping 外接 Box の各投影面に割り当てる UV 矩形
		/// @return 直方体枠の 3D メッシュ。引数または `uvMapping` が不正な場合は空の 3D メッシュ
		[[nodiscard]]
		static Mesh3D BoxFrame(
			Vec3 size,
			Vec3 beamSize,
			const BoxUVMapping& uvMapping);

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
		//	ChamferedBox
		//
		////////////////////////////////////////////////////////////////

		/// @brief 原点を中心とする面取り直方体の 3D メッシュを作成します。
		/// @param size 面取り直方体の各軸方向の大きさ
		/// @param chamfer 面取り幅。0 以上、`size` の最小成分の半分未満である必要があります。
		/// @return 面取り直方体の 3D メッシュ。引数が不正な場合は空の 3D メッシュ
		/// @remark `chamfer == 0` の場合は `Box(size)` と同じ 3D メッシュを返します。
		/// @remark すべての面は平面で、面ごとに独立した頂点とハードエッジを持ちます。
		/// @remark UV 座標は、形状全体の外接 Box に対する平面投影で割り当てられます。
		[[nodiscard]]
		static Mesh3D ChamferedBox(
			Vec3 size = Vec3{ 1.0, 1.0, 1.0 },
			double chamfer = 0.1);

		/// @brief 原点を中心とする面取り直方体の 3D メッシュを作成します。
		/// @param size 面取り直方体の各軸方向の大きさ
		/// @param chamfer 面取り幅。0 以上、`size` の最小成分の半分未満である必要があります。
		/// @param uvMapping 各投影面に割り当てる UV 矩形
		/// @return 面取り直方体の 3D メッシュ。引数または `uvMapping` が不正な場合は空の 3D メッシュ
		/// @remark `chamfer == 0` の場合は `Box(size, uvMapping)` と同じ 3D メッシュを返します。
		/// @remark 辺面と角面の投影軸が複数同率になる場合は、X 軸、Y 軸、Z 軸の順に優先します。
		[[nodiscard]]
		static Mesh3D ChamferedBox(
			Vec3 size,
			double chamfer,
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
		/// @remark 閉じたプロファイルを表す先頭・末尾の一致を除き、float 変換後に連続する 2 点が同じになるプロファイルは無効です。
		/// @remark 外側輪郭を Y 座標の小さい側から大きい側へ並べると外向きの面になります。プロファイルの順序を反転すると面と法線の向きも反転します。
		/// @remark UV 座標の U は `+X` 方向を 0 として Y 軸周りの正の回転方向（`-Z` 方向）へ増加し、V はプロファイルの累積距離を `[0, 1]` に正規化した値です。
		/// @remark プロファイルの自己交差および回転後の自己交差は検査しません。
		[[nodiscard]]
		static Mesh3D Revolve(std::span<const Vec2> profile, uint32 segments = 32);

		/// @brief 2D プロファイルを Y 軸の周りに一周回転させた 3D メッシュを作成します。
		/// @param profile 回転させるプロファイル。各要素の X 座標を半径、Y 座標を生成後の Y 座標として使用します。
		/// @param segments 回転方向の分割数。3 以上である必要があります。
		/// @return 回転体の 3D メッシュ。引数が不正な場合、または頂点数が上限を超える場合は空の 3D メッシュ
		/// @remark プロファイル、開口、および UV 座標の規約は `std::span` を受け取るオーバーロードと同じです。
		[[nodiscard]]
		static Mesh3D Revolve(std::initializer_list<Vec2> profile, uint32 segments = 32);

		/// @brief 2D プロファイルを Y 軸の周りに一周回転させ、プロファイル方向の法線を角度に応じて補間した 3D メッシュを作成します。
		/// @param profile 回転させるプロファイル。各要素の X 座標を半径、Y 座標を生成後の Y 座標として使用します。
		/// @param segments 回転方向の分割数。3 以上である必要があります。
		/// @param smoothingAngle プロファイル方向の法線を補間する隣接面間の最大角度（ラジアン）。0 以上 π 以下
		/// @return 回転体の 3D メッシュ。引数が不正な場合、または頂点数が上限を超える場合は空の 3D メッシュ
		/// @remark `smoothingAngle` 以下の角度で接続するプロファイル線分間では、共有するプロファイル頂点の法線を補間します。回転方向の法線は常に滑らかに接続されます。
		/// @remark 座標、開口、および UV 座標の規約は 2 引数版の `Revolve()` と同じです。
		[[nodiscard]]
		static Mesh3D Revolve(std::span<const Vec2> profile, uint32 segments, double smoothingAngle);

		/// @brief 2D プロファイルを Y 軸の周りに一周回転させ、プロファイル方向の法線を角度に応じて補間した 3D メッシュを作成します。
		/// @param profile 回転させるプロファイル。各要素の X 座標を半径、Y 座標を生成後の Y 座標として使用します。
		/// @param segments 回転方向の分割数。3 以上である必要があります。
		/// @param smoothingAngle プロファイル方向の法線を補間する隣接面間の最大角度（ラジアン）。0 以上 π 以下
		/// @return 回転体の 3D メッシュ。引数が不正な場合、または頂点数が上限を超える場合は空の 3D メッシュ
		/// @remark プロファイル、開口、および UV 座標の規約は `std::span` を受け取るオーバーロードと同じです。
		[[nodiscard]]
		static Mesh3D Revolve(std::initializer_list<Vec2> profile, uint32 segments, double smoothingAngle);

		/// @brief 2D プロファイルを Y 軸の周りに指定した角度だけ回転させた 3D メッシュを作成します。
		/// @param profile 回転させるプロファイル。各要素の X 座標を半径、Y 座標を生成後の Y 座標として使用します。
		/// @param startAngle 回転を開始する角度（ラジアン）。0 は `+X` 方向です。
		/// @param sweepAngle Y 軸周りの正の回転方向（`+X` から `-Z`）へ進む角度（ラジアン）。0 より大きく 2π 以下である必要があります。
		/// @param segments 回転方向の分割数。部分回転では 1 以上、完全な一周では 3 以上である必要があります。
		/// @param closeEnds 回転方向の始端と終端を閉じる場合は `CloseEnds::Yes`、端面を作成しない場合は `CloseEnds::No`
		/// @return 回転体の 3D メッシュ。引数が不正な場合、または頂点数が上限を超える場合は空の 3D メッシュ
		/// @remark `CloseEnds::Yes` の場合、プロファイルを直線で閉じた単純多角形として解釈し、回転方向の始端と終端に平面の端面を作成します。端面を作成できないプロファイルの場合は生成に失敗します。
		/// @remark `sweepAngle == 2π` の場合は回転方向の端面を作成せず、継ぎ目を接続します。`startAngle == 0` なら 2 引数版の `Revolve()` と同じ形状になります。
		/// @remark プロファイル、開口、UV 座標、および法線の規約は 2 引数版の `Revolve()` と同じです。側面の U 座標は指定した回転範囲を `[0, 1]` に正規化します。
		[[nodiscard]]
		static Mesh3D Revolve(
			std::span<const Vec2> profile,
			double startAngle,
			double sweepAngle,
			uint32 segments,
			CloseEnds closeEnds = CloseEnds::No);

		/// @brief 2D プロファイルを Y 軸の周りに指定した角度だけ回転させた 3D メッシュを作成します。
		/// @param profile 回転させるプロファイル
		/// @param startAngle 回転を開始する角度（ラジアン）
		/// @param sweepAngle 回転する角度（ラジアン）
		/// @param segments 回転方向の分割数
		/// @param closeEnds 回転方向の始端と終端を閉じるかどうか
		/// @return 回転体の 3D メッシュ。引数が不正な場合、または頂点数が上限を超える場合は空の 3D メッシュ
		/// @remark プロファイル、角度、端面、および UV 座標の規約は `std::span` を受け取る角度指定オーバーロードと同じです。
		[[nodiscard]]
		static Mesh3D Revolve(
			std::initializer_list<Vec2> profile,
			double startAngle,
			double sweepAngle,
			uint32 segments,
			CloseEnds closeEnds = CloseEnds::No);

		/// @brief 2D プロファイルを Y 軸の周りに指定した角度だけ回転させ、プロファイル方向の法線を角度に応じて補間した 3D メッシュを作成します。
		/// @param profile 回転させるプロファイル
		/// @param startAngle 回転を開始する角度（ラジアン）
		/// @param sweepAngle 回転する角度（ラジアン）
		/// @param segments 回転方向の分割数
		/// @param smoothingAngle プロファイル方向の法線を補間する隣接面間の最大角度（ラジアン）。0 以上 π 以下
		/// @param closeEnds 回転方向の始端と終端を閉じるかどうか
		/// @return 回転体の 3D メッシュ。引数が不正な場合、または頂点数が上限を超える場合は空の 3D メッシュ
		/// @remark 法線の補間規約は 3 引数版の `Revolve()`、角度、端面、および UV 座標の規約は角度指定オーバーロードと同じです。
		[[nodiscard]]
		static Mesh3D Revolve(
			std::span<const Vec2> profile,
			double startAngle,
			double sweepAngle,
			uint32 segments,
			double smoothingAngle,
			CloseEnds closeEnds = CloseEnds::No);

		/// @brief 2D プロファイルを Y 軸の周りに指定した角度だけ回転させ、プロファイル方向の法線を角度に応じて補間した 3D メッシュを作成します。
		/// @param profile 回転させるプロファイル
		/// @param startAngle 回転を開始する角度（ラジアン）
		/// @param sweepAngle 回転する角度（ラジアン）
		/// @param segments 回転方向の分割数
		/// @param smoothingAngle プロファイル方向の法線を補間する隣接面間の最大角度（ラジアン）。0 以上 π 以下
		/// @param closeEnds 回転方向の始端と終端を閉じるかどうか
		/// @return 回転体の 3D メッシュ。引数が不正な場合、または頂点数が上限を超える場合は空の 3D メッシュ
		/// @remark プロファイル、法線、角度、端面、および UV 座標の規約は `std::span` を受け取る角度指定オーバーロードと同じです。
		[[nodiscard]]
		static Mesh3D Revolve(
			std::initializer_list<Vec2> profile,
			double startAngle,
			double sweepAngle,
			uint32 segments,
			double smoothingAngle,
			CloseEnds closeEnds = CloseEnds::No);

		////////////////////////////////////////////////////////////////
		//
		//	Tube
		//
		////////////////////////////////////////////////////////////////

		/// @brief 3D 経路に沿う一定半径のチューブを作成します。
		/// @param path チューブの中心を通る経路の頂点。開路は 2 点以上、閉路は 3 点以上である必要があります。
		/// @param radius チューブの半径
		/// @param sides チューブ断面の分割数。3 以上である必要があります。
		/// @param uvScale UV 座標の拡大率
		/// @param uvOffset UV 座標のオフセット
		/// @param closeRing 経路を閉じる場合は `CloseRing::Yes`、開いた経路として扱う場合は `CloseRing::No`
		/// @return 経路に沿うチューブの 3D メッシュ。引数が不正な場合、または頂点数が上限を超える場合は空の 3D メッシュ
		/// @remark parallel-transport frame を使って断面を経路に沿って運びます。折れ点の断面は前後の線分方向を平均した平面上に配置されます。
		/// @remark `CloseRing::No` の場合、始端と終端は閉じられ、側面との境界はハードエッジになります。`CloseRing::Yes` の場合は端面を作らず、末尾から先頭へ接続します。
		/// @remark 側面の U 座標は円周を `[0, 1]` とし、V 座標は経路の始端からの累積距離です。その後に `uvScale` と `uvOffset` が適用されます。
		/// @remark 端面の UV 座標は各円を `[0, 1]` 四方に収める平面投影とし、同じ `uvScale` と `uvOffset` が適用されます。
		/// @remark 折れ点では断面が前後の経路方向の二等分面に置かれるため、断面の頂点は各線分に垂直な平面上の公称位置からずれることがあります。
		/// @remark 始点と終点が float 変換後に一致する経路は無効です。閉路では始点を末尾に重複させず、3 点以上を指定します。float 変換後に連続する 2 点が同じになる経路、および 180° 折り返す経路には対応しません。鋭い折れ点における自己交差は検査しません。
		/// @remark 閉路では経路方向の UV 継ぎ目に断面を複製し、parallel-transport frame の残留回転を経路長に比例して分散します。
		[[nodiscard]]
		static Mesh3D Tube(
			std::span<const Vec3> path,
			double radius,
			uint32 sides = 12,
			Vec2 uvScale = Vec2{ 1.0, 1.0 },
			Vec2 uvOffset = Vec2{ 0.0, 0.0 },
			CloseRing closeRing = CloseRing::No);

		/// @brief 3D 閉路に沿う一定半径のチューブを作成します。
		/// @param path チューブの中心を通る閉路の頂点。始点を末尾に重複させず、3 点以上を指定します。
		/// @param radius チューブの半径
		/// @param closeRing 経路を閉じる場合は `CloseRing::Yes`
		/// @param sides チューブ断面の分割数。3 以上である必要があります。
		/// @param uvScale UV 座標の拡大率
		/// @param uvOffset UV 座標のオフセット
		/// @return 経路に沿うチューブの 3D メッシュ。引数が不正な場合、または頂点数が上限を超える場合は空の 3D メッシュ
		/// @remark `CloseRing::Yes` の場合は端面を生成しません。経路方向の UV 継ぎ目に断面を複製し、parallel-transport frame の残留回転を経路長に比例して分散します。
		[[nodiscard]]
		static Mesh3D Tube(
			std::span<const Vec3> path,
			double radius,
			CloseRing closeRing,
			uint32 sides = 12,
			Vec2 uvScale = Vec2{ 1.0, 1.0 },
			Vec2 uvOffset = Vec2{ 0.0, 0.0 });

		/// @brief 3D 経路に沿う一定半径のチューブを作成します。
		/// @param path チューブの中心を通る経路の頂点。開路は 2 点以上、閉路は 3 点以上である必要があります。
		/// @param radius チューブの半径
		/// @param sides チューブ断面の分割数。3 以上である必要があります。
		/// @param uvScale UV 座標の拡大率
		/// @param uvOffset UV 座標のオフセット
		/// @param closeRing 経路を閉じる場合は `CloseRing::Yes`、開いた経路として扱う場合は `CloseRing::No`
		/// @return 経路に沿うチューブの 3D メッシュ。引数が不正な場合、または頂点数が上限を超える場合は空の 3D メッシュ
		/// @remark 経路、端面、および UV 座標の規約は `std::span` を受け取るオーバーロードと同じです。
		[[nodiscard]]
		static Mesh3D Tube(
			std::initializer_list<Vec3> path,
			double radius,
			uint32 sides = 12,
			Vec2 uvScale = Vec2{ 1.0, 1.0 },
			Vec2 uvOffset = Vec2{ 0.0, 0.0 },
			CloseRing closeRing = CloseRing::No);

		/// @brief 初期化子リストで指定した 3D 閉路に沿う一定半径のチューブを作成します。
		/// @param path チューブの中心を通る閉路の頂点。始点を末尾に重複させず、3 点以上を指定します。
		/// @param radius チューブの半径
		/// @param closeRing 経路を閉じる場合は `CloseRing::Yes`
		/// @param sides チューブ断面の分割数
		/// @param uvScale UV 座標の拡大率
		/// @param uvOffset UV 座標のオフセット
		/// @return 経路に沿うチューブの 3D メッシュ。引数が不正な場合、または頂点数が上限を超える場合は空の 3D メッシュ
		/// @remark 閉路、UV 座標、およびフレームの規約は `std::span` を受け取る閉路オーバーロードと同じです。
		[[nodiscard]]
		static Mesh3D Tube(
			std::initializer_list<Vec3> path,
			double radius,
			CloseRing closeRing,
			uint32 sides = 12,
			Vec2 uvScale = Vec2{ 1.0, 1.0 },
			Vec2 uvOffset = Vec2{ 0.0, 0.0 });

		/// @brief 経路点ごとに半径を指定したチューブを作成します。
		/// @param path チューブの中心を通る経路の頂点。開路は 2 点以上、閉路は 3 点以上である必要があります。
		/// @param radii 各経路点における半径。要素数は `path.size()` と等しく、各要素は float 変換後も正である必要があります。
		/// @param sides チューブ断面の分割数。3 以上である必要があります。
		/// @param uvScale UV 座標の拡大率
		/// @param uvOffset UV 座標のオフセット
		/// @param closeRing 経路を閉じる場合は `CloseRing::Yes`、開いた経路として扱う場合は `CloseRing::No`
		/// @return 経路に沿うチューブの 3D メッシュ。引数が不正な場合、または頂点数が上限を超える場合は空の 3D メッシュ
		/// @remark 隣接する経路点の間では半径を線形に変化させます。側面法線には経路方向の半径変化を反映します。
		/// @remark 経路、端面、および UV 座標の規約は一定半径の `Tube()` と同じです。開路の各端面には対応する端点の半径を使用します。
		/// @remark `CloseRing::Yes` の場合、最後の経路点の半径から最初の経路点の半径へ接続し、端面を生成しません。
		[[nodiscard]]
		static Mesh3D Tube(
			std::span<const Vec3> path,
			std::span<const double> radii,
			uint32 sides = 12,
			Vec2 uvScale = Vec2{ 1.0, 1.0 },
			Vec2 uvOffset = Vec2{ 0.0, 0.0 },
			CloseRing closeRing = CloseRing::No);

		/// @brief 初期化子リストで経路点ごとの半径を指定したチューブを作成します。
		/// @param path チューブの中心を通る経路の頂点
		/// @param radii 各経路点における半径
		/// @param sides チューブ断面の分割数
		/// @param uvScale UV 座標の拡大率
		/// @param uvOffset UV 座標のオフセット
		/// @param closeRing 経路を閉じる場合は `CloseRing::Yes`
		/// @return 経路に沿うチューブの 3D メッシュ。引数が不正な場合、または頂点数が上限を超える場合は空の 3D メッシュ
		/// @remark 半径、経路、端面、および UV 座標の規約は `std::span` を受け取る経路点別半径オーバーロードと同じです。
		[[nodiscard]]
		static Mesh3D Tube(
			std::initializer_list<Vec3> path,
			std::initializer_list<double> radii,
			uint32 sides = 12,
			Vec2 uvScale = Vec2{ 1.0, 1.0 },
			Vec2 uvOffset = Vec2{ 0.0, 0.0 },
			CloseRing closeRing = CloseRing::No);

		////////////////////////////////////////////////////////////////
		//
		//	Sweep
		//
		////////////////////////////////////////////////////////////////

		/// @brief 2D 断面を 3D 経路に沿わせた 3D メッシュを作成します。
		/// @param crossSection 経路に沿わせる断面。穴を含むことができます。
		/// @param path 断面の中心を通る経路の頂点。開路は 2 点以上、閉路は 3 点以上である必要があります。
		/// @param uvScale UV 座標の拡大率
		/// @param uvOffset UV 座標のオフセット
		/// @param closeRing 経路を閉じる場合は `CloseRing::Yes`、開いた経路として扱う場合は `CloseRing::No`
		/// @return 断面を経路に沿わせた 3D メッシュ。引数が不正な場合、または頂点数が上限を超える場合は空の 3D メッシュ
		/// @remark parallel-transport frame を使って断面を経路に沿って運びます。開始時の断面の向きは、最初の経路方向と最も平行でない座標軸から自動的に決定します。
		/// @remark 断面の X 座標はフレームの第 1 軸 N、Y 座標は第 2 軸 `N.cross(T)` に対応します。T は経路の接線です。断面点 `(x, y)` は経路上の位置 P に対して `P + N * x + (N.cross(T)) * y` に配置されます。
		/// @remark `CloseRing::No` の場合、始端と終端は閉じられ、端面と側面、および断面の各頂点はハードエッジになります。`CloseRing::Yes` の場合は端面を作らず、末尾から先頭へ接続します。
		/// @remark 側面の U 座標は外周と各穴の周長をそれぞれ `[0, 1]` とし、V 座標は経路の始端からの累積距離です。その後に `uvScale` と `uvOffset` が適用されます。
		/// @remark 端面の UV 座標は断面全体のバウンディングボックスを `[0, 1]` 四方に正規化し、同じ `uvScale` と `uvOffset` を適用します。
		/// @remark 折れ点では断面が前後の経路方向の二等分面に置かれます。このため、断面のオフセット面は隣接する線分に対して厳密に平行にならず、その影響は隣の断面までの側面全体に及びます。
		/// @remark 始点と終点が float 変換後に一致する経路は無効です。閉路では始点を末尾に重複させず、3 点以上を指定します。float 変換後に連続する 2 点が同じになる経路、および 180° 折り返す経路には対応しません。鋭い折れ点における自己交差は検査しません。
		/// @remark 閉路では経路方向の UV 継ぎ目に断面を複製し、parallel-transport frame の残留回転を経路長に比例して分散します。
		[[nodiscard]]
		static Mesh3D Sweep(
			const Polygon& crossSection,
			std::span<const Vec3> path,
			Vec2 uvScale = Vec2{ 1.0, 1.0 },
			Vec2 uvOffset = Vec2{ 0.0, 0.0 },
			CloseRing closeRing = CloseRing::No);

		/// @brief 2D 断面を 3D 閉路に沿わせた 3D メッシュを作成します。
		/// @param crossSection 経路に沿わせる断面。穴を含むことができます。
		/// @param path 断面の中心を通る閉路の頂点。始点を末尾に重複させず、3 点以上を指定します。
		/// @param closeRing 経路を閉じる場合は `CloseRing::Yes`
		/// @param uvScale UV 座標の拡大率
		/// @param uvOffset UV 座標のオフセット
		/// @return 断面を経路に沿わせた 3D メッシュ。引数が不正な場合、または頂点数が上限を超える場合は空の 3D メッシュ
		/// @remark `CloseRing::Yes` の場合は端面を生成しません。経路方向の UV 継ぎ目に断面を複製し、parallel-transport frame の残留回転を経路長に比例して分散します。
		[[nodiscard]]
		static Mesh3D Sweep(
			const Polygon& crossSection,
			std::span<const Vec3> path,
			CloseRing closeRing,
			Vec2 uvScale = Vec2{ 1.0, 1.0 },
			Vec2 uvOffset = Vec2{ 0.0, 0.0 });

		/// @brief 2D 断面を 3D 経路に沿わせた 3D メッシュを作成します。
		/// @param crossSection 経路に沿わせる断面。穴を含むことができます。
		/// @param path 断面の中心を通る経路の頂点。開路は 2 点以上、閉路は 3 点以上である必要があります。
		/// @param uvScale UV 座標の拡大率
		/// @param uvOffset UV 座標のオフセット
		/// @param closeRing 経路を閉じる場合は `CloseRing::Yes`、開いた経路として扱う場合は `CloseRing::No`
		/// @return 断面を経路に沿わせた 3D メッシュ。引数が不正な場合、または頂点数が上限を超える場合は空の 3D メッシュ
		/// @remark 断面、経路、および UV 座標の規約は `std::span` を受け取るオーバーロードと同じです。
		[[nodiscard]]
		static Mesh3D Sweep(
			const Polygon& crossSection,
			std::initializer_list<Vec3> path,
			Vec2 uvScale = Vec2{ 1.0, 1.0 },
			Vec2 uvOffset = Vec2{ 0.0, 0.0 },
			CloseRing closeRing = CloseRing::No);

		/// @brief 初期化子リストで指定した 3D 閉路に 2D 断面を沿わせた 3D メッシュを作成します。
		/// @param crossSection 経路に沿わせる断面。穴を含むことができます。
		/// @param path 断面の中心を通る閉路の頂点。始点を末尾に重複させず、3 点以上を指定します。
		/// @param closeRing 経路を閉じる場合は `CloseRing::Yes`
		/// @param uvScale UV 座標の拡大率
		/// @param uvOffset UV 座標のオフセット
		/// @return 断面を経路に沿わせた 3D メッシュ。引数が不正な場合、または頂点数が上限を超える場合は空の 3D メッシュ
		[[nodiscard]]
		static Mesh3D Sweep(
			const Polygon& crossSection,
			std::initializer_list<Vec3> path,
			CloseRing closeRing,
			Vec2 uvScale = Vec2{ 1.0, 1.0 },
			Vec2 uvOffset = Vec2{ 0.0, 0.0 });

		/// @brief 開始時の断面の X 軸方向を指定し、2D 断面を 3D 経路に沿わせた 3D メッシュを作成します。
		/// @param crossSection 経路に沿わせる断面。穴を含むことができます。
		/// @param path 断面の中心を通る経路の頂点。開路は 2 点以上、閉路は 3 点以上である必要があります。
		/// @param initialXAxis 開始時に断面の X 軸を向ける方向。最初の経路方向に垂直な平面へ投影して使用します。
		/// @param uvScale UV 座標の拡大率
		/// @param uvOffset UV 座標のオフセット
		/// @param closeRing 経路を閉じる場合は `CloseRing::Yes`、開いた経路として扱う場合は `CloseRing::No`
		/// @return 断面を経路に沿わせた 3D メッシュ。引数が不正な場合、または頂点数が上限を超える場合は空の 3D メッシュ
		/// @remark `initialXAxis` がゼロベクトル、非有限値、または最初の経路方向と平行な場合は空の 3D メッシュを返します。
		/// @remark 断面、経路、および UV 座標の規約は自動方向版の `Sweep()` と同じです。
		/// @code
		/// Mesh3D mesh = Mesh3D::Sweep(crossSection, path, Arg::initialXAxis = Vec3::UnitX());
		/// @endcode
		[[nodiscard]]
		static Mesh3D Sweep(
			const Polygon& crossSection,
			std::span<const Vec3> path,
			Arg::initialXAxis_<Vec3> initialXAxis,
			Vec2 uvScale = Vec2{ 1.0, 1.0 },
			Vec2 uvOffset = Vec2{ 0.0, 0.0 },
			CloseRing closeRing = CloseRing::No);

		/// @brief 開始時の断面の X 軸方向を指定し、2D 断面を 3D 閉路に沿わせた 3D メッシュを作成します。
		/// @param crossSection 経路に沿わせる断面。穴を含むことができます。
		/// @param path 断面の中心を通る閉路の頂点。始点を末尾に重複させず、3 点以上を指定します。
		/// @param initialXAxis 開始時に断面の X 軸を向ける方向
		/// @param closeRing 経路を閉じる場合は `CloseRing::Yes`
		/// @param uvScale UV 座標の拡大率
		/// @param uvOffset UV 座標のオフセット
		/// @return 断面を経路に沿わせた 3D メッシュ。引数が不正な場合、または頂点数が上限を超える場合は空の 3D メッシュ
		/// @remark `initialXAxis` は UV 継ぎ目における断面の向きを固定します。残留回転の補正はこの向きを保ったまま経路全体へ分散されます。
		[[nodiscard]]
		static Mesh3D Sweep(
			const Polygon& crossSection,
			std::span<const Vec3> path,
			Arg::initialXAxis_<Vec3> initialXAxis,
			CloseRing closeRing,
			Vec2 uvScale = Vec2{ 1.0, 1.0 },
			Vec2 uvOffset = Vec2{ 0.0, 0.0 });

		/// @brief 開始時の断面の X 軸方向を指定し、2D 断面を 3D 経路に沿わせた 3D メッシュを作成します。
		/// @param crossSection 経路に沿わせる断面。穴を含むことができます。
		/// @param path 断面の中心を通る経路の頂点。開路は 2 点以上、閉路は 3 点以上である必要があります。
		/// @param initialXAxis 開始時に断面の X 軸を向ける方向。最初の経路方向に垂直な平面へ投影して使用します。
		/// @param uvScale UV 座標の拡大率
		/// @param uvOffset UV 座標のオフセット
		/// @param closeRing 経路を閉じる場合は `CloseRing::Yes`、開いた経路として扱う場合は `CloseRing::No`
		/// @return 断面を経路に沿わせた 3D メッシュ。引数が不正な場合、または頂点数が上限を超える場合は空の 3D メッシュ
		/// @remark 断面、経路、および UV 座標の規約は `std::span` を受け取るオーバーロードと同じです。
		[[nodiscard]]
		static Mesh3D Sweep(
			const Polygon& crossSection,
			std::initializer_list<Vec3> path,
			Arg::initialXAxis_<Vec3> initialXAxis,
			Vec2 uvScale = Vec2{ 1.0, 1.0 },
			Vec2 uvOffset = Vec2{ 0.0, 0.0 },
			CloseRing closeRing = CloseRing::No);

		/// @brief 初期 X 軸方向を指定し、初期化子リストの 3D 閉路に 2D 断面を沿わせた 3D メッシュを作成します。
		/// @param crossSection 経路に沿わせる断面。穴を含むことができます。
		/// @param path 断面の中心を通る閉路の頂点。始点を末尾に重複させず、3 点以上を指定します。
		/// @param initialXAxis 開始時に断面の X 軸を向ける方向
		/// @param closeRing 経路を閉じる場合は `CloseRing::Yes`
		/// @param uvScale UV 座標の拡大率
		/// @param uvOffset UV 座標のオフセット
		/// @return 断面を経路に沿わせた 3D メッシュ。引数が不正な場合、または頂点数が上限を超える場合は空の 3D メッシュ
		[[nodiscard]]
		static Mesh3D Sweep(
			const Polygon& crossSection,
			std::initializer_list<Vec3> path,
			Arg::initialXAxis_<Vec3> initialXAxis,
			CloseRing closeRing,
			Vec2 uvScale = Vec2{ 1.0, 1.0 },
			Vec2 uvOffset = Vec2{ 0.0, 0.0 });

		/// @brief 経路点ごとに断面の拡大率と twist を指定した Sweep 形状を作成します。
		/// @param crossSection 経路に沿わせる断面。穴を含むことができます。
		/// @param path 断面の中心を通る経路の頂点。開路は 2 点以上、閉路は 3 点以上である必要があります。
		/// @param sectionTransforms 各経路点における断面変換。要素数は `path.size()` と等しい必要があります。
		/// @param options 初期断面方向、UV 変換、および経路の閉鎖方法
		/// @return 断面を経路に沿わせた 3D メッシュ。引数が不正な場合、または頂点数が上限を超える場合は空の 3D メッシュ
		/// @remark `sectionTransforms` の各 `scale` 成分は float 変換後も正である必要があります。断面の拡大後に `twist` を適用します。
		/// @remark 正の `twist` は経路の接線 T を軸として、断面の +X 軸 N を `-N.cross(T)` 方向へ回転させます。経路が world +Y、断面の +X が world +X の場合、world +X から world -Z へ回転します。
		/// @remark 各経路点で変換した断面を隣接する断面と三角形で接続します。側面の法線と接線は、断面変換による傾きとねじれを反映します。
		/// @remark 隣接する断面間で側面の三角形が縮退または反転する変換は無効です。大きな twist は経路点を追加して分割してください。
		/// @remark 端面の UV 座標と側面の U 座標は変換前の断面を基準にします。V 座標は経路の始端からの累積距離です。
		/// @remark `CloseRing::Yes` の場合、最後の経路点の断面変換から最初の経路点の断面変換へ接続し、UV 継ぎ目では最初の断面を複製します。
		/// @remark 経路、断面の頂点順序、端面、および自己交差に関する規約は一定断面の `Sweep()` と同じです。
		[[nodiscard]]
		static Mesh3D Sweep(
			const Polygon& crossSection,
			std::span<const Vec3> path,
			std::span<const SweepSectionTransform> sectionTransforms,
			const SweepOptions& options = {});

		/// @brief 初期化子リストで経路点ごとの断面変換を指定した Sweep 形状を作成します。
		/// @param crossSection 経路に沿わせる断面。穴を含むことができます。
		/// @param path 断面の中心を通る経路の頂点
		/// @param sectionTransforms 各経路点における断面変換
		/// @param options 初期断面方向、UV 変換、および経路の閉鎖方法
		/// @return 断面を経路に沿わせた 3D メッシュ。引数が不正な場合、または頂点数が上限を超える場合は空の 3D メッシュ
		/// @remark 断面変換、経路、端面、および UV 座標の規約は `std::span` を受け取る経路点別変換オーバーロードと同じです。
		[[nodiscard]]
		static Mesh3D Sweep(
			const Polygon& crossSection,
			std::initializer_list<Vec3> path,
			std::initializer_list<SweepSectionTransform> sectionTransforms,
			const SweepOptions& options = {});

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

		/// @brief 実行時に指定した複数の断面を高さ方向に接続した 3D メッシュを作成します。
		/// @param sections 各断面の頂点範囲。断面数は 2 以上で、各断面は同じ 3 個以上の頂点を持つ必要があります。
		/// @param heights 各断面の Y 座標。断面数と同じ要素数で、厳密な昇順である必要があります。
		/// @param uvScale UV 座標の拡大率
		/// @param uvOffset UV 座標のオフセット
		/// @return 断面を接続した 3D メッシュ。引数が不正な場合、または頂点数が上限を超える場合は空の 3D メッシュ
		/// @remark 各断面の `Vec2` を `(X, -Z)` に対応させます。各断面は先頭頂点を末尾に重複させず、符号付き面積の 2 倍 `Σ(x[i] * y[i+1] - x[i+1] * y[i])` が正になる順序で指定します。例えば `{ { 0, 0 }, { 1, 0 }, { 1, 1 }, { 0, 1 } }` は有効です。
		/// @remark 各断面では、末尾から先頭へ戻る辺を含め、float 変換後に連続する 2 頂点が同じになってはいけません。
		/// @remark `sections[i][j]` と `sections[i + 1][j]` を対応する頂点として接続します。始端と終端は閉じます。
		/// @remark 側面の U 座標は最初の断面の周長に沿って `[0, 1]`、V 座標は `heights[0]` からの実距離です。
		/// @remark 輪郭の各頂点はハードエッジとし、断面間では法線と接線を滑らかに接続します。
		/// @remark 中間断面の自己交差、および異なる断面間での側面の自己交差は検査しません。
		/// @remark `uvScale.y` を単位高さあたりの反復数として使用すると、Repeat sampler で高さ方向に一定密度のタイリングができます。
		[[nodiscard]]
		static Mesh3D Loft(
			std::span<const std::span<const Vec2>> sections,
			std::span<const double> heights,
			Vec2 uvScale = Vec2{ 1.0, 1.0 },
			Vec2 uvOffset = Vec2{ 0.0, 0.0 });

		/// @brief 実行時に指定した複数の断面を高さ方向に接続した 3D メッシュを作成します。
		/// @param sections 各断面の頂点配列。断面数は 2 以上で、各断面は同じ 3 個以上の頂点を持つ必要があります。
		/// @param heights 各断面の Y 座標。断面数と同じ要素数で、厳密な昇順である必要があります。
		/// @param uvScale UV 座標の拡大率
		/// @param uvOffset UV 座標のオフセット
		/// @return 断面を接続した 3D メッシュ。引数が不正な場合、または頂点数が上限を超える場合は空の 3D メッシュ
		/// @remark 座標、接続、端面、および UV 座標の規約は、断面を `std::span<const std::span<const Vec2>>` で受け取るオーバーロードと同じです。
		/// @remark 呼び出し時に各断面を参照する一時配列を内部で作成します。繰り返し生成する場合は、断面を `std::span<const std::span<const Vec2>>` で受け取るオーバーロードを使用すると、この一時配列を呼び出し側で再利用できます。
		[[nodiscard]]
		static Mesh3D Loft(
			const Array<Array<Vec2>>& sections,
			std::span<const double> heights,
			Vec2 uvScale = Vec2{ 1.0, 1.0 },
			Vec2 uvOffset = Vec2{ 0.0, 0.0 });

		/// @brief 複数の断面を高さ方向に接続した 3D メッシュを作成します。
		/// @tparam SectionCount 断面数。2 以上である必要があります。
		/// @tparam VertexCount 各断面の頂点数。3 以上である必要があります。
		/// @param sections 各断面の頂点配列
		/// @param heights 各断面の Y 座標。厳密な昇順である必要があります。
		/// @param uvScale UV 座標の拡大率
		/// @param uvOffset UV 座標のオフセット
		/// @return 断面を接続した 3D メッシュ。引数が不正な場合、または頂点数が上限を超える場合は空の 3D メッシュ
		/// @remark 各断面の `Vec2` を `(X, -Z)` に対応させます。各断面は先頭頂点を末尾に重複させず、符号付き面積の 2 倍 `Σ(x[i] * y[i+1] - x[i+1] * y[i])` が正になる順序で指定します。例えば `{ { 0, 0 }, { 1, 0 }, { 1, 1 }, { 0, 1 } }` は有効です。
		/// @remark 各断面では、末尾から先頭へ戻る辺を含め、float 変換後に連続する 2 頂点が同じになってはいけません。
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
		//	Sphere
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
		static Mesh3D Sphere(double radius, uint32 slices = 32, uint32 stacks = 16);

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
		//	reserve
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した要素数まで再確保なしで格納できるよう、頂点配列と三角形インデックス配列の容量を確保します。
		/// @param vertexCapacity 確保する頂点容量
		/// @param triangleCapacity 確保する三角形容量
		/// @remark `vertexCapacity` が `MaxVertexCount` を超える場合、容量は変更されません。
		void reserve(size_t vertexCapacity, size_t triangleCapacity);

		////////////////////////////////////////////////////////////////
		//
		//	clear
		//
		////////////////////////////////////////////////////////////////

		/// @brief すべての頂点と三角形を削除し、空の 3D メッシュにします。
		/// @remark 頂点配列と三角形インデックス配列の容量は変更されません。
		void clear() noexcept;

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
		/// @remark 保存に失敗した場合、理由を Fail レベルのエンジンログへ出力します。
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
		/// @remark 保存に失敗した場合、理由を Fail レベルのエンジンログへ出力します。
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
		/// @remark 書き出しに失敗した場合、理由を Fail レベルのエンジンログへ出力します。
		bool encodeOBJ(IWriter& writer) const;

		/// @brief 3D メッシュを Wavefront OBJ 形式でエンコードします。
		/// @return エンコードされたデータ。エンコードに失敗した場合は空の Blob
		/// @remark Siv3D の左手座標系から OBJ の右手座標系へ変換するため、頂点座標と法線の Z 成分は符号を反転し、三角形の巻き順は反転してエンコードされます。
		/// @remark Siv3D の上端を V = 0 とする UV 座標から OBJ の下端を V = 0 とする UV 座標へ変換するため、V 成分は `1 - V` としてエンコードされます。U 成分は変更されません。
		/// @remark 各 `Vertex3D` には、位置、UV、法線で共通の 1 始まりの OBJ インデックスが割り当てられます。
		/// @remark 接線、材質、オブジェクト名、およびグループはエンコードされません。
		/// @remark UTF-8（BOM なし）、LF 改行でエンコードされます。
		/// @remark 空のメッシュ、インデックスが不正なメッシュ、または出力対象の頂点属性に非有限値が含まれるメッシュはエンコードできません。
		/// @remark エンコードに失敗した場合、理由を Fail レベルのエンジンログへ出力します。
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
		//	transformUV
		//
		////////////////////////////////////////////////////////////////

		/// @brief すべての頂点の UV 座標を変換します。
		/// @param transform UV 座標に適用する変換行列
		/// @return *this
		/// @remark 各頂点の現在の UV 座標に `transform` が適用されます。その他の頂点属性と三角形インデックスは変更されません。
		Mesh3D& transformUV(const Mat3x2& transform) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	computeNormals
		//
		////////////////////////////////////////////////////////////////

		/// @brief 頂点法線を計算します。
		/// @param weighting 各三角形の法線を合成するときの重み付け方式
		/// @remark 同じ頂点インデックスを共有する三角形の法線が合成されます。
		/// @remark ハードエッジを保持するには、その境界で頂点が分割されている必要があります。
		/// @remark 三角形がない場合、頂点法線は変更されません。
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
		/// @remark 三角形がない場合、頂点の接線は変更されません。
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

	};
}

# include "detail/Mesh3D.ipp"
