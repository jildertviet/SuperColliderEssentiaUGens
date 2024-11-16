// PluginEssentiaHFC.hpp
// Jildert Viet (jildert.viet@site.com)

#pragma once

#include "SC_PlugIn.hpp"
#include <essentia/algorithmfactory.h>
#include <memory>
#include <vector>

namespace EssentiaSpectralComplexity {

class EssentiaSpectralComplexity : public SCUnit {
public:
  EssentiaSpectralComplexity();
  ~EssentiaSpectralComplexity();

private:
  // Calc function
  void next(int nSamples);
  void initializeEssentia();
  void linkAlgorithms();
  void computeEssentia();

  std::unique_ptr<essentia::standard::Algorithm> dcremoval;
  std::unique_ptr<essentia::standard::Algorithm> window;
  std::unique_ptr<essentia::standard::Algorithm> spectrum;
  std::unique_ptr<essentia::standard::Algorithm> spectralComplexity;

  std::vector<essentia::Real> audioBuffer;
  std::vector<essentia::Real> audioBuffer_dc;
  std::vector<essentia::Real> windowedframe;
  std::vector<essentia::Real> spec;

  essentia::Real spectralComplexityValue;
  int frameSize;
  int writePos;
  bool computed;
  const float *input = in(0);
  float *output = out(0);
  int zeropadding;

  // Member variables
};

} // namespace EssentiaSpectralComplexity
