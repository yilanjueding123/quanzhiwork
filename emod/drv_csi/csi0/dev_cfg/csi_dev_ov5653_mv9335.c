
#include  "csi_dev_i.h"

#if(CSI_DEV_TYPE == CSI_DEV_TYPE_OV5653_MV9335)

#define MIRROR 0//0 = normal, 1 = mirror
#define FLIP   0//0 = normal, 1 = flip

//每个device私有特性的配置都在此文件完成
//包括reset pin，power down pin的操作
//mclk的配置
//各个模式下的i2c配置以及对应的csi配置
//思路如下：
//通常sensor的设置有
//1.初始化
//2.从任何模式到模式A
//3.从任何模式到模式B
//...
//因此定义几个参数表：reg_init， reg_A，reg_B，...
//在csi_dev_init函数中一次性配置reg_init
//在需要的时候可以多次配置reg_A或reg_B...

//==================全局变量dev_csi.c要调用========================
//每个模组必须要有这些参数
__bool              bProgressiveSrc = 1;   // Indicating the source is progressive or not
__bool              bTopFieldFirst = 0;    // VPO should check this flag when bProgressiveSrc is FALSE
__u16               eAspectRatio = 1000;     //the source picture aspect ratio

__u32 csi_mclk = 24; //MHz

__csi_mode_t csi_mode;//当前使用模式
__csi_mode_all_t csi_mode_all;//支持的模式集
__csi_conf_t conf;//csi配置参数

__csi_color_effect_t csi_color_effect = CSI_COLOR_EFFECT_NORMAL;   	//当前的csi_color_effect
__csi_awb_t          csi_awb 					= CSI_AWB_AUTO;   						//当前的csi_awb
__csi_ae_t					 csi_ae 					= CSI_AE_0;   								//当前的csi_ae
__csi_bright_t			 csi_bright 			= CSI_BRIGHT_0;   						//当前的csi_bright
__csi_contrast_t		 csi_contrast 		= CSI_CONTRAST_0;   					//当前的csi_contrast
__csi_band_t				 csi_band 				= CSI_BAND_50HZ;   						//当前的csi_band
//==================================================================================

extern ES_FILE   *fiic;//i2c句柄


//==================非全局变量仅在此文件中用到========================
__u32  last_mode_num = 0xff;
__buf_scrab_mode_t  last_buf_mode = CSI_SC_NA;//区分单双csi工作状态，last_buf_mode为双，切换不reset power，单->单不reset，单->双reset一次

//static  __u8 da_5653 = 0x60>>1;//ov5653 slave addr

static  __u8 da_9335 = 0x50 >> 1; //mv9335 slave addr

static  __u8 da_flash = 0x40 >> 1; //mv9335 onchip flash addr

static __u32 reg_step_9335 = 2; //2*16bit

__u8 *flash_bin_addr;
__u32 bin_size;

char BIN_24FPS[] = "d:\\res\\bin\\ischhd24.bin"; //960*1080@30fps
char BIN_30FPS[] = "d:\\res\\bin\\iscfhd30.bin"; //1920*1080@30

char BIN_UPDATE[] = "F:\update_9335.bin";


__u8 *read_data;

static __u8 test[] =
{
    7, 2, 3, 4, 5, 6, 7, 8, 9, 10,
};


static __u8 flash_data__[] =
{
    0x00, 0x00,
};

static __u8 reg_band_50hz[] =
{
    0x00, 0x00,
};


static __u8 reg_band_60hz[] =
{
    0x00, 0x00,
};



static __u8 reg_color_effect_normal[] =
{
    0x00, 0x00,
};

static __u8 reg_color_effect_grayscale[] =
{
    0x00, 0x00,
};

static __u8 reg_color_effect_colorinv[] =
{
    0x00, 0x00,
};

static __u8 reg_color_effect_sepia[] =
{
    0x00, 0x00,
};

static __u8 reg_color_effect_sepiagreen[] =
{
    0x00, 0x00,
};

static __u8 reg_color_effect_sepiablue[] =
{
    0x00, 0x00,
};

static __u8 reg_color_effect_sepiared[] =
{
    0x00, 0x00,
};

static __u8 reg_color_effect_sepiayellow[] =
{
    0x00, 0x00,
};



static __u8 reg_awb_auto[] =
{
    0x00, 0x00,
};

static __u8 reg_awb_daylight[] = //6500k
{
    0x00, 0x00,
};

static __u8 reg_awb_fluorescent[] = //5000k
{
    0x00, 0x00,
};

static __u8 reg_awb_cloudy[] = //7500k
{
    0x00, 0x00,
};

static __u8 reg_awb_tungsten[] = //
{
    0x00, 0x00,
};



static __u8 reg_ae_n3[] = //-3
{
    0x00, 0x00,
};

static __u8 reg_ae_n2[] = //-2
{
    0x00, 0x00,
};

static __u8 reg_ae_n1[] = //-1
{
    0x00, 0x00,
};

static __u8 reg_ae_0[] = //0
{
    0x00, 0x00,
};

static __u8 reg_ae_p1[] = //1
{
    0x00, 0x00,
};

static __u8 reg_ae_p2[] = //2
{
    0x00, 0x00,
};

static __u8 reg_ae_p3[] = //3
{
    0x00, 0x00,
};



