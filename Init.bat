@ECHO OFF

SET "LIB="
SET VC_PATH=C:\Program Files (x86)\Microsoft Visual Studio\2019\Community

IF NOT DEFINED "%LIB%" (IF EXIST "%VC_PATH%" (call "%VC_PATH%\VC\Auxiliary\Build\vcvarsall.bat" x64))
