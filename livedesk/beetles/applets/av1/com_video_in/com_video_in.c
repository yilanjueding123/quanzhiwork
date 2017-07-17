
#include "epdk.h"
#include "video_linklist_manager.h"

#define TVD_REGS_BASE   0xf1c0b000
#define REG_RD32(reg)    (*((volatile __u32 *)(reg)))
#define REG_WR32(reg, value) (*((volatile __u32 *)(reg))  = (value))

#if  1
//#define __here__            eLIBs_printf("@L%d(%s)\n", __LINE__, __FILE__);
#define __inf(...)    		(eLIBs_printf("MSG:L%d(%s):", __LINE__, __FILE__),                 \
						     eLIBs_printf(__VA_ARGS__)									        )
#else
#define __inf(...)    	
#endif



typedef enum
{
    VIDEO_TYPE_CSI,
    VIDEO_TYPE_TVD,

} video_source_e ;


typedef struct __VPLY_FRM_Q
{
    __tvd_frame_t      frame[VPLY_FRM_Q_SIZE];             /* video_plybk自己的图象帧对列                           */
    __s32               isShown[VPLY_FRM_Q_SIZE];          /* 图象的状态，0为已设进DE_DRV(SetFB)，1为还未设进DE_DRV   */
    __u8                curShow;                                /* 存放当前正在播放的图象帧的数组pic_frm[]元素的下标,              */
    __u8                nextReq;                                /* 存放下一次要申请的图象帧的数组pic_frm[]元素的下标               */
    __u8                nextRel;                                /* 存放下一次要释放的图象帧的数组pic_frm[]元素的下标               */
    __s32               frmNum;                                 /* 装了显示帧的元素个数,初始值为0，每申请到一帧的时候加1，释放掉一帧的时候减1*/

} __vply_frm_q_t;

typedef struct com_video_s
{
    video_source_e 				video_source ;
    __u8						t_di_task ;
    __u8						t_show_task ;
    ES_FILE					*pvideo_dev ;
    __disp_layer_info_t 		layer_para;
    __vply_frm_q_t 				FrameQueue;
    __tvd_frame_queue_t 		TvdFrameQueue;
    __u32 						video_src_size ;
    ES_FILE  					*fp_di;
    DI_Fb_Addr_t 				DI_Fb_Addr ;
    DI_Source_Config_t 			DI_Source_Config;
    ES_FILE 					 *disphd ;
    __u32						h_lyr ;
    __video_linklist_manager_t	*full ;
    __video_linklist_manager_t	*free ;
} __com_video_ctrl_t ;

typedef enum
{
    MP_FMAT_YUV_420,
    MP_FMAT_YUV_411,
    MP_FMAT_YUV_422,
    MP_FMAT_ARGB
} mp_format_t;

typedef enum
{
    MP_ANG_NONE,
    MP_ANG_RIGHT_90,
    MP_ANG_RIGHT_180,
    MP_ANG_RIGHT_270,
    MP_ANG_FLIP_HOR,
    MP_ANG_FLIP_VER
} mp_angle_t;

typedef struct
{
    __u32				rot_src_y;
    __u32				rot_src_c;
    __u32				rot_dst_y;
    __u32				rot_dst_c;
    __u32				src_width;
    __u32				src_height;
    mp_format_t				rot_format;
    mp_angle_t				rot_angle;
} mp_rotate_mirror;

typedef struct
{
    __u32 Yptr;
    __u32 uvptr;
    __s32 size;
    __s32 width;
    __s32 height;
} __yuv_format;


static __com_video_ctrl_t video_ctrl ;
static __s32 g_grap_data_flag = 0;
static __s32	skip_frm_counter ;
static __s32	g_to_quit_video = 0;

static ES_FILE *g_fs_mp = NULL;
mp_rotate_mirror g_mp_rotate = {0};

__u32 mode;
__u32 dir;

__u32  atmp[3];
//__u32  atmp1[3];
//__u32  atmp2[3];
//__u8  flag;

static __u32 BSP_TVD_get_status(__u32 id);


static __s32 __generate_frame_id(__s32 index)
{
    __s32 prefix = 0xff % 16;
    __s32 frame_id;
    frame_id = (prefix << 4 | index);
    return frame_id;
}