static __u8 reg_bright_n3[] = //-3
{
    0x00, 0x00,
};

static __u8 reg_bright_n2[] = //-2
{
    0x00, 0x00,
};

static __u8 reg_bright_n1[] = //-1
{
    0x00, 0x00,
};

static __u8 reg_bright_0[] = //0
{
    0x00, 0x00,
};

static __u8 reg_bright_p1[] = //1
{
    0x00, 0x00,
};

static __u8 reg_bright_p2[] = //2
{
    0x00, 0x00,
};

static __u8 reg_bright_p3[] = //3
{
    0x00, 0x00,
};



static __u8 reg_contrast_n3[] = //-3
{
    0x00, 0x00,
};

static __u8 reg_contrast_n2[] = //-2
{
    0x00, 0x00,
};

static __u8 reg_contrast_n1[] = //-1
{
    0x00, 0x00,
};

static __u8 reg_contrast_0[] = //0
{
    0x00, 0x00,
};

static __u8 reg_contrast_p1[] = //1
{
    0x00, 0x00,
};

static __u8 reg_contrast_p2[] = //2
{
    0x00, 0x00,
};

static __u8 reg_contrast_p3[] = //3
{
    0x00, 0x00,
};


__s32  csi_power_on(void)
{
    csi_rst(0);//rst=0

    csi_pwren(1);//pwr=1
    csi_mclk_on_off(1);//mclk on
    esKRNL_TimeDly(1);

    csi_rst(1);//rst=1

    return EPDK_OK;
}

__s32  csi_power_off(void)
{
    csi_rst(0);//rst=0
    csi_mclk_on_off(0);
    csi_pwren(0);//pwr=0
    return EPDK_OK;
}

__s32  csi_clk_on(void)
{
    csi_mclk_on_off(1);
    return EPDK_OK;
}

__s32  csi_clk_off(void)
{
    csi_mclk_on_off(0);
    return EPDK_OK;
}



__s32 write_flash(__u8 *reg, __u32 length)
{
    __s32 iic_ret = EPDK_FAIL;
    __twi_dev_para_ex_t  siic;
    __u8 iic_addr[1];

    __u32 data_len = length;

    siic.slave_addr        = da_flash;
    siic.slave_addr_flag   = TWI_SLAVE_ADDRESS_07BIT;
    siic.byte_addr         = iic_addr;
    siic.byte_addr_width   = 0;
    siic.data              = reg;
    siic.byte_count        = data_len;

    iic_ret = csi_fioctrl(fiic, TWI_WRITE_SPEC_RS,  0, (void *)&siic);

    __inf("iic write %s, %x\n", (iic_ret == EPDK_OK) ? "OK" : "FAIL", reg[0]);

    return iic_ret;
}

__s32 set_9335_pll(__u32 pclk)//just for preview pclk=48/96/192(MHz)
{
    __s32 iic_ret;
    __u8 data, R5080;
    __u8 R5040, R5041, R5042, R5049;

    //diable pll
    iic_write_8_8(fiic, da_9335, 0x80, 0x00);
    //set mem clock div
    iic_write_8_8(fiic, da_9335, 0xd0, 0x30);

    if(pclk == 0) return EPDK_OK;

    R5080 = 0x03;
    R5040 = 0x01;
    R5041 = 0x40;
    R5042 = 0x05;

    if(pclk == 192) R5049 = 0x03;
    else if(pclk == 96) R5049 = 0x04;
    else R5049 = 0x05; //48MHz

    iic_write_8_8(fiic, da_9335, 0x40, R5040);
    iic_write_8_8(fiic, da_9335, 0x41, R5041);
    iic_write_8_8(fiic, da_9335, 0x42, R5042);
    iic_write_8_8(fiic, da_9335, 0x49, R5049);

    //enable pll
    iic_write_8_8(fiic, da_9335, 0x80, R5080);

}

__s32 load_bin(char *bin_name)
{
    __s32 ret = EPDK_FAIL;

    __hdle   fp;

    fp = esFSYS_fopen(bin_name, "r");

    if(fp)
    {
        __inf("bin_name=%s\n", bin_name);

        bin_size = eLIBs_GetFileSize(bin_name);
        if(bin_size > 0 && bin_size <= 64 * 1024 * 1024) __inf("csi load file OK, size=%d\n", bin_size);
        else __wrn("csi load file size error!\n");

        flash_bin_addr = esMEMS_Balloc(bin_size);
        if(flash_bin_addr)
        {
            __s32 rett;
            rett = esFSYS_fread((__u8 *)flash_bin_addr, 1, bin_size, fp);
            __inf("read file bytes=%d\n", rett);
        }
        else __wrn("csi balloc error!\n");

        esFSYS_fclose(fp);
        esMEMS_Bfree(flash_bin_addr, bin_size);

        esKRNL_TimeDly(200);//delay 200ms
    }
    else
    {
        ret = EPDK_FAIL;
    }

    return ret;
}

