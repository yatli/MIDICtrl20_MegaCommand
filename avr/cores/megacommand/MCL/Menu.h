#ifndef MENU_H__
#define MENU_H__

#include "MCL.h"

#define MAX_MENU_ITEMS 16
#define MAX_MENU_OPTIONS 16

typedef void (*FP)();

struct menu_option_t {
  uint8_t pos;
  char name[17];
};

struct menu_item_t {
  char name[17];
  uint8_t min;
  uint8_t range;
  uint8_t number_of_options;
  uint8_t *destination_var; // pointer to variable to be updated by param change
  Page *page_callback;
  void (*row_function)();
  menu_option_t options[MAX_MENU_OPTIONS];
};

struct menu_t {
  char name[11];
  uint8_t number_of_items;
  menu_item_t items[MAX_MENU_ITEMS];
  void (*exit_function)();
  Page *exit_page_callback;
};

class Menu {

public:
  menu_t *layout;
  uint8_t values[MAX_MENU_ITEMS];
  uint8_t entry_mask[(MAX_MENU_ITEMS + 7) / 8];

  Menu();

  void set_layout(menu_t *menu_layout);
  void enable_entry(uint8_t entry_index, bool en);
  bool is_entry_enable(uint8_t entry_index);

  PGM_P get_name();

  uint8_t get_number_of_items();
  menu_item_t *get_item(uint8_t item_n);
  uint8_t get_item_index(uint8_t item_n);
  PGM_P get_item_name(uint8_t item_n);
  Page *get_page_callback(uint8_t item_n);
  // Page *get_exit_page_callback();
  FP get_exit_function();
  FP get_row_function(uint8_t item_n);
  uint8_t *get_dest_variable(uint8_t item_n);
  uint8_t get_option_min(uint8_t item_n);
  uint8_t get_option_range(uint8_t item_n);
  uint8_t get_number_of_options(uint8_t item_n);
  PGM_P get_option_name(uint8_t item_n, uint8_t option_n);
};

#endif /* MENU_H__ */
