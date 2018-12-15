@echo off
cls

REM folder containing the exe
REM leave the double \\ at the end of folder paths!!!!
set FFBIN=C:\Users\danielebosetti.hp\Desktop\CO_FormFinding2\trunk\to_package\package_dir\bin\\
REM folder containing the data (input and output)
set FFDATA=C:\Users\danielebosetti.hp\Desktop\CO_FormFinding2\trunk\to_package\package_dir\IO_txt\\
REM the folder containing the setting files
set FFSETTINGS=C:\Users\danielebosetti.hp\Desktop\CO_FormFinding2\trunk\to_package\package_dir\settings\\

REM the input file name
set FILE_INPUT=sample001small.txt

REM don t change below!
set FFEXE=flattening.exe

set FFPARAMS=
set FFPARAMS=%FFPARAMS% APPLICATION_DIR "%FFBIN%"
set FFPARAMS=%FFPARAMS% DATA_DIR "%FFDATA%"
set FFPARAMS=%FFPARAMS% SETTINGS_DIR "%FFSETTINGS%"
set FFPARAMS=%FFPARAMS% FILE_INPUT "%FILE_INPUT%"
set FFPARAMS=%FFPARAMS% FILE_OUTPUT "outdebug_1small.txt"




set RUN_CMD="%FFBIN%%FFEXE%" %FFPARAMS%
echo on
%RUN_CMD%
PAUSE