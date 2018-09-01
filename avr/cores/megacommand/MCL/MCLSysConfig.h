/* Copyright 2018, Justin Mammarella jmamma@gmail.com */

#ifndef MCLSYSCONFIG_H__
#define MCLSYSCONFIG_H__

#include "SdFat.h"
#define CONFIG_VERSION 2019

#define MIDI_OMNI_MODE 17
#define MIDI_LOCAL_MODE 16

class MCLSysConfigData {
public:
  uint32_t version;
  char project[16];
  uint8_t number_projects;
  uint8_t uart1_turbo;
  uint8_t uart2_turbo;
  uint8_t clock_send;
  uint8_t clock_rec;
  uint8_t drumRouting[16];
  uint8_t cue_output;
  uint32_t cues;
  uint8_t cur_row;
  uint8_t cur_col;
  uint8_t poly_start;
  uint8_t poly_max;
  uint8_t uart2_ctrl_mode;
  uint32_t mutes;
  uint8_t display_mirror;
  uint16_t tempo;
  uint8_t midi_forward;
  uint8_t auto_save;
};

class MCLSysConfig : public MCLSysConfigData {
public:
  uint16_t cfg_save_lastclock = 0;
  File cfgfile;
  bool write_cfg();
  bool cfg_init();
};

extern MCLSysConfig mcl_cfg;

#endif /* MCLSYSCONFIG_H__ */