__s32 flash_read(void)
{
    __s32 ret = EPDK_FAIL;
    __u32 i = 0;
    __u32 sav_size = 64 * 10; //64*1024;
    __u8 timeout = 0;
    ES_FILE  *fp_sav_bin = (ES_FILE *)0;

    __u32 *sav_bin_addr;

    sav_bin_addr = (__u32 *)esMEMS_Balloc(sav_size);

    __inf("9335 read flash start...\n");

    fp_sav_bin = eLIBs_fopen("F:\\sav.bin", "w"); //eLIBs_fwrite/eLIBs_fclose
    if(!fp_sav_bin)
    {
        __wrn("open writing file fail!!\n");
        return EPDK_FAIL;
    }
    else
    {
        __inf("open writing file ok, handle=%x\n", fp_sav_bin);
    }

    //set pll for burning
    iic_write_8_8(fiic, da_9335, 0x80, 0x00);
    iic_write_8_8(fiic, da_9335, 0xd0, 0x30);
    iic_write_8_8(fiic, da_9335, 0x40, 0x01);
    iic_write_8_8(fiic, da_9335, 0x41, 0x40);
    iic_write_8_8(fiic, da_9335, 0x42, 0x05);
    iic_write_8_8(fiic, da_9335, 0x49, 0x03);
    iic_write_8_8(fiic, da_9335, 0x80, 0x02);

    iic_write_8_8(fiic, da_9335, 0x5b, 0x01);//enter program mode
    //  esKRNL_TimeDly(1);

    iic_write_16_8(fiic, da_flash, 0x00, 0x00, 0x00);//read mode &addr

    //  __inf("start read\n");
    //  __getc();

    for(i = 0; i < sav_size / 4; i = i + 1)
    {
        __u8 tmp[4];
        __s32 ret;
        //ret=
        iic_read_8_8(fiic, da_flash, 0x00, tmp);
        iic_read_8_8(fiic, da_flash, 0x00, tmp + 1);
        iic_read_8_8(fiic, da_flash, 0x00, tmp + 2);
        iic_read_8_8(fiic, da_flash, 0x00, tmp + 3);
        //if(ret!=EPDK_OK)  __inf("iic read fail,tmp_data=%x\n",tmp_data);
        sav_bin_addr[0] = tmp[3] << 24 | tmp[2] << 16 | tmp[1] << 8 | tmp[0];
        __inf("%x write data=%x\n", sav_bin_addr, sav_bin_addr[0]);
        sav_bin_addr += 1;
    }


    ret = eLIBs_fwrite(sav_bin_addr, sav_size, 1, fp_sav_bin);
    if(ret != 1)  __inf("write file fail.\n");
    else __inf("write file OK.\n");

    eLIBs_fclose(fp_sav_bin);

    __inf("stop read\n");
    __getc();
    //__getc();
    iic_write_8_8(fiic, da_9335, 0x5b, 0x00);//exit_prog_mode

    return ret;
}

__s32 flash_burning_and_check(void)
{
    __s32 ret = EPDK_FAIL;
    __u32 i = 0;
    __u8 timeout = 0;

    __inf("9335 write flash start...\n");


    //  //set pll for burning
    iic_write_8_8(fiic, da_9335, 0x80, 0x00);
    iic_write_8_8(fiic, da_9335, 0xd0, 0x30);
    iic_write_8_8(fiic, da_9335, 0x40, 0x01);
    iic_write_8_8(fiic, da_9335, 0x41, 0x40);
    iic_write_8_8(fiic, da_9335, 0x42, 0x05);
    iic_write_8_8(fiic, da_9335, 0x49, 0x03);
    iic_write_8_8(fiic, da_9335, 0x80, 0x02);

    //  __inf("stop 1\n");
    //  __getc();

    //  esKRNL_TimeDly(10);//delay 100ms
    iic_write_8_8(fiic, da_9335, 0x5b, 0x01);//enter program mode
    //  esKRNL_TimeDly(1);//delay 10ms

    //  __inf("stop 2\n");
    //  __getc();

    iic_write_16_8(fiic, da_flash, 0x00, 0x00, 0x02);//erase
    esKRNL_TimeDly(12);//delay 120ms

    //  __inf("stop 3\n");
    //  __getc();

    iic_write_16_8(fiic, da_flash, 0x00, 0x00, 0x50);//addr from 0x0000
    //  __inf("stop 4\n");
    //  __getc();

    //  if( write_flash((__u8 *)flash_bin_addr,bin_size)!=EPDK_OK )
    //    {
    //      __wrn("9335 write flash fail!\n");
    //     // return EPDK_FAIL;
    //    }

    __inf("flash_bin_addr=%x, bin_size=%d\n", flash_bin_addr, bin_size);
    for(i = 0; i < bin_size; i = i + 2)
    {
        ret = iic_write_8_8(fiic, da_flash, flash_bin_addr[i], flash_bin_addr[i + 1]);
        if(ret != EPDK_OK)
        {
            __wrn("flash burning fail!");
            return EPDK_FAIL;
        }
    }
    if(ret == EPDK_OK) __inf("write bin data fininshed\n");

    //  __inf("stop 5\n");
    //  __getc();


    iic_write_8_8(fiic, da_9335, 0x5b, 0x00);//exit program mode

    //  __inf("stop 7\n");
    //  __getc();
    //reset 9335
    csi_rst(0);
    esKRNL_TimeDly(1);
    csi_rst(1);

    //set pll for check sum
    iic_write_8_8(fiic, da_9335, 0x80, 0x00);
    iic_write_8_8(fiic, da_9335, 0xd0, 0x30);
    iic_write_8_8(fiic, da_9335, 0x40, 0x01);
    iic_write_8_8(fiic, da_9335, 0x41, 0x40);
    iic_write_8_8(fiic, da_9335, 0x42, 0x05);
    iic_write_8_8(fiic, da_9335, 0x49, 0x03);
    iic_write_8_8(fiic, da_9335, 0x80, 0x02);

    timeout = 0;
    //wait 9335 ready
    while(timeout <= 20)
    {
        __u8 R02 = 0x00;
        __u8 R03 = 0x00;
        __s32 iicret;

        iicret = iic_read_8_8(fiic, da_9335, 0x02, &R02);
        if(iicret == EPDK_OK) iicret = iic_read_8_8(fiic, da_9335, 0x03, &R03);

        if(iicret == EPDK_OK && R02 == 0x02 && R03 == 0x2c)
        {
            ret = EPDK_OK;
            break;
        }
        else
        {
            ret = EPDK_FAIL;
            esKRNL_TimeDly(1);
            timeout++;
            break;
        }
    }
    if(timeout > 20) __wrn("9335 not ready\n");

    //check sum
    iic_write_8_8(fiic, da_9335, 0x18, 0x01);


    timeout = 0;
    //wait check sum complete
    while(timeout <= 20)
    {
        __u8 R90 = 0x00;
        __u8 R91 = 0x00;
        __s32 iicret;

        iicret = iic_read_8_8(fiic, da_9335, 0x90, &R90);
        if(iicret == EPDK_OK) iicret = iic_read_8_8(fiic, da_9335, 0x91, &R91);

        if(iicret == EPDK_OK && R90 == 0x2d && R91 == 0x01)
        {
            ret = EPDK_OK;
            break;
        }
        else
        {
            ret = EPDK_FAIL;
            esKRNL_TimeDly(1);
            timeout++;
            break;
        }
    }
    if(timeout > 20) __wrn("9335 check sum fail\n");


    if(ret == EPDK_OK)__inf("9335 buring flash and check sum OK.\n");

    return ret;
}

