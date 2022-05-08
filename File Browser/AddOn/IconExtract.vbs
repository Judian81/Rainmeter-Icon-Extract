dim temppath, workingpath, videoextensions, pictureextensions, musicextensions, thumbnailsize


temppath = WScript.Arguments(0)


'MsgBox """" & temppath & """", 65, "MsgBox Example"

workingpath = WScript.Arguments(1)
videoextensions = WScript.Arguments(2)
pictureextensions = WScript.Arguments(3)
musicextensions = WScript.Arguments(4)
thumbnailsize = WScript.Arguments(5)

CreateObject("Wscript.Shell").Run """" & temppath & """" &  " " & """" & workingpath & """" & " " & """" &  videoextensions & """" & " " & """" & pictureextensions & """" & " " & """" & musicextensions & """" & " " & """" & thumbnailsize & """", 0, False

