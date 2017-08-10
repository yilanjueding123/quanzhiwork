/*
**************************************************************************************************************
*											         ePDK
*						            the Easy Portable/Player Develop Kits
*									           desktop system
*
* File    	: 	FileList
*                 采用链表结构是为了提高一开始搜索文件的速度.
* By      	: 	Libaiao
* Func	:
* Version	: 	v1.0
* ============================================================================================================
* 2010-6-5 	15:24:52  Libaiao  create this file, implements the fundemental interface;
**************************************************************************************************************
*/

#include "beetles_app.h"

#include "FileList.h"

#define		FileListMalloc	eLIBs_malloc


//递归函数中调用，放在函数中的话，会引起堆栈溢出
//char            fullpath[RAT_MAX_FULL_PATH_LEN];


/*
************************************************************************************************************************
*Function	:
*
*Description	:
*
*Arguments  	:
*
*Return     	:
*
************************************************************************************************************************
*/
static __s32 FileList(void)
{
    file_item_t *item;
    file_list_t *list;

    return EPDK_OK;
}

//根据搜索到的文件信息, 新建一个文件条目,存储文件或文件夹相关的信息
file_item_t *new_file_item(ES_DIRENT   *pDirent)
{
    file_item_t *file_item = NULL;

    if(pDirent == NULL)
    {
        return NULL;
    }

    //file_item = (file_item_t*)esMEMS_Balloc(sizeof(file_item_t));
    file_item = (file_item_t *)eLIBs_malloc(sizeof(file_item_t));

    if(file_item  == NULL)
    {
        __err("memory is not enough for new a file list\n");
        return NULL;
    }

    eLIBs_memset(file_item, 0, sizeof(file_item_t));

    file_item->name_len = eLIBs_strlen(pDirent->d_name);
    if(file_item->name_len != 0)
    {
        file_item->name_len += 5;		//需多申请一个byte
        file_item->name = (char *)eLIBs_malloc(file_item->name_len);
        //file_item->name = (char*)esMEMS_Balloc(file_item->name_len);
        if(file_item->name == NULL)
        {
            return NULL;
        }
        eLIBs_memset(file_item->name, 0, file_item->name_len);
        eLIBs_strcpy(file_item->name, pDirent->d_name);
    }

    file_item->file_size = pDirent->d_size;
    file_item->fatdirattr = pDirent->fatdirattr;
    return file_item;
}

//删除一个文件条目,释放相关的内存
__s32 delete_file_item(file_item_t *file_item)
{
    if(file_item == NULL)
    {
        return EPDK_FAIL;
    }
    if(file_item->name_len != 0)
    {
        if(file_item->name != NULL)
        {
            //esMEMS_Bfree(file_item->name, file_item->name_len);
            eLIBs_free(file_item->name);
            file_item->name = NULL;
        }
    }
    //esMEMS_Bfree(file_item, sizeof(file_item));
    eLIBs_free(file_item);
    return EPDK_OK;
}