static void __di_task(void *p_arg)
{
    __s32	frame_idx;
    __tvd_frame_t *p_display_buf;
    __tvd_frame_t   frame[2] = {0}; // 0 存放previous frame, 1 存放cur frame
    __u32			IsFirstFrm = 0 ;

    IsFirstFrm = 0 ;

    __here__;
    while(1)
    {
        if(esKRNL_TDelReq(EXEC_prioself) == OS_TASK_DEL_REQ)
        {
            goto EXIT_TASK;
        }

        //__here__;
        //try to get video frame from tvd module
        if( EPDK_OK != eLIBs_fioctrl(video_ctrl.pvideo_dev, DRV_TVD_CMD_REQUEST_FRAME, 0, &p_display_buf))
        {
            //__msg("request tvd frame fail\n");
            esKRNL_TimeDly(1);
            continue;
        }
        if( IsFirstFrm == 0)
        {
            eLIBs_memcpy((void *)&frame[0], (void *)p_display_buf, sizeof(__tvd_frame_t));
            IsFirstFrm = 1 ;
            continue ;
        }
        esKRNL_SchedLock();
        frame_idx = video_ctrl.free->delete_element(video_ctrl.free);
        esKRNL_SchedUnlock();
        if ( frame_idx != -1 )
        {
            eLIBs_memcpy((void *)&frame[1], (void *)p_display_buf, sizeof(__tvd_frame_t));
            video_ctrl.DI_Fb_Addr.pre_frame_buf.ch0_addr = frame[0].addr[0];
            video_ctrl.DI_Fb_Addr.pre_frame_buf.ch1_addr = frame[0].addr[1];
            video_ctrl.DI_Fb_Addr.cur_frame_buf.ch0_addr = frame[1].addr[0];
            video_ctrl.DI_Fb_Addr.cur_frame_buf.ch1_addr = frame[1].addr[1];
            video_ctrl.DI_Fb_Addr.out_frame_buf.ch0_addr = video_ctrl.FrameQueue.frame[frame_idx].addr[0] ;
            video_ctrl.DI_Fb_Addr.out_frame_buf.ch1_addr = video_ctrl.FrameQueue.frame[frame_idx].addr[1] ;


            if(mode == 0)
            {
                //__inf("di video_ctrl.FrameQueue.frame[%x].addr[0]=%x\n",frame_idx,video_ctrl.FrameQueue.frame[frame_idx].addr[0]);
                //__inf("di video_ctrl.FrameQueue.frame[%x].addr[1]=%x\n",frame_idx,video_ctrl.FrameQueue.frame[frame_idx].addr[1]);
                eLIBs_fioctrl(video_ctrl.fp_di, DI_CMD_SET_FB_ADDR, 0, (void *)(&video_ctrl.DI_Fb_Addr));
                //TestTimerReport(1);
                eLIBs_fioctrl(video_ctrl.fp_di, DI_CMD_START, 0, 0);
                //TestTimerReport(2);
            }
            else if(mode == 1)
            {
                //__inf("di video_ctrl.FrameQueue.frame[%x].addr[0]=%x\n",frame_idx,video_ctrl.FrameQueue.frame[frame_idx].addr[0]);
                //__inf("di video_ctrl.FrameQueue.frame[%x].addr[1]=%x\n",frame_idx,video_ctrl.FrameQueue.frame[frame_idx].addr[1]);
                eLIBs_fioctrl(video_ctrl.fp_di, DI_CMD_SET_FB_ADDR, 0, (void *)(&video_ctrl.DI_Fb_Addr));
                //TestTimerReport(1);
                eLIBs_fioctrl(video_ctrl.fp_di, DI_CMD_START, 0, 0);
                //TestTimerReport(2);
            }
            else if(mode == 2)
            {
                //eLIBs_printf("frame_idx = %d\n",frame_idx);
                eLIBs_memcpy((void *)video_ctrl.FrameQueue.frame[frame_idx].addr[0], (void *)frame[0].addr[0], video_ctrl.video_src_size);
                eLIBs_memcpy((void *)video_ctrl.FrameQueue.frame[frame_idx].addr[1], (void *)frame[0].addr[1], video_ctrl.video_src_size / 2);
            }


            /*
            #if COM_VIDEO_IN_MP_ROTATE

            			eLIBs_memcpy((void*)video_ctrl.FrameQueue.frame[frame_idx].addr[0],(void*)frame[0].addr[0],video_ctrl.video_src_size);
            			eLIBs_memcpy((void*)video_ctrl.FrameQueue.frame[frame_idx].addr[1],(void*)frame[0].addr[1],video_ctrl.video_src_size/2);

            #elif COM_VIDEO_IN_SOFT_ROTATE
            			//__inf("di video_ctrl.FrameQueue.frame[%x].addr[0]=%x\n",frame_idx,video_ctrl.FrameQueue.frame[frame_idx].addr[0]);
            			//__inf("di video_ctrl.FrameQueue.frame[%x].addr[1]=%x\n",frame_idx,video_ctrl.FrameQueue.frame[frame_idx].addr[1]);
            			eLIBs_fioctrl(video_ctrl.fp_di,DI_CMD_SET_FB_ADDR,0,(void*)(&video_ctrl.DI_Fb_Addr));
            			//TestTimerReport(1);
            			eLIBs_fioctrl(video_ctrl.fp_di,DI_CMD_START,0,0);
            			//TestTimerReport(2);
            #elif COM_VIDEO_NOT_ANY_ROTATE
            			//__inf("di video_ctrl.FrameQueue.frame[%x].addr[0]=%x\n",frame_idx,video_ctrl.FrameQueue.frame[frame_idx].addr[0]);
            			//__inf("di video_ctrl.FrameQueue.frame[%x].addr[1]=%x\n",frame_idx,video_ctrl.FrameQueue.frame[frame_idx].addr[1]);
            			eLIBs_fioctrl(video_ctrl.fp_di,DI_CMD_SET_FB_ADDR,0,(void*)(&video_ctrl.DI_Fb_Addr));
            			//TestTimerReport(1);
            			eLIBs_fioctrl(video_ctrl.fp_di,DI_CMD_START,0,0);
            			//TestTimerReport(2);

            #endif
            */

            eLIBs_fioctrl(video_ctrl.pvideo_dev, DRV_TVD_CMD_RELEASE_FRAME, frame[0].frame_id, NULL);

            //__inf("di frame_idx=%x \n",frame_idx);
            eLIBs_memcpy((void *)&frame[0], (void *)&frame[1], sizeof(__tvd_frame_t));
            esKRNL_SchedLock();
            video_ctrl.full->insert_element(video_ctrl.full, frame_idx);
            esKRNL_SchedUnlock();

        }
        else
        {
            eLIBs_fioctrl(video_ctrl.pvideo_dev, DRV_TVD_CMD_RELEASE_FRAME, p_display_buf->frame_id, NULL);
        }
        esKRNL_TimeDly(1);

    }


EXIT_TASK:
    //TestTimerExit();
    esKRNL_TDel(EXEC_prioself);

}


void  yuv420sp_y_rotate ( __u8 *psrc , __u8 *pdst , __u32 src_w, __u32 src_h )
{
    __u8 *y = psrc, *dsty = pdst, *tempYptr = NULL, *tempY = NULL;
    __s32 size = src_w * src_h;
    __s32 width = 0;
    __s32 height = 0;

    tempYptr = y + size; // tempYptr 指针 定位到 buff  最后一个点
    tempY = tempYptr;
    for(width = src_w; width > 0; width--)
    {
        tempYptr -= width;					// 定位 到buff 那一列
        for(height = 0; height < src_h; height++) //读取 buff 一列数据
        {
            *dsty++ = *tempYptr;
            tempYptr -= src_w; //定位列数据
        }
        tempYptr = tempY;
    }
}

void yuv420sp_uv_rotate ( __u16 *psrc , __u16 *pdst , __u32 src_w, __u32 src_h )
{

    __u16 *ptempsrc = NULL, *temp;
    __s32 size = (src_w * src_h) >> 2;
    __s32 width = 0, fix_width = src_w >> 1; // 以2字节单位 搬动数据      //=dst.width;
    __s32 height = 0, fix_height = src_h >> 1; //由 YUV420的数据格式 决定        //=dst.height;

    ptempsrc = psrc + size;
    temp = ptempsrc;
    for(width = fix_width; width > 0; width--)
    {
        ptempsrc -= width;
        for(height = 0; height < fix_height; height++)
        {
            *pdst++ = *ptempsrc;
            ptempsrc -= fix_width;
        }
        ptempsrc = temp;
    }
}

