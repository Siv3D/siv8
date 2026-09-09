# Requires PowerShell 7 and Visual Studio C++ tools. Run from any directory.
param(
    [ValidateSet('Debug', 'Release')][string]$Configuration = 'Release',
    [ValidateRange(1, 64)][int]$Jobs = 4,
    [string[]]$TestArguments = @(),
    [switch]$SkipBuild
)
$ErrorActionPreference = 'Stop'
if (-not $IsWindows) { throw 'This runner requires Windows and PowerShell 7.' }
$repo = (Resolve-Path (Join-Path $PSScriptRoot '..')).Path
& python "$repo/tools/check_test_projects.py" --windows-only
if ($LASTEXITCODE -ne 0) { throw 'Visual Studio test registration failed.' }
if (-not $SkipBuild) {
    $vswhere = "${env:ProgramFiles(x86)}/Microsoft Visual Studio/Installer/vswhere.exe"
    $vs = & $vswhere -latest -products '*' -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
    if (-not $vs) { throw 'MSVC x64 tools were not found.' }
    & "$vs/MSBuild/Current/Bin/MSBuild.exe" "$PSScriptRoot/OpenSiv3D.slnx" /nologo /v:minimal "/m:$Jobs" "/p:CL_MPCount=$Jobs" "/p:Configuration=$Configuration" /p:Platform=x64
    if ($LASTEXITCODE -ne 0) { throw 'Windows build failed.' }
}
$name = if ($Configuration -eq 'Debug') { 'Siv3D-Test(debug).exe' } else { 'Siv3D-Test.exe' }
$exe = Join-Path $PSScriptRoot "App/$name"
if (-not (Test-Path -LiteralPath $exe)) { throw "Test executable not found: $exe" }
$reports = Join-Path $PSScriptRoot 'Intermediate/TestReports'
New-Item -ItemType Directory -Force -Path $reports | Out-Null
$report = Join-Path $reports ("{0}-{1}.txt" -f $Configuration, (Get-Date -Format 'yyyyMMdd-HHmmss-ffff'))
$info = [Diagnostics.ProcessStartInfo]::new()
$info.FileName = $exe
$info.WorkingDirectory = Join-Path $PSScriptRoot 'App'
$info.UseShellExecute = $false
$info.CreateNoWindow = $true
$info.WindowStyle = [Diagnostics.ProcessWindowStyle]::Hidden
foreach ($argument in (@('--test-only', '--no-colors', "--out=$report") + $TestArguments)) {
    $info.ArgumentList.Add($argument)
}
$process = [Diagnostics.Process]::Start($info)
try {
    $process.WaitForExit()
    $code = $process.ExitCode
} finally {
    $process.Dispose()
}
if (Test-Path -LiteralPath $report) { Get-Content -LiteralPath $report }
else { throw "The test application produced no report (exit $code). Inspect WindowsDesktop/App logs." }
Write-Host "Test report: $report"
if ($code -ne 0) { throw "Windows tests failed (exit $code)." }
if (-not (Select-String -LiteralPath $report -Quiet -Pattern '^\[doctest\] Status: SUCCESS!$')) {
    throw 'The test application did not produce a successful completion summary. Inspect the report for an interrupted run; use the doctest console reporter.'
}
