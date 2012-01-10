# Microsoft Developer Studio Project File - Name="_D3DD" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** 編集しないでください **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=_D3DD - Win32 Debug
!MESSAGE これは有効なﾒｲｸﾌｧｲﾙではありません。 このﾌﾟﾛｼﾞｪｸﾄをﾋﾞﾙﾄﾞするためには NMAKE を使用してください。
!MESSAGE [ﾒｲｸﾌｧｲﾙのｴｸｽﾎﾟｰﾄ] ｺﾏﾝﾄﾞを使用して実行してください
!MESSAGE 
!MESSAGE NMAKE /f "_D3DD.mak".
!MESSAGE 
!MESSAGE NMAKE の実行時に構成を指定できます
!MESSAGE ｺﾏﾝﾄﾞ ﾗｲﾝ上でﾏｸﾛの設定を定義します。例:
!MESSAGE 
!MESSAGE NMAKE /f "_D3DD.mak" CFG="_D3DD - Win32 Debug"
!MESSAGE 
!MESSAGE 選択可能なﾋﾞﾙﾄﾞ ﾓｰﾄﾞ:
!MESSAGE 
!MESSAGE "_D3DD - Win32 Release" ("Win32 (x86) Application" 用)
!MESSAGE "_D3DD - Win32 Debug" ("Win32 (x86) Application" 用)
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "_D3DD - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /Zi /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x411 /d "NDEBUG"
# ADD RSC /l 0x411 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 XviDDec60.lib ogg_static.lib vorbis_static.lib msacm32.lib winmm.lib d3dx8dt.lib d3d8.lib d3dxof.lib dxguid.lib ddraw.lib dsound.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"TtT.exe"

!ELSEIF  "$(CFG)" == "_D3DD - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x411 /d "_DEBUG"
# ADD RSC /l 0x411 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 XviDDec60.lib ogg_static.lib vorbis_static.lib msacm32.lib winmm.lib d3dx8dt.lib d3d8.lib d3dxof.lib dxguid.lib ddraw.lib dsound.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"TtT.exe" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "_D3DD - Win32 Release"
# Name "_D3DD - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\my_inc2\Comp_LZS.cpp
# End Source File
# Begin Source File

SOURCE=..\my_inc2\Comp_pac.cpp
# End Source File
# Begin Source File

SOURCE=..\my_inc2\ctrl.cpp
# End Source File
# Begin Source File

SOURCE=..\my_inc2\d3d_draw.cpp
# End Source File
# Begin Source File

SOURCE=..\my_inc2\d3d_Font.cpp
# End Source File
# Begin Source File

SOURCE=..\my_inc2\d3d_main.cpp
# End Source File
# Begin Source File

SOURCE=..\my_inc2\d3d_text.cpp
# End Source File
# Begin Source File

SOURCE=..\my_inc2\dispSprite.cpp
# End Source File
# Begin Source File

SOURCE=..\my_inc2\Enum.cpp
# End Source File
# Begin Source File

SOURCE=.\src\Escript.cpp
# End Source File
# Begin Source File

SOURCE=..\my_inc2\GlobalDraw.cpp
# End Source File
# Begin Source File

SOURCE=.\src\GM_Avg.cpp
# End Source File
# Begin Source File

SOURCE=.\src\GM_Battle.cpp
# End Source File
# Begin Source File

SOURCE=.\src\GM_Castle.cpp
# End Source File
# Begin Source File

SOURCE=.\src\GM_Char.cpp
# End Source File
# Begin Source File

SOURCE=.\src\GM_CharThink.cpp
# End Source File
# Begin Source File

SOURCE=.\src\GM_Item.cpp
# End Source File
# Begin Source File

SOURCE=.\src\GM_Magic.cpp
# End Source File
# Begin Source File

SOURCE=.\src\GM_Main.cpp
# End Source File
# Begin Source File

SOURCE=.\src\GM_SetBmp.cpp
# End Source File
# Begin Source File

SOURCE=.\src\GM_Title.cpp
# End Source File
# Begin Source File

SOURCE=.\src\GM_World.cpp
# End Source File
# Begin Source File

SOURCE=..\my_inc2\keybord.cpp
# End Source File
# Begin Source File

SOURCE=..\my_inc2\mouse.cpp
# End Source File
# Begin Source File

SOURCE=..\oggDec\oggDec.cpp
# End Source File
# Begin Source File

SOURCE=..\my_inc2\readFile.cpp
# End Source File
# Begin Source File

SOURCE=.\src\Script1.rc
# End Source File
# Begin Source File

SOURCE=..\my_inc2\soundDS.cpp
# End Source File
# Begin Source File

