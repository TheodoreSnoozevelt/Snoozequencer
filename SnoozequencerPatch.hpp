#ifndef SNOOZEQUENCERPATCH_HPP
#define SNOOZEQUENCERPATCH_HPP
#define USE_MIDI_CALLBACK

#include "BassDrum.hpp"
#include "SnareDrum.hpp"
#include "Hat.hpp"
#include "AdsrEnvelope.h"
#include "Patch.h"

class SnoozequencerPatch : public Patch {
private:
    int blockSize;
    float sampleRate;
    SineOscillator* sin;
    AdsrEnvelope<true>* env;
    FloatArray sinFloat;
    FloatArray drumOutFloat;
    FloatArray drum2OutFloat;
    FloatArray drum3OutFloat;
    bool bdTriggers[16*4];
    bool sdTriggers[16*4];
    bool hdTriggers[16*4];
    bool on;
    int pos;
    BassDrum* bd;
    SnareDrum* sd;
    Hat* hd;
    int samples;
    int samples2;
    int lastClock;
    int clockLength;

public:
    SnoozequencerPatch() :
    sampleRate(getSampleRate()),
    blockSize(getBlockSize()),
    pos(0),
    samples(0),
    samples2(0),
    clockLength(0),
    on(false) {
        registerParameter(PARAMETER_A, "Metronome");

        sinFloat = FloatArray::create(blockSize);
        drumOutFloat = FloatArray::create(blockSize);
        drum2OutFloat = FloatArray::create(blockSize);
        drum3OutFloat = FloatArray::create(blockSize);

        sin = SineOscillator::create(sampleRate);
        sin->setFrequency(800);

        env = AdsrEnvelope<true>::create(sampleRate);
        env->setAttack(.01);
        env->setDecay(.05);
        env->setSustain(0);

        bd = new BassDrum(sampleRate);
        sd = new SnareDrum(sampleRate);
        hd = new Hat(sampleRate);
    }

    void buttonChanged(PatchButtonId bid, uint16_t value, uint16_t memes) override {
        if (bid == BUTTON_1 && value == ON) {
            on = true;
            lastClock = samples;
            samples = 0;
            clockLength = lastClock / 4;
        }
    }

    void processMidi(MidiMessage msg) override {
        if (msg.isNoteOn() && msg.getNote() == 36) {
            bdTriggers[pos] = true;
        }

        if (msg.isNoteOn() && msg.getNote() == 37) {
            sdTriggers[pos] = true;
        }

        if (msg.isNoteOn() && msg.getNote() == 38) {
            hdTriggers[pos] = true;
        }
    }

    void tick() {
        samples2 = 0;
        pos = (pos + 1) % 64;
        if (pos % (4*4) == 0) {
            sin->setFrequency(1000);
        }
        else {
            sin->setFrequency(800);
        }

        if (pos == 0) {
            sin->setFrequency(1200);
        }

        if (pos % 4 == 0)
            env->trigger();

        if (bdTriggers[pos]) {
            bd->trigger();
        }

        if (sdTriggers[pos]) {
            sd->trigger();
        }

        if (hdTriggers[pos]) {
            hd->trigger();
        }
    }

    void processAudio(AudioBuffer &buffer) override {
        samples += blockSize;
        samples2 += blockSize;
        if (on && samples2 > clockLength) {
            tick();
        }
        FloatArray inOutLeft = buffer.getSamples(0);

        sin->generate(sinFloat);
        sinFloat.multiply(getParameterValue(PARAMETER_A));

        bd->generate(drumOutFloat);
        sd->generate(drum2OutFloat);
        hd->generate(drum3OutFloat);

        env->process(sinFloat, inOutLeft);
        inOutLeft.add(drumOutFloat);
        inOutLeft.add(drum2OutFloat);
        inOutLeft.add(drum3OutFloat);
    }
};

#endif
