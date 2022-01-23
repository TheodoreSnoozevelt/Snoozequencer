//
// Created by theo on 1/23/22.
//

#ifndef SNOOZEQUENCER_HAT_HPP
#define SNOOZEQUENCER_HAT_HPP
#include "SineOscillator.h"
#include "SignalGenerator.h"
#include "AdsrEnvelope.h"
#include "ExponentialDecayEnvelope.h"
#include "NoiseOscillator.h"

class Hat : public SignalGenerator {
private:
    NoiseOscillator* noise;
    ExponentialDecayEnvelope* env;

public:
    Hat(float sr) {
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
        float n = noise->generate();
        return (n * .5) * ev;
    }
};
#endif //SNOOZEQUENCER_HAT_HPP
