
 
  
   
    
     
      

#ifndef	_DRAWPRIM_H
#define _DRAWPRIM_H


extern BOOL PRM_DrawRect_T( BMP_T *dest, int dx, int dy, int w, int h, RECT *clip, int r, int g, int b, DWORD wparam );

extern BOOL PRM_DrawF3_T( BMP_T *dest,	int x1, int y1, int x2, int y2, int x3, int y3, RECT *clip, int r, int g, int b, DWORD wparam );
extern BOOL PRM_DrawF4_T( BMP_T *dest,	int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, RECT *clip, int r, int g, int b, DWORD wparam );

extern BOOL PRM_DrawY3_T( BMP_T *dest,	int x1, int y1,
										int x2, int y2,
										int x3, int y3, RECT *clip,
										int sy, int sr, int sg, int sb,
										int ey, int er, int eg, int eb, DWORD wparam );
extern BOOL PRM_DrawY4_T( BMP_T *dest,	int x1, int y1,
										int x2, int y2,
										int x3, int y3,
										int x4, int y4, RECT *clip,
										int sy, int sr, int sg, int sb,
										int ey, int er, int eg, int eb, DWORD wparam );

extern BOOL PRM_DrawG3_T( BMP_T *dest,	int x1, int y1, int r1, int g1, int b1,
										int x2, int y2, int r2, int g2, int b2,
										int x3, int y3, int r3, int g3, int b3, RECT *clip, DWORD wparam );
extern BOOL PRM_DrawG4_T( BMP_T *dest,	int x1, int y1, int r1, int g1, int b1,
										int x2, int y2, int r2, int g2, int b2,
										int x3, int y3, int r3, int g3, int b3,
										int x4, int y4, int r4, int g4, int b4, RECT *clip, DWORD wparam );

extern BOOL PRM_DrawFLine_T( BMP_T *dest,	int x1, int y1, int x2, int y2, RECT *clip, int r, int g, int b, DWORD wparam );
extern BOOL PRM_DrawGLine_T( BMP_T *dest,	int x1, int y1, int r1, int g1, int b1, int x2, int y2, int r2, int g2, int b2, RECT *clip, DWORD wparam );

extern BOOL PRM_DrawMarukage_T( BMP_T *dest, int cx, int cy, int w, RECT *clip );


extern BOOL PRM_DrawRect_F( BMP_F *dest, int dx, int dy, int w, int h, RECT *clip, int r, int g, int b, DWORD wparam );

extern BOOL PRM_DrawF3_F( BMP_F *dest,	int x1, int y1, int x2, int y2, int x3, int y3, RECT *clip, int r, int g, int b, DWORD wparam );
extern BOOL PRM_DrawF4_F( BMP_F *dest,	int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, RECT *clip, int r, int g, int b, DWORD wparam );

extern BOOL PRM_DrawY3_F( BMP_F *dest,	int x1, int y1,
										int x2, int y2,
										int x3, int y3, RECT *clip,
										int sy, int sr, int sg, int sb,
										int ey, int er, int eg, int eb, DWORD wparam );
extern BOOL PRM_DrawY4_F( BMP_F *dest,	int x1, int y1,
										int x2, int y2,
										int x3, int y3,
										int x4, int y4, RECT *clip,
										int sy, int sr, int sg, int sb,
										int ey, int er, int eg, int eb, DWORD wparam );

extern BOOL PRM_DrawG3_F( BMP_F *dest,	int x1, int y1, int r1, int g1, int b1,
										int x2, int y2, int r2, int g2, int b2,
										int x3, int y3, int r3, int g3, int b3, RECT *clip, DWORD wparam );
extern BOOL PRM_DrawG4_F( BMP_F *dest,	int x1, int y1, int r1, int g1, int b1,
										int x2, int y2, int r2, int g2, int b2,
										int x3, int y3, int r3, int g3, int b3,
										int x4, int y4, int r4, int g4, int b4, RECT *clip, DWORD wparam );

extern BOOL PRM_DrawFLine_F( BMP_F *dest,	int x1, int y1, int x2, int y2, RECT *clip, int r, int g, int b, DWORD wparam );
extern BOOL PRM_DrawGLine_F( BMP_F *dest,	int x1, int y1, int r1, int g1, int b1, int x2, int y2, int r2, int g2, int b2, RECT *clip, DWORD wparam );

extern BOOL PRM_DrawHanabi_F( BMP_F *dest,	int cx, int cy, int rate, RECT *clip );

extern BOOL PRM_DrawMarukage_F( BMP_F *dest, int cx, int cy, int w, RECT *clip );
extern BOOL PRM_DrawRipple_F( BMP_F *dest, int dx, int dy, RECT *clip, int r, int g, int b, DWORD wparam );



extern BOOL PRM_DrawRect_H( BMP_H *dest, int dx, int dy, int w, int h, RECT *clip, int r, int g, int b, DWORD wparam );

extern BOOL PRM_DrawF3_H( BMP_H *dest,	int x1, int y1, int x2, int y2, int x3, int y3, RECT *clip, int r, int g, int b, DWORD wparam );
extern BOOL PRM_DrawF4_H( BMP_H *dest,	int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, RECT *clip, int r, int g, int b, DWORD wparam );

extern BOOL PRM_DrawY3_H( BMP_H *dest,	int x1, int y1,
										int x2, int y2,
										int x3, int y3, RECT *clip,
										int sy, int sr, int sg, int sb,
										int ey, int er, int eg, int eb, DWORD wparam );
extern BOOL PRM_DrawY4_H( BMP_H *dest,	int x1, int y1,
										int x2, int y2,
										int x3, int y3,
										int x4, int y4, RECT *clip,
										int sy, int sr, int sg, int sb,
										int ey, int er, int eg, int eb, DWORD wparam );

extern BOOL PRM_DrawG3_H( BMP_H *dest,	int x1, int y1, int r1, int g1, int b1,
										int x2, int y2, int r2, int g2, int b2,
										int x3, int y3, int r3, int g3, int b3, RECT *clip, DWORD wparam );
extern BOOL PRM_DrawG4_H( BMP_H *dest,	int x1, int y1, int r1, int g1, int b1,
										int x2, int y2, int r2, int g2, int b2,
										int x3, int y3, int r3, int g3, int b3,
										int x4, int y4, int r4, int g4, int b4, RECT *clip, DWORD wparam );

extern BOOL PRM_DrawFLine_H( BMP_H *dest,	int x1, int y1, int x2, int y2, RECT *clip, int r, int g, int b, DWORD wparam );
extern BOOL PRM_DrawGLine_H( BMP_H *dest,	int x1, int y1, int r1, int g1, int b1, int x2, int y2, int r2, int g2, int b2, RECT *clip, DWORD wparam );

extern BOOL PRM_DrawMarukage_H( BMP_H *dest, int cx, int cy, int w, RECT *clip );

#endif