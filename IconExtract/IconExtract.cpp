//official

#include <windows.h>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <direct.h>
#include <stdlib.h>
#include <stdio.h>

//help

#include <vector>
#include <gdiplus.h>
#include <regex>



//above ^ are official

#include <stdexcept>

#include <locale>

#include <codecvt>


#include <shlwapi.h>
#include <shlobj_core.h>
#include <shellapi.h>
#include <commoncontrols.h>
#include <io.h>
#include <msi.h>
#pragma comment(lib,"Msi.lib")
#pragma comment(lib, "gdiplus.lib")
#include <thread>

using namespace std;


#pragma pack(push, 2)
typedef struct	// 16 bytes
{
	BYTE        bWidth;				// Width, in pixels, of the image
	BYTE        bHeight;			// Height, in pixels, of the image
	BYTE        bColorCount;		// Number of colors in image (0 if >=8bpp)
	BYTE        bReserved;			// Reserved ( must be 0)
	WORD        wPlanes;			// Color Planes
	WORD        wBitCount;			// Bits per pixel
	DWORD       dwBytesInRes;		// How many bytes in this resource?
	DWORD       dwImageOffset;		// Where in the file is this image?
} ICONDIRENTRY, * LPICONDIRENTRY;

typedef struct	// 22 bytes
{
	WORD           idReserved;		// Reserved (must be 0)
	WORD           idType;			// Resource Type (1 for icons)
	WORD           idCount;			// How many images?
	ICONDIRENTRY   idEntries[1];	// An entry for each image (idCount of 'em)
} ICONDIR, * LPICONDIR;
#pragma pack(pop)

enum IconSize
{
	SMALL = 0,	// 16x16
	MEDIUM = 1,	// 32x32
	LARGE = 2,	// 48x48
	EXLARGE = 3	// 256x256
};

struct Property
{
	std::wstring path;
	std::wstring wholepath;
	std::wstring name;
	std::wstring ext;
	std::wstring nameandext;
	std::wstring icon;

	Property() :
		icon(L""),
		path(L""),
		wholepath(L""),
		name(L""),
		ext(L""),
		nameandext(L"")
	{}
};

struct Measure
{
	int scaleto;                            // this is the scaling option for the image.
	std::wstring filefrom;                 //this is the file were i want to get the picture from.
	std::wstring fileto;                   //this is the place were the picture has to be saved to.
	std::vector<std::wstring> command;
	Property property;
	bool useNewStyle;
	bool getTarget;
	bool allowNonstorage;
	bool copyLink;
	bool getIcon;
	IconSize iconSize;
	std::wstring linkCache;
	std::wstring iconCache;
	std::wstring fileInitDir;
	std::wstring imgInitDir;
	std::wstring* pReturn;		// Pointer to the string shown by GetString()

	void* rm = NULL;			// Pointer to the plugin measure
	void* skin = NULL;			// Pointer to the skin
	HWND hskin = NULL;
	IFileDialog* pfd = NULL;

	Measure() :
		command(),
		useNewStyle(false),
		getTarget(true),
		allowNonstorage(false),
		copyLink(false),
		getIcon(false),
		iconSize(EXLARGE),
		pReturn(nullptr),
		rm(nullptr),
		skin(nullptr) {}
};

void GetIcon(Measure* measure, std::wstring* filepath);
bool IsIconOnly48(Gdiplus::Bitmap* bmp);
bool SaveIcon(Measure* measure, HICON icon);



bool MakeSingleIcons(std::string);
bool MakeGlobalIcons(std::string);
bool MakeDirectorys(std::string);
bool MakeSubdirectorys(std::string);
bool MakeIconffmpegPicture(std::string);
bool MakeIconffmpegMusic(std::string);
bool MakeIconffmpegVideo(std::string);
bool UseExtract(std::string, std::string, std::string, std::string, char*);

int MsgBox(LPCWSTR toshow);
wstring ExePath();
std::string GetFileExtension(const std::string& FileName);
std::string GetFileName(const string& s);
std::string GetFullPath(const std::string& fname);
std::string ws2s(const std::wstring& str);
inline bool exists_test3(const std::string& name);

DWORD GetModuleFileName(HMODULE hModule, LPSTR lpFilename, DWORD nSize);

wstring ExePath() {
	TCHAR buffer[MAX_PATH] = { 0 };
	GetModuleFileName(NULL, buffer, MAX_PATH);
	std::wstring::size_type pos = std::wstring(buffer).find_last_of(L"\\/");
	return std::wstring(buffer).substr(0, pos);
}

int MsgBox(LPCWSTR toshow) {
	int msgboxID = MessageBox(NULL, toshow, toshow, MB_DEFBUTTON2);

	switch (msgboxID)
	{
	case IDCANCEL:
		// TODO: add code
		break;
	case IDTRYAGAIN:
		// TODO: add code
		break;
	case IDCONTINUE:
		// TODO: add code
		break;
	}
	return msgboxID;
}

std::string GetFileExtension(const std::string& Extension)
{
	for (int i = Extension.length(); i > 2 ; i--) {
		if (Extension.substr(i, 1) == "\\") {
			return Extension.substr(i + 1, (Extension.length() - i));
		} else if (Extension.substr(i, 1) == ".") {
			return Extension.substr(i, (Extension.length() -i));
		}
	}
	return "";
}

string GetFileName(const string& s) {

	char sep = '/';
	sep = '\\';
	size_t i = s.rfind(sep, s.length());
	if (i != string::npos) {
		string filename = s.substr(i + 1, s.length() - i);
		size_t lastindex = filename.find_last_of(".");
		string rawname = filename.substr(0, lastindex);
		return(rawname);
	}
	return "";
}

