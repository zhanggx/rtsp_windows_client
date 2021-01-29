
#include "GdiVideoRender.h"


#include <wincon.h>
#include <winuser.h>

//Not Efficient, Just an example
//change endian of a pixel (32bit)
void CHANGE_ENDIAN_32(unsigned char* data) {
	char temp3, temp2;
	temp3 = data[3];
	temp2 = data[2];
	data[3] = data[0];
	data[2] = data[1];
	data[0] = temp3;
	data[1] = temp2;
}
//change endian of a pixel (24bit)
void CHANGE_ENDIAN_24(unsigned char* data) {
	char temp2 = data[2];
	data[2] = data[0];
	data[0] = temp2;
}
//Change endian of a picture
void CHANGE_ENDIAN_PIC(unsigned char* image, int w, int h, int bpp) {
	unsigned char* pixeldata = NULL;
	for (int i = 0; i < h; i++)
		for (int j = 0; j < w; j++) {
			pixeldata = image + (i * w + j) * bpp / 8;
			if (bpp == 32) {
				CHANGE_ENDIAN_32(pixeldata);
			}
			else if (bpp == 24) {
				CHANGE_ENDIAN_24(pixeldata);
			}
		}
}
GdiVideoRender::GdiVideoRender(QLabel* localVideo, QLabel* remoteVideo)
{
	m_localVideo = localVideo;
	m_remoteVideo = remoteVideo;
	m_bRemoteTranStoped = true;
	m_localHwnd = static_cast<HWND>((void*)m_localVideo->winId());
	if (m_remoteVideo) {
		m_remoteHwnd = static_cast<HWND>((void*)m_remoteVideo->winId());
	}
	else {
		m_remoteHwnd = 0L;
	}

	//HDC hdc = GetDC(m_localHwnd);
	//SetStretchBltMode(hdc, STRETCH_HALFTONE);
}
void GdiVideoRender::PlayLocalVideo(unsigned char* data, int videoWidth, int videoHeight)
{
	ReaderVideoData(m_localHwnd, data, videoWidth, videoHeight);
}
void GdiVideoRender::PlayRemoteVideo(unsigned char* data, int videoWidth, int videoHeight)
{
	if (!m_bRemoteTranStoped&& m_remoteHwnd>0L)
	{
		ReaderVideoData(m_remoteHwnd, data, videoWidth, videoHeight);
	}
}

void GdiVideoRender::SetRemoteTranStoped(bool stoped)
{
	m_bRemoteTranStoped = stoped;
}
void GdiVideoRender::ReaderVideoData(HWND hwnd, unsigned char* data, int videoWidth, int videoHeight)
{
	HDC hdc = GetDC(hwnd);
	RECT rect;
	GetWindowRect(hwnd, &rect);
	int screen_w = rect.right - rect.left;
	int screen_h = rect.bottom - rect.top;

	//BMP Header
	BITMAPINFO m_bmphdr = { 0 };
	DWORD dwBmpHdr = sizeof(BITMAPINFO);
	//24bit
	m_bmphdr.bmiHeader.biBitCount = 24;
	m_bmphdr.bmiHeader.biClrImportant = 0;
	m_bmphdr.bmiHeader.biSize = dwBmpHdr;
	m_bmphdr.bmiHeader.biSizeImage = 0;
	m_bmphdr.bmiHeader.biWidth = videoWidth;
	//Notice: BMP storage pixel data in opposite direction of Y-axis (from bottom to top).
	//So we must set reverse biHeight to show image correctly.
	m_bmphdr.bmiHeader.biHeight = -videoHeight;
	m_bmphdr.bmiHeader.biXPelsPerMeter = 0;
	m_bmphdr.bmiHeader.biYPelsPerMeter = 0;
	m_bmphdr.bmiHeader.biClrUsed = 0;
	m_bmphdr.bmiHeader.biPlanes = 1;
	m_bmphdr.bmiHeader.biCompression = BI_RGB;

	//如果是RGB24就要转换，如果是BGR24就不需要转换
	//Change to Little Endian
	CHANGE_ENDIAN_PIC(data, videoWidth, videoHeight, 24);
	SetStretchBltMode(hdc, STRETCH_HALFTONE);
	int nResult = StretchDIBits(hdc,
		0, 0,
		screen_w, screen_h,
		0, 0,
		videoWidth, videoHeight,
		data,
		&m_bmphdr,
		DIB_RGB_COLORS,
		SRCCOPY);
	ReleaseDC(hwnd, hdc);
}