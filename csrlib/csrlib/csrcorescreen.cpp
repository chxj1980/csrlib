#include "csrcorescreen.h"

#include <Gdiplusheaders.h>
#include <gdiplus.h>


int SaveBMPToFile(HBITMAP  hBitmap, CString szfilename, int x, int y)
{
	HDC     hDC;
	//��ǰ�ֱ�����ÿ������ռ�ֽ���            
	int     iBits;
	//λͼ��ÿ������ռ�ֽ���            
	WORD     wBitCount;
	//�����ɫ���С��     λͼ�������ֽڴ�С     ��λͼ�ļ���С     ��     д���ļ��ֽ���                
	DWORD     dwPaletteSize = 0, dwBmBitsSize = 0, dwDIBSize = 0, dwWritten = 0;
	//λͼ���Խṹ                
	BITMAP     Bitmap;
	//λͼ�ļ�ͷ�ṹ            
	BITMAPFILEHEADER     bmfHdr;
	//λͼ��Ϣͷ�ṹ                
	BITMAPINFOHEADER     bi;
	//ָ��λͼ��Ϣͷ�ṹ                    
	LPBITMAPINFOHEADER     lpbi;
	//�����ļ��������ڴ�������ɫ����                
	HANDLE     fh, hDib, hPal, hOldPal = NULL;

	//����λͼ�ļ�ÿ��������ռ�ֽ���                
	hDC = CreateDC(L"DISPLAY", NULL, NULL, NULL);
	iBits = GetDeviceCaps(hDC, BITSPIXEL)*GetDeviceCaps(hDC, PLANES);
	DeleteDC(hDC);
	if (iBits <= 1)
		wBitCount = 1;
	else  if (iBits <= 4)
		wBitCount = 4;
	else if (iBits <= 8)
		wBitCount = 8;
	else
		wBitCount = 24;

	GetObject(hBitmap, sizeof(Bitmap), (LPSTR)&Bitmap);
	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = Bitmap.bmWidth;
	bi.biHeight = Bitmap.bmHeight;
	bi.biPlanes = 1;
	bi.biBitCount = wBitCount;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrImportant = 0;
	bi.biClrUsed = 0;

	dwBmBitsSize = ((Bitmap.bmWidth *wBitCount + 31) / 32) * 4 * Bitmap.bmHeight;

	//Ϊλͼ���ݷ����ڴ�                
	hDib = GlobalAlloc(GHND, dwBmBitsSize + dwPaletteSize + sizeof(BITMAPINFOHEADER));
	lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDib);
	*lpbi = bi;

	//     �����ɫ��                    
	hPal = GetStockObject(DEFAULT_PALETTE);
	if (hPal)
	{
		hDC = ::GetDC(NULL);
		hOldPal = ::SelectPalette(hDC, (HPALETTE)hPal, FALSE);
		RealizePalette(hDC);
	}

	//     ��ȡ�õ�ɫ�����µ�����ֵ                
	GetDIBits(hDC, hBitmap, 0, (UINT)Bitmap.bmHeight,
		(LPSTR)lpbi + sizeof(BITMAPINFOHEADER) + dwPaletteSize,
		(BITMAPINFO *)lpbi, DIB_RGB_COLORS);

	//�ָ���ɫ��                    
	if (hOldPal)
	{
		::SelectPalette(hDC, (HPALETTE)hOldPal, TRUE);
		RealizePalette(hDC);
		::ReleaseDC(NULL, hDC);
	}

	//����λͼ�ļ�                    
	fh = CreateFile(szfilename, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);

	if (fh == INVALID_HANDLE_VALUE)         return     FALSE;

	//     ����λͼ�ļ�ͷ                
	bmfHdr.bfType = 0x4D42;     //     "BM"                
	dwDIBSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + dwPaletteSize + dwBmBitsSize;
	bmfHdr.bfSize = dwDIBSize;
	bmfHdr.bfReserved1 = 0;
	bmfHdr.bfReserved2 = 0;
	bmfHdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER) + dwPaletteSize;
	//     д��λͼ�ļ�ͷ                
	WriteFile(fh, (LPSTR)&bmfHdr, sizeof(BITMAPFILEHEADER), &dwWritten, NULL);
	//     д��λͼ�ļ���������                
	WriteFile(fh, (LPSTR)lpbi, dwDIBSize, &dwWritten, NULL);
	//���                    
	GlobalUnlock(hDib);
	GlobalFree(hDib);
	CloseHandle(fh);
	return  TRUE;
}