std::string GetFullPath(const std::string& fname)
{
	size_t pos = fname.find_last_of("\\/");
	return (std::string::npos == pos)
		? ""
		: fname.substr(0, pos);
}

//std::wstring s2ws(const std::string& s) {
//	int len;
//	int slength = (int)s.length() + 1;
//	len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
//	wchar_t* buf = new wchar_t[len];
//	MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
//	std::wstring r(buf);
//	delete[] buf;
//	return r;
//}

std::wstring s2ws(const std::string& string)
{
	if (string.empty())
	{
		return L"";
	}

	const auto size_needed = MultiByteToWideChar(CP_UTF8, 0, &string.at(0), (int)string.size(), nullptr, 0);
	if (size_needed <= 0)
	{
		throw std::runtime_error("MultiByteToWideChar() failed: " + std::to_string(size_needed));
	}

	std::wstring result(size_needed, 0);
	MultiByteToWideChar(CP_UTF8, 0, &string.at(0), (int)string.size(), &result.at(0), size_needed);
	return result;
}

//std::string ws2s(const std::wstring& str) {
//	std::string s(str.begin(), str.end());
//	return s;
//}
//
//std::string ws2s(const std::wstring& wstr)
//{
//	using convert_typeX = std::codecvt_utf8<wchar_t>;
//	std::wstring_convert<convert_typeX, wchar_t> converterX;
//
//	return converterX.to_bytes(wstr);
//}

std::string ws2s(const std::wstring& wide_string)
{
	if (wide_string.empty())
	{
		return "";
	}

	const auto size_needed = WideCharToMultiByte(CP_UTF8, 0, &wide_string.at(0), (int)wide_string.size(), nullptr, 0, nullptr, nullptr);
	if (size_needed <= 0)
	{
		throw std::runtime_error("WideCharToMultiByte() failed: " + std::to_string(size_needed));
	}

	std::string result(size_needed, 0);
	WideCharToMultiByte(CP_UTF8, 0, &wide_string.at(0), (int)wide_string.size(), &result.at(0), size_needed, nullptr, nullptr);
	return result;
}

inline bool exists_test3(const std::string& name) {
	ifstream f(name.c_str());
	return f.good();
}

int main(int argc, char* argv[], char* envp[]) {
	std::string command1 = argv[1];
	//--------------------------------------------------------------------------------------------------------------------------------------
	//--------------------------------------------------------------------------------------------------------------------------------------
	//--------------------------------------------------------------------------------------------------------------------------------------
	//--------------------------------------------------------------------------------------------------------------------------------------
	//--------------------------------------------------------------------------------------------------------------------------------------
	//--------------------------------------------------------------------------------------------------------------------------------------
	if (command1 == "-help") {
		std::cout << "'-safe-ffmpeg-wrapper-video' this is used to put ffmpeg to work and create icons for video files. the path is secured inside the IconExtract folder. you couse yourself what a video file is or not." << endl;
		std::cout << "you use it like this. IconExtract.exe '-safe-ffmpeg-wrapper-video' 'drive\\path\\file.avi'" << endl;
		std::cout << "\n";
		std::cout << "'-safe-ffmpeg-wrapper-picture' this is used to put ffmpeg to work and create icons for picture files. the path is secured inside the IconExtract folder. you couse yourself what is a picture file." << endl;
		std::cout << "you use it like this. IconExtract.exe '-safe-ffmpeg-wrapper-picture' 'drive\\path\\file.avi'" << endl;
		std::cout << "\n";
		std::cout << "'-safe-ffmpeg-wrapper-music' this is used to put ffmpeg to work and create icons for music files. the path is secured inside the IconExtract folder. you couse yourself what is a music file." << endl;
		std::cout << "you use it like this. IconExtract.exe '-safe-ffmpeg-wrapper-music' 'drive\\path\\file.avi'" << endl;
		std::cout << "\n";
		std::cout << "'-create-cache-subfolders' this is to create icons for all the folders there are in the path you want." << endl;
		std::cout << "you use it like this. IconExtract.exe '-create-cache-subfolders' 'drive\\path\\'" << endl;
		std::cout << "\n";
		std::cout << "'-create-cache-path' this is to secure that the path already exist before making icons." << endl;
		std::cout << "it is used to be like this '-create-cache-path' 'drive\\path\\'" << endl;
		std::cout << "\n";
		std::cout << "'-auto-single' this is used to extract a personal icon. mostly used for .exe and .url files. " << endl << "you couse yourself if it is about a exe file or other file." << endl;
		std::cout << "it is normaly used about IconExtract.exe '-auto-single' 'drive\\path\\program.exe'" << endl;
		std::cout << "\n";
		std::cout << "'-auto-global' this is used as last fallback. it here could not be an music icon created or something else the will be made a global one. like it will be used for a text file." << endl;
		std::cout << "it is normaly used like '-auto-global' 'drive\\path\\text.txt'" << endl;
		std::cout << "\n";
		std::cout << "\n";
		std::cout << "this is the most unsafe version. you have all the controle but it could happen that the files will be saved in folders you do not want to." << endl;
		std::cout << "type -extract \"drive:path\\filenamefrom.extension\" \"drive:\\cachepath\\\" -scale 256";
		std::cout << "\n";
		std::cout << "this wil make it possible to extract the icon from filenamefrom.";
		std::cout << "\n";
		std::cout << "the icon from filenamefrom wil be saved in the \"drive:\\cachepath\\\" location as cache icon.";
		std::cout << "\n";
		std::cout << "the -scale can be from 16, 32, 48 to 256. sorry 64 and 128 is not possible.";
		std::cout << "if you fill in 64 it will be 48. and if you fill in 128 it will be automaticly 256.";
		std::cout << "\n";
	}
	//--------------------------------------------------------------------------------------------------------------------------------------
	//--------------------------------------------------------------------------------------------------------------------------------------
	//--------------------------------------------------------------------------------------------------------------------------------------
	//--------------------------------------------------------------------------------------------------------------------------------------
	//--------------------------------------------------------------------------------------------------------------------------------------
	//--------------------------------------------------------------------------------------------------------------------------------------
	else if (command1 == "-auto-single") {
		MakeSingleIcons(argv[2]);
	} else if (command1 == "-auto-global") {
		MakeGlobalIcons(argv[2]);
	} else if (command1 == "-create-cache-path") {
		MakeDirectorys(argv[2]);
	} else if (command1 == "-create-cache-subfolders") {
		MakeSubdirectorys(argv[2]);
	} else if (command1 == "-safe-ffmpeg-wrapper-picture") {
		MakeIconffmpegPicture(argv[2]);
	} else if (command1 == "-safe-ffmpeg-wrapper-music") {
		MakeIconffmpegMusic(argv[2]);
	} else if (command1 == "-safe-ffmpeg-wrapper-video") {
		MakeIconffmpegVideo(argv[2]);
	} else if (command1 == "-extract") {
		UseExtract(argv[2], argv[3], argv[4], argv[5], argv[6]);
	}
}

