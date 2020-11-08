!include MUI2.nsh

!define VERSION "0.0.0"

Name "dex ${VERSION}"

OutFile "release\dex-setup.exe"  

Unicode True

;Default installation folder
InstallDir "$PROGRAMFILES64\dex"

;--------------------------------
;Pages

  !insertmacro MUI_PAGE_LICENSE "LICENSE.txt"
  !insertmacro MUI_PAGE_COMPONENTS
  !insertmacro MUI_PAGE_DIRECTORY
  !insertmacro MUI_PAGE_INSTFILES
  
  !insertmacro MUI_UNPAGE_CONFIRM
  !insertmacro MUI_UNPAGE_INSTFILES

;--------------------------------
;Languages
 
  !insertmacro MUI_LANGUAGE "English"


!include LogicLib.nsh

Function .onInit
  MessageBox MB_YESNO "This will install dex. Do you wish to continue?" IDYES gogogo
    Abort
  gogogo:
FunctionEnd

Section "dex"
  SetOutPath $INSTDIR
  File "release\dex.exe"
  File "release\*.dll"
  ;Create uninstaller
  WriteUninstaller "$INSTDIR\Uninstall.exe"
SectionEnd

Section "un.dex"
SectionEnd


;--------------------------------
;Uninstaller Section

Section "Uninstall"

  Delete "$INSTDIR\dex.exe"
  Delete "$INSTDIR\*.dll"

  Delete "$INSTDIR\Uninstall.exe"

  RMDir "$INSTDIR"

SectionEnd