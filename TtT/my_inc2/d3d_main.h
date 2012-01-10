


#ifndef	_D3D_MAIN_H_
#define _D3D_MAIN_H_


#include <d3d8.h>
#include <d3dx8.h>
#include <ddraw.h>


#define RELEASE_3D(x) 	if(x){x->Release();x=NULL;}


typedef struct{
	char	m_R8G8B8;
	char	m_A8R8G8B8;
	char	m_X8R8G8B8;

	char	m_R5G6B5;
	char	m_X1R5G5B5;
	char	m_A1R5G5B5;
	char	m_A4R4G4B4;
	char	m_X4R4G4B4;

	char	m_A8P8;
	char	m_A8;
	char	m_P8;
}TEXTURE_CAPS;

typedef struct{
	D3DDISPLAYMODE	*m_DisplayMode;
	int				m_DisplayModeNum;
	int				m_FullModeNum;

	D3DDISPLAYMODE	m_NowDisplayMode;
	D3DDISPLAYMODE	m_WindowDisplayMode;

	DDCAPS			m_ddCaps;
	D3DCAPS8		m_d3dCaps;
	TEXTURE_CAPS	m_ttCaps;
}D3DCAPS_STRUCT;

typedef struct{
	int			m_WindowMode;
	int			m_FullScreenOnly;

	int			m_WindowModeChange;
	int			m_DisplayModeChange;
	int			m_FlipActive;
	HWND		m_DrawHwnd;
	HMENU		m_MenuHwnd;
}D3D_MAIN;

extern D3DCAPS_STRUCT	D3DCapsStruct;

extern D3D_MAIN		D3DMain;


extern LPDIRECT3D8				pD3D;
extern LPDIRECT3DDEVICE8		pD3DDevice;
extern D3DPRESENT_PARAMETERS	d3dppApp;


typedef struct{
	int		low_level;
	int		mip_map;
	int		full_16bit;
	int		txsel05;
	int		next_start;
}DRAW_SETTING;
extern DRAW_SETTING	DrawSetting;




extern void D3DM_SetWindowXY( int x, int y );
extern void D3DM_SetWindowStyle( DWORD win, DWORD full );

extern int D3DM_InitDevices( HWND hwnd, int w, int h, int full, HMENU menu=NULL );
extern int D3DM_TermDevices(void);

extern BOOL D3DM_ChangeF2W( void );



#endif	