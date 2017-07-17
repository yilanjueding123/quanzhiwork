::----------------------------------------------------------------
:: create by james at 2011-09-01
::----------------------------------------------------------------

@echo off
echo update ramfs
::----------------------------------------------------------------
:: update ramfs
::----------------------------------------------------------------
@copy .\..\ramfs\pwm_cfg.ini            .\ramfs\pwm_cfg.ini
@copy .\..\ramfs\drv\twi.drv            .\ramfs\drv\twi.drv
@copy .\..\ramfs\drv\rtc.drv            .\ramfs\drv\rtc.drv
@copy .\..\ramfs\drv\power2.drv          .\ramfs\drv\power.drv
    @copy .\..\ramfs\drv\spinor.drv     .\ramfs\drv\spinor.drv
@copy .\..\ramfs\drv\spi.drv            .\ramfs\drv\spi.drv
@copy .\..\ramfs\drv\monitor.drv        .\ramfs\drv\monitor.drv
@copy .\..\ramfs\drv\power_cfg.ini      .\ramfs\drv\power_cfg.ini
@copy .\..\ramfs\mod\slib.mod           .\ramfs\mod\slib.mod
@copy .\..\rootfs\drv\sdmmc.drv         .\ramfs\drv\sdmmc.drv
@copy .\..\beetles\ramfs\shell.zgj       .\ramfs\shell.zgj
@xcopy .\..\beetles\rootfs\apps        .\rootfs\apps  /s/i/yz
@xcopy .\..\beetles\rootfs\res        .\rootfs\res  /s/i/yz

echo update rootfs
::----------------------------------------------------------------
:: update rootfs
::----------------------------------------------------------------
@copy .\..\ramfs\pwm_cfg.ini            .\rootfs\pwm_cfg.ini
@copy .\..\ramfs\drv\key.drv      	.\rootfs\drv\key.drv
@copy .\..\rootfs\epos.img              .\rootfs\epos.img
@copy .\..\rootfs\drv\display.drv       .\rootfs\drv\display.drv
@copy .\..\rootfs\drv\audio.drv         .\rootfs\drv\audio.drv
@copy .\..\rootfs\drv\ir.drv         .\rootfs\drv\ir.drv
@copy .\..\rootfs\drv\usb_host0.drv         .\rootfs\drv\usb_host0.drv
@copy .\..\rootfs\drv\usbd_msc.drv         .\rootfs\drv\usbd_msc.drv
@copy .\..\rootfs\drv\sdmmc.drv         .\rootfs\drv\sdmmc.drv
@copy .\..\rootfs\mod\charset.mod       .\rootfs\mod\charset.mod
@copy .\..\rootfs\mod\charset.bin       .\rootfs\mod\charset.bin
@copy .\..\rootfs\mod\charset.mod        .\rootfs\mod\charset.mod
@copy .\..\rootfs\mod\orange.mod        .\rootfs\mod\orange.mod
@copy .\..\rootfs\mod\update.mod        .\rootfs\mod\update.mod
@copy .\..\beetles\rootfs\mod\desktop.mod        .\rootfs\mod\desktop.mod