//-------------------------------------------------------------------------------
//function		:
//descriptions		: 创建一个新的文件列表，该函数搜索指定目录下所有文件夹和文件
//				 该函数执行时间由文件个数决定
//				 文件夹单独形成一个链表
//				文件单独形成一个链表
//				最后把两个链表合并，文件夹在前面
//arguments		:	ext_para, 为扩展参数
//return 			: 返回搜索到文件的个数,
//-------------------------------------------------------------------------------
__s32  new_file_list(const char *path, file_item_t **pp_item_list, void *ext_para)
{
    ES_DIRENT   *pDirent = NULL;
    ES_DIR      *pDir = NULL;
    file_item_t *list = NULL;
    file_item_t *first = NULL;
    file_item_t *next = NULL;

    file_item_t *folder_first = NULL;			//文件夹列表
    file_item_t *folder_list = NULL;
    file_item_t *folder_next = NULL;

    __u32 count = 0, folder_count = 0;
    __s32 ret = 0;
    pDir = eLIBs_opendir(path);
    if( pDir == NULL )
    {
        return 0;
    }
    eLIBs_rewinddir(pDir);

    /*
        pDirent = eLIBs_readdir(pDir);				//获得第一个文件或文件夹
    	if(pDirent == NULL)
    	{
    		return 0;
    	}

    	if(pDirent->fatdirattr & FSYS_ATTR_DIRECTORY)
    	{
    		folder_first = new_file_item(pDirent);
    		if(folder_first != NULL)
    		{
    			folder_first->fatdirattr = FSYS_ATTR_DIRECTORY;
    			folder_list = folder_first;
    		}
    	}
    	else
    	{
    		first = new_file_item(pDirent);
    		if(first == NULL)
    		{
    			return 0;
    		}
    		list = first;
    		//*pp_item_list = first;						//列表首地址返回给调用者
    		count++;
    	}
    */
    while(1)
    {
        pDirent = eLIBs_readdir(pDir);
        if(pDirent == NULL)
        {
            break;
        }
        /* skip "." and ".." sub directory */
        if (0 == eLIBs_strcmp(".", pDirent->d_name)
                || 0 == eLIBs_strcmp("..", pDirent->d_name))
        {
            continue;
        }

        /*
        ret = eLIBs_strcmp(pDirent->d_name, ".");
        if(ret == 0)
        {
        	continue;
        }
        ret = eLIBs_strcmp(pDirent->d_name, "..");
        if(ret == 0)
        {
        	continue;
        }
        */
        if(pDirent->fatdirattr & FSYS_ATTR_DIRECTORY)		//文件夹单独形成一个链表
        {
            if(folder_first == NULL)
            {
                folder_first = new_file_item(pDirent);
                if(folder_first != NULL)
                {
                    folder_first->fatdirattr = FSYS_ATTR_DIRECTORY;
                    folder_list = folder_first;
                    //count++;
                    folder_count++;
                }
                else
                {
                    break;
                }
            }
            else
            {
                folder_next = new_file_item(pDirent);
                if(folder_next != NULL)
                {
                    folder_list->next = folder_next;
                    folder_next->previous = folder_list;
                    folder_list = folder_next;					//folder_list 为链表的最后一个点
                    //count++;
                    folder_count++;
                }
                else
                {
                    break;
                }
            }
        }
        else										//文件单独形成一个链表
        {
            if(first == NULL)
            {
                first = new_file_item(pDirent);
                if(first == NULL)
                {
                    break;
                }
                list = first;
                count++;
            }
            else
            {
                next = new_file_item(pDirent);

                if(next != NULL)
                {
                    list->next = next;

                    next->previous = list;
                    list = next;								//list 为链表的最后一个点
                    count++;
                }
                else
                {
                    break;
                }
            }
        }
    }

    __msg("get %d folders & %d files totally\n", folder_count, count);

    //将两个链表合并起来,文件夹链表在前
    if(folder_first != NULL)
    {
        if(first != NULL)
        {
            folder_list->next = first;						//文件夹与文件链接起来
            first->previous = folder_list;					//连接文件夹
            list->next = folder_first;						//文件列表的结尾指向文件夹列表开始的地方
            folder_first->previous = list;
        }
        else												//只有文件夹
        {
            folder_list->next = folder_first;
            folder_first->previous = folder_list;
        }

        *pp_item_list = folder_first;						//列表首地址返回给调用者
    }
    else
    {
        if(first != NULL)									//只有文件
        {
            list->next = first;
            first->previous = list;		//形成双向链表
            *pp_item_list = first;						//列表首地址返回给调用者
        }
        else
        {
            __msg("nothing in this folder\n");
        }
    }

    eLIBs_closedir(pDir);
    return count + folder_count;
}

//删除一个文件列表(使用链表删除)
//item_list = 列表首地址
//total = 总类表个数
__s32 delete_file_list(file_item_t  *item_list, __u32 total)
{
    __u32 i = 0;
    file_item_t *list = NULL;
    file_item_t *temp = NULL;
    if(item_list == NULL)
    {
        return EPDK_FAIL;
    }
    list = item_list;
    for(i = 0; i < total; i++)
    {
        if(list == NULL)
        {
            break;
        }
        if(list->next == NULL)
        {
            break;
        }
        temp = list->next;				//先保存下一个条目地址
        if(temp != NULL)
        {
            delete_file_item(list);
            list = temp;
        }
        else
        {
            break;
        }
    }
    return EPDK_OK;
}

