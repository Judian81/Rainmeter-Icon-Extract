# Rainmeter-Icon-Extract
this saves icons in cache folder from files you browse.


this is some bat code i use for extracting icons to a cache folder.

//----------------------------------------------------------------------------------------------------

%CachePath2% = "C:\somefolder\temp\C\another folder\"

%ScriptPath2%\..\AddOn\IconExtract.exe = "this is the folder where IconExtract.exe is in."

%BrowsePath2% = "the folder we work or browse in" like for example "C:\another folder\"

%SkinTemp2%\..\Custom Icons\%%~xf.png = "save an icon for examle as .pdf.png. we just have to extract it 1 time."

%%f = "the file we give to make some txt reference for a png file. or make the png file it self."

%%~xf = "this is only the extension of a file."



echo this is used to extract icon file and create a text file to refere.
echo --------------------------------------------------------------------------------------------
  if not exist "%CachePath2%%%f.png.txt" (
    call "%ScriptPath2%\..\AddOn\IconExtract.exe" -extract "%BrowsePath2%%%f" "%CachePath2%%%f" -scale %size%
    echo|set /p= "%CachePath2%%%f.png" > "%CachePath2%%%f.png.txt"
  )
 )
echo --------------------------------------------------------------------------------------------
echo.
echo extract all icons that are left.
echo --------------------------------------------------------------------------------------------
for /f "tokens=* delims=" %%f in ('dir /b/a-d /b *') do (
  if not exist "%CachePath2%%%f.png.txt" (
    call "%ScriptPath2%\..\AddOn\IconExtract.exe" -extract "%BrowsePath2%%%f" "%SkinTemp2%\..\Custom Icons\%%~xf" -scale %size%
rem	if not exist "%SkinTemp2%\..\Custom Icons\%%~xf.png" (
      echo|set /p= "%SkinTemp2%\..\Custom Icons\%%~xf.png" > "%CachePath2%%%f.png.txt"
rem	)
  )
)

