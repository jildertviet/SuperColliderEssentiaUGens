// PluginEssentiaSpectralComplexity.hpp
// Jildert Viet (jildert.viet@site.com)

#pragma once

#include "SC_PlugIn.hpp"

namespace EssentiaSpectralComplexity {

class EssentiaSpectralComplexity : public SCUnit {
public:
    EssentiaSpectralComplexity();

    // Destructor
    // ~EssentiaSpectralComplexity();

private:
    // Calc function
    void next(int nSamples);

    // Member variables
};

} // namespace EssentiaSpectralComplexity