//VC�°�HBITMAP����ΪbmpͼƬ   
BOOL  SaveBmp(HBITMAP  hBitmap, CString  FileName)
{
	HDC hDC;
	//��ǰ�ֱ�����ÿ������ռ�ֽ���         
	int iBits;
	//λͼ��ÿ������ռ�ֽ���         
	WORD wBitCount;
	//�����ɫ���С��     λͼ�������ֽڴ�С     ��λͼ�ļ���С     ��     д���ļ��ֽ���             
	DWORD dwPaletteSize = 0, dwBmBitsSize = 0, dwDIBSize = 0, dwWritten = 0;
	//λͼ���Խṹ             
	BITMAP Bitmap;
	//λͼ�ļ�ͷ�ṹ         
	BITMAPFILEHEADER bmfHdr;
	//λͼ��Ϣͷ�ṹ             
	BITMAPINFOHEADER bi;
	//ָ��λͼ��Ϣͷ�ṹ                 
	LPBITMAPINFOHEADER lpbi;
	//�����ļ��������ڴ�������ɫ����             
	HANDLE fh, hDib, hPal, hOldPal = NULL;

	int aa = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + 288 * 352 * 24 / 8;

	//����λͼ�ļ�ÿ��������ռ�ֽ���             
	hDC = CreateDC(L"DISPLAY", NULL, NULL, NULL);
	iBits = GetDeviceCaps(hDC, BITSPIXEL)     *     GetDeviceCaps(hDC, PLANES);
	DeleteDC(hDC);
	if (iBits <= 1)
		wBitCount = 1;
	else  if (iBits <= 4)
		wBitCount = 4;
	else if (iBits <= 8)
		wBitCount = 8;
	else
		wBitCount = 24;

	GetObject(hBitmap, sizeof(Bitmap), (LPSTR)&Bitmap);
	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = Bitmap.bmWidth;
	bi.biHeight = Bitmap.bmHeight;
	bi.biPlanes = 1;
	bi.biBitCount = wBitCount;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = Bitmap.bmWidth * Bitmap.bmHeight * 3;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrImportant = 0;
	bi.biClrUsed = 0;

	dwBmBitsSize = ((Bitmap.bmWidth *wBitCount + 31) / 32) * 4 * Bitmap.bmHeight;

	//Ϊλͼ���ݷ����ڴ�             
	hDib = GlobalAlloc(GHND, dwBmBitsSize + dwPaletteSize + sizeof(BITMAPINFOHEADER));
	lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDib);
	*lpbi = bi;

	//     �����ɫ��                 
	hPal = GetStockObject(DEFAULT_PALETTE);
	if (hPal)
	{
		hDC = ::GetDC(NULL);
		hOldPal = ::SelectPalette(hDC, (HPALETTE)hPal, FALSE);
		RealizePalette(hDC);
	}

	//     ��ȡ�õ�ɫ�����µ�����ֵ             
	GetDIBits(hDC, hBitmap, 0, (UINT)Bitmap.bmHeight,
		(LPSTR)lpbi + sizeof(BITMAPINFOHEADER) + dwPaletteSize,
		(BITMAPINFO *)lpbi, DIB_RGB_COLORS);

	//�ָ���ɫ��                 
	if (hOldPal)
	{
		::SelectPalette(hDC, (HPALETTE)hOldPal, TRUE);
		RealizePalette(hDC);
		::ReleaseDC(NULL, hDC);
	}

	//����λͼ�ļ�                 
	fh = CreateFile(FileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);

	if (fh == INVALID_HANDLE_VALUE)         
		return  FALSE;

	//     ����λͼ�ļ�ͷ             
	bmfHdr.bfType = 0x4D42;     //"BM"             
	dwDIBSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + dwPaletteSize + dwBmBitsSize;
	bmfHdr.bfSize = dwDIBSize;
	bmfHdr.bfReserved1 = 0;
	bmfHdr.bfReserved2 = 0;
	bmfHdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER) + dwPaletteSize;
	//     д��λͼ�ļ�ͷ             
	WriteFile(fh, (LPSTR)&bmfHdr, sizeof(BITMAPFILEHEADER), &dwWritten, NULL);
	//     д��λͼ�ļ���������             
	WriteFile(fh, (LPSTR)lpbi, dwDIBSize - 14, &dwWritten, NULL);
	//���                 
	GlobalUnlock(hDib);
	GlobalFree(hDib);
	CloseHandle(fh);
	return  TRUE;
}

