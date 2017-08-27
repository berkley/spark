/*
Library to simulate particle emission with an RGB LED strip.
Copyright (C) P. Mark Anderson
MIT license
*/

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>

#define MAX_PARTICLES 20
#define GOLDEN_RATIO 1.618

typedef struct {
  float x;
  float y; 
  float z;
} Coord3D;

typedef struct {
    Coord3D velocity;
    uint8_t redColor;
    uint8_t greenColor;
    uint8_t blueColor;
    bool dimmed;
    Coord3D coord;
} LEDParticle;

class ParticleEmitter {

 public:

  ParticleEmitter(uint16_t numPixels, uint8_t maxColor);
  ParticleEmitter(void);
  void
    begin(void);
  LEDParticle
    updateParticle(uint16_t i),
    newParticle();
  float
    stripPosition,
    maxVelocity;    
  uint16_t
    numPixels,    // Number of RGB LEDs in strip
    numParticles;
  uint8_t
    maxColor;
  bool
    respawnOnOtherSide,
    flicker,
    threed;

 private:

  LEDParticle
    particles[MAX_PARTICLES];
  float
    zDeltaDirection;
};