// =================================================
// ==== Make Functions From Console Arguments
// =================================================

bool MakeIconffmpegVideo(std::string file1) {
	std::cout << "--------------------------------------" << endl;
	std::cout << "-safe-ffmpeg-wrapper-video" << endl;
	std::cout << "--------------------------------------" << endl;
	bool fileexist = exists_test3(file1);
	if (fileexist == 1) {
		std::cout << "file: " << "\'" << GetFileName(file1) << GetFileExtension(file1) << "\'" << " exist. go on with progress." << endl << endl;
	}
	else {
		std::cout << "file: " << "\'" << GetFileName(file1) << GetFileExtension(file1) << "\'" << " does not exist, cancel mission." << endl << endl;
		return 0;
	}
	std::string makepath = ws2s(ExePath()) + "\\Temp\\";
	std::string tempdirectory = GetFullPath(file1) + "\\";
	std::string driveletter = tempdirectory.substr(0, 1);
	std::string cachedir = tempdirectory.substr(2);
	std::string totalcachepath = makepath + driveletter + cachedir;
	std::cout << "cache path: " << "\'" << totalcachepath << "\'" << endl << endl;
	std::string file2 = totalcachepath + GetFileName(file1) + GetFileExtension(file1);
	std::string textlink = file2 + ".png.txt";
	std::string iconlink = file2 + ".png";
	fileexist = exists_test3(iconlink);
	if (fileexist == 1) {
		std::cout << "icon file: " << "\'" << iconlink << "\'" << " does exist, aborting mission." << endl << endl;
		return 0;
	}
	else {
		std::cout << "icon file: " << "\'" << iconlink << "\'" << " does not exist. trying to create one." << endl << endl;
	}
	std::stringstream ss;
	std::string pathOfInputFile1 = ws2s(ExePath()) + "\\ffmpeg.exe";
	std::string pathOfCommand1 = " -hide_banner -loglevel error -ss 20 -n -i ";
	std::string pathOfInputFile2 = file1;
	std::string pathOfCommand2 = " -r 1 -frames 1 -vf scale=256:-1 ";
	std::string pathOfInputFile3 = iconlink;
	ss << "\"";
	ss << "\"" << pathOfInputFile1 << "\"";
	ss << pathOfCommand1 ;
	ss << "\"" << pathOfInputFile2 << "\"";
	ss << pathOfCommand2 ;
	ss << "\"" << pathOfInputFile3 << "\"";
	ss << "\"";
	std::cout << endl;
	system(ss.str().c_str());
	std::cout << endl;
	fileexist = exists_test3(iconlink);
	if (fileexist == 1) {
		std::cout << "icon file: " << "\'" << iconlink << "\'" << " does exist, created succesfull." << endl << endl;
	}
	else {
		std::cout << "icon file: " << "\'" << iconlink << "\'" << " does not exist. creation failed." << endl << endl;
		return 0;
	}
	fileexist = exists_test3(textlink);
	if (fileexist == 1) {
		std::cout << "cache link: " << "\'" << textlink << "\'" << " does exist, no need for create one." << endl << endl;
		return 0;
	}
	else {
		std::cout << "cache link: " << "\'" << textlink << "\'" << " does not exist. trying to create one." << endl << endl;
	}
	ofstream myfile;
	myfile.open(textlink);
	myfile << iconlink;
	myfile.close();
	fileexist = exists_test3(textlink);
	if (fileexist == 1) {
		std::cout << "cache link: " << "\'" << textlink << "\'" << " does exist, creation is succesfully." << endl << endl;
	}
	else {
		std::cout << "cache link: " << "\'" << textlink << "\'" << " does not exist. creating failed." << endl << endl;
	}
	std::cout << "end of the function is reached." << endl;
	return 1;
}

