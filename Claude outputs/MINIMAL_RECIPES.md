# Mesh3D 最小生成コード

各モデルの造形・色・部品構成を保ち、生成と OBJ + MTL 保存に絞ったコードです。
独立した幾何検査・期待値の照合・レポート出力は含めません。

- 各コードブロックは単独で完結した `Main.cpp` です。そのまま貼って実行すると、
  作業ディレクトリ直下の `models/` に OBJ + MTL を書き出します。
- 生成・登録の `Result` は `.value()` で取り出します。失敗時は例外で停止するため、
  失敗した追加を無視して欠けたモデルを保存し続けることはありません。
  `(void)` は成功時の戻り値が不要な箇所に付けています。保存失敗も `Error` で報告します。
  エラーから回復したい場合は `Result` の成否と `error()` を明示的に処理してください。
- 元の `Main.cpp` から抽出したコードを、現行 API に合わせて整理しています。
  下表は既定パラメータの値です。パラメータを変更したモデルの形状や接続の正しさは、
  生成・保存の成功だけでは保証されません。

| モデル | 部品 | 共有形状 | 材質 | 頂点 | 三角形 |
| --- | --- | --- | --- | --- | --- |
| `spiral_staircase` | 117 + ピボット 23 | 21 | 6 | 21162 | 24960 |
| `doric_temple` | 632 + ピボット 64 | 37 | 9 | 148354 | 131868 |
| `gorge_arch_bridge` | 201 + ピボット 5 | 44 | 10 | 51961 | 62410 |
| `tank_locomotive` | 214 + ピボット 0 | 65 | 9 | 24925 | 21744 |
| `sailing_ship` | 103 + ピボット 0 | 92 | 9 | 7712 | 8560 |
| `armillary_sphere` | 65 + ピボット 2 | 24 | 8 | 11043 | 12208 |

生成の使い分けは次の通りです。

- 単一形状は `Mesh3D` の生成関数。輪郭や経路を組み立てる局所的な処理にはラムダも使います。
- 複数形状の合成や生成時の配置には `Mesh3DBuilder`。
  `BuildMesh()` は、このサンプル内で Builder を用意し、`obtainMesh()` で取り出すだけの補助関数です。
- 形状・材質・親子関係・配置は `Mesh3DAssembly` に登録します。
  恒等配置と `metallic = 0.0` は既定値なので省略しています。
- 円周上の配置は `Cylindrical{ radius, angle, y }.toVec3()`。
  輪郭の向きは `Geometry2D::IsClockwise()` で揃えます。これは自己交差の修復や妥当性検査ではありません。

登録した形状は複数の部品から共有できます。神殿では柱身 1 つを 54 本の柱で共有します。
OBJ 書き出し時には各部品の配置を適用した頂点へ展開されます。

検証: macOS で整理前後の全 6 レシピをコンパイル・実行し、OBJ・MTL 計 12 ファイルの
バイト一致を確認しました。`models/` がない状態からの保存も確認しています。

---

## 1. 螺旋階段

扇形の踏板・支柱・連続する螺旋手すり・子柱・上下の踊り場と出入口。

使っている主な API: `Extrude`（扇形の輪郭）、`Tube`（螺旋手すり）、`Mesh3DBuilder`（子柱）、切り欠き輪郭による戸口の開口。

```cpp
# include <Siv3D.hpp> // Siv3D v0.8.0

using MaterialID = Mesh3DAssembly::MaterialID;

/// @brief Mesh3DBuilder で組んだ複数の形状を 1 つの Mesh3D にまとめます。
template <class Compose>
[[nodiscard]]
Mesh3D BuildMesh(Compose compose)
{
	Mesh3DBuilder builder;
	compose(builder);
	return builder.obtainMesh();
}

/// @brief Closed contour of an annular sector, wound so that the signed area is positive.
[[nodiscard]]
Array<Vec2> AnnularSectorContour(double innerRadius, double outerRadius, double startAngle, double endAngle, int32 segments)
{
	Array<Vec2> points;
	points.reserve((segments + 1) * 2);

	for (int32 i = 0; i <= segments; ++i)
	{
		const double t = (startAngle + ((endAngle - startAngle) * (i / static_cast<double>(segments))));
		points << Vec2{ (outerRadius * std::cos(t)), (outerRadius * std::sin(t)) };
	}

	for (int32 i = 0; i <= segments; ++i)
	{
		const double t = (endAngle + ((startAngle - endAngle) * (i / static_cast<double>(segments))));
		points << Vec2{ (innerRadius * std::cos(t)), (innerRadius * std::sin(t)) };
	}

	return points;
}

/// @brief Reverses a contour when needed so that Polygon and Loft accept it.
[[nodiscard]]
Array<Vec2> PositivelyWound(Array<Vec2> points)
{
	// Siv3D screen coordinates: clockwise means positive signed area.
	// This only fixes winding; it does not repair a self-intersecting contour.
	if (not Geometry2D::IsClockwise(points))
	{
		points.reverse();
	}

	return points;
}

/// @brief Shared shape for one baluster: shaft, two collars and a central knop.
void BuildBaluster(Mesh3DBuilder& builder, double length, double radius, double knopRadius)
{
	(void)builder.addCylinder(radius, length, 14, Vec3{ 0, (length * 0.5), 0 }).value();
	(void)builder.addCylinder((radius * 1.9), 0.045, 14, Vec3{ 0, 0.045, 0 }).value();
	(void)builder.addCylinder((radius * 1.9), 0.045, 14, Vec3{ 0, (length - 0.045), 0 }).value();
	(void)builder.addSphere(knopRadius, 16, 10, Vec3{ 0, (length * 0.42), 0 }).value();
}

//==========================================================================
//
//	Model 1: spiral staircase
//
//	Sector treads around a newel column, a continuous helical handrail with
//	one baluster per tread, lower and upper landings, doorways at both ends,
//	and two outer posts carrying the upper landing.
//
//==========================================================================

struct SpiralStairSpec
{
	int32 stepCount = 21;
	double stepAngle = 22.5_deg;
	double riser = 0.20;
	double treadThickness = 0.07;
	double treadInnerRadius = 0.18;
	double treadOuterRadius = 1.60;
	int32 treadArcSegments = 10;
	double columnRadius = 0.22;
	double columnBottomY = -0.54;
	double columnTopY = 4.86;
	double railRadius = 1.48;
	double railTubeRadius = 0.045;
	double railHeight = 0.95;
	int32 railSamplesPerStep = 4;
	double bracketOuterRadius = 1.05;
	double bracketDrop = 0.48;
	double bracketThickness = 0.05;
	double balusterRadius = 0.024;
	double balusterKnopRadius = 0.052;
	double landingOuterRadius = 2.06;
	double landingThickness = 0.20;
	double lowerLandingSweep = 70_deg;
	double upperLandingSweep = 60_deg;
	double basePlateRadius = 2.60;
	double basePlateStepRadius = 2.40;
	double basePlateLowerHeight = 0.16;
	double basePlateUpperHeight = 0.18;
	double postRadius = 0.06;
	double postInset = 8_deg;
	double postCircleRadius = 1.75;
	double wallInnerRadius = 0.30;
	double wallOuterRadius = 2.04;
	double wallThickness = 0.30;
	double wallHeight = 2.42;
	double wallCopingHeight = 0.14;
	double openingInnerRadius = 0.62;
	double openingOuterRadius = 1.86;
	double portalOpeningHeight = 2.10;
	double portalJambRadialSize = 0.16;
	double portalJambTangentialSize = 0.42;   // wider than wallThickness so no face is coplanar
	double portalJambReveal = 0.03;           // pilasters overlap the notch edge by this much
	double portalLintelHeight = 0.24;
	double finialRadius = 0.20;
	// ---- values derived from the parameters above ----

	[[nodiscard]] double topY() const noexcept { return (stepCount * riser); }
	[[nodiscard]] double stairSweep() const noexcept { return (stepCount * stepAngle); }
	[[nodiscard]] double lowerLandingStart() const noexcept { return (-lowerLandingSweep); }
	[[nodiscard]] double upperLandingEnd() const noexcept { return (stairSweep() + upperLandingSweep); }
	[[nodiscard]] double landingTopY() const noexcept { return topY(); }
	[[nodiscard]] double basePlateTopY() const noexcept { return (-landingThickness); }
	[[nodiscard]] double basePlateBottomY() const noexcept { return (basePlateTopY() - basePlateUpperHeight - basePlateLowerHeight); }
	[[nodiscard]] double treadBalusterLength() const noexcept { return (railHeight - (0.5 * riser)); }
	[[nodiscard]] double openingWidth() const noexcept { return (openingOuterRadius - openingInnerRadius); }
	/// @brief Handrail centreline height at a given sweep angle.
	[[nodiscard]] double railY(double angle) const noexcept
	{
		const double clamped = Clamp(angle, 0.0, stairSweep());
		return (((riser * clamped) / stepAngle) + railHeight);
	}
};

void Main()
{
	const SpiralStairSpec spec;

	Mesh3DAssembly assembly;

	const MaterialID oak = assembly.addMaterial(Material{ .name = U"oak_tread", .baseColor = ColorF{ 0.58, 0.39, 0.21 },
		.roughness = 0.75 });
	const MaterialID steel = assembly.addMaterial(Material{ .name = U"dark_steel", .baseColor = ColorF{ 0.23, 0.25, 0.30 },
		.roughness = 0.45 });
	const MaterialID brass = assembly.addMaterial(Material{ .name = U"brass", .baseColor = ColorF{ 0.76, 0.57, 0.22 },
		.roughness = 0.35 });
	const MaterialID stone = assembly.addMaterial(Material{ .name = U"pale_stone", .baseColor = ColorF{ 0.72, 0.70, 0.63 },
		.roughness = 0.85 });
	const MaterialID darkStone = assembly.addMaterial(Material{ .name = U"dark_stone", .baseColor = ColorF{ 0.41, 0.40, 0.38 },
		.roughness = 0.90 });
	const MaterialID paint = assembly.addMaterial(Material{ .name = U"blue_grey_paint", .baseColor = ColorF{ 0.29, 0.39, 0.46 },
		.roughness = 0.60 });

	// ---- shared shapes ----

	// Tread: top surface at local y = 0, first sector of the helix.
	const auto treadShape = assembly.addMesh(Mesh3D::Extrude(Polygon{ AnnularSectorContour(spec.treadInnerRadius, spec.treadOuterRadius,
			0.0, spec.stepAngle, spec.treadArcSegments) }, spec.treadThickness)
		.translated(Float3{ 0.0f, static_cast<float>(-spec.treadThickness * 0.5), 0.0f })).value();

	// Brass nosing along the outer edge of the tread.
	const auto nosingShape = assembly.addMesh([&]()
		{
			Array<Vec3> path;

			for (int32 i = 0; i <= spec.treadArcSegments; ++i)
			{
				const double t = ((spec.stepAngle * i) / spec.treadArcSegments);
				path << Cylindrical{ spec.treadOuterRadius, t, (-spec.treadThickness * 0.5) }.toVec3();
			}

			return Mesh3D::Tube(path, (spec.treadThickness * 0.5), TubeOptions{ .sides = 10 });
		}()).value();

	// Gusset under each tread. Contour (x, y) becomes world (radius, height) via RotateX(90 deg),
	// and RotateY puts it on the bisector of its own tread.
	const auto bracketShape = assembly.addMesh(BuildMesh([&](Mesh3DBuilder& builder)
		{
			const Array<Vec2> contour = PositivelyWound({
				Vec2{ spec.treadInnerRadius, -spec.treadThickness },
				Vec2{ spec.bracketOuterRadius, -spec.treadThickness },
				Vec2{ spec.treadInnerRadius, (-spec.treadThickness - spec.bracketDrop) } });

			(void)builder.addExtrude(Polygon{ contour }, spec.bracketThickness,
				Mesh3DPlacement{ (Quaternion::RotateX(90_deg) * Quaternion::RotateY(static_cast<float>(spec.stepAngle * 0.5))) }).value();
		})).value();

	const auto treadBalusterShape = assembly.addMesh(BuildMesh([&](Mesh3DBuilder& builder)
		{
			BuildBaluster(builder, spec.treadBalusterLength(), spec.balusterRadius, spec.balusterKnopRadius);
		})).value();

	const auto landingBalusterShape = assembly.addMesh(BuildMesh([&](Mesh3DBuilder& builder)
		{
			BuildBaluster(builder, spec.railHeight, spec.balusterRadius, spec.balusterKnopRadius);
		})).value();

	// Wall panel with the doorway cut out of its lower edge. Contour (x, y) is placed in
	// world (X, Y) by RotateX(90 deg), so x is the radius and y the height above the landing.
	const auto wallShape = assembly.addMesh(BuildMesh([&](Mesh3DBuilder& builder)
		{
			const Array<Vec2> contour{
				Vec2{ spec.wallInnerRadius, 0.0 },
				Vec2{ spec.openingInnerRadius, 0.0 },
				Vec2{ spec.openingInnerRadius, spec.portalOpeningHeight },
				Vec2{ spec.openingOuterRadius, spec.portalOpeningHeight },
				Vec2{ spec.openingOuterRadius, 0.0 },
				Vec2{ spec.wallOuterRadius, 0.0 },
				Vec2{ spec.wallOuterRadius, spec.wallHeight },
				Vec2{ spec.wallInnerRadius, spec.wallHeight } };

			(void)builder.addExtrude(Polygon{ contour }, spec.wallThickness,
				Quaternion::RotateX(90_deg)).value();
		})).value();

	const double jambCentreInner = (spec.openingInnerRadius - (spec.portalJambRadialSize * 0.5) + spec.portalJambReveal);
	const double jambCentreOuter = (spec.openingOuterRadius + (spec.portalJambRadialSize * 0.5) - spec.portalJambReveal);
	const double lintelRun = ((jambCentreOuter - jambCentreInner) + spec.portalJambRadialSize + 0.16);

	const auto jambShape = assembly.addMesh(Mesh3D::Box(Vec3{ spec.portalJambRadialSize, (spec.portalOpeningHeight + 0.06), spec.portalJambTangentialSize })).value();
	const auto lintelShape = assembly.addMesh(Mesh3D::Box(Vec3{ lintelRun, spec.portalLintelHeight, (spec.portalJambTangentialSize + 0.04) })).value();
	const auto copingShape = assembly.addMesh(Mesh3D::Box(Vec3{ ((spec.wallOuterRadius - spec.wallInnerRadius) + 0.16), spec.wallCopingHeight,
			(spec.wallThickness + 0.16) })).value();
	const auto thresholdShape = assembly.addMesh(Mesh3D::Box(Vec3{ (spec.openingWidth() + 0.16), 0.06, (spec.wallThickness + 0.20) })).value();

	// ---- base, column and landings ----

	(void)assembly.addPart({ .name = U"base_plate_lower", .mesh = assembly.addMesh(Mesh3D::Cylinder(spec.basePlateRadius, spec.basePlateLowerHeight, 72)).value(), .material = darkStone, .placement = Vec3{ 0, (spec.basePlateBottomY() + (spec.basePlateLowerHeight * 0.5)), 0 } }).value();

	(void)assembly.addPart({ .name = U"base_plate_upper", .mesh = assembly.addMesh(Mesh3D::Cylinder(spec.basePlateStepRadius, spec.basePlateUpperHeight, 72)).value(), .material = stone, .placement = Vec3{ 0, (spec.basePlateTopY() - (spec.basePlateUpperHeight * 0.5)), 0 } }).value();

	(void)assembly.addPart({ .name = U"newel_column", .mesh = assembly.addMesh(Mesh3D::Cylinder(spec.columnRadius, (spec.columnTopY - spec.columnBottomY), 48)).value(), .material = steel, .placement = Vec3{ 0, ((spec.columnTopY + spec.columnBottomY) * 0.5), 0 } }).value();

	(void)assembly.addPart({ .name = U"newel_base_flare", .mesh = assembly.addMesh(Mesh3D::ConicalFrustum(0.40, 0.26, 0.26, 48)).value(), .material = steel, .placement = Vec3{ 0, (spec.basePlateTopY() + 0.13), 0 } }).value();

	(void)assembly.addPart({ .name = U"newel_cap", .mesh = assembly.addMesh(Mesh3D::ConicalFrustum(0.34, 0.24, 0.18, 48)).value(), .material = brass, .placement = Vec3{ 0, (spec.columnTopY + 0.09), 0 } }).value();

	(void)assembly.addPart({ .name = U"newel_finial", .mesh = assembly.addMesh(Mesh3D::Sphere(spec.finialRadius, 24, 14)).value(), .material = brass, .placement = Vec3{ 0, (spec.columnTopY + 0.18 + spec.finialRadius), 0 } }).value();

	(void)assembly.addPart({ .name = U"landing_lower", .mesh = assembly.addMesh(Mesh3D::Extrude(Polygon{ AnnularSectorContour(spec.treadInnerRadius, spec.landingOuterRadius,
			spec.lowerLandingStart(), 0.0, 40) }, spec.landingThickness)).value(), .material = stone, .placement = Vec3{ 0, (-spec.landingThickness * 0.5), 0 } }).value();

	(void)assembly.addPart({ .name = U"landing_upper", .mesh = assembly.addMesh(Mesh3D::Extrude(Polygon{ AnnularSectorContour(spec.treadInnerRadius, spec.landingOuterRadius,
			spec.stairSweep(), spec.upperLandingEnd(), 40) }, spec.landingThickness)).value(), .material = stone, .placement = Vec3{ 0, (spec.landingTopY() - (spec.landingThickness * 0.5)), 0 } }).value();

	// ---- steps: one pivot part per step carries tread, nosing and baluster ----

	for (int32 i = 1; i <= spec.stepCount; ++i)
	{
		const String tag = U"step_{:02d}"_fmt(i);
		const auto pivot = assembly.addPart({ .name = tag, .placement = Mesh3DPlacement{ Vec3{ 0, (i * spec.riser), 0 }, Quaternion::RotateY(static_cast<float>((i - 1) * spec.stepAngle)) } }).value();

		(void)assembly.addPart({ .name = tag + U"_tread", .mesh = treadShape, .material = oak, .parent = pivot }).value();
		(void)assembly.addPart({ .name = tag + U"_nosing", .mesh = nosingShape, .material = brass, .parent = pivot }).value();
		(void)assembly.addPart({ .name = tag + U"_bracket", .mesh = bracketShape, .material = steel, .parent = pivot }).value();
		(void)assembly.addPart({ .name = tag + U"_baluster", .mesh = treadBalusterShape, .material = steel, .parent = pivot, .placement = Cylindrical{ spec.railRadius, (spec.stepAngle * 0.5), 0.0 }.toVec3() }).value();
	}

	// ---- landing balusters ----

	for (int32 k = 1; k <= 4; ++k)
	{
		const double lowerAngle = (spec.lowerLandingStart() * (1.0 - (k / 5.0)));
		(void)assembly.addPart({ .name = U"landing_lower_baluster_{}"_fmt(k), .mesh = landingBalusterShape, .material = steel, .placement = Cylindrical{ spec.railRadius, lowerAngle, 0.0 }.toVec3() }).value();

		const double upperAngle = (spec.stairSweep() + ((spec.upperLandingSweep * k) / 5.0));
		(void)assembly.addPart({ .name = U"landing_upper_baluster_{}"_fmt(k), .mesh = landingBalusterShape, .material = steel, .placement = Cylindrical{ spec.railRadius, upperAngle, spec.landingTopY() }.toVec3() }).value();
	}

	// ---- continuous handrail ----

	{
		Array<Vec3> path;

		for (int32 i = 0; i < 6; ++i)
		{
			const double t = (spec.lowerLandingStart() * (1.0 - (i / 6.0)));
			path << Cylindrical{ spec.railRadius, t, spec.railY(t) }.toVec3();
		}

		const int32 stairSamples = (spec.stepCount * spec.railSamplesPerStep);

		for (int32 i = 0; i <= stairSamples; ++i)
		{
			const double t = ((spec.stairSweep() * i) / stairSamples);
			path << Cylindrical{ spec.railRadius, t, spec.railY(t) }.toVec3();
		}

		for (int32 i = 1; i <= 6; ++i)
		{
			const double t = (spec.stairSweep() + ((spec.upperLandingSweep * i) / 6.0));
			path << Cylindrical{ spec.railRadius, t, spec.railY(t) }.toVec3();
		}

		(void)assembly.addPart({ .name = U"handrail", .mesh = assembly.addMesh(Mesh3D::Tube(path, spec.railTubeRadius, TubeOptions{ .sides = 12 })).value(), .material = brass }).value();
	}

	// ---- outer posts carrying the upper landing ----

	{
		const double postTopY = (spec.landingTopY() - spec.landingThickness);
		const double postBottomY = spec.basePlateTopY();
		const auto postShape = assembly.addMesh(Mesh3D::Cylinder(spec.postRadius, (postTopY - postBottomY), 16)).value();
		const auto collarShape = assembly.addMesh(Mesh3D::ConicalFrustum(0.13, 0.08, 0.12, 16)).value();

		const double angles[2] = { (spec.stairSweep() + spec.postInset), (spec.upperLandingEnd() - spec.postInset) };

		for (int32 k = 0; k < 2; ++k)
		{
			(void)assembly.addPart({ .name = U"support_post_{}"_fmt(k), .mesh = postShape, .material = steel, .placement = Cylindrical{ spec.postCircleRadius, angles[k], ((postTopY + postBottomY) * 0.5) }.toVec3() }).value();
			(void)assembly.addPart({ .name = U"support_post_collar_{}"_fmt(k), .mesh = collarShape, .material = steel, .placement = Cylindrical{ spec.postCircleRadius, angles[k], (postBottomY + 0.06) }.toVec3() }).value();
		}
	}

	// ---- doorways at both ends ----

	{
		const double wallMidRadius = ((spec.wallInnerRadius + spec.wallOuterRadius) * 0.5);
		const double openingMidRadius = ((spec.openingInnerRadius + spec.openingOuterRadius) * 0.5);

		struct PortalPlacement { String tag; double angle; double baseY; };
		const PortalPlacement portals[2] = {
			{ U"portal_lower", spec.lowerLandingStart(), 0.0 },
			{ U"portal_upper", spec.upperLandingEnd(), spec.landingTopY() } };

		for (const auto& portal : portals)
		{
			const auto pivot = assembly.addPart({ .name = portal.tag, .placement = Mesh3DPlacement{ Vec3{ 0, portal.baseY, 0 }, Quaternion::RotateY(static_cast<float>(portal.angle)) } }).value();

			(void)assembly.addPart({ .name = portal.tag + U"_wall", .mesh = wallShape, .material = stone, .parent = pivot }).value();
			(void)assembly.addPart({ .name = portal.tag + U"_jamb_inner", .mesh = jambShape, .material = paint, .parent = pivot, .placement = Vec3{ jambCentreInner, ((spec.portalOpeningHeight + 0.06) * 0.5), 0 } }).value();
			(void)assembly.addPart({ .name = portal.tag + U"_jamb_outer", .mesh = jambShape, .material = paint, .parent = pivot, .placement = Vec3{ jambCentreOuter, ((spec.portalOpeningHeight + 0.06) * 0.5), 0 } }).value();
			(void)assembly.addPart({ .name = portal.tag + U"_lintel", .mesh = lintelShape, .material = paint, .parent = pivot, .placement = Vec3{ openingMidRadius, (spec.portalOpeningHeight + 0.06 + (spec.portalLintelHeight * 0.5)), 0 } }).value();
			(void)assembly.addPart({ .name = portal.tag + U"_coping", .mesh = copingShape, .material = paint, .parent = pivot, .placement = Vec3{ wallMidRadius, (spec.wallHeight + (spec.wallCopingHeight * 0.5)), 0 } }).value();
			(void)assembly.addPart({ .name = portal.tag + U"_threshold", .mesh = thresholdShape, .material = darkStone, .parent = pivot, .placement = Vec3{ openingMidRadius, 0.03, 0 } }).value();
		}
	}

	if (not assembly.saveOBJ(U"models/spiral_staircase.obj"))
	{
		throw Error{ U"Failed to export spiral_staircase" };
	}
}
```