//从列表中删除一个节点
__s32 delete_file_item_from_list(file_item_t *item, file_list_t *list)
{
    file_item_t *previous = NULL;
    file_item_t *next = NULL;
    __s32 ret = 0;
    if(item == NULL)
    {
        return EPDK_FAIL;
    }
    if(list == NULL)
    {
        return EPDK_FAIL;
    }
    if(list->item_list == item)
    {
        list->item_list = item->next;		//删除的是第一个头节点,需要改变item_list的值
    }
    if(item->next == item)					//链表中只有一个节点
    {
        __msg("=========================only one item\n");
        ret = delete_file_item(item);
        list->cur_item = NULL;
        list->item_list = NULL;
        return ret;
    }
    else
    {
        if(item->previous != NULL)
        {
            previous = item->previous;
        }
        if(item->next != NULL)
        {
            next = item->next;
        }
        if(previous == next)
        {
            //相等表示只有两个条目
            __msg("=========================there are two items \n");
            next->next = next;
            next->previous = next;
            ret = delete_file_item(item);
        }
        else						//超过两个
        {
            __msg("=========================more than two items \n");
            previous->next = next;
            next->previous = previous;
            ret = delete_file_item(item);
        }
    }
    return ret;
}

//cur_item为当前条目
//item为要插入的条目,插入到cur_item的后面
__s32 insert_file_item_to_list(file_item_t *cur_item, file_item_t *item)
{
    file_item_t *previous = NULL;
    file_item_t *next = NULL;
    if(cur_item == NULL)
    {
        return EPDK_FAIL;
    }
    if(item == NULL)
    {
        return EPDK_FAIL;
    }
    next = cur_item->next;
    cur_item->next = item;
    item->previous = cur_item;
    item->next = next;

    next->previous = item;

    return EPDK_OK;
}

//为filelist 链表申请一个节点
file_list_t *new_file_list_nod(const char *path, void *ext_para)
{
    file_list_t *nod = NULL;

    nod = (file_list_t *)eLIBs_malloc(sizeof(file_list_t));
    if(nod == NULL)
    {
        __err("memeor is not enough for file list\n");
    }
    eLIBs_memset((void *)nod, 0, sizeof(file_list_t));

    nod->total = new_file_list(path, &(nod->item_list), ext_para);
    if(nod->total != 0)
    {
        __msg("total = %d\n", nod->total);
    }
    eLIBs_strcpy(nod->file_path, path);

    return nod;
}

//删除一个节点,且删除节点下的itemlist
__s32 delete_file_list_nod(file_list_t *nod)
{
    __s32 ret = 0;
    if(nod == NULL)
    {
        return EPDK_FAIL;
    }

    if(nod->item_list != NULL)
    {
        ret = delete_file_list(nod->item_list, nod->total);
    }

    eLIBs_free(nod);
    return ret;
}

