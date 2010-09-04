; Installer script for win32 LinQ
; Hu Zheng <huzheng_001@163.com>

;--------------------------------
;Configuration

;General

!define LINQ_VERSION		"2.0.0"

OutFile "linq-${LINQ_VERSION}.exe"

XPStyle on

; SetCompressor bzip2

DirShow show
ShowInstDetails show
ShowUninstDetails show
SetDateSave on

; $INSTDIR is set in .onInit function..

!include "MUI.nsh"
!include Sections.nsh

;--------------------------------
;Defines

!define MUI_PRODUCT			"LinQ"
!define MUI_VERSION			${LINQ_VERSION}

!define MUI_ICON			.\linq.ico
!define MUI_UNICON			.\linq-uninst.ico
!define MUI_SPECIALBITMAP		.\src\win32\nsis\linq-intro.bmp
!define MUI_HEADERBITMAP		.\src\win32\nsis\linq-header.bmp

!define LINQ_NSIS_INCLUDE_PATH		".\src\win32\nsis"

!define LINQ_REG_KEY			"SOFTWARE\linq"
!define LINQ_UNINSTALL_KEY		"SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\LinQ"
!define HKLM_APP_PATHS_KEY		"SOFTWARE\Microsoft\Windows\CurrentVersion\App Paths\linq.exe"
!define LINQ_UNINST_EXE			"linq-uninst.exe"

!define GTK_VERSION			"2.2.1"
!define GTK_REG_KEY			"SOFTWARE\GTK\2.0"
!define GTK_INSTALL_VERIFIER		"lib\libgtk-win32-2.0-0.dll"



;--------------------------------
;Modern UI Configuration

  !define MUI_CUSTOMPAGECOMMANDS

  !define MUI_WELCOMEPAGE
  !define MUI_LICENSEPAGE
  !define MUI_DIRECTORYPAGE
  !define MUI_FINISHPAGE
  
  !define MUI_ABORTWARNING

  !define MUI_UNINSTALLER
  !define MUI_UNCONFIRMPAGE

;--------------------------------
;Pages
  
  !insertmacro MUI_PAGECOMMAND_WELCOME
  !insertmacro MUI_PAGECOMMAND_LICENSE
  !insertmacro MUI_PAGECOMMAND_DIRECTORY
  !insertmacro MUI_PAGECOMMAND_INSTFILES
  !insertmacro MUI_PAGECOMMAND_FINISH

;--------------------------------
;Languages
 
  !insertmacro MUI_LANGUAGE "English"

;--------------------------------
;Language Strings
  LangString GTK_INSTALLER_NEEDED		${LANG_ENGLISH} \
		"The GTK+ runtime environment is either missing or needs to be upgraded.$\r \
		Please install v${GTK_VERSION} or higher of the GTK+ runtime"


  ; LinQ Section Prompts and Texts
  LangString LINQ_UNINSTALL_DESC		${LANG_ENGLISH} \
		"LinQ (remove only)"
  LangString LINQ_PROMPT_WIPEOUT		${LANG_ENGLISH} \
		"You're old LinQ directory is about to be deleted. Would you like to continue?$\r$\r \
		LinQ user settings will not be affected."
  LangString LINQ_PROMPT_DIR_EXISTS		${LANG_ENGLISH} \
		"The installation directory you specified already exists. Any contents $\r \
		it may have will be deleted. Would you like to continue?"

  ; Uninstall Section Prompts
  LangString un.LINQ_UNINSTALL_ERROR_1         	${LANG_ENGLISH} \
		"The uninstaller could not find registry entries for LinQ.$\r \
		It is likely that another user installed this application."
  LangString un.LINQ_UNINSTALL_ERROR_2         	${LANG_ENGLISH} \
		"You do not have permission to uninstall this application."

;--------------------------------
;Data
  
  LicenseData "./COPYING"

;--------------------------------
;Uninstall any old version of LinQ

