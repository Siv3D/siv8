#!/usr/bin/env python3
"""Check registration of Test/Test_*.cpp in both platform test projects.

Engine sources, platform entry points, helpers, and manual tests are out of scope.
Xcode input is parsed by plutil; only the Siv3D-Test target's Sources phases count.
"""

import argparse
from collections import Counter
from copy import deepcopy
import json
from pathlib import Path
import subprocess
import tempfile
import xml.etree.ElementTree as ET


def is_shared_test(path, root):
    return path.parent == root / "Test" and path.match("Test_*.cpp")


def compare(label, expected, entries):
    counts = Counter(entries)
    errors = [f"{label}: missing {p.name}" for p in sorted(expected - counts.keys())]
    errors += [f"{label}: stale {p.name}" for p in sorted(counts.keys() - expected)]
    errors += [f"{label}: duplicate {p.name}" for p, count in sorted(counts.items()) if count > 1]
    return errors


def windows_entries(document, project_dir, root):
    entries, errors = [], []

    def visit(element, conditional=False):
        conditional |= bool(element.get("Condition"))
        if element.tag.rsplit("}", 1)[-1] == "ClCompile" and element.get("Include"):
            path = (project_dir / element.get("Include").replace("\\", "/")).resolve()
            if is_shared_test(path, root):
                entries.append(path)
                excluded = any(child.tag.rsplit("}", 1)[-1] == "ExcludedFromBuild"
                               and (child.text or "").strip().lower() != "false"
                               for child in element)
                if conditional or excluded:
                    errors.append(f"conditional or excluded shared test: {path.name}")
        for child in element:
            visit(child, conditional)

    visit(document)
    return entries, errors


def xcode_entries(project, project_dir, root):
    objects = project["objects"]
    paths = {}

    def visit(key, parent):
        obj = objects[key]
        tree = obj.get("sourceTree", "<group>")
        if tree not in {"<group>", "SOURCE_ROOT", "<absolute>"}:
            return
        base = project_dir if tree == "SOURCE_ROOT" else parent
        path = (base / obj.get("path", "")).resolve()
        if obj["isa"] == "PBXFileReference":
            paths[key] = path
        elif obj["isa"] == "PBXGroup":
            for child in obj.get("children", []):
                visit(child, path)

    visit(objects[project["rootObject"]]["mainGroup"], project_dir)
    targets = [obj for obj in objects.values()
               if obj.get("isa") == "PBXNativeTarget" and obj.get("name") == "Siv3D-Test"]
    if len(targets) != 1:
        raise ValueError("expected exactly one Siv3D-Test target")
    entries, errors = [], []
    for phase_id in targets[0]["buildPhases"]:
        phase = objects[phase_id]
        if phase["isa"] != "PBXSourcesBuildPhase":
            continue
        for build_id in phase.get("files", []):
            build = objects[build_id]
            path = paths.get(build.get("fileRef"))
            if path is not None and is_shared_test(path, root):
                entries.append(path)
                if build.get("platformFilter") or build.get("platformFilters"):
                    errors.append(f"platform-filtered shared test: {path.name}")
    return entries, errors


def check(root, xcode, windows, filters):
    root = root.resolve()
    expected = {p.resolve() for p in (root / "Test").glob("Test_*.cpp") if p.is_file()}
    if not expected:
        return ["No Test/Test_*.cpp files found; check the repository root."]
    errors = []
    for label, (entries, details) in (
        ("Xcode Siv3D-Test Sources", xcode_entries(xcode, root / "macOS", root)),
        ("Siv3D-Test.vcxproj", windows_entries(windows, root / "WindowsDesktop", root)),
        ("Siv3D-Test.vcxproj.filters", windows_entries(filters, root / "WindowsDesktop", root)),
    ):
        errors.extend(compare(label, expected, entries))
        errors.extend(f"{label}: {detail}" for detail in details)
    return errors