bool MakeIconffmpegMusic(std::string file1) {
	std::cout << "--------------------------------------" << endl;
	std::cout << "-safe-ffmpeg-wrapper-music" << endl;
	std::cout << "--------------------------------------" << endl;
	bool fileexist = exists_test3(file1);
	if (fileexist == 1) {
		std::cout << "file: " << "\'" << file1 << "\'" << " exist. go on with progress." << endl << endl;
	}
	else {
		std::cout << "file: " << "\'" << file1 << "\'" << " does not exist, cancel mission." << endl << endl;
		return 0;
	}
	std::string makepath = ws2s(ExePath()) + "\\Temp\\";
	std::string tempdirectory = GetFullPath(file1) + "\\";
	std::string driveletter = tempdirectory.substr(0, 1);
	std::string cachedir = tempdirectory.substr(2);
	std::string totalcachepath = makepath + driveletter + cachedir;
	std::cout << "cache path: " << "\'" << totalcachepath << "\'" << endl << endl;
	std::string file2 = totalcachepath + GetFileName(file1) + GetFileExtension(file1);
	std::string textlink = file2 + ".png.txt";
	std::string iconlink = file2 + ".png";
	fileexist = exists_test3(iconlink);
	if (fileexist == 1) {
		std::cout << "icon file: " << "\'" << iconlink << "\'" << " does exist, aborting mission." << endl << endl;
		return 0;
	}
	else {
		std::cout << "icon file: " << "\'" << iconlink << "\'" << " does not exist. trying to create one." << endl << endl;
	}
	std::stringstream ss;
	std::string pathOfInputFile1 = ws2s(ExePath()) + "\\ffmpeg.exe";
	std::string pathOfCommand1 = " -i ";
	std::string pathOfInputFile2 = file1;
	std::string pathOfCommand2 = " -an -vcodec copy ";
	std::string pathOfInputFile3 = iconlink;
	ss << "\"";
	ss << "\"" << pathOfInputFile1 << "\"";
	ss << pathOfCommand1;
	ss << "\"" << pathOfInputFile2 << "\"";
	ss << pathOfCommand2;
	ss << "\"" << pathOfInputFile3 << "\"";
	ss << "\"";
	std::cout << endl;
	system(ss.str().c_str());
	std::cout << endl;
	fileexist = exists_test3(iconlink);
	if (fileexist == 1) {
		std::cout << "icon file: " << "\'" << iconlink << "\'" << " does exist, created succesfull." << endl << endl;
	}
	else {
		std::cout << "icon file: " << "\'" << iconlink << "\'" << " does not exist. creation failed." << endl << endl;
		return 0;
	}
	fileexist = exists_test3(textlink);
	if (fileexist == 1) {
		std::cout << "cache link: " << "\'" << textlink << "\'" << " does exist, no need for create one." << endl << endl;
		return 0;
	}
	else {
		std::cout << "cache link: " << "\'" << textlink << "\'" << " does not exist. trying to create one." << endl << endl;
	}
	ofstream myfile;
	myfile.open(textlink);
	myfile << iconlink;
	myfile.close();
	fileexist = exists_test3(textlink);
	if (fileexist == 1) {
		std::cout << "cache link: " << "\'" << textlink << "\'" << " does exist, creation is succesfully." << endl << endl;
	}
	else {
		std::cout << "cache link: " << "\'" << textlink << "\'" << " does not exist. creating failed." << endl << endl;
	}
	std::cout << "end of the function is reached." << endl;
	return 1;
}

bool MakeIconffmpegPicture(std::string file1) {
	std::cout << "--------------------------------------" << endl;
	std::cout << "-safe-ffmpeg-wrapper-picture" << endl;
	std::cout << "--------------------------------------" << endl;
	bool fileexist = exists_test3(file1);
	if (fileexist == 1) {
		std::cout << "file: " << "\'" << file1 << "\'" << " exist. go on with progress." << endl << endl;
	}
	else {
		std::cout << "file: " << "\'" << file1 << "\'" << " does not exist, cancel mission." << endl << endl;
		return 0;
	}
	std::string makepath = ws2s(ExePath()) + "\\Temp\\";
	std::string tempdirectory = GetFullPath(file1) + "\\";
	std::string driveletter = tempdirectory.substr(0, 1);
	std::string cachedir = tempdirectory.substr(2);
	std::string totalcachepath = makepath + driveletter + cachedir;
	std::cout << "cache path: " << "\'" << totalcachepath << "\'" << endl << endl;
	std::string file2 = totalcachepath + GetFileName(file1) + GetFileExtension(file1);
	std::string textlink = file2 + ".png.txt";
	std::string iconlink = file2 + ".png";
	fileexist = exists_test3(iconlink);
	if (fileexist == 1) {
		std::cout << "icon file: " << "\'" << iconlink << "\'" << " does exist, aborting mission." << endl << endl;
		return 0;
	}
	else {
		std::cout << "icon file: " << "\'" << iconlink << "\'" << " does not exist. trying to create one." << endl << endl;
	}
	std::stringstream ss;
	std::string pathOfInputFile1 = ws2s(ExePath()) + "\\ffmpeg.exe";
	std::string pathOfCommand1 = " -hide_banner -loglevel error -n -i ";
	std::string pathOfInputFile2 = file1;
	std::string pathOfCommand2 = " -vf scale=256:-1 ";
	std::string pathOfInputFile3 = iconlink;
	ss << "\"";
	ss << "\"" << pathOfInputFile1 << "\"";
	ss << pathOfCommand1;
	ss << "\"" << pathOfInputFile2 << "\"";
	ss << pathOfCommand2;
	ss << "\"" << pathOfInputFile3 << "\"";
	ss << "\"";
	std::cout << endl;
	system(ss.str().c_str());
	std::cout << endl;
	fileexist = exists_test3(iconlink);
	if (fileexist == 1) {
		std::cout << "icon file: " << "\'" << iconlink << "\'" << " does exist, created succesfull." << endl << endl;
	}
	else {
		std::cout << "icon file: " << "\'" << iconlink << "\'" << " does not exist. creation failed." << endl << endl;
		return 0;
	}
	fileexist = exists_test3(textlink);
	if (fileexist == 1) {
		std::cout << "cache link: " << "\'" << textlink << "\'" << " does exist, no need for create one." << endl << endl;
		return 0;
	}
	else {
		std::cout << "cache link: " << "\'" << textlink << "\'" << " does not exist. trying to create one." << endl << endl;
	}
	ofstream myfile;
	myfile.open(textlink);
	myfile << iconlink;
	myfile.close();
	fileexist = exists_test3(textlink);
	if (fileexist == 1) {
		std::cout << "cache link: " << "\'" << textlink << "\'" << " does exist, creation is succesfully." << endl << endl;
	}
	else {
		std::cout << "cache link: " << "\'" << textlink << "\'" << " does not exist. creating failed." << endl << endl;
	}
	std::cout << "end of the function is reached." << endl;
	return 1;
}

