// PluginEssentiaHFC.cpp
// Jildert Viet (jildert.viet@site.com)

#include "EssentiaHFC.hpp"
#include <essentia/algorithmfactory.h>
#include <essentia/essentia.h>

static InterfaceTable *ft;

namespace EssentiaHFC {

EssentiaHFC::EssentiaHFC()
    : frameSize(in0(2)), // Frame size input
      writePos(0), computed(false) {

  std::cout << frameSize << std::endl;
  essentia::init();
  essentia::warningLevelActive = false; // deactivate warnings
  std::cout << "X" << std::endl;
  audioBuffer.resize(frameSize, 0.0f);
  audioBuffer_dc.resize(frameSize, 0.0f);
  windowedframe.resize(frameSize, 0.0f);

  spec.resize(frameSize / 2 + 1, 0.0f);

  initializeEssentia();
  linkAlgorithms();
  input = in(0);
  output = out(0);

  mCalcFunc = make_calc_function<EssentiaHFC, &EssentiaHFC::next>();
  next(1);
}

EssentiaHFC::~EssentiaHFC() { essentia::shutdown(); }

void EssentiaHFC::initializeEssentia() {
  auto &factory = essentia::standard::AlgorithmFactory::instance();

  dcremoval = std::unique_ptr<essentia::standard::Algorithm>(
      factory.create("DCRemoval", "sampleRate", sampleRate()));
  window = std::unique_ptr<essentia::standard::Algorithm>(
      factory.create("Windowing", "type", "hann"));
  spectrum = std::unique_ptr<essentia::standard::Algorithm>(
      factory.create("Spectrum"));
  hfc = std::unique_ptr<essentia::standard::Algorithm>(
      factory.create("HFC", "sampleRate", sampleRate(), "type", "Masri"));
}

void EssentiaHFC::linkAlgorithms() {
  dcremoval->input("signal").set(audioBuffer);
  dcremoval->output("signal").set(audioBuffer_dc);

  window->input("frame").set(audioBuffer_dc);
  window->output("frame").set(windowedframe);

  spectrum->input("frame").set(windowedframe);
  spectrum->output("spectrum").set(spec);

  hfc->input("spectrum").set(spec);
  hfc->output("hfc").set(hfcValue);
}

void EssentiaHFC::computeEssentia() {
  dcremoval->compute();
  window->compute();
  spectrum->compute();
  hfc->compute();
  computed = true;
}

void EssentiaHFC::next(int nSamples) {

  for (int i = 0; i < nSamples; ++i) {
    audioBuffer[writePos++] = input[i];

    if (writePos >= frameSize) {
      writePos = 0;
      computeEssentia();
    }
    output[i] = computed ? hfcValue : 0.0f;
  }
}

} // namespace EssentiaHFC

PluginLoad(EssentiaHFCUGens) {
  ft = inTable;
  registerUnit<EssentiaHFC::EssentiaHFC>(ft, "EssentiaHFC");
}
