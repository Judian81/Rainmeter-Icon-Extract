# Rainmeter-Icon-Extract
this saves icons in cache folder from files you browse.


this is the command line options:

1. call IconExtract.exe -extract - "c:\documents\file.pdf" "c:\cache\c\documents\" -scale "256"
this will make an icon to a place you want to. from the place you want to.
this is the unsave way to do this. but you have full controle over what you are doing.
this could also be used without rainmeter.

2. call IconExtract.exe "-create-cache-path" "c:\programdata\2cool\folder1\folder2\lastfolder\"
this will make all the directorys that would be needed to place the icon to.
without this the directorys would not exist and so could not be created an icon for.

3. call IconExtract.exe "-create-cache-subfolders" "c:\programdata\2cool\subfolder1"
all the subfolders in this folder will be made icons for.

4. call IconExtract.exe "-safe-ffmpeg-wrapper-video" "c:\programdata\2cool\video.mp4"
this will make an icon from a file that could be a video file.
the cache folder is already made so this it the save version to make icon files.

5. call IconExtract.exe "-safe-ffmpeg-wrapper-picture" "c:\programdata\2cool\picture.png"
this will make a icon from a picture file. and be stored in the cache path automatically.

6. call IconExtract.exe "-safe-ffmpeg-wrapper-music" "c:\programdata\2cool\music.mp3"
this will make a icon from a music file. and be stored in the cache path automatically.

7. call IconExtract.exe "-auto-single" "c:\programdata\2cool\program.exe"
this will make a icon from a exe file or an url file. and be stored in the cache path automatically.

8. call IconExtract.exe "-auto-global" "c:\programdata\2cool\text.txt"
this will make a icon from every file that are no icons from extracted. and be stored in the cache path automatically.

9. call IconExtract.exe -help



![file browser](https://user-images.githubusercontent.com/46109964/160290028-c177da72-057e-4ae0-aff5-5fed599e083c.png)



as you can see, there will be made thumbnail for video, music and a picture file.
also you can get the the icon from a exe, lnk and url file.
if there is no personal icon. then you get the icon from the file type it self.



File Browser

i try to make this as save as possible. because it makes files and folders i made it like the way it can only be saved in the folder of the IconExtract.exe folder. also what is true is i used the ffmpeg.exe file and try to keep it save in it own circle. you can download this on http://ffmpeg.org/download.html. there are a lot of files to be downloaded from. but you only need to download the package were there are only exe files in. and if you have that file you only need the ffmpeg.exe file to be extracted in the AddOn folder. on this way you can save icons from video, music and picture files. i already downloaded the file and put it in AddOn folder. but if you want to be sure it came from the right resource or want an updated version. go to. http://ffmpeg.org/download.html.


if you want to comment. the origional post can be found here.
https://forum.rainmeter.net/viewtopic.php?p=206612&sid=15f83bd089c9a222f639d766c74dcecc#p206606
