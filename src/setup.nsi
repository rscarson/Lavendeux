; Script generated by the HM NIS Edit Script Wizard.

; HM NIS Edit Wizard helper defines
!define PRODUCT_NAME "Lavendeux"
!define PRODUCT_PUBLISHER "Richard Carson"
!define PRODUCT_WEB_SITE "http://rscarson.github.io/Lavendeux/"
!define PRODUCT_DOCUMENTATION "https://github.com/rscarson/Lavendeux/wiki"
!define PRODUCT_DIR_REGKEY "Software\Microsoft\Windows\CurrentVersion\App Paths\lavendeux.exe"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"

; MUI 1.67 compatible ------
!include "MUI2.nsh"

; MUI Settings
!define MUI_ABORTWARNING
!define MUI_ICON "..\icon\lavendeux.ico"
!define MUI_UNICON "..\icon\lavendeux.ico"

; Language Selection Dialog Settings
!define MUI_LANGDLL_REGISTRY_ROOT "${PRODUCT_UNINST_ROOT_KEY}"
!define MUI_LANGDLL_REGISTRY_KEY "${PRODUCT_UNINST_KEY}"
!define MUI_LANGDLL_REGISTRY_VALUENAME "NSIS:Language"

; Welcome page
!insertmacro MUI_PAGE_WELCOME
; License page
!insertmacro MUI_PAGE_LICENSE "..\LICENSE"
; Directory page
!insertmacro MUI_PAGE_DIRECTORY
; Instfiles page
!insertmacro MUI_PAGE_INSTFILES

; Finish page
!define MUI_FINISHPAGE_RUN
!define MUI_FINISHPAGE_RUN_TEXT "Run Lavendeux"
!define MUI_FINISHPAGE_RUN_FUNCTION "LaunchLink"
!define MUI_FINISHPAGE_SHOWREADME_TEXT "Open documentation"
!define MUI_FINISHPAGE_SHOWREADME "http://rscarson.github.io/Lavendeux/features"
!insertmacro MUI_PAGE_FINISH

Function LaunchLink
  ExecShell "" "$SMPROGRAMS\Lavendeux\Lavendeux.lnk"
FunctionEnd

; Uninstaller pages
!insertmacro MUI_UNPAGE_INSTFILES

; Language files
!insertmacro MUI_LANGUAGE "English"
!insertmacro MUI_LANGUAGE "French"

; MUI end ------

Name "${PRODUCT_NAME}"
OutFile "..\bin\lavendeux-setup.exe"
InstallDir "$PROGRAMFILES\Lavendeux"
InstallDirRegKey HKLM "${PRODUCT_DIR_REGKEY}" ""
ShowInstDetails show
ShowUnInstDetails show

Function .onInit
  !insertmacro MUI_LANGDLL_DISPLAY
FunctionEnd

Section "MainSection" SEC01
  SetOutPath "$INSTDIR"
  SetOverwrite ifnewer
  File "..\bin\lavendeux.exe"
  CreateDirectory "$SMPROGRAMS\Lavendeux"
  CreateShortCut "$SMPROGRAMS\Lavendeux\Lavendeux.lnk" "$INSTDIR\lavendeux.exe"
  CreateShortCut "$SMPROGRAMS\Lavendeux\Extensions.lnk" "$INSTDIR\extensions\"
  CreateShortCut "$DESKTOP\Lavendeux.lnk" "$INSTDIR\lavendeux.exe"
  File "..\LICENSE"
  File "..\CHANGELOG"
  File "..\bin\python27.zip"
  File "..\bin\python27.dll"

  CreateDirectory "$INSTDIR\extensions"
  SetOutPath "$INSTDIR\extensions"
  File /r "..\bin\extensions\"

  CreateDirectory "$INSTDIR\lib"
  SetOutPath "$INSTDIR\lib"
  File /r "..\bin\lib\"
SectionEnd

Section -AdditionalIcons
  CreateShortCut "$SMPROGRAMS\Lavendeux\Documentation.lnk" "${PRODUCT_DOCUMENTATION}"
  CreateShortCut "$SMPROGRAMS\Lavendeux\Uninstall.lnk" "$INSTDIR\uninst.exe"
SectionEnd

Section -Post
  WriteUninstaller "$INSTDIR\uninst.exe"
  WriteRegStr HKLM "${PRODUCT_DIR_REGKEY}" "" "$INSTDIR\lavendeux.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName" "$(^Name)"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\uninst.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayIcon" "$INSTDIR\lavendeux.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "URLInfoAbout" "${PRODUCT_WEB_SITE}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "Publisher" "${PRODUCT_PUBLISHER}"
SectionEnd


Function un.onUninstSuccess
  HideWindow
  MessageBox MB_ICONINFORMATION|MB_OK "$(^Name) was successfully removed from your computer."
FunctionEnd

Function un.onInit
!insertmacro MUI_UNGETLANGUAGE
  MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "Are you sure you want to completely remove $(^Name) and all of its components?" IDYES +2
  Abort
FunctionEnd

Section Uninstall
  Delete "$INSTDIR\${PRODUCT_NAME}.url"
  Delete "$INSTDIR\uninst.exe"
  Delete "$INSTDIR\CHANGELOG"
  Delete "$INSTDIR\LICENSE"
  Delete "$INSTDIR\README"
  Delete "$INSTDIR\python27.dll"
  Delete "$INSTDIR\python27.zip"
  Delete "$INSTDIR\lavendeux.exe"

  Delete "$SMPROGRAMS\Lavendeux\Uninstall.lnk"
  Delete "$SMPROGRAMS\Lavendeux\Documentation.lnk"
  Delete "$DESKTOP\Lavendeux.lnk"
  Delete "$SMPROGRAMS\Lavendeux\Lavendeux.lnk"

  RMDir "$SMPROGRAMS\Lavendeux"
  RMDir "$INSTDIR"

  DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
  DeleteRegKey HKLM "${PRODUCT_DIR_REGKEY}"
  SetAutoClose true
SectionEnd