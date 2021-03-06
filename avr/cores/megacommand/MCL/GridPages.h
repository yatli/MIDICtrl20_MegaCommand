/* Copyright Justin Mammarella jmamma@gmail.com 2018 */

#ifndef GRIDPAGES_H__
#define GRIDPAGES_H__

#ifdef OLED_DISPLAY

#define ENCODER_RES_GRID 1
#define ENCODER_RES_PAT 2

#else

#define ENCODER_RES_GRID 4
#define ENCODER_RES_PAT 4

#endif

#include "GridEncoder.h"
#include "GridPage.h"

#include "MCLEncoder.h"
#include "GridSavePage.h"
#include "GridWritePage.h"
#include "Menu.h"
#include "MenuPage.h"
#include "GridChain.h"
#include "GridTrack.h"
#include "MCLSysConfig.h"

extern MCLEncoder param1;
extern MCLEncoder param2;
extern MCLEncoder param3;
extern MCLEncoder param4;

extern GridPage grid_page;

extern MCLEncoder gridio_param1;
extern MCLEncoder gridio_param2;
extern MCLEncoder gridio_param3;
extern MCLEncoder gridio_param4;

extern GridSavePage grid_save_page;
extern GridWritePage grid_write_page;


extern GridTrack slot;

extern MCLEncoder grid_slot_param1;
extern MCLEncoder grid_slot_param2;

extern MenuPage<
    #ifndef OLED_DISPLAY
    8
    #else
    7
    #endif
>
grid_slot_page;
#endif /* GRIDPAGES_H__ */
