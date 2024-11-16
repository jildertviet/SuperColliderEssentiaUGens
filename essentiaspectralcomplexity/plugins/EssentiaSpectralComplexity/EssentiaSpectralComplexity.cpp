// PluginEssentiaSpectralComplexity.cpp
// Jildert Viet (jildert.viet@site.com)

#include "SC_PlugIn.hpp"
#include "EssentiaSpectralComplexity.hpp"

static InterfaceTable* ft;

namespace EssentiaSpectralComplexity {

EssentiaSpectralComplexity::EssentiaSpectralComplexity() {
    mCalcFunc = make_calc_function<EssentiaSpectralComplexity, &EssentiaSpectralComplexity::next>();
    next(1);
}

void EssentiaSpectralComplexity::next(int nSamples) {

    // Audio rate input
    const float* input = in(0);

    // Control rate parameter: gain.
    const float gain = in0(1);

    // Output buffer
    float* outbuf = out(0);

    // simple gain function
    for (int i = 0; i < nSamples; ++i) {
        outbuf[i] = input[i] * gain;
    }
}

} // namespace EssentiaSpectralComplexity

PluginLoad(EssentiaSpectralComplexityUGens) {
    // Plugin magic
    ft = inTable;
    registerUnit<EssentiaSpectralComplexity::EssentiaSpectralComplexity>(ft, "EssentiaSpectralComplexity", false);
}