static void __video_task(void *p_arg)
{
    __u32 arg[3] = {0};
    __disp_video_fb_t video_fb ;
    __s32	cur_show_frame_id ;
    __s32	frame_idx ;
    __tvd_frame_t *p_tmp_buf;
    __vply_frm_q_t	frmQueue;


    eLIBs_memset((void *)&video_fb, 0, sizeof(__disp_video_fb_t));
    eLIBs_memset((void *)&frmQueue, 0, sizeof(__vply_frm_q_t));


    arg[0] = video_ctrl.h_lyr;
    arg[1] = 0;
    arg[2] = 0;
    //eLIBs_fioctrl(video_ctrl.disphd,DISP_CMD_LAYER_TOP,0,(void*)arg);
    eLIBs_fioctrl(video_ctrl.disphd, DISP_CMD_LAYER_OPEN, 0, (void *)arg);
    eLIBs_fioctrl(video_ctrl.disphd, DISP_CMD_VIDEO_START, 0, (void *)arg);

    while(1)
    {
        if(esKRNL_TDelReq(EXEC_prioself) == OS_TASK_DEL_REQ)
        {
            goto EXIT_TASK;
        }

        if(frmQueue.frmNum < 2)
        {
            esKRNL_SchedLock();
            frame_idx = video_ctrl.full->delete_element(video_ctrl.full);
            esKRNL_SchedUnlock();
            if( frame_idx == -1)
            {
                esKRNL_TimeDly(1);
                continue ;
            }
            eLIBs_memcpy((void *)&frmQueue.frame[frmQueue.nextReq], (void *)&video_ctrl.FrameQueue.frame[frame_idx], sizeof(__tvd_frame_t));
            //frmQueue.frame[frmQueue.nextReq].frame_id = frame_idx ;// record the frame_idx for release
            frmQueue.curShow = frmQueue.nextReq;
            frmQueue.isShown[frmQueue.curShow] = EPDK_FALSE;
            frmQueue.nextReq = (frmQueue.nextReq + 1) % VPLY_FRM_Q_SIZE;
            frmQueue.frmNum++;

        }

        if(frmQueue.isShown[frmQueue.curShow] == EPDK_FALSE)  //如果当前帧未显示，就设给DE
        {
            p_tmp_buf = &frmQueue.frame[frmQueue.curShow];

            //eLIBs_printf("frmQueue.curShow = %d..\n",frmQueue.curShow);

            if(mode == 0)
            {
                video_fb.addr[0] = p_tmp_buf->addr[0];
                video_fb.addr[1] = p_tmp_buf->addr[1];
            }
            else if(mode == 1)
            {

                //TestTimerReport(0);  //测试软件旋转效率
                yuv420sp_y_rotate((__u8 *)p_tmp_buf->addr[0], (__u8 *)atmp[0], 720, 480);
                yuv420sp_uv_rotate((__u16 *)p_tmp_buf->addr[1], (__u16 *)atmp[1], 720, 480);

                //TestTimerReport(1);

                video_fb.addr[0] = atmp[0];
                video_fb.addr[1] = atmp[1];

            }
            else if(mode == 2)
            {

                //TestTimerReport(2);


                g_mp_rotate.rot_dst_y = (__u32)atmp[0];
                g_mp_rotate.rot_dst_c = (__u32)atmp[1];

                //	g_mp_rotate.rot_dst_y=(__u32)atmp[0];
                //g_mp_rotate.rot_dst_c=(__u32)atmp[1];
                g_mp_rotate.rot_src_y = (__u32)p_tmp_buf->addr[0];
                g_mp_rotate.rot_src_c = (__u32)p_tmp_buf->addr[1];

                eLIBs_fioctrl(g_fs_mp, MP_CMD_ROTATE , 0, (void *)&g_mp_rotate );
                //TestTimerReport(3);
                ///video_fb.addr[0]=(__u32)g_disp_yuvbuff.Yptr;
                ///video_fb.addr[1]=(__u32)g_disp_yuvbuff.uvptr;


                video_fb.addr[0] = (__u32)atmp[0];
                video_fb.addr[1] = (__u32)atmp[1];


            }

            /*
            #if COM_VIDEO_IN_SOFT_ROTATE
            			eLIBs_memset((void *)atmp[0],0,(video_ctrl.video_src_size + 1023)>>10);
            			eLIBs_memset((void *)atmp[1],0,(video_ctrl.video_src_size + 1023)>>10);

            			TestTimerReport(0);  //测试软件旋转效率
            			yuv420sp_y_rotate((__u8 *)p_tmp_buf->addr[0],(__u8 *)atmp[0],720,480);
            			yuv420sp_uv_rotate((__u16 *)p_tmp_buf->addr[1],(__u16 *)atmp[1],720,480);

            			TestTimerReport(1);

            			video_fb.addr[0] = atmp[0];
            			video_fb.addr[1] = atmp[1];

            #elif COM_VIDEO_IN_MP_ROTATE

            			eLIBs_memset((void *)atmp[0],0,(video_ctrl.video_src_size + 1023)>>10);
            			eLIBs_memset((void *)atmp[1],0,(video_ctrl.video_src_size + 1023)>>10);

            			TestTimerReport(2);
            		   	g_mp_rotate.rot_dst_y=(__u32)atmp[0];
            			g_mp_rotate.rot_dst_c=(__u32)atmp[1];
            			g_mp_rotate.rot_src_y=(__u32)p_tmp_buf->addr[0];
            			g_mp_rotate.rot_src_c=(__u32)p_tmp_buf->addr[1];

            			eLIBs_fioctrl(g_fs_mp, MP_CMD_ROTATE , 0, (void*)&g_mp_rotate );
            			TestTimerReport(3);
            			///video_fb.addr[0]=(__u32)g_disp_yuvbuff.Yptr;
            			///video_fb.addr[1]=(__u32)g_disp_yuvbuff.uvptr;

            			video_fb.addr[0]=(__u32)atmp[0];
            			video_fb.addr[1]=(__u32)atmp[1];

            #elif COM_VIDEO_NOT_ANY_ROTATE
            			video_fb.addr[0] = p_tmp_buf->addr[0];
            			video_fb.addr[1] = p_tmp_buf->addr[1];
            #endif

            */
            video_fb.id = p_tmp_buf->frame_id ;

            arg[0] = video_ctrl.h_lyr;
            arg[1] = (__u32)&video_fb;

            eLIBs_fioctrl(video_ctrl.disphd, DISP_CMD_VIDEO_SET_FB, 0, (void *)arg);
            //__inf("show frame_idx=%x \n",frame_idx);

            //__inf("show frmQueue.curShow,frmQueue.frame[%d].addr[0]=%x\n",frmQueue.curShow,frmQueue.frame[frmQueue.curShow].addr[0]);
            //__inf("show frmQueue.curShow,frmQueue.frame[%d].addr[1]=%x\n",frmQueue.curShow,frmQueue.frame[frmQueue.curShow].addr[1]);

            //__inf("show video_ctrl.FrameQueue.frame[%x].addr[0]=%x\n",frame_idx,video_ctrl.FrameQueue.frame[frame_idx].addr[0]);
            //__inf("show video_ctrl.FrameQueue.frame[%x].addr[1]=%x\n",frame_idx,video_ctrl.FrameQueue.frame[frame_idx].addr[1]);
            frmQueue.isShown[frmQueue.curShow] = EPDK_TRUE;

        }


        cur_show_frame_id = eLIBs_fioctrl(video_ctrl.disphd, DISP_CMD_VIDEO_GET_FRAME_ID, 0, (void *)arg) ;
        if( p_tmp_buf->frame_id != cur_show_frame_id )
        {
            //__inf("p_tmp_buf->frame_id:%x cur_show_frame_id:%x \n",p_tmp_buf->frame_id,cur_show_frame_id);
            esKRNL_TimeDly(1);
            continue ;
        }


        if(frmQueue.frmNum < 2)
        {
            esKRNL_TimeDly(1);
            continue;
        }


        esKRNL_SchedLock();
        video_ctrl.free->insert_element(video_ctrl.free, frmQueue.frame[frmQueue.nextRel].frame_id);
        esKRNL_SchedUnlock();
        //__inf("rel frame id =%x\n",frmQueue.frame[frmQueue.nextRel].frame_id);
        frmQueue.nextRel = (frmQueue.nextRel + 1) % VPLY_FRM_Q_SIZE;
        frmQueue.frmNum--;

        esKRNL_TimeDly(1);
    }

EXIT_TASK:

    esKRNL_TDel(EXEC_prioself);
}

