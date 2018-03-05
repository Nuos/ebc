
#include "stdafx.h"
#include "DesktopWindow.h"

CDesktopWindow::CDesktopWindow(void)
: m_hDesktopWindow(NULL)
, m_hScreenDC(NULL)
, m_hMemDC(NULL)
, m_hCurrentBitmap(NULL)
, m_pScreenData(NULL)

{
	m_sizeScreen.cx = 0;
	m_sizeScreen.cy = 0;
	memset(&m_biHeader, 0, sizeof(BITMAPINFOHEADER));
}

CDesktopWindow::~CDesktopWindow(void)
{
	Release();
}

bool CDesktopWindow::Create(int nLimitWith)
{
	if (IsValid())
		return true;

	//int width = GetSystemMetrics ( SM_CXSCREEN ); 
	//int height= GetSystemMetrics ( SM_CYSCREEN ); 
	//CString sText;
	//sText.Format(_T("111 width=%d height=%d"),width,height);
	//AfxMessageBox(sText);

	//int m_nScreenWidth = GetSystemMetrics(SM_CXSCREEN); //��Ļ���   
	//int m_nScreenHeight = GetSystemMetrics(SM_CYFULLSCREEN);
	//m_nScreenHeight += GetSystemMetrics(SM_CYCAPTION);
	//sText.Format(_T("222 width=%d height=%d"),width,height);
	//AfxMessageBox(sText);

	//HDC hdcScreen = GetDC( NULL ); 
	//int cx = GetDeviceCaps( hdcScreen, HORZRES ); 
	//int cy = GetDeviceCaps( hdcScreen, VERTRES ); 
	//sText.Format(_T("333 width=%d height=%d"),cx,cy);
	//AfxMessageBox(sText);

	m_hDesktopWindow = ::GetDesktopWindow();//�����Ļ��HWND.
	m_hScreenDC = ::GetDC(m_hDesktopWindow);   //�����Ļ��HDC.
	m_hMemDC = ::CreateCompatibleDC(m_hScreenDC);
	if (m_hMemDC == NULL)
		return false;
	RECT rect; 
	::GetWindowRect(m_hDesktopWindow, &rect);
	m_sizeScreen.cx = rect.right-rect.left;
	m_sizeScreen.cy = rect.bottom-rect.top;
	if ((m_sizeScreen.cx%2)==1)
		m_sizeScreen.cx -= 1;
	if ((m_sizeScreen.cy%2)==1)
		m_sizeScreen.cy -= 1;
	m_sizeReal.cx = m_sizeScreen.cx;
	m_sizeReal.cy = m_sizeScreen.cy;

	DEVMODE pDevMode;
	memset(&pDevMode,0,sizeof(pDevMode));
	if (EnumDisplaySettings(NULL,ENUM_CURRENT_SETTINGS,&pDevMode))
	{
		const double cx = pDevMode.dmPelsWidth;
		const double cy = pDevMode.dmPelsHeight;
		if (cx>m_sizeScreen.cx && cy>m_sizeScreen.cy)
		{
			m_sizeReal.cx = cx;
			m_sizeReal.cy = cy;
			//const double dx = cx/(double)m_sizeScreen.cx;
			//double dy = cy/(double)m_sizeScreen.cy;
			//if (dx>1.95 && dx<2.05)
			//{
			//	m_sizeScreen.cx = cx/2;
			//	m_sizeScreen.cy = cy/2;
			//	m_sizeReal.cx = cx;
			//	m_sizeReal.cy = cy;
			//}
			//CString sText;
			//sText.Format(_T("%f,width=%d height=%d"),dx,m_sizeReal.cx,m_sizeReal.cy);
			//AfxMessageBox(sText);
		}
	}

	int nBitmapWidth = nLimitWith==0?m_sizeScreen.cx:nLimitWith;
	if ((nBitmapWidth%2)==1)
		nBitmapWidth -= 1;
	int nBitmapHeight = nLimitWith==0?m_sizeScreen.cy:((nBitmapWidth*m_sizeScreen.cy)/m_sizeScreen.cx);
	if ((nBitmapHeight%2)==1)
		nBitmapHeight -= 1;

	m_hCurrentBitmap = ::CreateCompatibleBitmap(m_hScreenDC, nBitmapWidth, nBitmapHeight);
	if (IsValid())
	{
		WORD wBitCount = 0;
		int iBits = GetDeviceCaps(m_hScreenDC, BITSPIXEL) * GetDeviceCaps(m_hScreenDC, PLANES);
		if (iBits <= 1)
			wBitCount = 1;
		else if (iBits <= 4)
			wBitCount = 4;
		else if (iBits <= 8)
			wBitCount = 8;
		else if (iBits <= 16)	// add by HD
			wBitCount = 16;
		else if (iBits <= 24)
			wBitCount = 24;
		else
			wBitCount = 32;

		//CString str;
		//str.Format(_T("iBits = %d"), iBits);
		//MessageBox(NULL, str, NULL, MB_OK);

		m_biHeader.biSize            = sizeof(BITMAPINFOHEADER);
		m_biHeader.biWidth           = nBitmapWidth;
		m_biHeader.biHeight          = nBitmapHeight;
		m_biHeader.biPlanes          = 1;
		m_biHeader.biBitCount         = wBitCount;
		m_biHeader.biCompression      = BI_RGB;
		m_biHeader.biSizeImage        = 0;
		m_biHeader.biXPelsPerMeter     = 0;
		m_biHeader.biYPelsPerMeter     = 0;
		m_biHeader.biClrUsed         = 0;
		m_biHeader.biClrImportant      = 0;
	}

	return IsValid();
}