//�����������ر����ļ���·��
int screencut(std::string strPath)
{
	//��ȡ��ĻDC
	HDC hDesktopDC = CreateDC(L"DISPLAY", NULL, NULL, NULL);
	//�ڴ�DC
	HDC hMemoryDC = CreateCompatibleDC(hDesktopDC);
	//�õ���Ļ���
	int nScreenWidth = GetSystemMetrics(SM_CXSCREEN);
	int nScreenHeight = GetSystemMetrics(SM_CYSCREEN);
	//������ĻDC������Ļλͼ
	HBITMAP hCaptureBitmap = CreateCompatibleBitmap(hDesktopDC, nScreenWidth, nScreenHeight);
	//��λͼѡ���ڴ�DC��
	BitBlt(hMemoryDC, 0, 0, nScreenWidth, nScreenHeight, hDesktopDC, 0, 0, SRCCOPY);
	//�õ����λ��
	POINT pt;
	GetCursorPos(&pt);
	//�õ����λ��
	HCURSOR m_hcursor = LoadCursor(NULL, IDC_ARROW);
	DrawIconEx(hMemoryDC, pt.x, pt.y, m_hcursor, 0, 0, 0, NULL, DI_NORMAL | DI_COMPAT);
	SelectObject(hMemoryDC, hCaptureBitmap);
	//char str1[20] = "D:/123.bmp";
	//SaveBMPToFile(hCaptureBitmap, strPath.c_str(), pt.x, pt.y);
	SaveBmp(hCaptureBitmap, (CString)strPath.c_str());
	DeleteDC(hDesktopDC);
	DeleteDC(hMemoryDC);
	DeleteObject(hCaptureBitmap);
	return 0;
}


#if 0
BOOL SaveBitmapToFile(HBITMAP hBitmap, LPCTSTR lpszFileName)
{
	CLSID pngClsid;

	Bitmap bmp(hBitmap, NULL);

	//��ȡBMP�ļ��ı��뷽ʽ(���ϣ����ȡJPEG�ı��뷽ʽ��

	//��ô����һҪ����Ϊ��_TEXT("image/jpeg")������֧�ֵ�ͼƬ��ʽ����)
	int nResult = GetEncoderClsid(_TEXT("image/bmp"), &pngClsid);

	if (nResult >= 0)
	{
		//��������ȡ����ĻͼƬ
		bmp.Save(lpszFileName, &pngClsid);
	}
	else
	{
		return FALSE;
	}

	return TRUE;
}

//@brief ��ȡͼƬ�ļ��ı��뷽ʽ��֧��bmp��jpg��jpeg��gif��tiff��png�ȸ�ʽͼƬ
//@date   1-13-2009  
//@param [in]  format ͼƬ��ʽ ֵ����Ϊ���¼���
//@"image/bmp"
//@"image/jpeg"
//@"image/gif"
//@"image/tiff"
//@"image/png"
//@param [in]  pClsid
//@return  �ɹ��򷵻�ֵ >= 0��ʧ���򷵻�ֵΪ-1
int GetEncoderClsid(const WCHAR* format, CLSID *pClsid)
{
	int nRet = -1;
	ImageCodecInfo* pCodecInfo = NULL;
	UINT nNum = 0, nSize = 0;
	GetImageEncodersSize(&nNum, &nSize);

	if (nSize<0)
	{
		return nRet;
	}

	pCodecInfo = new ImageCodecInfo[nSize];

	if (pCodecInfo == NULL)
	{
		return nRet;
	}

	GetImageEncoders(nNum, nSize, pCodecInfo);

	for (UINT i = 0; i<nNum;i++)
	{
		if (wcscmp(pCodecInfo[i].MimeType, format) == 0)
		{
			*pClsid = pCodecInfo[i].Clsid;
			nRet = i;
			delete[] pCodecInfo;
			return nRet;
		}
		else
		{
			continue;
		}
	}

	delete[] pCodecInfo;
	return nRet;
}
#endif

