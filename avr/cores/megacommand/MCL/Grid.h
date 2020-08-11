/* Copyright Justin Mammarella jmamma@gmail.com 2018 */

#ifndef GRID_H__
#define GRID_H__

#include "GridPages.h"
#include "A4Track.h"

#define GRID_LENGTH 128
#define GRID_WIDTH 16
#define GRID_SLOT_BYTES 4096

#define GRID_VERSION 3000

class GridHeader {
public:
  uint32_t version;
  uint8_t id;
  uint32_t hash;
};

class Grid : public GridHeader {
public:

  File file;
  bool new_file(const char *gridname);
  bool new(const char *gridname);
  bool write_header();

  void setup();

  uint8_t get_slot_model(int column, int row, bool load);
  uint32_t get_slot_offset(int16_t column, int16_t row);
  uint32_t get_header_offset(int16_t row);
  bool copy_slot(int16_t s_col, int16_t s_row, int16_t d_col, int16_t d_row, bool destination_same);
  bool clear_slot(int16_t column, int16_t row, bool update_header = true);
  bool clear_row(int16_t row);
  bool clear_model(int16_t column, uint16_t row);

  bool write(void *data, size_t len, uint8_t col, uint16_t row);
  bool read(void *data, size_t len, uint8_t col, uint16_t row);

};

#endif /* GRID_H__ */
