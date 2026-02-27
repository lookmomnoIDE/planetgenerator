@echo off
REM Define the target directory name relative to the batch file's location
SET "TARGET_DIR=C:\projects\LiteEngine\build"

REM Change the current directory to the location of the batch file
CD /D "%~dp0"

REM Check if the target directory exists
IF EXIST "%TARGET_DIR%\" (
    ECHO Cleaning the contents of: %TARGET_DIR%
    
    REM Delete all files recursively (/S) and quietly (/Q), force deletion of read-only files (/F)
    DEL /S /Q /F "%TARGET_DIR%\*.*"

    REM Remove all subdirectories recursively (/S) and quietly (/Q)
    FOR /D %%i IN ("%TARGET_DIR%\*") DO (
        IF EXIST "%%i" RD /S /Q "%%i"
    )
    
    ECHO Cleaning complete.
) ELSE (
    ECHO Target directory "%TARGET_DIR%" not found.
)

PAUSE