//检测存储设备是否ready
//
file_list_t *new_file_root_list_nod(void *ext_para)
{
    file_list_t *root_nod = NULL;
    file_item_t *item = NULL;
    __s32 ret = 0, len = 0;
    __u32 count = 0;
    char disk_name[RAT_MAX_PARTITION][4];

    ES_DIRENT dirent;


    root_nod = (file_list_t *)eLIBs_malloc(sizeof(file_list_t));
    if(root_nod == NULL)
    {
        __err("memeor is not enough for file list\n");
        return NULL;
    }
    eLIBs_memset((void *)root_nod, 0, sizeof(file_list_t));

    eLIBs_memset(&dirent, 0, sizeof(ES_DIRENT));
    eLIBs_memset(disk_name, 0, sizeof(disk_name));

    ret = rat_get_partition_name(RAT_USB_DISK, disk_name);
    len = eLIBs_strlen(disk_name[0]);
    if((ret == EPDK_OK) && (len != 0))
    {
        dirent.d_size = 0;
        dirent.fatdirattr = RAT_MEDIA_TYPE_USB_DEVICE;
        //eLIBs_strcpy(root_nod->USB_path, disk_name[0]);		//保存盘符
        eLIBs_strcpy(dirent.d_name, disk_name[0]);
        item = new_file_item(&dirent);
        if(item == NULL)
        {
            __msg("error in file list!\n");
        }
        else
        {
            count++;
        }
    }

    eLIBs_memset(&dirent, 0, sizeof(ES_DIRENT));
    eLIBs_memset(disk_name, 0, sizeof(disk_name));
    ret = rat_get_partition_name(RAT_SD_CARD, disk_name);
    len = eLIBs_strlen(disk_name[0]);
    if((ret == EPDK_OK) && (len != 0))
    {
        dirent.d_size = 0;
        dirent.fatdirattr = RAT_MEDIA_TYPE_SD_DEVICE;
        //eLIBs_strcpy(root_nod->SD_path, disk_name[0]);		//保存盘符
        eLIBs_strcpy(dirent.d_name, disk_name[0]);
        if(item == NULL)								//只有一个盘符
        {
            item = new_file_item(&dirent);
            if(item != NULL)
            {
                item->next = item;
                item->previous = item;
                count++;
            }
        }
        else
        {
            item->next = new_file_item(&dirent);
            if(item->next != NULL)
            {
                item->next->previous = item;			//有两个盘符
                item->next->next = item;
                item->previous  = item->next;
                count++;
            }
            else
            {
                item->next = item;					//只有一个盘符
                item->previous = item;
            }
        }
    }

    if(item != NULL)
    {
        root_nod->item_list = item;
        root_nod->total = count;
        //eLIBs_strcpy(root_nod->file_path,path);			//path = "", parent = NULL;
    }
    else
    {
        //一个盘符都没有
    }
    return root_nod;
}

char root_device_str[2][20] =
{
    {"USB"},
    {"SD"}
};

char *get_root_device_disp_name(char *string, __u8 attr)
{
    char *str;
    if(attr == RAT_MEDIA_TYPE_USB_DEVICE)
    {
        str = root_device_str[0];
        eLIBs_strcpy(string, str);
        return str;
    }
    else if(attr == RAT_MEDIA_TYPE_SD_DEVICE)
    {
        str = root_device_str[1];
        eLIBs_strcpy(string, str);
        return str;
    }
    return NULL;
}


__s32 file_list_get_root_path(char *path, file_list_t *list, __u32 index)
{
    if(list == NULL)
    {
        return EPDK_FAIL;
    }
    if(index >= list->total)
    {
        return EPDK_FAIL;
    }
}
__s32 update_file_root_list_nod(void *ext_para)
{

    return EPDK_OK;
}
//根据索引号从nod中搜索
file_item_t *get_file_list_item(file_list_t *nod, __u32 index)
{
    __u32 i = 0, count = 0;
    file_item_t *temp = NULL;
    file_item_t *list = NULL;
    if(nod == NULL)
    {
        return NULL;
    }

    __msg("nod->total = %d  index = %d \n " , nod->total  , index );

    if(nod->item_list == NULL)
    {
        __msg("nod->item_list is null \n");
        return NULL;
    }
    if(index >= nod->total)
    {
        __msg("index is bigger than total \n");
        return NULL;
    }
    list = nod->item_list;

    for(i = 0; i <= index; i++)
    {
        temp = 	list;
        list = list->next;
    }
    return temp;
}


char *get_file_list_item_name(file_list_t *nod, __u32 index)
{
    file_item_t *temp = NULL;

    temp = get_file_list_item(nod, index);
    if(temp == NULL)
    {
        return NULL;
    }
    return temp->name;
}

//获得文件条目文件类型
rat_media_type_t  get_file_list_item_file_type(file_item_t *item)
{
    rat_media_type_t media_type = RAT_MEDIA_TYPE_UNKNOWN;

    if(item == NULL)
    {
        return RAT_MEDIA_TYPE_UNKNOWN;
    }
    if(item->fatdirattr & FSYS_ATTR_DIRECTORY)
    {
    	if(eLIBs_memcmp(item->name,"VIDEO",sizeof("VIDEO")))
    	{
    		return RAT_MEDIA_TYPE_VIDEO;
    	}
		else if(eLIBs_memcmp(item->name,"PHOTO",sizeof("PHOTO")))
		{
			return RAT_MEDIA_TYPE_PIC;
		}
		else
		{
			return RAT_MEDIA_TYPE_FOLDER;
		}
    }
    if(item->fatdirattr == RAT_MEDIA_TYPE_SD_DEVICE)
    {
        return RAT_MEDIA_TYPE_SD_DEVICE;
    }
    if(item->fatdirattr == RAT_MEDIA_TYPE_USB_DEVICE)
    {
        return RAT_MEDIA_TYPE_USB_DEVICE;
    }
    media_type = rat_get_file_type(item->name);
    return media_type;
}

