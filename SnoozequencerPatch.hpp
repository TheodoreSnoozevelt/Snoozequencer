#ifndef SNOOZEQUENCERPATCH_HPP
#define SNOOZEQUENCERPATCH_HPP
#define USE_MIDI_CALLBACK
#include "MidiMessage.h"
#include "Patch.h"
#include "SineOscillator.h"
#include "VoltsPerOctave.h"
#include "AdsrEnvelope.h"


class SnoozequencerPatch : public Patch {
private:
    int blockSize;
    float sampleRate;
    SineOscillator* sin;
    SineOscillator* drum;
    AdsrEnvelope<true>* env;
    AdsrEnvelope<true>* drumEnv;
    FloatArray sinFloat;
    FloatArray drumFloat;
    FloatArray drumOutFloat;
    bool triggers[16];
    int pos;

public:
    SnoozequencerPatch() : sampleRate(getSampleRate()), blockSize(getBlockSize()), pos(0) {
        sinFloat = FloatArray::create(blockSize);
        drumFloat = FloatArray::create(blockSize);
        drumOutFloat = FloatArray::create(blockSize);

        sin = SineOscillator::create(sampleRate);
        sin->setFrequency(800);

        drum = SineOscillator::create(sampleRate);
        drum->setFrequency(500);

        drumEnv = AdsrEnvelope<true>::create(sampleRate);
        drumEnv->setAttack(.02);
        drumEnv->setDecay(.1);
        drumEnv->setSustain(0);

        env = AdsrEnvelope<true>::create(sampleRate);
        env->setAttack(.01);
        env->setDecay(.05);
        env->setSustain(0);
    }

    void buttonChanged(PatchButtonId bid, uint16_t value, uint16_t samples) override {
        if (bid == BUTTON_1 && value == ON) {
            pos = (pos + 1) % 16;
            if (pos % 4 == 0) {
                sin->setFrequency(1000);
            }
            else {
                sin->setFrequency(800);
            }

            if (pos == 0)
                sin->setFrequency(1200);
            
            env->trigger();

            if (triggers[pos])
                drumEnv->trigger();
        }
    }

    void processMidi(MidiMessage msg) override {
        if (msg.isNoteOn()) {
            triggers[pos] ^= true;
        }
    }

    void processAudio(AudioBuffer &buffer) override {
        FloatArray inOutLeft = buffer.getSamples(0);
        FloatArray inOutRight = buffer.getSamples(1);

        sin->generate(sinFloat);
        sinFloat.multiply(.3);

        drum->generate(drumFloat);
        drumEnv->process(drumFloat, drumOutFloat);

        env->process(sinFloat, inOutLeft);
        inOutLeft.add(drumOutFloat);
    }
};

#endif
