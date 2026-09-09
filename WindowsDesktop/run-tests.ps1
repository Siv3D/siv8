#Requires -Version 7.0

<#
.SYNOPSIS
Build and run the Windows x64 automated tests from any directory.
.EXAMPLE
./WindowsDesktop/run-tests.ps1
.EXAMPLE
./WindowsDesktop/run-tests.ps1 -SkipBuild -TestArguments '--test-case=*BinaryFileReader*', '--duration'
.PARAMETER Configuration
Build configuration (Release by default).
.PARAMETER Jobs
Maximum MSBuild nodes and compiler processes per project (default: 4).
.PARAMETER SkipBuild
Run the existing executable; the caller must ensure it matches current sources.
.PARAMETER TestArguments
Doctest execution arguments, one per array element. The runner owns reporting
options and requires at least one executed test; query-only flags are rejected.
.PARAMETER TimeoutSeconds
Maximum test application runtime, excluding the build. Default: 600 seconds.
#>
[CmdletBinding()]
param(
    [ValidateSet('Debug', 'Release')][string]$Configuration = 'Release',
    [ValidateRange(1, 64)][int]$Jobs = 4,
    [string[]]$TestArguments = @(),
    [switch]$SkipBuild,
    [ValidateRange(1, 86400)][int]$TimeoutSeconds = 600
)
$ErrorActionPreference = 'Stop'
# Handle native exit codes explicitly, including in shells that opt into native errors.
$PSNativeCommandUseErrorActionPreference = $false
if (-not $IsWindows) { throw 'This runner requires Windows.' }

# These options can suppress, redirect or replace the completion report, or skip execution.
# doctest counts matching cases before applying first/last, so ranges could falsely
# report a nonzero count without running a test. Use named filters instead.
$reserved = @('out', 'o', 'reporters', 'r', 'quiet', 'q', 'minimal', 'm',
    'no-colors', 'nc', 'force-colors', 'fc', 'no-exitcode', 'ne', 'no-run', 'nr',
    'help', 'h', '?', 'version', 'v', 'count', 'c', 'list-test-cases', 'ltc',
    'list-test-suites', 'lts', 'list-reporters', 'lr', 'first', 'f', 'last', 'l')
foreach ($argument in $TestArguments) {
    if ($argument -cmatch '^-+(?:dt-)?([^=]+)(?:=.*)?$' -and $reserved -ccontains $Matches[1]) {
        throw "Doctest option '$argument' is reserved: this runner requires its console report and actual test execution."
    }
}

$repo = (Resolve-Path (Join-Path $PSScriptRoot '..')).Path
$reports = Join-Path $PSScriptRoot 'Intermediate/TestReports'
New-Item -ItemType Directory -Force -Path $reports | Out-Null
# Hold an OS file lock across the build and run: Test/output and App are shared.
# The empty file may remain; closing the handle releases the lock even after a crash.
try {
    $runLock = [IO.File]::Open((Join-Path $reports 'runner.lock'),
        [IO.FileMode]::OpenOrCreate, [IO.FileAccess]::ReadWrite, [IO.FileShare]::None)
} catch [IO.IOException] {
    throw "Cannot acquire the test runner lock. Another runner may be active in this checkout. $($_.Exception.Message)"
}
try {
    & python "$repo/tools/check_test_projects.py" --windows-only
    if ($LASTEXITCODE -ne 0) { throw 'Visual Studio test registration failed.' }
    if (-not $SkipBuild) {
        $vswhere = "${env:ProgramFiles(x86)}/Microsoft Visual Studio/Installer/vswhere.exe"
        if (-not (Test-Path -LiteralPath $vswhere -PathType Leaf)) {
            throw 'Visual Studio Installer (vswhere.exe) was not found. Install Visual Studio C++ tools.'
        }
        $vs = & $vswhere -latest -products '*' -requires Microsoft.Component.MSBuild Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
        if ($LASTEXITCODE -ne 0 -or -not $vs) { throw 'MSBuild and MSVC x64 tools were not found.' }
        & "$vs/MSBuild/Current/Bin/MSBuild.exe" "$PSScriptRoot/OpenSiv3D.slnx" /nologo /v:minimal "/m:$Jobs" "/p:CL_MPCount=$Jobs" "/p:Configuration=$Configuration" /p:Platform=x64
        if ($LASTEXITCODE -ne 0) { throw 'Windows build failed.' }
    }
    $name = if ($Configuration -eq 'Debug') { 'Siv3D-Test(debug).exe' } else { 'Siv3D-Test.exe' }
    $exe = Join-Path $PSScriptRoot "App/$name"
    if (-not (Test-Path -LiteralPath $exe -PathType Leaf)) { throw "Test executable not found: $exe" }
    $reportName = "{0}-{1}.txt" -f $Configuration, (Get-Date -Format 'yyyyMMdd-HHmmss-fffffff')
    $report = Join-Path $reports $reportName
    Write-Host "Test report: $report"
    $info = [Diagnostics.ProcessStartInfo]::new()
    $info.FileName = $exe
    $info.WorkingDirectory = Join-Path $PSScriptRoot 'App'
    $info.UseShellExecute = $false
    $info.CreateNoWindow = $true
    $info.WindowStyle = [Diagnostics.ProcessWindowStyle]::Hidden
    # doctest opens its output through a narrow filename on Windows. Keep this
    # path ASCII and relative to App, even when the checkout path contains Unicode.
    foreach ($argument in (@('--test-only', '--reporters=console', '--no-colors',
        "--out=../Intermediate/TestReports/$reportName") + $TestArguments)) {
        $info.ArgumentList.Add($argument)
    }
    $process = [Diagnostics.Process]::Start($info)
    $timer = [Diagnostics.Stopwatch]::StartNew()
    $timedOut = $false
    try {
        # Poll so PowerShell can handle Ctrl+C and run the cleanup below.
        while (-not $process.WaitForExit(200)) {
            if ($timer.Elapsed.TotalSeconds -ge $TimeoutSeconds) {
                $timedOut = $true
                break
            }
        }
    } finally {
        try {
            if (-not $process.HasExited) {
                $process.Kill($true)
                $process.WaitForExit()
            }
            $code = $process.ExitCode
        } finally {
            $process.Dispose()
        }
    }
    $testCount = $null
    $completed = $false
    if (Test-Path -LiteralPath $report -PathType Leaf) {
        foreach ($line in [IO.File]::ReadLines($report)) {
            Write-Output $line
            if ($line -cmatch '^\[doctest\] test cases:\s+([0-9]+)\s+\|') {
                $testCount = [long]$Matches[1]
            }
            if ($line -ceq '[doctest] Status: SUCCESS!') { $completed = $true }
        }
    }
    if ($timedOut) { throw "Test application exceeded $TimeoutSeconds seconds and was terminated. Report: $report" }
    if ($code -ne 0) { throw "Windows tests failed (exit $code). Report: $report" }
    if (-not $completed -or $null -eq $testCount) {
        throw "The test application did not complete its report. Inspect $report and WindowsDesktop/App logs."
    }
    if ($testCount -eq 0) { throw "No test cases ran. Check -TestArguments filters. Report: $report" }
} finally {
    $runLock.Dispose()
}
