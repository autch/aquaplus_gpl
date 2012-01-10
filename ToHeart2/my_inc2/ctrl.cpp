#include "mm_std.h"

#include "ctrl.h"




int ListGetSelListEx( HWND hwnd, int idc, int **buf )
{
	int	size = ListGetSelCount(hwnd,idc);

	if(size>0){
		*buf = (int *)GAlloc( size*sizeof(int) );
		ListGetSelList( hwnd, idc, size, *buf );
	}
	return size;
}



void SetSliderParam( HWND hwnd, int idc_no, int max, int page, int start )
{
	SetSliderParamEx( hwnd, idc_no, 0, max, page, 1, page*2, start );
}

void SetSliderParamEx( HWND hwnd, int idc_no, int min, int max, int page, int line, int tick, int start )
{
	SliderSetRange( hwnd, idc_no, min, max );	
	SliderSetPos(  hwnd, idc_no, start );		
	SliderSetPage( hwnd, idc_no, page );		
	SliderSetLine( hwnd, idc_no, line );		
	SliderSetTick( hwnd, idc_no, tick );		
}



void SetMenuStr( HWND hwnd, int id, char *str )
{
	HMENU	hMenu  = GetMenu( hwnd );
	ModifyMenu( hMenu, id, MF_BYCOMMAND|MF_STRING, id, str );
	DrawMenuBar( hwnd );
}
void SetNewMenuStr( HWND hwnd, int id, int new_id, char *str )
{
	HMENU	hMenu  = GetMenu( hwnd );
	ModifyMenu( hMenu, id, MF_BYCOMMAND|MF_STRING, new_id, str );
	DrawMenuBar( hwnd );
}

void InsertMenuStr( HWND hwnd, int ins_id, int id, char *str )
{
	HMENU	hMenu  = GetMenu( hwnd );
	InsertMenu( hMenu, ins_id, MF_BYCOMMAND|MF_STRING, id, str );
	DrawMenuBar( hwnd );
}

void ResetMenu( HWND hwnd, int id, char *str )
{
	HMENU	hMenu  = GetMenu( hwnd );
	DeleteMenu( hMenu, id, MF_BYCOMMAND );
	DrawMenuBar( hwnd );
}