__s32   Init_9335()
{
    __s32 ret = EPDK_FAIL;
    __u8 tmp = 0xff;

    __u32 cfg_size;

    //  __u8 R50XX[4]={0x01,0x40,0x04,0x03};
    __u8 R50XX[4] = {0x00, 0xf8, 0x05, 0x03};

    char cfg_file[] = "F:\cfg_9335.bin";

    __hdle   fp;

    fp = esFSYS_fopen(cfg_file, "r");
    cfg_size = eLIBs_GetFileSize(cfg_file); //must open file first
    __inf("cfg size=%d\n", cfg_size);

    if(cfg_size == 4)
    {
        __inf("cfg OK, size=%d\n", cfg_size);

        esFSYS_fread(&R50XX, 1, 4, fp);
        __inf("R50XX=%x, %x, %x, %x\n", R50XX[0], R50XX[1], R50XX[2], R50XX[3]);
    }
    else __wrn("csi seek no cfg file...\n");

    esFSYS_fclose(fp);

    iic_write_8_8(fiic, da_9335, 0x80, 0x00);
    iic_write_8_8(fiic, da_9335, 0xd0, 0x30);

    iic_write_8_8(fiic, da_9335, 0x40, R50XX[0]);
    iic_write_8_8(fiic, da_9335, 0x41, R50XX[1]);
    iic_write_8_8(fiic, da_9335, 0x42, R50XX[2]);
    iic_write_8_8(fiic, da_9335, 0x49, R50XX[3]);

    iic_write_8_8(fiic, da_9335, 0x80, 0x03);

    //  //init
    //  iic_write_8_8(fiic, da_9335, 0x80, 0x00);
    //  iic_write_8_8(fiic, da_9335, 0xd0, 0x30);
    //
    //  iic_write_8_8(fiic, da_9335, 0x40, 0x01);
    //  iic_write_8_8(fiic, da_9335, 0x41, 0x40);
    //
    //  iic_write_8_8(fiic, da_9335, 0x42, 0x04);
    //  iic_write_8_8(fiic, da_9335, 0x49, 0x03);
    //
    //  iic_write_8_8(fiic, da_9335, 0x80, 0x03);
    //
    //  iic_read_8_8(fiic, da_9335, 0x0f, &tmp);
    //  __inf("0x0f=%x\n",tmp);
    //
    //  iic_write_8_8(fiic, da_9335, 0x0f, 0x00);//reg group 0
    //  iic_read_8_8(fiic, da_9335, 0x16, &tmp);
    //  __inf("0x16=%x\n",tmp);
    //
    //  iic_write_8_8(fiic, da_9335, 0x16, (tmp&0xfc)|0x02);//flicker auto detect
    //  iic_read_8_8(fiic, da_9335, 0x16, &tmp);
    //  __inf("0x16=%x\n",tmp);
    //
    //  iic_write_8_8(fiic, da_9335, 0x0f, 0x01);//reg group 1
    //  iic_read_8_8(fiic, da_9335, 0x0f, &tmp);
    //  __inf("0x0f=%x\n",tmp);
    //
    //
    //  iic_read_8_8(fiic, da_9335, 0x15, &tmp);
    //  __inf("0x15=%x\n",tmp);
    //  iic_write_8_8(fiic, da_9335, 0x15, tmp|0x01);//YC inverse
    //  iic_read_8_8(fiic, da_9335, 0x15, &tmp);
    //  __inf("0x15=%x\n",tmp);

    //  iic_read_8_8(fiic, da_9335, 0x16, &tmp);
    //  __inf("0x16=%x\n",tmp);
    //  iic_write_8_8(fiic, da_9335, 0x16, tmp|0x01);//RB inverse
    //  iic_read_8_8(fiic, da_9335, 0x16, &tmp);
    //  __inf("0x16=%x\n",tmp);


    return ret;
}
__s32   set_preview()
{
    __s32 ret = EPDK_FAIL;

    return ret;
}

