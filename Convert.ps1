param 
(
    [string]$mipLevel = "-ml 0",
    [string]$outputDir
)

$files = Get-Item *.jpg, *.png
foreach($f in $files)
{
    $targetDir = if ($outputDir) { $outputDir } else { $f.DirectoryName }
    Start-Process -FilePath TextureConverter.exe -ArgumentList "$f $targetDir $mipLevel" -Wait
}
pause