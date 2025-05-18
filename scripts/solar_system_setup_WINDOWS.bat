@echo off
setlocal

set "SUBFOLDER=bin"
set "EXECUTABLE=generator.exe"

set "SPHERE_ARGS=sphere 1 100 100 sphere.3d"
set "TORUS_ARGS=torus 1.0 1.25 100 100 torus.3d"
set "PATCH_ARGS=patch teapot.patch 20 patch.3d"

:: generate sphere
"%CD%\%SUBFOLDER%\%EXECUTABLE%" %SPHERE_ARGS%

:: generate torus
"%CD%\%SUBFOLDER%\%EXECUTABLE%" %TORUS_ARGS%

:: generate patch 
"%CD%\%SUBFOLDER%\%EXECUTABLE%" %PATCH_ARGS%

endlocal