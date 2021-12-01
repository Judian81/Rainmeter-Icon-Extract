# Rainmeter-Icon-Extract
this saves icons in cache folder from files you browse.


this is the command line options:

1. call IconExtract.exe -extract - "c:\documents\file.pdf" "c:\cache\c\documents\" -scale "256"
this will make an icon to a place you want to. from the place you want to.
this is the unsave way to do this. but you have full controle over what you are doing.
this could also be used without rainmeter.

2. call IconExtract.exe "-create-cache-path" "%browse2%\"
this will make all the directorys that would be needed to place the icon to.
without this the directorys would not exist and so could not be created an icon for.

3. call IconExtract.exe "-safe-ffmpeg-wrapper-video" "c:\programdata\2cool\video.mp4"
this will make an icon from a file that could be a video file.
the cache folder is already made so this it the save version to make icon files.

4. call IconExtract.exe "-safe-ffmpeg-wrapper-picture" "c:\programdata\2cool\picture.png"
this will make a icon from a picture file. and be stored in the cache path automatically.

5. call IconExtract.exe "-safe-ffmpeg-wrapper-music" "c:\programdata\2cool\music.mp3"
this will make a icon from a music file. and be stored in the cache path automatically.

6. call IconExtract.exe "-auto-single" "c:\programdata\2cool\program.exe"
this will make a icon from a exe file or an url file. and be stored in the cache path automatically.

7. call IconExtract.exe "-auto-global" "c:\programdata\2cool\text.txt"
this will make a icon from every file that are no icons from extracted. and be stored in the cache path automatically.

8. call IconExtract.exe -help