//根据给出的顶部节点，删除整个filelist 链表
//注意只向下删除链表
__s32 delete_file_list_chain(file_list_t *top_nod)
{
    file_list_t *parent_nod = NULL;
    file_list_t *child_nod = NULL;
    if(top_nod == NULL)
    {
        return EPDK_FAIL;
    }
    parent_nod = top_nod;
    child_nod = parent_nod->child;				//保存子节点地址
    if(child_nod == NULL)
    {
        delete_file_list_nod(parent_nod);		//删除父节点

        return EPDK_OK;
    }

    while(child_nod != NULL)
    {
        delete_file_list_nod(parent_nod);		//删除父节点
        parent_nod = child_nod;					//指向下一个节点
        child_nod = parent_nod->child;			//保存子节点地址
    }
    return EPDK_OK;
}

//char            sub_dir[RAT_MAX_FULL_PATH_LEN];				//放在函数中，会引起堆栈溢出
//获得一个文件夹目录下文件条目总数，包括文件夹
__s32 get_item_num_in_dir(const char *dirpath, __u32 *total)
{
    ES_DIR          *dirp;
    ES_DIRENT       *direntp;
    char            sub_dir[RAT_MAX_FULL_PATH_LEN];
    //char            *sub_dir;
    static __s32    dir_level = 0;

    if(dirpath == NULL)
    {
        return EPDK_FAIL;
    }
    else if(dir_level > C_MAX_DIR_SCAN_LEVEL)
    {
        //return EPDK_FAIL;
    }
    else
    {
        dir_level++;
        __msg("dir level is %d\n", dir_level);
    }
    {
        __s32 len;

        len = eLIBs_strlen(dirpath);
        __msg("len = %d\n", len);
        __msg("dirpath=%s\n", dirpath);
    }
    dirp = (ES_DIR *)eLIBs_opendir(dirpath);
    if (dirp == NULL)
    {
        __here__;
        return EPDK_FAIL;
    }

#if 0
    sub_dir = eLIBs_malloc(RAT_MAX_FULL_PATH_LEN);
    if(!sub_dir)
    {
        return EPDK_FAIL;
    }
#endif

    while((direntp = (ES_DIRENT *)eLIBs_readdir(dirp)) != NULL)
    {
        if (direntp->fatdirattr & FSYS_ATTR_DIRECTORY)
        {
            /* skip "." and ".." sub directory */
            if (0 == eLIBs_strcmp(".", direntp->d_name)
                    || 0 == eLIBs_strcmp("..", direntp->d_name))
            {
                continue;
            }

            eLIBs_memset(sub_dir, 0, RAT_MAX_FULL_PATH_LEN);
            eLIBs_strcpy(sub_dir, dirpath);
            eLIBs_strcat(sub_dir, "\\");
            eLIBs_strcat(sub_dir, (const char *)direntp->d_name);
            if(get_item_num_in_dir(sub_dir, total) == EPDK_FAIL)
            {
                return EPDK_FAIL;
            }
            if(dir_level > 0)
            {
                dir_level--;
                __msg("dir level is %d\n", dir_level);
            }
            __msg("---total: %d, dirpath: %s, sub_dir: %s\n", *total, dirpath, sub_dir);
        }
        else
        {
            (*total)++;
            __msg("---total: %d, dirpath : %s, file: %s\n", *total, dirpath, (const char *)direntp->d_name);
        }
    }
#if 0
    if(sub_dir)
    {
        eLIBs_free(sub_dir);
        sub_dir = NULL;
    }
#endif

    if(eLIBs_closedir(dirp) == EPDK_FAIL)
    {
        __here__;
        return EPDK_FAIL;
    }
    (*total)++;


    __msg("----total: %d\n", *total);

    return EPDK_OK;
}


