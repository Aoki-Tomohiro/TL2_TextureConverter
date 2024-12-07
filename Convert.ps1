$files = Get-Item *.jpg
foreach($f in $files)
{
    Start-Process -FilePath TextureConverter.exe -ArgumentList $f -Wait
}
pause