bool MakeDirectorys(std::string file1) {
	std::cout << "--------------------------------------" << endl;
	std::cout << "-create-cache-path" << endl;
	std::cout << "--------------------------------------" << endl;
	std::cout << "main folder: " << "\'" << file1 << "\'" << endl;
	std::string makepath = ws2s(ExePath()) + "\\Temp\\";
	std::string makepath2 = ws2s(ExePath()) + "\\Temp Icons\\";
	bool nothing;
	nothing = _mkdir(makepath.c_str());
	nothing = _mkdir(makepath2.c_str());
	std::cout << "temp path: " << makepath << endl;
	std::string tempdirectory = file1;
	std::string driveletter = tempdirectory.substr(0, 1);
	std::cout << "drive: " << driveletter << endl;
	std::string cachedir = tempdirectory.substr(2);
	std::cout << "cache dir: " << "\'" << cachedir << "\'" << endl;
	std::string totalcachepath = makepath + driveletter + cachedir;
	std::cout << "total cache path: " << "\'" << totalcachepath << "\'" << endl;
	for (size_t n = 4; n < totalcachepath.length(); n++) {
		if (totalcachepath.substr(n, 1) == "\\") {
			std::cout << totalcachepath.substr(0, n) << endl;
			std::cout << "dir made: " << _mkdir(totalcachepath.substr(0, n).c_str()) << endl;
		}
	}
	std::cout << "the end of the function make directorys reached." << endl;
	return 1;
}

bool MakeSubdirectorys(std::string file1) {
	std::cout << "--------------------------------------" << endl;
	std::cout << "-create-cache-subfolders" << endl;
	std::cout << "--------------------------------------" << endl;
	std::cout << "folder: " << "\'" << file1 << "\'" << endl;
	std::string makepath = ws2s(ExePath());
	std::cout << "temp directory: " << "\'" << makepath << "\\Temp\\" << "\'" << endl;
	std::string tempdirectory = file1;
	std::string driveletter = tempdirectory.substr(0, 1);
	std::cout << "drive: " << driveletter << endl;
	std::string cachedir = tempdirectory.substr(2);
	std::cout << "cache dir: " << cachedir << endl;
	std::string textlink = makepath + "\\Temp\\" + driveletter + cachedir + ".png.txt";
	bool fileexist = exists_test3(textlink);
	if (fileexist == 1) {
		std::cout << "icon link: " << "\'" << textlink << "\'" << " already exist. cancel mission." << endl << endl;
		return 0;
	}
	else {
		std::cout << "icon link: " << "\'" << textlink << "\'" << " does not exist, trying to create one." << endl << endl;
	}
	std::string iconlink = makepath + "\\folder.png";
	fileexist = exists_test3(iconlink);
	if (fileexist == 1) {
		std::cout << "icon file: " << "\'" << iconlink << "\'" << " exist. go on with progress." << endl << endl;
	}
	else {
		std::cout << "icon file: " << "\'" << iconlink << "\'" << " does not exist, cancel mission." << endl << endl;
		return 0;
	}
	ofstream myfile;
	myfile.open(textlink);
	myfile << iconlink;
	myfile.close();
	std::cout << "the end of the function is reached." << endl;
	return 1;
}

bool MakeSingleIcons(std::string file1) {
	std::cout << "--------------------------------------" << endl;
	std::cout << "-auto-single" << endl;
	std::cout << "--------------------------------------" << endl;
	std::string makepath = ws2s(ExePath()) + "\\Temp\\";
	bool fileexist = exists_test3(file1);
	if (fileexist == 1) {
		std::cout << "icon from: " << "\'" << file1 << "\'" << " exist. go on with progress." << endl;
	}
	else {
		std::cout << "icon from: " << "\'" << file1 << "\'" << " does not exist. cancel mission." << endl;
		return 0;
	}
	std::string tempdirectory = GetFullPath(file1) + "\\";
	std::cout << "directory from: " << "\'" << tempdirectory << "\'" << endl;
	std::string driveletter = tempdirectory.substr(0, 1);
	std::string cachedir = tempdirectory.substr(2);
	std::cout << "drive: " << driveletter << endl;
	std::string totalcachepath = makepath + driveletter + cachedir;
	std::string file2 = totalcachepath + GetFileName(file1) + GetFileExtension(file1);
	fileexist = exists_test3(file2 + ".png");
	if (fileexist == 1) {
		std::cout << "cache icon: " << "\'" << file2 << ".png" << "\'" << " does exist, aborting mission." << endl;
		return 0;
	}
	else {
		std::cout << "cache icon: " << "\'" << file2 << ".png" << "\'" << " does not exist. trying to create one." << endl;
	}
	std::string textlink = file2 + ".png.txt";
	std::string iconlink = file2 + ".png";
	fileexist = exists_test3(textlink);
	if (fileexist == 1) {
		std::cout << "text link: " << "\'" << textlink << "\'" << " does exist, aborting mission." << endl;
		return 0;
	}
	else {
		std::cout << "text link: " << "\'" << textlink << "\'" << " does not exist. trying to create one." << endl;
	}
	ofstream myfile;
	myfile.open(textlink);
	myfile << iconlink;
	myfile.close();
	std::string command2 = "-scale";
	int scale1 = 256;
	Measure* measure = new Measure;
	std::wstring wCachePath = s2ws(GetFullPath(file2));
	measure->linkCache = wCachePath + L"\\";
	measure->iconCache = wCachePath + L"\\";
	measure->iconSize = EXLARGE;
	measure->getIcon = true;
	std::wstring wCacheName = s2ws(GetFileName(file1));
	measure->property.name = wCacheName;
	std::wstring wCacheExtension = s2ws(GetFileExtension(file1));
	measure->property.ext = wCacheExtension;
	measure->property.nameandext = wCacheName + wCacheExtension;
	measure->property.wholepath = L"";
	measure->useNewStyle = 1;
	measure->allowNonstorage = false;
	measure->property.path = s2ws(file1);
	measure->filefrom = s2ws(file1);
	measure->fileto = s2ws(file2);
	measure->scaleto = scale1;
	std::wstring wCachePathFrom = s2ws(GetFullPath(file1));
	measure->fileInitDir = wCachePathFrom + L"\\";
	if ((file1 != "") && (file2 != "")) {
		GetIcon(measure, nullptr);
	}
	else {
		std::cout << "error: type -help for more information." << endl;
	}
	std::cout << "the end of the function is reached." << endl;
	return 1;
}