---

## 2. ドーリア式周柱式神殿

段状基壇・8×17 のフルート付き列柱・柱頭と柱脚・梁・破風・軒の装飾・内部のケッラ。

使っている主な API: `Loft`（エンタシス付きのフルート柱身を 1 つの共有形状として 54 本で再利用）、`Revolve`（エキヌス）、`TriangularPrism`（屋根）。

```cpp
# include <Siv3D.hpp> // Siv3D v0.8.0

using MaterialID = Mesh3DAssembly::MaterialID;

/// @brief Mesh3DBuilder で組んだ複数の形状を 1 つの Mesh3D にまとめます。
template <class Compose>
[[nodiscard]]
Mesh3D BuildMesh(Compose compose)
{
	Mesh3DBuilder builder;
	compose(builder);
	return builder.obtainMesh();
}

/// @brief Reverses a contour when needed so that Polygon and Loft accept it.
[[nodiscard]]
Array<Vec2> PositivelyWound(Array<Vec2> points)
{
	// Siv3D screen coordinates: clockwise means positive signed area.
	// This only fixes winding; it does not repair a self-intersecting contour.
	if (not Geometry2D::IsClockwise(points))
	{
		points.reverse();
	}

	return points;
}

/// @brief Closed contour of a fluted column of unit radius, positively wound.
[[nodiscard]]
Array<Vec2> FlutedContour(int32 flutes, int32 samplesPerFlute, double depth)
{
	Array<Vec2> points;
	points.reserve(static_cast<size_t>(flutes) * samplesPerFlute);

	for (int32 f = 0; f < flutes; ++f)
	{
		for (int32 k = 0; k < samplesPerFlute; ++k)
		{
			const double u = (k / static_cast<double>(samplesPerFlute));
			const double radius = (1.0 - (depth * std::sin(u * Math::Pi)));
			const double angle = (((f + u) / flutes) * Math::TwoPi);
			points << Vec2{ (radius * std::cos(angle)), (radius * std::sin(angle)) };
		}
	}

	return points;
}

/// @brief Radius of a lofted shaft at the given section parameter.
[[nodiscard]]
double ShaftRadius(double bottomRadius, double topRadius, double entasis, double t) noexcept
{
	return (bottomRadius + ((topRadius - bottomRadius) * t) + (entasis * std::sin(t * Math::Pi)));
}

/// @brief Tapered, fluted shaft built by lofting the same contour at several radii.
[[nodiscard]]
Mesh3D FlutedShaft(std::span<const Vec2> contour, double bottomY, double height,
	double bottomRadius, double topRadius, double entasis, int32 sections)
{
	Array<LoftSection> loftSections;
	loftSections.reserve(sections + 1);

	for (int32 i = 0; i <= sections; ++i)
	{
		const double t = (i / static_cast<double>(sections));
		const double radius = ShaftRadius(bottomRadius, topRadius, entasis, t);
		loftSections << LoftSection{ .points = contour,
			.frame = Mesh3DPlacement{ (Mat4x4::Scale(static_cast<float>(radius))
				* Mat4x4::Translate(Float3{ 0.0f, static_cast<float>(bottomY + (height * t)), 0.0f })) } };
	}

	// 25 degrees keeps the flute hollows smooth and the arrises between them sharp.
	return Mesh3D::Loft(loftSections, LoftOptions{ .endCaps = Mesh3DEndCaps::Both, .smoothingAngle = 25_deg });
}

/// @brief Adds the four beams of a rectangular ring without overlapping corners.
void AddRingBeams(Mesh3DAssembly& assembly, const String& tag, MaterialID material,
	double halfX, double halfZ, double depth, double bottomY, double height)
{
	const auto endShape = assembly.addMesh(Mesh3D::Box(Vec3{ (halfX * 2.0), height, depth })).value();
	const auto sideShape = assembly.addMesh(Mesh3D::Box(Vec3{ depth, height, ((halfZ - depth) * 2.0) })).value();
	const double centreY = (bottomY + (height * 0.5));

	(void)assembly.addPart({ .name = tag + U"_front", .mesh = endShape, .material = material, .placement = Vec3{ 0, centreY, (halfZ - (depth * 0.5)) } }).value();
	(void)assembly.addPart({ .name = tag + U"_back", .mesh = endShape, .material = material, .placement = Vec3{ 0, centreY, (-halfZ + (depth * 0.5)) } }).value();
	(void)assembly.addPart({ .name = tag + U"_left", .mesh = sideShape, .material = material, .placement = Vec3{ (-halfX + (depth * 0.5)), centreY, 0 } }).value();
	(void)assembly.addPart({ .name = tag + U"_right", .mesh = sideShape, .material = material, .placement = Vec3{ (halfX - (depth * 0.5)), centreY, 0 } }).value();
}

//==========================================================================
//
//	Model 2: Doric peripteral temple
//
//	Stepped crepidoma, 8 x 17 fluted columns with feet and capitals, a two
//	band entablature with triglyphs and mutules, a gable roof, pediments at
//	both ends, and a walled cella with porches and an inner colonnade.
//
//==========================================================================

struct TempleSpec
{
	// Column order. Every other dimension is expressed through these.
	double columnRadius = 0.50;
	double columnTopRadius = 0.405;
	double columnEntasis = 0.050;
	double footHeight = 0.12;
	double shaftHeight = 5.30;
	double neckingHeight = 0.09;
	double echinusHeight = 0.32;
	double echinusRadius = 0.70;
	double abacusHeight = 0.22;
	double abacusSize = 1.18;
	int32 fluteCount = 20;
	int32 fluteSamples = 4;
	double fluteDepth = 0.055;
	int32 shaftSections = 7;
	// Colonnade grid.
	int32 frontColumns = 8;
	int32 sideColumns = 17;
	double columnSpacing = 2.20;
	double stylobateMargin = 0.85;
	// Crepidoma.
	int32 crepidomaSteps = 3;
	double crepidomaStepHeight = 0.30;
	double crepidomaStepInset = 0.42;
	// Entablature.
	double architraveHeight = 0.74;
	double friezeHeight = 0.70;
	double friezeInset = 0.03;
	double corniceHeight = 0.42;
	double corniceOverhang = 0.42;
	double corniceDepth = 1.90;
	double triglyphWidth = 0.36;
	double triglyphRelief = 0.08;
	double mutuleSize = 0.44;
	double mutuleDrop = 0.10;
	double taeniaHeight = 0.10;
	double taeniaRelief = 0.06;
	double regulaHeight = 0.12;
	double ridgeCapWidth = 0.34;
	double ridgeCapHeight = 0.20;
	// Roof and pediments.
	double roofRise = 2.60;
	double roofOverrun = 0.40;      // roof prism end relative to the cornice end
	double tympanumInset = 0.51;    // horizontal inset from the roof edge
	double tympanumThickness = 0.42;
	double rakeHeight = 0.30;
	double rakeDepth = 0.95;
	double simaHeight = 0.40;
	double simaThickness = 0.24;
	// Cella.
	double cellaHalfWidth = 4.70;
	double cellaHalfLength = 12.00;
	double cellaWallThickness = 0.62;
	double cellaDoorWidth = 3.00;
	double cellaDoorHeight = 4.40;
	double porchColumnZ = 13.20;
	double porchColumnX = 3.30;
	double innerColumnX = 2.60;
	double innerColumnSpacing = 3.60;
	int32 innerColumnRows = 5;
	double innerColumnRadius = 0.34;
	double innerColumnTopRadius = 0.28;
	double innerShaftHeight = 3.90;
	// ---- derived ----

	[[nodiscard]] double columnHeight() const noexcept
	{
		return (footHeight + shaftHeight + neckingHeight + echinusHeight + abacusHeight);
	}
	[[nodiscard]] double stylobateTopY() const noexcept { return (crepidomaSteps * crepidomaStepHeight); }
	[[nodiscard]] double columnTopY() const noexcept { return (stylobateTopY() + columnHeight()); }
	[[nodiscard]] double frontAxisX() const noexcept { return ((frontColumns - 1) * columnSpacing * 0.5); }
	[[nodiscard]] double sideAxisZ() const noexcept { return ((sideColumns - 1) * columnSpacing * 0.5); }
	[[nodiscard]] double stylobateHalfX() const noexcept { return (frontAxisX() + stylobateMargin); }
	[[nodiscard]] double stylobateHalfZ() const noexcept { return (sideAxisZ() + stylobateMargin); }
	[[nodiscard]] double architraveHalfX() const noexcept { return (frontAxisX() + (abacusSize * 0.5)); }
	[[nodiscard]] double architraveHalfZ() const noexcept { return (sideAxisZ() + (abacusSize * 0.5)); }
	[[nodiscard]] double architraveTopY() const noexcept { return (columnTopY() + architraveHeight); }
	[[nodiscard]] double friezeTopY() const noexcept { return (architraveTopY() + friezeHeight); }
	[[nodiscard]] double corniceTopY() const noexcept { return (friezeTopY() + corniceHeight); }
	[[nodiscard]] double corniceHalfX() const noexcept { return (architraveHalfX() + corniceOverhang); }
	[[nodiscard]] double corniceHalfZ() const noexcept { return (architraveHalfZ() + corniceOverhang); }
	[[nodiscard]] double roofSlope() const noexcept { return (roofRise / corniceHalfX()); }
	[[nodiscard]] double ridgeY() const noexcept { return (corniceTopY() + roofRise); }
	[[nodiscard]] double roofHalfLength() const noexcept { return (corniceHalfZ() - roofOverrun); }
	[[nodiscard]] double tympanumHalfWidth() const noexcept { return (corniceHalfX() - tympanumInset); }
	[[nodiscard]] double tympanumHeight() const noexcept { return (tympanumHalfWidth() * roofSlope()); }
};

void Main()
{
	const TempleSpec spec;

	Mesh3DAssembly assembly;

	const MaterialID marble = assembly.addMaterial(Material{ .name = U"marble_column", .baseColor = ColorF{ 0.87, 0.85, 0.79 },
		.roughness = 0.55 });
	const MaterialID marbleBase = assembly.addMaterial(Material{ .name = U"marble_crepidoma", .baseColor = ColorF{ 0.77, 0.75, 0.69 },
		.roughness = 0.80 });
	const MaterialID marbleBeam = assembly.addMaterial(Material{ .name = U"marble_entablature", .baseColor = ColorF{ 0.84, 0.82, 0.75 },
		.roughness = 0.60 });
	const MaterialID friezeStone = assembly.addMaterial(Material{ .name = U"frieze_ground", .baseColor = ColorF{ 0.70, 0.68, 0.62 },
		.roughness = 0.75 });
	const MaterialID triglyphStone = assembly.addMaterial(Material{ .name = U"triglyph", .baseColor = ColorF{ 0.55, 0.58, 0.61 },
		.roughness = 0.60 });
	const MaterialID roofTile = assembly.addMaterial(Material{ .name = U"roof_tile", .baseColor = ColorF{ 0.69, 0.60, 0.50 },
		.roughness = 0.85 });
	const MaterialID terracotta = assembly.addMaterial(Material{ .name = U"terracotta", .baseColor = ColorF{ 0.71, 0.40, 0.27 },
		.roughness = 0.70 });
	const MaterialID pedimentGround = assembly.addMaterial(Material{ .name = U"pediment_ground", .baseColor = ColorF{ 0.58, 0.56, 0.51 },
		.roughness = 0.80 });
	const MaterialID cellaWall = assembly.addMaterial(Material{ .name = U"cella_wall", .baseColor = ColorF{ 0.81, 0.78, 0.71 },
		.roughness = 0.75 });

	// ---- shared column shapes, all measured from the stylobate surface ----

	const Array<Vec2> fluteContour = FlutedContour(spec.fluteCount, spec.fluteSamples, spec.fluteDepth);
	const double shaftTopY = (spec.footHeight + spec.shaftHeight);
	const double neckingTopY = (shaftTopY + spec.neckingHeight);
	const double echinusTopY = (neckingTopY + spec.echinusHeight);

	const auto footShape = assembly.addMesh(Mesh3D::ConicalFrustum((spec.columnRadius + 0.065), (spec.columnRadius + 0.02), spec.footHeight, 40)).value();
	const auto shaftShape = assembly.addMesh(FlutedShaft(fluteContour, spec.footHeight, spec.shaftHeight,
			spec.columnRadius, spec.columnTopRadius, spec.columnEntasis, spec.shaftSections)).value();
	const auto neckingShape = assembly.addMesh(Mesh3D::ConicalFrustum(spec.columnTopRadius, (spec.columnTopRadius + 0.025), spec.neckingHeight, 40)).value();
	const auto echinusShape = assembly.addMesh([&]()
		{
			const double r0 = (spec.columnTopRadius + 0.025);
			const double r1 = spec.echinusRadius;
			Array<Vec2> profile;

			for (int32 i = 0; i <= 6; ++i)
			{
				const double t = (i / 6.0);
				profile << Vec2{ (r0 + ((r1 - r0) * std::pow(t, 0.55))), (neckingTopY - 0.02 + (spec.echinusHeight * t)) };
			}

			profile << Vec2{ r1, (echinusTopY + 0.03) };
			return Mesh3D::Revolve(profile, RevolveOptions{ .segments = 40, .smoothingAngle = 40_deg });
		}()).value();
	const auto abacusShape = assembly.addMesh(Mesh3D::Box(Vec3{ spec.abacusSize, spec.abacusHeight, spec.abacusSize })).value();

	const double abacusCentreY = (echinusTopY + (spec.abacusHeight * 0.5));

	/// Places one complete column with its own pivot part.
	const auto placeColumn = [&](const String& tag, double x, double z)
		{
			const auto pivot = assembly.addPart({ .name = tag, .placement = Vec3{ x, spec.stylobateTopY(), z } }).value();

			(void)assembly.addPart({ .name = tag + U"_foot", .mesh = footShape, .material = marble, .parent = pivot, .placement = Vec3{ 0, (spec.footHeight * 0.5), 0 } }).value();
			(void)assembly.addPart({ .name = tag + U"_shaft", .mesh = shaftShape, .material = marble, .parent = pivot }).value();
			(void)assembly.addPart({ .name = tag + U"_necking", .mesh = neckingShape, .material = marble, .parent = pivot, .placement = Vec3{ 0, (shaftTopY + (spec.neckingHeight * 0.5)), 0 } }).value();
			(void)assembly.addPart({ .name = tag + U"_echinus", .mesh = echinusShape, .material = marble, .parent = pivot }).value();
			(void)assembly.addPart({ .name = tag + U"_abacus", .mesh = abacusShape, .material = marble, .parent = pivot, .placement = Vec3{ 0, abacusCentreY, 0 } }).value();
		};

	// ---- crepidoma ----

	for (int32 i = 0; i < spec.crepidomaSteps; ++i)
	{
		const double inset = (spec.crepidomaStepInset * (spec.crepidomaSteps - 1 - i));
		const double halfX = (spec.stylobateHalfX() + inset);
		const double halfZ = (spec.stylobateHalfZ() + inset);
		(void)assembly.addPart({ .name = U"crepidoma_{}"_fmt(i), .mesh = assembly.addMesh(Mesh3D::Box(Vec3{ (halfX * 2.0), spec.crepidomaStepHeight, (halfZ * 2.0) })).value(), .material = marbleBase, .placement = Vec3{ 0, ((i + 0.5) * spec.crepidomaStepHeight), 0 } }).value();
	}

	// ---- peristyle: the outer ring of the 8 x 17 grid ----

	for (int32 ix = 0; ix < spec.frontColumns; ++ix)
	{
		for (int32 iz = 0; iz < spec.sideColumns; ++iz)
		{
			const bool onRing = ((ix == 0) || (ix == (spec.frontColumns - 1))
				|| (iz == 0) || (iz == (spec.sideColumns - 1)));

			if (not onRing)
			{
				continue;
			}

			placeColumn(U"peristyle_{:02d}_{:02d}"_fmt(ix, iz),
				(-spec.frontAxisX() + (ix * spec.columnSpacing)),
				(-spec.sideAxisZ() + (iz * spec.columnSpacing)));
		}
	}

	// ---- porch columns in front of and behind the cella ----

	for (int32 side = 0; side < 2; ++side)
	{
		const double z = ((side == 0) ? spec.porchColumnZ : -spec.porchColumnZ);

		for (int32 k = 0; k < 4; ++k)
		{
			const double x = (((k < 2) ? -1.0 : 1.0)
				* (((k % 2) == 0) ? spec.porchColumnX : (spec.porchColumnX - spec.columnSpacing)));
			placeColumn(U"porch_{}_{}"_fmt(side, k), x, z);
		}
	}

	// ---- entablature ----

	AddRingBeams(assembly, U"architrave", marbleBeam, spec.architraveHalfX(), spec.architraveHalfZ(),
		spec.abacusSize, spec.columnTopY(), spec.architraveHeight);
	AddRingBeams(assembly, U"frieze", friezeStone, (spec.architraveHalfX() - spec.friezeInset),
		(spec.architraveHalfZ() - spec.friezeInset), spec.abacusSize, spec.architraveTopY(), spec.friezeHeight);
	AddRingBeams(assembly, U"taenia", marbleBeam, (spec.architraveHalfX() + spec.taeniaRelief),
		(spec.architraveHalfZ() + spec.taeniaRelief), (spec.abacusSize + (spec.taeniaRelief * 2.0)),
		(spec.architraveTopY() - spec.taeniaHeight), spec.taeniaHeight);
	AddRingBeams(assembly, U"cornice", marbleBeam, spec.corniceHalfX(), spec.corniceHalfZ(),
		spec.corniceDepth, spec.friezeTopY(), spec.corniceHeight);

	// Triglyphs sit on the frieze face at every half bay; mutules hang under the cornice above them.
	{
		const double friezeHalfX = (spec.architraveHalfX() - spec.friezeInset);
		const double friezeHalfZ = (spec.architraveHalfZ() - spec.friezeInset);
		const double half = (spec.columnSpacing * 0.5);
		const int32 endCount = ((spec.frontColumns - 1) * 2);
		const int32 sideCount = ((spec.sideColumns - 1) * 2);

		const auto triglyphShape = assembly.addMesh(Mesh3D::Box(Vec3{ spec.triglyphWidth, (spec.friezeHeight - 0.08), spec.triglyphRelief })).value();
		const auto triglyphSideShape = assembly.addMesh(Mesh3D::Box(Vec3{ spec.triglyphRelief, (spec.friezeHeight - 0.08), spec.triglyphWidth })).value();
		const auto mutuleShape = assembly.addMesh(Mesh3D::Box(Vec3{ spec.mutuleSize, spec.mutuleDrop, spec.mutuleSize })).value();
		const auto regulaShape = assembly.addMesh(Mesh3D::Box(Vec3{ spec.triglyphWidth, spec.regulaHeight, spec.triglyphRelief })).value();
		const auto regulaSideShape = assembly.addMesh(Mesh3D::Box(Vec3{ spec.triglyphRelief, spec.regulaHeight, spec.triglyphWidth })).value();
		const double regulaY = (spec.architraveTopY() - spec.taeniaHeight - (spec.regulaHeight * 0.5));

		const double triglyphY = (spec.architraveTopY() + ((spec.friezeHeight - 0.08) * 0.5));
		const double mutuleY = (spec.friezeTopY() - (spec.mutuleDrop * 0.5) + 0.01);

		for (int32 k = -(endCount / 2); k <= (endCount / 2); ++k)
		{
			const double x = (k * half);

			for (int32 side = 0; side < 2; ++side)
			{
				const double sign = ((side == 0) ? 1.0 : -1.0);
				(void)assembly.addPart({ .name = U"triglyph_end_{}_{}"_fmt(side, (k + (endCount / 2))), .mesh = triglyphShape, .material = triglyphStone, .placement = Vec3{ x, triglyphY, (sign * (friezeHalfZ + (spec.triglyphRelief * 0.5))) } }).value();
				(void)assembly.addPart({ .name = U"mutule_end_{}_{}"_fmt(side, (k + (endCount / 2))), .mesh = mutuleShape, .material = marbleBeam, .placement = Vec3{ x, mutuleY, (sign * (friezeHalfZ + 0.30)) } }).value();
				(void)assembly.addPart({ .name = U"regula_end_{}_{}"_fmt(side, (k + (endCount / 2))), .mesh = regulaShape, .material = marbleBeam, .placement = Vec3{ x, regulaY, (sign * (spec.architraveHalfZ() + (spec.triglyphRelief * 0.5))) } }).value();
			}
		}

		for (int32 k = -(sideCount / 2); k <= (sideCount / 2); ++k)
		{
			const double z = (k * half);

			for (int32 side = 0; side < 2; ++side)
			{
				const double sign = ((side == 0) ? 1.0 : -1.0);
				(void)assembly.addPart({ .name = U"triglyph_side_{}_{}"_fmt(side, (k + (sideCount / 2))), .mesh = triglyphSideShape, .material = triglyphStone, .placement = Vec3{ (sign * (friezeHalfX + (spec.triglyphRelief * 0.5))), triglyphY, z } }).value();
				(void)assembly.addPart({ .name = U"mutule_side_{}_{}"_fmt(side, (k + (sideCount / 2))), .mesh = mutuleShape, .material = marbleBeam, .placement = Vec3{ (sign * (friezeHalfX + 0.30)), mutuleY, z } }).value();
				(void)assembly.addPart({ .name = U"regula_side_{}_{}"_fmt(side, (k + (sideCount / 2))), .mesh = regulaSideShape, .material = marbleBeam, .placement = Vec3{ (sign * (spec.architraveHalfX() + (spec.triglyphRelief * 0.5))), regulaY, z } }).value();
			}
		}
	}

	// ---- gable roof, ridge along Z ----

	(void)assembly.addPart({ .name = U"roof", .mesh = assembly.addMesh(Mesh3D::TriangularPrism(Vec3{ (spec.roofHalfLength() * 2.0), spec.roofRise, (spec.corniceHalfX() * 2.0) })).value(), .material = roofTile, .placement = Mesh3DPlacement{ Vec3{ 0, (spec.corniceTopY() + (spec.roofRise * 0.5)), 0 }, Quaternion::RotateY(90_deg) } }).value();

	(void)assembly.addPart({ .name = U"ridge_cap", .mesh = assembly.addMesh(Mesh3D::Box(Vec3{ spec.ridgeCapWidth, spec.ridgeCapHeight, (spec.roofHalfLength() * 2.0) })).value(), .material = terracotta, .placement = Vec3{ 0, (spec.ridgeY() - (spec.ridgeCapHeight * 0.25)), 0 } }).value();

	// Sima: an upright band along both long eaves.
	{
		const auto simaShape = assembly.addMesh(Mesh3D::Box(Vec3{ spec.simaThickness, spec.simaHeight, (spec.roofHalfLength() * 2.0) })).value();

		for (int32 side = 0; side < 2; ++side)
		{
			const double sign = ((side == 0) ? 1.0 : -1.0);
			(void)assembly.addPart({ .name = U"sima_{}"_fmt(side), .mesh = simaShape, .material = terracotta, .placement = Vec3{ (sign * (spec.corniceHalfX() - (spec.simaThickness * 0.5))),
					(spec.corniceTopY() + (spec.simaHeight * 0.5)), 0 } }).value();
		}
	}

	// ---- pediments ----

	{
		const double slopeAngle = std::atan(spec.roofSlope());
		const double rakeLength = (std::hypot(spec.corniceHalfX(), spec.roofRise) + (spec.rakeHeight * spec.roofSlope()));
		const double pedimentZ = (spec.roofHalfLength() + (spec.tympanumThickness * 0.5));
		const double rakeZ = (spec.roofHalfLength() + (spec.rakeDepth * 0.5) - 0.05);

		const auto tympanumShape = assembly.addMesh(BuildMesh([&](Mesh3DBuilder& builder)
			{
				const Array<Vec2> contour = PositivelyWound({
					Vec2{ -spec.tympanumHalfWidth(), 0.0 },
					Vec2{ spec.tympanumHalfWidth(), 0.0 },
					Vec2{ 0.0, spec.tympanumHeight() } });
				(void)builder.addExtrude(Polygon{ contour }, spec.tympanumThickness,
					Quaternion::RotateX(90_deg)).value();
			})).value();
		const auto rakeShape = assembly.addMesh(Mesh3D::Box(Vec3{ rakeLength, spec.rakeHeight, spec.rakeDepth })).value();
		const auto acroterionShape = assembly.addMesh([&]()
			{
				const Array<Vec2> profile{ Vec2{ 0.0, 0.0 }, Vec2{ 0.30, 0.0 }, Vec2{ 0.24, 0.10 },
					Vec2{ 0.30, 0.22 }, Vec2{ 0.22, 0.40 }, Vec2{ 0.30, 0.58 }, Vec2{ 0.12, 0.76 }, Vec2{ 0.0, 0.84 } };
				return Mesh3D::Revolve(profile, RevolveOptions{ .segments = 28, .smoothingAngle = 35_deg });
			}()).value();

		for (int32 side = 0; side < 2; ++side)
		{
			const double sign = ((side == 0) ? 1.0 : -1.0);

			(void)assembly.addPart({ .name = U"tympanum_{}"_fmt(side), .mesh = tympanumShape, .material = pedimentGround, .placement = Vec3{ 0, spec.corniceTopY(), (sign * pedimentZ) } }).value();

			for (int32 k = 0; k < 2; ++k)
			{
				const double dir = ((k == 0) ? -1.0 : 1.0);
				(void)assembly.addPart({ .name = U"rake_{}_{}"_fmt(side, k), .mesh = rakeShape, .material = marbleBeam, .placement = Mesh3DPlacement{
						Vec3{ (dir * spec.corniceHalfX() * 0.5),
							(spec.corniceTopY() + (spec.roofRise * 0.5) + ((spec.rakeHeight * 0.5) / std::cos(slopeAngle))),
							(sign * rakeZ) },
						Quaternion::RotateZ(static_cast<float>(-dir * slopeAngle)) } }).value();
			}

			(void)assembly.addPart({ .name = U"acroterion_apex_{}"_fmt(side), .mesh = acroterionShape, .material = terracotta, .placement = Vec3{ 0, (spec.ridgeY() + (spec.rakeHeight * 0.5)), (sign * rakeZ) } }).value();

			for (int32 k = 0; k < 2; ++k)
			{
				const double dir = ((k == 0) ? -1.0 : 1.0);
				(void)assembly.addPart({ .name = U"acroterion_corner_{}_{}"_fmt(side, k), .mesh = acroterionShape, .material = terracotta, .placement = Vec3{ (dir * (spec.corniceHalfX() - 0.20)), (spec.corniceTopY() + (spec.rakeHeight * 0.5)),
						(sign * rakeZ) } }).value();
			}
		}
	}

	// ---- cella ----

	{
		const double wallBottom = spec.stylobateTopY();
		const double wallHeight = spec.columnHeight();
		const double t = spec.cellaWallThickness;
		const double innerHalfX = (spec.cellaHalfWidth - t);
		const double endWidth = (innerHalfX * 2.0);

		const auto sideWallShape = assembly.addMesh(Mesh3D::Box(Vec3{ t, wallHeight, (spec.cellaHalfLength * 2.0) })).value();
		const auto backWallShape = assembly.addMesh(Mesh3D::Box(Vec3{ endWidth, wallHeight, t })).value();

		for (int32 side = 0; side < 2; ++side)
		{
			const double sign = ((side == 0) ? 1.0 : -1.0);
			(void)assembly.addPart({ .name = U"cella_wall_x{}"_fmt(side), .mesh = sideWallShape, .material = cellaWall, .placement = Vec3{ (sign * (spec.cellaHalfWidth - (t * 0.5))), (wallBottom + (wallHeight * 0.5)), 0 } }).value();
		}

		(void)assembly.addPart({ .name = U"cella_wall_back", .mesh = backWallShape, .material = cellaWall, .placement = Vec3{ 0, (wallBottom + (wallHeight * 0.5)), (-spec.cellaHalfLength + (t * 0.5)) } }).value();

		// Front wall with the doorway: two piers and a lintel.
		{
			const double pierWidth = ((endWidth - spec.cellaDoorWidth) * 0.5);
			const auto pierShape = assembly.addMesh(Mesh3D::Box(Vec3{ pierWidth, spec.cellaDoorHeight, t })).value();
			const auto lintelShape = assembly.addMesh(Mesh3D::Box(Vec3{ endWidth, (wallHeight - spec.cellaDoorHeight), t })).value();
			const double frontZ = (spec.cellaHalfLength - (t * 0.5));

			for (int32 side = 0; side < 2; ++side)
			{
				const double sign = ((side == 0) ? 1.0 : -1.0);
				(void)assembly.addPart({ .name = U"cella_pier_{}"_fmt(side), .mesh = pierShape, .material = cellaWall, .placement = Vec3{ (sign * ((spec.cellaDoorWidth + pierWidth) * 0.5)),
						(wallBottom + (spec.cellaDoorHeight * 0.5)), frontZ } }).value();
			}

			(void)assembly.addPart({ .name = U"cella_front_lintel", .mesh = lintelShape, .material = cellaWall, .placement = Vec3{ 0, (wallBottom + spec.cellaDoorHeight + ((wallHeight - spec.cellaDoorHeight) * 0.5)), frontZ } }).value();

			(void)assembly.addPart({ .name = U"cella_threshold", .mesh = assembly.addMesh(Mesh3D::Box(Vec3{ (spec.cellaDoorWidth + 0.6), 0.08, (t + 0.4) })).value(), .material = marbleBase, .placement = Vec3{ 0, (wallBottom + 0.04), frontZ } }).value();
		}

		// Inner colonnade and pedestal.
		{
			const Array<Vec2> innerContour = FlutedContour(16, spec.fluteSamples, spec.fluteDepth);
			const auto innerShaft = assembly.addMesh(FlutedShaft(innerContour, 0.0, spec.innerShaftHeight,
					spec.innerColumnRadius, spec.innerColumnTopRadius, 0.02, 5)).value();
			const auto innerEchinus = assembly.addMesh(Mesh3D::ConicalFrustum(spec.innerColumnTopRadius, (spec.innerColumnRadius + 0.10), 0.20, 28)).value();
			const auto innerAbacus = assembly.addMesh(Mesh3D::Box(Vec3{ 0.86, 0.16, 0.86 })).value();

			for (int32 side = 0; side < 2; ++side)
			{
				const double sign = ((side == 0) ? 1.0 : -1.0);

				for (int32 k = 0; k < spec.innerColumnRows; ++k)
				{
					const double z = ((k - ((spec.innerColumnRows - 1) * 0.5)) * spec.innerColumnSpacing);
					const String tag = U"inner_column_{}_{}"_fmt(side, k);
					const auto pivot = assembly.addPart({ .name = tag, .placement = Vec3{ (sign * spec.innerColumnX), wallBottom, z } }).value();

					(void)assembly.addPart({ .name = tag + U"_shaft", .mesh = innerShaft, .material = marble, .parent = pivot }).value();
					(void)assembly.addPart({ .name = tag + U"_echinus", .mesh = innerEchinus, .material = marble, .parent = pivot, .placement = Vec3{ 0, (spec.innerShaftHeight + 0.09), 0 } }).value();
					(void)assembly.addPart({ .name = tag + U"_abacus", .mesh = innerAbacus, .material = marble, .parent = pivot, .placement = Vec3{ 0, (spec.innerShaftHeight + 0.27), 0 } }).value();
				}
			}

			(void)assembly.addPart({ .name = U"cella_pedestal", .mesh = assembly.addMesh(Mesh3D::Box(Vec3{ 3.20, 0.70, 1.80 })).value(), .material = marbleBase, .placement = Vec3{ 0, (wallBottom + 0.35), -8.60 } }).value();
			(void)assembly.addPart({ .name = U"cella_pedestal_cap", .mesh = assembly.addMesh(Mesh3D::Box(Vec3{ 3.60, 0.14, 2.20 })).value(), .material = marbleBeam, .placement = Vec3{ 0, (wallBottom + 0.77), -8.60 } }).value();
		}
	}

	if (not assembly.saveOBJ(U"models/doric_temple.obj"))
	{
		throw Error{ U"Failed to export doric_temple" };
	}
}
```

