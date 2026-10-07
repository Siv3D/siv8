param(
    [string]$BaselineRef = 'aeb21416cf34a5a5393bdc2b659edd61d1d427f9'
)
$ErrorActionPreference = 'Stop'
if (-not $IsWindows) { throw 'This tool requires Windows and PowerShell 7.' }
$repo = (Resolve-Path (Join-Path $PSScriptRoot '../..')).Path
$out = Join-Path $PSScriptRoot 'out'
New-Item -ItemType Directory -Force -Path $out | Out-Null
$vswhere = "${env:ProgramFiles(x86)}/Microsoft Visual Studio/Installer/vswhere.exe"
$vs = & $vswhere -latest -products '*' -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
if (-not $vs) { throw 'MSVC x64 tools were not found.' }
Import-Module (Join-Path $vs 'Common7/Tools/Microsoft.VisualStudio.DevShell.dll')
Enter-VsDevShell -VsInstallPath $vs -SkipAutomaticLocation -DevCmdArguments '-arch=x64 -host_arch=x64' | Out-Null
$common = @('/nologo', '/std:c++latest', '/O2', '/EHsc', '/utf-8', '/Zc:__cplusplus',
    '/D_ENABLE_EXTENDED_ALIGNED_STORAGE', '/D_SILENCE_CXX23_ALIGNED_STORAGE_DEPRECATION_WARNING',
    '/D_SILENCE_CXX23_DENORM_DEPRECATION_WARNING', "/I$repo/Siv3D/include", "/I$repo/Siv3D/include/ThirdParty")
$common += @('/MT', '/DNDEBUG')
$link = @('/link', "/LIBPATH:$repo/Siv3D/lib/Windows", 'shell32.lib', 'gdi32.lib', 'advapi32.lib', 'user32.lib', '/OPT:REF', '/OPT:ICF')
$library = "$repo/Siv3D/lib/Windows/siv3d/Siv3D.lib"
if (-not (Test-Path -LiteralPath $library)) { throw "Build the Windows Siv3D Release x64 library first." }
$native = "$repo/Siv3D/src/Siv3D-Platform/WindowsDesktop"
$detailPath = "$native/Siv3D/BinaryFileReader"
$public = "$repo/Siv3D/src/Siv3D/BinaryFileReader/SivBinaryFileReader.cpp"

# A failed rebuild must not leave a valid manifest for mixed old/new executables.
if (Test-Path -LiteralPath "$out/build.json") { Remove-Item -LiteralPath "$out/build.json" }
# Preserve a reproducible ifstream baseline after the engine switches backends.
@"
from pathlib import Path
import hashlib,json,subprocess,sys
repo,ref,out = sys.argv[1:]
revision=subprocess.run(['git','-C',repo,'rev-parse','--verify',ref+'^{commit}'],check=True,capture_output=True,text=True).stdout.strip()
base=Path(out)/'baseline'
# Paths in the historical baseline revision.
paths={
'Siv3D/BinaryFileReader/BinaryFileReaderDetail.hpp':'Siv3D/src/Siv3D-Platform/WindowsDesktop/Siv3D/BinaryFileReader/BinaryFileReaderDetail.hpp',
'Siv3D/BinaryFileReader/BinaryFileReaderDetail.cpp':'Siv3D/src/Siv3D-Platform/WindowsDesktop/Siv3D/BinaryFileReader/BinaryFileReaderDetail.cpp',
'SivBinaryFileReader.cpp':'Siv3D/src/Siv3D/BinaryFileReader/SivBinaryFileReader.cpp'}
hashes={}
for target,source in paths.items():
    data=subprocess.run(['git','-C',repo,'show',revision+':'+source],check=True,capture_output=True).stdout
    path=base/target
    path.parent.mkdir(parents=True,exist_ok=True)
    path.write_bytes(data)
    hashes[source]=hashlib.sha256(data).hexdigest()
