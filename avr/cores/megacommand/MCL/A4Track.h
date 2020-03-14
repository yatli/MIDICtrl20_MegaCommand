/* Justin Mammarella jmamma@gmail.com 2018 */

#ifndef A4TRACK_H__
#define A4TRACK_H__

#include "ExtTrack.h"
// include full MDTrack specification to calculate size
#include "MDTrack.h"
#include "A4.h"
#include "Project.h"
#include "MCLMemory.h"
#include "Bank1Object.h"

class A4Track : public ExtTrack, 
                public Bank1Object<A4Track, NUM_MD_TRACKS, BANK1_A4_TRACKS_START> {
public:
  A4Sound sound;
  void load_seq_data(uint8_t tracknumber);
  bool get_track_from_sysex(uint8_t tracknumber, uint8_t column);
  bool place_track_in_sysex(uint8_t tracknumber, uint8_t column,
                           A4Sound *analogfour_sound);
  bool load_track_from_grid(uint8_t column, uint8_t row, int m = 0);
  bool store_track_in_grid(uint8_t column, uint8_t row, uint8_t track = 255, bool online = false);

};

#endif /* A4TRACK_H__ */
