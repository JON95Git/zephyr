# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(lvgl)

FILE(GLOB app_sources src/*.c
    src/gui/gui_app.c
    src/gui/screen_tabs/gui_mass_storage_tab.c
    src/gui/screen_tabs/gui_random_tab.c
    src/gui/components/gui_spinbox.c
    src/gui/components/gui_widgets.c
)
target_sources(app PRIVATE ${app_sources})
