// This #include statement was automatically added by the Particle IDE.
#include "led-strip-particles.h"
#include "neopixel/neopixel.h"

//particle params
#define MAX_COLOR 255
#define NUM_PARTICLES 12
#define FPS 30
#define MILLIS_PER_FRAME (1000 / FPS)

//neopixel
#define PIXEL_TYPE WS2812B
#define PIXEL_COUNT 150
#define PIXEL_PIN D2

SYSTEM_MODE(MANUAL);

Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE);
ParticleEmitter emitter = ParticleEmitter(PIXEL_COUNT, MAX_COLOR);

void setup() 
{
	//Serial.begin(9600);
	strip.begin();
	strip.show();

	int np = 15;
	int mv = 3;
	bool respawn = false;
	bool threed = false;
	bool flicker = false;

	emitter.respawnOnOtherSide = respawn;
	emitter.threed = threed;
	emitter.flicker = flicker;
	emitter.numParticles = np;
	float mvf = mv / 10.0;
	emitter.maxVelocity = mvf / FPS;
}

void loop()
{
	particles();	
}

void particles() {
    unsigned long frameStartMillis = millis();
    emitter.stripPosition = 0.5; //random(100) / 100.0;

    // Draw each particle
    for (int i=0; i < emitter.numParticles; i++) 
    {
        // Update this particle's position
        LEDParticle prt = emitter.updateParticle(i);

        float zScale = (emitter.threed ? (1.0 - prt.coord.z) : 1.0);
        double tailLength = (1.0 + abs(prt.velocity.x * 15) * zScale);
        int16_t startSlot = emitter.numPixels * prt.coord.x;
        int16_t currentSlot = startSlot;
        int16_t oldSlot = currentSlot;

        // Draw the particle and its tail
        // High velocity particles have longer tails
        for (int ii=0; ii < tailLength; ii++) {

            // Taper the tail fade
            float colorScale = ((tailLength - (ii * GOLDEN_RATIO)) / tailLength);

            if (ii == 0 && prt.dimmed) {
            // Flicker the first particle
                colorScale *= (random(50) / 100) + 0.05;
            }      

            if (emitter.threed) {
                colorScale *= zScale;
            }

            if (colorScale < 0.05) {
                colorScale = 0.0;
            }

            // Draw particle
            strip.setPixelColor(currentSlot, 
                                strip.Color(prt.redColor * colorScale, 
                                            prt.greenColor * colorScale, 
                                            prt.blueColor * colorScale));

            oldSlot = currentSlot;
            currentSlot = startSlot + ((ii+1) * (prt.velocity.x > 0 ? -1 : 1));
        }

        //Terminate the tail
        strip.setPixelColor(oldSlot, strip.Color(0, 0, 0));
    }

    uint16_t frameElapsedMillis = millis() - frameStartMillis;
    uint16_t frameDelayMillis = (MILLIS_PER_FRAME - frameElapsedMillis);

    if (frameDelayMillis > 0.0) {
        Serial.println(frameDelayMillis);
        delay(frameDelayMillis);
        strip.show();
    }
}

