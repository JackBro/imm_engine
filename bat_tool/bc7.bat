for /f "tokens=*" %%i in ('dir /b *.png') do BC6HBC7EncoderCS.exe /bc7 "%%i"
pause