bool MakeGlobalIcons(std::string file1) {
	std::cout << "--------------------------------------" << endl;
	std::cout << "-auto-global" << endl;
	std::cout << "--------------------------------------" << endl;
	std::string makepath = ws2s(ExePath()) + "\\Temp\\";
	std::cout << "temp path: " << makepath << endl;
	std::string makepath2 = ws2s(ExePath()) + "\\Temp Icons\\";
	std::cout << "temp icon path: " << makepath2 << endl;
	bool fileexist = exists_test3(file1);
	if (fileexist == 1) {
		std::cout << "work file: " << "\'" << file1 << "\'" << " exist . go on with progress." << endl;
	}
	else {
		std::cout << "work file: " << "\'" << file1 << "\'" << " does not exist, cancel progress." << endl;
		return 0;
	}
	std::string tempdirectory = GetFullPath(file1) + "\\";
	std::cout << "work directory: " << tempdirectory << endl;
	std::string driveletter = tempdirectory.substr(0, 1);
	std::cout << "drive: " << driveletter << endl;
	std::string cachedir = tempdirectory.substr(2);
	std::cout << "cache dir: " << cachedir << endl;
	std::string totalcachepath = makepath + driveletter + cachedir;
	std::cout << "total cache dir: " << totalcachepath << endl;
	std::cout << "file extension?: " << GetFileExtension(file1) << endl;
	std::string file2 = makepath2 + GetFileExtension(file1);
	std::cout << "file2 dir: " << file2 << endl;
	std::cout << "the path of the cache file is: " << file2 << ".png" << endl;
	std::string textlink = totalcachepath + GetFileName(file1) + GetFileExtension(file1) + ".png.txt";
	std::string iconlink = makepath2 + GetFileExtension(file1) + ".png";
	fileexist = exists_test3(textlink);
	if (fileexist == 1) {
		std::cout << "cache link does exist, aborting mission." << endl;
		return 0;
	}
	else {
		std::cout << "cache link does not exist. trying to create one." << endl;
	}
	ofstream myfile;
	myfile.open(textlink);
	myfile << iconlink;
	myfile.close();
	fileexist = exists_test3(file2 + ".png");
	if (fileexist == 1) {
		std::cout << "cache icon does exist, aborting mission." << endl;
		return 0;
	}
	else {
		std::cout << "cache icon does not exist. trying to create one." << endl;
	}
	std::string command2 = "-scale";
	int scale1 = 256;
	Measure* measure = new Measure;
	std::wstring wCachePath = s2ws(GetFullPath(file2));
	measure->linkCache = wCachePath + L"\\";
	measure->iconCache = wCachePath + L"\\";
	measure->iconSize = EXLARGE;
	measure->getIcon = true;
	std::wstring wCacheName = s2ws(GetFileName(file1));
	measure->property.name = wCacheName;
	std::wstring wCacheExtension = s2ws(GetFileExtension(file1));
	measure->property.ext = wCacheExtension;
	measure->property.nameandext = wCacheName + wCacheExtension;
	measure->property.wholepath = L"";
	measure->useNewStyle = 1;
	measure->allowNonstorage = false;
	measure->property.path = s2ws(file1);
	measure->filefrom = s2ws(file1);
	measure->fileto = s2ws(file2);
	measure->scaleto = scale1;
	std::wstring wCachePathFrom = s2ws(GetFullPath(file1));
	measure->fileInitDir = wCachePathFrom + L"\\";
	std::cout << "all code was running without error." << endl;
	if ((file1 != "") && (file2 != "")) {
		GetIcon(measure, nullptr);
	}
	else {
		std::cout << "error: type -help for more information.";
	}
	std::cout << "the end of the function is reache." << endl;
	return 1;
}

