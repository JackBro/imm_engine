@set VS_EX_PATH_INCLUDE=%VS_EX_PATH_INCLUDE%C:\Ashlotte\imm_library
@set VS_EX_PATH_INCLUDE=%VS_EX_PATH_INCLUDE%;C:\Ashlotte\imm_library_third_party
@set VS_EX_PATH_INCLUDE=%VS_EX_PATH_INCLUDE%;C:\Ashlotte\header_files_third_party
@set VS_EX_PATH_LIB=%VS_EX_PATH_LIB%;C:\Ashlotte\header_files_third_party
@if not "%VS_EX_PATH_FLAG%"=="1" set INCLUDE=%INCLUDE%%VS_EX_PATH_INCLUDE%;
@if not "%VS_EX_PATH_FLAG%"=="1" set LIB=%LIB%%VS_EX_PATH_LIB%;
@set VS_EX_PATH_FLAG=1
::Debug
cl /EHsc /MDd /Zi /W4 /fp:fast main.cc
::Relase
::cl /EHsc /MD /Ox /W4 /fp:fast /GL main.cc