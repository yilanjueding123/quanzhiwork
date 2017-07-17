@echo off
    @echo *********************************************
    @echo *   Delete *.svn                            *
    @echo *********************************************
    @echo Are you sure?
    @echo 0: No
    @echo 1: Yes
    @echo *********************************************
    set /p SEL=Please Select:
    if %SEL%==1     goto :conti
    exit

:conti:
    @echo *********************************************
    @echo * Please confirm again                      *
    @echo *********************************************
    @echo 0: No
    @echo 1: Yes
    @echo *********************************************
    set /p SEL=Please Select:
    if %SEL%==1     goto :conti1
    exit
:conti1:

set DelNumber=0

for /d /r  %%b in (.svn) do (
    if exist %%b (
        rd /s /q "%%b"
        set /a DelNumber+=1
    )
)

echo Total delete number : %DelNumber%
pause
