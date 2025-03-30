@echo off
setlocal

set "SUBFOLDER=bin"
set "EXECUTABLE=generator.exe"


set "SPHERE_ARGS=sphere 1 20 20 sphere.3d"
set "CONE_ARGS=cone 1 2 20 20 cone.3d"
set "CYLINDER_ARGS=cylinder 1 2 20 20 cylinder.3d"
set "TORUS_ARGS=torus 0.5 0.25 20 20 torus.3d"
set "BOX_ARGS=box 1 5 box.3d"
set "PLANE_ARGS=plane 1 5 plane.3d"

"%CD%\%SUBFOLDER%\%EXECUTABLE%" %SPHERE_ARGS%
"%CD%\%SUBFOLDER%\%EXECUTABLE%" %CONE_ARGS%
"%CD%\%SUBFOLDER%\%EXECUTABLE%" %CYLINDER_ARGS%
"%CD%\%SUBFOLDER%\%EXECUTABLE%" %TORUS_ARGS%
"%CD%\%SUBFOLDER%\%EXECUTABLE%" %BOX_ARGS%
"%CD%\%SUBFOLDER%\%EXECUTABLE%" %PLANE_ARGS%

endlocal