__s32 block_write(ES_FILE *hdl_iic, __u8 slv_addr, __u8 *reg, __u32 length)
{
    __u32 i = 0;
    __s32 ret = EPDK_FAIL;

    //  reg_step=2;
    //
    //	for (i=0;i<length;i+=reg_step)
    //	{
    //	    //__u8 iic_val=0;
    //	    //__u8 tmp=0;
    //	    //__inf("8_8\n");
    //	    if(iic_write_8_8(hdl_iic,slv_addr,reg[i],reg[i+1])!= EPDK_OK)
    //	    	{
    //	        __wrn("csi0 iic write fail %x = %x\n",reg[i],reg[i+1]);
    //	      }
    //      if (i==0)esKRNL_TimeDly(1);//must wait
    //	    //__inf("csi0 iic write %d, %x = %x\n", ret, reg[i],reg[i+1]);
    //	}

    return ret;
}





//设备模式设置函数，配置设备，同时根据设备配置相应的csi设置
__s32 csi_dev_set_mode(__u32 mode)
{
    //__u32 i = 0;
    __u8 *reg_table;//根据sensor的iic宽度而定__u8还是__u16或其他
    __u32 reg_table_len;

    if (last_mode_num == mode)
    {
        __inf("no need to change csi mode!\n");
    }

    else
    {
        csi_mode = csi_mode_all.csi_mode[mode];

        switch (mode)
        {
        case 0: // for CSI X1
        {
            //            reg_table = reg_7725_640_480_25000;
            //            reg_table_len = sizeof( reg_7725_640_480_25000 );
            conf.input_fmt  = CSI_YUV422;
            conf.output_fmt = CSI_UV_CB_YUV422;//CSI_PLANAR_YUV422;//420 for preview and encode;422 for picture
            conf.field_sel  = CSI_EVEN;
            conf.seq        = CSI_UYVY;//根据实际修改
            conf.vref       = CSI_HIGH;//根据实际修改
            conf.href       = CSI_HIGH;//根据实际修改
            conf.clock      = CSI_RISING;//根据实际修改

            CSI_set_base((CSI_PORT == 0x00) ? 0x00 : 0x01);
            CSI_configure(&conf);
            CSI_set_size(0, csi_mode.size.width * 2, 0, csi_mode.size.height, csi_mode.size.width);
            break;
        }
        case 1: // for CSI X1
        {
            //            reg_table = reg_7725_640_480_25000;
            //            reg_table_len = sizeof( reg_7725_640_480_25000 );
            conf.input_fmt  = CSI_YUV422;
            conf.output_fmt = CSI_UV_CB_YUV420;//CSI_PLANAR_YUV420;//420 for preview and encode;422 for picture
            conf.field_sel  = CSI_EVEN;
            conf.seq        = CSI_UYVY;
            conf.vref       = CSI_HIGH;
            conf.href       = CSI_HIGH;
            conf.clock      = CSI_RISING;

            CSI_set_base((CSI_PORT == 0x00) ? 0x00 : 0x01);
            CSI_configure(&conf);
            CSI_set_size(0, csi_mode.size.width * 2, 0, csi_mode.size.height, csi_mode.size.width);

            break;
        }
        case 2: // for CSI X1
        {
            //            reg_table = reg_7725_640_480_50000;
            //            reg_table_len = sizeof( reg_7725_640_480_50000 );
            conf.input_fmt  = CSI_YUV422;
            conf.output_fmt = CSI_UV_CB_YUV420;//CSI_PLANAR_YUV420;//420 for preview and encode;422 for picture
            conf.field_sel  = CSI_EVEN;
            conf.seq        = CSI_UYVY;
            conf.vref       = CSI_HIGH;
            conf.href       = CSI_HIGH;
            conf.clock      = CSI_RISING;

            CSI_set_base((CSI_PORT == 0x00) ? 0x00 : 0x01);
            CSI_configure(&conf);
            CSI_set_size(0, csi_mode.size.width * 2, 0, csi_mode.size.height, csi_mode.size.width);

            break;
        }
        case 3: //for pc cam
        {
            //            reg_table = reg_7725_640_480_25000;
            //            reg_table_len = sizeof(reg_7725_640_480_25000);
            conf.input_fmt  = CSI_RAW;
            conf.output_fmt = CSI_PASS_THROUTH;//CSI_PLANAR_YUV422;//420 for preview and encode;422 for picture
            conf.field_sel  = CSI_EVEN;
            conf.seq        = CSI_YUYV;//根据实际修改
            conf.vref       = CSI_HIGH;//根据实际修改
            conf.href       = CSI_HIGH;//根据实际修改
            conf.clock      = CSI_RISING;//根据实际修改

            CSI_set_base((CSI_PORT == 0x00) ? 0x00 : 0x01);
            CSI_configure(&conf);
            CSI_set_size(0, csi_mode.size.width * 2, 0, csi_mode.size.height, csi_mode.size.width * 2);

            break;
        }
        }

        //设备的设置
        //  block_write(fiic,da,reg_table,reg_table_len);

        //	 	if(0)
        //	 		{
        //	 		csi_dev_set_color_effect(csi_color_effect);
        //	 		csi_dev_set_awb(csi_awb);
        //	 		csi_dev_set_ae(csi_ae);
        //	 		csi_dev_set_bright(csi_bright);
        //	 		csi_dev_set_contrast(csi_contrast);
        //	 		csi_dev_set_band(csi_band);
        //	 		}

        last_buf_mode = csi_mode.csi_buf_scrab_mode;
        last_mode_num = mode;
        __inf("set mode %d finished! buf_scrab_mode=%d (0=CSI0, 2=01LR, 4=01UD, 5/7=TDM_2/4CH)\n", mode, last_buf_mode);
    }
    return EPDK_OK;
}



