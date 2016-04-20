@if not "%IMM_SET%"=="1" set IMM_INCLUDE=%IMM_INCLUDE%D:\Ashlotte\imm_include;
@if not "%IMM_SET%"=="1" set IMM_INCLUDE=%IMM_INCLUDE%D:\Ashlotte\imm_include_3rd_party;
@if not "%IMM_SET%"=="1" set IMM_INCLUDE=%IMM_INCLUDE%D:\Ashlotte\common_3rd_party;
@if not "%IMM_SET%"=="1" set IMM_LIB=%IMM_LIB%D:\Ashlotte\common_3rd_party;
@if not "%IMM_SET%"=="1" set IMM_INCLUDE=%IMM_INCLUDE%C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Include;
@if not "%IMM_SET%"=="1" set INCLUDE=%INCLUDE%%IMM_INCLUDE%
@if not "%IMM_SET%"=="1" set LIB=%LIB%%IMM_LIB%
@set IMM_SET=1
::Debug
cl /EHsc /MDd /Zi /W4 /fp:fast /bigobj main.cc
::Relase
::cl /EHsc /MD /Ox /W4 /fp:fast /GL main.cc
