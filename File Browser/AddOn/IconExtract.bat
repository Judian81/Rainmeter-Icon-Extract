@echo off
Chcp 10000

if %1 == "" (
  goto :eof
)
if %2 == "" (
  goto :eof
)
if %3 == "" (
  goto :eof
)
if %4 == "" (
  goto :eof
)
if %5 == "" (
  goto :eof
)

set browse1=%1
for /F "delims=" %%f in (%browse1%) do set browse2=%%f
rem c:\programdata\2cool\
set skinaddon2=%~dp0
rem C:\rainmeter\skin\AddOn\
set temp1=%browse1:C:=C%
for /F "delims=" %%f in (%temp1%) do set temp2=%%f
rem "C\programdata\2cool\"
set cachedir1=%skinaddon2%Temp\%temp2%
rem C:\rainmeter\skin\AddOn\Temp\C\programdata\2cool\
set videofile1=%2
for /F "delims=" %%f in (%videofile1%) do set videofile2=%%f
rem .avi
set imagefile1=%3
for /F "delims=" %%f in (%imagefile1%) do set imagefile2=%%f
rem .png
set musicfile1=%4
for /F "delims=" %%f in (%musicfile1%) do set musicfile2=%%f
rem .mp3
set size1=%5
for /F "delims=" %%f in (%size1%) do set size2=%%f

rem c:\programdata\2cool\
cd "%browse2%"

rem this is save to use, make al the folders we need to save or cache icons to.
call "%skinaddon2%\IconExtract.exe" "-create-cache-path" "%browse2%\"

rem call "%skinaddon2%\IconExtract.exe" "-safe-ffmpeg-wrapper-video" "c:\programdata\2cool\video.mp4"
for /f "tokens=* delims=" %%f in ('dir /b/a-d /b %videofile2% ^| sort') do (
  if not exist "%cachedir1%%%f.png.txt" (
    call "%skinaddon2%\IconExtract.exe" "-safe-ffmpeg-wrapper-video" "%browse2%%%f"
  )
)

rem call "%skinaddon2%\IconExtract.exe" "-safe-ffmpeg-wrapper-picture" "c:\programdata\2cool\picture.png"
for /f "tokens=* delims=" %%f in ('dir /b/a-d /b %imagefile2% ^| sort') do (
  if not exist "%cachedir1%%%f.png.txt" (
    call "%skinaddon2%\IconExtract.exe" "-safe-ffmpeg-wrapper-picture" "%browse2%%%f"
  )
)

rem call "%skinaddon2%\IconExtract.exe" "-safe-ffmpeg-wrapper-video" "c:\programdata\2cool\music.mp3"
for /f "tokens=* delims=" %%f in ('dir /b/a-d /b %musicfile2% ^| sort') do (
  if not exist "%cachedir1%%%f.png.txt" (
    call "%skinaddon2%\IconExtract.exe" "-safe-ffmpeg-wrapper-music" "%browse2%%%f"
	)
  )
)

rem safe to use.
for /f "tokens=* delims=" %%f in ('dir /b/a-d /b *.exe *.lnk *.url ^| sort') do (
  if not exist "%cachedir1%%%f.png.txt" (
    call "%skinaddon2%\IconExtract.exe" -auto-single "%browse2%%%f"
  )
)

rem function safe to use.
for /f "tokens=* delims=" %%f in ('dir /b/a-d /b * ^| sort') do (
  if not exist "%cachedir1%%%f.png.txt" (
    call "%skinaddon2%\IconExtract.exe" -auto-global "%browse2%%%f"
  )
)

rem this function is save. it could not spin out of controle with making icons in directory you do not want to.
for /f "tokens=* delims=" %%f in ('dir /b/ad  * ^| sort') do (
  if not exist "%cachedir1%%%f.png.txt" (
	call "%skinaddon2%\IconExtract.exe" -create-cache-subfolders "%browse2%%%f"
  )
)

rem this function is also safe to use.
if not exist "%cachedir1%...png.txt" (
  call "%skinaddon2%\IconExtract.exe" -create-cache-subfolders "%browse2%.."
)
