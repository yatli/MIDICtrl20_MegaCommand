/* Copyright 2018, Justin Mammarella jmamma@gmail.com */
#include "MCL.h"
#include "MCLActions.h"

void MCLActions::setup() {
  DEBUG_PRINTLN("mcl actions setup");
  mcl_actions_callbacks.setup_callbacks();
  mcl_actions_midievents.setup_callbacks();
}

void MCLActions::kit_reload(uint8_t pattern) {
  DEBUG_PRINT_FN();
  if (mcl_actions.do_kit_reload != 255) {
    if (mcl_actions.writepattern == pattern) {
      MD.loadKit(mcl_actions.do_kit_reload);
    }
    mcl_actions.do_kit_reload = 255;
  }
}

MCLActions mcl_actions;

bool MCLActions::place_track_inpattern(int curtrack, int column, int row,
                                       A4Sound *analogfour_sound) {

  //       if (Grids[encodervaluer] != NULL) {
  MDTrack *md_track = (MDTrack *)&temp_track;
  A4Track *a4_track = (A4Track *)&temp_track;
  ExtTrack *ext_track = (ExtTrack *)&temp_track;

  DEBUG_PRINT_FN();
  if (column < 16) {

    if (md_track->load_track_from_grid(column, row, 0)) {
      if (md_track->active != EMPTY_TRACK_TYPE) {
        md_track->place_track_in_sysex(curtrack, column);
      }
    }
  } else {
    if (Analog4.connected) {

      if (a4_track->load_track_from_grid(column, row, 0)) {
        if (a4_track->active != EMPTY_TRACK_TYPE) {

          return a4_track->place_track_in_sysex(curtrack, column,
                                                analogfour_sound);
        }
      }
    } else {
      if (ext_track->load_track_from_grid(column, row, 0)) {
        if (ext_track->active != EMPTY_TRACK_TYPE) {

          return ext_track->place_track_in_sysex(curtrack, column);
        }
      }
    }
  }
}

void MCLActions::md_setsysex_recpos(uint8_t rec_type, uint8_t position) {
  DEBUG_PRINT_FN();

  uint8_t data[] = {0x6b, (uint8_t)rec_type & 0x7F, position,
                    (uint8_t)1 & 0x7f};
  MD.sendSysex(data, countof(data));

  //  MD.sendRequest(0x6b,00000011);
}

void MCLActions::store_tracks_in_mem(int column, int row,
                                     int store_behaviour_) {
  DEBUG_PRINT_FN();

  MDTrack *md_track = (MDTrack *)&temp_track;
  A4Track *a4_track = (A4Track *)&temp_track;
  ExtTrack *ext_track = (ExtTrack *)&temp_track;

  int16_t tclock = slowclock;
  uint8_t readpattern = MD.currentPattern;
  if ((gridio_param1.getValue() * 16 + gridio_param2.getValue()) !=
      MD.currentPattern) {
    readpattern = (gridio_param1.getValue() * 16 + gridio_param2.getValue());
  }

  store_behaviour = store_behaviour_;
  setLed();
  patternswitch = PATTERN_STORE;

  bool save_md_tracks = false;
  bool save_a4_tracks = false;
  uint8_t i = 0;
  for (i = 0; i < 16; i++) {
    if (note_interface.notes[i] == 3) {
      save_md_tracks = true;
    }
  }
  for (i = 16; i < 20; i++) {
    if (note_interface.notes[i] == 3) {
      save_a4_tracks = true;
    }
  }
  if (save_md_tracks) {
    if (!MD.getBlockingPattern(readpattern)) {
      DEBUG_PRINTLN("could not receive pattern");
      return;
    }
    int curkit;
    if (readpattern != MD.currentPattern) {
      curkit = MD.pattern.kit;
    } else {

      curkit = MD.getCurrentKit(CALLBACK_TIMEOUT);
      if ((mcl_cfg.auto_save == 1)) {
        MD.saveCurrentKit(MD.currentKit);
      }
    }

    if (!MD.getBlockingKit(curkit)) {
      DEBUG_PRINTLN("could not receive kit");
      return;
    }
  }

  // A4Track analogfour_track;
  //  MDTrack md_track->;

  bool n;
  /*Send a quick sysex message to get the current selected track of the MD*/

  //       int curtrack = 0;

  uint8_t first_note = 254;

  int curtrack = 0;
  if (store_behaviour == STORE_AT_SPECIFIC) {
    curtrack = last_md_track;
    // MD.getCurrentTrack(CALLBACK_TIMEOUT);
  }
  uint8_t max_notes = 20;
  if (!Analog4.connected) {
    max_notes = 16;
  }
  for (i = 0; i < max_notes; i++) {
    if (note_interface.notes[i] == 3) {
      if (first_note == 254) {
        first_note = i;
      }

      if (store_behaviour == STORE_IN_PLACE) {
        if ((i >= 16) && (i < 20)) {
          if (Analog4.connected) {
            DEBUG_PRINTLN("a4 get sound");
            Analog4.getBlockingSoundX(i - 16);
            a4_track->sound.fromSysex(MidiSysex2.data + 8,
                                      MidiSysex2.recordLen - 8);
          }
          n = a4_track->store_track_in_grid(i, i, grid_page.getRow());
        } else {
          n = md_track->store_track_in_grid(i, i, grid_page.getRow());
        }
      }

      if ((store_behaviour == STORE_AT_SPECIFIC) && (i < 16)) {
        n = md_track->store_track_in_grid(
            (i - first_note), grid_page.getCol() + i, grid_page.getRow());
      }
      // CLEAR_BIT32(note_interface.notes, i);
    }
  }
  grid_page.row_headers[grid_page.cur_row].active = true;
  grid_page.row_headers[grid_page.cur_row].write(grid_page.getRow());
  clearLed();
  DEBUG_PRINTLN(slowclock - tclock);
}

