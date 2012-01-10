/*****************************************************************************
 *
 *  XVID MPEG-4 VIDEO CODEC
 *  - Configuration processing -
 *
 *  Copyright(C) 2002-2004 Peter Ross <pross@xvid.org>
 *
 *  This program is free software ; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation ; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY ; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program ; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 *
 * $Id$
 *
 ****************************************************************************/

#include <windows.h>
#include <commctrl.h>
#include "config.h"
#include "debug.h"
#include "resource.h"


// -----------------------------------------
// global config structure
CONFIG g_config;



void LoadRegistryInfo()
{
	HKEY hKey;
	DWORD size;
	RegOpenKeyEx(XVID_REG_KEY, XVID_REG_SUBKEY, 0, KEY_READ, &hKey);

	// Set the default post-processing settings
	REG_GET_N("Brightness", g_config.nBrightness, 25)
	REG_GET_N("Deblock_Y",  g_config.nDeblock_Y, 0)
	REG_GET_N("Deblock_UV", g_config.nDeblock_UV, 0)
	REG_GET_N("Dering",  g_config.nDering, 0)
	REG_GET_N("FilmEffect", g_config.nFilmEffect, 0)
	REG_GET_N("ForceColorspace", g_config.nForceColorspace, 0)
	REG_GET_N("FlipVideo",  g_config.nFlipVideo, 0)
	REG_GET_N("Supported_4CC",  g_config.supported_4cc, 0)
	REG_GET_N("Videoinfo_Compat",  g_config.videoinfo_compat, 0)

	RegCloseKey(hKey);
}

void SaveRegistryInfo()
{
	HKEY hKey;
	DWORD dispo;

	if (RegCreateKeyEx(
			XVID_REG_KEY,
			XVID_REG_SUBKEY,
			0,
			XVID_REG_CLASS,
			REG_OPTION_NON_VOLATILE,
			KEY_WRITE,
			0, 
			&hKey, 
			&dispo) != ERROR_SUCCESS)
	{
		OutputDebugString("Couldn't create XVID_REG_SUBKEY");
		return;
	}

	REG_SET_N("Brightness", g_config.nBrightness);
	REG_SET_N("Deblock_Y",  g_config.nDeblock_Y);
	REG_SET_N("Deblock_UV", g_config.nDeblock_UV);
	REG_SET_N("Dering", g_config.nDering);
	REG_SET_N("FilmEffect", g_config.nFilmEffect);
	REG_SET_N("ForceColorspace", g_config.nForceColorspace);
	REG_SET_N("FlipVideo", g_config.nFlipVideo);
	REG_SET_N("Supported_4CC",  g_config.supported_4cc);
	REG_SET_N("Videoinfo_Compat",  g_config.videoinfo_compat);

	RegCloseKey(hKey);
}



