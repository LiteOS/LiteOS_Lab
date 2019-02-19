#ifndef _DEV_COM_STRUCT_H
#define _DEV_COM_STRUCT_H

#define HLK_DATA_TYPE_STR        0x00
#define HLK_DATA_TYPE_BOOL       0x01
#define HLK_DATA_TYPE_INT        0x02
#define HLK_DATA_TYPE_UNKNOWN    0x7f

#define HLK_AC_NUM_LEN         (48)

typedef struct  
{
    unsigned short svc_inst;
    char* st;        
    char* svc_id;
} svc_info_t;

typedef struct 
{
	char* sn;		
	char* prodId;       
	char* model;		
	char* dev_t;		
	char* manu;	
} dev_info_t;

typedef struct 
{           
	char* devt_nm;		
	char* manu_nm;	
} dev_nmen_t;

typedef struct
{
    unsigned short map_id;
    char* svc_name;
    char* charact_name;
    unsigned char pro_type;
}dev_key_map_t;

typedef struct
{
    unsigned short svc_inst;
    unsigned short map_id;
    int value;
}hilink_msg_info_t;

typedef int(*hilink_msg_func)(hilink_msg_info_t* msg_info);

typedef struct
{
	hilink_msg_info_t msg_info;
	hilink_msg_func msg_func;
}hilink_msg_t;

typedef struct _dev_profile_t
{
    void *prof[3];
}dev_profile_t;

/* wifi module can't store over 128 keys */
#define DEV_MAX_KEY_COUNT 128
#define DEV_MAX_SVC_COUNT 128

#define DEV_PROF_T_SVCS 0x0
#define DEV_PROF_T_KEYS (DEV_PROF_T_SVCS + 1)
#define DEV_PROF_T_VALS (DEV_PROF_T_KEYS + 1)
#define DEV_KEY_MASK_SET 1
#define DEV_KEY_MASK_USET 0
#define DEV_SVC_MASK_SET 1
#define DEV_SVC_MASK_USET 0

#define UNSIGNED_CHAR_BITS  8
#define UNSIGNED_INT_BITS   (sizeof(int) * UNSIGNED_CHAR_BITS)

void* dev_get_profinfo_and_itssize(int type, int *size);
void *dev_get_dev_info(void);
void *dev_get_dev_birsa(void);
void *dev_get_dev_ac(void);
void *dev_get_dev_enname(void);
void *dev_get_dev_profilever(void);

int hlk_update_local_val(short svcid, 
                        short keyid, 
                        int val, 
                        char *str);
void dev_set_all_keyv_unmsk(void);
int dev_chk_keyv_chg_by_index(int idx, int mskflg);
int dev_chk_svc_chg_by_index(int idx, int mskflg);

int dev_get_keyv_chg_flag(void);

int device_init_devprofile_vals(void);

#endif