---

## 3. 峡谷とアーチ橋

起伏のある両岸・川床と水面・厚みのあるアーチ・迫石・橋台・橋面・欄干・地形へ続く道。

使っている主な API: `HeightField`（地形）、手組みの `Mesh3D`（地形の側面と底）、`Extrude`（アーチ環とスパンドレル）、`Revolve`（欄干の親柱）。

```cpp
# include <Siv3D.hpp> // Siv3D v0.8.0

using MaterialID = Mesh3DAssembly::MaterialID;

/// @brief Mesh3DBuilder で組んだ複数の形状を 1 つの Mesh3D にまとめます。
template <class Compose>
[[nodiscard]]
Mesh3D BuildMesh(Compose compose)
{
	Mesh3DBuilder builder;
	compose(builder);
	return builder.obtainMesh();
}

/// @brief Reverses a contour when needed so that Polygon and Loft accept it.
[[nodiscard]]
Array<Vec2> PositivelyWound(Array<Vec2> points)
{
	// Siv3D screen coordinates: clockwise means positive signed area.
	// This only fixes winding; it does not repair a self-intersecting contour.
	if (not Geometry2D::IsClockwise(points))
	{
		points.reverse();
	}

	return points;
}

/// @brief Scattered ground features. Shared by the recipe and by the expectations.
struct GroundFeature
{
	double x;
	double z;
	double scale;
};

constexpr GroundFeature BridgeBoulders[8] = { { -10.4, 5.6, 0.95 }, { -9.6, -6.8, 0.72 },
	{ 10.8, 7.4, 0.85 }, { 10.2, -5.2, 1.05 }, { -6.4, 9.4, 0.62 }, { 7.2, -10.6, 0.70 },
	{ -12.6, -11.4, 0.80 }, { 12.9, 12.0, 0.66 } };

constexpr GroundFeature BridgeTrees[5] = { { -18.4, 8.6, 1.15 }, { -16.2, -9.8, 0.95 },
	{ 17.8, -8.2, 1.20 }, { 16.4, 10.4, 1.00 }, { -20.6, -2.4, 0.85 } };

inline constexpr double BridgeTreePlantDepth = 0.15;

[[nodiscard]]
double SmoothStep01(double edge0, double edge1, double value) noexcept
{
	const double t = Clamp(((value - edge0) / (edge1 - edge0)), 0.0, 1.0);
	return (t * t * (3.0 - (2.0 * t)));
}

//==========================================================================
//
//	Model 3: river gorge with a segmental arch bridge
//
//	A height field gorge with a flat road corridor, a water slab, a stone
//	arch barrel with voussoir faces, spandrel walls, abutments, deck,
//	balustrade, approach roads, boulders and trees.
//
//==========================================================================

struct BridgeSpec
{
	// Terrain.
	double terrainSizeX = 46.0;
	double terrainSizeZ = 34.0;
	int32 terrainGridX = 141;
	int32 terrainGridZ = 105;
	double plateauY = 3.05;
	double bedY = -1.60;
	double bedHalfWidth = 8.80;
	double gorgeEdgeX = 13.50;
	double terrainBaseY = -3.40;
	// Road corridor carved flat through the banks.
	double roadHalfWidth = 3.20;
	double corridorFade = 2.20;
	double corridorStartX = 13.00;
	double corridorEndX = 14.60;
	// Water.
	double waterY = -0.40;
	double waterHalfWidth = 12.00;
	double waterBottomY = -2.60;
	double waterEndInset = 0.05;   // keeps the slab ends off the coplanar skirt walls
	// Arch.
	double springingY = -0.90;
	double archHalfSpan = 9.60;
	double archRise = 2.70;
	double archThickness = 0.75;
	double barrelHalfWidth = 3.20;
	int32 archSegments = 96;
	int32 voussoirCount = 25;
	double voussoirRelief = 0.07;
	double voussoirJoint = 0.35_deg;
	double voussoirInset = 0.035;
	// Deck and abutments.
	double deckTopY = 3.30;
	double deckThickness = 0.40;
	double deckHalfLength = 15.00;
	double abutmentOuterX = 13.60;
	double abutmentBottomY = -1.95;
	double wingWallLength = 3.40;
	// Balustrade.
	double parapetPlinthHeight = 0.32;
	double balusterHeight = 0.56;
	double copingHeight = 0.18;
	double parapetThickness = 0.30;
	double postSize = 0.52;
	int32 postCount = 5;
	int32 balustersPerBay = 10;
	// Approaches.
	double roadTopY = 3.30;
	double roadThickness = 0.25;
	double kerbWidth = 0.22;
	double kerbHeight = 0.14;
	// ---- derived ----

	[[nodiscard]] double deckBottomY() const noexcept { return (deckTopY - deckThickness); }
	[[nodiscard]] double crownIntradosY() const noexcept { return (springingY + archRise); }
	[[nodiscard]] double archRadius() const noexcept
	{
		return (((archHalfSpan * archHalfSpan) + (archRise * archRise)) / (2.0 * archRise));
	}
	[[nodiscard]] double archCentreY() const noexcept { return (crownIntradosY() - archRadius()); }
	[[nodiscard]] double archHalfAngle() const noexcept { return std::asin(archHalfSpan / archRadius()); }
	[[nodiscard]] double extradosRadius() const noexcept { return (archRadius() + archThickness); }
	/// @brief Height of the extrados directly above x, valid for |x| <= archHalfSpan.
	[[nodiscard]] double extradosY(double x) const noexcept
	{
		const double r = extradosRadius();
		return (archCentreY() + std::sqrt(Max((r * r) - (x * x), 0.0)));
	}
	[[nodiscard]] double terrainHalfX() const noexcept { return (terrainSizeX * 0.5); }
	[[nodiscard]] double terrainHalfZ() const noexcept { return (terrainSizeZ * 0.5); }
};

/// @brief Terrain height. Shared by the height field, the skirt and the expectations.
[[nodiscard]]
double TerrainHeight(const BridgeSpec& spec, double x, double z) noexcept
{
	const double bankT = SmoothStep01(spec.bedHalfWidth, spec.gorgeEdgeX, Math::Abs(x));
	const double base = (spec.bedY + ((spec.plateauY - spec.bedY) * bankT));

	const double rolling = ((0.55 * std::sin((0.29 * x) + 0.7))
		+ (0.42 * std::cos((0.37 * z) - 0.4))
		+ (0.28 * std::sin((0.17 * x) + (0.61 * z)))
		+ (0.18 * std::cos((0.53 * z) + 1.9)));
	const double ripple = ((0.10 * std::sin((0.9 * z) + 0.3)) + (0.06 * std::cos(1.4 * x)));

	const double natural = (base + (rolling * bankT) + (ripple * (1.0 - bankT)));

	// Flatten a road corridor on the banks so the approach slabs rest on the ground.
	const double corridor = ((1.0 - SmoothStep01(spec.roadHalfWidth, (spec.roadHalfWidth + spec.corridorFade), Math::Abs(z)))
		* SmoothStep01(spec.corridorStartX, spec.corridorEndX, Math::Abs(x)));

	return (natural + ((base - natural) * corridor));
}

/// @brief Closed side walls and floor under a height field, with explicit outward normals.
/// @remark The top edge is sampled on the same grid as Mesh3D::HeightField, so the seams match exactly.
[[nodiscard]]
Mesh3D BuildTerrainSkirt(const BridgeSpec& spec, double baseY)
{
	Array<Vertex3D> vertices;
	Array<TriangleIndex32> indices;

	const auto pushVertex = [&](Vec3 position, Vec3 normal)
		{
			const Vec3 helper = ((Math::Abs(normal.y) > 0.5) ? Vec3{ 1, 0, 0 } : Vec3{ 0, 1, 0 });
			const Vec3 tangent = normal.cross(helper).normalized();
			vertices.push_back(Vertex3D{ .pos = Float3{ position }, .normal = Float3{ normal },
				.tex = Float2{ 0.0f, 0.0f }, .tangent = Float4{ Float3{ tangent }, 1.0f } });
		};

	// One wall per edge. The traversal direction t satisfies normal == cross(unitY, t),
	// which makes the quads (A, B, B'), (A, B', A') face outward.
	const auto addWall = [&](Vec3 normal, const Array<Vec3>& top)
		{
			for (size_t i = 0; (i + 1) < top.size(); ++i)
			{
				const uint32 base = static_cast<uint32>(vertices.size());
				pushVertex(top[i], normal);
				pushVertex(top[i + 1], normal);
				pushVertex(Vec3{ top[i + 1].x, baseY, top[i + 1].z }, normal);
				pushVertex(Vec3{ top[i].x, baseY, top[i].z }, normal);
				indices.push_back(TriangleIndex32{ base, (base + 1), (base + 2) });
				indices.push_back(TriangleIndex32{ base, (base + 2), (base + 3) });
			}
		};

	const double halfX = spec.terrainHalfX();
	const double halfZ = spec.terrainHalfZ();

	const auto columnX = [&](int32 c) { return (-halfX + ((spec.terrainSizeX * c) / (spec.terrainGridX - 1.0))); };
	const auto rowZ = [&](int32 r) { return (halfZ - ((spec.terrainSizeZ * r) / (spec.terrainGridZ - 1.0))); };

	{
		Array<Vec3> edge;                       // +X wall, traversed toward +Z
		for (int32 r = (spec.terrainGridZ - 1); r >= 0; --r)
		{
			edge << Vec3{ halfX, TerrainHeight(spec, halfX, rowZ(r)), rowZ(r) };
		}
		addWall(Vec3{ 1, 0, 0 }, edge);
	}
	{
		Array<Vec3> edge;                       // -X wall, traversed toward -Z
		for (int32 r = 0; r < spec.terrainGridZ; ++r)
		{
			edge << Vec3{ -halfX, TerrainHeight(spec, -halfX, rowZ(r)), rowZ(r) };
		}
		addWall(Vec3{ -1, 0, 0 }, edge);
	}
	{
		Array<Vec3> edge;                       // +Z wall, traversed toward -X
		for (int32 c = (spec.terrainGridX - 1); c >= 0; --c)
		{
			edge << Vec3{ columnX(c), TerrainHeight(spec, columnX(c), halfZ), halfZ };
		}
		addWall(Vec3{ 0, 0, 1 }, edge);
	}
	{
		Array<Vec3> edge;                       // -Z wall, traversed toward +X
		for (int32 c = 0; c < spec.terrainGridX; ++c)
		{
			edge << Vec3{ columnX(c), TerrainHeight(spec, columnX(c), -halfZ), -halfZ };
		}
		addWall(Vec3{ 0, 0, -1 }, edge);
	}

	{
		const uint32 base = static_cast<uint32>(vertices.size());
		const Vec3 down{ 0, -1, 0 };
		pushVertex(Vec3{ -halfX, baseY, -halfZ }, down);
		pushVertex(Vec3{ -halfX, baseY, halfZ }, down);
		pushVertex(Vec3{ halfX, baseY, halfZ }, down);
		pushVertex(Vec3{ halfX, baseY, -halfZ }, down);
		indices.push_back(TriangleIndex32{ base, (base + 2), (base + 1) });
		indices.push_back(TriangleIndex32{ base, (base + 3), (base + 2) });
	}

	return Mesh3D{ std::move(vertices), std::move(indices) };
}

/// @brief Closed contour of the arch ring between two sweep angles, measured from the crown.
[[nodiscard]]
Array<Vec2> ArchRingContour(const BridgeSpec& spec, double fromAngle, double toAngle, double radialInset, int32 segments)
{
	const double innerRadius = (spec.archRadius() + radialInset);
	const double outerRadius = (spec.extradosRadius() - radialInset);
	Array<Vec2> points;
	points.reserve((segments + 1) * 2);

	for (int32 i = 0; i <= segments; ++i)
	{
		const double t = (fromAngle + ((toAngle - fromAngle) * (i / static_cast<double>(segments))));
		points << Vec2{ (innerRadius * std::sin(t)), (spec.archCentreY() + (innerRadius * std::cos(t))) };
	}

	for (int32 i = segments; i >= 0; --i)
	{
		const double t = (fromAngle + ((toAngle - fromAngle) * (i / static_cast<double>(segments))));
		points << Vec2{ (outerRadius * std::sin(t)), (spec.archCentreY() + (outerRadius * std::cos(t))) };
	}

	return PositivelyWound(std::move(points));
}

void Main()
{
	const BridgeSpec spec;

	Mesh3DAssembly assembly;

	const MaterialID turf = assembly.addMaterial(Material{ .name = U"turf", .baseColor = ColorF{ 0.35, 0.44, 0.25 },
		.roughness = 0.95 });
	const MaterialID subsoil = assembly.addMaterial(Material{ .name = U"subsoil", .baseColor = ColorF{ 0.47, 0.40, 0.32 },
		.roughness = 0.95 });
	const MaterialID water = assembly.addMaterial(Material{ .name = U"water", .baseColor = ColorF{ 0.18, 0.40, 0.48 },
		.roughness = 0.15 });
	const MaterialID stone = assembly.addMaterial(Material{ .name = U"bridge_stone", .baseColor = ColorF{ 0.72, 0.69, 0.62 },
		.roughness = 0.80 });
	const MaterialID voussoirStone = assembly.addMaterial(Material{ .name = U"voussoir", .baseColor = ColorF{ 0.62, 0.59, 0.53 },
		.roughness = 0.80 });
	const MaterialID coping = assembly.addMaterial(Material{ .name = U"coping", .baseColor = ColorF{ 0.80, 0.77, 0.70 },
		.roughness = 0.70 });
	const MaterialID asphalt = assembly.addMaterial(Material{ .name = U"road_surface", .baseColor = ColorF{ 0.33, 0.32, 0.30 },
		.roughness = 0.95 });
	const MaterialID boulder = assembly.addMaterial(Material{ .name = U"boulder", .baseColor = ColorF{ 0.54, 0.53, 0.50 },
		.roughness = 0.90 });
	const MaterialID bark = assembly.addMaterial(Material{ .name = U"bark", .baseColor = ColorF{ 0.33, 0.25, 0.18 },
		.roughness = 0.95 });
	const MaterialID foliage = assembly.addMaterial(Material{ .name = U"foliage", .baseColor = ColorF{ 0.22, 0.40, 0.20 },
		.roughness = 0.90 });

	// ---- terrain ----

	(void)assembly.addPart({ .name = U"terrain", .mesh = assembly.addMesh(Mesh3D::HeightField(Size{ spec.terrainGridX, spec.terrainGridZ },
			SizeF{ spec.terrainSizeX, spec.terrainSizeZ },
			[&](Point p)
			{
				const double x = (-spec.terrainHalfX() + ((spec.terrainSizeX * p.x) / (spec.terrainGridX - 1.0)));
				const double z = (spec.terrainHalfZ() - ((spec.terrainSizeZ * p.y) / (spec.terrainGridZ - 1.0)));
				return TerrainHeight(spec, x, z);
			})).value(), .material = turf }).value();

	(void)assembly.addPart({ .name = U"terrain_skirt", .mesh = assembly.addMesh(BuildTerrainSkirt(spec, spec.terrainBaseY)).value(), .material = subsoil }).value();

	(void)assembly.addPart({ .name = U"water", .mesh = assembly.addMesh(Mesh3D::Box(Vec3{ (spec.waterHalfWidth * 2.0), (spec.waterY - spec.waterBottomY),
			(spec.terrainSizeZ - (spec.waterEndInset * 2.0)) })).value(), .material = water, .placement = Vec3{ 0, ((spec.waterY + spec.waterBottomY) * 0.5), 0 } }).value();

	// ---- arch barrel and voussoir faces ----

	const double alpha = spec.archHalfAngle();

	(void)assembly.addPart({ .name = U"arch_barrel", .mesh = assembly.addMesh(BuildMesh([&](Mesh3DBuilder& builder)
		{
			(void)builder.addExtrude(Polygon{ ArchRingContour(spec, -alpha, alpha, 0.0, spec.archSegments) },
				(spec.barrelHalfWidth * 2.0), Quaternion::RotateX(90_deg)).value();
		})).value(), .material = stone }).value();

	{
		const double step = ((alpha * 2.0) / spec.voussoirCount);

		for (int32 i = 0; i < spec.voussoirCount; ++i)
		{
			const double from = (-alpha + (step * i) + spec.voussoirJoint);
			const double to = (-alpha + (step * (i + 1)) - spec.voussoirJoint);
			const bool keystone = (i == (spec.voussoirCount / 2));
			const double relief = (keystone ? (spec.voussoirRelief + 0.03) : spec.voussoirRelief);
			const double inset = (keystone ? 0.0 : spec.voussoirInset);

			const auto faceShape = assembly.addMesh(BuildMesh([&](Mesh3DBuilder& builder)
				{
					(void)builder.addExtrude(Polygon{ ArchRingContour(spec, from, to, inset, 6) },
						relief, Quaternion::RotateX(90_deg)).value();
				})).value();

			for (int32 side = 0; side < 2; ++side)
			{
				const double sign = ((side == 0) ? 1.0 : -1.0);
				(void)assembly.addPart({ .name = U"{}_{}_{:02d}"_fmt((keystone ? U"keystone" : U"voussoir"), side, i), .mesh = faceShape, .material = voussoirStone, .placement = Vec3{ 0, 0, (sign * (spec.barrelHalfWidth + (relief * 0.5) - 0.01)) } }).value();
			}
		}
	}

	// ---- spandrel: solid fill from the extrados up to the deck soffit ----

	(void)assembly.addPart({ .name = U"spandrel", .mesh = assembly.addMesh(BuildMesh([&](Mesh3DBuilder& builder)
		{
			Array<Vec2> contour;
			const int32 samples = 96;

			for (int32 i = 0; i <= samples; ++i)
			{
				const double x = (-spec.archHalfSpan + ((spec.archHalfSpan * 2.0 * i) / samples));
				contour << Vec2{ x, spec.extradosY(x) };
			}

			contour << Vec2{ spec.archHalfSpan, spec.deckBottomY() };
			contour << Vec2{ -spec.archHalfSpan, spec.deckBottomY() };

			(void)builder.addExtrude(Polygon{ PositivelyWound(std::move(contour)) },
				(spec.barrelHalfWidth * 2.0), Quaternion::RotateX(90_deg)).value();
		})).value(), .material = stone }).value();

	// ---- abutments and wing walls ----

	{
		const double length = (spec.abutmentOuterX - spec.archHalfSpan);
		const double height = (spec.deckBottomY() - spec.abutmentBottomY);
		const auto abutmentShape = assembly.addMesh(Mesh3D::Box(Vec3{ length, height, (spec.barrelHalfWidth * 2.0) })).value();
		const auto wingShape = assembly.addMesh(Mesh3D::Box(Vec3{ spec.wingWallLength, (height - 1.30), 0.60 })).value();

		for (int32 side = 0; side < 2; ++side)
		{
			const double sign = ((side == 0) ? 1.0 : -1.0);
			(void)assembly.addPart({ .name = U"abutment_{}"_fmt(side), .mesh = abutmentShape, .material = stone, .placement = Vec3{ (sign * (spec.archHalfSpan + (length * 0.5))),
					((spec.deckBottomY() + spec.abutmentBottomY) * 0.5), 0 } }).value();

			for (int32 k = 0; k < 2; ++k)
			{
				const double zSign = ((k == 0) ? 1.0 : -1.0);
				(void)assembly.addPart({ .name = U"wing_wall_{}_{}"_fmt(side, k), .mesh = wingShape, .material = stone, .placement = Vec3{ (sign * (spec.abutmentOuterX + (spec.wingWallLength * 0.5) - 0.60)),
						((spec.deckBottomY() + spec.abutmentBottomY + 1.30) * 0.5),
						(zSign * (spec.barrelHalfWidth - 0.30)) } }).value();
			}
		}
	}

	// ---- deck, approach roads and kerbs ----

	(void)assembly.addPart({ .name = U"deck", .mesh = assembly.addMesh(Mesh3D::Box(Vec3{ (spec.deckHalfLength * 2.0), spec.deckThickness, (spec.barrelHalfWidth * 2.0) })).value(), .material = stone, .placement = Vec3{ 0, (spec.deckTopY - (spec.deckThickness * 0.5)), 0 } }).value();

	(void)assembly.addPart({ .name = U"road_surface_deck", .mesh = assembly.addMesh(Mesh3D::Box(Vec3{ (spec.deckHalfLength * 2.0), 0.06, ((spec.barrelHalfWidth - spec.parapetThickness) * 2.0) })).value(), .material = asphalt, .placement = Vec3{ 0, (spec.deckTopY + 0.02), 0 } }).value();

	{
		const double from = spec.deckHalfLength;
		const double to = spec.terrainHalfX();
		const double length = (to - from);
		const auto roadShape = assembly.addMesh(Mesh3D::Box(Vec3{ length, spec.roadThickness, (spec.barrelHalfWidth * 2.0) })).value();
		const auto kerbShape = assembly.addMesh(Mesh3D::Box(Vec3{ length, spec.kerbHeight, spec.kerbWidth })).value();

		for (int32 side = 0; side < 2; ++side)
		{
			const double sign = ((side == 0) ? 1.0 : -1.0);
			(void)assembly.addPart({ .name = U"approach_road_{}"_fmt(side), .mesh = roadShape, .material = asphalt, .placement = Vec3{ (sign * (from + (length * 0.5))), (spec.roadTopY - (spec.roadThickness * 0.5)), 0 } }).value();

			for (int32 k = 0; k < 2; ++k)
			{
				const double zSign = ((k == 0) ? 1.0 : -1.0);
				(void)assembly.addPart({ .name = U"kerb_{}_{}"_fmt(side, k), .mesh = kerbShape, .material = coping, .placement = Vec3{ (sign * (from + (length * 0.5))), (spec.roadTopY + (spec.kerbHeight * 0.5)),
						(zSign * (spec.barrelHalfWidth - (spec.kerbWidth * 0.5))) } }).value();
			}
		}
	}

	// ---- balustrade ----

	{
		const double plinthY = (spec.deckTopY + (spec.parapetPlinthHeight * 0.5));
		const double balusterBase = (spec.deckTopY + spec.parapetPlinthHeight);
		const double copingY = (balusterBase + spec.balusterHeight + (spec.copingHeight * 0.5));
		const double zOffset = (spec.barrelHalfWidth - (spec.parapetThickness * 0.5));

		const auto plinthShape = assembly.addMesh(Mesh3D::Box(Vec3{ (spec.deckHalfLength * 2.0), spec.parapetPlinthHeight, spec.parapetThickness })).value();
		const auto copingShape = assembly.addMesh(Mesh3D::Box(Vec3{ (spec.deckHalfLength * 2.0), spec.copingHeight, (spec.parapetThickness + 0.12) })).value();
		const auto postShape = assembly.addMesh(Mesh3D::Box(Vec3{ spec.postSize, (spec.balusterHeight + 0.06), spec.postSize })).value();
		const auto postCapShape = assembly.addMesh(Mesh3D::Box(Vec3{ (spec.postSize + 0.14), 0.14, (spec.postSize + 0.14) })).value();
		const auto balusterShape = assembly.addMesh([&]()
			{
				const double h = spec.balusterHeight;
				const Array<Vec2> profile{
					Vec2{ 0.0, 0.0 }, Vec2{ 0.105, 0.0 }, Vec2{ 0.105, (h * 0.09) }, Vec2{ 0.072, (h * 0.16) },
					Vec2{ 0.098, (h * 0.34) }, Vec2{ 0.066, (h * 0.58) }, Vec2{ 0.050, (h * 0.74) },
					Vec2{ 0.086, (h * 0.86) }, Vec2{ 0.086, h }, Vec2{ 0.0, h } };
				return Mesh3D::Revolve(profile, RevolveOptions{ .segments = 16, .smoothingAngle = 40_deg });
			}()).value();

		for (int32 side = 0; side < 2; ++side)
		{
			const double sign = ((side == 0) ? 1.0 : -1.0);
			(void)assembly.addPart({ .name = U"parapet_plinth_{}"_fmt(side), .mesh = plinthShape, .material = stone, .placement = Vec3{ 0, plinthY, (sign * zOffset) } }).value();
			(void)assembly.addPart({ .name = U"parapet_coping_{}"_fmt(side), .mesh = copingShape, .material = coping, .placement = Vec3{ 0, copingY, (sign * zOffset) } }).value();

			for (int32 i = 0; i < spec.postCount; ++i)
			{
				const double x = (-spec.deckHalfLength
					+ ((spec.deckHalfLength * 2.0 * i) / (spec.postCount - 1.0)));
				(void)assembly.addPart({ .name = U"parapet_post_{}_{}"_fmt(side, i), .mesh = postShape, .material = stone, .placement = Vec3{ x, (balusterBase + ((spec.balusterHeight + 0.06) * 0.5)), (sign * zOffset) } }).value();
				(void)assembly.addPart({ .name = U"parapet_post_cap_{}_{}"_fmt(side, i), .mesh = postCapShape, .material = coping, .placement = Vec3{ x, (balusterBase + spec.balusterHeight + 0.13), (sign * zOffset) } }).value();
			}

			for (int32 bay = 0; bay < (spec.postCount - 1); ++bay)
			{
				const double bayLength = ((spec.deckHalfLength * 2.0) / (spec.postCount - 1.0));
				const double bayStart = (-spec.deckHalfLength + (bayLength * bay) + (spec.postSize * 0.5));
				const double usable = (bayLength - spec.postSize);

				for (int32 k = 0; k < spec.balustersPerBay; ++k)
				{
					const double x = (bayStart + ((usable * (k + 0.5)) / spec.balustersPerBay));
					(void)assembly.addPart({ .name = U"baluster_{}_{}_{}"_fmt(side, bay, k), .mesh = balusterShape, .material = coping, .placement = Vec3{ x, balusterBase, (sign * zOffset) } }).value();
				}
			}
		}
	}

	// ---- boulders and trees ----

	{
		const auto boulderShape = assembly.addMesh(Mesh3D::IcoSphere(1.0, 2)).value();

		for (int32 i = 0; i < 8; ++i)
		{
			const GroundFeature& rock = BridgeBoulders[i];
			const double y = (TerrainHeight(spec, rock.x, rock.z) + (rock.scale * 0.45));
			(void)assembly.addPart({ .name = U"boulder_{}"_fmt(i), .mesh = boulderShape, .material = boulder, .placement = Mesh3DPlacement{ (Mat4x4::Scale(Float3{ static_cast<float>(rock.scale * 1.25),
						static_cast<float>(rock.scale * 0.8), static_cast<float>(rock.scale) })
					* Mat4x4::Rotate(Quaternion::RotateY(static_cast<float>(i * 37_deg)))
					* Mat4x4::Translate(Float3{ static_cast<float>(rock.x), static_cast<float>(y),
						static_cast<float>(rock.z) })) } }).value();
		}

		const auto trunkShape = assembly.addMesh(Mesh3D::ConicalFrustum(0.26, 0.15, 2.30, 12)).value();
		const auto crownShape = assembly.addMesh(Mesh3D::IcoSphere(1.0, 2)).value();

		for (int32 i = 0; i < 5; ++i)
		{
			const GroundFeature& tree = BridgeTrees[i];
			const double ground = TerrainHeight(spec, tree.x, tree.z);
			const String tag = U"tree_{}"_fmt(i);
			const auto pivot = assembly.addPart({ .name = tag, .placement = Mesh3DPlacement{ (Mat4x4::Scale(static_cast<float>(tree.scale))
					* Mat4x4::Translate(Float3{ static_cast<float>(tree.x),
						static_cast<float>(ground - BridgeTreePlantDepth), static_cast<float>(tree.z) })) } }).value();

			(void)assembly.addPart({ .name = tag + U"_trunk", .mesh = trunkShape, .material = bark, .parent = pivot, .placement = Vec3{ 0, 1.15, 0 } }).value();
			(void)assembly.addPart({ .name = tag + U"_crown_0", .mesh = crownShape, .material = foliage, .parent = pivot, .placement = Mesh3DPlacement{ (Mat4x4::Scale(Float3{ 1.35f, 1.05f, 1.35f })
					* Mat4x4::Translate(Float3{ 0.0f, 2.75f, 0.0f })) } }).value();
			(void)assembly.addPart({ .name = tag + U"_crown_1", .mesh = crownShape, .material = foliage, .parent = pivot, .placement = Mesh3DPlacement{ (Mat4x4::Scale(Float3{ 1.0f, 0.85f, 1.0f })
					* Mat4x4::Translate(Float3{ 0.45f, 2.10f, -0.35f })) } }).value();
			(void)assembly.addPart({ .name = tag + U"_crown_2", .mesh = crownShape, .material = foliage, .parent = pivot, .placement = Mesh3DPlacement{ (Mat4x4::Scale(Float3{ 0.95f, 0.80f, 0.95f })
					* Mat4x4::Translate(Float3{ -0.40f, 2.25f, 0.40f })) } }).value();
		}
	}

	if (not assembly.saveOBJ(U"models/gorge_arch_bridge.obj"))
	{
		throw Error{ U"Failed to export gorge_arch_bridge" };
	}
}
```

