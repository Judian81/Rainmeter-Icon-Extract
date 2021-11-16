#include <Windows.h>
#include <vector>
#include <regex>
#include <shlobj_core.h>
#include <shellapi.h>
#include <commoncontrols.h>
#include <gdiplus.h>
#include <fstream>
#include <string>
#include <io.h>
#include <msi.h>
#include <thread>
#pragma comment(lib,"Msi.lib")
#pragma comment(lib, "gdiplus.lib")

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

// Custom Functions Declare

//void BangReplaceAndExecute(Measure* measure, size_t numCommand);
//bool ChooseFile(Measure* measure, size_t argNum);
//bool ChooseFolder(Measure* measure, size_t argNum);
//bool ChooseImage(Measure* measure, size_t argNum);
//bool ResolvePath(Measure* measure, LPCWSTR path);
//bool ResolveLink(Measure* measure, bool* iffolder);
//bool ResolveUrl(Measure* measure);
//void GetFileInfoFromPath(Measure* measure, bool* iffolder);
//void CopyLink(Measure* measure);
//void GetIcon(Measure* measure, std::wstring* filepath);
//bool SaveIcon(Measure* measure, HICON icon);

// Custom Functions Declare

bool IsIconOnly48(Gdiplus::Bitmap* bmp);
public bool GetAndSaveIcon(std::wstring* filepath);

//---------------------------main---------------------------

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

public bool GetAndSaveIcon(std::wstring* filepath)
{
	//get icon//////////////////////////////////
	std::wstring* filePath;
	std::wstring browser;
	if (filepath != nullptr) {
		filePath = filepath;
	}
	else {
		return false;
	}

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
			int size = 48;

			size = 256;
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
		int size = 0;
		size = SHIL_JUMBO;
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
					if (IsIconOnly48(&bmpIcon))
					{
						DestroyIcon(icon);
						SHGetImageList(SHIL_EXTRALARGE, IID_IImageList, (void**)&hImageList);		//Get 48X48
						((IImageList*)hImageList)->GetIcon(sfi.iIcon, ILD_TRANSPARENT, &icon);
						DeleteObject(iconInfo.hbmColor);
						DeleteObject(iconInfo.hbmMask);
					}
				}
			}
		}
		CoUninitialize();
	}


	////////////////////////////////////////////////////////////
	// Code from FileView.dll Ver.2.0.3.0
	FILE* fpicon = nullptr;
	//std::wstring iconFilePath = measure->iconCache + measure->property.name + L"_" + measure->property.ext + L".png";
	std::wstring iconFilePath = filepath[0] + L".png";
	errno_t error = _wfopen_s(&fpicon, iconFilePath.c_str(), L"wb");
	if (error != 0) {
		fwrite(iconFilePath.c_str(), 1, 1, fpicon);		// Clears previous icon
		fclose(fpicon);
		return false;
	}

	ICONINFO iconInfo;
	BITMAP bmColor;
	BITMAP bmMask;
	if (!fpicon || nullptr == icon || !GetIconInfo(icon, &iconInfo) ||
		!GetObject(iconInfo.hbmColor, sizeof(bmColor), &bmColor) ||
		!GetObject(iconInfo.hbmMask, sizeof(bmMask), &bmMask)) {
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
	//we do not need to pass the icon back to nothing
	//measure->property.icon = iconFilePath;
	return true;
}