bool CDesktopWindow::IsValid(void) const
{
	return m_hCurrentBitmap != NULL;
}

bool CDesktopWindow::IsInvalid(void) const
{
	return m_hCurrentBitmap == NULL;
}

void CDesktopWindow::Release(void)
{
	if ( m_hCurrentBitmap )
	{
		::DeleteObject(m_hCurrentBitmap);
		m_hCurrentBitmap = NULL;
	}

	if (m_hMemDC)
	{
		::DeleteObject(m_hMemDC);
		m_hMemDC = NULL;
	}

	if (m_hDesktopWindow && m_hScreenDC)
	{
		::ReleaseDC(m_hDesktopWindow, m_hScreenDC);
	}
	m_hDesktopWindow = NULL;
	m_hScreenDC = NULL;

	if (m_pScreenData)
	{
		delete[] m_pScreenData;
		m_pScreenData = NULL;
	}
}

const HBITMAP & CDesktopWindow::CopyScreen(void) const
{
	if (IsValid())
	{
		HGDIOBJ hOldBMP = ::SelectObject(m_hMemDC, m_hCurrentBitmap);
		if (m_biHeader.biWidth==m_sizeScreen.cx)
		{
			if (m_sizeReal.cx>m_sizeScreen.cx && m_sizeReal.cy>m_sizeScreen.cy)
				::StretchBlt(m_hMemDC, 0, 0, m_biHeader.biWidth,m_biHeader.biHeight,m_hScreenDC,0,0,m_sizeReal.cx,m_sizeReal.cy,SRCCOPY);
			else
				::BitBlt(m_hMemDC, 0, 0, m_biHeader.biWidth, m_biHeader.biHeight, m_hScreenDC, 0, 0, SRCCOPY);
		}else
		{
			::StretchBlt(m_hMemDC, 0, 0, m_biHeader.biWidth,m_biHeader.biHeight,m_hScreenDC,0,0,m_sizeReal.cx,m_sizeReal.cy,SRCCOPY);
		}
		::SelectObject(m_hMemDC,hOldBMP);
	}

	return m_hCurrentBitmap;
}

const char * CDesktopWindow::GetScreenData(void)
{
	if (CopyScreen() == NULL)
		return NULL;

	if (m_pScreenData == NULL)
	{
		m_pScreenData = new char[GetBitmapSize()];
	}

	//HDC     hDC;         
	//HPALETTE     hOldPal=NULL;

	//// �����ɫ��   
	//HANDLE hPal = GetStockObject(DEFAULT_PALETTE);
	//if (hPal)
	//{
	//	hDC = ::GetDC(NULL);
	//	hOldPal=SelectPalette(hDC,(HPALETTE)hPal,FALSE);
	//	RealizePalette(hDC);
	//}

	if(0==GetDIBits(m_hScreenDC, m_hCurrentBitmap, 0, GetHeight(), m_pScreenData, (BITMAPINFO *)&m_biHeader, DIB_RGB_COLORS))
	{
		DWORD error = GetLastError();
	}
	//
	////�ָ���ɫ��   
	//if (hOldPal)
	//{
	//	SelectPalette(hDC, hOldPal, TRUE);
	//	RealizePalette(hDC);
	//	::ReleaseDC(NULL, hDC);
	//}

	return m_pScreenData;
}