bool UseExtract(std::string file1, std::string file2, std::string command1, std::string command2, char* vScale) {
	int scale1 = atoi(vScale);
	std::cout << "\n";
	std::cout << "the info you passed.";
	std::cout << "\n------------------------------------------------------------------------";
	std::cout << "\n";
	std::cout << "first command: " + command1;
	std::cout << "\n";
	std::cout << "filename: " + file1 << std::endl;
	std::cout << "save icon to: " + file2;
	std::cout << "\n";
	std::cout << "Second command: " + command2;
	std::cout << "\n";
	std::cout << "icon size is: " + std::to_string(scale1);
	std::cout << "\n------------------------------------------------------------------------";
	std::cout << "\n\n";
	Measure* measure = new Measure;
	std::wstring wCachePath = s2ws(GetFullPath(file2));
	measure->linkCache = wCachePath + L"\\";
	measure->iconCache = wCachePath + L"\\";
	measure->iconSize = EXLARGE;
	measure->getIcon = true;
	std::wstring wCacheName = s2ws(GetFileName(file1));
	measure->property.name = wCacheName;
	std::wstring wCacheExtension = s2ws(GetFileExtension(file1));
	measure->property.ext = wCacheExtension;
	measure->property.nameandext = wCacheName + wCacheExtension;
	measure->property.wholepath = L"";
	measure->useNewStyle = 1;
	measure->allowNonstorage = false;
	measure->property.path = s2ws(file1);
	measure->filefrom = s2ws(file1);
	measure->fileto = s2ws(file2);
	measure->scaleto = scale1;
	std::wstring wCachePathFrom = s2ws(GetFullPath(file1));
	measure->fileInitDir = wCachePathFrom + L"\\";
	if ((file1 != "") && (file2 != "")) {
		GetIcon(measure, nullptr);
	}
	else {
		std::cout << "error: type -help for more information.";
	}
	std::cout << "the end of the function is reached." << endl;
	return 1;
}

// =================================================
// ==== Custom Help Functions
// =================================================

void GetIcon(Measure* measure, std::wstring* filepath)
{
	// Code from FileView.dll Ver.2.0.3.0
	// Code from rainmeter plugin : quicklink.dll Ver.0.9.1.2
	// https://forum.rainmeter.net/viewtopic.php?f=18&t=20773&hilit=quicklink
	std::wstring* filePath;
	std::wstring browser;
	filePath = &(measure->property.path);

	SHFILEINFO sfi;
	HICON icon = nullptr;
	HIMAGELIST* hImageList = nullptr;
	HRESULT hr;

	std::wstring wRegex = L"^.*\\.url$";
	std::regex_constants::syntax_option_type fl = std::regex_constants::icase;
	std::wregex regExpress(wRegex, fl);
	std::wsmatch ms;
	if (std::regex_match(*filePath, ms, regExpress))
	{
		// ".url" files
		WCHAR buffer[MAX_PATH] = L"";
		GetPrivateProfileString(L"InternetShortcut", L"IconFile", L"", buffer, sizeof(buffer), filePath->c_str());
		if (*buffer)
		{
			std::wstring file = buffer;
			int iconIndex = 0;

			GetPrivateProfileString(L"InternetShortcut", L"IconIndex", L"-1", buffer, sizeof(buffer), filePath->c_str());
			if (buffer != L"-1")
				iconIndex = _wtoi(buffer);
			int size = 256;
			//int size = 48;
			//switch (measure->iconSize)
			//{
			//case EXLARGE: size = 256; break;
			//case LARGE: size = 48; break;
			//case MEDIUM: size = 32; break;
			//case SMALL: size = 16; break;
			//}
			PrivateExtractIcons(file.c_str(), iconIndex, size, size, &icon, nullptr, 1, LR_LOADTRANSPARENT);
		}
		else
		{
			WCHAR buffer[MAX_PATH];
			DWORD size = MAX_PATH;
			HKEY hKey;

			RegOpenKeyEx(HKEY_CLASSES_ROOT, L"http\\shell\\open\\command", 0, KEY_QUERY_VALUE, &hKey);
			RegQueryValueEx(hKey, nullptr, nullptr, nullptr, (LPBYTE)buffer, &size);
			RegCloseKey(hKey);
			//Strip quotes
			if (buffer[0] == L'"')
			{
				browser = buffer; browser = browser.substr(1);
				size_t pos = browser.find_first_of(L'"');
				browser = browser.substr(0, pos);
			}
			filePath = &browser;
		}
	}
	if (icon == nullptr)
	{
		CoInitialize(NULL);
		SHGetFileInfo(filePath->c_str(), 0, &sfi, sizeof(sfi), SHGFI_SYSICONINDEX);
		int size = SHIL_JUMBO;
		//int size = 0;
		//switch (measure->iconSize)
		//{
		//case EXLARGE: size = SHIL_JUMBO; break;
		//case LARGE: size = SHIL_EXTRALARGE; break;
		//case MEDIUM: size = SHIL_LARGE; break;
		//case SMALL: size = SHIL_SMALL; break;
		//}
		hr = SHGetImageList(size, IID_IImageList, (void**)&hImageList);
		if (SUCCEEDED(hr))
		{
			hr = ((IImageList*)hImageList)->GetIcon(sfi.iIcon, ILD_TRANSPARENT, &icon);
			if (SUCCEEDED(hr))
			{
				if (size == SHIL_JUMBO)
				{
					ICONINFO iconInfo = { 0 };
					GetIconInfo(icon, &iconInfo);
					Gdiplus::Bitmap bmpIcon(iconInfo.hbmColor, NULL);
					if (IsIconOnly48(&bmpIcon)) {
						DestroyIcon(icon);
						SHGetImageList(SHIL_JUMBO, IID_IImageList, (void**)&hImageList);		//Get 48X48
						//SHGetImageList(SHIL_EXTRALARGE, IID_IImageList, (void**)&hImageList);		//Get 48X48
						((IImageList*)hImageList)->GetIcon(sfi.iIcon, ILD_TRANSPARENT, &icon);
						DeleteObject(iconInfo.hbmColor);
						DeleteObject(iconInfo.hbmMask);
					}
				}
			}
		}
		CoUninitialize();
	}
	SaveIcon(measure, icon);
	DestroyIcon(icon);
}