SOURCE=..\my_inc2\STD_Debug.cpp
# End Source File
# Begin Source File

SOURCE=..\my_inc2\STD_File.cpp
# End Source File
# Begin Source File

SOURCE=..\my_inc2\STD_Windows.cpp
# End Source File
# Begin Source File

SOURCE=..\my_inc2\tga.cpp
# End Source File
# Begin Source File

SOURCE=.\src\windows.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\my_inc2\Bmp.h
# End Source File
# Begin Source File

SOURCE=..\my_inc2\ClCommon.h
# End Source File
# Begin Source File

SOURCE=..\oggDec\codec.h
# End Source File
# Begin Source File

SOURCE=..\my_inc2\Comp_BmpPac.h
# End Source File
# Begin Source File

SOURCE=..\my_inc2\comp_LZS.h
# End Source File
# Begin Source File

SOURCE=..\my_inc2\comp_pac.h
# End Source File
# Begin Source File

SOURCE=..\my_inc2\ctrl.h
# End Source File
# Begin Source File

SOURCE=..\my_inc2\d3d_draw.h
# End Source File
# Begin Source File

SOURCE=..\my_inc2\d3d_Font.h
# End Source File
# Begin Source File

SOURCE=..\my_inc2\d3d_main.h
# End Source File
# Begin Source File

SOURCE=..\my_inc2\d3d_text.h
# End Source File
# Begin Source File

SOURCE=.\src\DAT_CharName.h
# End Source File
# Begin Source File

SOURCE=.\src\DAT_ItemParam.h
# End Source File
# Begin Source File

SOURCE=.\src\DAT_MagicParam.h
# End Source File
# Begin Source File

SOURCE=.\src\DAT_ScriptListName.h
# End Source File
# Begin Source File

SOURCE=..\my_inc2\DISP.H
# End Source File
# Begin Source File

SOURCE=..\my_inc2\dispSprite.h
# End Source File
# Begin Source File

SOURCE=..\my_inc2\Draw.h
# End Source File
# Begin Source File

SOURCE=..\my_inc2\DrawPrim.h
# End Source File
# Begin Source File

SOURCE=..\my_inc2\Enum.h
# End Source File
# Begin Source File

SOURCE=.\mes\Escr.h
# End Source File
# Begin Source File

SOURCE=.\src\Escript.h
# End Source File
# Begin Source File

SOURCE=..\my_inc2\font.h
# End Source File
# Begin Source File

SOURCE=..\my_inc2\GlobalDraw.h
# End Source File
# Begin Source File

SOURCE=.\src\GM_Avg.h
# End Source File
# Begin Source File

SOURCE=.\src\GM_Battle.h
# End Source File
# Begin Source File

SOURCE=.\src\GM_Char.h
# End Source File
# Begin Source File

SOURCE=.\src\GM_CharThink.h
# End Source File
# Begin Source File

SOURCE=.\src\GM_Item.h
# End Source File
# Begin Source File

SOURCE=.\src\GM_Magic.h
# End Source File
# Begin Source File

SOURCE=.\src\GM_Main.h
# End Source File
# Begin Source File

SOURCE=.\src\GM_SetBmp.h
# End Source File
# Begin Source File

SOURCE=.\src\GM_World.h
# End Source File
# Begin Source File

SOURCE=..\my_inc2\keybord.h
# End Source File
# Begin Source File

SOURCE=.\src\main.h
# End Source File
# Begin Source File

SOURCE=..\my_inc2\mouse.h
# End Source File
# Begin Source File

SOURCE=..\oggDec\oggDec.h
# End Source File
# Begin Source File

SOURCE=..\my_inc2\Palette.h
# End Source File
# Begin Source File

SOURCE=..\my_inc2\readFile.h
# End Source File
# Begin Source File

SOURCE=.\src\resource.h
# End Source File
# Begin Source File

SOURCE=..\my_inc2\soundDS.h
# End Source File
# Begin Source File

SOURCE=..\my_inc2\STD_Debug.h
# End Source File
# Begin Source File

SOURCE=..\my_inc2\STD_File.h
# End Source File
# Begin Source File

SOURCE=..\my_inc2\STD_Windows.h
# End Source File
# Begin Source File

SOURCE=..\my_inc2\text.h
# End Source File
# Begin Source File

SOURCE=..\my_inc2\tga.h
# End Source File
# Begin Source File

SOURCE=..\XviD\XVidDec_60\bin\xvid.h
# End Source File
# Begin Source File

SOURCE=..\XviD\XVidDec_60\bin\xvid_dec.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\src\icon1.ico
# End Source File
# End Group
# End Target
# End Project
