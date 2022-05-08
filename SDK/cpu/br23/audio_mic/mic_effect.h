#ifndef __MIC_EFFECT_H__
#define __MIC_EFFECT_H__

// #if 1//(!defined(CONFIG_MEDIA_EFFECT_DISABLE))

#if (TCFG_MIC_EFFECT_ENABLE)

#include "system/includes.h"
#include "audio_mic/effect_reg.h"
#include "audio_mic/effect_cfg.h"
#include "app_config.h"
#include "application/audio_pemafrow.h"
enum {
    MIC_EQ_MODE_SHOUT_WHEAT = 0x0,
    MIC_EQ_MODE_LOW_SOUND,
    MIC_EQ_MODE_HIGH_SOUND,
};

void mic_effect_reverb_parm_fill(REVERBN_PARM_SET *parm, u8 fade, u8 online);
void mic_effect_echo_parm_fill(ECHO_PARM_SET *parm, u8 fade, u8 online);
void mic_effect_pitch_parm_fill(PITCH_PARM_SET2 *parm, u8 fade, u8 online);
void mic_effect_noisegate_parm_fill(NOISE_PARM_SET *parm, u8 fade, u8 online);
void mic_effect_shout_wheat_parm_fill(SHOUT_WHEAT_PARM_SET *parm, u8 fade, u8 online);
void mic_effect_low_sound_parm_fill(LOW_SOUND_PARM_SET *parm, u8 fade, u8 online);
void mic_effect_high_sound_parm_fill(HIGH_SOUND_PARM_SET *parm, u8 fade, u8 online);
void mic_effect_mic_gain_parm_fill(EFFECTS_MIC_GAIN_PARM *parm, u8 fade, u8 online);


void mic_effect_set_mic_parm(struct __mic_stream_parm *parm);
bool mic_effect_start(void);
void mic_effect_stop(void);
void mic_effect_pause(u8 mark);
void mic_effect_dac_pause(u8 mark);
void set_pitch_para(u32 shiftv, u32 sr, u8 effect, u32 formant_shift);
void mic_effect_change_mode(u16 mode);
u16 mic_effect_get_cur_mode(void);

u8 mic_effect_get_status(void);

void mic_effect_set_function_mask(u32 mask);
u32 mic_effect_get_function_mask(void);

void mic_effect_set_dvol(u8 vol);
u8 mic_effect_get_dvol(void);

void mic_effect_set_reverb_wet(int wet);
int mic_effect_get_reverb_wet(void);

void mic_effect_set_echo_deep(u32 deep);
u32 mic_effect_get_echo_deep(void);

void mic_effect_set_echo_decay(u32 decay);
u32 mic_effect_get_echo_decay(void);

void mic_effect_cal_coef(u8 type, u32 gainN);
u8 mic_effect_eq_section_num(void);

void mic_dodge_ctr(void);
u8 mic_dodge_get_status(void);


struct eq_seg_info *mic_eq_get_info(u16 index);
int mic_eq_set_info(struct eq_seg_info *info);
void mic_mode_switch(u8 eff_mode);
void mic_effect_to_usbmic_onoff(u8 mark);
#endif /* (!defined(CONFIG_MEDIA_EFFECT_DISABLE)) */

#endif// __MIC_EFFECT_H__