bool CDesktopWindow::SaveToFile(const char* filename, const unsigned char * pRGBBuffer, int width,int height,int bpp)
{
	BITMAPFILEHEADER bmpheader;
	BITMAPINFO bmpinfo;
	FILE *fp;

	fp = fopen(filename, "wb");
	if(!fp)return false;

	bmpheader.bfType = ('M'<<8)|'B';
	bmpheader.bfReserved1 = 0;
	bmpheader.bfReserved2 = 0;
	bmpheader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	bmpheader.bfSize = bmpheader.bfOffBits + width*height*bpp/8;

	bmpinfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmpinfo.bmiHeader.biWidth = width;
	bmpinfo.bmiHeader.biHeight = height;
	bmpinfo.bmiHeader.biPlanes = 1;
	bmpinfo.bmiHeader.biBitCount = bpp;
	bmpinfo.bmiHeader.biCompression = BI_RGB;
	bmpinfo.bmiHeader.biSizeImage = 0;
	bmpinfo.bmiHeader.biXPelsPerMeter = 0;//100;
	bmpinfo.bmiHeader.biYPelsPerMeter = 0;//100;
	bmpinfo.bmiHeader.biClrUsed = 0;
	bmpinfo.bmiHeader.biClrImportant = 0;

	fwrite(&bmpheader,sizeof(BITMAPFILEHEADER),1,fp);
	fwrite(&bmpinfo.bmiHeader,sizeof(BITMAPINFOHEADER),1,fp);
	fwrite(pRGBBuffer,width*height*bpp/8,1,fp);
	fclose(fp);

	return true;
}