__s32  csi_dev_init(void)
{
    //__u32 i = 0;
    __u8 *reg_table;
    __u32 reg_table_len;

    iic_set_clock(fiic, 400 * 1000); //200*1000=200kHz

    __inf("csi_dev_init......\n");

    //填写本设备支持的模式
    csi_mode_all.number = 4;//所支持模式数量
    //手动改，视方案需求，最大为8个，至少保证2个mode，一个pic用YUV422_NONMB_UVCB，一个video用YUV420_NONMB_UVCB
    //有pccam需要加多一个YUV422_YUV_MOD_INTERLEAVED的VGA或更小尺寸的模式

    //这些设置都是指输出的数据参数，为了传递给显示或者编码时的帧参数
    //=====================csi X1================================
    //640*400@25#yuv422
    csi_mode_all.csi_mode[0].color_format = PIXEL_YUV422;//格式，编码要求pic用422，video用420
    csi_mode_all.csi_mode[0].component_seq = YUV_SEQ_UVUV;//排列顺序，编码要求，固定填UVUV
    csi_mode_all.csi_mode[0].store_mode = YUV_MOD_UV_NON_MB_COMBINED;//编码要求，固定填NONMB_UVCB
    csi_mode_all.csi_mode[0].size.width = 640;//宽
    csi_mode_all.csi_mode[0].size.height = 480;//高
    csi_mode_all.csi_mode[0].frame_rate = 30000;//填fps*1000//帧率*1000
    csi_mode_all.csi_mode[0].frame_period = 33333;//填1000*1000/fps;//帧长（=1/帧率，单位us）
    csi_mode_all.csi_mode[0].color_space = BT601;//色彩空间，固定填BT601
    csi_mode_all.csi_mode[0].csi_buf_scrab_mode = CSI0_FULL;
    //640*400@25#yuv420
    csi_mode_all.csi_mode[1].color_format = PIXEL_YUV420;//格式，编码要求pic用422，video用420
    csi_mode_all.csi_mode[1].component_seq = YUV_SEQ_UVUV;
    csi_mode_all.csi_mode[1].store_mode = YUV_MOD_UV_NON_MB_COMBINED;
    csi_mode_all.csi_mode[1].size.width = 640;
    csi_mode_all.csi_mode[1].size.height = 480;
    csi_mode_all.csi_mode[1].frame_rate = 30000;
    csi_mode_all.csi_mode[1].frame_period = 33333;
    csi_mode_all.csi_mode[1].color_space = BT601;
    csi_mode_all.csi_mode[1].csi_buf_scrab_mode = CSI0_FULL;
    //640*400@50#yuv420
    csi_mode_all.csi_mode[2].color_format = PIXEL_YUV420;
    csi_mode_all.csi_mode[2].component_seq = YUV_SEQ_UVUV;
    csi_mode_all.csi_mode[2].store_mode = YUV_MOD_UV_NON_MB_COMBINED;
    csi_mode_all.csi_mode[2].size.width = 1920;
    csi_mode_all.csi_mode[2].size.height = 1080;
    csi_mode_all.csi_mode[2].frame_rate = 30000;
    csi_mode_all.csi_mode[2].frame_period = 33333;
    csi_mode_all.csi_mode[2].color_space = BT601;
    csi_mode_all.csi_mode[2].csi_buf_scrab_mode = CSI0_FULL;

    ////=====================pc cam================================
    //640*400@50#interleave yuv422
    csi_mode_all.csi_mode[3].color_format = PIXEL_YUV422;
    csi_mode_all.csi_mode[3].component_seq = YUV_SEQ_UYVY;
    csi_mode_all.csi_mode[3].store_mode = YUV_MOD_INTERLEAVED;
    csi_mode_all.csi_mode[3].size.width = 640;
    csi_mode_all.csi_mode[3].size.height = 480;
    csi_mode_all.csi_mode[3].frame_rate = 30000;
    csi_mode_all.csi_mode[3].frame_period = 33333;
    csi_mode_all.csi_mode[3].color_space = BT601;
    csi_mode_all.csi_mode[3].csi_buf_scrab_mode = CSI0_FULL;

    csi_power_off();

    esKRNL_TimeDly(10);

    csi_power_on();


    if(1)
    {
        __u8 input;

        __inf("input erase: 1:yes, other:no\n");
        input = __getc();

        if(input == 0x31)
        {
            load_bin(BIN_UPDATE);
            flash_burning_and_check();
        }
    }

    //  if(1)
    //  {
    //  __u32 bin_size;
    //  //ES_FILE  *fp_update_bin = (ES_FILE *)0;
    //
    //  __hdle   fp=NULL;
    //
    //  fp = esFSYS_fopen(BIN_UPDATE, "r");
    //
    //  bin_size=eLIBs_GetFileSize(BIN_UPDATE);
    //  __inf("bin size=%d\n",bin_size);
    //
    //  if(bin_size>0 && bin_size<=64*1024*1024)
    //    {
    //      __inf("test bin file OK, size=%d\n",bin_size);
    //      esFSYS_fclose(fp);
    //      load_bin(BIN_UPDATE);
    //      flash_burning_and_check();
    //    }
    //    else
    //    {
    //      __wrn("csi seek no bin file...\n");
    //      esFSYS_fclose(fp);
    //    }
    //  }




    //flash_read();

    Init_9335();

    set_preview();

    __inf("9335 init OK\n");

    //csi_dev_set_mode(1);//不要忘记设置一个默认模式

    return EPDK_OK;
}

