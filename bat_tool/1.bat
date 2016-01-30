@if not "%VS_EX_SET%"=="1" set VS_EX_INCLUDE=%VS_EX_INCLUDE%D:\Ashlotte\imm_include;
@if not "%VS_EX_SET%"=="1" set VS_EX_INCLUDE=%VS_EX_INCLUDE%D:\Ashlotte\imm_include_3rd_party;
@if not "%VS_EX_SET%"=="1" set VS_EX_INCLUDE=%VS_EX_INCLUDE%D:\Ashlotte\common_3rd_party;
@if not "%VS_EX_SET%"=="1" set VS_EX_LIB=%VS_EX_LIB%D:\Ashlotte\common_3rd_party;
@if not "%VS_EX_SET%"=="1" set VS_EX_INCLUDE=%VS_EX_INCLUDE%C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Include;
@if not "%VS_EX_SET%"=="1" set INCLUDE=%INCLUDE%%VS_EX_INCLUDE%
@if not "%VS_EX_SET%"=="1" set LIB=%LIB%%VS_EX_LIB%
@set VS_EX_SET=1
::Debug
cl /EHsc /MDd /Zi /W4 /fp:fast /bigobj main.cc
::Relase
::cl /EHsc /MD /Ox /W4 /fp:fast /GL main.cc
