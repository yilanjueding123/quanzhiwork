::----------------------------------------------------------------
:: create by james at 2011-09-01
::----------------------------------------------------------------

@ECHO OFF

echo make image begin...

set BUILDTOOLPATH=..\..\..\..\eStudio\Softwares\mod_update
set DRAGONTOOLPATH=..\..\..\..\eStudio\Softwares\eDragonEx200

::----------------------------------------------------------------
:: 更新系统文件
::----------------------------------------------------------------
call update.bat > update.txt

::----------------------------------------------------------------
:: 解析配置脚本
::----------------------------------------------------------------
if exist ..\eFex\sys_config.bin del ..\eFex\sys_config.bin
%BUILDTOOLPATH%\script.exe ..\eFex\sys_config.fex

::----------------------------------------------------------------
:: 更新BOOT0和BOOT1文件头信息
::----------------------------------------------------------------
copy ..\eGon\storage_media\sdcard\boot0.bin boot0.fex
copy ..\eGon\storage_media\sdcard\boot1.bin boot1.fex
%BUILDTOOLPATH%\update.exe ..\eFex\sys_config.bin boot0.fex boot1.fex SDMMC_CARD

::----------------------------------------------------------------
:: 更新MBR
::----------------------------------------------------------------
%BUILDTOOLPATH%\script_old.exe part_config.fex
%BUILDTOOLPATH%\update_mbr.exe part_config.bin mbr.fex

::----------------------------------------------------------------
:: 打包镜像文件
::----------------------------------------------------------------
call fsbuild.bat

::----------------------------------------------------------------
:: 生成烧写文件校验和
::----------------------------------------------------------------
%DRAGONTOOLPATH%\FileAddSum.exe rootfs.fex ..\eFex\verify.fex

::----------------------------------------------------------------
:: 生成烧写镜像包文件
::----------------------------------------------------------------
if exist PHOENIXv100.img del PHOENIXv100.img
%DRAGONTOOLPATH%\compile -o image.bin image.cfg
%DRAGONTOOLPATH%\dragon image.cfg > image.txt

::----------------------------------------------------------------
:: 清理临时文件
::----------------------------------------------------------------
del rootfs.fex
del dlinfo.fex
del mbr.fex
del boot0.fex
del boot1.fex
del ..\eFex\sys_config.bin
del part_config.bin

echo make image finished...

PAUSE