static void BSP_TVD_input_select(__u32 input)
{
    __u32 reg_val;
    if(input == 0)
    {
        reg_val = REG_RD32(TVD_REGS_BASE + 0x0e04);
        reg_val &= ~(1 << 0);
        REG_WR32(TVD_REGS_BASE + 0x0e04, reg_val);
    }
    else
    {
        reg_val = REG_RD32(TVD_REGS_BASE + 0x0e04);
        reg_val |= (1 << 0);
        REG_WR32(TVD_REGS_BASE + 0x0e04, reg_val);
    }
}

// system:是否自动识别TVD_SYSTEM_FORMAT_MAX : 是； 其它:否
__s32 video_dev_init(video_source_e video_source, __s32 tvd_channel,  __s32 enable_3d, __drv_TVD_system system)
{
    __s32 i;
    __u32 mod_num = 0;
    __tvd_mode_all_t tvd_mode_all;
    __tvd_mode_t tvd_mode;

    __s32 frame_period_ms = 0;
    __u32 arg[3] = {0};

    __tvd_frame_t *p_display_buf;

    __tvd_config_para_t config_para;

    if(esCFG_GetKeyValue("cvbs_para", "mode", &mode, 1))
    {
        mode = 0;
    }
    if(esCFG_GetKeyValue("cvbs_para", "dir", &dir, 1))
    {
        dir = 0;
    }

    eLIBs_printf("cvbs para mode = %d\n", mode);
    eLIBs_printf("cvbs para dir = %d\n", dir);

    //clear warnings
    arg[0] = arg[0];
    frame_period_ms = frame_period_ms;


    //esDEV_Plugin("\\drv\\tvd.drv", 0, 0, 1);
    esKRNL_TimeDly(1);
    video_ctrl.pvideo_dev = eLIBs_fopen("b:\\USER\\TVD", "r+");
    if(video_ctrl.pvideo_dev == NULL)
    {
        __err("tvd driver open failed\n");
        return EPDK_FAIL;
    }

    //esDEV_Plugin("\\drv\\mp.drv", 0, 0, 1);
    esKRNL_TimeDly(1);
    g_fs_mp = eLIBs_fopen("b:\\MP\\MIXER", "r+");
    if(!g_fs_mp)
    {

        eLIBs_printf("open mp fail\n");
    }

    //esDEV_Plugin("\\drv\\di.drv", 0, 0, 1);
    esKRNL_TimeDly(1);
    video_ctrl.fp_di = eLIBs_fopen("b:\\USER\\DI", "r+");
    if(video_ctrl.fp_di == NULL)
    {
        __err("di driver open failed\n");
        return EPDK_FAIL;
    }

    if(VIDEO_TYPE_TVD == video_source)
    {
        __log("-----jh_dbg_0918_0-----\n");
        com_video_set_blue_flag(0);
    }

    //先初始化
    eLIBs_fioctrl(video_ctrl.pvideo_dev, DRV_TVD_CMD_INIT, tvd_channel, 0);
    esKRNL_TimeDly(10);

    if(system == TVD_SYSTEM_FORMAT_MAX)
    {
        system = eLIBs_fioctrl(video_ctrl.pvideo_dev, DRV_TVD_CMD_GET_ADC_STATUS, tvd_channel, 0);
        __log("system=%x\n", system);
        if (system != EPDK_FAIL)
        {
            if (system & 0x1)
            {
                config_para.system		= ((system >> 4) & 0x01);
            }
        }
    }
    else
    {
        config_para.system = system;
    }

    //config_para.system  = TVD_SOURCE_PAL;
    config_para.tvd_channel = tvd_channel;

    if(mode == 0)
    {
        config_para.format	= TVD_UV_NON_MB_COMBINED_YUV420;
    }
    else if(mode == 1)
    {
        config_para.format	= TVD_UV_NON_MB_COMBINED_YUV420;
    }
    else if(mode == 2)
    {
        config_para.format		= TVD_UV_MB_COMBINED_YUV420;
    }


    config_para.filter_3D_en = enable_3d;
    eLIBs_fioctrl(video_ctrl.pvideo_dev, DRV_TVD_CMD_CONFIG, 0, (void *)&config_para);


    //获取所有模式
    eLIBs_fioctrl(video_ctrl.pvideo_dev, DRV_TVD_CMD_GET_MODE_ALL, 0, &tvd_mode_all);
#if (EPDK_DEBUG_LEVEL_GATE >= 2)

    for (i = 0; i < tvd_mode_all.number; i++)
    {
        __inf("#########################\n");
        __inf("### mode %d ###\n", i);
        __inf("# size: %dx%d\n", 				tvd_mode_all.tvd_mode[i].size.width, tvd_mode_all.tvd_mode[i].size.height);
        __inf("# frame_rate: %d\n", 		tvd_mode_all.tvd_mode[i].frame_rate);
        __inf("# color_format: %d\n", 	tvd_mode_all.tvd_mode[i].color_format);
        __inf("# frame_period: %d\n", 	tvd_mode_all.tvd_mode[i].frame_period);
        __inf("# component_seq: %d\n", 	tvd_mode_all.tvd_mode[i].component_seq);
        __inf("# store_mode: %d\n", 		tvd_mode_all.tvd_mode[i].store_mode);
        __inf("# color_space: %d\n", 		tvd_mode_all.tvd_mode[i].color_space);
        __inf("#########################\n");
    }
#endif
    mod_num = 0;
    __here__;
    eLIBs_fioctrl(video_ctrl.pvideo_dev, DRV_TVD_CMD_SET_MODE, 0, &(tvd_mode_all.tvd_mode[mod_num]));

    //取得当前模式
    eLIBs_fioctrl(video_ctrl.pvideo_dev, DRV_TVD_CMD_GET_MODE, 0, &tvd_mode);
    __inf("color_fmt=%d:%dx%d@%d\n", tvd_mode.color_format, tvd_mode.size.width, tvd_mode.size.height, tvd_mode.frame_rate);

    video_ctrl.TvdFrameQueue.num = TVD_BUFFER_NUM;

    video_ctrl.video_src_size = tvd_mode.size.width * tvd_mode.size.height; //for YUV422(/420)_UV_CB
    __inf("video_ctrl.video_src_size=%dkByte\n", video_ctrl.video_src_size / 1024);

    __here__;
    for (i = 0; i < video_ctrl.TvdFrameQueue.num; i++)
    {
        //address 0
        if((video_ctrl.TvdFrameQueue.tvd_frame_array[i].addr[0] = (__u32)esMEMS_Palloc(((video_ctrl.video_src_size + 1023) >> 10), MEMS_PALLOC_MODE_BND_NONE | MEMS_PALLOC_MODE_BNK_NONE)) == NULL)
        {
            __wrn("Failed to malloc!\n");
            return EPDK_FAIL;
        }
        eLIBs_memset((__u8 *)video_ctrl.TvdFrameQueue.tvd_frame_array[i].addr[0], 0, video_ctrl.video_src_size);
        eLIBs_CleanFlushDCacheRegion((void *)video_ctrl.TvdFrameQueue.tvd_frame_array[i].addr[0], video_ctrl.video_src_size);

        //address 1
        if((video_ctrl.TvdFrameQueue.tvd_frame_array[i].addr[1] = (__u32)esMEMS_Palloc(((video_ctrl.video_src_size / 2 + 1023) >> 10), MEMS_PALLOC_MODE_BND_NONE | MEMS_PALLOC_MODE_BNK_NONE)) == NULL)
        {
            __wrn("Failed to malloc!\n");
        }
        eLIBs_memset((__u8 *)video_ctrl.TvdFrameQueue.tvd_frame_array[i].addr[1], 0, video_ctrl.video_src_size / 2);
        eLIBs_CleanFlushDCacheRegion((void *)video_ctrl.TvdFrameQueue.tvd_frame_array[i].addr[1], video_ctrl.video_src_size / 2);

        video_ctrl.TvdFrameQueue.tvd_frame_array[i].frame_id = __generate_frame_id(i);
        __msg("request frame id %d = %x\n", i, video_ctrl.TvdFrameQueue.tvd_frame_array[i].frame_id );
        video_ctrl.TvdFrameQueue.tvd_frame_array[i].bProgressiveSrc = 0; //bProgressiveSrc;  值由csidrv决定，这里随便赋值
        video_ctrl.TvdFrameQueue.tvd_frame_array[i].bTopFieldFirst = 0;   //bTopFieldFirst;
        video_ctrl.TvdFrameQueue.tvd_frame_array[i].eAspectRatio = 0;     //eAspectRatio;
        video_ctrl.TvdFrameQueue.tvd_frame_array[i].color_format = tvd_mode.color_format;
        video_ctrl.TvdFrameQueue.tvd_frame_array[i].src_rect.width = video_ctrl.TvdFrameQueue.tvd_frame_array[i].dst_rect.width = tvd_mode.size.width;
        video_ctrl.TvdFrameQueue.tvd_frame_array[i].src_rect.height = video_ctrl.TvdFrameQueue.tvd_frame_array[i].dst_rect.height = tvd_mode.size.height;
        video_ctrl.TvdFrameQueue.tvd_frame_array[i].frame_rate = tvd_mode.frame_rate;
        video_ctrl.TvdFrameQueue.tvd_frame_array[i].uPts = 0;   //pts;
        video_ctrl.TvdFrameQueue.tvd_frame_array[i].bPtsValid = 1;
        video_ctrl.TvdFrameQueue.tvd_frame_array[i].component_seq = tvd_mode.component_seq;;
        video_ctrl.TvdFrameQueue.tvd_frame_array[i].store_mode = tvd_mode.store_mode;
        __msg("buffer%d:%x,%x,%x\n", 0, video_ctrl.TvdFrameQueue.tvd_frame_array[i].addr[0], video_ctrl.TvdFrameQueue.tvd_frame_array[i].addr[1], video_ctrl.TvdFrameQueue.tvd_frame_array[i].addr[2]);

    }

    eLIBs_fioctrl(video_ctrl.pvideo_dev, DRV_TVD_CMD_SET_FRAME_QUEUE, 0, &video_ctrl.TvdFrameQueue);

    video_ctrl.full = video_linklist_manager_init();
    video_ctrl.free = video_linklist_manager_init();
    video_ctrl.full->initial(video_ctrl.full, LISTTYPE_FULL);
    video_ctrl.free->initial(video_ctrl.free, LISTTYPE_FREE);

    for (i = 0; i < VPLY_FRM_Q_SIZE; i++)
    {
        //address 0
        if((video_ctrl.FrameQueue.frame[i].addr[0] = (__u32)esMEMS_Palloc(((video_ctrl.video_src_size + 1023) >> 10), MEMS_PALLOC_MODE_BND_NONE | MEMS_PALLOC_MODE_BNK_NONE)) == NULL)
        {
            __wrn("Failed to malloc!\n");
            return EPDK_FAIL;
        }
        eLIBs_memset((__u8 *)video_ctrl.FrameQueue.frame[i].addr[0], 0, video_ctrl.video_src_size);
        eLIBs_CleanFlushDCacheRegion((void *)video_ctrl.FrameQueue.frame[i].addr[0], video_ctrl.video_src_size);

        //address 1
        if((video_ctrl.FrameQueue.frame[i].addr[1] = (__u32)esMEMS_Palloc(((video_ctrl.video_src_size / 2 + 1023) >> 10), MEMS_PALLOC_MODE_BND_NONE | MEMS_PALLOC_MODE_BNK_NONE)) == NULL)
        {
            __wrn("Failed to malloc!\n");
        }
        eLIBs_memset((__u8 *)video_ctrl.FrameQueue.frame[i].addr[1], 0, video_ctrl.video_src_size / 2);
        eLIBs_CleanFlushDCacheRegion((void *)video_ctrl.FrameQueue.frame[i].addr[1], video_ctrl.video_src_size / 2);

        __inf("vplaybak buffer[%d]:%x,%x\n", i, video_ctrl.FrameQueue.frame[i].addr[0], video_ctrl.FrameQueue.frame[i].addr[1]);
        video_ctrl.FrameQueue.frame[i].frame_id = i;
        __inf("video_ctrl.FrameQueue.frame[%d].frame_id=%x\n", i, video_ctrl.FrameQueue.frame[i].frame_id);
        video_ctrl.free->insert_element(video_ctrl.free, i);

    }

    frame_period_ms = tvd_mode.frame_period / 1000;

    video_ctrl.disphd = eLIBs_fopen("b:\\DISP\\DISPLAY", "r+");
    if(!video_ctrl.disphd)
    {
        __wrn("open display device fail!\n");
        return EPDK_FAIL;
    }

    eLIBs_memset(&video_ctrl.layer_para, 0, sizeof(__disp_layer_info_t));

    video_ctrl.layer_para.fb.addr[0]       = video_ctrl.TvdFrameQueue.tvd_frame_array[0].addr[0];
    video_ctrl.layer_para.fb.addr[1]       = video_ctrl.TvdFrameQueue.tvd_frame_array[0].addr[1];
    //video_ctrl.layer_para.fb.addr[2]       = video_ctrl.TvdFrameQueue.tvd_frame_array[0].addr[2];



    if(mode == 0)
    {
        video_ctrl.layer_para.fb.size.width    = tvd_mode.size.width;
        video_ctrl.layer_para.fb.size.height   = tvd_mode.size.height;
    }
    else if(mode == 1)
    {
        video_ctrl.layer_para.fb.size.width    = tvd_mode.size.height;
        video_ctrl.layer_para.fb.size.height   = tvd_mode.size.width;
    }
    else if(mode == 2)
    {
        video_ctrl.layer_para.fb.size.width    = tvd_mode.size.height;
        video_ctrl.layer_para.fb.size.height   = tvd_mode.size.width;
    }
    /*
    #if COM_VIDEO_IN_MP_ROTATE
    	video_ctrl.layer_para.fb.size.width    = tvd_mode.size.height;
        video_ctrl.layer_para.fb.size.height   = tvd_mode.size.width;
    #elif COM_VIDEO_IN_SOFT_ROTATE
    	video_ctrl.layer_para.fb.size.width    = tvd_mode.size.height;
        video_ctrl.layer_para.fb.size.height   = tvd_mode.size.width;
    #elif COM_VIDEO_NOT_ANY_ROTATE
        video_ctrl.layer_para.fb.size.width    = tvd_mode.size.width;
        video_ctrl.layer_para.fb.size.height   = tvd_mode.size.height;
    #endif
    */


    if(mode == 0)
    {
        video_ctrl.layer_para.fb.mode          = DISP_MOD_NON_MB_UV_COMBINED;
    }
    else if(mode == 1)
    {
        video_ctrl.layer_para.fb.mode          = DISP_MOD_NON_MB_UV_COMBINED;
    }
    else if(mode == 2)
    {
        video_ctrl.layer_para.fb.mode          = DISP_MOD_MB_UV_COMBINED;
    }
    /*
    #if COM_VIDEO_IN_MP_ROTATE
        video_ctrl.layer_para.fb.mode          = DISP_MOD_MB_UV_COMBINED;
    #elif COM_VIDEO_IN_SOFT_ROTATE
        video_ctrl.layer_para.fb.mode          = DISP_MOD_NON_MB_UV_COMBINED;
    #elif COM_VIDEO_NOT_ANY_ROTATE
        video_ctrl.layer_para.fb.mode          = DISP_MOD_NON_MB_UV_COMBINED;
    #endif
    */

    video_ctrl.layer_para.fb.format        = DISP_FORMAT_YUV420;
    video_ctrl.layer_para.fb.br_swap       = 0;
    video_ctrl.layer_para.fb.seq           = DISP_SEQ_UVUV;
    video_ctrl.layer_para.ck_enable        = 0;
    video_ctrl.layer_para.alpha_en         = 1;
    video_ctrl.layer_para.alpha_val        = 0xff;
    video_ctrl.layer_para.pipe             = 0;



    if(mode == 0)
    {
        video_ctrl.layer_para.src_win.x        = 10;
        video_ctrl.layer_para.src_win.y        = 3 + 8;
        video_ctrl.layer_para.src_win.width    = tvd_mode.size.width - 30; //tvd_mode.size.width;
        video_ctrl.layer_para.src_win.height   = tvd_mode.size.height - 6  - 8 * 2; //tvd_mode.size.height;
    }
    else if(mode == 1)
    {
        video_ctrl.layer_para.src_win.x		  = 0;
        video_ctrl.layer_para.src_win.y		  = 0;
        video_ctrl.layer_para.src_win.width	  = tvd_mode.size.height - 6  - 8 * 2;
        video_ctrl.layer_para.src_win.height   = tvd_mode.size.width - 30;
    }
    else if(mode == 2)
    {
        video_ctrl.layer_para.src_win.x		  = 0;
        video_ctrl.layer_para.src_win.y		  = 0;
        video_ctrl.layer_para.src_win.width	  = tvd_mode.size.height - 6  - 8 * 2;
        video_ctrl.layer_para.src_win.height   = tvd_mode.size.width - 30;
    }

    /*
    #if COM_VIDEO_IN_MP_ROTATE
    video_ctrl.layer_para.src_win.x		  =0;
    video_ctrl.layer_para.src_win.y		  =0;
    video_ctrl.layer_para.src_win.width	  = tvd_mode.size.height;
    video_ctrl.layer_para.src_win.height   = tvd_mode.size.width;
    #elif COM_VIDEO_IN_SOFT_ROTATE
    video_ctrl.layer_para.src_win.x		  =0;
    video_ctrl.layer_para.src_win.y		  =0;
    video_ctrl.layer_para.src_win.width	  = tvd_mode.size.height;
    video_ctrl.layer_para.src_win.height   = tvd_mode.size.width;
    #elif COM_VIDEO_NOT_ANY_ROTATE
    video_ctrl.layer_para.src_win.x        = 10;
    video_ctrl.layer_para.src_win.y        = 3+8;
    video_ctrl.layer_para.src_win.width    = tvd_mode.size.width -30;//tvd_mode.size.width;
    video_ctrl.layer_para.src_win.height   = tvd_mode.size.height-6  -8*2;//tvd_mode.size.height;
    #endif
    */

    video_ctrl.layer_para.scn_win.x        = 0;
    video_ctrl.layer_para.scn_win.y        = 0;
    video_ctrl.layer_para.scn_win.width    = eLIBs_fioctrl(video_ctrl.disphd, DISP_CMD_SCN_GET_WIDTH, 0, 0);;
    video_ctrl.layer_para.scn_win.height   = eLIBs_fioctrl(video_ctrl.disphd, DISP_CMD_SCN_GET_HEIGHT, 0, 0);

    //layer0
    arg[0] = DISP_LAYER_WORK_MODE_SCALER;
    arg[1] = 0;
    arg[2] = 0;
    video_ctrl.h_lyr = eLIBs_fioctrl(video_ctrl.disphd, DISP_CMD_LAYER_REQUEST, 0, (void *)arg);
    if(video_ctrl.h_lyr == NULL)
    {
        __wrn("request layer0 fail\n");
    }
    else
    {
        video_ctrl.layer_para.mode = DISP_LAYER_WORK_MODE_SCALER;
        video_ctrl.layer_para.pipe = 0;
        arg[0] = video_ctrl.h_lyr;
        arg[1] = (__u32)&video_ctrl.layer_para;
        arg[2] = 0;
        eLIBs_fioctrl(video_ctrl.disphd, DISP_CMD_LAYER_SET_PARA, 0, (void *)arg);
        eLIBs_fioctrl(video_ctrl.disphd, DISP_CMD_LAYER_CLOSE, 0, (void *)arg);
    }
    __here__;

    if(tvd_mode.store_mode == YUV_MOD_UV_MB_COMBINED)
    {
        video_ctrl.DI_Source_Config.type.mod = DI_MB_UVCOMBINE;
    }
    else if(tvd_mode.store_mode == YUV_MOD_UV_NON_MB_COMBINED)
    {

        video_ctrl.DI_Source_Config.type.mod = DI_NON_MB_UVCOMBINE;
    }
    else
    {
        __err("di format not support\n");
    }

    video_ctrl.DI_Source_Config.src_width = tvd_mode.size.width;
    video_ctrl.DI_Source_Config.src_height = tvd_mode.size.height;
    video_ctrl.DI_Source_Config.type.ps = 0 ;
    __here__;

    eLIBs_fioctrl(video_ctrl.fp_di, DI_CMD_CONFIG_SRC, 0, (void *)(&video_ctrl.DI_Source_Config));
    __here__;
    eLIBs_fioctrl(video_ctrl.pvideo_dev, DRV_TVD_CMD_CAPTURE_ON, 0, 0);
    __here__;

    if(mode == 0)
    {
        ////
    }
    else if(mode == 1)
    {
        ////
    }
    else if(mode == 2)
    {
        g_mp_rotate.src_width = tvd_mode.size.width; //dst_yuv.width;
        g_mp_rotate.src_height = tvd_mode.size.height; //dst_yuv.height;

        g_mp_rotate.rot_format = MP_FMAT_YUV_420;
        g_mp_rotate.rot_angle = dir;
    }

    skip_frm_counter = 0 ;
    while(1)
    {
        if(skip_frm_counter < 15 )
        {
            //try to get video frame from tvd module
            if( EPDK_OK != eLIBs_fioctrl(video_ctrl.pvideo_dev, DRV_TVD_CMD_REQUEST_FRAME, 0, &p_display_buf))
            {
                esKRNL_TimeDly(1);
                continue;
            }
            skip_frm_counter++ ;
            eLIBs_fioctrl(video_ctrl.pvideo_dev, DRV_TVD_CMD_RELEASE_FRAME, p_display_buf->frame_id, NULL);

        }
        else
        {
            DebugPrintf("skip frame finish\n");
            break ;
        }
    }


    atmp[0] = (__u32)esMEMS_Palloc(((video_ctrl.video_src_size + 1023) >> 10), MEMS_PALLOC_MODE_BND_NONE | MEMS_PALLOC_MODE_BNK_NONE);
    atmp[1] = (__u32)esMEMS_Palloc(((video_ctrl.video_src_size / 2 + 1023) >> 10), MEMS_PALLOC_MODE_BND_NONE | MEMS_PALLOC_MODE_BNK_NONE);

    video_ctrl.t_di_task = esKRNL_TCreate(__di_task, NULL, 0x8000, KRNL_priolevel4);
    video_ctrl.t_show_task = esKRNL_TCreate(__video_task, NULL, 0x8000, KRNL_priolevel3);

    return EPDK_OK;
}




