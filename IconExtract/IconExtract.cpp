#include <iostream>

using namespace std;

#include <Windows.h>
#include <string>
#include <vector>
#include <regex>
#include <shlobj_core.h>
#include <shellapi.h>
#include <commoncontrols.h>
#include <gdiplus.h>
#include <fstream>
#include <io.h>
#include <msi.h>
#pragma comment(lib,"Msi.lib")
#pragma comment(lib, "gdiplus.lib")
//#include <thread>

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

int MsgBox(LPCWSTR toshow);
std::string GetFileExtension(const std::string& FileName);
std::string GetFileName(const string& s);
std::string GetFullPath(const std::string& fname);

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

std::string GetFileExtension(const std::string& FileName)
{
	if (FileName.find_last_of(".") != std::string::npos)
		return FileName.substr(FileName.find_last_of(".") + 1);
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
}

std::string GetFullPath(const std::string& fname)
{
	size_t pos = fname.find_last_of("\\/");
	return (std::string::npos == pos)
		? ""
		: fname.substr(0, pos);
}

int main(int argc, char* argv[], char* envp[]) {

	std::string command1 = argv[1];

	if (command1 == "-help") {

		std::cout << "\n";
		std::cout << "type -extract \"drive:path\\filenamefrom.extension\" \"drive:\\cachepath\\fileto.extension\" -scale 256";
		std::cout << "\n";
		std::cout << "this wil make it possible to extract the icon from filenamefrom.";
		std::cout << "\n";
		std::cout << "the icon from filenamefrom wil be saved in the filenameto location as cache icon.";
		std::cout << "\n";
		std::cout << "the -scale can be from 16, 32, 48 to 256. sorry 64 and 128 is not possible.";
		std::cout << "if you fill in 64 it will be 48. and if you fill in 128 it will be automaticly 256.";
		std::cout << "\n";
	}
	std::string file1 = argv[2];
	std::string file2 = argv[3];
	std::string command2 = argv[4];
	std::string scale1 = argv[5];

	if (command1 == "-extract") {

		std::cout << "\n";
		std::cout << "the info you passed.";
		std::cout << "\n------------------------------------------------------------------------";
		std::cout << "\n";
		std::cout << "first command: " + command1;
		std::cout << "\n";
		std::cout << "icon from: " + file1;
		std::cout << "\n";
		std::cout << "save icon to: " + file2;
		std::cout << "\n";
		std::cout << "Second command: " + command2;
		std::cout << "\n";
		std::cout << "icon size is: " + scale1;
		std::cout << "\n------------------------------------------------------------------------";
		std::cout << "\n\n";

		//convert data to what we need to have.
		//-----------------------------------------------------------------------
		std::wstring filetemp1(file1.begin(), file1.end());
		std::wstring filetemp2(file2.begin(), file2.end());
		int scaletemp1 = atoi(scale1.c_str());
		////-----------------------------------------------------------------------

		std::cout << "\n";
		std::cout << " all the info we still have afther converting information";
		std::cout << "\n------------------------------------------------------------------------";
		std::cout << "\n";
		std::wcout << L"icon from: " + filetemp1;
		std::cout << "\n";
		std::wcout << L"icon to: " + filetemp2;
		std::cout << "\n";
		std::cout << "scale size: " + std::to_string(scaletemp1);
		std::cout << "\n------------------------------------------------------------------------";
		std::cout << "\n\n";

		//make pointers to the info we have.
		//-----------------------------------------------------------------------
		std::wstring filefrom = filetemp1;
		std::wstring fileto = filetemp2;
		int* scaleto = &scaletemp1;
		//-----------------------------------------------------------------------

		std::cout << "\n";
		std::cout << "---------------------the rest converted--------------------------";
		std::cout << "\n";
		std::wcout << L"filefrom: " + filefrom;
		std::cout << "\n";
		std::wcout << L"fileto: " + fileto;
		std::cout << "\n";
		std::cout << "scale: " + std::to_string(scaletemp1);
		std::cout << "\n";
		std::cout << "---------------------the rest converted ended--------------------------";
		std::cout << "\n";

		//-----------------------------------------------------------------------------
		//we have to store some parrameters. in the hope i can use the code optiomal.
		Measure* measure = new Measure;
		//create the path were to stall the cache icons.
		std::string sCachePath = GetFullPath(file2);
		std::wstring wCachePath(sCachePath.begin(), sCachePath.end());
		measure->iconCache = wCachePath + L"\\";
		//set the icon size as big as we can
		measure->iconSize = EXLARGE;
		//wihtout this there will not be an icon saved at all.
		measure->getIcon = true;
		//put in the file name we need.
		std::string sCacheName = GetFileName(file1);
		std::wstring wCacheName(sCacheName.begin(), sCacheName.end());
		measure->property.name = wCacheName;
		//put in the extension.
		std::string sCacheExtension = GetFileExtension(file1);
		std::wstring wCacheExtension(sCacheExtension.begin(), sCacheExtension.end());
		measure->property.ext = wCacheExtension;
		//set the name and extension in one variable.
		measure->property.nameandext = wCacheName + L"." + wCacheExtension;
		
		//this is the target file location ?!?!??!
		measure->linkCache = wCachePath + L"\\";


		//create the path were to stall the cache icons.
		std::string sCachePathFrom = GetFullPath(file1);
		std::wstring wCachePathFrom(sCachePathFrom.begin(), sCachePathFrom.end());
		measure->fileInitDir = wCachePathFrom + L"\\";


		//measure->getTarget = 1;

		measure->useNewStyle = 1;
		
		
		//this are my own parrameters. i think i am god. hahaha.
		measure->filefrom = filefrom;
		measure->fileto = fileto;
		//i think i can disable this action. it is just for the best.
		measure->scaleto = scaletemp1;


		// can not be used if enabled.
		//i do not know what this is meant for. so i disable it.
		//measure->copyLink = 0;
		measure->allowNonstorage = false;
		//measure->fileInitDir = L"C:\\ProgramData\\Cool\\";
		//measure->imgInitDir = L"C:\\ProgramData\\Cool\\Ow\\";
		measure->property.wholepath = L"";
		//----------------------------------------------------------------------------

		if ((file1 != "") && (file2 != "")) {
			//this is the path from the file we want to extract the icon from.
			measure->property.path = filefrom;
			GetIcon(measure, nullptr);
		} else {
			std::cout << "error: type -help for more information.";
		}
	}
}

// =================================================
// ==== Custom Help Functions
// =================================================

//DONE//DONE//DONE//DONE//DONE//DONE//DONE//DONE//DONE//DONE//DONE//DONE//
void GetIcon(Measure* measure, std::wstring* filepath)
{
	//this is fucking cool. yay. now we come somewhere. boe!
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

//DONE//DONE//DONE//DONE//DONE//DONE//DONE//DONE//DONE//DONE//DONE//DONE//
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