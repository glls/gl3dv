;NSIS Modern User Interface
;Basic Example Script
;Written by Joost Verburg

;--------------------------------
;Include Modern UI

  !include "MUI.nsh"

;--------------------------------
;General

  ;Name and file
  Name "gl3dv"
  OutFile "gl3dv_setup.exe"

  ;Default installation folder
  InstallDir "$PROGRAMFILES\gl3dv"
  
  ;Get installation folder from registry if available
  ;InstallDirRegKey HKCU "Software\gl3dv" ""

;--------------------------------
;Interface Settings

  !define MUI_ABORTWARNING

;--------------------------------
;Pages

  !insertmacro MUI_PAGE_LICENSE "DISCLAIMER.txt"
  !insertmacro MUI_PAGE_COMPONENTS
  !insertmacro MUI_PAGE_DIRECTORY
  !insertmacro MUI_PAGE_INSTFILES
  
  !insertmacro MUI_UNPAGE_CONFIRM
  !insertmacro MUI_UNPAGE_INSTFILES
  
;--------------------------------
;Languages
 
  !insertmacro MUI_LANGUAGE "English"

;--------------------------------
;Installer Sections

Section "Core files" SecDummy

  SetOutPath "$INSTDIR"
  
  ;ADD YOUR OWN FILES HERE...
  ; http://www.coin3d.org/windows/redistribution
  File "release\gl3dv.exe"
  File "release\coin2.dll"
  ;File "release\libsndfile.dll"
  File "release\simage1.dll"
  File "release\sowin1.dll"
  ;File "release\zlib1.dll"
  File "README.txt"
  File "DISCLAIMER.txt"
  File "test_data\pizza.jpg"
  File "test_data\pizza.wrl"
  File "test_data\test.pts"
  
  ;Store installation folder
  ;WriteRegStr HKCU "Software\Modern UI Test" "" $INSTDIR
  
  ;Create uninstaller
  WriteUninstaller "$INSTDIR\Uninstall.exe"

SectionEnd

; Optional section (can be disabled by the user)
Section "Start Menu Shortcuts"

  CreateDirectory "$SMPROGRAMS\gl3dv"
  CreateShortCut "$SMPROGRAMS\gl3dv\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
  CreateShortCut "$SMPROGRAMS\gl3dv\gl3dv.lnk" "$INSTDIR\gl3dv.exe" "" "$INSTDIR\gl3dv.exe" 0
  
SectionEnd

;--------------------------------
;Descriptions

  ;Language strings
  LangString DESC_SecDummy ${LANG_ENGLISH} "gl3dv executable and libraries"

  ;Assign language strings to sections
  !insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
    !insertmacro MUI_DESCRIPTION_TEXT ${SecDummy} $(DESC_SecDummy)
  !insertmacro MUI_FUNCTION_DESCRIPTION_END

;--------------------------------
;Uninstaller Section

Section "Uninstall"

  ;ADD YOUR OWN FILES HERE...
  Delete "$INSTDIR\gl3dv.exe"
  Delete "$INSTDIR\coin2.dll"
  ;Delete "$INSTDIR\libsndfile.dll"
  Delete "$INSTDIR\simage1.dll"
  Delete "$INSTDIR\sowin1.dll"
  ;Delete "$INSTDIR\zlib1.dll"
  Delete "$INSTDIR\README.txt"
  Delete "$INSTDIR\DISCLAIMER.txt"
  Delete  "$INSTDIR\pizza.jpg"
  Delete  "$INSTDIR\pizza.wrl"
  Delete  "$INSTDIR\test.pts"

  Delete "$INSTDIR\Uninstall.exe"

  ; Remove shortcuts, if any
  Delete "$SMPROGRAMS\gl3dv\*.*"

  ; Remove directories used
  RMDir "$INSTDIR"
  RMDir "$SMPROGRAMS\gl3dv"

  DeleteRegKey /ifempty HKCU "Software\gl3dv"

SectionEnd