__s32 video_dev_exit(video_source_e video_source)
{
    __u32 arg[3] ;
    __u32 i ;

    if(video_ctrl.t_show_task)
    {
        while(1)
        {
            if(esKRNL_TDelReq(video_ctrl.t_show_task ) == OS_TASK_NOT_EXIST)
            {
                break;
            }
            esKRNL_TimeDly(1);
        }
        video_ctrl.t_show_task = 0;
    }

    if(video_ctrl.t_di_task)
    {
        while(1)
        {
            if(esKRNL_TDelReq(video_ctrl.t_di_task ) == OS_TASK_NOT_EXIST)
            {
                break;
            }
            esKRNL_TimeDly(1);
        }
        video_ctrl.t_di_task = 0;
    }

    esMEMS_Pfree((void *)atmp[0], ((video_ctrl.video_src_size + 1023) >> 10));
    esMEMS_Pfree((void *)atmp[1], ((video_ctrl.video_src_size / 2 + 1023) >> 10));


    arg[0] = video_ctrl.h_lyr;
    arg[1] = 0;
    arg[2] = 0;
    eLIBs_fioctrl(video_ctrl.disphd, DISP_CMD_VIDEO_STOP, 0, (void *)arg);
    eLIBs_fioctrl(video_ctrl.disphd, DISP_CMD_LAYER_CLOSE, 0, (void *)arg);
    eLIBs_fioctrl(video_ctrl.disphd, DISP_CMD_LAYER_RELEASE, 0, (void *)arg);

    video_ctrl.free->exit(video_ctrl.free);
    video_ctrl.full->exit(video_ctrl.full);

    if(video_ctrl.disphd)
    {
        eLIBs_fclose(video_ctrl.disphd);
        video_ctrl.disphd = NULL;
    }


    eLIBs_fioctrl(video_ctrl.pvideo_dev, DRV_TVD_CMD_CAPTURE_OFF, 0, 0);
    eLIBs_fioctrl(video_ctrl.pvideo_dev, DRV_TVD_CMD_EXIT, 0, 0);//应用程序退出必须调用此步，否则内存没有释放

    if(video_ctrl.pvideo_dev)
    {
        eLIBs_fclose(video_ctrl.pvideo_dev);
        video_ctrl.pvideo_dev = NULL;
    }
    for (i = 0 ; i < video_ctrl.TvdFrameQueue.num ; i++)
    {
        if(video_ctrl.TvdFrameQueue.tvd_frame_array[i].addr[0] && video_ctrl.video_src_size)
        {
            esMEMS_Pfree((void *)(video_ctrl.TvdFrameQueue.tvd_frame_array[i].addr[0]), (video_ctrl.video_src_size + 1023) >> 10);
            video_ctrl.TvdFrameQueue.tvd_frame_array[i].addr[0] = NULL;
        }
        if(video_ctrl.TvdFrameQueue.tvd_frame_array[i].addr[1] && video_ctrl.video_src_size)
        {
            esMEMS_Pfree((void *)(video_ctrl.TvdFrameQueue.tvd_frame_array[i].addr[1]), (video_ctrl.video_src_size / 2 + 1023) >> 10);
            video_ctrl.TvdFrameQueue.tvd_frame_array[i].addr[1] = NULL;
        }
    }
    eLIBs_memset(&video_ctrl.TvdFrameQueue, 0, sizeof(__tvd_frame_queue_t));

    for (i = 0 ; i < VPLY_FRM_Q_SIZE ; i++)
    {
        if(video_ctrl.FrameQueue.frame[i].addr[0] && video_ctrl.video_src_size)
        {
            esMEMS_Pfree((void *)(video_ctrl.FrameQueue.frame[i].addr[0]), (video_ctrl.video_src_size + 1023) >> 10);
            video_ctrl.FrameQueue.frame[i].addr[0] = NULL;
        }
        if(video_ctrl.FrameQueue.frame[i].addr[1] && video_ctrl.video_src_size)
        {
            esMEMS_Pfree((void *)(video_ctrl.FrameQueue.frame[i].addr[1]), (video_ctrl.video_src_size / 2 + 1023) >> 10);
            video_ctrl.FrameQueue.frame[i].addr[1] = NULL;
        }
    }
    eLIBs_memset(&video_ctrl.FrameQueue, 0, sizeof(__vply_frm_q_t));

    video_ctrl.video_src_size = 0;




    if(video_ctrl.pvideo_dev)
    {
        eLIBs_fclose(video_ctrl.pvideo_dev);
        video_ctrl.pvideo_dev = NULL;
        //		esDEV_Plugout("\\drv\\tvd.drv", 0);
    }

    if(video_ctrl.fp_di)
    {
        eLIBs_fclose(video_ctrl.fp_di);
        video_ctrl.fp_di = NULL;
        //		esDEV_Plugout("\\drv\\di.drv", 0);

    }

    if(g_fs_mp)
    {
        eLIBs_fclose(g_fs_mp);
        g_fs_mp = NULL;
        //		esDEV_Plugout("\\drv\\mp.drv", 0);
    }

}

