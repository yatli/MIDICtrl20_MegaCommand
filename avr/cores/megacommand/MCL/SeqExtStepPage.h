/* Justin Mammarella jmamma@gmail.com 2018 */

#ifndef SEQEXTSTEPPAGE_H__
#define SEQEXTSTEPPAGE_H__

#include "SeqPage.h"

struct musical_notes;

class SeqExtStepMidiEvents : public MidiCallback {
  bool state;

  void setup_callbacks();
  void remove_callbacks();

  void onNoteOnCallback_Midi2(uint8_t *msg);
  void onNoteOffCallback_Midi2(uint8_t *msg);
};

class SeqExtStepPage : public SeqPage {

public:
  SeqExtStepMidiEvents midi_events;
  SeqExtStepPage(Encoder *e1 = NULL, Encoder *e2 = NULL, Encoder *e3 = NULL,
                 Encoder *e4 = NULL)
      : SeqPage(e1, e2, e3, e4) {}
  bool handleEvent(gui_event_t *event);
  void display();

  void pattern_len_handler(Encoder *enc);
  void setup();
  void init();
};

#endif /* SEQEXTSTEPPAGE_H__ */
