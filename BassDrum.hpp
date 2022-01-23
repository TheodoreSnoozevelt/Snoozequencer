//
// Created by theo on 1/23/22.
//

#ifndef SNOOZEQUENCER_BASSDRUM_HPP
#define SNOOZEQUENCER_BASSDRUM_HPP
#include "SineOscillator.h"
#include "ExponentialDecayEnvelope.h"

class BassDrum : public SignalGenerator {
private:
    SineOscillator* sine;
    ExponentialDecayEnvelope* env;

public:
    BassDrum(float sr) {
        sine = SineOscillator::create(sr);
        env = ExponentialDecayEnvelope::create(sr);
        env->setDecay(.1);
    }

    void trigger() {
        env->trigger();
    }

    using SignalGenerator::generate;
    float generate() {
        float ev = env->generate();
        sine->setFrequency(300 * ev);
        return sine->generate(0) * ev;
    }
};

#endif //SNOOZEQUENCER_BASSDRUM_HPP