bool IsIconOnly48(Gdiplus::Bitmap* bmp)
{
	// Code from rainmeter plugin : quicklink.dll Ver.0.9.1.2
	// https://forum.rainmeter.net/viewtopic.php?f=18&t=20773&hilit=quicklink
	bool is48 = true;
	Gdiplus::Color color, checkColor(255, 0, 0, 0);
	for (int x = 73; x != 256; ++x)//(int x = 49; x != 256; ++x)
	{
		for (int y = 73; y != 256; ++y)//(int y = 49; y != 256; ++y)
		{
			bmp->GetPixel(x, y, &color);
			if (color.GetValue() != checkColor.GetValue())
			{
				is48 = false;
				break;
			}
		}
		if (is48 == false) break;
	}
	return is48;
}

//DONE//DONE//DONE//DONE//DONE//DONE//DONE//DONE//DONE//DONE//DONE//DONE//
bool SaveIcon(Measure* measure, HICON icon) {
	//this code is as cool as it can..........
	// Code from FileView.dll Ver.2.0.3.0
	FILE* fpicon = nullptr;
	std::wstring iconFilePath = measure->fileto + L".png";
	//measure->iconCache + measure->property.name + L"." + measure->property.ext + L".png";
	errno_t error = _wfopen_s(&fpicon, iconFilePath.c_str(), L"wb");
	if (error != 0) {
		fwrite(iconFilePath.c_str(), 1, 1, fpicon);		// Clears previous icon
		fclose(fpicon);
		return false;
	}
	
	ICONINFO iconInfo;
	BITMAP bmColor;
	BITMAP bmMask;
	if (!fpicon || nullptr == icon || !GetIconInfo(icon, &iconInfo) || !GetObject(iconInfo.hbmColor, sizeof(bmColor), &bmColor) || !GetObject(iconInfo.hbmMask, sizeof(bmMask), &bmMask)) {
		fwrite(iconFilePath.c_str(), 1, 1, fpicon);		// Clears previous icon
		fclose(fpicon);
		return false;
	}
	// support only 16/32 bit icon now
	if (bmColor.bmBitsPixel != 16 && bmColor.bmBitsPixel != 32) {
		fwrite(iconFilePath.c_str(), 1, 1, fpicon);		// Clears previous icon
		fclose(fpicon);
		return false;
	}

	HDC dc = GetDC(nullptr);
	BYTE bmiBytes[sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD)];
	BITMAPINFO* bmi = (BITMAPINFO*)bmiBytes;

	// color bits
	memset(bmi, 0, sizeof(BITMAPINFO));
	bmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	GetDIBits(dc, iconInfo.hbmColor, 0, bmColor.bmHeight, nullptr, bmi, DIB_RGB_COLORS);
	int colorBytesCount = bmi->bmiHeader.biSizeImage;
	BYTE* colorBits = new BYTE[colorBytesCount];
	GetDIBits(dc, iconInfo.hbmColor, 0, bmColor.bmHeight, colorBits, bmi, DIB_RGB_COLORS);

	// mask bits
	memset(bmi, 0, sizeof(BITMAPINFO));
	bmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	GetDIBits(dc, iconInfo.hbmMask, 0, bmMask.bmHeight, nullptr, bmi, DIB_RGB_COLORS);
	int maskBytesCount = bmi->bmiHeader.biSizeImage;
	BYTE* maskBits = new BYTE[maskBytesCount];
	GetDIBits(dc, iconInfo.hbmMask, 0, bmMask.bmHeight, maskBits, bmi, DIB_RGB_COLORS);
	ReleaseDC(nullptr, dc);

	// icon data
	BITMAPINFOHEADER bmihIcon;
	memset(&bmihIcon, 0, sizeof(bmihIcon));
	bmihIcon.biSize = sizeof(BITMAPINFOHEADER);
	bmihIcon.biWidth = bmColor.bmWidth;
	bmihIcon.biHeight = bmColor.bmHeight * 2;	// icXOR + icAND
	bmihIcon.biPlanes = bmColor.bmPlanes;
	bmihIcon.biBitCount = bmColor.bmBitsPixel;
	bmihIcon.biSizeImage = colorBytesCount + maskBytesCount;

	// icon header
	ICONDIR dir;
	dir.idReserved = 0;		// must be 0
	dir.idType = 1;			// 1 for icons
	dir.idCount = 1;
	dir.idEntries[0].bWidth = (BYTE)bmColor.bmWidth;
	dir.idEntries[0].bHeight = (BYTE)bmColor.bmHeight;
	dir.idEntries[0].bColorCount = 0;	// 0 if >= 8bpp
	dir.idEntries[0].bReserved = 0;		// must be 0
	dir.idEntries[0].wPlanes = bmColor.bmPlanes;
	dir.idEntries[0].wBitCount = bmColor.bmBitsPixel;
	dir.idEntries[0].dwBytesInRes = sizeof(bmihIcon) + bmihIcon.biSizeImage;
	dir.idEntries[0].dwImageOffset = sizeof(ICONDIR);

	fwrite(&dir, sizeof(dir), 1, fpicon);
	fwrite(&bmihIcon, sizeof(bmihIcon), 1, fpicon);
	fwrite(colorBits, colorBytesCount, 1, fpicon);
	fwrite(maskBits, maskBytesCount, 1, fpicon);

	// Clean up
	DeleteObject(iconInfo.hbmColor);
	DeleteObject(iconInfo.hbmMask);
	delete[] colorBits;
	delete[] maskBits;

	fclose(fpicon);
	measure->property.icon = iconFilePath;
	return true;
}