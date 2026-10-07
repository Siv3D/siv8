#!/usr/bin/env python3
"""Exercise the Windows runner in a disposable checkout with a tiny fake test app.

Requires Windows, Python, PowerShell 7 and the Windows .NET Framework C# compiler.
Does not build or launch Siv3D, change the real checkout, or retain test artifacts.
"""

import os
from pathlib import Path
import shutil
import subprocess
import tempfile
import time
import unittest


APP = r"""
using System;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Threading;
class App {
    static int Main(string[] args) {
        string mode = args.FirstOrDefault(a => a.StartsWith("--runner-fixture=")) ?? "";
        if (mode.EndsWith("child")) { Thread.Sleep(60000); return 0; }
        File.WriteAllLines("arguments.txt", args);
        string output = args.Single(a => a.StartsWith("console::out=")).Substring(13);
        string xmlOutput = args.Single(a => a.StartsWith("xml::out=")).Substring(9);
        if (new[] { output, xmlOutput }.Any(p => Path.IsPathRooted(p) || p.Any(c => c > 127))) return 90;
        if (!args.Contains("--test-only") || args.Count(a => a == "--reporter") != 2) return 91;
        if (mode.EndsWith("hang")) {
            var child = Process.Start(new ProcessStartInfo(
                Process.GetCurrentProcess().MainModule.FileName, "--runner-fixture=child") {
                UseShellExecute = false, CreateNoWindow = true
            });
            File.WriteAllText("child.pid", child.Id.ToString());
            Thread.Sleep(60000);
        }
        if (mode.EndsWith("missing")) return 0;
        File.WriteAllText(output, "Catch2 console report fixture\n");
        if (mode.EndsWith("incomplete")) {
            File.WriteAllText(xmlOutput, "<Catch2TestRun>");
            return 0;
        }
        bool failed = mode.EndsWith("failure") || mode.EndsWith("false-success");
        string passed = mode.EndsWith("zero") || mode.EndsWith("skipped") || failed ? "0" : "1";
        string totals = " successes=\"" + passed + "\" failures=\"" + (failed ? "1" : "0") +
            "\" expectedFailures=\"0\" skips=\"" + (mode.EndsWith("skipped") ? "1" : "0") + "\"";
        File.WriteAllText(xmlOutput, "<Catch2TestRun><TestCase><StdOut>fixture stdout \u65e5\u672c\u8a9e</StdOut>" +
            "<StdErr>fixture stderr</StdErr></TestCase><OverallResults" + totals +
            "/><OverallResultsCases" + totals + "/></Catch2TestRun>");
        return mode.EndsWith("failure") || mode.EndsWith("bad-exit") ? 3 : 0;
    }
}
"""


