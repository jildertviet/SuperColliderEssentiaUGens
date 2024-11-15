#include "SC_PlugIn.h"
#include <iostream>
#include <math.h>

#include "essentia.h"
#include <essentiamath.h>

#include <algorithmfactory.h>
#include <fstream>
#include <pool.h>

using namespace essentia;
using namespace standard;
using namespace std;

static InterfaceTable *ft;

struct EssentiaHFC : public Unit {
  Algorithm *hfc;
  Algorithm *spectrum;
  Algorithm *window;
  Algorithm *dcremoval;

  vector<Real> *spec;
  vector<Real> *windowedframe;
  vector<Real> *audioBuffer_dc;
  vector<Real> *audioBuffer;
  Real hfcValue;
  int frameSize;
  int sampleRate;
  int zeropadding;
  string hfcType = "Masri";
  int writePos;
  bool bComputed;
};

extern "C" {
void EssentiaHFC_Ctor(EssentiaHFC *unit);
void processs(EssentiaHFC *unit, int inNumSamples);
void EssentiaHFC_Dtor(EssentiaHFC *unit);
void initEssentia(EssentiaHFC *unit);
void linkAlgorithms(EssentiaHFC *unit);
}

void EssentiaHFC_Ctor(EssentiaHFC *unit) {
  string types[3] = {"Masri", "Jensen", "Brossier"};
  cout << "0" << endl;
  unit->frameSize = IN0(2); // Default in Essentia is 2048 (in SC class as well)
  cout << "0" << endl;
  unit->hfcType = types[(int)IN0(1)]; // Causes crash!?
  cout << "0" << endl;
  unit->sampleRate = SAMPLERATE;
  cout << "0" << endl;
  unit->zeropadding = 0;

  unit->spec = new vector<Real>;
  cout << "0" << endl;
  unit->windowedframe = new vector<Real>;
  cout << "0" << endl;
  unit->audioBuffer_dc = new vector<Real>;
  cout << "0" << endl;
  unit->audioBuffer = new vector<Real>;
  for (int i = 0; i < unit->frameSize; i++) // I should look into RTAlloc?
    unit->audioBuffer->push_back(0);
  unit->writePos = 0;
  unit->bComputed = false;

  initEssentia(unit);

  SETCALC(processs);
  processs(unit, 1);
}

void initEssentia(EssentiaHFC *unit) {
  essentia::init();
  AlgorithmFactory &factory = AlgorithmFactory::instance();

  // Initiate algorithms
  unit->dcremoval = factory.create("DCRemoval", "sampleRate", unit->sampleRate);
  unit->window = factory.create("Windowing", "type", "hann", "zeroPadding",
                                unit->zeropadding);
  unit->spectrum = factory.create(
      "Spectrum", "size",
      unit->frameSize); // the expected size of the input audio signal (this is
                        // an optional parameter to optimize memory allocation)
  unit->hfc = factory.create("HFC", "sampleRate", unit->sampleRate, "type",
                             unit->hfcType);
  // Link all the algorithms
  linkAlgorithms(unit);
}

void linkAlgorithms(EssentiaHFC *unit) {
  unit->dcremoval->input("signal").set(*(unit->audioBuffer));
  unit->dcremoval->output("signal").set(*(unit->audioBuffer_dc));

  unit->window->input("frame").set(*(unit->audioBuffer_dc));
  unit->window->output("frame").set(*(unit->windowedframe));

  unit->spectrum->input("frame").set(*(unit->windowedframe));
  unit->spectrum->output("spectrum").set(*(unit->spec));

  unit->hfc->input("spectrum").set(*(unit->spec));
  unit->hfc->output("hfc").set(unit->hfcValue);
}

void processs(EssentiaHFC *unit, int inNumSamples) {
  if (inNumSamples == 1) {
    OUT(0)[0] = 0;
    return;
  }

  bool bCalculate = false;
  for (int i = 0; i < inNumSamples; i++) { // Read the incoming signal
    Real value = (Real)IN(0)[i];
    unit->audioBuffer->at(unit->writePos) = value;
    unit->writePos++;
    if (unit->writePos >= unit->frameSize) {
      unit->writePos = 0;
      bCalculate = true;
    }
  }

  if (bCalculate) {
    unit->dcremoval->compute();
    unit->window->compute();
    unit->spectrum->compute();
    unit->hfc->compute();
    unit->bComputed = true;
  }

  for (int i = 0; i < inNumSamples; i++) {
    if (unit->bComputed) {
      OUT(0)[i] = unit->hfcValue;
    } else {
      OUT(0)[i] = 0;
    }
  }
}

void EssentiaHFC_Dtor(EssentiaHFC *unit) {
  unit->audioBuffer->clear();
  unit->audioBuffer_dc->clear();
  unit->windowedframe->clear();
  unit->spec->clear();
}

PluginLoad(EssentiaHFC) {
  ft = inTable;
  DefineDtorUnit(EssentiaHFC);
}