bool CDesktopWindow::SaveToFile(const HBITMAP & hBitmap, int wBitCount, const char * lpFileName) 
{   
	HDC     hDC;         
	//�豸������
	//int     iBits;      
	//��ǰ��ʾ�ֱ�����ÿ��������ռbitλ��
	//WORD    wBitCount;   
	//λͼ��ÿ��������ռbitλ��
	//�����ɫ���С�� λͼ�������ֽڴ�С �� λͼ�ļ���С �� д���ļ��ֽ���
	DWORD           dwPaletteSize=0,dwBmBitsSize,dwDIBSize, dwWritten;
	BITMAP          Bitmap;        
	//λͼ���Խṹ
	BITMAPFILEHEADER   bmfHdr;        
	//λͼ�ļ�ͷ�ṹ
	BITMAPINFOHEADER   bi;            
	//λͼ��Ϣͷ�ṹ 
	LPBITMAPINFOHEADER lpbi;          
	//ָ��λͼ��Ϣͷ�ṹ
	HANDLE          fh, hDib, hPal;
	HPALETTE     hOldPal=NULL;
	//�����ļ��������ڴ�������ɫ����

	//����λͼ�ļ�ÿ��������ռbitλ��
	//hDC = CreateDC("DISPLAY",NULL,NULL,NULL);
	//iBits = GetDeviceCaps(hDC, BITSPIXEL) * 
	//	GetDeviceCaps(hDC, PLANES);
	//DeleteDC(hDC);
	//if (iBits <= 1)
	//	wBitCount = 1;
	//else if (iBits <= 4)
	//	wBitCount = 4;
	//else if (iBits <= 8)
	//	wBitCount = 8;
	//else if (iBits <= 24)
	//	wBitCount = 24;
	//else
	//	wBitCount = 32;
	//�����ɫ���С
	if (wBitCount <= 8)
		dwPaletteSize=(1<<wBitCount)*sizeof(RGBQUAD);

	//����λͼ��Ϣͷ�ṹ
	GetObject(hBitmap, sizeof(BITMAP), (LPSTR)&Bitmap);
	bi.biSize            = sizeof(BITMAPINFOHEADER);
	bi.biWidth           = Bitmap.bmWidth;
	bi.biHeight          = Bitmap.bmHeight;
	bi.biPlanes          = 1;
	bi.biBitCount         = wBitCount;
	bi.biCompression      = BI_RGB;
	bi.biSizeImage        = 0;
	bi.biXPelsPerMeter     = 0;
	bi.biYPelsPerMeter     = 0;
	bi.biClrUsed         = 0;
	bi.biClrImportant      = 0;

	dwBmBitsSize = Bitmap.bmWidth*Bitmap.bmHeight*((wBitCount)/8);
	//Ϊλͼ���ݷ����ڴ�

	/*xxxxxxxx����λͼ��С�ֽ�һ��(����һ����������)xxxxxxxxxxxxxxxxxxxx 
	//ÿ��ɨ������ռ���ֽ���Ӧ��Ϊ4���������������㷨Ϊ:
	int biWidth = (Bitmap.bmWidth*wBitCount) / 32;
	if((Bitmap.bmWidth*wBitCount) % 32)
	biWidth++; //�����������ļ�1
	biWidth *= 4;//���������õ���Ϊÿ��ɨ���е��ֽ�����
	dwBmBitsSize = biWidth * Bitmap.bmHeight;//�õ���С
	xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx*/

	hDib = GlobalAlloc(GHND,dwBmBitsSize+dwPaletteSize+sizeof(BITMAPINFOHEADER));
	lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDib);
	*lpbi = bi;
	// �����ɫ��   
	hPal = GetStockObject(DEFAULT_PALETTE);
	if (hPal)
	{
		hDC = ::GetDC(NULL);
		hOldPal=SelectPalette(hDC,(HPALETTE)hPal,FALSE);
		RealizePalette(hDC);
	}
	// ��ȡ�õ�ɫ�����µ�����ֵ
	if(0==GetDIBits(hDC,hBitmap,0,(UINT)Bitmap.bmHeight,(LPSTR)lpbi+sizeof(BITMAPINFOHEADER)+dwPaletteSize,
		(BITMAPINFO *)&bi,DIB_RGB_COLORS))
	{
		//Quit();
		//PostQuitMessage(0);
		return false;
	}
	//�ָ���ɫ��   
	if (hOldPal)
	{
		SelectPalette(hDC, hOldPal, TRUE);
		RealizePalette(hDC);
		::ReleaseDC(NULL, hDC);
	}
	//����λͼ�ļ�    
	fh=CreateFileA(lpFileName, GENERIC_WRITE,0, NULL, CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if (fh==INVALID_HANDLE_VALUE)
		return false;
	// ����λͼ�ļ�ͷ
	bmfHdr.bfType = 0x4D42; // "BM"
	dwDIBSize=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+dwPaletteSize+dwBmBitsSize; 
	bmfHdr.bfSize = dwDIBSize;
	bmfHdr.bfReserved1 = 0;
	bmfHdr.bfReserved2 = 0;
	bmfHdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER)+(DWORD)sizeof(BITMAPINFOHEADER)+dwPaletteSize;
	// д��λͼ�ļ�ͷ
	WriteFile(fh, (LPSTR)&bmfHdr, sizeof(BITMAPFILEHEADER), &dwWritten, NULL);
	// д��λͼ�ļ���������
	WriteFile(fh, (LPSTR)lpbi, sizeof(BITMAPINFOHEADER)+dwPaletteSize+dwBmBitsSize , &dwWritten, NULL); 
	//���   
	GlobalUnlock(hDib);
	GlobalFree(hDib);
	CloseHandle(fh);
	return TRUE;
}

bool CDesktopWindow::SaveToFile(const char * fileName)
{
	return IsValid() ? CDesktopWindow::SaveToFile(fileName, (const unsigned char*)GetScreenData(), GetWidth(), GetHeight(), m_biHeader.biBitCount) : false;
	//return IsValid() ? CDesktopWindow::SaveToFile(fileName, (const unsigned char*)GetScreenData(), GetWidth(), GetHeight(), m_biHeader.biBitCount) : false;
}

UINT CDesktopWindow::GetBitmapSize(void) const
{
	return m_biHeader.biWidth*m_biHeader.biHeight*(m_biHeader.biBitCount/8);
}

UINT CDesktopWindow::GetWidth(void) const
{
	return m_biHeader.biWidth;
}

UINT CDesktopWindow::GetHeight(void) const
{
	return m_biHeader.biHeight;
}

UINT CDesktopWindow::GetBitCount(void) const
{
	return m_biHeader.biBitCount;
}
