@if not "%VS_PATH_SETTED%"=="1" set VS_EX_PATH_INCLUDE=%VS_EX_PATH_INCLUDE%C:\Ashlotte\imm_library
@if not "%VS_PATH_SETTED%"=="1" set VS_EX_PATH_INCLUDE=%VS_EX_PATH_INCLUDE%;C:\Ashlotte\imm_library_third_party
@if not "%VS_PATH_SETTED%"=="1" set VS_EX_PATH_INCLUDE=%VS_EX_PATH_INCLUDE%;C:\Ashlotte\include_third_party
@if not "%VS_PATH_SETTED%"=="1" set VS_EX_PATH_LIB=%VS_EX_PATH_LIB%;C:\Ashlotte\include_third_party
@if not "%VS_PATH_SETTED%"=="1" set VS_EX_PATH_INCLUDE=%VS_EX_PATH_INCLUDE%;C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Include
@if not "%VS_PATH_SETTED%"=="1" set INCLUDE=%INCLUDE%%VS_EX_PATH_INCLUDE%;
@if not "%VS_PATH_SETTED%"=="1" set LIB=%LIB%%VS_EX_PATH_LIB%;
@set VS_PATH_SETTED=1
::Debug
cl /EHsc /MDd /Zi /W4 /fp:fast main.cc
::Relase
::cl /EHsc /MD /Ox /W4 /fp:fast /GL main.cc
