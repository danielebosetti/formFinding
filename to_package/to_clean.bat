del *.zip



REM release bin
SET BIN_RELEASE=C:\Users\danielebosetti.hp\Desktop\CO_FormFinding2\trunk\x64\Release\\
SET BIN_PACKAGE=C:\Users\danielebosetti.hp\Desktop\CO_FormFinding2\trunk\to_package\package_dir\bin\\

REM settings dir
SET SETTINGS_RELEASE=C:\Users\danielebosetti.hp\Desktop\CO_FormFinding2\trunk\x64\Release\settings\\
SET SETTINGS_PACKAGE=C:\Users\danielebosetti.hp\Desktop\CO_FormFinding2\trunk\to_package\package_dir\settings\\

REM settings dir
SET DATA_RELEASE=C:\Users\danielebosetti.hp\Desktop\CO_FormFinding2\trunk\x64\Release\IO_txt\\
SET DATA_PACKAGE=C:\Users\danielebosetti.hp\Desktop\CO_FormFinding2\trunk\to_package\package_dir\IO_txt\\

REM BAT-s dir
SET BAT_RELEASE=C:\Users\danielebosetti.hp\Desktop\CO_FormFinding2\trunk\bats\\
SET BAT_PACKAGE=C:\Users\danielebosetti.hp\Desktop\CO_FormFinding2\trunk\to_package\package_dir\\





REM clean bin
del "%BIN_PACKAGE%*.dll"
del "%BIN_PACKAGE%*.exe"


REM clean settings
del "%SETTINGS_PACKAGE%*.dat"
del "%SETTINGS_PACKAGE%*.ini"
del "%SETTINGS_PACKAGE%*.txt"

REM sample data file
del "%DATA_PACKAGE%*.txt"

REM BAT file
del "%BAT_PACKAGE%run_ff_gui.bat"
del "%BAT_PACKAGE%run_ff_cmd.bat"
del "%BAT_PACKAGE%run_ff_flattening.bat"
del "%BAT_PACKAGE%run_flattening.bat"
del "%BAT_PACKAGE%run_sgm.bat"