// ���� SaveBmpToFile
int SaveBmpToFile(const char *filename, BITMAPINFO *info, UINT *bits)
{
	FILE* fp;
	UINT size, //Size of file
		infosize, //Size of bitmap info
		bitsize;  //Size of bitmap pixels
	BITMAPFILEHEADER header;
	if ((fp = fopen(filename, "wb")) == NULL)
		return -1;
	if (info->bmiHeader.biSizeImage == 0)
		bitsize = (info->bmiHeader.biWidth *
			info->bmiHeader.biBitCount + 7) / 8 *
		abs(info->bmiHeader.biHeight);
	else
		bitsize = info->bmiHeader.biSizeImage;

	infosize = sizeof(BITMAPINFO);
	size = sizeof(BITMAPFILEHEADER) + infosize + bitsize;

	header.bfType = 'MB';
	header.bfSize = size;
	header.bfReserved1 = 0;
	header.bfReserved2 = 0;
	header.bfOffBits = sizeof(BITMAPFILEHEADER) + infosize;

	if (fwrite(&header, 1, sizeof(BITMAPFILEHEADER), fp) <
		sizeof(BITMAPFILEHEADER))
	{
		fclose(fp);
		return -1;
	}
}

#if 0
void ImageUtils()//�Ҷȴ���
{
	// TODO: Add your control notification handler code here
	CImage img;
	int Height, Width;
	int r, g, b;
	img.Load("C:\\Documents and Settings\\Administrator\\����\\p1.bmp");
	Height = img.GetHeight(); //��ȡͼƬ�ĸ߶�
	Width = img.GetWidth(); //��ȡͼƬ�Ŀ��
							/*���붯̬�Ķ�ά����*/
	int **bmp;//**
	bmp = new int*[Height];
	for (int k = 0;k<Height;k++)
	{
		bmp[k] = new int[Width];
	}
	for (int i = 0;i<Width;i++)
	{/*��λͼ������ֵ�����ά����*/
		for (int j = 0;j<Height;j++)
		{
			COLORREF col = img.GetPixel(i, j);
			r = GetRValue(col);
			g = GetGValue(col);
			b = GetBValue(col);
			int rgb = int((r + g + b) / 3); //�Ҷȴ���
			bmp[j][i] = rgb;
		}
	}
	/*��ά�����������أ���ʾ*/
	CDC *pDC;
	pDC = GetDC();
	for (int i = 0;i<Height;i++)
	{
		for (int j = 0;j<Width;j++)
		{
			pDC->SetPixel(j, i, RGB(bmp[i][j], bmp[i][j], bmp[i][j]));
		}
	}
	/*����ֵд��data.txt�ļ�*/
	FILE *fp = fopen("data.txt", "w");
	for (int i = 0;i<Height;i++)
	{
		for (int j = 0;j<Width;j++)
		{
			if (j == Width - 1)//?
			{
				fprintf(fp, "%d", bmp[i][j]);
			}
			else
				fprintf(fp, "%d ", bmp[i][j]);
		}
		fprintf(fp, "\n", 1);
	}
	fclose(fp);
	/*�ͷŶ�̬�Ķ�ά����*/
	for (int i = 0;i<Height;i++)
		delete[]bmp[i]; //����Ϊ��λ�ͷ���ֵ�ռ� 
	delete[]bmp;
	///����Ҷ�ͼ
	//CBitmap bmp;
	//img.BitBlt()
	//img.Save("C:\\Documents and Settings\\Administrator\\����\\p11.bmp");
}

#endif