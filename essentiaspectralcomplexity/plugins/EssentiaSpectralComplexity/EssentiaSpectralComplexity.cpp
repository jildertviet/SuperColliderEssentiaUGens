// PluginEssentiaSpectralComplexity.cpp
// Jildert Viet (jildert.viet@site.com)

#include "EssentiaSpectralComplexity.hpp"
#include <essentia/algorithmfactory.h>

static InterfaceTable *ft;

namespace EssentiaSpectralComplexity {

EssentiaSpectralComplexity::EssentiaSpectralComplexity()
    : frameSize(in0(2)), // Frame size input
      writePos(0), computed(false) {

  std::cout << frameSize << std::endl;
  essentia::init();
  std::cout << "X" << std::endl;
  audioBuffer.resize(frameSize, 0.0f);
  audioBuffer_dc.resize(frameSize, 0.0f);
  windowedframe.resize(frameSize, 0.0f);

  spec.resize(frameSize / 2 + 1, 0.0f);

  initializeEssentia();
  linkAlgorithms();
  input = in(0);
  output = out(0);

  mCalcFunc = make_calc_function<EssentiaSpectralComplexity,
                                 &EssentiaSpectralComplexity::next>();
  next(1);
}

EssentiaSpectralComplexity::~EssentiaSpectralComplexity() {
  essentia::shutdown();
}

void EssentiaSpectralComplexity::initializeEssentia() {
  auto &factory = essentia::standard::AlgorithmFactory::instance();

  dcremoval = std::unique_ptr<essentia::standard::Algorithm>(
      factory.create("DCRemoval", "sampleRate", sampleRate()));
  window = std::unique_ptr<essentia::standard::Algorithm>(
      factory.create("Windowing", "type", "hann"));
  spectrum = std::unique_ptr<essentia::standard::Algorithm>(
      factory.create("Spectrum"));
  spectralComplexity =
      std::unique_ptr<essentia::standard::Algorithm>(factory.create(
          "SpectralComplexity", "sampleRate", sampleRate(),
          "magnitudeThreshold",
          in0(1))); // the minimum spectral-peak magnitude that contributes
                    // to spectral complexity", "[0,inf)", 0.005);
}

void EssentiaSpectralComplexity::linkAlgorithms() {
  dcremoval->input("signal").set(audioBuffer);
  dcremoval->output("signal").set(audioBuffer_dc);

  window->input("frame").set(audioBuffer_dc);
  window->output("frame").set(windowedframe);

  spectrum->input("frame").set(windowedframe);
  spectrum->output("spectrum").set(spec);

  spectralComplexity->input("spectrum").set(spec);
  spectralComplexity->output("spectralComplexity").set(spectralComplexityValue);
}

void EssentiaSpectralComplexity::computeEssentia() {
  dcremoval->compute();
  window->compute();
  spectrum->compute();
  spectralComplexity->compute();
  computed = true;
}

void EssentiaSpectralComplexity::next(int nSamples) {
  for (int i = 0; i < nSamples; ++i) {
    audioBuffer[writePos++] = input[i];

    if (writePos >= frameSize) {
      writePos = 0;
      computeEssentia();
    }
    output[i] = computed ? spectralComplexityValue : 0.0f;
  }
}

} // namespace EssentiaSpectralComplexity

PluginLoad(EssentiaSpectralComplexityUGens) {
  // Plugin magic
  ft = inTable;
  registerUnit<EssentiaSpectralComplexity::EssentiaSpectralComplexity>(
      ft, "EssentiaSpectralComplexity", false);
}
