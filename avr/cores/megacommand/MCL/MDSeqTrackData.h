/* Justin Mammarella jmamma@gmail.com 2018 */

#ifndef MDSEQTRACKDATA_H__
#define MDSEQTRACKDATA_H__

#define NUM_MD_LOCKS 4

class MDSeqStep {
public:
  bool active;
  uint8_t locks[NUM_MD_LOCKS];
  uint8_t conditional;
  uint8_t timing;
  bool lock_mask;
  bool pattern_mask;
};

class MDSeqTrackData {
public:
  uint8_t length;
  uint8_t resolution;
  uint8_t reserved[4];
  uint8_t locks[NUM_MD_LOCKS][64];
  uint8_t locks_params[NUM_MD_LOCKS];
  uint64_t pattern_mask;
  uint64_t lock_mask;
  uint8_t conditional[64];
  uint8_t timing[64];
  void init() {
  length = 16;
  resolution = 1;
  memset(&locks, 0, NUM_MD_LOCKS * 64);
  memset(&locks_params, 0, NUM_MD_LOCKS);
  pattern_mask = 0;
  lock_mask = 0;
  memset(&conditional, 0, 64);
  memset(&timing, 12, 64);
  }
};

#endif /* MDSEQTRACKDATA_H__ */
