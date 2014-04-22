#include "trace.h"
#include "randomizer.h"

RenderSettings settings;

inline Vect4 reflect(const Vect4& D, Vect4 N) {
  return (D + (2 * N * -dot(N, D))).unit();
}

inline Vect4 refract(const Vect4& D, Vect4 N, Real n) {
  Real c1 = -dot(D, N);
  if (c1 > 0) n = 1.0 / n; //This is an entrance.
  else if (abs(c1) < EPS) return D;//I don't bother with nearly parallel refraction
  else {N = -N; c1 = -c1;} //This is an exit
    
  Real c2 = 1 - n * n * (1 - c1 * c1);
  if (c2 > 0) c2 = sqrt(c2);
  else c2 = sqrt(-c2); //This should really never happen unless the refractive index is abused
  return n * D + (n * c1 - c2) * N;
}

Vect4 traceRay(const Scene& scene, const Vect4& O, const Vect4& D, int nrecurse) {
  Vect4 color;
  
  if (nrecurse >= settings.recurse_depth) return color;
 
  Intersection nearest = scene.intersect(O, D);

  if (nearest.t <= 0) return scene.bgcolor;

  //Ambient lighting
  color += nearest.obj->material.ambient;  

  //Reflective lighting
  if (nonzero(nearest.obj->material.reflective)) {
    Vect4 _D = reflect(D, nearest.N);
    Vect4 _O = nearest.P + EPS * _D;
    color += nearest.obj->material.reflective.multComp(traceRay(scene, _O, _D, nrecurse + 1));
  }

  //Refractive lighting
  if (nonzero(nearest.obj->material.refractive)) {
    Vect4 _D = refract(D, nearest.N, nearest.obj->material.refractive_index);
    Vect4 _O = nearest.P + EPS * _D;
    color += nearest.obj->material.refractive.multComp(traceRay(scene, _O, _D, nrecurse + 1));
  }

  //Lighting-dependent color
  for (int l = 0; l < scene.countLights(); l++) {
    Vect4 lv, shadow_ray;
    Real diffuse_power = 0.0, specular_power = 0.0, coef;
    for (int s = 0; s < settings.nshadows; s++) {
      if (settings.point_lights)
	lv = scene.getLight(l)->position - nearest.P;
      else
	lv = scene.getLight(l)->position + randomizer.randomSpherical(scene.getLight(l)->radius) - nearest.P;
      shadow_ray = lv.unit();

      Intersection shadow = scene.intersect(nearest.P + EPS * shadow_ray, shadow_ray, TRACE_SHADOW);
      if (shadow.t >= 0 && shadow.t < lv.length()) continue;

      //Specular
      if (nearest.obj->material.specular > 0 && nearest.obj->material.shininess > 0) {
	Vect4 R = reflect(D, nearest.N);
	coef = dot(shadow_ray, R);

	if (scene.getLight(l)->falloff) coef /= lv.sqLength();
	if (coef < 0.0 && nearest.obj->material.twosided) coef = -coef;
	if (coef > 0.0) specular_power += pow(coef, nearest.obj->material.shininess);
      }
    
      //Diffuse
      coef = dot(shadow_ray, nearest.N);
      if (scene.getLight(l)->falloff) coef /= lv.sqLength();
      if (coef < 0.0 && nearest.obj->material.twosided) coef = -coef;
      if (coef > 0.0) diffuse_power += coef;
    }

    diffuse_power /= settings.nshadows;
    specular_power /= settings.nshadows;

    color += nearest.obj->material.diffuse.multComp(scene.getLight(l)->getColor() * diffuse_power);
    color += scene.getLight(l)->getColor() * nearest.obj->material.specular * specular_power;
  }
  
  return color;
}

void traceAt(const Scene& scene, Texture& screen, int r, int c) {
  if (settings.coherence) randomizer.reseed();

  Vect4 O, D, color;
  float rmag, rth, rx, ry;
  Camera camera;
  for (int i = 0; i < settings.nsamples; i++) {    
    if (settings.dof_range > 0.0) {
      rmag = randomizer.uniform() * settings.dof_range; 
      rth = randomizer.uniform() * 2 * PI;
      rx = rmag * cos(rth);
      ry = rmag * sin(rth);
    }
    else {rx = ry = 0.0;}

    camera = scene.camera;
    camera.xrotate(rx);
    camera.yrotate(ry);    
    O = camera.getOrigin();
    D = camera.getDirection(r, c);
    color += traceRay(scene, O, D);
  }
      
  screen.setColor(r, c, color / settings.nsamples);
}

void traceAt_AA(const Scene& scene, Texture& screen, int r, int c) {
  if (settings.coherence) randomizer.reseed();

  Vect4 O, D, color;
  float rmag, rth, rx, ry;
  Camera camera;
  for (int i = 0; i < settings.nsamples; i++) {    
    if (settings.dof_range > 0.0) {
      rmag = randomizer.uniform() * settings.dof_range; 
      rth = randomizer.uniform() * 2 * PI;
      rx = rmag * cos(rth);
      ry = rmag * sin(rth);
    }
    else {rx = ry = 0.0;}

    camera = scene.camera;
    camera.xrotate(rx);
    camera.yrotate(ry);
    
    O = scene.camera.getOrigin();
    for (Real r1 = -0.5; r1 <= 0.5; r1 += 0.5)
      for (Real c1 = -0.5; c1 <= 0.5; c1 += 0.5) {
	D = scene.camera.getDirection(r + r1, c + c1);
	color += traceRay(scene, O, D) / 9.0;
      }
  }
      
  screen.setColor(r, c, color / settings.nsamples);
}
