$files = Get-Item *.jpg
foreach($f in $files)
{
    Start-Process -FilePath TextureConverter.exe -ArgumentList "$f -mi 3" -Wait
}
pause