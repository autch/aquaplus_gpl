/*****************************************************************************
 *
 *  XVID MPEG-4 VIDEO CODEC
 *  - Configure from command line  -
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
#include "resource.h"

/* "DllEntryPoint@12" in strmbase.lib\dllentry.obj stores the module in g_hInst 
    this function must be called on DllEntry, inorder for property pages to function
	likewise, we need g_hInst inorder to display property sheets from command line
*/
extern HINSTANCE g_hInst;


int adv_dialog(HWND hwndOwner)
{
	PROPSHEETPAGE psp [1];
	PROPSHEETHEADER psh;

	psp[0].dwSize = sizeof (PROPSHEETPAGE);
	psp[0].dwFlags = PSP_USETITLE;
	psp[0].hInstance = g_hInst;
	psp[0].pszTemplate = MAKEINTRESOURCE (IDD_ABOUT);
	psp[0].pszIcon = NULL;
	psp[0].pfnDlgProc = adv_proc;
	psp[0].pszTitle = "About";
	psp[0].lParam = 0;

	psh.dwSize = sizeof (PROPSHEETHEADER);
	psh.dwFlags = PSH_PROPSHEETPAGE;
	psh.hwndParent = hwndOwner;
	psh.hInstance = g_hInst;
	psh.pszIcon = NULL;
	psh.pszCaption = (LPSTR)"XviD Configuration";
	psh.nPages = sizeof (psp) / sizeof (PROPSHEETPAGE);
	psh.ppsp = (LPCPROPSHEETPAGE) &psp;

	return PropertySheet (&psh);
}



void CALLBACK Configure(HWND hWndParent, HINSTANCE hInstParent, LPSTR lpCmdLine, int nCmdShow ) 
{
	InitCommonControls();
	LoadRegistryInfo();
	adv_dialog( GetDesktopWindow() );
}