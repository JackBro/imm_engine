@set VS_EX_PATH_INCLUDE=%VS_EX_PATH_INCLUDE%D:\Ashlotte\imm_library
@set VS_EX_PATH_INCLUDE=%VS_EX_PATH_INCLUDE%;D:\Ashlotte\imm_library_third_party
@set VS_EX_PATH_INCLUDE=%VS_EX_PATH_INCLUDE%;D:\Ashlotte\luna12_common
@set VS_EX_PATH_LIB=%VS_EX_PATH_LIB%;D:\Ashlotte\luna12_common
@if not "%VS_EX_PATH_FLAG%"=="1" set INCLUDE=%INCLUDE%%VS_EX_PATH_INCLUDE%;
@if not "%VS_EX_PATH_FLAG%"=="1" set LIB=%LIB%%VS_EX_PATH_LIB%;
@set VS_EX_PATH_FLAG=1
cl /EHsc /MDd /Zi /W4 /fp:fast main.cc
::cl /EHsc /MD /Ox /W4 /fp:fast /GL main.cc