(base/'source.json').write_text(json.dumps(dict(revision=revision,sha256=hashes),indent=2)+'\n',encoding='utf-8')
"@ | & python - $repo $BaselineRef $out
if ($LASTEXITCODE -ne 0) { throw 'Could not export the baseline. Ensure BaselineRef is present in local git history.' }
$baseline = "$out/baseline"
$overlay = "$out/overlay"
$overlayDetail = "$overlay/Siv3D/BinaryFileReader"
New-Item -ItemType Directory -Force -Path $overlayDetail | Out-Null
$header = [IO.File]::ReadAllText("$baseline/Siv3D/BinaryFileReader/BinaryFileReaderDetail.hpp")
$source = [IO.File]::ReadAllText("$baseline/Siv3D/BinaryFileReader/BinaryFileReaderDetail.cpp")
function Replace-Once([string]$Text, [string]$Old, [string]$New) {
    if (($Text.Split(@($Old), [StringSplitOptions]::None).Length - 1) -ne 1) { throw "Review the baseline substitution: $Old" }
    return $Text.Replace($Old, $New)
}
$header = Replace-Once $header '# include <fstream>' '# include "Win32InputStream.hpp"'
$header = Replace-Once $header 'std::ifstream file;' 'benchmark::Win32InputStream file;'
$source = Replace-Once $source '# include "BinaryFileReaderDetail.hpp"' '# include <Siv3D/BinaryFileReader/BinaryFileReaderDetail.hpp>'
[IO.File]::WriteAllText("$overlayDetail/BinaryFileReaderDetail.hpp", $header.Replace("`r`n", "`n"), [Text.UTF8Encoding]::new($false))
[IO.File]::WriteAllText("$overlayDetail/BinaryFileReaderDetail.cpp", $source.Replace("`r`n", "`n"), [Text.UTF8Encoding]::new($false))
$harness = "$out/benchmark.obj"
& cl @common /c "$PSScriptRoot/benchmark.cpp" "/Fo$harness"
if ($LASTEXITCODE -ne 0) { throw "Benchmark compilation failed (cl exit $LASTEXITCODE)." }
$variants = @(
    @{ Name = 'production'; Buffer = 16384; Sequential = 0 },
    @{ Name = 'ifstream'; Buffer = -1; Sequential = 0 },
    @{ Name = 'win32-raw'; Buffer = 0; Sequential = 0 },
    @{ Name = 'win32-4k'; Buffer = 4096; Sequential = 0 },
    @{ Name = 'win32-16k'; Buffer = 16384; Sequential = 0 },
    @{ Name = 'win32-64k'; Buffer = 65536; Sequential = 0 },
    @{ Name = 'win32-64k-sequential'; Buffer = 65536; Sequential = 1 }
)
foreach ($variant in $variants) {
    $dir = "$out/$($variant.Name)"
    New-Item -ItemType Directory -Force -Path $dir | Out-Null
    $flags = @()
    $detail = "$baseline/Siv3D/BinaryFileReader/BinaryFileReaderDetail.cpp"
    $api = "$baseline/SivBinaryFileReader.cpp"
    if ($variant.Name -eq 'production') {
        $detail = "$detailPath/BinaryFileReaderDetail.cpp"
        $api = $public
        $flags += "/I$native"
    } elseif ($variant.Buffer -ge 0) {
        $flags += @("/I$overlay", "/I$PSScriptRoot", "/DREADER_BUFFER_SIZE=$($variant.Buffer)", "/DREADER_SEQUENTIAL_SCAN=$($variant.Sequential)")
        $detail = "$overlayDetail/BinaryFileReaderDetail.cpp"
    } else {
        $flags += "/I$baseline"
    }
    & cl @common @flags /c $api "/Fo$dir/public.obj"
    if ($LASTEXITCODE -ne 0) { throw "Public API compilation failed: $($variant.Name)" }
    & cl @common @flags /c $detail "/Fo$dir/detail.obj"
    if ($LASTEXITCODE -ne 0) { throw "Backend compilation failed: $($variant.Name)" }
    & cl /nologo $harness "$dir/public.obj" "$dir/detail.obj" "/Fe$dir/benchmark.exe" @link "/MAP:$dir/benchmark.map"
    if ($LASTEXITCODE -ne 0) { throw "Link failed: $($variant.Name)" }
    $map = [IO.File]::ReadAllText("$dir/benchmark.map")
    if ($map -notmatch '(?m)read@BinaryFileReader@s3d@@[^\r\n]*\s+public\.obj\s*$' -or
        $map -notmatch '(?m)read@BinaryFileReaderDetail@BinaryFileReader@s3d@@[^\r\n]*\s+detail\.obj\s*$') {
        throw "The linker map did not identify the expected reader objects: $($variant.Name)"
    }
}
$binaryHashes = [ordered]@{}
foreach ($variant in $variants) { $binaryHashes[$variant.Name] = (Get-FileHash "$out/$($variant.Name)/benchmark.exe").Hash.ToLowerInvariant() }
$metadata = [ordered]@{
    executable_sha256 = $binaryHashes
    baseline = (Get-Content -Raw "$baseline/source.json" | ConvertFrom-Json)
    os = [System.Runtime.InteropServices.RuntimeInformation]::OSDescription
    cpu = (Get-CimInstance Win32_Processor | Select-Object -ExpandProperty Name)
    visualStudio = $vs
    compiler = (Get-Item (Get-Command cl).Source).VersionInfo.FileVersion
    flags = $common
    librarySHA256 = (Get-FileHash $library).Hash
    publicSHA256 = (Get-FileHash $public).Hash
    detailSHA256 = (Get-FileHash "$detailPath/BinaryFileReaderDetail.cpp").Hash
    harnessSHA256 = (Get-FileHash "$PSScriptRoot/benchmark.cpp").Hash
    detailHeaderSHA256 = (Get-FileHash "$detailPath/BinaryFileReaderDetail.hpp").Hash
    adapterSHA256 = (Get-FileHash "$PSScriptRoot/Win32InputStream.hpp").Hash
    gitRevision = (& git -C $repo rev-parse HEAD)
    gitStatus = @(& git -C $repo status --short)
    variants = $variants
}
$metadata | ConvertTo-Json -Depth 5 | Set-Content -Encoding utf8 "$out/build.json"
Write-Host "Built $($variants.Count) variants in $out"