---

## 4. タンク機関車と線路

スポーク動輪・実際に閉じるスライダクランク機構・軸箱と軸箱守を備えた台枠・先台車と従台車・バラスト軌道。

使っている主な API: `Extrude`（レール断面・台枠板・肉抜き穴）、`Tube`（ボイラ・主連棒）、`HollowCylinder`（タイヤとフランジ）、`Polygon` の穴（キャブ窓）。

```cpp
# include <Siv3D.hpp> // Siv3D v0.8.0

using MaterialID = Mesh3DAssembly::MaterialID;

/// @brief Mesh3DBuilder で組んだ複数の形状を 1 つの Mesh3D にまとめます。
template <class Compose>
[[nodiscard]]
Mesh3D BuildMesh(Compose compose)
{
	Mesh3DBuilder builder;
	compose(builder);
	return builder.obtainMesh();
}

/// @brief Reverses a contour when needed so that Polygon and Loft accept it.
[[nodiscard]]
Array<Vec2> PositivelyWound(Array<Vec2> points)
{
	// Siv3D screen coordinates: clockwise means positive signed area.
	// This only fixes winding; it does not repair a self-intersecting contour.
	if (not Geometry2D::IsClockwise(points))
	{
		points.reverse();
	}

	return points;
}

/// @brief Reverses a contour so that it is accepted as a Polygon hole.
[[nodiscard]]
Array<Vec2> NegativelyWound(Array<Vec2> points)
{
	if (Geometry2D::IsClockwise(points))
	{
		points.reverse();
	}

	return points;
}

//==========================================================================
//
//	Model 4: 0-6-0 side tank locomotive on a length of track
//
//	A mechanical subject: spoked wheels built from repeated parts, a real
//	slider-crank linkage whose rod length is a design constant, boiler
//	fittings, a cab with cut window openings, and a ballasted track.
//
//==========================================================================

struct LocomotiveSpec
{
	// Track. The rail head top is the y = 0 datum.
	double gauge = 1.435;
	double railHeight = 0.17;
	double railHeadHalfWidth = 0.038;
	double trackHalfLength = 15.00;
	double sleeperSpacing = 0.65;
	double sleeperHalfLength = 1.30;
	double sleeperHeight = 0.26;
	double sleeperThickness = 0.24;
	// The ballast is packed up around the sleepers, as on real track, so the wheels do not
	// appear to hover over a distant surface.
	double ballastTopY = -0.30;
	double ballastTopHalfWidth = 2.10;
	double ballastBottomHalfWidth = 2.95;
	double ballastHeight = 0.62;
	// Wheels.
	double driverRadius = 0.86;
	double driverSpacing = 2.10;
	double driverCentreX = 0.25;
	double wheelHalfGauge = 0.71750;
	double wheelWidth = 0.13;
	double flangeDepth = 0.034;
	double flangeWidth = 0.030;
	double flangeInnerCut = 0.020;
	double tyreThickness = 0.09;
	int32 tyreSegments = 48;
	int32 spokeCount = 12;
	double crankRadius = 0.34;
	double crankAngleNear = 45_deg;      // +Z side
	double crankAngleFar = -45_deg;      // -Z side, quartered
	double crankPinHalfGauge = 0.87;
	double ponyRadius = 0.46;
	double ponyX = 4.05;
	double trailingRadius = 0.52;
	double trailingX = -3.95;
	// Frames and running board. The frame plates drop past the driving axle centres and
	// carry a horn gap at each axle, so every axle is held by the chassis.
	double frameHalfGauge = 0.50;
	double frameThickness = 0.09;
	double frameTopY = 1.55;
	double frameDeepBottomY = 0.62;
	double frameEndBottomY = 1.02;
	double frameDeepMargin = 1.00;      // how far the deep section runs past the outer drivers
	double frameFrontX = 4.50;
	double frameBackX = -4.55;
	double hornHalfWidth = 0.19;
	double hornTopY = 1.06;
	double hornGuideWidth = 0.06;
	double lighteningHoleHalfWidth = 0.30;
	double lighteningHoleBottomY = 1.16;
	double lighteningHoleTopY = 1.42;
	double axleRadius = 0.11;
	double carryingAxleRadius = 0.09;
	double axleboxLength = 0.34;
	double axleboxHeight = 0.30;
	double axleboxWidth = 0.20;
	double springLength = 0.74;
	double springHeight = 0.10;
	double springY = 1.17;
	double stretcherHeight = 0.34;
	double stretcherY = 1.36;
	double stretcherLength = 0.18;
	double runningBoardY = 1.60;
	double runningBoardHalfWidth = 1.30;
	// Carrying trucks: each swings on a pivot pin that passes through the main frames.
	double truckHalfGauge = 0.34;
	double truckPlateThickness = 0.09;
	double truckPlateHalfDepth = 0.15;
	double ponyPivotX = 2.88;
	double ponyPivotY = 0.80;
	double trailingPivotX = -2.90;
	double trailingPivotY = 0.84;
	double pivotPinRadius = 0.08;
	double pivotPinHalfLength = 0.56;
	// Brake gear.
	double brakeShoeGap = 0.06;
	double brakeShoeLength = 0.12;
	double brakeShoeHalfGauge = 0.69;
	double brakeHangerHalfGauge = 0.62;
	// Boiler group.
	double boilerRadius = 0.78;
	double boilerAxisY = 2.20;
	double boilerBackX = -1.70;
	double boilerFrontX = 3.10;
	double smokeboxRadius = 0.84;
	double smokeboxFrontX = 4.40;
	double chimneyX = 3.75;
	double chimneyTopY = 3.92;
	double steamDomeX = 1.35;
	double sandDomeX = 2.35;
	// Firebox and cab.
	double fireboxBackX = -2.90;
	double fireboxTopY = 2.60;
	double cabFrontX = -1.55;
	double cabBackX = -3.70;
	double cabHalfWidth = 1.25;
	double cabWallThickness = 0.09;
	double cabFloorY = 1.66;
	double cabWallTopY = 3.15;
	double cabRoofRise = 0.24;
	double cabRoofThickness = 0.09;
	double cabRoofOverhang = 0.30;
	double windowInsetX = 0.35;
	double windowBottomY = 2.30;
	double windowTopY = 2.95;
	// Tanks and bunker.
	double tankInnerZ = 0.82;
	double tankOuterZ = 1.24;
	double tankFrontX = 3.00;
	double tankBackX = -1.30;
	double tankTopY = 2.66;
	double bunkerBackX = -4.70;
	double bunkerTopY = 2.70;
	// Motion.
	double cylinderAxisY = 1.40;
	double cylinderHalfGauge = 0.95;
	double cylinderBackX = 2.55;
	double cylinderFrontX = 3.65;
	double mainRodLength = 1.90;
	double mainRodRadius = 0.055;
	double couplingRodHeight = 0.20;
	double couplingRodThickness = 0.08;
	// Buffers.
	double frontBeamX = 4.72;
	double rearBeamX = -4.90;
	double beamThickness = 0.22;
	double bufferHalfGauge = 0.86;
	// ---- derived ----

	[[nodiscard]] double driverX(int32 i) const noexcept
	{
		return (driverCentreX + ((i - 1) * driverSpacing));
	}
	[[nodiscard]] double crankAngle(int32 side) const noexcept
	{
		return ((side == 0) ? crankAngleNear : crankAngleFar);
	}
	[[nodiscard]] Vec3 crankPin(int32 side, int32 i) const noexcept
	{
		const double angle = crankAngle(side);
		return Vec3{ (driverX(i) + (crankRadius * std::cos(angle))),
			(driverRadius + (crankRadius * std::sin(angle))),
			(((side == 0) ? 1.0 : -1.0) * crankPinHalfGauge) };
	}
	/// @brief Crosshead position from the slider-crank closure, not from a hand placed value.
	[[nodiscard]] Vec3 crosshead(int32 side) const noexcept
	{
		const Vec3 pin = crankPin(side, 1);
		const double dy = (cylinderAxisY - pin.y);
		return Vec3{ (pin.x + std::sqrt((mainRodLength * mainRodLength) - (dy * dy))), cylinderAxisY, pin.z };
	}
	[[nodiscard]] int32 sleeperCount() const noexcept
	{
		return ((2 * static_cast<int32>(trackHalfLength / sleeperSpacing)) + 1);
	}
	[[nodiscard]] double ballastBottomY() const noexcept { return (ballastTopY - ballastHeight); }
	/// @brief Distance from the wheel centre line to the middle of the flange.
	[[nodiscard]] double flangeHalfGauge() const noexcept
	{
		return (wheelHalfGauge - (wheelWidth * 0.5) - (flangeWidth * 0.5));
	}
	[[nodiscard]] double frameDeepFrontX() const noexcept { return (driverX(2) + frameDeepMargin); }
	[[nodiscard]] double frameDeepBackX() const noexcept { return (driverX(0) - frameDeepMargin); }
	[[nodiscard]] double frameOuterZ() const noexcept { return (frameHalfGauge + (frameThickness * 0.5)); }
	/// @brief Distance from a driving axle centre to the face of its brake shoe.
	[[nodiscard]] double brakeShoeReach() const noexcept
	{
		return (driverRadius + flangeDepth + brakeShoeGap + (brakeShoeLength * 0.5));
	}
	/// @brief X positions of the four frame lightening holes and cross stretchers.
	[[nodiscard]] double bayX(int32 i) const noexcept
	{
		return (driverX(0) - (driverSpacing * 0.5) + (i * driverSpacing));
	}
};

/// @brief The painted centre of one wheel: rim band, hub, spokes and an optional crank boss.
/// @remark The bright tyre and its flange are separate parts, because the flange has to be
/// placed on the inboard side of each wheel and that side differs between the two rails.
/// @param crankRadius Adds a crank boss on the spoke at local +X when positive.
void BuildSpokedWheel(Mesh3DBuilder& builder, const LocomotiveSpec& spec, double radius, int32 spokes, double crankRadius)
{
	const double webRadius = (radius - spec.tyreThickness);
	const double hubRadius = Max((radius * 0.20), 0.13);

	// No solid web: the gaps between the spokes stay open so the wheel reads as a spoked wheel.
	(void)builder.addHollowCylinder((webRadius - 0.035), webRadius, (spec.wheelWidth * 0.55), spec.tyreSegments).value();
	(void)builder.addCylinder(hubRadius, (spec.wheelWidth * 1.7), 20).value();

	const double spokeLength = (webRadius - hubRadius);

	for (int32 k = 0; k < spokes; ++k)
	{
		const double angle = ((Math::TwoPi * k) / spokes);
		(void)builder.addBox(Vec3{ spokeLength, (spec.wheelWidth * 0.85), (radius * 0.085) },
			Mesh3DPlacement{ (Mat4x4::Translate(Float3{ static_cast<float>(hubRadius + (spokeLength * 0.5)), 0.0f, 0.0f })
				* Mat4x4::Rotate(Quaternion::RotateY(static_cast<float>(angle)))) }).value();
	}

	if (crankRadius > 0.0)
	{
		// The crank boss sits on the spoke at local +X, which the placement turns to the crank angle.
		(void)builder.addCylinder(0.145, (spec.wheelWidth * 1.5), 16,
			Vec3{ crankRadius, 0, 0 }).value();
	}

}

void Main()
{
	const LocomotiveSpec spec;

	Mesh3DAssembly assembly;

	const MaterialID lined = assembly.addMaterial(Material{ .name = U"boiler_black", .baseColor = ColorF{ 0.10, 0.12, 0.12 },
		.roughness = 0.35 });
	const MaterialID green = assembly.addMaterial(Material{ .name = U"loco_green", .baseColor = ColorF{ 0.09, 0.24, 0.18 },
		.roughness = 0.40 });
	const MaterialID red = assembly.addMaterial(Material{ .name = U"frame_red", .baseColor = ColorF{ 0.42, 0.10, 0.09 },
		.roughness = 0.50 });
	const MaterialID steel = assembly.addMaterial(Material{ .name = U"bright_steel", .baseColor = ColorF{ 0.46, 0.48, 0.50 },
		.roughness = 0.30 });
	const MaterialID brass = assembly.addMaterial(Material{ .name = U"brass_fittings", .baseColor = ColorF{ 0.74, 0.57, 0.23 },
		.roughness = 0.30 });
	const MaterialID roofGrey = assembly.addMaterial(Material{ .name = U"cab_roof", .baseColor = ColorF{ 0.29, 0.31, 0.32 },
		.roughness = 0.70 });
	const MaterialID timber = assembly.addMaterial(Material{ .name = U"sleeper_timber", .baseColor = ColorF{ 0.33, 0.25, 0.17 },
		.roughness = 0.95 });
	const MaterialID ballastStone = assembly.addMaterial(Material{ .name = U"ballast", .baseColor = ColorF{ 0.51, 0.49, 0.46 },
		.roughness = 0.95 });
	const MaterialID coal = assembly.addMaterial(Material{ .name = U"coal", .baseColor = ColorF{ 0.09, 0.09, 0.10 },
		.roughness = 0.60 });

	// ---- track ----

	{
		const Quaternion alongX = (Quaternion::RotateX(90_deg) * Quaternion::RotateY(90_deg));

		(void)assembly.addPart({ .name = U"ballast", .mesh = assembly.addMesh(BuildMesh([&](Mesh3DBuilder& builder)
			{
				const Array<Vec2> contour = PositivelyWound({
					Vec2{ -spec.ballastBottomHalfWidth, spec.ballastBottomY() },
					Vec2{ spec.ballastBottomHalfWidth, spec.ballastBottomY() },
					Vec2{ spec.ballastTopHalfWidth, spec.ballastTopY },
					Vec2{ -spec.ballastTopHalfWidth, spec.ballastTopY } });
				(void)builder.addExtrude(Polygon{ contour }, (spec.trackHalfLength * 2.0), alongX).value();
			})).value(), .material = ballastStone }).value();

		const auto railShape = assembly.addMesh(BuildMesh([&](Mesh3DBuilder& builder)
			{
				const double foot = 0.058;
				const double web = 0.019;
				const double head = spec.railHeadHalfWidth;
				const Array<Vec2> contour = PositivelyWound({
					Vec2{ -foot, -spec.railHeight }, Vec2{ foot, -spec.railHeight },
					Vec2{ foot, (-spec.railHeight + 0.030) }, Vec2{ web, (-spec.railHeight + 0.045) },
					Vec2{ web, -0.048 }, Vec2{ head, -0.036 }, Vec2{ head, 0.0 },
					Vec2{ -head, 0.0 }, Vec2{ -head, -0.036 }, Vec2{ -web, -0.048 },
					Vec2{ -web, (-spec.railHeight + 0.045) }, Vec2{ -foot, (-spec.railHeight + 0.030) } });
				(void)builder.addExtrude(Polygon{ contour }, (spec.trackHalfLength * 2.0), alongX).value();
			})).value();

		for (int32 side = 0; side < 2; ++side)
		{
			const double sign = ((side == 0) ? 1.0 : -1.0);
			(void)assembly.addPart({ .name = U"rail_{}"_fmt(side), .mesh = railShape, .material = steel, .placement = Vec3{ 0, 0, (sign * (spec.gauge * 0.5)) } }).value();
		}

		const auto sleeperShape = assembly.addMesh(Mesh3D::Box(Vec3{ spec.sleeperThickness, spec.sleeperHeight, (spec.sleeperHalfLength * 2.0) })).value();
		const int32 half = ((spec.sleeperCount() - 1) / 2);

		for (int32 i = -half; i <= half; ++i)
		{
			(void)assembly.addPart({ .name = U"sleeper_{:03d}"_fmt((i + half)), .mesh = sleeperShape, .material = timber, .placement = Vec3{ (i * spec.sleeperSpacing), (-spec.railHeight - (spec.sleeperHeight * 0.5)), 0 } }).value();
		}
	}

	// ---- wheels ----

	{
		const Quaternion stand = Quaternion::RotateX(90_deg);

		const auto driverShape = assembly.addMesh(BuildMesh([&](Mesh3DBuilder& builder)
			{
				return BuildSpokedWheel(builder, spec, spec.driverRadius, spec.spokeCount, spec.crankRadius);
			})).value();
		const auto ponyShape = assembly.addMesh(BuildMesh([&](Mesh3DBuilder& builder)
			{
				return BuildSpokedWheel(builder, spec, spec.ponyRadius, 8, 0.0);
			})).value();
		const auto trailingShape = assembly.addMesh(BuildMesh([&](Mesh3DBuilder& builder)
			{
				return BuildSpokedWheel(builder, spec, spec.trailingRadius, 8, 0.0);
			})).value();
		// Bright tyre and, on the inboard face, a flange that drops past the rail head.
		const auto tyreShape = [&](double radius)
			{
				return assembly.addMesh(Mesh3D::HollowCylinder((radius - spec.tyreThickness),
					radius, spec.wheelWidth, spec.tyreSegments)).value();
			};
		const auto flangeShape = [&](double radius)
			{
				return assembly.addMesh(Mesh3D::HollowCylinder((radius - spec.flangeInnerCut),
					(radius + spec.flangeDepth), spec.flangeWidth, spec.tyreSegments)).value();
			};
		const auto driverTyre = tyreShape(spec.driverRadius);
		const auto ponyTyre = tyreShape(spec.ponyRadius);
		const auto trailingTyre = tyreShape(spec.trailingRadius);
		const auto driverFlange = flangeShape(spec.driverRadius);
		const auto ponyFlange = flangeShape(spec.ponyRadius);
		const auto trailingFlange = flangeShape(spec.trailingRadius);

		const auto pinShape = assembly.addMesh(Mesh3D::Cylinder(0.085, 0.20, 16)).value();

		for (int32 side = 0; side < 2; ++side)
		{
			const double sign = ((side == 0) ? 1.0 : -1.0);
			const double angle = spec.crankAngle(side);

			for (int32 i = 0; i < 3; ++i)
			{
				(void)assembly.addPart({ .name = U"driver_{}_{}"_fmt(side, i), .mesh = driverShape, .material = red, .placement = Mesh3DPlacement{ Vec3{ spec.driverX(i), spec.driverRadius, (sign * spec.wheelHalfGauge) },
						(Quaternion::RotateY(static_cast<float>(angle)) * stand) } }).value();

				(void)assembly.addPart({ .name = U"driver_tyre_{}_{}"_fmt(side, i), .mesh = driverTyre, .material = steel, .placement = Mesh3DPlacement{ Vec3{ spec.driverX(i), spec.driverRadius, (sign * spec.wheelHalfGauge) }, stand } }).value();
				(void)assembly.addPart({ .name = U"driver_flange_{}_{}"_fmt(side, i), .mesh = driverFlange, .material = steel, .placement = Mesh3DPlacement{ Vec3{ spec.driverX(i), spec.driverRadius, (sign * spec.flangeHalfGauge()) }, stand } }).value();

				const Vec3 pin = spec.crankPin(side, i);
				(void)assembly.addPart({ .name = U"crank_pin_{}_{}"_fmt(side, i), .mesh = pinShape, .material = steel, .placement = Mesh3DPlacement{ Vec3{ pin.x, pin.y, pin.z }, stand } }).value();
			}

			(void)assembly.addPart({ .name = U"pony_wheel_{}"_fmt(side), .mesh = ponyShape, .material = red, .placement = Mesh3DPlacement{ Vec3{ spec.ponyX, spec.ponyRadius, (sign * spec.wheelHalfGauge) }, stand } }).value();
			(void)assembly.addPart({ .name = U"pony_tyre_{}"_fmt(side), .mesh = ponyTyre, .material = steel, .placement = Mesh3DPlacement{ Vec3{ spec.ponyX, spec.ponyRadius, (sign * spec.wheelHalfGauge) }, stand } }).value();
			(void)assembly.addPart({ .name = U"pony_flange_{}"_fmt(side), .mesh = ponyFlange, .material = steel, .placement = Mesh3DPlacement{ Vec3{ spec.ponyX, spec.ponyRadius, (sign * spec.flangeHalfGauge()) }, stand } }).value();
			(void)assembly.addPart({ .name = U"trailing_wheel_{}"_fmt(side), .mesh = trailingShape, .material = red, .placement = Mesh3DPlacement{ Vec3{ spec.trailingX, spec.trailingRadius, (sign * spec.wheelHalfGauge) }, stand } }).value();
			(void)assembly.addPart({ .name = U"trailing_tyre_{}"_fmt(side), .mesh = trailingTyre, .material = steel, .placement = Mesh3DPlacement{ Vec3{ spec.trailingX, spec.trailingRadius, (sign * spec.wheelHalfGauge) }, stand } }).value();
			(void)assembly.addPart({ .name = U"trailing_flange_{}"_fmt(side), .mesh = trailingFlange, .material = steel, .placement = Mesh3DPlacement{ Vec3{ spec.trailingX, spec.trailingRadius, (sign * spec.flangeHalfGauge()) }, stand } }).value();
		}
	}

	// ---- frames, hornblocks, axles, carrying trucks, running board, buffer beams ----

	{
		// Frame plate: deep over the driving wheelbase with a horn gap at each axle,
		// stepped up at both ends to clear the carrying trucks, lightened between the bays.
		const auto frameShape = assembly.addMesh(BuildMesh([&](Mesh3DBuilder& builder)
			{
				Array<Vec2> outer;
				outer << Vec2{ spec.frameBackX, spec.frameEndBottomY };
				outer << Vec2{ spec.frameDeepBackX(), spec.frameEndBottomY };
				outer << Vec2{ spec.frameDeepBackX(), spec.frameDeepBottomY };

				for (int32 i = 0; i < 3; ++i)
				{
					const double x = spec.driverX(i);
					outer << Vec2{ (x - spec.hornHalfWidth), spec.frameDeepBottomY };
					outer << Vec2{ (x - spec.hornHalfWidth), spec.hornTopY };
					outer << Vec2{ (x + spec.hornHalfWidth), spec.hornTopY };
					outer << Vec2{ (x + spec.hornHalfWidth), spec.frameDeepBottomY };
				}

				outer << Vec2{ spec.frameDeepFrontX(), spec.frameDeepBottomY };
				outer << Vec2{ spec.frameDeepFrontX(), spec.frameEndBottomY };
				outer << Vec2{ spec.frameFrontX, spec.frameEndBottomY };
				outer << Vec2{ spec.frameFrontX, spec.frameTopY };
				outer << Vec2{ spec.frameBackX, spec.frameTopY };

				Array<Array<Vec2>> holes;

				for (int32 i = 0; i < 4; ++i)
				{
					const double x = spec.bayX(i);
					holes << NegativelyWound({
						Vec2{ (x - spec.lighteningHoleHalfWidth), spec.lighteningHoleBottomY },
						Vec2{ (x + spec.lighteningHoleHalfWidth), spec.lighteningHoleBottomY },
						Vec2{ (x + spec.lighteningHoleHalfWidth), spec.lighteningHoleTopY },
						Vec2{ (x - spec.lighteningHoleHalfWidth), spec.lighteningHoleTopY } });
				}

				(void)builder.addExtrude(Polygon{ PositivelyWound(std::move(outer)), std::move(holes) },
					spec.frameThickness, Quaternion::RotateX(90_deg)).value();
			})).value();

		const auto axleboxShape = assembly.addMesh(Mesh3D::Box(Vec3{ spec.axleboxLength, spec.axleboxHeight, spec.axleboxWidth })).value();
		const auto hornGuideShape = assembly.addMesh(Mesh3D::Box(Vec3{ spec.hornGuideWidth, (spec.hornTopY - spec.frameDeepBottomY),
				(spec.frameThickness + 0.05) })).value();
		const auto springShape = assembly.addMesh(Mesh3D::Box(Vec3{ spec.springLength, spec.springHeight, (spec.frameThickness + 0.06) })).value();

		for (int32 side = 0; side < 2; ++side)
		{
			const double sign = ((side == 0) ? 1.0 : -1.0);
			const double z = (sign * spec.frameHalfGauge);
			(void)assembly.addPart({ .name = U"frame_{}"_fmt(side), .mesh = frameShape, .material = red, .placement = Vec3{ 0, 0, z } }).value();

			for (int32 i = 0; i < 3; ++i)
			{
				const double x = spec.driverX(i);
				(void)assembly.addPart({ .name = U"axlebox_{}_{}"_fmt(side, i), .mesh = axleboxShape, .material = steel, .placement = Vec3{ x, spec.driverRadius, z } }).value();
				(void)assembly.addPart({ .name = U"spring_{}_{}"_fmt(side, i), .mesh = springShape, .material = steel, .placement = Vec3{ x, spec.springY, z } }).value();

				for (int32 k = 0; k < 2; ++k)
				{
					const double xSign = ((k == 0) ? -1.0 : 1.0);
					(void)assembly.addPart({ .name = U"horn_guide_{}_{}_{}"_fmt(side, i, k), .mesh = hornGuideShape, .material = steel, .placement = Vec3{ (x + (xSign * (spec.hornHalfWidth + (spec.hornGuideWidth * 0.5)))),
							((spec.hornTopY + spec.frameDeepBottomY) * 0.5), z } }).value();
				}
			}
		}

		// Cross stretchers tie the two frame plates together, visible through the holes.
		{
			const auto stretcherShape = assembly.addMesh(Mesh3D::Box(Vec3{ spec.stretcherLength, spec.stretcherHeight, (spec.frameOuterZ() * 2.0) })).value();

			for (int32 i = 0; i < 4; ++i)
			{
				(void)assembly.addPart({ .name = U"frame_stretcher_{}"_fmt(i), .mesh = stretcherShape, .material = red, .placement = Vec3{ spec.bayX(i), spec.stretcherY, 0 } }).value();
			}
		}

		// Axles. Every wheelset has one, and it runs right through both frame planes.
		{
			const auto driverAxleShape = assembly.addMesh(Mesh3D::Cylinder(spec.axleRadius, (spec.wheelHalfGauge * 2.0), 20)).value();
			const auto carryingAxleShape = assembly.addMesh(Mesh3D::Cylinder(spec.carryingAxleRadius, (spec.wheelHalfGauge * 2.0), 16)).value();

			for (int32 i = 0; i < 3; ++i)
			{
				(void)assembly.addPart({ .name = U"driver_axle_{}"_fmt(i), .mesh = driverAxleShape, .material = steel, .placement = Mesh3DPlacement{ Vec3{ spec.driverX(i), spec.driverRadius, 0 }, Quaternion::RotateX(90_deg) } }).value();
			}

			(void)assembly.addPart({ .name = U"pony_axle", .mesh = carryingAxleShape, .material = steel, .placement = Mesh3DPlacement{ Vec3{ spec.ponyX, spec.ponyRadius, 0 }, Quaternion::RotateX(90_deg) } }).value();
			(void)assembly.addPart({ .name = U"trailing_axle", .mesh = carryingAxleShape, .material = steel, .placement = Mesh3DPlacement{ Vec3{ spec.trailingX, spec.trailingRadius, 0 }, Quaternion::RotateX(90_deg) } }).value();
		}

		// Carrying trucks: side plates from the axle back to a pivot pin that passes
		// through both main frames, so the leading and trailing wheels hang off the chassis.
		{
			struct Truck { String name; double pivotX; double pivotY; double axleX; double axleY; };
			const Truck trucks[2] = {
				{ U"pony", spec.ponyPivotX, spec.ponyPivotY, spec.ponyX, spec.ponyRadius },
				{ U"trailing", spec.trailingPivotX, spec.trailingPivotY, spec.trailingX, spec.trailingRadius } };

			const auto pinShape = assembly.addMesh(Mesh3D::Cylinder(spec.pivotPinRadius, (spec.pivotPinHalfLength * 2.0), 16)).value();
			const auto truckBoxShape = assembly.addMesh(Mesh3D::Box(Vec3{ 0.24, 0.26, (spec.truckPlateThickness + 0.10) })).value();

			for (const auto& truck : trucks)
			{
				const auto plateShape = assembly.addMesh(BuildMesh([&](Mesh3DBuilder& builder)
					{
						const double d = spec.truckPlateHalfDepth;
						const Array<Vec2> contour = PositivelyWound({
							Vec2{ truck.pivotX, (truck.pivotY - d) }, Vec2{ truck.axleX, (truck.axleY - d) },
							Vec2{ truck.axleX, (truck.axleY + d) }, Vec2{ truck.pivotX, (truck.pivotY + d) } });
						(void)builder.addExtrude(Polygon{ contour }, spec.truckPlateThickness,
							Quaternion::RotateX(90_deg)).value();
					})).value();

				(void)assembly.addPart({ .name = truck.name + U"_truck_pivot", .mesh = pinShape, .material = steel, .placement = Mesh3DPlacement{ Vec3{ truck.pivotX, truck.pivotY, 0 }, Quaternion::RotateX(90_deg) } }).value();

				for (int32 side = 0; side < 2; ++side)
				{
					const double sign = ((side == 0) ? 1.0 : -1.0);
					(void)assembly.addPart({ .name = U"{}_truck_plate_{}"_fmt(truck.name, side), .mesh = plateShape, .material = red, .placement = Vec3{ 0, 0, (sign * spec.truckHalfGauge) } }).value();
					(void)assembly.addPart({ .name = U"{}_truck_axlebox_{}"_fmt(truck.name, side), .mesh = truckBoxShape, .material = steel, .placement = Vec3{ truck.axleX, truck.axleY, (sign * spec.truckHalfGauge) } }).value();
				}
			}
		}

		// Brake gear hanging from the frames behind each driving wheel.
		{
			const auto shoeShape = assembly.addMesh(Mesh3D::Box(Vec3{ spec.brakeShoeLength, 0.36, 0.18 })).value();
			const auto hangerShape = assembly.addMesh(Mesh3D::Box(Vec3{ 0.07, 0.60, 0.09 })).value();

			for (int32 side = 0; side < 2; ++side)
			{
				const double sign = ((side == 0) ? 1.0 : -1.0);

				for (int32 i = 0; i < 3; ++i)
				{
					const double x = (spec.driverX(i) - spec.brakeShoeReach());
					(void)assembly.addPart({ .name = U"brake_shoe_{}_{}"_fmt(side, i), .mesh = shoeShape, .material = steel, .placement = Vec3{ x, spec.driverRadius, (sign * spec.brakeShoeHalfGauge) } }).value();
					(void)assembly.addPart({ .name = U"brake_hanger_{}_{}"_fmt(side, i), .mesh = hangerShape, .material = steel, .placement = Vec3{ x, (spec.driverRadius + 0.34), (sign * spec.brakeHangerHalfGauge) } }).value();
				}
			}
		}

		(void)assembly.addPart({ .name = U"running_board", .mesh = assembly.addMesh(Mesh3D::Box(Vec3{ (spec.frameFrontX - spec.frameBackX), 0.07, (spec.runningBoardHalfWidth * 2.0) })).value(), .material = red, .placement = Vec3{ ((spec.frameFrontX + spec.frameBackX) * 0.5), spec.runningBoardY, 0 } }).value();

		const auto beamShape = assembly.addMesh(Mesh3D::Box(Vec3{ spec.beamThickness, 0.66, (spec.runningBoardHalfWidth * 2.0) })).value();
		const auto bufferShape = assembly.addMesh(Mesh3D::Cylinder(0.17, 0.34, 16)).value();
		const auto guardIronShape = assembly.addMesh(Mesh3D::Box(Vec3{ 0.06, 0.68, 0.11 })).value();

		for (int32 end = 0; end < 2; ++end)
		{
			const double x = ((end == 0) ? spec.frontBeamX : spec.rearBeamX);
			const double sign = ((end == 0) ? 1.0 : -1.0);
			(void)assembly.addPart({ .name = U"buffer_beam_{}"_fmt(end), .mesh = beamShape, .material = red, .placement = Vec3{ x, 1.36, 0 } }).value();

			for (int32 side = 0; side < 2; ++side)
			{
				const double zSign = ((side == 0) ? 1.0 : -1.0);
				(void)assembly.addPart({ .name = U"buffer_{}_{}"_fmt(end, side), .mesh = bufferShape, .material = steel, .placement = Mesh3DPlacement{ Vec3{ (x + (sign * 0.28)), 1.36, (zSign * spec.bufferHalfGauge) },
						Quaternion::RotateZ(90_deg) } }).value();
				(void)assembly.addPart({ .name = U"guard_iron_{}_{}"_fmt(end, side), .mesh = guardIronShape, .material = red, .placement = Vec3{ (x + (sign * 0.06)), 0.69, (zSign * 0.58) } }).value();
			}
		}
	}

	// ---- boiler, smokebox, chimney and domes ----

	{
		(void)assembly.addPart({ .name = U"boiler", .mesh = assembly.addMesh(Mesh3D::Tube({ Vec3{ spec.boilerBackX, spec.boilerAxisY, 0 },
			Vec3{ spec.boilerFrontX, spec.boilerAxisY, 0 } }, spec.boilerRadius,
			TubeOptions{ .sides = 40 })).value(), .material = lined }).value();

		(void)assembly.addPart({ .name = U"smokebox", .mesh = assembly.addMesh(Mesh3D::Tube({ Vec3{ (spec.boilerFrontX - 0.10), spec.boilerAxisY, 0 },
			Vec3{ spec.smokeboxFrontX, spec.boilerAxisY, 0 } }, spec.smokeboxRadius,
			TubeOptions{ .sides = 40 })).value(), .material = lined }).value();

		(void)assembly.addPart({ .name = U"smokebox_door", .mesh = assembly.addMesh(Mesh3D::Hemisphere(spec.smokeboxRadius * 0.94, CloseBottom::Yes, 32, 10)).value(), .material = lined, .placement = Mesh3DPlacement{ Vec3{ spec.smokeboxFrontX, spec.boilerAxisY, 0 }, Quaternion::RotateZ(-90_deg) } }).value();

		(void)assembly.addPart({ .name = U"smokebox_door_hinge", .mesh = assembly.addMesh(Mesh3D::Box(Vec3{ 0.05, 0.10, (spec.smokeboxRadius * 1.7) })).value(), .material = brass, .placement = Vec3{ (spec.smokeboxFrontX + 0.24), spec.boilerAxisY, 0 } }).value();
		(void)assembly.addPart({ .name = U"smokebox_door_dart", .mesh = assembly.addMesh(Mesh3D::Cylinder(0.055, 0.26, 14)).value(), .material = brass, .placement = Mesh3DPlacement{ Vec3{ (spec.smokeboxFrontX + 0.34), spec.boilerAxisY, 0 }, Quaternion::RotateZ(90_deg) } }).value();

		const auto bandShape = assembly.addMesh(Mesh3D::HollowCylinder(spec.boilerRadius, (spec.boilerRadius + 0.025), 0.10, 40)).value();

		for (int32 i = 0; i < 4; ++i)
		{
			(void)assembly.addPart({ .name = U"boiler_band_{}"_fmt(i), .mesh = bandShape, .material = brass, .placement = Mesh3DPlacement{ Vec3{ (-1.10 + (i * 1.00)), spec.boilerAxisY, 0 }, Quaternion::RotateZ(90_deg) } }).value();
		}

		(void)assembly.addPart({ .name = U"chimney", .mesh = assembly.addMesh(BuildMesh([&](Mesh3DBuilder& builder)
			{
				const double base = (spec.boilerAxisY + (spec.smokeboxRadius * 0.55));
				const Array<Vec2> profile{ Vec2{ 0.34, base }, Vec2{ 0.30, (base + 0.16) },
					Vec2{ 0.22, (base + 0.32) }, Vec2{ 0.21, (spec.chimneyTopY - 0.22) },
					Vec2{ 0.30, (spec.chimneyTopY - 0.10) }, Vec2{ 0.32, spec.chimneyTopY },
					Vec2{ 0.24, spec.chimneyTopY }, Vec2{ 0.23, (spec.chimneyTopY - 0.16) },
					Vec2{ 0.17, (spec.chimneyTopY - 0.30) }, Vec2{ 0.17, base } };
				(void)builder.addRevolve(profile, Vec3{ spec.chimneyX, 0, 0 },
					RevolveOptions{ .segments = 32, .smoothingAngle = 30_deg }).value();
			})).value(), .material = lined }).value();

		const auto domeShape = assembly.addMesh([&]()
			{
				const double base = (spec.boilerAxisY + (spec.boilerRadius * 0.62));
				const Array<Vec2> profile{ Vec2{ 0.36, base }, Vec2{ 0.36, (base + 0.20) },
					Vec2{ 0.33, (base + 0.36) }, Vec2{ 0.24, (base + 0.48) }, Vec2{ 0.0, (base + 0.53) } };
				return Mesh3D::Revolve(profile, RevolveOptions{ .segments = 28, .smoothingAngle = 45_deg });
			}()).value();

		(void)assembly.addPart({ .name = U"steam_dome", .mesh = domeShape, .material = brass, .placement = Vec3{ spec.steamDomeX, 0, 0 } }).value();
		(void)assembly.addPart({ .name = U"sand_dome", .mesh = domeShape, .material = lined, .placement = Vec3{ spec.sandDomeX, 0, 0 } }).value();

		(void)assembly.addPart({ .name = U"safety_valve", .mesh = assembly.addMesh(Mesh3D::ConicalFrustum(0.17, 0.12, 0.30, 16)).value(), .material = brass, .placement = Vec3{ 0.10, (spec.boilerAxisY + spec.boilerRadius + 0.09), 0 } }).value();
		(void)assembly.addPart({ .name = U"whistle", .mesh = assembly.addMesh(Mesh3D::Cylinder(0.065, 0.34, 14)).value(), .material = brass, .placement = Vec3{ -0.55, (spec.boilerAxisY + spec.boilerRadius + 0.14), 0 } }).value();

		const auto handrailShape = assembly.addMesh(Mesh3D::Tube({ Vec3{ (spec.smokeboxFrontX - 0.10), 0, 0 },
			Vec3{ (spec.cabFrontX - 0.05), 0, 0 } }, 0.028, TubeOptions{ .sides = 8 })).value();

		for (int32 side = 0; side < 2; ++side)
		{
			const double sign = ((side == 0) ? 1.0 : -1.0);
			(void)assembly.addPart({ .name = U"boiler_handrail_{}"_fmt(side), .mesh = handrailShape, .material = brass, .placement = Vec3{ 0, (spec.boilerAxisY + (spec.boilerRadius * 0.72)), (sign * (spec.boilerRadius * 0.80)) } }).value();
		}

		(void)assembly.addPart({ .name = U"headlamp", .mesh = assembly.addMesh(Mesh3D::Box(Vec3{ 0.26, 0.30, 0.26 })).value(), .material = lined, .placement = Vec3{ (spec.smokeboxFrontX + 0.16), (spec.boilerAxisY + spec.smokeboxRadius + 0.02), 0 } }).value();
		(void)assembly.addPart({ .name = U"headlamp_lens", .mesh = assembly.addMesh(Mesh3D::Cylinder(0.10, 0.05, 16)).value(), .material = brass, .placement = Mesh3DPlacement{ Vec3{ (spec.smokeboxFrontX + 0.31), (spec.boilerAxisY + spec.smokeboxRadius + 0.02), 0 },
				Quaternion::RotateZ(90_deg) } }).value();
	}

	// ---- firebox, tanks and bunker ----

	{
		(void)assembly.addPart({ .name = U"firebox", .mesh = assembly.addMesh(Mesh3D::RectangularFrustum(SizeF{ (spec.boilerBackX - spec.fireboxBackX), 1.62 },
				SizeF{ (spec.boilerBackX - spec.fireboxBackX), 1.34 }, (spec.fireboxTopY - 1.30))).value(), .material = lined, .placement = Vec3{ ((spec.boilerBackX + spec.fireboxBackX) * 0.5), ((spec.fireboxTopY + 1.30) * 0.5), 0 } }).value();

		const auto tankShape = assembly.addMesh(Mesh3D::Box(Vec3{ (spec.tankFrontX - spec.tankBackX), (spec.tankTopY - spec.runningBoardY - 0.04),
				(spec.tankOuterZ - spec.tankInnerZ) })).value();
		const auto tankCapShape = assembly.addMesh(Mesh3D::Cylinder(0.16, 0.09, 16)).value();

		for (int32 side = 0; side < 2; ++side)
		{
			const double sign = ((side == 0) ? 1.0 : -1.0);
			(void)assembly.addPart({ .name = U"side_tank_{}"_fmt(side), .mesh = tankShape, .material = green, .placement = Vec3{ ((spec.tankFrontX + spec.tankBackX) * 0.5),
					((spec.tankTopY + spec.runningBoardY + 0.04) * 0.5),
					(sign * ((spec.tankInnerZ + spec.tankOuterZ) * 0.5)) } }).value();
			(void)assembly.addPart({ .name = U"tank_filler_{}"_fmt(side), .mesh = tankCapShape, .material = brass, .placement = Vec3{ (spec.tankFrontX - 0.45), (spec.tankTopY + 0.045),
					(sign * ((spec.tankInnerZ + spec.tankOuterZ) * 0.5)) } }).value();
		}

		// Beading around each tank side panel, and the cab steps.
		{
			const double x0 = (spec.tankBackX + 0.14);
			const double x1 = (spec.tankFrontX - 0.14);
			const double y0 = (spec.runningBoardY + 0.18);
			const double y1 = (spec.tankTopY - 0.14);
			const double bead = 0.05;
			const auto beadHorizontal = assembly.addMesh(Mesh3D::Box(Vec3{ (x1 - x0), bead, bead })).value();
			const auto beadVertical = assembly.addMesh(Mesh3D::Box(Vec3{ bead, (y1 - y0), bead })).value();
			const auto stepShape = assembly.addMesh(Mesh3D::Box(Vec3{ 0.62, 0.05, 0.30 })).value();

			for (int32 side = 0; side < 2; ++side)
			{
				const double sign = ((side == 0) ? 1.0 : -1.0);
				const double z = (sign * (spec.tankOuterZ + (bead * 0.4)));

				for (int32 k = 0; k < 2; ++k)
				{
					(void)assembly.addPart({ .name = U"tank_bead_h_{}_{}"_fmt(side, k), .mesh = beadHorizontal, .material = brass, .placement = Vec3{ ((x0 + x1) * 0.5), ((k == 0) ? y0 : y1), z } }).value();
					(void)assembly.addPart({ .name = U"tank_bead_v_{}_{}"_fmt(side, k), .mesh = beadVertical, .material = brass, .placement = Vec3{ ((k == 0) ? x0 : x1), ((y0 + y1) * 0.5), z } }).value();
				}

				for (int32 k = 0; k < 2; ++k)
				{
					(void)assembly.addPart({ .name = U"cab_step_{}_{}"_fmt(side, k), .mesh = stepShape, .material = red, .placement = Vec3{ -2.55, (1.02 - (k * 0.34)), (sign * (spec.cabHalfWidth + 0.05)) } }).value();
				}
			}
		}

		(void)assembly.addPart({ .name = U"bunker", .mesh = assembly.addMesh(Mesh3D::Box(Vec3{ (spec.cabBackX - spec.bunkerBackX), (spec.bunkerTopY - spec.cabFloorY),
				(spec.cabHalfWidth * 2.0) })).value(), .material = green, .placement = Vec3{ ((spec.cabBackX + spec.bunkerBackX) * 0.5), ((spec.bunkerTopY + spec.cabFloorY) * 0.5), 0 } }).value();
		(void)assembly.addPart({ .name = U"coal_load", .mesh = assembly.addMesh(Mesh3D::IcoSphere(1.0, 2)).value(), .material = coal, .placement = Mesh3DPlacement{ (Mat4x4::Scale(Float3{ 0.44f, 0.20f, 1.05f })
				* Mat4x4::Translate(Float3{ static_cast<float>((spec.cabBackX + spec.bunkerBackX) * 0.5),
					static_cast<float>(spec.bunkerTopY), 0.0f })) } }).value();
	}

	// ---- cab ----

	{
		const auto sideShape = assembly.addMesh(BuildMesh([&](Mesh3DBuilder& builder)
			{
				const Array<Vec2> outer = PositivelyWound({
					Vec2{ spec.cabBackX, spec.cabFloorY }, Vec2{ spec.cabFrontX, spec.cabFloorY },
					Vec2{ spec.cabFrontX, spec.cabWallTopY }, Vec2{ spec.cabBackX, spec.cabWallTopY } });
				Array<Array<Vec2>> holes;
				holes << NegativelyWound({
					Vec2{ (spec.cabBackX + spec.windowInsetX), spec.windowBottomY },
					Vec2{ (spec.cabFrontX - spec.windowInsetX), spec.windowBottomY },
					Vec2{ (spec.cabFrontX - spec.windowInsetX), spec.windowTopY },
					Vec2{ (spec.cabBackX + spec.windowInsetX), spec.windowTopY } });
				(void)builder.addExtrude(Polygon{ outer, std::move(holes) }, spec.cabWallThickness,
					Quaternion::RotateX(90_deg)).value();
			})).value();

		for (int32 side = 0; side < 2; ++side)
		{
			const double sign = ((side == 0) ? 1.0 : -1.0);
			(void)assembly.addPart({ .name = U"cab_side_{}"_fmt(side), .mesh = sideShape, .material = green, .placement = Vec3{ 0, 0, (sign * (spec.cabHalfWidth - (spec.cabWallThickness * 0.5))) } }).value();
		}

		(void)assembly.addPart({ .name = U"cab_front", .mesh = assembly.addMesh(Mesh3D::Box(Vec3{ spec.cabWallThickness, (spec.cabWallTopY - spec.fireboxTopY),
				(spec.cabHalfWidth * 2.0) })).value(), .material = green, .placement = Vec3{ (spec.cabFrontX - (spec.cabWallThickness * 0.5)),
				((spec.cabWallTopY + spec.fireboxTopY) * 0.5), 0 } }).value();

		(void)assembly.addPart({ .name = U"cab_floor", .mesh = assembly.addMesh(Mesh3D::Box(Vec3{ (spec.cabFrontX - spec.cabBackX), 0.07, (spec.cabHalfWidth * 2.0) })).value(), .material = roofGrey, .placement = Vec3{ ((spec.cabFrontX + spec.cabBackX) * 0.5), (spec.cabFloorY - 0.035), 0 } }).value();

		// Arched roof: the contour is placed with (x, y) -> world (-Z, Y) and extruded along X.
		(void)assembly.addPart({ .name = U"cab_roof", .mesh = assembly.addMesh(BuildMesh([&](Mesh3DBuilder& builder)
			{
				const double halfWidth = (spec.cabHalfWidth + spec.cabRoofOverhang);
				Array<Vec2> contour;
				const int32 samples = 18;

				for (int32 i = 0; i <= samples; ++i)
				{
					const double u = (-halfWidth + ((halfWidth * 2.0 * i) / samples));
					const double t = (u / halfWidth);
					contour << Vec2{ u, (spec.cabRoofRise * (1.0 - (t * t))) };
				}

				for (int32 i = samples; i >= 0; --i)
				{
					const double u = (-halfWidth + ((halfWidth * 2.0 * i) / samples));
					const double t = (u / halfWidth);
					contour << Vec2{ u, ((spec.cabRoofRise * (1.0 - (t * t))) - spec.cabRoofThickness) };
				}

				(void)builder.addExtrude(Polygon{ PositivelyWound(std::move(contour)) },
					((spec.cabFrontX - spec.cabBackX) + (spec.cabRoofOverhang * 2.0)),
					Mesh3DPlacement{ Vec3{ ((spec.cabFrontX + spec.cabBackX) * 0.5), spec.cabWallTopY, 0 },
						(Quaternion::RotateX(90_deg) * Quaternion::RotateY(90_deg)) }).value();
			})).value(), .material = roofGrey }).value();
	}

	// ---- cylinders and valve gear ----

	{
		const auto blockShape = assembly.addMesh(Mesh3D::Box(Vec3{ (spec.cylinderFrontX - spec.cylinderBackX), 0.72, 0.46 })).value();
		const auto coverShape = assembly.addMesh(Mesh3D::Cylinder(0.30, 0.09, 20)).value();
		const auto crossheadShape = assembly.addMesh(Mesh3D::Box(Vec3{ 0.30, 0.26, 0.16 })).value();
		const auto slideBarShape = assembly.addMesh(Mesh3D::Box(Vec3{ 0.90, 0.05, 0.09 })).value();

		for (int32 side = 0; side < 2; ++side)
		{
			const double sign = ((side == 0) ? 1.0 : -1.0);
			const double z = (sign * spec.cylinderHalfGauge);
			const Vec3 cross = spec.crosshead(side);
			const Vec3 pin = spec.crankPin(side, 1);

			(void)assembly.addPart({ .name = U"cylinder_block_{}"_fmt(side), .mesh = blockShape, .material = lined, .placement = Vec3{ ((spec.cylinderFrontX + spec.cylinderBackX) * 0.5), spec.cylinderAxisY, z } }).value();

			for (int32 end = 0; end < 2; ++end)
			{
				const double x = ((end == 0) ? (spec.cylinderFrontX + 0.045) : (spec.cylinderBackX - 0.045));
				(void)assembly.addPart({ .name = U"cylinder_cover_{}_{}"_fmt(side, end), .mesh = coverShape, .material = steel, .placement = Mesh3DPlacement{ Vec3{ x, spec.cylinderAxisY, z }, Quaternion::RotateZ(90_deg) } }).value();
			}

			for (int32 k = 0; k < 2; ++k)
			{
				const double ySign = ((k == 0) ? 1.0 : -1.0);
				(void)assembly.addPart({ .name = U"slide_bar_{}_{}"_fmt(side, k), .mesh = slideBarShape, .material = steel, .placement = Vec3{ (spec.cylinderBackX - 0.45), (spec.cylinderAxisY + (ySign * 0.155)), z } }).value();
			}

			(void)assembly.addPart({ .name = U"crosshead_{}"_fmt(side), .mesh = crossheadShape, .material = steel, .placement = Vec3{ cross.x, cross.y, z } }).value();

			(void)assembly.addPart({ .name = U"piston_rod_{}"_fmt(side), .mesh = assembly.addMesh(Mesh3D::Tube({ Vec3{ cross.x, spec.cylinderAxisY, z },
				Vec3{ (spec.cylinderBackX + 0.10), spec.cylinderAxisY, z } }, 0.045,
				TubeOptions{ .sides = 12 })).value(), .material = steel }).value();

			(void)assembly.addPart({ .name = U"main_rod_{}"_fmt(side), .mesh = assembly.addMesh(Mesh3D::Tube({ Vec3{ cross.x, cross.y, pin.z }, pin }, spec.mainRodRadius,
				TubeOptions{ .sides = 12 })).value(), .material = steel }).value();

			const Vec3 first = spec.crankPin(side, 0);
			const Vec3 last = spec.crankPin(side, 2);
			(void)assembly.addPart({ .name = U"coupling_rod_{}"_fmt(side), .mesh = assembly.addMesh(Mesh3D::Box(Vec3{ (last.x - first.x + 0.34), spec.couplingRodHeight, spec.couplingRodThickness })).value(), .material = steel, .placement = Vec3{ ((first.x + last.x) * 0.5), first.y, (pin.z + (sign * 0.09)) } }).value();
		}
	}

	if (not assembly.saveOBJ(U"models/tank_locomotive.obj"))
	{
		throw Error{ U"Failed to export tank_locomotive" };
	}
}
```

