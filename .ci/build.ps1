choco install ant
# For some reason Gradle doesn't seem to find WINSDK_DIR environment variable.
$env:WINSDK_DIR='C:\Program Files (x86)\Windows Kits\10'
$env:VCINSTALLDIR='C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build'
$msvcToolsVer = Get-Content "$env:VCINSTALLDIR\Microsoft.VCToolsVersion.default.txt"
if ([string]::IsNullOrWhitespace($msvcToolsVer)) {
  # The MSVC tools version file can have txt *or* props extension.
  $msvcToolsVer = Get-Content "$env:VCINSTALLDIR\Microsoft.VCToolsVersion.default.props"
}
$env:MSVC_VER = $msvcToolsVer
$env:VS150COMNTOOLS = $env:VCINSTALLDIR
$env:VSVARS32FILE = "$env:VCINSTALLDIR\vcvars32.bat"
refreshenv
.\gradlew build -PCOMPILE_WEBKIT=false -PCONF=DebugNative --stacktrace -x :web:test --info