Section -SecUninstallOldLinQ
  ; Check install rights..
  Call CheckUserInstallRights
  Pop $R0

  StrCmp $R0 "HKLM" linq_hklm
  StrCmp $R0 "HKCU" linq_hkcu done

  linq_hkcu:
      ReadRegStr $R1 HKCU ${LINQ_REG_KEY} ""
      ReadRegStr $R2 HKCU ${LINQ_REG_KEY} "Version"
      ReadRegStr $R3 HKCU "${LINQ_UNINSTALL_KEY}" "UninstallString"
      Goto try_uninstall

  linq_hklm:
      ReadRegStr $R1 HKLM ${LINQ_REG_KEY} ""
      ReadRegStr $R2 HKLM ${LINQ_REG_KEY} "Version"
      ReadRegStr $R3 HKLM "${LINQ_UNINSTALL_KEY}" "UninstallString"

  ; If previous version exists .. remove
  try_uninstall:
    StrCmp $R1 "" done
      ; Version key started with 0.8.2. Prior versions can't be 
      ; automaticlly uninstalled.
      StrCmp $R2 "" uninstall_problem
        ; Check if we have uninstall string..
        IfFileExists $R3 0 uninstall_problem
          ; Have uninstall string.. go ahead and uninstall.
          SetOverwrite on
          ; Need to copy uninstaller outside of the install dir
          ClearErrors
          CopyFiles /SILENT $R3 "$TEMP\${LINQ_UNINST_EXE}"
          SetOverwrite off
          IfErrors uninstall_problem
            ; Ready to uninstall..
            ClearErrors
	    ExecWait '"$TEMP\${LINQ_UNINST_EXE}" /S _?=$R1'
	    IfErrors exec_error
              Delete "$TEMP\${LINQ_UNINST_EXE}"
	      Goto done

	    exec_error:
              Delete "$TEMP\${LINQ_UNINST_EXE}"
              Goto uninstall_problem

        uninstall_problem:
	  ; In this case just wipe out previous LinQ install dir..
          MessageBox MB_YESNO $(LINQ_PROMPT_WIPEOUT) IDYES do_wipeout IDNO cancel_install
          cancel_install:
            Quit

          do_wipeout:
            StrCmp $R0 "HKLM" linq_del_lm_reg linq_del_cu_reg
            linq_del_cu_reg:
              DeleteRegKey HKCU ${LINQ_REG_KEY}
              Goto uninstall_prob_cont
            linq_del_lm_reg:
              DeleteRegKey HKLM ${LINQ_REG_KEY}

            uninstall_prob_cont:
	      RMDir /r "$R1"

  done:
SectionEnd

;--------------------------------
;LinQ Install Section

Section SecLinQ
  SectionIn 1 RO

  ; Check install rights..
  Call CheckUserInstallRights
  Pop $R0

  ; Get GTK+ lib dir if we have it..

  StrCmp $R0 "NONE" linq_none
  StrCmp $R0 "HKLM" linq_hklm linq_hkcu

  linq_hklm:
    ReadRegStr $R1 HKLM ${GTK_REG_KEY} "Path"
    WriteRegStr HKLM "${HKLM_APP_PATHS_KEY}" "Path" "$R1\lib"
    WriteRegStr HKLM ${LINQ_REG_KEY} "" "$INSTDIR"
    WriteRegStr HKLM ${LINQ_REG_KEY} "Version" "${LINQ_VERSION}"
    WriteRegStr HKLM "${LINQ_UNINSTALL_KEY}" "DisplayName" $(LINQ_UNINSTALL_DESC)
    WriteRegStr HKLM "${LINQ_UNINSTALL_KEY}" "UninstallString" "$INSTDIR\${LINQ_UNINST_EXE}"
    ; Sets scope of the desktop and Start Menu entries for all users.
    SetShellVarContext "all"
    Goto linq_install_files

  linq_hkcu:
    ReadRegStr $R1 HKCU ${GTK_REG_KEY} "Path"
    StrCmp $R1 "" 0 linq_hkcu1
      ReadRegStr $R1 HKLM ${GTK_REG_KEY} "Path"
    linq_hkcu1:
    WriteRegStr HKCU ${LINQ_REG_KEY} "" "$INSTDIR"
    WriteRegStr HKCU ${LINQ_REG_KEY} "Version" "${LINQ_VERSION}"
    WriteRegStr HKCU "${LINQ_UNINSTALL_KEY}" "DisplayName" $(LINQ_UNINSTALL_DESC)
    WriteRegStr HKCU "${LINQ_UNINSTALL_KEY}" "UninstallString" "$INSTDIR\${LINQ_UNINST_EXE}"
    Goto linq_install_files

  linq_none:
    ReadRegStr $R1 HKLM ${GTK_REG_KEY} "Path"

  linq_install_files:
    SetOutPath "$INSTDIR"
    ; LinQ files
    SetOverwrite on
    File /r .\win32-install-dir\*.*

    ; If we don't have install rights and no hklm GTK install.. then Start in lnk property should
    ; remain linq dir.. otherwise it should be set to the GTK lib dir. (to avoid dll hell)
    StrCmp $R0 "NONE" 0 startin_gtk
      StrCmp $R1 "" startin_linq
    startin_gtk:
      SetOutPath "$R1\lib"
    startin_linq:
    CreateDirectory "$SMPROGRAMS\LinQ"
    CreateShortCut "$SMPROGRAMS\LinQ\LinQ.lnk" "$INSTDIR\linq.exe"
    CreateShortCut "$DESKTOP\LinQ.lnk" "$INSTDIR\linq.exe"
    SetOutPath "$INSTDIR"

    ; If we don't have install rights.. we're done
    StrCmp $R0 "NONE" done
    CreateShortCut "$SMPROGRAMS\LinQ\Uninstall.lnk" "$INSTDIR\${LINQ_UNINST_EXE}"
    SetOverwrite off

    ; write out uninstaller
    SetOverwrite on
    WriteUninstaller "$INSTDIR\${LINQ_UNINST_EXE}"
    SetOverwrite off

  done:
