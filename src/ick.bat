@rem Wrapper for ickmain.exe that gets around the 126-byte system() limit

@echo off
ickmain %*
if exist ickgcc.rsp gcc @ickgcc.rsp
if exist ickgcc.rsp del ickgcc.rsp
if exist ickyuk.bat call ickyuk.bat
if exist ickyuk.bat del ickyuk.bat
