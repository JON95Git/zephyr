#include "gui_app.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static gui_app_st gui;
static void gui_create_tabview_screen(void);

void gui_app(void)
{
    gui_create_tabview_screen();
    gui_create_mass_storage_tab(gui.tab_mass_storage);
    gui_create_random_tab(gui.tab_random);
}

static void gui_create_tabview_screen(void)
{
    gui.tabview = lv_tabview_create(lv_scr_act(), LV_DIR_TOP, 50);
    gui.tab_mass_storage = lv_tabview_add_tab(gui.tabview, LV_SYMBOL_SD_CARD" Mass storage");
    gui.tab_random = lv_tabview_add_tab(gui.tabview, LV_SYMBOL_SHUFFLE" Random");
}