//删除一个文件夹
__s32 delete_dir(char *dir_name, file_op_info_t *op_info)
{
    ES_DIR          *dir = NULL;
    ES_DIRENT       *dirent = NULL;
    char            fullpath[RAT_MAX_FULL_PATH_LEN];
    __s32           ret = EPDK_OK;
    //static __s32    dir_level = 0;

    if(dir_name == NULL)
    {
        __msg("------------------------------------------input param error\n");
        return EPDK_FAIL;
    }
    else if(op_info->dir_level > C_MAX_DIR_SCAN_LEVEL)
    {
        if(op_info != NULL)
        {
            op_info->percent = 100;
        }
        __msg("------------------------------------------dir_level is over than MAX_DIR_SCAN_LEVEL, failed\n");
        //return EPDK_FAIL;
    }
    else
    {
        op_info->dir_level++;
        __msg("------------------------------------------dir_level is %d\n", op_info->dir_level);
    }
    /*
    if(_is_dir_exist(dir_name) == EPDK_FAIL)
    {
    	info->percent = 101;
    	return EPDK_FAIL;
    }
    */
    eLIBs_memset(fullpath, 0, RAT_MAX_FULL_PATH_LEN);

    /*open dir*/
    dir = eLIBs_opendir(dir_name);
    if(dir == NULL)
    {
        return EPDK_FAIL;
    }

    while(1)
    {
        /*read dir*/
        dirent = eLIBs_readdir(dir);
        if(dirent == NULL)
        {
            __msg("----read dir %s is empty\n", dir_name);
            break;
        }

        eLIBs_memset(fullpath, 0, RAT_MAX_FULL_PATH_LEN);
        eLIBs_strcpy(fullpath, dir_name);
        eLIBs_strcat(fullpath, "\\");
        eLIBs_strcat(fullpath, (const char *)dirent->d_name);

        __msg("----read dir %s ---fullpath: %s\n", (const char *)dirent->d_name, fullpath);

        if(dirent->fatdirattr & FSYS_ATTR_DIRECTORY)
        {
            if (0 == eLIBs_strcmp(".", dirent->d_name)
                    || 0 == eLIBs_strcmp("..", dirent->d_name))
            {
                continue;
            }

            ret = delete_dir(fullpath, op_info);
            if(ret == EPDK_FAIL)
            {
                __msg("------------------------_inner_delete_dir failed---fullpath: %s\n", fullpath);
                ret = EPDK_FAIL;
                break;
            }
            else
            {
                __msg("------------------------_inner_delete_dir success----fullpath: %s\n", fullpath);
            }
            if(op_info->dir_level > 0)
            {
                op_info->dir_level--;
                __msg("------------------------------------------dir_level is %d\n", op_info->dir_level);
            }
        }
        else
        {
            if(eLIBs_remove(fullpath) == -1)
            {
                __msg("-------------------------remove file %s failed\n", fullpath);
                ret = EPDK_FAIL;
                break;
            }
            else
            {
                __msg("------------------------remove file %s successfully\n", fullpath);
                if(op_info != NULL)
                {
                    op_info->finish_num++;
                    op_info->percent = (op_info->finish_num * 100) / op_info->total_num;
                    if(op_info->OnDraw != NULL)
                    {
                        op_info->OnDraw(op_info);
                    }
                }
            }
        }
    }


    if(op_info != NULL)
    {
        op_info->finish_num++;
        op_info->percent = (op_info->finish_num * 100) / op_info->total_num;
        if(op_info->OnDraw != NULL)
        {
            op_info->OnDraw(op_info);
        }
    }
    eLIBs_closedir(dir);

    ret = eLIBs_rmdir(dir_name);
    __msg("------------------------eLIBs_rmdir %s--- ret: %d\n", dir_name, ret);
    if(ret == EPDK_FAIL)
    {
        if(op_info != NULL)
        {
            op_info->percent = 101;
        }
    }
    //dir_level = 0;
    return ret;
}
