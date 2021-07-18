cls 
REM --------------------- Folder to compress -----------------------------------

set name= Motion
set dir="C:\sw\"%name% 
REM ----------------------------------------------------------------------------

REM Path to WinRAR executable in Program Files. Change if location is different
REM ---------------------- WinRAR Directory ------------------------------------
set path="C:\Program Files\WinRAR\";%path%
REM ----------------------------------------------------------------------------


echo.
echo Folder to compress in *.RAR format:   
echo %dir%

rem Get the datetime in a format that can go in a filename.
set _my_datetime=%date%_%time%
set _my_datetime=%_my_datetime: =_%
set _my_datetime=%_my_datetime::=%
set _my_datetime=%_my_datetime:/=_%
set _my_datetime=%_my_datetime:.=_%

rem Now use the timestamp by in a new ZIP file name.
REM "d:\Program Files\7-Zip\7z.exe" a -r Code_%_my_datetime%.zip Code

REM 4. Compress all files in directory and subdirectories into a single archive
echo. 
echo.
echo Today's date and time will be added to the base filename
set excl=*.ncb *.zip *.obj *.pch *.pdb *.idb *.tlh *.tli *.aps *.opt *.lib *.bsc *.sbr *.ilk *.map *.cdf *.dbf *.dat *.bak *CVS/*.*
set incl=*.*

rar a -r %name%_%_my_datetime%.rar   -x*.obj -x*.ncb -x*.zip  -x*.pch -x*.pdb -x*.idb -x*.tlh -x*.tli -x*.aps -x*.opt -x*.lib -x*.bsc -x*.sbr -x*.ilk -x*.map -x*.cdf -x*.dbf -x*.dat -x*.bak -x*CVS/*.*  -x*\Debug -x*\Release -x*.vs -x*\ipch

echo.
echo "Task Completed"
echo.

REM --------------------------- exit -----------------------------------------
:end
REM EXIT /B 0
echo ready.
REM pause