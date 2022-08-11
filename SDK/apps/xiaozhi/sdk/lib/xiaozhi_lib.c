// #include "DES.h"
#include "system/includes.h"

typedef enum {
    XZ_BLE_SUCCESS  = 0x00,
    XZ_BLE_ERR_INTERNAL,
    XZ_BLE_ERR_NOT_FOUND,
    XZ_BLE_ERR_NO_EVENT,
    XZ_BLE_ERR_NO_MEM,
    XZ_BLE_ERR_INVALID_ADDR,     // Invalid pointer supplied
    XZ_BLE_ERR_INVALID_PARAM,    // Invalid parameter(s) supplied.
    XZ_BLE_ERR_INVALID_STATE,    // Invalid state to perform operation.
    XZ_BLE_ERR_INVALID_LENGTH,
    XZ_BLE_ERR_DATA_SIZE,
    XZ_BLE_ERR_TIMEOUT,
    XZ_BLE_ERR_BUSY,
    XZ_BLE_ERR_COMMON,
    XZ_BLE_ERR_RESOURCES,
    XZ_BLE_ERR_UNKNOWN,          // other ble sdk errors
} xz_ble_status_t;


#define MAC_LEN     6

#define BOUND_INFO_LEN      20
#define GATT_MTU_LEN_MAX    520

typedef unsigned char    u8,uint8_t;
typedef unsigned short   u16;
typedef unsigned int     u32,uint32_t ;

typedef struct
{
    uint8_t   mac_type;
    uint8_t   mac[MAC_LEN];
    uint8_t   bound_flag;
    uint8_t   bound_info[BOUND_INFO_LEN];

} xz_ble_auth_settings_t;
#define NULL        0

#define RC4_KEY_LEN_MAX  GATT_MTU_LEN_MAX
#define RC4_KEY_LEN      8

char *Rc4Key = "12345678";

int Keylen = 0;
unsigned char Rc4Sbox[256];
unsigned char Rc4_key_buff[RC4_KEY_LEN];
extern xz_ble_auth_settings_t xz_ble_auth;

/*初始化函数*/
void xiaozhi_enc_init(void)
{

	int j=0;
	unsigned char k[256];
	int tmp = 0;
    Keylen = strlen(Rc4Key);

	for(int i = 0; i < 256; i++){
		Rc4Sbox[i] = i;
		k[i] = Rc4Key[i % Keylen];
	}

	for(int i = 0;i < 256; i++){
		j = (j + Rc4Sbox[i] + k[i]) % 256;
        tmp = Rc4Sbox[i];
        Rc4Sbox[i]= Rc4Sbox[j];//交换s[i]和s[j]
        Rc4Sbox[j] = tmp;
	}
    g_printf("xiaozhi_enc_init\n");
    put_buf(Rc4Sbox,sizeof(Rc4Sbox));
}

void xiaozhi_change_enc_key(uint8_t *key_buf,uint8_t len)
{
	int j=0;
	unsigned char k[256];
	int tmp = 0;



	for(int i = 0; i < 256; i++){
		Rc4Sbox[i] = i;
		k[i] = key_buf[i % len];
	}

	for(int i = 0;i < 256; i++){
		j = (j + Rc4Sbox[i] + k[i]) % 256;
        tmp = Rc4Sbox[i];
        Rc4Sbox[i]= Rc4Sbox[j];//交换s[i]和s[j]
        Rc4Sbox[j] = tmp;
	}
}

/*加解密*/
int xiaozhi_crypt(unsigned char *Data, unsigned char Len)
{
	int i = 0, j = 0, t = 0,sLen = 0;
    int k = 0;
    int tmp;
    unsigned char s[RC4_KEY_LEN_MAX];
    unsigned char *result = Data;
    for (sLen = 0; sLen < RC4_KEY_LEN_MAX; sLen++)
    {
        s[sLen] = Rc4Sbox[sLen];
    }
    for (k = 0; k < Len; k++)
    {
        i = (i + 1) % 256;
        j = (j + s[i]) % 256;
        tmp = s[i];
        s[i] = s[j];//交换s[x]和s[y]
        s[j] = tmp;
        t = (s[i] + s[j]) % 256;
        result[k] = Data[k] ^ s[t];
    }
    return result;
}

/* 初始化 */

// void xiaozhi_test(void)
// {
//     rc4_init();
//     unsigned char input[] = {0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01};
//     rc4_crypt(input, sizeof(input));
//     printf("rc4 enc data\n");
//     put_buf(input,sizeof(input));

//     rc4_crypt(input, sizeof(input));
//     printf("rc4 edc data\n");
//     put_buf(input,sizeof(input));

// }

static unsigned char xiao_zhi_UUID[24];

extern xz_ble_status_t xz_get_uuid(u8 *uuid,u8 len);
extern xz_ble_status_t xz_get_random_number(u8 *buff,u8 len);
extern xz_ble_status_t xz_get_edr_mac(uint8_t *buff,uint8_t len);

