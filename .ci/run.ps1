# Run this script from the git repository root (one level up from here).

$jfxBuild = $ExecutionContext.SessionState.Path.GetUnresolvedProviderPathFromPSPath('.\')
$env:JFX_BUILD = (Get-Item $jfxBuild).fullName
Set-Alias -Name javafx -Value "$env:JAVA_HOME\bin\java.exe @$env:JFX_BUILD\build\run.args"