@unittest.skipUnless(os.name == "nt", "Windows runner requires Windows")
class RunnerTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.pwsh = shutil.which("pwsh")
        compiler = Path(os.environ["WINDIR"]) / "Microsoft.NET/Framework64/v4.0.30319/csc.exe"
        if not cls.pwsh or not compiler.is_file():
            raise RuntimeError("PowerShell 7 and the Windows .NET Framework C# compiler are required")
        cls.temp = tempfile.TemporaryDirectory(prefix="siv3d-runner-")
        cls.addClassCleanup(cls.temp.cleanup)
        cls.root = Path(cls.temp.name) / "checkout with spaces 日本語"
        platform = cls.root / "WindowsDesktop"
        cls.app = platform / "App"
        cls.app.mkdir(parents=True)
        (cls.root / "tools").mkdir()
        (cls.root / "Test").mkdir()
        (cls.root / "Test/Test_Fixture.cpp").touch()
        repo = Path(__file__).resolve().parents[1]
        cls.runner = platform / "run-tests.ps1"
        shutil.copyfile(repo / "WindowsDesktop/run-tests.ps1", cls.runner)
        shutil.copyfile(repo / "tools/check_test_projects.py", cls.root / "tools/check_test_projects.py")
        project = '<Project><ItemGroup><ClCompile Include="../Test/Test_Fixture.cpp" /></ItemGroup></Project>'
        for name in ("Siv3D-Test.vcxproj", "Siv3D-Test.vcxproj.filters"):
            (platform / name).write_text(project, encoding="utf-8")
        source = cls.root / "app.cs"
        source.write_text(APP, encoding="utf-8")
        executable = cls.app / "Siv3D-Test.exe"
        subprocess.run([str(compiler), "/nologo", "/target:exe", f"/out:{executable}", str(source)], check=True)
        shutil.copyfile(executable, cls.app / "Siv3D-Test(debug).exe")

    def command(self, *arguments):
        return [self.pwsh, "-NoProfile", "-File", str(self.runner), "-SkipBuild", *arguments]

    def run_case(self, *arguments, error=None):
        result = subprocess.run(self.command(*arguments), cwd=self.temp.name,
                                capture_output=True, text=True, encoding="utf-8", timeout=20)
        output = result.stdout + result.stderr
        if error is None:
            self.assertEqual(result.returncode, 0, output)
        else:
            self.assertNotEqual(result.returncode, 0, output)
            self.assertIn(error, output)
        return output

    def test_success_and_argument_boundaries(self):
        output = self.run_case("-Configuration", "Debug", "-TestArguments", '--test-case=*spaces 日本語 "quoted"*')
        args = (self.app / "arguments.txt").read_text(encoding="utf-8-sig").splitlines()
        self.assertIn('--test-case=*spaces 日本語 "quoted"*', args)
        self.assertTrue(any("Debug-" in arg for arg in args if arg.startswith("console::out=")))
        report_path = next(arg[len("console::out="):] for arg in args if arg.startswith("console::out="))
        report = (self.app / report_path).read_text(encoding="utf-8")
        for message in ("fixture stdout 日本語", "fixture stderr"):
            self.assertIn(message, output)
            self.assertIn(message, report)

    def test_incomplete_and_failed_runs(self):
        for mode, error in [("missing", "did not complete"), ("incomplete", "did not complete"),
                            ("zero", "No test cases ran"), ("skipped", "No test cases ran"),
                            ("false-success", "failed in the XML report"), ("failure", "exit 3"), ("bad-exit", "exit 3")]:
            with self.subTest(mode=mode):
                self.run_case("-TestArguments", f"--runner-fixture={mode}", error=error)
        self.run_case()  # All failure paths must release the checkout lock.

    def test_reserved_options(self):
        for argument in ("--out=elsewhere.txt", "--reporter=xml", "--colour-mode=ansi", "-rxml", "-srxml",
                         "--list-tests", "--list-tags", "--help", "--libidentify", "--allow-running-no-tests", "--"):
            with self.subTest(argument=argument):
                self.run_case("-TestArguments", argument, error="is reserved")

    def test_preflight_failure_releases_lock(self):
        project = self.root / "WindowsDesktop/Siv3D-Test.vcxproj"
        original = project.read_bytes()
        try:
            project.write_text("<Project/>", encoding="utf-8")
            self.run_case(error="test registration failed")
        finally:
            project.write_bytes(original)
        executable = self.app / "Siv3D-Test.exe"
        unavailable = executable.with_suffix(".unavailable")
        executable.rename(unavailable)
        try:
            self.run_case(error="Test executable not found")
        finally:
            unavailable.rename(executable)
        self.run_case()

    def test_timeout_and_concurrent_run(self):
        pid_file = self.app / "child.pid"
        with subprocess.Popen(self.command("-TimeoutSeconds", "5", "-TestArguments", "--runner-fixture=hang"),
                              cwd=self.temp.name, stdout=subprocess.PIPE, stderr=subprocess.PIPE,
                              text=True, encoding="utf-8") as running:
            try:
                deadline = time.monotonic() + 10
                while not pid_file.exists() and running.poll() is None and time.monotonic() < deadline:
                    time.sleep(0.05)
                self.assertTrue(pid_file.exists(), "Fake test app did not start")
                self.run_case(error="Another runner may be active")
                stdout, stderr = running.communicate(timeout=15)
                self.assertNotEqual(running.returncode, 0, stdout + stderr)
                self.assertIn("exceeded 5 seconds", stdout + stderr)
            finally:
                if running.poll() is None:
                    # Stop the disposable process tree even if an assertion fails.
                    subprocess.run(["taskkill", "/PID", str(running.pid), "/T", "/F"],
                                   capture_output=True, check=False)
        child_pid = int(pid_file.read_text())
        result = subprocess.run([self.pwsh, "-NoProfile", "-Command",
                                 f"if (Get-Process -Id {child_pid} -ErrorAction SilentlyContinue) {{ exit 1 }}"],
                                timeout=10)
        self.assertEqual(result.returncode, 0, "Timed-out app's child process was left running")
        self.run_case()  # A leftover lock file must not prevent reuse.


if __name__ == "__main__":
    unittest.main(verbosity=2)