SectionEnd ; end of default LinQ section


;--------------------------------
;Uninstaller Section


Section Uninstall
  Call un.CheckUserInstallRights
  Pop $R0
  StrCmp $R0 "NONE" no_rights
  StrCmp $R0 "HKCU" try_hkcu try_hklm

  try_hkcu:
    ReadRegStr $R0 HKCU ${LINQ_REG_KEY} ""
    StrCmp $R0 $INSTDIR 0 cant_uninstall
      ; HKCU install path matches our INSTDIR.. so uninstall
      DeleteRegKey HKCU ${LINQ_REG_KEY}
      DeleteRegKey HKCU "${LINQ_UNINSTALL_KEY}"
      Goto cont_uninstall

  try_hklm:
    ReadRegStr $R0 HKLM ${LINQ_REG_KEY} ""
    StrCmp $R0 $INSTDIR 0 try_hkcu
      ; HKLM install path matches our INSTDIR.. so uninstall
      DeleteRegKey HKLM ${LINQ_REG_KEY}
      DeleteRegKey HKLM "${LINQ_UNINSTALL_KEY}"
      DeleteRegKey HKLM "${HKLM_APP_PATHS_KEY}"
      ; Sets start menu and desktop scope to all users..
      SetShellVarContext "all"

  cont_uninstall:
    RMDir /r "$INSTDIR\locale"
    RMDir /r "$INSTDIR\face"
    RMDir /r "$INSTDIR\pixmaps"
    Delete "$INSTDIR\*.dll"
    Delete "$INSTDIR\linq.exe"
    Delete "$INSTDIR\${LINQ_UNINST_EXE}"
    ;Try to remove LinQ install dir .. if empty
    RMDir "$INSTDIR"

    ; Shortcuts..
    RMDir /r "$SMPROGRAMS\LinQ"
    Delete "$DESKTOP\LinQ.lnk"

    Goto done

  cant_uninstall:
    MessageBox MB_OK $(un.LINQ_UNINSTALL_ERROR_1) IDOK
    Quit

  no_rights:
    MessageBox MB_OK $(un.LINQ_UNINSTALL_ERROR_2) IDOK
    Quit

  done:
  ;Display the Finish header
  !insertmacro MUI_UNFINISHHEADER
SectionEnd ; end of uninstall section


;--------------------------------
;Functions

Function CheckUserInstallRights
	ClearErrors
	UserInfo::GetName
	IfErrors Win9x
	Pop $0
	UserInfo::GetAccountType
	Pop $1

	StrCmp $1 "Admin" 0 +3
                StrCpy $1 "HKLM"
		Goto done
	StrCmp $1 "Power" 0 +3
                StrCpy $1 "HKLM"
		Goto done
	StrCmp $1 "User" 0 +3
		StrCpy $1 "HKCU"
		Goto done
	StrCmp $1 "Guest" 0 +3
		StrCpy $1 "NONE"
		Goto done
	; Unknown error
	StrCpy $1 "NONE"
        Goto done

	Win9x:
		StrCpy $1 "HKLM"

	done:
        Push $1
FunctionEnd