//dev_index:0 for csi, 1 for tv in
//tvd_channel: tv0/tv1
//P、N制式由软件自己获取
// system:是否自动识别TVD_SYSTEM_FORMAT_MAX : 是； 其它:否

__s32 com_video_in_open(__s32 dev_index, __s32 tvd_channel, __s32 enable_3d, __drv_TVD_system system)
{

    eLIBs_memset((void *)&video_ctrl, 0 , sizeof(__com_video_ctrl_t) );


    DebugPrintf("com_video_in_open,dev_index=%d tvd_channel:%d,\n", dev_index, tvd_channel);

    g_to_quit_video = 0;

    video_ctrl.video_source = VIDEO_TYPE_TVD;

    video_dev_init(video_ctrl.video_source, tvd_channel, enable_3d, system);

    return EPDK_OK;
}

__s32 com_video_in_close(void)
{
    g_to_quit_video = 1;
    __here__;
    video_dev_exit(video_ctrl.video_source);
    eLIBs_memset((void *)&video_ctrl, 0 , sizeof(__com_video_ctrl_t) );
    __here__;
    return EPDK_OK;
}

//tvd_channel: tv0/tv1
__s32 com_video_in_set_sel(__s32 tvd_channel)
{
    __wrn("com_video_in_set_sel, tvd_channel:%d\n", tvd_channel);

    if(video_ctrl.pvideo_dev)
    {
        eLIBs_fioctrl(video_ctrl.pvideo_dev, DRV_TVD_CMD_SET_TV_SEL, tvd_channel, 0);
    }

    return EPDK_FAIL;
}


