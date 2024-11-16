// PluginEssentiaHFC.hpp
// Jildert Viet (jildert.viet@site.com)

#pragma once

#include "SC_PlugIn.hpp"
#include <essentia/algorithmfactory.h>
#include <memory>
#include <vector>

namespace EssentiaHFC {

class EssentiaHFC : public SCUnit {
public:
  EssentiaHFC();
  ~EssentiaHFC();

private:
  void next(int nSamples);
  void initializeEssentia();
  void linkAlgorithms();
  void computeEssentia();

  std::unique_ptr<essentia::standard::Algorithm> dcremoval;
  std::unique_ptr<essentia::standard::Algorithm> window;
  std::unique_ptr<essentia::standard::Algorithm> spectrum;
  std::unique_ptr<essentia::standard::Algorithm> hfc;

  std::vector<essentia::Real> audioBuffer;
  std::vector<essentia::Real> audioBuffer_dc;
  std::vector<essentia::Real> windowedframe;
  std::vector<essentia::Real> spec;

  essentia::Real hfcValue;
  int frameSize;
  int writePos;
  bool computed;
};

} // namespace EssentiaHFC
