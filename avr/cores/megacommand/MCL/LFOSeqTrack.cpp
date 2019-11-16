#include "LFO.h"
#include "LFOSeqTrack.h"
#include "MCL.h"

uint8_t LFOSeqTrack::get_wav_value(uint8_t sample_count, uint8_t param) {
  uint8_t offset = params[param].offset;
  uint8_t depth = params[param].depth;
  int8_t val;

  switch (offset_behaviour) {
  case LFO_OFFSET_CENTRE:
    val = offset + (wav_table[param][sample_count] - (depth / 2));
    if (val > 127) {
      return 127;
    }
    if (val < 0) {
      return 0;
    } else {
      return val;
    }
    break;
  case LFO_OFFSET_MAX:
    val = offset - depth + wav_table[param][sample_count];
    if (val > 127) {
      return 127;
    }
    if (val < 0) {
      return 0;
    } else {
      return val;
    }
    break;
  }
  return offset;
}

void LFOSeqTrack::seq() {

  if ((MidiClock.mod12_counter == 0) && (mode != LFO_MODE_FREE) &&
      IS_BIT_SET64(pattern_mask, step_count)) {
    sample_count = 0;
  }
  if (enable) {
    for (uint8_t i = 0; i < NUM_LFO_PARAMS; i++) {

      if (params[i].dest > 0) {
        // MD CC LFO
        if (params[i].dest <= NUM_MD_TRACKS) {
          MD.setTrackParam_inline(params[i].dest - 1, params[i].param,
                                  get_wav_value(sample_count, i));
        }
        // MD FX LFO
        else {
          MD.sendFXParam(params[i].param, get_wav_value(sample_count, i),
                         MD_FX_ECHO + params[i].dest - NUM_MD_TRACKS - 1);
        }
      }
    }
  }

  if (speed == 0) {
    sample_count += 2;
  }
  else {
    sample_hold += 1;
    if (sample_hold >= speed - 1) {
      sample_hold = 0;
      sample_count += 1;
    }
  }
  if (sample_count > LFO_LENGTH) {
    // Free running LFO should reset, oneshot should hold at last value.
    if (mode == LFO_MODE_ONE) {
      sample_count = LFO_LENGTH;
    } else {
      sample_count = 0;
    }
  }

  if (MidiClock.mod12_counter == 11) {
    if (step_count == length - 1) {
      step_count = 0;
    } else {
      step_count++;
    }
  }
}

void LFOSeqTrack::check_and_update_params_offset(uint8_t dest, uint8_t value) {
  for (uint8_t n = 0; n < NUM_LFO_PARAMS; n++) {
    if (params[n].dest == dest) {
      params[n].offset = value;
    }
  }
}

void LFOSeqTrack::reset_params_offset() {
  for (uint8_t n = 0; n < NUM_LFO_PARAMS; n++) {
    params[n].reset_param_offset();
  }
}

void LFOSeqTrack::update_params_offset() {
  for (uint8_t n = 0; n < NUM_LFO_PARAMS; n++) {
    params[n].update_offset();
  }
}
void LFOSeqParam::update_offset() { offset = get_param_offset(dest, param); }
void LFOSeqParam::reset_param_offset() { reset_param(dest, param, offset); }

void LFOSeqParam::reset_param(uint8_t dest, uint8_t param, uint8_t value) {
  if (dest <= NUM_MD_TRACKS) {
    MD.setTrackParam(dest - 1, param, value);
  } else {
    MD.sendFXParam(param, value, MD_FX_ECHO + dest - NUM_MD_TRACKS - 1);
  }
}

uint8_t LFOSeqParam::get_param_offset(uint8_t dest, uint8_t param) {
  if (dest <= NUM_MD_TRACKS) {
    return MD.kit.params[dest - 1][param];
  } else {
    switch (dest - NUM_MD_TRACKS - 1) {
    case MD_FX_ECHO - MD_FX_ECHO:
      return MD.kit.delay[param];
      break;
    case MD_FX_DYN - MD_FX_ECHO:
      return MD.kit.dynamics[param];
      break;

    case MD_FX_REV - MD_FX_ECHO:
      return MD.kit.reverb[param];
      break;
    case MD_FX_EQ - MD_FX_ECHO:
      return MD.kit.eq[param];
      break;
    }
  }
  return 255;
}