#include "ExtTrack.h"
#include "MCL.h"

bool ExtTrack::get_track_from_sysex(int tracknumber, uint8_t column) {

 active = EXT_TRACK_TYPE;
  return true;
}
bool ExtTrack::place_track_in_sysex(int tracknumber, uint8_t column) {
#ifdef EXT_TRACKS
  if (seq_data.resolution == 0) { seq_data.resolution = 1; }
  memcpy(&mcl_seq.ext_tracks[tracknumber], &seq_data,
           sizeof(seq_data));
#endif
  return true;
}
bool ExtTrack::load_track_from_grid(int32_t column, int32_t row, int m) {
  bool ret;
  int b = 0;

  int32_t offset = grid.get_slot_offset(column, row);

  int32_t len;
  ret = proj.file.seekSet(offset);
  if (!ret) {
    DEBUG_PRINT_FN();
    DEBUG_PRINTLN("Seek failed");
    return false;
  }
  if (m > 0) {
    ret = mcl_sd.read_data((uint8_t *)(this), m, &proj.file);
  } else {
    ret = mcl_sd.read_data((uint8_t *)(this), sizeof(ExtTrack), &proj.file);
  }

  if (!ret) {
    DEBUG_PRINT_FN();
    DEBUG_PRINTLN("Read failed");
    return false;
  }
  if (active == EMPTY_TRACK_TYPE) {
  seq_data.length = 16;
  }
  return true;
}
bool ExtTrack::store_track_in_grid(int track, int32_t column, int32_t row, bool online) {
  /*Assign a track to Grid i*/
  /*Extraact track data from received pattern and kit and store in track
   * object*/
  bool ret;

  int b = 0;
  DEBUG_PRINT_FN();
  int32_t len;

  int32_t offset = (column + (row * (int32_t)GRID_WIDTH)) * (int32_t)GRID_SLOT_BYTES;

  ret = proj.file.seekSet(offset);
  if (!ret) {
    DEBUG_PRINTLN("Seek failed");
    return false;
  }
  #ifdef EXT_TRACKS
  if (online) {
    get_track_from_sysex(track - 16, column - 16);
    memcpy(&seq_data, &mcl_seq.ext_tracks[track - 16],sizeof(seq_data));
  }
  #endif

  ret = mcl_sd.write_data((uint8_t *)this, sizeof(ExtTrack), &proj.file);
  if (!ret) {
    DEBUG_PRINTLN("Write failed");
    return false;
  }
  uint8_t model = column;
  grid_page.row_headers[grid_page.cur_row].update_model(column, model, EXT_TRACK_TYPE);

  return true;
}
