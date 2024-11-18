// PluginEssentiaOnsetDetector.cpp
// Jildert Viet (jildert.viet@site.com)

#include "EssentiaOnsetDetector.hpp"
#include "SC_PlugIn.hpp"

static InterfaceTable *ft;

namespace EssentiaOnsetDetector {

EssentiaOnsetDetector::EssentiaOnsetDetector()
    : frameSize(in0(1)), // Frame size input
      writePos(0) {
  essentia::init();
  essentia::warningLevelActive = false; // deactivate warnings
  audioBuffer.resize(frameSize, 0.0f);
  // audioBuffer_dc.resize(frameSize, 0.0f);
  windowedframe.resize(frameSize, 0.0f);

  initializeEssentia();
  linkAlgorithms();
  input = in(0);
  output = out(0);

  mCalcFunc =
      make_calc_function<EssentiaOnsetDetector, &EssentiaOnsetDetector::next>();
  next(1);
}

EssentiaOnsetDetector::~EssentiaOnsetDetector() { essentia::shutdown(); }

void EssentiaOnsetDetector::initializeEssentia() {
  auto &factory = essentia::standard::AlgorithmFactory::instance();

  // dcremoval = std::unique_ptr<essentia::standard::Algorithm>(
  // factory.create("DCRemoval", "sampleRate", sampleRate()));
  window = std::unique_ptr<essentia::standard::Algorithm>(
      factory.create("Windowing", "type", "hann"));
  fft = std::unique_ptr<essentia::standard::Algorithm>(factory.create("FFT"));
  cartesianToPolar = std::unique_ptr<essentia::standard::Algorithm>(
      factory.create("CartesianToPolar"));
  onsetDetection = std::unique_ptr<essentia::standard::Algorithm>(
      factory.create("OnsetDetection", "method", "complex"));
  // spectrum = std::unique_ptr<essentia::standard::Algorithm>(
  // factory.create("Spectrum"));
}
void EssentiaOnsetDetector::linkAlgorithms() {
  // dcremoval->input("signal").set(audioBuffer);
  // dcremoval->output("signal").set(audioBuffer_dc);

  window->input("frame").set(audioBuffer);
  window->output("frame").set(windowedframe);

  fft->input("frame").set(windowedframe);
  fft->output("fft").set(fftComplex);

  cartesianToPolar->input("complex").set(fftComplex);
  cartesianToPolar->output("magnitude").set(magnitude);
  cartesianToPolar->output("phase").set(phase);

  onsetDetection->input("spectrum").set(magnitude);
  onsetDetection->input("phase").set(phase);
  onsetDetection->output("onsetDetection").set(onset);

  // spectrum->input("frame").set(windowedframe);
  // spectrum->output("spectrum").set(spec);
}
void EssentiaOnsetDetector::computeEssentia() {
  // dcremoval->compute();
  window->compute();
  fft->compute();
  cartesianToPolar->compute();
  onsetDetection->compute();
}

void EssentiaOnsetDetector::next(int nSamples) {
  for (int i = 0; i < nSamples; ++i) {
    audioBuffer[writePos++] = input[i];

    if (writePos >= frameSize) {
      writePos = 0;
      computeEssentia();
    }
    output[i] = onset;
  }
}

} // namespace EssentiaOnsetDetector

PluginLoad(EssentiaOnsetDetectorUGens) {
  // Plugin magic
  ft = inTable;
  registerUnit<EssentiaOnsetDetector::EssentiaOnsetDetector>(
      ft, "EssentiaOnsetDetector", false);
}