///flag: 0: 取消蓝屏，检测无信号也照样不蓝屏  1:直接输出蓝屏，不检测信号， 2:有信号则正常显示，无信号显示蓝屏
__s32 com_video_set_blue_flag(__s32 flag)
{
    //	return;

#if 1
    __u32 v = REG_RD32(TVD_REGS_BASE + 0xf14);
    v &= ~(0x3 << 4);
    v |= ((flag & 0x3) << 4);
    REG_WR32(TVD_REGS_BASE + 0xf14, v);

    return EPDK_OK;
#endif
}


///抓一批数据到U盘
__s32 com_video_grap_data(void)
{
    g_grap_data_flag = 1;
    return EPDK_OK;
}


//0<=val<=255 默认128
__s32 com_video_set_contrast(__s32 val)
{
    __u32 v = REG_RD32(TVD_REGS_BASE + 0x00c);

    v &= ~(0xff << 0);

    val &= 0xff;
    v |= (val << 0);
    REG_WR32(TVD_REGS_BASE + 0x00c, v);

    return EPDK_OK;
}

//0<=val<=255 默认32 ,val必须是32的整数倍
__s32 com_video_set_bright(__s32 val)
{
    __u32 v = REG_RD32(TVD_REGS_BASE + 0x00c);

    v &= ~(0xff << 8);

    val &= 0xff;
    v |= (val << 8) | (1 << 16);
    REG_WR32(TVD_REGS_BASE + 0x00c, v);

    return EPDK_OK;
}