BOOL CALLBACK adv_proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HWND hBrightness;

	switch ( uMsg )
	{
	case WM_DESTROY:
		{
			int nForceColorspace;
			nForceColorspace = SendMessage(GetDlgItem(hwnd, IDC_COLORSPACE), CB_GETCURSEL, 0, 0); 
			if ( g_config.nForceColorspace != nForceColorspace )
			{
				MessageBox(0, "You have changed the output colorspace.\r\nClose the movie and open it for the new colorspace to take effect.", "XviD DShow", 0);
			}
			g_config.nForceColorspace = nForceColorspace;
			SaveRegistryInfo();
		}
		break;

	case WM_INITDIALOG:

		// Load Force Colorspace Box
		SendMessage(GetDlgItem(hwnd, IDC_COLORSPACE), CB_ADDSTRING, 0, (LPARAM)"No Force"); 
		SendMessage(GetDlgItem(hwnd, IDC_COLORSPACE), CB_ADDSTRING, 0, (LPARAM)"YV12"); 
		SendMessage(GetDlgItem(hwnd, IDC_COLORSPACE), CB_ADDSTRING, 0, (LPARAM)"YUY2"); 
		SendMessage(GetDlgItem(hwnd, IDC_COLORSPACE), CB_ADDSTRING, 0, (LPARAM)"RGB24"); 
		SendMessage(GetDlgItem(hwnd, IDC_COLORSPACE), CB_ADDSTRING, 0, (LPARAM)"RGB32"); 

		// Select Colorspace
		SendMessage(GetDlgItem(hwnd, IDC_COLORSPACE), CB_SETCURSEL, g_config.nForceColorspace, 0); 

		hBrightness = GetDlgItem(hwnd, IDC_BRIGHTNESS);
		SendMessage(hBrightness, TBM_SETRANGE, (WPARAM) (BOOL) TRUE, (LPARAM) MAKELONG(0, 50));
		SendMessage(hBrightness, TBM_SETPOS, (WPARAM) (BOOL) TRUE, (LPARAM) g_config.nBrightness);

		// Load Buttons
		SendMessage(GetDlgItem(hwnd, IDC_DEBLOCK_Y), BM_SETCHECK, g_config.nDeblock_Y, 0);
		SendMessage(GetDlgItem(hwnd, IDC_DEBLOCK_UV), BM_SETCHECK, g_config.nDeblock_UV, 0);
		SendMessage(GetDlgItem(hwnd, IDC_DERING), BM_SETCHECK, g_config.nDering, 0);
		SendMessage(GetDlgItem(hwnd, IDC_FILMEFFECT), BM_SETCHECK, g_config.nFilmEffect, 0);
		SendMessage(GetDlgItem(hwnd, IDC_FLIPVIDEO), BM_SETCHECK, g_config.nFlipVideo, 0);

		// 4CC checkbuttons
		SendMessage(GetDlgItem(hwnd, IDC_DIVX), BM_SETCHECK, g_config.supported_4cc & SUPPORT_DIVX, 0);
		SendMessage(GetDlgItem(hwnd, IDC_DX50), BM_SETCHECK, g_config.supported_4cc & SUPPORT_DX50, 0);
		SendMessage(GetDlgItem(hwnd, IDC_MP4V), BM_SETCHECK, g_config.supported_4cc & SUPPORT_MP4V, 0);
		SendMessage(GetDlgItem(hwnd, IDC_COMPAT), BM_SETCHECK, g_config.videoinfo_compat, 0);

		// Set Date & Time of Compilation
		DPRINTF("(%s %s)", __DATE__, __TIME__);
		break;

	case WM_COMMAND:
		switch ( wParam )
		{
		case IDC_RESET:
			ZeroMemory(&g_config, sizeof(CONFIG));
			g_config.nBrightness = 25;
			hBrightness = GetDlgItem(hwnd, IDC_BRIGHTNESS);
			SendMessage(hBrightness, TBM_SETPOS, (WPARAM) (BOOL) TRUE, (LPARAM) g_config.nBrightness);
			// Load Buttons
			SendMessage(GetDlgItem(hwnd, IDC_DEBLOCK_Y), BM_SETCHECK, g_config.nDeblock_Y, 0);
			SendMessage(GetDlgItem(hwnd, IDC_DEBLOCK_UV), BM_SETCHECK, g_config.nDeblock_UV, 0);
			SendMessage(GetDlgItem(hwnd, IDC_DERING), BM_SETCHECK, g_config.nDering, 0);
			SendMessage(GetDlgItem(hwnd, IDC_FILMEFFECT), BM_SETCHECK, g_config.nFilmEffect, 0);
			SendMessage(GetDlgItem(hwnd, IDC_FLIPVIDEO), BM_SETCHECK, g_config.nFlipVideo, 0);
			g_config.nForceColorspace = 0;
			SendMessage(GetDlgItem(hwnd, IDC_COLORSPACE), CB_SETCURSEL, g_config.nForceColorspace, 0); 

			break;
		case IDC_DEBLOCK_Y:
			g_config.nDeblock_Y = !g_config.nDeblock_Y;
			break;
		case IDC_DEBLOCK_UV:
			g_config.nDeblock_UV = !g_config.nDeblock_UV;
			break;
		case IDC_DERING:
			g_config.nDering = !g_config.nDering;
			break;
		case IDC_FILMEFFECT:
			g_config.nFilmEffect = !g_config.nFilmEffect;
			break;
		case IDC_FLIPVIDEO:
			g_config.nFlipVideo = !g_config.nFlipVideo;
			break;
		case IDC_DIVX:
			g_config.supported_4cc ^= SUPPORT_DIVX;
			break;
		case IDC_DX50:
			g_config.supported_4cc ^= SUPPORT_DX50;
			break;
		case IDC_MP4V:
			g_config.supported_4cc ^= SUPPORT_MP4V;
			break;
		case IDC_COMPAT:
			g_config.videoinfo_compat = !g_config.videoinfo_compat;
			break;
		default :
			return FALSE;
		}
		SaveRegistryInfo();
		break;
	case WM_NOTIFY:
		hBrightness = GetDlgItem(hwnd, IDC_BRIGHTNESS);
		g_config.nBrightness = SendMessage(hBrightness, TBM_GETPOS, (WPARAM)NULL, (LPARAM)NULL);
		SaveRegistryInfo();
		break;
	default :
		return FALSE;
	}
	
	return TRUE; /* ok */
}