---

## 5. 三檣帆船

二重曲面の船体・ブルワーク・甲板・ワール・帆桁と畳帆・シュラウドとステー・展示用の架台。

使っている主な API: `Loft`（船体・ブルワーク・甲板の凹みまでを 1 つの閉じた U 字断面で生成）、`Tube`（ワール・索・帆桁）。

```cpp
# include <Siv3D.hpp> // Siv3D v0.8.0

using MaterialID = Mesh3DAssembly::MaterialID;

/// @brief Mesh3DBuilder で組んだ複数の形状を 1 つの Mesh3D にまとめます。
template <class Compose>
[[nodiscard]]
Mesh3D BuildMesh(Compose compose)
{
	Mesh3DBuilder builder;
	compose(builder);
	return builder.obtainMesh();
}

/// @brief Reverses a contour when needed so that Polygon and Loft accept it.
[[nodiscard]]
Array<Vec2> PositivelyWound(Array<Vec2> points)
{
	// Siv3D screen coordinates: clockwise means positive signed area.
	// This only fixes winding; it does not repair a self-intersecting contour.
	if (not Geometry2D::IsClockwise(points))
	{
		points.reverse();
	}

	return points;
}

// Descending edges shape the stern; keep this normalized form for both directions.
[[nodiscard]]
double SmoothStep01(double edge0, double edge1, double value) noexcept
{
	const double t = Clamp(((value - edge0) / (edge1 - edge0)), 0.0, 1.0);
	return (t * t * (3.0 - (2.0 * t)));
}

//==========================================================================
//
//	Model 5: three masted ship on a display cradle
//
//	A doubly curved subject. The hull, the bulwarks and the deck come from a
//	single Loft whose closed section is a U with wall thickness, the wales
//	and rigging follow paths sampled from the same section functions, and the
//	ship rests in a cradle cut to the hull it actually produced.
//
//==========================================================================

struct ShipSpec
{
	double hullLength = 26.0;
	double maxHalfBeam = 2.90;
	double minHalfBeamFraction = 0.13;
	int32 stations = 25;
	int32 sideSamples = 11;       // hull outer points per side, keel to deck edge
	double deckMidY = 3.15;
	double sheerStern = 0.75;
	double sheerBow = 1.30;
	double keelRiseStern = 1.35;
	double keelRiseBow = 2.45;
	double keelHalfWidth = 0.14;
	double bulwarkHeight = 0.62;
	double bulwarkThickness = 0.17;
	double bulwarkTumblehome = 0.96;
	double deckRecess = 0.05;
	double waleHeightFraction = 0.74;
	double waleLowerFraction = 0.58;
	double waleRadius = 0.10;
	double capRailRadius = 0.12;
	// Masts, measured from the deck at the mast station.
	double mastX[3] = { 6.40, 0.20, -5.80 };
	double lowerMastHeight[3] = { 9.20, 10.80, 8.20 };
	double topMastHeight[3] = { 6.60, 8.00, 5.40 };
	double mastRadius[3] = { 0.30, 0.34, 0.26 };
	int32 yardsPerMast = 3;
	double yardSpread[3] = { 6.60, 7.60, 5.60 };
	double bowspritLength = 7.00;
	double bowspritRise = 2.20;
	int32 shroudsPerSide = 5;
	double shroudRadius = 0.045;
	double stayRadius = 0.055;
	double cradleBaseTopY = -0.60;
	double cradleBaseThickness = 0.55;
	double cradleX[2] = { -6.00, 6.00 };
	// ---- derived ----

	[[nodiscard]] double sternX() const noexcept { return (-hullLength * 0.5); }
	[[nodiscard]] double bowX() const noexcept { return (hullLength * 0.5); }
	[[nodiscard]] double stationX(int32 i) const noexcept
	{
		return (sternX() + ((hullLength * i) / (stations - 1.0)));
	}
	[[nodiscard]] double stationT(int32 i) const noexcept { return (i / (stations - 1.0)); }
	[[nodiscard]] double parameterAt(double x) const noexcept
	{
		return Clamp(((x - sternX()) / hullLength), 0.0, 1.0);
	}
	/// @brief Half beam of the hull at the deck edge.
	[[nodiscard]] double halfBeam(double t) const noexcept
	{
		const double fullness = (0.46 + (0.54 * std::pow(std::sin(Math::Pi * std::pow(t, 1.22)), 0.55)));
		const double bowTaper = (1.0 - (0.82 * SmoothStep01(0.70, 1.0, t)));
		return (maxHalfBeam * Max((fullness * bowTaper), minHalfBeamFraction));
	}
	/// @brief Height of the keel line: flat amidships, rising toward both ends.
	[[nodiscard]] double keelY(double t) const noexcept
	{
		return ((keelRiseStern * SmoothStep01(0.30, 0.0, t)) + (keelRiseBow * SmoothStep01(0.58, 1.0, t)));
	}
	/// @brief Height of the deck edge: the sheer line.
	[[nodiscard]] double deckY(double t) const noexcept
	{
		return (deckMidY + (sheerStern * SmoothStep01(0.46, 0.0, t)) + (sheerBow * SmoothStep01(0.60, 1.0, t)));
	}
	/// @brief Beam fraction of the hull surface at height fraction p above the keel.
	[[nodiscard]] double sectionWidthFraction(double p) const noexcept
	{
		const double flare = std::pow(std::sin(p * Math::HalfPi), 0.72);
		const double tumblehome = (1.0 - (0.10 * SmoothStep01(0.72, 1.0, p)));
		return (flare * tumblehome);
	}
	/// @brief Outer hull point of a section: contour (u, v) with u across and v up.
	[[nodiscard]] Vec2 hullPoint(double t, double p) const noexcept
	{
		const double b = halfBeam(t);
		const double k = keelY(t);
		const double d = deckY(t);
		const double u = (keelHalfWidth + ((b - keelHalfWidth) * sectionWidthFraction(p)));
		return Vec2{ u, (k + ((d - k) * p)) };
	}
	[[nodiscard]] double capOuterHalfBeam(double t) const noexcept { return (halfBeam(t) * bulwarkTumblehome); }
	[[nodiscard]] double capInnerHalfBeam(double t) const noexcept
	{
		return Max((capOuterHalfBeam(t) - bulwarkThickness), 0.12);
	}
	[[nodiscard]] double capTopY(double t) const noexcept { return (deckY(t) + bulwarkHeight); }
	[[nodiscard]] double mastFootY(int32 m) const noexcept { return (deckY(parameterAt(mastX[m])) - deckRecess); }
	[[nodiscard]] double mastTopY(int32 m) const noexcept
	{
		return (mastFootY(m) + lowerMastHeight[m] + topMastHeight[m]);
	}
	[[nodiscard]] double bowspritTipX() const noexcept { return (bowX() + bowspritLength); }
	[[nodiscard]] double bowspritTipY() const noexcept { return (deckY(1.0) + bowspritRise); }
};

/// @brief Closed hull section: outer shell, bulwark walls, cap rails and the recessed deck.
[[nodiscard]]
Array<Vec2> ShipSection(const ShipSpec& spec, double t)
{
	Array<Vec2> contour;
	contour.reserve((spec.sideSamples + 3) * 2);

	const double capOuter = spec.capOuterHalfBeam(t);
	const double capInner = spec.capInnerHalfBeam(t);
	const double capTop = spec.capTopY(t);
	const double deck = (spec.deckY(t) - spec.deckRecess);

	for (int32 side = 0; side < 2; ++side)
	{
		const double sign = ((side == 0) ? 1.0 : -1.0);

		if (side == 0)
		{
			for (int32 i = 0; i < spec.sideSamples; ++i)
			{
				contour << spec.hullPoint(t, (i / (spec.sideSamples - 1.0)));
			}

			contour << Vec2{ capOuter, capTop };
			contour << Vec2{ capInner, capTop };
			contour << Vec2{ capInner, deck };
		}
		else
		{
			contour << Vec2{ (sign * capInner), deck };
			contour << Vec2{ (sign * capInner), capTop };
			contour << Vec2{ (sign * capOuter), capTop };

			for (int32 i = (spec.sideSamples - 1); i >= 0; --i)
			{
				const Vec2 point = spec.hullPoint(t, (i / (spec.sideSamples - 1.0)));
				contour << Vec2{ -point.x, point.y };
			}
		}
	}

	return PositivelyWound(std::move(contour));
}

void Main()
{
	const ShipSpec spec;

	Mesh3DAssembly assembly;

	const MaterialID planking = assembly.addMaterial(Material{ .name = U"hull_planking", .baseColor = ColorF{ 0.35, 0.24, 0.16 },
		.roughness = 0.85 });
	const MaterialID deckPlank = assembly.addMaterial(Material{ .name = U"deck_planking", .baseColor = ColorF{ 0.68, 0.57, 0.39 },
		.roughness = 0.90 });
	const MaterialID wale = assembly.addMaterial(Material{ .name = U"wale_black", .baseColor = ColorF{ 0.12, 0.11, 0.10 },
		.roughness = 0.75 });
	const MaterialID trim = assembly.addMaterial(Material{ .name = U"gilt_trim", .baseColor = ColorF{ 0.72, 0.55, 0.23 },
		.roughness = 0.45 });
	const MaterialID timber = assembly.addMaterial(Material{ .name = U"spar_timber", .baseColor = ColorF{ 0.46, 0.33, 0.20 },
		.roughness = 0.85 });
	const MaterialID rope = assembly.addMaterial(Material{ .name = U"rigging_rope", .baseColor = ColorF{ 0.27, 0.24, 0.20 },
		.roughness = 0.95 });
	const MaterialID canvas = assembly.addMaterial(Material{ .name = U"furled_canvas", .baseColor = ColorF{ 0.88, 0.86, 0.79 },
		.roughness = 0.95 });
	const MaterialID cradleTimber = assembly.addMaterial(Material{ .name = U"cradle_timber", .baseColor = ColorF{ 0.42, 0.34, 0.26 },
		.roughness = 0.90 });
	const MaterialID plinth = assembly.addMaterial(Material{ .name = U"display_plinth", .baseColor = ColorF{ 0.28, 0.29, 0.31 },
		.roughness = 0.80 });

	// The loft frame: contour +X -> world -Z, contour +Y -> world +Y, sections advance along +X.
	const Quaternion sectionFrame = Quaternion::FromUnitVectorPairs(
		{ Vec3{ 1, 0, 0 }, Vec3{ 0, 0, -1 } }, { Vec3{ 0, 0, -1 }, Vec3{ 0, 1, 0 } });

	// ---- hull ----

	{
		Array<Array<Vec2>> contours;
		contours.reserve(spec.stations);

		for (int32 i = 0; i < spec.stations; ++i)
		{
			contours << ShipSection(spec, spec.stationT(i));
		}

		Array<LoftSection> sections;
		sections.reserve(spec.stations);

		for (int32 i = 0; i < spec.stations; ++i)
		{
			sections << LoftSection{ .points = contours[i],
				.frame = Mesh3DPlacement{ Vec3{ spec.stationX(i), 0, 0 }, sectionFrame } };
		}

		(void)assembly.addPart({ .name = U"hull", .mesh = assembly.addMesh(Mesh3D::Loft(sections, LoftOptions{ .endCaps = Mesh3DEndCaps::Both, .smoothingAngle = 42_deg })).value(), .material = planking }).value();

		// Deck planking: a thin slab lofted on the same stations, recessed into the hull deck.
		Array<Array<Vec2>> deckContours;
		deckContours.reserve(spec.stations);

		for (int32 i = 0; i < spec.stations; ++i)
		{
			const double t = spec.stationT(i);
			const double halfWidth = (spec.capInnerHalfBeam(t) - 0.01);
			const double top = spec.deckY(t);
			deckContours << PositivelyWound({
				Vec2{ -halfWidth, (top - spec.deckRecess) }, Vec2{ halfWidth, (top - spec.deckRecess) },
				Vec2{ halfWidth, top }, Vec2{ -halfWidth, top } });
		}

		Array<LoftSection> deckSections;
		deckSections.reserve(spec.stations);

		for (int32 i = 0; i < spec.stations; ++i)
		{
			deckSections << LoftSection{ .points = deckContours[i],
				.frame = Mesh3DPlacement{ Vec3{ spec.stationX(i), 0, 0 }, sectionFrame } };
		}

		(void)assembly.addPart({ .name = U"deck", .mesh = assembly.addMesh(Mesh3D::Loft(deckSections, LoftOptions{ .endCaps = Mesh3DEndCaps::Both })).value(), .material = deckPlank }).value();
	}

	// ---- wales and cap rails, sampled from the same section functions ----

	{
		struct Strake { String name; double heightFraction; double radius; MaterialID material; };
		const Strake strakes[2] = { { U"wale_lower", spec.waleLowerFraction, spec.waleRadius, wale },
			{ U"wale_upper", spec.waleHeightFraction, spec.waleRadius, wale } };

		for (const auto& strake : strakes)
		{
			for (int32 side = 0; side < 2; ++side)
			{
				const double sign = ((side == 0) ? 1.0 : -1.0);
				Array<Vec3> path;

				for (int32 i = 1; i < (spec.stations - 1); ++i)
				{
					const double t = spec.stationT(i);
					const Vec2 point = spec.hullPoint(t, strake.heightFraction);
					path << Vec3{ spec.stationX(i), point.y, (sign * point.x) };
				}

				(void)assembly.addPart({ .name = U"{}_{}"_fmt(strake.name, side), .mesh = assembly.addMesh(Mesh3D::Tube(path, strake.radius, TubeOptions{ .sides = 10 })).value(), .material = strake.material }).value();
			}
		}

		for (int32 side = 0; side < 2; ++side)
		{
			const double sign = ((side == 0) ? 1.0 : -1.0);
			Array<Vec3> path;

			for (int32 i = 1; i < (spec.stations - 1); ++i)
			{
				const double t = spec.stationT(i);
				const double u = ((spec.capOuterHalfBeam(t) + spec.capInnerHalfBeam(t)) * 0.5);
				path << Vec3{ spec.stationX(i), spec.capTopY(t), (sign * u) };
			}

			(void)assembly.addPart({ .name = U"cap_rail_{}"_fmt(side), .mesh = assembly.addMesh(Mesh3D::Tube(path, spec.capRailRadius, TubeOptions{ .sides = 10 })).value(), .material = trim }).value();
		}
	}

	// ---- deck structures ----

	{
		const double quarterT = spec.parameterAt(-8.20);
		(void)assembly.addPart({ .name = U"quarterdeck_house", .mesh = assembly.addMesh(Mesh3D::Box(Vec3{ 4.40, 1.30, (spec.capInnerHalfBeam(quarterT) * 1.55) })).value(), .material = trim, .placement = Vec3{ -8.20, (spec.deckY(quarterT) + 0.65), 0 } }).value();
		(void)assembly.addPart({ .name = U"quarterdeck_roof", .mesh = assembly.addMesh(Mesh3D::Box(Vec3{ 4.80, 0.14, (spec.capInnerHalfBeam(quarterT) * 1.75) })).value(), .material = deckPlank, .placement = Vec3{ -8.20, (spec.deckY(quarterT) + 1.37), 0 } }).value();

		const double foreT = spec.parameterAt(9.60);
		(void)assembly.addPart({ .name = U"forecastle_deck", .mesh = assembly.addMesh(Mesh3D::Box(Vec3{ 3.60, 0.16, (spec.capInnerHalfBeam(foreT) * 1.85) })).value(), .material = deckPlank, .placement = Vec3{ 9.60, (spec.deckY(foreT) + 0.92), 0 } }).value();
		(void)assembly.addPart({ .name = U"forecastle_bulkhead", .mesh = assembly.addMesh(Mesh3D::Box(Vec3{ 0.16, 0.92, (spec.capInnerHalfBeam(foreT) * 1.85) })).value(), .material = trim, .placement = Vec3{ 7.85, (spec.deckY(foreT) + 0.46), 0 } }).value();

		const auto hatchShape = assembly.addMesh(Mesh3D::Box(Vec3{ 1.50, 0.34, 1.90 })).value();
		const auto grateShape = assembly.addMesh(Mesh3D::Box(Vec3{ 1.28, 0.08, 1.68 })).value();
		const double hatchX[3] = { 3.60, -0.90, -4.60 };

		for (int32 i = 0; i < 3; ++i)
		{
			const double t = spec.parameterAt(hatchX[i]);
			(void)assembly.addPart({ .name = U"hatch_{}"_fmt(i), .mesh = hatchShape, .material = timber, .placement = Vec3{ hatchX[i], (spec.deckY(t) + 0.17), 0 } }).value();
			(void)assembly.addPart({ .name = U"hatch_grate_{}"_fmt(i), .mesh = grateShape, .material = deckPlank, .placement = Vec3{ hatchX[i], (spec.deckY(t) + 0.36), 0 } }).value();
		}

		const double capstanT = spec.parameterAt(1.80);
		(void)assembly.addPart({ .name = U"capstan", .mesh = assembly.addMesh([&]()
			{
				const Array<Vec2> profile{ Vec2{ 0.0, 0.0 }, Vec2{ 0.46, 0.0 }, Vec2{ 0.34, 0.30 },
					Vec2{ 0.30, 0.72 }, Vec2{ 0.42, 0.86 }, Vec2{ 0.42, 0.98 }, Vec2{ 0.0, 0.98 } };
				return Mesh3D::Revolve(profile, RevolveOptions{ .segments = 20, .smoothingAngle = 35_deg });
			}()).value(), .material = timber, .placement = Vec3{ 1.80, spec.deckY(capstanT), 0 } }).value();

		const double wheelT = spec.parameterAt(-6.30);
		(void)assembly.addPart({ .name = U"wheel_pedestal", .mesh = assembly.addMesh(Mesh3D::Box(Vec3{ 0.44, 1.00, 0.90 })).value(), .material = timber, .placement = Vec3{ -6.30, (spec.deckY(wheelT) + 0.50), 0 } }).value();
		(void)assembly.addPart({ .name = U"ships_wheel", .mesh = assembly.addMesh(BuildMesh([&](Mesh3DBuilder& builder)
			{
				(void)builder.addTorus(0.56, 0.055, 24, 8).value();
				(void)builder.addCylinder(0.13, 0.20, 14).value();

				for (int32 k = 0; k < 8; ++k)
				{
					(void)builder.addBox(Vec3{ 0.62, 0.06, 0.06 },
						Mesh3DPlacement{ (Mat4x4::Translate(Float3{ 0.34f, 0.0f, 0.0f })
							* Mat4x4::Rotate(Quaternion::RotateY(static_cast<float>((Math::TwoPi * k) / 8)))) }).value();
				}

			})).value(), .material = timber, .placement = Mesh3DPlacement{ Vec3{ -6.30, (spec.deckY(wheelT) + 1.10), 0 }, Quaternion::RotateX(90_deg) } }).value();
	}

	// ---- masts, yards and furled sails ----

	{
		const auto topShape = assembly.addMesh(Mesh3D::Box(Vec3{ 0.90, 0.12, 1.90 })).value();
		const auto truckShape = assembly.addMesh(Mesh3D::Sphere(0.16, 14, 8)).value();

		for (int32 m = 0; m < 3; ++m)
		{
			const double foot = spec.mastFootY(m);
			const double lowerTop = (foot + spec.lowerMastHeight[m]);
			const double radius = spec.mastRadius[m];
			const String tag = U"mast_{}"_fmt(m);

			(void)assembly.addPart({ .name = tag + U"_lower", .mesh = assembly.addMesh(Mesh3D::ConicalFrustum(radius, (radius * 0.74), spec.lowerMastHeight[m], 16)).value(), .material = timber, .placement = Vec3{ spec.mastX[m], ((foot + lowerTop) * 0.5), 0 } }).value();

			(void)assembly.addPart({ .name = tag + U"_top_platform", .mesh = topShape, .material = timber, .placement = Vec3{ spec.mastX[m], (lowerTop - 0.30), 0 } }).value();

			(void)assembly.addPart({ .name = tag + U"_upper", .mesh = assembly.addMesh(Mesh3D::ConicalFrustum((radius * 0.68), (radius * 0.34), spec.topMastHeight[m], 14)).value(), .material = timber, .placement = Vec3{ spec.mastX[m], (lowerTop + (spec.topMastHeight[m] * 0.5)), 0 } }).value();

			(void)assembly.addPart({ .name = tag + U"_truck", .mesh = truckShape, .material = trim, .placement = Vec3{ spec.mastX[m], spec.mastTopY(m), 0 } }).value();

			for (int32 y = 0; y < spec.yardsPerMast; ++y)
			{
				const double fraction = (0.42 + (0.24 * y));
				const double height = (foot + ((spec.mastTopY(m) - foot) * fraction));
				const double spread = (spec.yardSpread[m] * (1.0 - (0.22 * y)));
				const String yardTag = U"{}_yard_{}"_fmt(tag, y);

				(void)assembly.addPart({ .name = yardTag, .mesh = assembly.addMesh(Mesh3D::Tube({ Vec3{ spec.mastX[m], height, -spread },
					Vec3{ spec.mastX[m], height, 0 }, Vec3{ spec.mastX[m], height, spread } },
					{ 0.055, 0.115, 0.055 }, TubeOptions{ .sides = 10 })).value(), .material = timber }).value();

				(void)assembly.addPart({ .name = yardTag + U"_sail", .mesh = assembly.addMesh(Mesh3D::Tube({ Vec3{ spec.mastX[m], (height - 0.24), (-spread * 0.86) },
					Vec3{ spec.mastX[m], (height - 0.30), 0 },
					Vec3{ spec.mastX[m], (height - 0.24), (spread * 0.86) } },
					{ 0.15, 0.34, 0.15 }, TubeOptions{ .sides = 12 })).value(), .material = canvas }).value();
			}
		}

		(void)assembly.addPart({ .name = U"bowsprit", .mesh = assembly.addMesh(Mesh3D::Tube({ Vec3{ (spec.bowX() - 2.20), (spec.deckY(1.0) - 0.10), 0 },
			Vec3{ spec.bowspritTipX(), spec.bowspritTipY(), 0 } }, { 0.28, 0.11 },
			TubeOptions{ .sides = 14 })).value(), .material = timber }).value();
	}

	// ---- stem, rudder and channels ----

	{
		const double bowT = 1.0;
		(void)assembly.addPart({ .name = U"stem_post", .mesh = assembly.addMesh(Mesh3D::Box(Vec3{ 0.34, (spec.capTopY(bowT) - spec.keelY(bowT)),
				(spec.capOuterHalfBeam(bowT) * 2.0) })).value(), .material = timber, .placement = Vec3{ (spec.bowX() + 0.10), ((spec.capTopY(bowT) + spec.keelY(bowT)) * 0.5), 0 } }).value();

		const double sternT = 0.0;
		(void)assembly.addPart({ .name = U"sternpost", .mesh = assembly.addMesh(Mesh3D::Box(Vec3{ 0.30, (spec.capTopY(sternT) - spec.keelY(sternT)),
				(spec.capOuterHalfBeam(sternT) * 2.0) })).value(), .material = timber, .placement = Vec3{ (spec.sternX() - 0.10), ((spec.capTopY(sternT) + spec.keelY(sternT)) * 0.5), 0 } }).value();

		(void)assembly.addPart({ .name = U"rudder", .mesh = assembly.addMesh(Mesh3D::Box(Vec3{ 0.90, (spec.deckY(sternT) - spec.keelY(sternT) + 0.30), 0.24 })).value(), .material = wale, .placement = Vec3{ (spec.sternX() - 0.55), ((spec.deckY(sternT) + spec.keelY(sternT) - 0.30) * 0.5), 0 } }).value();

		for (int32 m = 0; m < 3; ++m)
		{
			const double anchorFront = (spec.mastX[m] + 1.90);
			const double anchorBack = (spec.mastX[m] - 1.10);
			const double t = spec.parameterAt(spec.mastX[m]);
			const auto channelShape = assembly.addMesh(Mesh3D::Box(Vec3{ ((anchorFront - anchorBack) + 0.40), 0.12, 0.46 })).value();

			for (int32 side = 0; side < 2; ++side)
			{
				const double sign = ((side == 0) ? 1.0 : -1.0);
				(void)assembly.addPart({ .name = U"channel_{}_{}"_fmt(m, side), .mesh = channelShape, .material = trim, .placement = Vec3{ ((anchorFront + anchorBack) * 0.5), (spec.deckY(t) + 0.22),
						(sign * (spec.capOuterHalfBeam(t) + 0.10)) } }).value();
			}
		}
	}

	// ---- rigging ----

	{
		for (int32 m = 0; m < 3; ++m)
		{
			const double anchorFront = (spec.mastX[m] + 1.90);
			const double anchorBack = (spec.mastX[m] - 1.10);

			for (int32 side = 0; side < 2; ++side)
			{
				const double sign = ((side == 0) ? 1.0 : -1.0);

				for (int32 k = 0; k < spec.shroudsPerSide; ++k)
				{
					const double x = (anchorBack + (((anchorFront - anchorBack) * k) / (spec.shroudsPerSide - 1.0)));
					const double t = spec.parameterAt(x);
					const Vec3 foot{ x, spec.capTopY(t), (sign * spec.capOuterHalfBeam(t)) };
					const Vec3 head{ spec.mastX[m], (spec.mastFootY(m) + (spec.lowerMastHeight[m] * 0.94)),
						(sign * 0.34) };
					const String tag = U"shroud_{}_{}_{}"_fmt(m, side, k);

					(void)assembly.addPart({ .name = tag, .mesh = assembly.addMesh(Mesh3D::Tube({ foot, head }, spec.shroudRadius,
						TubeOptions{ .sides = 6 })).value(), .material = rope }).value();
				}
			}
		}

		// Fore and aft stays tie the mastheads together and to the bowsprit.
		struct Stay { String name; Vec3 from; Vec3 to; };
		const Array<Stay> stays{
			{ U"forestay", Vec3{ spec.bowspritTipX(), spec.bowspritTipY(), 0 },
				Vec3{ spec.mastX[0], (spec.mastFootY(0) + spec.lowerMastHeight[0]), 0 } },
			{ U"fore_topmast_stay", Vec3{ (spec.bowspritTipX() - 1.60), (spec.bowspritTipY() - 0.50), 0 },
				Vec3{ spec.mastX[0], (spec.mastTopY(0) - 0.60), 0 } },
			{ U"mainstay", Vec3{ (spec.mastX[0] + 0.10), (spec.mastFootY(0) + 1.20), 0 },
				Vec3{ spec.mastX[1], (spec.mastFootY(1) + spec.lowerMastHeight[1]), 0 } },
			{ U"mizzenstay", Vec3{ (spec.mastX[1] + 0.10), (spec.mastFootY(1) + 1.20), 0 },
				Vec3{ spec.mastX[2], (spec.mastFootY(2) + spec.lowerMastHeight[2]), 0 } },
			{ U"main_backstay", Vec3{ spec.mastX[1], (spec.mastTopY(1) - 0.50), 0 },
				Vec3{ (spec.sternX() + 1.20), spec.capTopY(spec.parameterAt(spec.sternX() + 1.20)), 0 } } };

		for (const auto& stay : stays)
		{
			(void)assembly.addPart({ .name = stay.name, .mesh = assembly.addMesh(Mesh3D::Tube({ stay.from, stay.to }, spec.stayRadius,
				TubeOptions{ .sides = 6 })).value(), .material = rope }).value();
		}
	}

	// ---- display cradle ----

	{
		(void)assembly.addPart({ .name = U"plinth", .mesh = assembly.addMesh(Mesh3D::Box(Vec3{ (spec.hullLength + 4.0), spec.cradleBaseThickness, (spec.maxHalfBeam * 3.0) })).value(), .material = plinth, .placement = Vec3{ 0, (spec.cradleBaseTopY - (spec.cradleBaseThickness * 0.5)), 0 } }).value();

		for (int32 c = 0; c < 2; ++c)
		{
			const double x = spec.cradleX[c];
			const double t = spec.parameterAt(x);
			const double keel = spec.keelY(t);
			const String tag = U"cradle_{}"_fmt(c);

			(void)assembly.addPart({ .name = tag + U"_post", .mesh = assembly.addMesh(Mesh3D::Box(Vec3{ 0.70, (keel - spec.cradleBaseTopY), (spec.keelHalfWidth * 4.0) })).value(), .material = cradleTimber, .placement = Vec3{ x, ((keel + spec.cradleBaseTopY) * 0.5), 0 } }).value();

			// Two chocks that meet the hull surface at a fixed height fraction.
			for (int32 side = 0; side < 2; ++side)
			{
				const double sign = ((side == 0) ? 1.0 : -1.0);
				const Vec2 contact = spec.hullPoint(t, 0.34);
				// The chock ends inside the hull surface so that it beds against the planking.
				const Vec3 foot{ x, spec.cradleBaseTopY, (sign * (contact.x + 0.55)) };
				const Vec3 head{ x, contact.y, (sign * (contact.x - 0.12)) };
				(void)assembly.addPart({ .name = U"{}_chock_{}"_fmt(tag, side), .mesh = assembly.addMesh(Mesh3D::Tube({ foot, head }, 0.19, TubeOptions{ .sides = 8 })).value(), .material = cradleTimber }).value();
			}
		}
	}

	if (not assembly.saveOBJ(U"models/sailing_ship.obj"))
	{
		throw Error{ U"Failed to export sailing_ship" };
	}
}
```