//0<=val<=255 默认128
__s32 com_video_set_saturation(__s32 val)
{
    __u32 v = REG_RD32(TVD_REGS_BASE + 0x010);

    v &= ~(0xff << 0);

    val &= 0xff;
    v |= (val << 0);
    REG_WR32(TVD_REGS_BASE + 0x010, v);

    return EPDK_OK;
}

//0<=val<=255 默认0
__s32 com_video_set_hue(__s32 val)
{
    __u32 v = REG_RD32(TVD_REGS_BASE + 0x010);

    v &= ~(0xff << 8);

    val &= 0xff;
    v |= (val << 8);
    REG_WR32(TVD_REGS_BASE + 0x010, v);

    return EPDK_OK;
}

//返回1代表有信号，返回0代表无信号
__s32 com_video_get_status(void)
{
    __s32 ret = 0;

    if(video_ctrl.pvideo_dev)
    {
        ret = eLIBs_fioctrl(video_ctrl.pvideo_dev, DRV_TVD_CMD_GET_ADC_STATUS, 0, 0);
    }

    DebugPrintf("ret:0x%x\n", ret);

    ret = BSP_TVD_get_status(0);

    DebugPrintf("ret:0x%x\n", ret);

    ret &= 0x01;

    return ret;
}

//返回1代表P制，返回0代表N制
__s32 com_video_get_format(void)
{
    __s32 ret = 0;

    if(video_ctrl.pvideo_dev)
    {
        ret = eLIBs_fioctrl(video_ctrl.pvideo_dev, DRV_TVD_CMD_GET_ADC_STATUS, 0, 0);
    }

    DebugPrintf("ret:0x%x\n", ret);

    ret = BSP_TVD_get_status(0);

    DebugPrintf("ret:0x%x\n", ret);

    ret >>= 4;

    ret &= 0x01;

    return ret;
}

// value from 00~ff
__s32 com_video_set_chrom_gain(__s32 val)
{
    __u32 v = REG_RD32(TVD_REGS_BASE + 0x0f1c);

    v &= ~(0xff << 16);

    val &= 0xff;
    v |= (val << 16);
    REG_WR32(TVD_REGS_BASE + 0x0f1c, v);

    return EPDK_OK;
}

__s32	com_video_printf_all_tvd_reg(void)
{
    volatile __u32	addr ;
    volatile __u32 reg_vule ;
    for(addr = 0xf1c0b000 ; addr <= (0xf1c0b000 + 0xfc) ; addr = addr + 4 )
    {
        reg_vule = *(volatile __u32 *)addr ;
        eLIBs_printf("tvd reg[%x]=%x\n", addr, reg_vule);
    }
    for(addr = 0xf1c0be00 ; addr <= (0xf1c0be00 + 0xfc) ; addr = addr + 4 )
    {
        reg_vule = *(volatile __u32 *)addr ;
        eLIBs_printf("tvd reg[%x]=%x\n", addr, reg_vule);
    }
}
static __u32 BSP_TVD_get_status(__u32 id)
{
    __u32 reg_val = 0;
    __u32 det = 0;
    __u32 system = 0;
    reg_val = REG_RD32(TVD_REGS_BASE + 0x0e40);
    __log("=shell= reg_val = %d==\n", reg_val);
    if(reg_val & 1)
    {
        det = 0;// no signal detect
    }
    else
    {
        det = 1;// detect tv signal
    }
    if(reg_val & (1 << 18))
    {
        system = 1;//get system = pal
    }
    else
    {
        system = 0;//get system = ntsc
    }

    __wrn("reg_val:0x%x, det:%d, system:%d\n", reg_val, det, system);

    return ((det << 0) + (system << 4)); //bit0=det bit4=system
}


