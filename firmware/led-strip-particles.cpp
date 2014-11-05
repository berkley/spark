/*
Arduino library to simulate particle emission with an RGB LED strip.
Copyright (C) P. Mark Anderson
MIT license
*/

#include "led-strip-particles.h"
#include <stdlib.h>
#include <math.h>
#include "application.h"

#define MAX_COLOR 255

/*****************************************************************************/

ParticleEmitter::ParticleEmitter(uint16_t _numPixels, uint8_t _maxColor) {
    maxColor = fmin(MAX_COLOR, fmax(0, _maxColor));
    numPixels = _numPixels;
    numParticles = MAX_PARTICLES;
    maxVelocity = 0.1 / 60.0;  // (random(100) / 100.0 * 0.028) + 0.004;
    stripPosition = 0.5;
    zDeltaDirection = 1.0;
    threed = false;
    flicker = true;
    
    for (int i=0; i < numParticles; i++) {
        particles[i] = newParticle();
    }
}

Particle ParticleEmitter::newParticle() {
    Particle p;
    
    p.coord.x = (random(2) == 0 ? 0.0 : 1.0); //random(67) / 100.0 + 0.33;
    p.coord.y = 0.0;
    p.coord.z = 0.0;  //(threed ? random(100) / 100.0 : 0.0);

    int8_t direction = (p.coord.x > 0.5 ? -1 : 1);
    p.velocity.x = ((random(77) + 33) / 100.0) * direction;
    p.velocity.y = 0.0;

    direction = (random(2) == 0 ? 1 : -1);
    p.velocity.z = (M_PI/45); //(M_PI / 180.0 * random(90) * direction);

    uint8_t prtMaxColor = maxColor * (1.0 - (random(50) / 100.0));

    p.redColor = random(prtMaxColor);
    p.greenColor = random(prtMaxColor);
    p.blueColor = random(prtMaxColor);

    p.dimmed = false;        
    return p;
}

void ParticleEmitter::begin(void) {
}

Particle ParticleEmitter::updateParticle(uint16_t i) {
    Particle *p = &particles[i];
    
    if (flicker) {
        p->dimmed = (random(3) == 0 ? 1 : 0);
    }

    float zScale = (1.0 - (p->coord.z * 0.9));
    p->coord.x += (maxVelocity * p->velocity.x) * zScale;
    p->coord.y += (maxVelocity * p->velocity.y) * zScale;


    // Z velocity acts as theta
    p->coord.z = min(0.9, sin(p->velocity.z));
    p->velocity.z += M_PI/200.0 * zDeltaDirection;

    if (p->velocity.z >= M_PI/2 || p->velocity.z <= M_PI/90) {
        zDeltaDirection *= -1.0;
    }

    float maxHighVelocity = maxVelocity * GOLDEN_RATIO * 10.0; 
    float extra = 0.33;
    
    if (respawnOnOtherSide && p->velocity.x < maxHighVelocity && p->velocity.y < maxHighVelocity) {        
        if (p->coord.x < 0.0-extra) {
            p->coord.x = 1.0;
            p->velocity.x *= GOLDEN_RATIO;
        }
        else if (p->coord.x > 1.0+extra) {
            p->coord.x = 0.0;            
            p->velocity.x *= GOLDEN_RATIO;
        }

        if (p->coord.y < 0.0-extra) {
            p->coord.y = 1.0;            
            p->velocity.y *= GOLDEN_RATIO;
        }
        else if (p->coord.y > 1.0+extra) {
            p->coord.y = 0.0;            
            p->velocity.y *= GOLDEN_RATIO;
        }
    }
    else {
        if (p->coord.x < 0.0-extra || p->coord.x > 1.0+extra ||
            p->coord.y < 0.0-extra || p->coord.y > 1.0+extra) {
            *p = newParticle();            
        }
    }

    return *p;
}