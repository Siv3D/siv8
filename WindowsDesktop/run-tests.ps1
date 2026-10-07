#Requires -Version 7.0

<#
.SYNOPSIS
Build and run the Windows x64 automated tests from any directory.
.EXAMPLE
./WindowsDesktop/run-tests.ps1
.EXAMPLE
./WindowsDesktop/run-tests.ps1 -SkipBuild -TestArguments '--test-case=*BinaryFileReader*', '--durations', 'yes'
.PARAMETER Configuration
Build configuration (Release by default).
.PARAMETER Jobs
Maximum MSBuild nodes and compiler processes per project (default: 4).
.PARAMETER SkipBuild
Run the existing executable; the caller must ensure it matches current sources.
.PARAMETER TestArguments
Catch2 execution arguments, one per array element. The runner owns reporting
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

# The runner owns both reports and requires test execution. Reject queries and
# report overrides, including attached/combined short options that could hide one.
$reserved = @('out', 'reporter', 'colour-mode', 'verbosity', 'help', 'libidentify',
    'list-tests', 'list-tags', 'list-reporters', 'list-listeners', 'allow-running-no-tests')
foreach ($argument in $TestArguments) {
    $reservedLong = $argument -cmatch '^--([^=]+)(?:=.*)?$' -and $reserved -ccontains $Matches[1]
    $unsupportedShort = $argument -cmatch '^-[^-]' -and
        $argument -cnotmatch '^-(?:s|b|i|a|x|w|d|f|c|#|n|e|x[0-9]+)$'
    if ($reservedLong -or $unsupportedShort -or $argument -ceq '--') {
        throw "Catch2 option '$argument' is reserved: this runner requires its reports and actual test execution."
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
    $xmlReport = [IO.Path]::ChangeExtension($report, 'xml')
    Write-Host "Test reports: $report and $xmlReport"
    $info = [Diagnostics.ProcessStartInfo]::new()
    $info.FileName = $exe
    $info.WorkingDirectory = Join-Path $PSScriptRoot 'App'
    $info.UseShellExecute = $false
    $info.CreateNoWindow = $true
    $info.WindowStyle = [Diagnostics.ProcessWindowStyle]::Hidden
    # Keep the reporter paths ASCII and relative to App, including in Unicode checkouts.
    $relativeReport = "../Intermediate/TestReports/$reportName"
    $relativeXmlReport = [IO.Path]::ChangeExtension($relativeReport, 'xml')
    foreach ($argument in (@('--test-only', '--colour-mode', 'none',
        '--reporter', "console::out=$relativeReport", '--reporter', "xml::out=$relativeXmlReport") + $TestArguments)) {
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
    if (Test-Path -LiteralPath $report -PathType Leaf) {
        foreach ($line in [IO.File]::ReadLines($report)) { Write-Output $line }
    }
    if ($timedOut) { throw "Test application exceeded $TimeoutSeconds seconds and was terminated. Report: $report" }
    if ($code -ne 0) { throw "Windows tests failed (exit $code). Report: $report" }
    # XML must be complete; a process can exit zero before the test runner finishes.
    try {
        $document = [xml]::new()
        $document.XmlResolver = $null
        $document.Load($xmlReport)
        $counts = $document.SelectSingleNode('/Catch2TestRun/OverallResultsCases')
        $assertions = $document.SelectSingleNode('/Catch2TestRun/OverallResults')
        if ($null -eq $counts -or $null -eq $assertions) { throw 'Missing totals' }
        foreach ($totals in @($counts, $assertions)) {
            foreach ($attribute in @('successes', 'failures', 'expectedFailures', 'skips')) {
                if ($totals.GetAttribute($attribute) -cnotmatch '^[0-9]+$') { throw 'Invalid totals' }
            }
        }
        $testCount = [long]$counts.GetAttribute('successes') + [long]$counts.GetAttribute('failures') +
            [long]$counts.GetAttribute('expectedFailures')
        $failed = [long]$counts.GetAttribute('failures') + [long]$assertions.GetAttribute('failures')
    } catch {
        throw "The test application did not complete its report. Inspect $xmlReport and WindowsDesktop/App logs."
    }
    # Catch2's console reporter does not write captured test output to its file.
    # Retain benchmark tables and other stdout/stderr alongside the console summary.
    foreach ($node in $document.SelectNodes('/Catch2TestRun/TestCase/StdOut | /Catch2TestRun/TestCase/StdErr')) {
        $captured = $node.InnerText
        [IO.File]::AppendAllText($report, "`n$captured`n")
        Write-Output $captured
    }
    if ($testCount -eq 0) { throw "No test cases ran. Check -TestArguments filters. Report: $report" }
    if ($failed -ne 0) { throw "Windows tests failed in the XML report. Report: $xmlReport" }
} finally {
    $runLock.Dispose()
}
