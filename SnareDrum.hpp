//
// Created by theo on 1/23/22.
//

#ifndef SNOOZEQUENCER_SNAREDRUM_HPP
#define SNOOZEQUENCER_SNAREDRUM_HPP
#include "SineOscillator.h"
#include "SignalGenerator.h"
#include "AdsrEnvelope.h"
#include "ExponentialDecayEnvelope.h"
#include "NoiseOscillator.h"

class SnareDrum : public SignalGenerator {
private:
    SineOscillator* sine;
    NoiseOscillator* noise;
    ExponentialDecayEnvelope* env;

public:
    SnareDrum(float sr) {
        sine = SineOscillator::create(sr);
        noise = NoiseOscillator::create(sr);
        env = ExponentialDecayEnvelope::create(sr);
        env->setDecay(.1);
    }

    void trigger() {
        env->trigger();
    }

    using SignalGenerator::generate;
    float generate() {
        float ev = env->generate();
        sine->setFrequency(800 * ev);
        float n = noise->generate();
        return (sine->generate(0) * .5 + n * .5) * ev;
    }
};
#endif //SNOOZEQUENCER_SNAREDRUM_HPP
