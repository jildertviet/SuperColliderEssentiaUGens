// PluginEssentiaOnsetDetector.hpp
// Jildert Viet (jildert.viet@site.com)

#pragma once

#include "SC_PlugIn.hpp"
#include <essentia/algorithmfactory.h>
#include <memory>
#include <vector>

namespace EssentiaOnsetDetector {

class EssentiaOnsetDetector : public SCUnit {
public:
  EssentiaOnsetDetector();
  ~EssentiaOnsetDetector();

private:
  // Calc function
  void next(int nSamples);
  void initializeEssentia();
  void linkAlgorithms();
  void computeEssentia();

  // std::unique_ptr<essentia::standard::Algorithm> dcremoval;
  std::unique_ptr<essentia::standard::Algorithm> window;
  // std::unique_ptr<essentia::standard::Algorithm> spectrum;
  std::unique_ptr<essentia::standard::Algorithm> fft;
  std::unique_ptr<essentia::standard::Algorithm> cartesianToPolar;

  std::unique_ptr<essentia::standard::Algorithm> onsetDetection;

  std::vector<essentia::Real> audioBuffer;
  // std::vector<essentia::Real> audioBuffer_dc;
  std::vector<essentia::Real> windowedframe;
  std::vector<std::complex<essentia::Real>> fftComplex;
  std::vector<essentia::Real> magnitude;
  std::vector<essentia::Real> phase;
  essentia::Real onset;
  // std::vector<essentia::Real> spec;

  int frameSize;
  int writePos;

  const float *input = in(0);
  float *output = out(0);

  // Member variables
};

} // namespace EssentiaOnsetDetector