Function un.CheckUserInstallRights
	ClearErrors
	UserInfo::GetName
	IfErrors Win9x
	Pop $0
	UserInfo::GetAccountType
	Pop $1
	StrCmp $1 "Admin" 0 +3
                StrCpy $1 "HKLM"
		Goto done
	StrCmp $1 "Power" 0 +3
                StrCpy $1 "HKLM"
		Goto done
	StrCmp $1 "User" 0 +3
		StrCpy $1 "HKCU"
		Goto done
	StrCmp $1 "Guest" 0 +3
		StrCpy $1 "NONE"
		Goto done
	; Unknown error
	StrCpy $1 "NONE"
        Goto done

	Win9x:
		StrCpy $1 "HKLM"

	done:
        Push $1
FunctionEnd

;
; Usage:
;   Push $0 ; Path string
;   Call VerifyDir
;   Pop $0 ; 0 - Bad path  1 - Good path
;
Function VerifyDir
  Pop $0
  Loop:
    IfFileExists $0 dir_exists
    StrCpy $1 $0 ; save last
    Push $0
    Call GetParent
    Pop $0
    StrLen $2 $0
    ; IfFileExists "C:" on xp returns true and on win2k returns false
    ; So we're done in such a case..
    StrCmp $2 "2" loop_done
    Goto Loop

  loop_done:
    StrCpy $1 "$0\LInQFooB"
    ; Check if we can create dir on this drive..
    ClearErrors
    CreateDirectory $1
    IfErrors DirBad DirGood

  dir_exists:
    ClearErrors
    FileOpen $1 "$0\linqfoo.bar" w
    IfErrors PathBad PathGood

    DirGood:
      RMDir $1
      Goto PathGood1

    DirBad:
      RMDir $1
      Goto PathBad1

    PathBad:
      FileClose $1
      Delete "$0\linqfoo.bar"
      PathBad1:
      StrCpy $0 "0"
      Push $0
      Return

    PathGood:
      FileClose $1
      Delete "$0\linqfoo.bar"
      PathGood1:
      StrCpy $0 "1"
      Push $0
FunctionEnd

Function .onVerifyInstDir
  Push $INSTDIR
  Call VerifyDir
  Pop $0
  StrCmp $0 "0" 0 dir_good
    Abort
  dir_good:
FunctionEnd

; GetParent
; input, top of stack  (e.g. C:\Program Files\Poop)
; output, top of stack (replaces, with e.g. C:\Program Files)
; modifies no other variables.
;
; Usage:
;   Push "C:\Program Files\Directory\Whatever"
;   Call GetParent
;   Pop $R0
;   ; at this point $R0 will equal "C:\Program Files\Directory"
Function GetParent
   Exch $0 ; old $0 is on top of stack
   Push $1
   Push $2
   StrCpy $1 -1
   loop:
     StrCpy $2 $0 1 $1
     StrCmp $2 "" exit
     StrCmp $2 "\" exit
     IntOp $1 $1 - 1
   Goto loop
   exit:
     StrCpy $0 $0 $1
     Pop $2
     Pop $1
     Exch $0 ; put $0 on top of stack, restore $0 to original value
FunctionEnd


; CheckGtkVersion
; inputs: Push 2 GTK+ version strings to check. The major and minor values
; need to be equal, for success.  If the micro val to check is equal or greater
; to the refrence micro value, then we have success.
;
; Usage:
;   Push "2.2.0"  ; Refrence version
;   Push "2.2.1"  ; Version to check
;   Call CheckGtkVersion
;   Pop $R0
;   $R0 will now equal "0", because 2.2.0 is less than 2.2.1
;
Function CheckGtkVersion
  ; Version we want to check
  Pop $6 
  ; Reference version
  Pop $8 

  ; Check that the string to check is at least 5 chars long (i.e. x.x.x)
  StrLen $7 $6
  IntCmp $7 5 0 bad_version

  ; Major version check
  StrCpy $7 $6 1
  StrCpy $9 $8 1
  IntCmp $7 $9 check_minor
    Goto bad_version

  check_minor:
    StrCpy $7 $6 1 2
    StrCpy $9 $8 1 2
    IntCmp $7 $9 check_micro
      Goto bad_version

  check_micro:
    StrCpy $7 $6 1 4
    StrCpy $9 $8 1 4
    IntCmp $7 $9 good_version bad_version good_version

  bad_version:
    StrCpy $6 "0"
    Push $6
    Goto done

  good_version:
    StrCpy $6 "1"
    Push $6
  done:
