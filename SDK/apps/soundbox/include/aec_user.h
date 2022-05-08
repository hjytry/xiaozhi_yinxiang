#ifndef _AEC_USER_H_
#define _AEC_USER_H_

#include "generic/typedef.h"
#include "user_cfg.h"
//#include "commproc.h"

#define AEC_DEBUG_ONLINE	0
#define AEC_READ_CONFIG		1

#define AEC_EN				BIT(0)
#define NLP_EN				BIT(1)
#define	ANS_EN				BIT(2)

/*aec module enable bit define*/
#define AEC_MODE_ADVANCE	(AEC_EN | NLP_EN | ANS_EN)
#define AEC_MODE_REDUCE		(NLP_EN | ANS_EN)
#define AEC_MODE_SIMPLEX	(ANS_EN)

/*SMS模式性能更好，同时也需要更多的ram和mips*/
#define TCFG_AUDIO_SMS_ENABLE	0

#if TCFG_AUDIO_SMS_ENABLE
#include "commproc_sms.h"
#define aec_open		sms_init
#define aec_close		sms_exit
#define aec_in_data		sms_fill_in_data
#define aec_ref_data	sms_fill_ref_data
void aec_cfg_fill(AEC_CONFIG *cfg);
#else
#include "commproc.h"
#define aec_open		aec_init
#define aec_close		aec_exit
#define aec_in_data		aec_fill_in_data
#define aec_ref_data	aec_fill_ref_data
void aec_cfg_fill(AEC_CONFIG *cfg);
#endif

extern struct aec_s_attr aec_param;
extern const u8 CONST_AEC_SIMPLEX;

struct aec_s_attr *aec_param_init(u16 sr);
s8 aec_debug_online(void *buf, u16 size);
//void aec_cfg_fill(AEC_CONFIG *cfg);

int audio_aec_init(u16 sample_rate);
/*
 *enablebit >= 0:使用enablebit设置aec模块使能
 * 			其他:使用读配置
 *out_hdl: 自定义回调函数，NULL则用默认回调
 */
int audio_aec_open(u16 sample_rate, s16 enablebit, int (*out_hdl)(s16 *data, u16 len));
void audio_aec_close(void);
void audio_aec_inbuf(s16 *buf, u16 len);
void audio_aec_refbuf(s16 *buf, u16 len);
int audio_aec_sw_src_outsr(u16 sample_rate);
int audio_aec_sw_src_out_handler(void *handler);
int audio_aec_output_data_size();
int audio_aec_output_read(s16 *buf, u16 len);

#endif/*_AEC_USER_H_*/