__s32  csi_dev_exit(void)
{
    //csi_clk_off();
    csi_power_off();

    return EPDK_OK;
}


//设备color effect设置函数
__s32 csi_dev_set_color_effect(__csi_color_effect_t color_effect)
{
    //__u32 i = 0;
    __u8 *reg_table;//根据sensor的iic宽度而定__u8还是__u16或其他
    __u32 reg_table_len;

    switch (color_effect)
    {
    case CSI_COLOR_EFFECT_NORMAL:
    {
        reg_table = reg_color_effect_normal;
        reg_table_len = sizeof(reg_color_effect_normal);
        break;
    }
    case CSI_COLOR_EFFECT_GRAYSCALE:
    {
        reg_table = reg_color_effect_grayscale;
        reg_table_len = sizeof(reg_color_effect_grayscale);
        break;
    }
    case CSI_COLOR_EFFECT_COLORINV:
    {
        reg_table = reg_color_effect_colorinv;
        reg_table_len = sizeof(reg_color_effect_colorinv);
        break;
    }
    case CSI_COLOR_EFFECT_SEPIA:
    {
        reg_table = reg_color_effect_sepia;
        reg_table_len = sizeof(reg_color_effect_sepia);
        break;
    }
    case CSI_COLOR_EFFECT_SEPIAGREEN:
    {
        reg_table = reg_color_effect_sepiagreen;
        reg_table_len = sizeof(reg_color_effect_sepiagreen);
        break;
    }
    case CSI_COLOR_EFFECT_SEPIABLUE:
    {
        reg_table = reg_color_effect_sepiablue;
        reg_table_len = sizeof(reg_color_effect_sepiablue);
        break;
    }
    case CSI_COLOR_EFFECT_SEPIARED:
    {
        reg_table = reg_color_effect_sepiared;
        reg_table_len = sizeof(reg_color_effect_sepiared);
        break;
    }
    case CSI_COLOR_EFFECT_SEPIAYELLOW:
    {
        reg_table = reg_color_effect_sepiayellow;
        reg_table_len = sizeof(reg_color_effect_sepiayellow);
        break;
    }
    }
    //__inf("set color\n");
    //  block_write(fiic,da,reg_table,reg_table_len);
    return EPDK_OK;
}
//设备awb设置函数
__s32 csi_dev_set_awb(__csi_awb_t awb)
{
    //__u32 i = 0;
    __u8 *reg_table;//根据sensor的iic宽度而定__u8还是__u16或其他
    __u32 reg_table_len;

    switch (awb)
    {
    case CSI_AWB_AUTO:
    {
        reg_table = reg_awb_auto;
        reg_table_len = sizeof(reg_awb_auto);
        break;
    }
    case CSI_AWB_DAYLIGHT:
    {
        reg_table = reg_awb_daylight;
        reg_table_len = sizeof(reg_awb_daylight);
        break;
    }
    case CSI_AWB_FLUORESCENT:
    {
        reg_table = reg_awb_fluorescent;
        reg_table_len = sizeof(reg_awb_fluorescent);
        break;
    }
    case CSI_AWB_CLOUDY:
    {
        reg_table = reg_awb_cloudy;
        reg_table_len = sizeof(reg_awb_cloudy);
        break;
    }
    case CSI_AWB_TUNGSTEN:
    {
        reg_table = reg_awb_tungsten;
        reg_table_len = sizeof(reg_awb_tungsten);
        break;
    }
    }
    //  block_write(fiic,da,reg_table,reg_table_len);
    return EPDK_OK;
}
//设备ae设置函数
__s32 csi_dev_set_ae(__csi_ae_t ae)
{
    //__u32 i = 0;
    __u8 *reg_table;//根据sensor的iic宽度而定__u8还是__u16或其他
    __u32 reg_table_len;

    switch (ae)
    {
    case CSI_AE_N3:
    {
        reg_table = reg_ae_n3;
        reg_table_len = sizeof(reg_ae_n3);
        break;
    }
    case CSI_AE_N2:
    {
        reg_table = reg_ae_n2;
        reg_table_len = sizeof(reg_ae_n2);
        break;
    }
    case CSI_AE_N1:
    {
        reg_table = reg_ae_n1;
        reg_table_len = sizeof(reg_ae_n1);
        break;
    }
    case CSI_AE_0:
    {
        reg_table = reg_ae_0;
        reg_table_len = sizeof(reg_ae_0);
        break;
    }
    case CSI_AE_P1:
    {
        reg_table = reg_ae_p1;
        reg_table_len = sizeof(reg_ae_p1);
        break;
    }
    case CSI_AE_P2:
    {
        reg_table = reg_ae_p2;
        reg_table_len = sizeof(reg_ae_p2);
        break;
    }
    case CSI_AE_P3:
    {
        reg_table = reg_ae_p3;
        reg_table_len = sizeof(reg_ae_p3);
        break;
    }
    }
    //  block_write(fiic,da,reg_table,reg_table_len);
    return EPDK_OK;
}
//设备bright设置函数
__s32 csi_dev_set_bright(__csi_bright_t bright)
{
    //__u32 i = 0;
    __u8 *reg_table;//根据sensor的iic宽度而定__u8还是__u16或其他
    __u32 reg_table_len;

    switch (bright)
    {
    case CSI_BRIGHT_N3:
    {
        reg_table = reg_bright_n3;
        reg_table_len = sizeof(reg_bright_n3);
        break;
    }
    case CSI_BRIGHT_N2:
    {
        reg_table = reg_bright_n2;
        reg_table_len = sizeof(reg_bright_n2);
        break;
    }
    case CSI_BRIGHT_N1:
    {
        reg_table = reg_bright_n1;
        reg_table_len = sizeof(reg_bright_n1);
        break;
    }
    case CSI_BRIGHT_0:
    {
        reg_table = reg_bright_0;
        reg_table_len = sizeof(reg_bright_0);
        break;
    }
    case CSI_BRIGHT_P1:
    {
        reg_table = reg_bright_p1;
        reg_table_len = sizeof(reg_bright_p1);
        break;
    }
    case CSI_BRIGHT_P2:
    {
        reg_table = reg_bright_p2;
        reg_table_len = sizeof(reg_bright_p2);
        break;
    }
    case CSI_BRIGHT_P3:
    {
        reg_table = reg_bright_p3;
        reg_table_len = sizeof(reg_bright_p3);
        break;
    }
    }
    //  block_write(fiic,da,reg_table,reg_table_len);
    return EPDK_OK;
}

