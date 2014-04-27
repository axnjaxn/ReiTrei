#ifndef _BPJ_REITREI_TRACE_H
#define _BPJ_REITREI_TRACE_H

#include "mat4.h"
#include "scene.h"
#include "texture.h"
#include "settings.h"

extern RenderSettings settings;

Vect3 traceRay(const Scene& scene, const Vect3& O, const Vect3& D, int nrecurse = 0);
void traceAt(const Scene& scene, Texture& screen, int r, int c);
void traceAt_AA(const Scene& scene, Texture& screen, int r, int c);

#endif