---

## 6. 渾天儀

地平環・子午環・緯度に傾けた極軸環（赤道・回帰線・極圏・分至圏）・黄道環と十二宮・轆轤挽きの台。

使っている主な API: `HollowCylinder`（各環）、`Mesh3DAssembly` の親子関係（極軸フレーム → 黄道フレーム → 十二宮）、`Revolve`（台の轆轤挽き）。

```cpp
# include <Siv3D.hpp> // Siv3D v0.8.0

using MaterialID = Mesh3DAssembly::MaterialID;

//==========================================================================
//
//	Model 6: armillary sphere on a turned stand
//
//	A subject of nested thin rings whose value is in exact angles and real
//	clearances: a fixed horizon and meridian, a polar armature tilted to the
//	design latitude, tropics, polar circles, colures and a tilted ecliptic.
//
//==========================================================================

struct ArmillarySpec
{
	double latitude = 35_deg;
	double obliquity = 23.44_deg;
	double horizonInnerRadius = 3.90;
	double horizonOuterRadius = 4.45;
	double horizonThickness = 0.24;
	double meridianInnerRadius = 3.42;
	double meridianOuterRadius = 3.84;
	double meridianThickness = 0.22;
	int32 meridianMarks = 24;
	double equatorInnerRadius = 2.80;
	double equatorOuterRadius = 3.10;
	double equatorThickness = 0.20;
	double colureInnerRadius = 2.82;
	double colureOuterRadius = 3.08;
	double colureThickness = 0.17;
	double eclipticInnerRadius = 2.88;
	double eclipticOuterRadius = 3.26;
	double eclipticThickness = 0.26;
	int32 zodiacSigns = 12;
	double smallRingHalfWidth = 0.11;
	double smallRingThickness = 0.11;
	double axisRadius = 0.10;
	double axisOverhang = 0.62;
	double globeRadius = 0.58;
	// The meridian ring is a full circle, so the whole stand has to clear its lowest point.
	double baseTopY = -4.30;
	double baseThickness = 0.40;
	double baseHalfSize = 2.60;
	double bossTopY = -3.94;
	double legFootRadius = 1.60;
	double legFootY = -4.02;
	int32 legCount = 4;
	double legAzimuthOffset = 45_deg;
	double legRadius = 0.16;
	double braceY = -3.20;
	double braceInnerRadius = 2.42;
	double braceOuterRadius = 2.62;
	double braceHeight = 0.16;
	// ---- derived ----

	/// @brief Tilt of the polar axis away from the vertical.
	[[nodiscard]] double polarTilt() const noexcept { return (Math::HalfPi - latitude); }
	[[nodiscard]] Quaternion polarRotation() const noexcept
	{
		return Quaternion::RotateZ(static_cast<float>(polarTilt()));
	}
	[[nodiscard]] double equatorRadius() const noexcept
	{
		return ((equatorInnerRadius + equatorOuterRadius) * 0.5);
	}
	[[nodiscard]] double tropicOffset() const noexcept { return (equatorRadius() * std::sin(obliquity)); }
	[[nodiscard]] double tropicRadius() const noexcept { return (equatorRadius() * std::cos(obliquity)); }
	[[nodiscard]] double polarCircleOffset() const noexcept { return (equatorRadius() * std::cos(obliquity)); }
	[[nodiscard]] double polarCircleRadius() const noexcept { return (equatorRadius() * std::sin(obliquity)); }
	[[nodiscard]] double axisHalfLength() const noexcept { return (equatorOuterRadius + axisOverhang); }
	[[nodiscard]] double legRadiusAt(double t) const noexcept
	{
		return (legFootRadius + ((((horizonInnerRadius + horizonOuterRadius) * 0.5) - legFootRadius)
			* (t * t * (3.0 - (2.0 * t)))));
	}
	[[nodiscard]] double legHeightAt(double t) const noexcept
	{
		return (legFootY + ((0.0 - legFootY) * std::pow(t, 1.55)));
	}
};

void Main()
{
	const ArmillarySpec spec;

	Mesh3DAssembly assembly;

	const MaterialID brass = assembly.addMaterial(Material{ .name = U"ring_brass", .baseColor = ColorF{ 0.76, 0.59, 0.25 },
		.roughness = 0.35 });
	const MaterialID bronze = assembly.addMaterial(Material{ .name = U"dark_bronze", .baseColor = ColorF{ 0.34, 0.27, 0.16 },
		.roughness = 0.55 });
	const MaterialID silver = assembly.addMaterial(Material{ .name = U"ecliptic_silver", .baseColor = ColorF{ 0.72, 0.74, 0.76 },
		.roughness = 0.30 });
	const MaterialID gold = assembly.addMaterial(Material{ .name = U"gilt_accent", .baseColor = ColorF{ 0.87, 0.73, 0.34 },
		.roughness = 0.25 });
	const MaterialID ocean = assembly.addMaterial(Material{ .name = U"globe_ocean", .baseColor = ColorF{ 0.18, 0.34, 0.48 },
		.roughness = 0.45 });
	const MaterialID land = assembly.addMaterial(Material{ .name = U"globe_land", .baseColor = ColorF{ 0.30, 0.46, 0.26 },
		.roughness = 0.70 });
	const MaterialID walnut = assembly.addMaterial(Material{ .name = U"stand_walnut", .baseColor = ColorF{ 0.32, 0.21, 0.13 },
		.roughness = 0.80 });
	const MaterialID slate = assembly.addMaterial(Material{ .name = U"base_slate", .baseColor = ColorF{ 0.27, 0.28, 0.30 },
		.roughness = 0.75 });

	// ---- fixed horizon and meridian ----

	(void)assembly.addPart({ .name = U"horizon_ring", .mesh = assembly.addMesh(Mesh3D::HollowCylinder(spec.horizonInnerRadius, spec.horizonOuterRadius, spec.horizonThickness, 96)).value(), .material = brass }).value();

	(void)assembly.addPart({ .name = U"meridian_ring", .mesh = assembly.addMesh(Mesh3D::HollowCylinder(spec.meridianInnerRadius, spec.meridianOuterRadius, spec.meridianThickness, 96)).value(), .material = brass, .placement = Mesh3DPlacement{ Quaternion::RotateX(90_deg) } }).value();

	{
		const auto bracketShape = assembly.addMesh(Mesh3D::Box(Vec3{ ((spec.horizonInnerRadius - spec.meridianInnerRadius) + 0.20),
				(spec.horizonThickness + 0.10), (spec.meridianThickness + 0.14) })).value();

		for (int32 side = 0; side < 2; ++side)
		{
			const double sign = ((side == 0) ? 1.0 : -1.0);
			(void)assembly.addPart({ .name = U"meridian_bracket_{}"_fmt(side), .mesh = bracketShape, .material = gold, .placement = Vec3{ (sign * ((spec.horizonInnerRadius + spec.meridianInnerRadius) * 0.5)), 0, 0 } }).value();
		}

		const auto markShape = assembly.addMesh(Mesh3D::Box(Vec3{ 0.16, 0.05, (spec.meridianThickness + 0.06) })).value();

		for (int32 i = 0; i < spec.meridianMarks; ++i)
		{
			const double angle = ((Math::TwoPi * i) / spec.meridianMarks);
			(void)assembly.addPart({ .name = U"meridian_mark_{:02d}"_fmt(i), .mesh = markShape, .material = gold, .placement = Mesh3DPlacement{ (Mat4x4::Translate(Float3{ static_cast<float>(spec.meridianOuterRadius - 0.08), 0.0f, 0.0f })
					* Mat4x4::Rotate(Quaternion::RotateZ(static_cast<float>(angle)))) } }).value();
		}
	}

	// ---- polar armature ----

	const auto polar = assembly.addPart({ .name = U"polar_armature", .placement = Mesh3DPlacement{ spec.polarRotation() } }).value();

	(void)assembly.addPart({ .name = U"polar_axis", .mesh = assembly.addMesh(Mesh3D::Cylinder(spec.axisRadius, (spec.axisHalfLength() * 2.0), 20)).value(), .material = bronze, .parent = polar }).value();

	const auto finialShape = assembly.addMesh([&]()
		{
			const Array<Vec2> profile{ Vec2{ 0.0, 0.0 }, Vec2{ 0.20, 0.05 }, Vec2{ 0.16, 0.16 },
				Vec2{ 0.22, 0.26 }, Vec2{ 0.13, 0.40 }, Vec2{ 0.0, 0.50 } };
			return Mesh3D::Revolve(profile, RevolveOptions{ .segments = 20, .smoothingAngle = 40_deg });
		}()).value();

	for (int32 end = 0; end < 2; ++end)
	{
		const double sign = ((end == 0) ? 1.0 : -1.0);
		(void)assembly.addPart({ .name = U"axis_finial_{}"_fmt(end), .mesh = finialShape, .material = gold, .parent = polar, .placement = Mesh3DPlacement{ Vec3{ 0, (sign * spec.axisHalfLength()), 0 },
				((end == 0) ? Quaternion::Identity() : Quaternion::RotateX(180_deg)) } }).value();
	}

	(void)assembly.addPart({ .name = U"equator_ring", .mesh = assembly.addMesh(Mesh3D::HollowCylinder(spec.equatorInnerRadius, spec.equatorOuterRadius, spec.equatorThickness, 80)).value(), .material = brass, .parent = polar }).value();

	// Tropics and polar circles, placed from the obliquity rather than by hand.
	struct SmallRing { String name; double offset; double radius; };
	const SmallRing smallRings[4] = {
		{ U"tropic_cancer", spec.tropicOffset(), spec.tropicRadius() },
		{ U"tropic_capricorn", -spec.tropicOffset(), spec.tropicRadius() },
		{ U"arctic_circle", spec.polarCircleOffset(), spec.polarCircleRadius() },
		{ U"antarctic_circle", -spec.polarCircleOffset(), spec.polarCircleRadius() } };

	for (const auto& ring : smallRings)
	{
		(void)assembly.addPart({ .name = ring.name, .mesh = assembly.addMesh(Mesh3D::HollowCylinder((ring.radius - spec.smallRingHalfWidth), (ring.radius + spec.smallRingHalfWidth),
				spec.smallRingThickness, 64)).value(), .material = silver, .parent = polar, .placement = Vec3{ 0, ring.offset, 0 } }).value();
	}

	// Colures: two great circles through the poles, at right angles to each other.
	const auto colureShape = assembly.addMesh(Mesh3D::HollowCylinder(spec.colureInnerRadius, spec.colureOuterRadius, spec.colureThickness, 80)).value();
	(void)assembly.addPart({ .name = U"equinoctial_colure", .mesh = colureShape, .material = bronze, .parent = polar, .placement = Mesh3DPlacement{ Quaternion::RotateX(90_deg) } }).value();
	(void)assembly.addPart({ .name = U"solstitial_colure", .mesh = colureShape, .material = bronze, .parent = polar, .placement = Mesh3DPlacement{ Quaternion::RotateZ(90_deg) } }).value();

	// Ecliptic: tilted from the equator by the obliquity, with the twelve signs marked.
	const auto ecliptic = assembly.addPart({ .name = U"ecliptic_frame", .parent = polar, .placement = Mesh3DPlacement{ Quaternion::RotateX(static_cast<float>(spec.obliquity)) } }).value();

	(void)assembly.addPart({ .name = U"ecliptic_ring", .mesh = assembly.addMesh(Mesh3D::HollowCylinder(spec.eclipticInnerRadius, spec.eclipticOuterRadius,
			spec.eclipticThickness, 80)).value(), .material = silver, .parent = ecliptic }).value();

	const auto signShape = assembly.addMesh(Mesh3D::Box(Vec3{ 0.20, (spec.eclipticThickness + 0.08), 0.26 })).value();

	for (int32 i = 0; i < spec.zodiacSigns; ++i)
	{
		const double angle = ((Math::TwoPi * i) / spec.zodiacSigns);
		(void)assembly.addPart({ .name = U"zodiac_{:02d}"_fmt(i), .mesh = signShape, .material = gold, .parent = ecliptic, .placement = Mesh3DPlacement{ (Mat4x4::Translate(Float3{ static_cast<float>(spec.eclipticOuterRadius - 0.06), 0.0f, 0.0f })
				* Mat4x4::Rotate(Quaternion::RotateY(static_cast<float>(angle)))) } }).value();
	}

	// ---- globe at the centre ----

	{
		(void)assembly.addPart({ .name = U"globe", .mesh = assembly.addMesh(Mesh3D::Sphere(spec.globeRadius, 40, 20)).value(), .material = ocean }).value();

		const auto landShape = assembly.addMesh(Mesh3D::IcoSphere(1.0, 2)).value();

		// Each land mass is a lens flattened along its own radius, so it stays on the surface.
		struct Continent { double azimuth; double elevation; double spread; double relief; };
		const Continent continents[5] = { { 20_deg, 25_deg, 0.27, 0.12 }, { 100_deg, -12_deg, 0.24, 0.11 },
			{ 190_deg, 32_deg, 0.22, 0.10 }, { 250_deg, -30_deg, 0.20, 0.10 }, { 320_deg, 5_deg, 0.18, 0.12 } };

		for (int32 i = 0; i < 5; ++i)
		{
			const Continent& c = continents[i];
			const Vec3 direction{ (std::cos(c.elevation) * std::cos(c.azimuth)), std::sin(c.elevation),
				(-std::cos(c.elevation) * std::sin(c.azimuth)) };
			const Quaternion align = Quaternion::FromUnitVectors(Vec3{ 0, 1, 0 }, direction);
			(void)assembly.addPart({ .name = U"continent_{}"_fmt(i), .mesh = landShape, .material = land, .placement = Mesh3DPlacement{ (Mat4x4::Scale(Float3{ static_cast<float>(c.spread),
						static_cast<float>(c.relief), static_cast<float>(c.spread * 0.8) })
					* Mat4x4::Rotate(align)
					* Mat4x4::Translate(Float3{ direction * static_cast<float>(spec.globeRadius * 0.86) })) } }).value();
		}
	}

	// ---- stand ----

	{
		(void)assembly.addPart({ .name = U"base_plinth", .mesh = assembly.addMesh(Mesh3D::ChamferedBox(Vec3{ (spec.baseHalfSize * 2.0), spec.baseThickness, (spec.baseHalfSize * 2.0) }, 0.09)).value(), .material = slate, .placement = Vec3{ 0, (spec.baseTopY - (spec.baseThickness * 0.5)), 0 } }).value();

		(void)assembly.addPart({ .name = U"base_moulding", .mesh = assembly.addMesh(Mesh3D::RectangularFrustum(SizeF{ (spec.baseHalfSize * 1.60), (spec.baseHalfSize * 1.60) },
				SizeF{ (spec.baseHalfSize * 1.05), (spec.baseHalfSize * 1.05) }, 0.28)).value(), .material = walnut, .placement = Vec3{ 0, (spec.baseTopY + 0.14), 0 } }).value();

		// A turned central boss, kept below the lowest point of the meridian ring.
		(void)assembly.addPart({ .name = U"centre_boss", .mesh = assembly.addMesh([&]()
			{
				const double bottom = (spec.baseTopY + 0.24);
				const double span = (spec.bossTopY - bottom);
				const Array<Vec2> profile{ Vec2{ 0.0, bottom }, Vec2{ 0.78, bottom },
					Vec2{ 0.70, (bottom + (span * 0.22)) }, Vec2{ 0.48, (bottom + (span * 0.46)) },
					Vec2{ 0.58, (bottom + (span * 0.72)) }, Vec2{ 0.42, spec.bossTopY },
					Vec2{ 0.0, spec.bossTopY } };
				return Mesh3D::Revolve(profile, RevolveOptions{ .segments = 40, .smoothingAngle = 35_deg });
			}()).value(), .material = walnut }).value();

		// Legs sit at 45 degrees so that none of them lies in the meridian ring plane.
		for (int32 i = 0; i < spec.legCount; ++i)
		{
			const double angle = (spec.legAzimuthOffset + ((Math::TwoPi * i) / spec.legCount));
			Array<Vec3> path;
			const int32 samples = 12;

			for (int32 k = 0; k <= samples; ++k)
			{
				const double t = (k / static_cast<double>(samples));
				path << Cylindrical{ spec.legRadiusAt(t), angle, spec.legHeightAt(t) }.toVec3();
			}

			(void)assembly.addPart({ .name = U"stand_leg_{}"_fmt(i), .mesh = assembly.addMesh(Mesh3D::Tube(path, spec.legRadius, TubeOptions{ .sides = 12 })).value(), .material = walnut }).value();
		}

		(void)assembly.addPart({ .name = U"stand_brace", .mesh = assembly.addMesh(Mesh3D::HollowCylinder(spec.braceInnerRadius, spec.braceOuterRadius, spec.braceHeight, 48)).value(), .material = walnut, .placement = Vec3{ 0, spec.braceY, 0 } }).value();
	}

	if (not assembly.saveOBJ(U"models/armillary_sphere.obj"))
	{
		throw Error{ U"Failed to export armillary_sphere" };
	}
}
```
