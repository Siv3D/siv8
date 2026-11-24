#Requires -Version 7.0
# Build the current Release engine with WindowsDesktop/run-tests.ps1 first.
$ErrorActionPreference = 'Stop'
$PSNativeCommandUseErrorActionPreference = $false
if (-not $IsWindows) { throw 'This check requires Windows and MSVC x64 tools.' }
$repo = (Resolve-Path (Join-Path $PSScriptRoot '../..')).Path
$out = Join-Path $repo 'WindowsDesktop/Intermediate/MemoryMappingChecks'
New-Item -ItemType Directory -Force -Path $out | Out-Null
$vswhere = "${env:ProgramFiles(x86)}/Microsoft Visual Studio/Installer/vswhere.exe"
$vs = & $vswhere -latest -products '*' -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
if ($LASTEXITCODE -ne 0 -or -not $vs) { throw 'MSVC x64 tools were not found.' }
Import-Module (Join-Path $vs 'Common7/Tools/Microsoft.VisualStudio.DevShell.dll')
Enter-VsDevShell -VsInstallPath $vs -SkipAutomaticLocation -DevCmdArguments '-arch=x64 -host_arch=x86' | Out-Null
$library = Join-Path $repo 'Siv3D/lib/Windows/siv3d/Siv3D.lib'
if (-not (Test-Path -LiteralPath $library)) { throw 'Build the current Windows Release engine first.' }
$native = Join-Path $repo 'Siv3D/src/Siv3D-Platform/WindowsDesktop'
$common = @('/nologo', '/std:c++latest', '/permissive-', '/O2', '/EHsc', '/utf-8', '/Zc:__cplusplus', '/W4', '/MT', '/DNDEBUG',
    '/D_ENABLE_EXTENDED_ALIGNED_STORAGE', '/D_SILENCE_CXX23_ALIGNED_STORAGE_DEPRECATION_WARNING',
    '/D_SILENCE_CXX23_DENORM_DEPRECATION_WARNING', "/I$repo/Siv3D/include", "/I$repo/Siv3D/include/ThirdParty", "/I$native")
$objects = @()
foreach ($type in @('MemoryMappedFile', 'MemoryMappedFileView')) {
    $publicObject = Join-Path $out "$type-public.obj"
    & cl @common /c "$repo/Siv3D/src/Siv3D/$type/Siv$type.cpp" "/Fo$publicObject"
    if ($LASTEXITCODE -ne 0) { throw "Public API compilation failed: $type" }
    $detailObject = Join-Path $out "$type-detail.obj"
    & cl @common "/FI$PSScriptRoot/Inject.hpp" /c "$native/Siv3D/$type/$($type)Detail.cpp" "/Fo$detailObject"
    if ($LASTEXITCODE -ne 0) { throw "Backend compilation failed: $type" }
    $objects += @($publicObject, $detailObject)
}
$harness = Join-Path $out 'check.obj'
& cl @common /c "$PSScriptRoot/check.cpp" "/Fo$harness"
if ($LASTEXITCODE -ne 0) { throw 'Harness compilation failed.' }
$exe = Join-Path $out 'memory-mapping-checks.exe'
$mapPath = Join-Path $out 'memory-mapping-checks.map'
& cl /nologo $harness @objects "/Fe$exe" /link "/LIBPATH:$repo/Siv3D/lib/Windows" shell32.lib gdi32.lib advapi32.lib user32.lib /OPT:REF /OPT:ICF "/MAP:$mapPath"
if ($LASTEXITCODE -ne 0) { throw 'Fault-injection link failed.' }
$map = [IO.File]::ReadAllText($mapPath)
foreach ($type in @('MemoryMappedFile', 'MemoryMappedFileView')) {
    if ($map -notmatch "(?m)map@$type@s3d@@[^\r\n]*\s+$type-public\.obj\s*$" -or
        $map -notmatch "(?m)map@$($type)Detail@$type@s3d@@[^\r\n]*\s+$type-detail\.obj\s*$") {
        throw "The linker map did not select the instrumented objects: $type"
    }
}
& $exe (Join-Path $out 'fixtures') | Tee-Object -FilePath (Join-Path $out 'results.txt')
if ($LASTEXITCODE -ne 0) { throw 'Memory-mapping fault injection failed.' }
Write-Host "Artifacts: $out"