void  make_xiao_zhi_uuid(void)
{

    unsigned char temp_buf[29];
    unsigned char temp_uuid[16];
    unsigned char temp_rand[3];
    unsigned char temp;
    unsigned char sum_check = 0;
    unsigned int sum_num = 0;


    // printf("make_xiao_zhi_uuid o data:\n");
        temp_buf[0] = 0x78;
        temp_buf[1] = 0x7A;
        xz_get_uuid(temp_uuid,16);
        // memcpy(temp_uuid, xz_get_uuid(), 16);
        xz_get_random_number(temp_rand,3);
        memcpy(&temp_buf[2],temp_uuid,5);
        temp_buf[7] = 0;
        memcpy(&temp_buf[8],&temp_uuid[4],7);
        temp_buf[15] = temp_rand[1];
        memcpy(&temp_buf[16],&temp_uuid[11],4);
        temp_buf[20] = 0x40;
        temp_buf[21] = 0x57;
        temp_buf[22] = 0x5A;
        xz_get_edr_mac(&temp_buf[23],6);
        //temp_buf[23] = temp_rand[2];

        for(int i=0;i<sizeof(temp_buf);i++){
            sum_check += temp_buf[i];
            sum_num += temp_buf[i];
        }
        temp_buf[7]= sum_check;
        printf("sum_check:%d,sum:%d\n",sum_check,sum_num);

        printf("make_xiao_zhi_uuid o data:\n");
        put_buf(temp_buf,sizeof(temp_buf));
        xiaozhi_crypt(temp_buf,sizeof(temp_buf));
        printf("make_xiao_zhi_uuid enc data:\n");
        put_buf(temp_buf,sizeof(temp_buf));
        memcpy(xiao_zhi_UUID,temp_buf,sizeof(temp_buf));
        xiaozhi_crypt(temp_buf,sizeof(temp_buf));
        printf("make_xiao_zhi_uuid dec data:\n");
        put_buf(temp_buf,sizeof(temp_buf));
}

unsigned char rcs_input[] = {0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01};
/* 初始化 */

void xiaozhi_test(void)
{
    unsigned char input[] = {0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01};
    xiaozhi_enc_init();
    xiaozhi_crypt(input, sizeof(input));
    printf("rc4 enc data\n");
    put_buf(input,sizeof(input));

    memcpy(rcs_input,input,sizeof(input));

    // rc4_crypt(input, sizeof(input));
    // printf("rc4 edc data\n");
    // put_buf(input,sizeof(input));

}

#define XZ_ADV_RSP_PACKET_MAX       31

//按(长度 + 类型 + 内容)这样的格,组合填入广播包数据
static inline unsigned char make_eir_packet_data(unsigned char *buf, unsigned char offset, unsigned char data_type, unsigned char *data, unsigned char data_len)
{
    if (XZ_ADV_RSP_PACKET_MAX - offset < data_len + 2) {
        return offset + data_len + 2;
    }

    buf[0] = data_len + 1;
    buf[1] = data_type;
    memcpy(buf + 2, data, data_len);
    return data_len + 2;
}

//按(长度 + 类型 + 内容)这样的格,组合填入广播包数据
static inline unsigned char make_eir_packet_val(u8 *buf, u16 offset, u8 data_type, u32 val, u8 val_size)
{
    if (XZ_ADV_RSP_PACKET_MAX - offset < val_size + 2) {
        return offset + val_size + 2;
    }

    buf[0] = val_size + 1;
    buf[1] = data_type;
    memcpy(buf + 2, &val, val_size);
    return val_size + 2;
}

static char *xiao_zhi_name = "XZJFYX0000";
static char *xiao_zhi_pid = "CLOC K";

unsigned char xz_ble_gap_advertising_adv_data_update(unsigned char *p_ad_data)
{
    unsigned char offset = 0;
    offset += make_eir_packet_val(&p_ad_data[offset], offset, 0x01, 0x06, 1);
    offset += make_eir_packet_data(&p_ad_data[offset], offset, 0x09, (void *)xiao_zhi_name,strlen(xiao_zhi_name));
    offset += make_eir_packet_data(&p_ad_data[offset], offset, 0x08, xiao_zhi_pid,strlen(xiao_zhi_pid));
    return offset;

}
unsigned char  xz_ble_gap_advertising_scan_rsp_data_update(unsigned char *p_ad_data)
{
    unsigned char offset = 0;
    xiaozhi_enc_init();
      make_xiao_zhi_uuid();
    // xiaozhi_test();
    offset += make_eir_packet_data(&p_ad_data[offset], offset, 0xFF, xiao_zhi_UUID, 29);

    return offset;
}

#define DPID_CHANGE_KEY     0XF0


unsigned char  xz_recive_decrypt_data(unsigned char *data,unsigned char len)
{
    printf("xz_recive_decrypt_data\n");
    xiaozhi_crypt(data,len);
    put_buf(data,len);
    for(int i=0;i<len;i++){
        printf("data[%d]:%d\t",i,data[i]);
    }
    printf("\n");
    if(DPID_CHANGE_KEY == data[0]){
        if(RC4_KEY_LEN == data[3]){
            printf("RC4_KEY_LEN\n");
            xiaozhi_change_enc_key(&data[4],RC4_KEY_LEN);
        }else{
            printf("DPID_CHANGE_KEY\n");
        }
        put_buf(data,len);
    }

}

unsigned char  xz_sent_encode_data(unsigned char *data,unsigned char len)
{
    xiaozhi_crypt(data,len);
    r_printf("xz_sent_encode_data\n");
    put_buf(data,len);
    for(int i=0;i<len;i++){
        printf("data[%d]:%d\t",i,data[i]);
    }
    printf("\n");
}