void MCLActions::write_tracks_to_md(int column, int row, int b) {
  DEBUG_PRINT_FN();

  store_behaviour = b;
  writepattern = MD.currentPattern;
  if (((gridio_param1.getValue() * 16 + gridio_param2.getValue()) !=
       MD.currentPattern)) {
    writepattern = (gridio_param1.getValue() * 16 + gridio_param2.getValue());
  }

  // Get pattern first, hopefully with the original kit assigned.
  if (write_original != 1) {
    if (!MD.getBlockingPattern(MD.currentPattern)) {
      DEBUG_PRINTLN("could not get blocking pattern");
      return;
    }
   if (gridio_param3.getValue() != MD.currentKit) {
      MD.currentKit = gridio_param3.getValue();
    } else {
      MD.saveCurrentKit(MD.currentKit);
    }

    MD.getBlockingKit(MD.currentKit);
  }
  patternswitch = 1;

  send_pattern_kit_to_md();
  patternswitch = PATTERN_UDEF;

  //  }

  // clearLed();
}

void MCLActions::send_pattern_kit_to_md() {
  DEBUG_PRINT_FN();

  MDTrack *md_track = (MDTrack *)&temp_track;
  A4Track *a4_track = (A4Track *)&temp_track;
  ExtTrack *ext_track = (ExtTrack *)&temp_track;

  md_track->load_track_from_grid(0, grid_page.getRow(), 0);
  // if (!Analog4.getBlockingKitX(0)) { return; }
  // if (!analog4_kit.fromSysex(MidiSysex2.data + 8, MidiSysex2.recordLen - 8))
  // { return; }

  /*Send a quick sysex message to get the current selected track of the MD*/
  int curtrack = last_md_track;
  // MD.getCurrentTrack(CALLBACK_TIMEOUT);
  uint8_t reload = 1;
  uint16_t quantize_mute = 0;
  uint8_t q_pattern_change = 0;
  if (writepattern != MD.currentPattern) {
    reload = 0;
  }
  if (gridio_param4.getValue() == 0) {
    quantize_mute = 0;
  } else if (gridio_param4.getValue() < 7) {
    quantize_mute = 1 << gridio_param4.getValue();
  }
  if (gridio_param4.getValue() == 7) {
    quantize_mute = 254;
  }
  if (gridio_param4.getValue() == 8) {
    quantize_mute = 254;
  }
  if (gridio_param4.getValue() >= 9) {
    quantize_mute = MD.pattern.patternLength;
    q_pattern_change = 1;
    reload = 0;
    if ((gridio_param4.getValue() == 9) &&
        (writepattern == MD.currentPattern)) {
      reload = 1;
    }
    if (gridio_param4.getValue() == 10) {
      if (writepattern == 127) {
        writepattern = 0;
      } else {
        writepattern = writepattern + 1;
      }
      gridio_param4.cur = 11;
    } else if (gridio_param4.getValue() == 11) {
      if (writepattern == 0) {
        writepattern = 127;
      } else {
        writepattern = writepattern - 1;
      }
      gridio_param4.cur = 10;
    }
  }

  /*Define sysex encoder objects for the Pattern and Kit*/
  ElektronDataToSysexEncoder encoder(&MidiUart);
  ElektronDataToSysexEncoder encoder2(&MidiUart);
  /*Write the selected trackinto a Pattern object by moving it from a Track
    object into a Pattern object The destination track is the currently selected
    track on the machinedrum.
  */

  uint8_t i = 0;
  int track = 0;
  uint8_t note_count = 0;
  uint8_t first_note = 254;

  // Used as a way of flaggin which A4 tracks are to be sent
  uint8_t a4_send[6] = {0, 0, 0, 0, 0, 0};
  A4Sound sound_array[4];
  KitExtra kit_extra;


  while ((i < 20)) {

    if ((note_interface.notes[i] > 1)) {
      if (first_note == 254) {
        first_note = i;
      }
      //  if (grid_page.encoders[0]->cur > 0) {
      if (store_behaviour == STORE_IN_PLACE) {
        track = i;

        if (i < 16) {
          if (i == first_note) {
          //Use first track's original kit values for write orig
          m_memcpy(&kit_extra,&(md_track->kitextra),sizeof(kit_extra));
          if (write_original == 1) { MD.pattern.patternLength = kit_extra.patternLength; }
          }
          place_track_inpattern(track, i, grid_page.getRow(),
                                (A4Sound *)&sound_array[0]);
        } else {
          track = track - 16;
          mcl_seq.ext_tracks[track].buffer_notesoff();
          if (place_track_inpattern(track, i, grid_page.getRow(),
                                    (A4Sound *)&sound_array[track])) {
            if (Analog4.connected) {
              sound_array[track].workSpace = true;
              a4_send[track] = 1;
            }
          }
        }
      }

      else if ((curtrack + (i - first_note) < 16) && (i < 16)) {
        track = curtrack + (i - first_note);
        place_track_inpattern(track, i, grid_page.getRow(), &sound_array[0]);
      }

      if (gridio_param4.getValue() == 8) {
        if (i < 16) {
          MD.kit.levels[track] = 0;
        } else if (Analog4.connected) {
          Analog4.setLevel(i - 16, 0);
        }
      }
      //   }

      note_count++;
      if ((quantize_mute > 0) && (gridio_param4.getValue() < 8)) {
        if (i < 16) {
          MD.muteTrack(track, true);
        } else {
          mcl_seq.ext_tracks[i - 16].mute = SEQ_MUTE_ON;
        }
      }
    }
    i++;
  }

  /*Set the pattern position on the MD the pattern is to be written to*/

  setLed();

  /*Send the encoded pattern to the MD via sysex*/

  // int temp = MD.getCurrentKit(CALLBACK_TIMEOUT);

  /*Tell the MD to receive the kit sysexdump in the current kit position*/

  /* Retrieve the position of the current kit loaded by the MD.
    Use this position to store the modi
  */
  // If write original, let's copy the master fx settings from the first track
  // in row Let's also set the kit receive position to be the original.

  if ((write_original == 1)) {

    //     MD.kit.origPosition = md_track->origPosition;
    for (uint8_t c = 0; c < 17; c++) {
      MD.kit.name[c] = grid_page.row_headers[grid_page.cur_row].name[c];
    }
    m_memcpy(&MD.kit.reverb[0], kit_extra.reverb,
             sizeof(kit_extra.reverb));
    m_memcpy(&MD.kit.delay[0], kit_extra.delay,
             sizeof(kit_extra.delay));
    m_memcpy(&MD.kit.eq[0], kit_extra.eq,
             sizeof(kit_extra.eq));
    m_memcpy(&MD.kit.dynamics[0], kit_extra.dynamics,
             sizeof(kit_extra.dynamics));
    MD.pattern.swingAmount = kit_extra.swingAmount;
    MD.pattern.accentAmount = kit_extra.accentAmount;
    MD.pattern.doubleTempo = kit_extra.doubleTempo;
    MD.pattern.scale = kit_extra.scale;
  }
  // MD.kit.origPosition = MD.currentKit;

  // Kit
  // If Kit is OG.
  if (gridio_param3.getValue() == 64) {

    MD.kit.origPosition = md_track->origPosition;
    MD.pattern.kit = md_track->origPosition;

  }

  else {

    MD.pattern.kit = MD.currentKit;
    MD.kit.origPosition = MD.currentKit;
    //       }
  }
  // If Pattern is OG
  if (gridio_param1.getValue() == 8) {
    MD.pattern.origPosition = md_track->patternOrigPosition;
    reload = 0;
  } else {
    MD.pattern.setPosition(writepattern);
  }

  // MidiUart.setActiveSenseTimer(0);
  in_sysex = 1;

  md_setsysex_recpos(8, MD.pattern.origPosition);

  MD.pattern.toSysex(encoder);
  in_sysex = 1;

  /*Send the encoded kit to the MD via sysex*/
  md_setsysex_recpos(4, MD.kit.origPosition);
  MD.kit.toSysex(encoder2);
  /*Instruct the MD to reload the kit, as the kit changes won't update until the
   * kit is reloaded*/
  //
  //    MidiUart.setActiveSenseTimer(290);
  if (reload == 1) {
    MD.loadKit(MD.pattern.kit);
  } else if ((q_pattern_change == 1) || (writepattern != MD.currentPattern)) {
    do_kit_reload = MD.pattern.kit;
    if (q_pattern_change == 1) {
      MD.loadPattern(writepattern);
    }
  }
  /*kit_sendmode != 1 therefore we are going to send the Machine via Sysex and
   * Midi cc without sending the kit*/

  // I fthe sequencer is running then we will pause and wait for the next
  // divison
  //  for (int n=0; n < 16; n++) {
  //      MD.global.baseChannel = 10;
  //      for (i=0; i < 16; i++) {
  //       MD.muteTrack(i,true);
  //      }
  // }
  //          delay(100);
  // Midiclock start hack

  // Send Analog4
  if (Analog4.connected) {
    uint8_t a4_kit_send = 0;
    // if (write_original == 1) {
    in_sysex2 = 1;
    for (i = 0; i < 4; i++) {
      if (a4_send[i] == 1) {
        sound_array[i].toSysex();
      }
    }
    in_sysex2 = 0;
  }

  if (mcl_actions_callbacks.start_clock32th > MidiClock.div32th_counter) {
    mcl_actions_callbacks.start_clock32th = 0;
  }
  if (mcl_actions_callbacks.start_clock96th > MidiClock.div96th_counter) {
    mcl_actions_callbacks.start_clock96th = 0;
  }

  if (quantize_mute > 0) {
    if (MidiClock.state == 2) {
      if ((q_pattern_change != 1) && (quantize_mute <= 64)) {
        // (MidiClock.div32th_counter - mcl_actions_callbacks.start_clock32th)
        //                   while (((MidiClock.div32th_counter + 3) %
        //                   (quantize_mute * 2))  != 0) {
        while ((((MidiClock.div32th_counter -
                  mcl_actions_callbacks.start_clock32th) +
                 3) %
                (quantize_mute * 2)) != 0) {
          GUI.display();
        }
      }

      if (q_pattern_change != 1) {
        for (i = 0; i < 20; i++) {
          // If we're in cue mode, send the track to cue before unmuting
          if ((note_interface.notes[i] > 1)) {
            if ((gridio_param4.getValue() == 7) && (i < 16)) {
              SET_BIT32(mcl_cfg.cues, i);
              MD.setTrackRouting(i, 5);
            }
            if (i < 16) {
              MD.muteTrack(i, false);
            } else {
              mcl_seq.ext_tracks[i - 16].mute = SEQ_MUTE_OFF;
            }
          }
        }
      }
    }
    if (gridio_param4.getValue() == 7) {
      md_exploit.send_globals();
    }
  }

  in_sysex = 0;

  clearLed();
  /*All the tracks have been sent so clear the write queue*/
  write_original = 0;
}
