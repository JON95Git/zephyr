#include <stdio.h>
#include "gui_mass_storage_tab.h"
#include "../components/gui_spinbox.h"
// #include "app.h"
#ifdef EMBEDDED
#include "app_threads.h"
#endif
#include <zephyr.h>
#define USER_COLOR lv_color_hex(0x034885)

static _gui_label_st name_label;
static _gui_label_st size_label;
static _gui_spinbox_st size_spinbox;
static lv_obj_t *start_button;
static lv_obj_t *name_text_area = NULL;
static lv_obj_t *kb = NULL;
static lv_obj_t *mbox1;
static lv_obj_t *mbox2;
static lv_obj_t *main_parent;
static lv_obj_t *preload;

static void _gui_init_spinbox(lv_obj_t *parent, lv_coord_t x_ofs,
                lv_coord_t y_ofs, _gui_spinbox_st *spinbox);
static void _gui_create_labels(lv_obj_t *parent);
static void _create_err_msg_box(const char *error_message);
static void _create_save_msg_box(lv_obj_t *parent);
static void create_keyboard(lv_obj_t *parent);
static void _create_text_area_name(lv_obj_t *parent);
static void _gui_create_save_button(lv_obj_t *parent);
static void _gui_create_param_label(lv_obj_t *parent, const char *string,
                lv_coord_t x_ofs, lv_coord_t y_ofs, _gui_label_st *label_pwm_st);
static void ta_event_cb(lv_event_t *event);

void gui_create_mass_storage_tab(lv_obj_t *parent)
{
    _gui_create_labels(parent);
    _gui_create_save_button(parent);
    _create_text_area_name(parent);
    _gui_init_spinbox(parent, 5, 0, &size_spinbox);
    main_parent = parent;
}

static void mbox1_event_cb(lv_event_t *evt)
{
    lv_event_code_t code = lv_event_get_code(evt);
    if (code == LV_EVENT_DELETE) {
        if (evt->target == mbox1) {
            mbox1 = NULL;
        }
    } else if (code == LV_EVENT_VALUE_CHANGED) {
        if (evt->target == mbox1) {
            lv_msgbox_close(mbox1);
        }
    }
}

static void mbox2_event_cb(lv_event_t *evt)
{
    lv_event_code_t code = lv_event_get_code(evt);
    if (code == LV_EVENT_DELETE) {
        mbox2 = NULL;
    } else if (code == LV_EVENT_VALUE_CHANGED) {
        lv_msgbox_close(mbox2);
    }
}

static void _create_err_msg_box(const char *error_message)
{
    // static const char *btns[] ={"OK",""};
    mbox1 = lv_msgbox_create(lv_scr_act(), "ERROR", error_message, NULL, true);
    // lv_msgbox_set_text(mbox1, error_message);
    // lv_msgbox_add_btns(mbox1, btns);
    lv_obj_set_width(mbox1, 200);
    lv_obj_add_event_cb(mbox1, mbox1_event_cb, LV_EVENT_ALL, NULL);
    lv_obj_align(mbox1, LV_ALIGN_CENTER, 0, 0);
}

static void _create_save_msg_box(lv_obj_t *parent)
{
    printf("_create_save_msg_box\n\r");
    // static const char *btns[] ={"OK",""};
    mbox2 = lv_msgbox_create(parent, "", "Saving samples on SD card...", NULL, false);
    // lv_msgbox_set_text(mbox2, "Saving samples on SD card...");
    lv_obj_set_size(mbox2, 200, 200);
    // lv_obj_set_width(mbox2, 100);
    lv_obj_add_event_cb(mbox2, mbox2_event_cb, LV_EVENT_ALL, NULL);
    lv_obj_align(mbox2, LV_ALIGN_CENTER, 0, 0);
}

static int _check_data_before_try_generate_file(char *string)
{
    int ret;
    const char *string_name;
    string_name = lv_textarea_get_text(name_text_area);
    printf("string_name: %s\n\r", string_name);
    ret = memcmp(string_name, "", 1);
    strcpy(string, string_name);
    return ret;
}

static void _gui_init_spinbox(lv_obj_t *parent, lv_coord_t x_ofs,
                lv_coord_t y_ofs, _gui_spinbox_st *spinbox)
{
    spinbox->x_ofs = x_ofs;
    spinbox->y_ofs = y_ofs;
    gui_create_spinbox(parent, spinbox);
}

static void kb_event_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_READY) {
        printf("deleting KB\n");
        lv_keyboard_set_textarea(kb, NULL);
        lv_obj_del(kb);
        kb = NULL;
    }
}

