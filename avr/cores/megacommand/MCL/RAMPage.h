/* Justin Mammarella jmamma@gmail.com 2018 */

#ifndef RAMPAGE_H__
#define RAMPAGE_H__

#include "GUI.h"
#include "MCLEncoder.h"

#define NUM_RAM_PAGES 2
#define SLOT_RAM_RECORD (1 << (sizeof(GridChain::row) * 8)) - 1 - 1
#define SLOT_RAM_PLAY (1 << (sizeof(GridChain::row) * 8)) - 1 - 2

// 'wheel1', 19x19px
const unsigned char wheel_top [] PROGMEM = {
	0x03, 0xf8, 0x00, 0x0e, 0x0e, 0x00, 0x1e, 0x0f, 0x00, 0x3e, 0x0f, 0x80, 0x7f, 0x1f, 0xc0, 0x7f,
	0x1f, 0xc0, 0xff, 0xbf, 0xe0, 0xff, 0xff, 0xe0, 0xff, 0xbf, 0xe0, 0xff, 0x5f, 0xe0, 0xff, 0xbf,
	0xe0, 0xf8, 0xe3, 0xe0, 0x60, 0xe0, 0xe0, 0x40, 0xe0, 0x40, 0x61, 0xf0, 0xc0, 0x31, 0xf1, 0x80,
	0x1b, 0xf7, 0x00, 0x0f, 0xfe, 0x00, 0x03, 0xf8, 0x00
};
// 'wheel2', 19x19px
const unsigned char wheel_angle [] PROGMEM = {
	0x03, 0xf8, 0x00, 0x0f, 0xfe, 0x00, 0x1f, 0xfb, 0x00, 0x3f, 0xf1, 0x80, 0x7f, 0xf0, 0xc0, 0x7f,
	0xe0, 0x40, 0xff, 0xe0, 0xe0, 0x8f, 0xe3, 0xe0, 0x83, 0xbf, 0xe0, 0x81, 0x5f, 0xe0, 0x83, 0xbf,
	0xe0, 0x8f, 0xff, 0xe0, 0xff, 0xbf, 0xe0, 0x7f, 0x1f, 0xc0, 0x7f, 0x1f, 0xc0, 0x3e, 0x0f, 0x80,
	0x1e, 0x0f, 0x00, 0x0e, 0x0e, 0x00, 0x03, 0xf8, 0x00
};
// 'wheel3', 19x19px
const unsigned char wheel_side [] PROGMEM = {
	0x03, 0xf8, 0x00, 0x0f, 0xfe, 0x00, 0x1b, 0xff, 0x00, 0x31, 0xff, 0x80, 0x61, 0xff, 0xc0, 0x40,
	0xff, 0xc0, 0xe0, 0xff, 0xe0, 0xf8, 0xfe, 0x20, 0xff, 0xb8, 0x20, 0xff, 0x50, 0x20, 0xff, 0xb8,
	0x20, 0xf8, 0xfe, 0x20, 0xe0, 0xff, 0xe0, 0x40, 0xff, 0xc0, 0x61, 0xff, 0xc0, 0x31, 0xff, 0x80,
	0x1b, 0xff, 0x00, 0x0f, 0xfe, 0x00, 0x03, 0xf8, 0x00
};

class RAMPage : public LightPage, MidiCallback {
public:
  RAMPage(uint8_t _page_id, Encoder *e1 = NULL, Encoder *e2 = NULL,
          Encoder *e3 = NULL, Encoder *e4 = NULL)
      : LightPage(e1, e2, e3, e4) {
    page_id = _page_id;
    if (page_id == 0) {
      track1 = 15;
      track2 = 14;
    }
  }

  bool handleEvent(gui_event_t *event);
  bool midi_state = false;

  static uint8_t rec_states[NUM_RAM_PAGES];
  static uint8_t slice_modes[NUM_RAM_PAGES];

  uint8_t rec_state;
  uint8_t track1;
  uint8_t track2;
  uint8_t page_id;
  uint16_t transition_step;
  uint8_t record_len;

  uint8_t wheel_spin;
  uint16_t wheel_spin_last_clock;
  void display();
  void setup();
  void init();
  void loop();
  void cleanup();
  void setup_ram_rec(uint8_t track, uint8_t model, uint8_t lev, uint8_t source, uint8_t len,
                     uint8_t rate, uint8_t pan, uint8_t linked_track = 255);
  void setup_ram_rec_mono(uint8_t track, uint8_t lev, uint8_t source, uint8_t len,
                          uint8_t rate);
  void setup_ram_rec_stereo(uint8_t track, uint8_t lev, uint8_t source, uint8_t len,
                            uint8_t rate);
  void setup_ram_play(uint8_t track, uint8_t model, uint8_t pan,
                      uint8_t linked_track = 255);

  void setup_ram_play_mono(uint8_t track);
  void setup_ram_play_stereo(uint8_t track);

  void reverse(uint8_t track);
  bool slice(uint8_t track, uint8_t linked_track);
  void setup_sequencer(uint8_t track);

  void setup_callbacks();
  void remove_callbacks();

  void onControlChangeCallback_Midi(uint8_t *msg);
};

extern MCLEncoder ram_a_param1;
extern MCLEncoder ram_a_param2;
extern MCLEncoder ram_a_param3;
extern MCLEncoder ram_a_param4;

extern MCLEncoder ram_b_param1;
extern MCLEncoder ram_b_param2;
extern MCLEncoder ram_b_param3;
extern MCLEncoder ram_b_param4;

#endif /* RAMPAGE_H__ */