@echo off
setlocal

set "SUBFOLDER=bin"
set "EXECUTABLE=generator.exe"

set "SPHERE_ARGS=sphere 1 20 20 sphere.3d"
set "TORUS_ARGS=torus 1.0 1.25 20 20 torus.3d"
set "PATCH_ARGS=patch teapot.patch 9 patch.3d"

:: generate sphere
"%CD%\%SUBFOLDER%\%EXECUTABLE%" %SPHERE_ARGS%

:: generate torus
"%CD%\%SUBFOLDER%\%EXECUTABLE%" %TORUS_ARGS%

:: generate patch 
"%CD%\%SUBFOLDER%\%EXECUTABLE%" %PATCH_ARGS%

endlocal