def self_test():
    # Small synthetic projects exercise registration, not platform compilation.
    with tempfile.TemporaryDirectory(prefix="siv8-project-check-") as directory:
        root = Path(directory).resolve()
        (root / "Test").mkdir()
        (root / "Test/Test_Example.cpp").touch()
        (root / "Siv3D/src").mkdir(parents=True)
        (root / "Siv3D/src/Test_PlatformOnly.cpp").touch()
        project = {"rootObject": "project", "objects": {
            "project": {"mainGroup": "main"},
            "main": {"isa": "PBXGroup", "children": ["tests", "engine"]},
            "tests": {"isa": "PBXGroup", "path": "../Test", "children": ["test"]},
            "test": {"isa": "PBXFileReference", "path": "Test_Example.cpp"},
            "engine": {"isa": "PBXFileReference", "path": "../Siv3D/src/Test_PlatformOnly.cpp"},
            "build": {"fileRef": "test"},
            "engine-build": {"fileRef": "engine"},
            "sources": {"isa": "PBXSourcesBuildPhase", "files": ["build"]},
            "other-sources": {"isa": "PBXSourcesBuildPhase", "files": ["engine-build"]},
            "target": {"isa": "PBXNativeTarget", "name": "Siv3D-Test", "buildPhases": ["sources"]},
            "other": {"isa": "PBXNativeTarget", "name": "Siv3D", "buildPhases": ["other-sources"]},
        }}
        xml = ET.fromstring('<Project xmlns="http://schemas.microsoft.com/developer/msbuild/2003">'
                            '<ItemGroup><ClCompile Include="../Test/Test_Example.cpp" /></ItemGroup></Project>')
        assert check(root, project, xml, xml) == []  # Unregistered engine file is allowed.

        missing = deepcopy(project)
        missing["objects"]["sources"]["files"] = []
        missing["objects"]["other-sources"]["files"].append("build")
        assert any("missing Test_Example.cpp" in e for e in check(root, missing, xml, xml))

        duplicate = deepcopy(project)
        duplicate["objects"]["sources"]["files"].append("build")
        assert any("duplicate" in e for e in check(root, duplicate, xml, xml))

        filtered = deepcopy(project)
        filtered["objects"]["build"]["platformFilter"] = "ios"
        assert any("platform-filtered" in e for e in check(root, filtered, xml, xml))

        wrong_path = deepcopy(project)
        wrong_path["objects"]["tests"]["path"] = "../Siv3D/src"
        assert any("missing" in e for e in check(root, wrong_path, xml, xml))

        empty = ET.fromstring('<Project/>')
        assert any("vcxproj: missing" in e for e in check(root, project, empty, xml))
        assert any("filters: missing" in e for e in check(root, project, xml, empty))
        repeated = ET.fromstring('<Project><ClCompile Include="../Test/Test_Example.cpp"/>'
                                '<ClCompile Include="..\\Test\\Test_Example.cpp"/></Project>')
        assert any("duplicate" in e for e in check(root, project, repeated, xml))
        excluded = ET.fromstring('<Project><ClCompile Include="../Test/Test_Example.cpp">'
                                '<ExcludedFromBuild Condition="Debug">true</ExcludedFromBuild>'
                                '</ClCompile></Project>')
        assert any("excluded" in e for e in check(root, project, excluded, xml))
        conditional = ET.fromstring('<Project><ItemGroup Condition="Debug">'
                                   '<ClCompile Include="../Test/Test_Example.cpp"/></ItemGroup></Project>')
        assert any("conditional" in e for e in check(root, project, conditional, xml))

        with_engine = ET.fromstring('<Project><ClCompile Include="../Test/Test_Example.cpp"/>'
                                   '<ClCompile Include="../Siv3D/src/Test_PlatformOnly.cpp">'
                                   '<ExcludedFromBuild>true</ExcludedFromBuild></ClCompile></Project>')
        project["objects"]["sources"]["files"].append("engine-build")
        assert check(root, project, with_engine, xml) == []  # Even engine exclusions are ignored.

        (root / "Test/Test_New.cpp").touch()
        assert sum("missing Test_New.cpp" in e for e in check(root, project, xml, xml)) == 3
        (root / "Test/Test_Example.cpp").unlink()
        assert sum("stale Test_Example.cpp" in e for e in check(root, project, xml, xml)) == 3
    print("Test-project checker self-tests passed.")


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--self-test", action="store_true")
    args = parser.parse_args()
    if args.self_test:
        self_test()
        return 0
    root = Path(__file__).resolve().parents[1]
    try:
        result = subprocess.run(["plutil", "-convert", "json", "-o", "-",
                                 str(root / "macOS/OpenSiv3D.xcodeproj/project.pbxproj")],
                                check=True, capture_output=True, text=True)
        errors = check(root, json.loads(result.stdout),
                       ET.parse(root / "WindowsDesktop/Siv3D-Test.vcxproj").getroot(),
                       ET.parse(root / "WindowsDesktop/Siv3D-Test.vcxproj.filters").getroot())
    except (OSError, subprocess.CalledProcessError, ValueError, KeyError, ET.ParseError) as error:
        print(f"Could not check test registration: {error}")
        return 1
    if errors:
        print("\n".join(errors))
        return 1
    print("Shared test registration OK (Test/Test_*.cpp only; engine sources excluded).")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
