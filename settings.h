#ifndef _BPJ_REITREI_SETTINGS_H
#define _BPJ_REITREI_SETTINGS_H

class RenderSettings {
public:
  bool show_preview;
  int nworkers;
  int nsamples;
  int nrenders;
  int nshadows;
  int recurse_depth;
  bool coherence;
  bool point_lights;
  float dof_range;
  bool aa_enabled;
  float aa_threshold;

  RenderSettings() {
    show_preview = 1;
    nworkers = nsamples = nrenders = nshadows = 1;
    recurse_depth = 10;
    coherence = point_lights = 0;
    dof_range = 0.0;
    aa_enabled = 1;
    aa_threshold = 0.2;
  }
};

#endif