static void create_keyboard(lv_obj_t *parent)
{
    kb = lv_keyboard_create(parent);
    lv_obj_set_size(kb,  LV_HOR_RES - 40, LV_VER_RES / 2);
    lv_obj_add_event_cb(kb, kb_event_cb, LV_EVENT_ALL, NULL);
    lv_obj_align(kb, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_keyboard_set_textarea(kb, name_text_area);
    // lv_keyboard_set_cursor_manage(kb, true);
}

static void _create_text_area_name(lv_obj_t *parent)
{
    name_text_area = lv_textarea_create(parent);
    lv_textarea_set_text(name_text_area, "");
    // lv_textarea_set_pwd_mode(name_text_area, false);
    // lv_textarea_set_cursor_hidden(name_text_area, true);
#ifndef EMBEDDED
    lv_obj_align(name_text_area, LV_ALIGN_TOP_RIGHT, -85, 10);
#else
    lv_obj_align(name_text_area, LV_ALIGN_TOP_RIGHT, -30, 20);
#endif
    lv_obj_set_size(name_text_area, 200, 40);
    lv_obj_add_event_cb(name_text_area, ta_event_cb, LV_EVENT_ALL, NULL);
}

static void ta_event_cb(lv_event_t *event)
{
    lv_event_code_t code = lv_event_get_code(event);
    if (code == LV_EVENT_CLICKED) {
        if(kb == NULL){
            create_keyboard(name_text_area->parent);
            lv_keyboard_set_textarea(kb, name_text_area);
            lv_obj_clear_flag(kb, LV_OBJ_FLAG_HIDDEN);
        }
    }

    if(code == LV_EVENT_INSERT) {
        const char * str = lv_event_get_user_data(event);
        lv_obj_clear_flag(kb, LV_OBJ_FLAG_HIDDEN);
        if(str[0] == '\n') {
            printf("Ready\n");
        }
    }
}

static void _gui_create_labels(lv_obj_t *parent)
{
    _gui_create_param_label(parent, "Name: ", -170, -70, &name_label);
    _gui_create_param_label(parent, "Size (MB): ", -160, 00, &size_label);
}

static void create_spinner(lv_obj_t *parent)
{
    printf("create_spinner\n\r");
    preload = lv_spinner_create(parent, 1000, 60);
    lv_obj_set_size(preload, 120, 120);
    lv_obj_set_align(preload, LV_ALIGN_OUT_RIGHT_MID);
    // lv_obj_center(preload);
}

static void save_button_event_handler(lv_event_t *event)
{
    int ret = 0;
    int32_t spinbox_value = 0;
    char string[50];

    if (event->code == LV_EVENT_CLICKED) {
        ret = _check_data_before_try_generate_file(string);
        if (ret == 0) {
            printf("Invalid name\n\r");
            _create_err_msg_box("Invalid name. Please insert a valid name");
            return;
        }
        gui_get_spinbox_value(size_spinbox.spinbox, &spinbox_value);
        if (spinbox_value == 0) {
            printf("Invalid file size\n\r");
            _create_err_msg_box("Invalid file size. Please insert a valid file size");
            return;
        }
        _create_save_msg_box(main_parent);
        create_spinner(mbox2);
#ifdef EMBEDDED 
        put_queue_rng_thread(string, spinbox_value);
# else
        // create_file_from_random_data(string, spinbox_value);
        // k_sleep(K_MSEC(5000));
        // gui_close();
#endif
    }
}

static void _gui_create_save_button(lv_obj_t *parent)
{
    lv_obj_t *button_label;

    start_button = lv_btn_create(parent);
    lv_obj_add_event_cb(start_button, save_button_event_handler, LV_EVENT_ALL, NULL);
    lv_obj_align(start_button, LV_ALIGN_CENTER, 0, 50);

    button_label = lv_label_create(start_button);
    lv_label_set_text(button_label, "Save");
}

static void _gui_create_param_label(lv_obj_t *parent, const char *string,
                lv_coord_t x_ofs, lv_coord_t y_ofs, _gui_label_st *label_pwm_st)
{
    *label_pwm_st = (_gui_label_st){.parent = parent, 
                                .text = string, 
                                .align = LV_ALIGN_CENTER, 
                                .x_ofs = x_ofs, 
                                .y_ofs = y_ofs};

    label_pwm_st->label  = gui_create_label(label_pwm_st);
}

void gui_close(void)
{
    lv_msgbox_close(mbox2);
}