//设备contrast设置函数
__s32 csi_dev_set_contrast(__csi_contrast_t contrast)
{
    //__u32 i = 0;
    __u8 *reg_table;//根据sensor的iic宽度而定__u8还是__u16或其他
    __u32 reg_table_len;

    switch (contrast)
    {
    case CSI_CONTRAST_N3:
    {
        reg_table = reg_contrast_n3;
        reg_table_len = sizeof(reg_contrast_n3);
        break;
    }
    case CSI_CONTRAST_N2:
    {
        reg_table = reg_contrast_n2;
        reg_table_len = sizeof(reg_contrast_n2);
        break;
    }
    case CSI_CONTRAST_N1:
    {
        reg_table = reg_contrast_n1;
        reg_table_len = sizeof(reg_contrast_n1);
        break;
    }
    case CSI_CONTRAST_0:
    {
        reg_table = reg_contrast_0;
        reg_table_len = sizeof(reg_contrast_0);
        break;
    }
    case CSI_CONTRAST_P1:
    {
        reg_table = reg_contrast_p1;
        reg_table_len = sizeof(reg_contrast_p1);
        break;
    }
    case CSI_CONTRAST_P2:
    {
        reg_table = reg_contrast_p2;
        reg_table_len = sizeof(reg_contrast_p2);
        break;
    }
    case CSI_CONTRAST_P3:
    {
        reg_table = reg_contrast_p3;
        reg_table_len = sizeof(reg_contrast_p3);
        break;
    }
    }
    //  block_write(fiic,da,reg_table,reg_table_len);
    return EPDK_OK;
}

//设备band设置函数
__s32 csi_dev_set_band(__csi_band_t band)
{
    //__u32 i = 0;
    __u8 *reg_table;//根据sensor的iic宽度而定__u8还是__u16或其他
    __u32 reg_table_len;

    switch (band)
    {
    case CSI_BAND_50HZ:
    {
        reg_table = reg_band_50hz;
        reg_table_len = sizeof(reg_band_50hz);
        break;
    }
    case CSI_BAND_60HZ:
    {
        reg_table = reg_band_60hz;
        reg_table_len = sizeof(reg_band_60hz);
        break;
    }
    }

    //  block_write(fiic,da,reg_table,reg_table_len);
    return EPDK_OK;
}


#endif