FunctionEnd

;
; Usage:
; Call DoWeNeedGtk
; First Pop:
;   0 - We have the correct version
;       Second Pop: Key where Version was found
;   1 - We have an old version that needs to be upgraded
;       Second Pop: HKLM or HKCU depending on where GTK was found.
;   2 - We don't have Gtk+ at all
;       Second Pop: "NONE, HKLM or HKCU" depending on our rights..
;
Function DoWeNeedGtk
  ; Logic should be:
  ; - Check what user rights we have (HKLM or HKCU)
  ;   - If HKLM rights..
  ;     - Only check HKLM key for GTK+
  ;       - If installed to HKLM, check it and return.
  ;   - If HKCU rights..
  ;     - First check HKCU key for GTK+
  ;       - if good or bad exists stop and ret.
  ;     - If no hkcu gtk+ install, check HKLM
  ;       - If HKLM ver exists but old, return as if no ver exits.
  ;   - If no rights
  ;     - Check HKLM

  Call CheckUserInstallRights
  Pop $3
  StrCmp $3 "HKLM" check_hklm
  StrCmp $3 "HKCU" check_hkcu check_hklm
    check_hkcu:
      ReadRegStr $0 HKCU ${GTK_REG_KEY} "Version"
      StrCpy $5 "HKCU"
      StrCmp $0 "" check_hklm have_gtk

    check_hklm:
      ReadRegStr $0 HKLM ${GTK_REG_KEY} "Version"
      StrCpy $5 "HKLM"
      StrCmp $0 "" no_gtk have_gtk


  have_gtk:
    ; GTK+ is already installed.. check version.
    StrCpy $1 ${GTK_VERSION} ; Minimum GTK+ version needed
    Push $1
    Push $0
    Call CheckGtkVersion
    Pop $2
    StrCmp $2 "1" good_version bad_version
    bad_version:
      ; Bad version. If hklm ver and we have hkcu or no rights.. return no gtk
      StrCmp $3 "NONE" no_gtk  ; if no rights.. can't upgrade
      StrCmp $3 "HKCU" 0 upgrade_gtk ; if HKLM can upgrade..
        StrCmp $5 "HKLM" no_gtk upgrade_gtk ; have hkcu rights.. if found hklm ver can't upgrade..
  
      upgrade_gtk:
        StrCpy $2 "1"
        Push $5
        Push $2
        Goto done

  good_version:
    ; Just make sure we have it. There was a gtk+ uninstaller that 
    ; left behind reg entries after uninstalling..
    StrCmp $5 "HKLM" have_hklm_gtk have_hkcu_gtk
      have_hkcu_gtk:
        ; Have HKCU version
        ReadRegStr $4 HKCU ${GTK_REG_KEY} "Path"
        StrCpy $1 "$4\${GTK_INSTALL_VERIFIER}"
        IfFileExists $1 good_version_verified
          DeleteRegKey HKCU ${GTK_REG_KEY}
          Goto no_gtk

      have_hklm_gtk:
        ReadRegStr $4 HKLM ${GTK_REG_KEY} "Path"
        StrCpy $1 "$4\${GTK_INSTALL_VERIFIER}"
        IfFileExists $1 good_version_verified
          DeleteRegKey HKLM ${GTK_REG_KEY}
          Goto no_gtk

    good_version_verified:
      StrCpy $2 "0"
      Push $4  ; The path to existing GTK+
      Push $2
      Goto done

    no_gtk:
      StrCpy $2 "2"
      Push $3 ; our rights
      Push $2
      Goto done

  done:
FunctionEnd

Function .onInit
  ; If this installer dosn't have GTK, check whether we need it.
    Call DoWeNeedGtk
    Pop $0
    Pop $1

    StrCmp $0 "0" have_gtk need_gtk
    need_gtk:
      MessageBox MB_OK $(GTK_INSTALLER_NEEDED) IDOK
      Quit
    have_gtk:

  Call CheckUserInstallRights
  Pop $0

  StrCmp $0 "HKLM" 0 user_dir
    StrCpy $INSTDIR "$PROGRAMFILES\LinQ"
    Goto instdir_done
  user_dir:
    StrCpy $2 "$SMPROGRAMS"
    Push $2
    Call GetParent
    Call GetParent
    Pop $2
    StrCpy $INSTDIR "$2\LinQ"

  instdir_done:

FunctionEnd
