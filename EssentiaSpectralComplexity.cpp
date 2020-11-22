#include "SC_PlugIn.h"
#include <iostream>
#include <math.h>

#include "essentia.h"
#include <essentiamath.h>

#include <fstream>
#include <algorithmfactory.h>
#include <pool.h>

using namespace essentia;
using namespace standard;
using namespace std;

static InterfaceTable *ft;

struct EssentiaSpectralComplexity : public Unit {
    Algorithm* spectrum;
    Algorithm* window;
    Algorithm* dcremoval;
    Algorithm* SpectralComplexity;
    
    vector<Real>* spec;
    vector<Real>* windowedframe;
    vector<Real>* audioBuffer_dc;
    vector<Real>* audioBuffer;
    
    Real spectralComplexityValue;
    int frameSize;
    int zeropadding;
    int bufferSize;
    int writePos;
    bool bComputed;
};

extern "C"{
    void EssentiaSpectralComplexity_Ctor(EssentiaSpectralComplexity* unit);
    void processs(EssentiaSpectralComplexity *unit, int inNumSamples);
    void EssentiaSpectralComplexity_Dtor(EssentiaSpectralComplexity* unit);
    void initEssentia(EssentiaSpectralComplexity* unit);
    void linkAlgorithms(EssentiaSpectralComplexity* unit);
}

void EssentiaSpectralComplexity_Ctor(EssentiaSpectralComplexity *unit){
    unit->frameSize = 1024;
    unit->zeropadding = 0;
    unit->writePos = 0;
    
    unit->spec = new vector<Real>;
    unit->windowedframe = new vector<Real>;
    unit->audioBuffer_dc = new vector<Real>;
    unit->audioBuffer = new vector<Real>(unit->frameSize, 0);
    
    initEssentia(unit);

    SETCALC(processs);
//    processs(unit, 1);
}

void initEssentia(EssentiaSpectralComplexity* unit){
    essentia::init();
    AlgorithmFactory& factory = AlgorithmFactory::instance();

    // Initiate algorithms
    unit->dcremoval = factory.create("DCRemoval", "sampleRate", SAMPLERATE);
    unit->window = factory.create("Windowing",
                            "type", "hann",
                            "zeroPadding", 0);
    unit->spectrum = factory.create("Spectrum",
                              "size", unit->frameSize);
    unit->SpectralComplexity = factory.create("SpectralComplexity", "sampleRate", SAMPLERATE);
    
    // Link all the algorithms
    linkAlgorithms(unit);
}

void linkAlgorithms(EssentiaSpectralComplexity* unit){
    // Link
    unit->dcremoval->input("signal").set(*(unit->audioBuffer));
    unit->dcremoval->output("signal").set(*(unit->audioBuffer_dc));
    
    unit->window->input("frame").set(*(unit->audioBuffer_dc));
    unit->window->output("frame").set(*(unit->windowedframe));
    
    unit->spectrum->input("frame").set(*(unit->windowedframe));
    unit->spectrum->output("spectrum").set(*(unit->spec));
    
    unit->SpectralComplexity->input("spectrum").set(*(unit->spec));
    unit->SpectralComplexity->output("spectralComplexity").set(unit->spectralComplexityValue);
}

void processs(EssentiaSpectralComplexity *unit, int inNumSamples){
    bool bCalculate = false;
    for (int i=0; i<inNumSamples; i++){ // Read the incoming signal
        Real value = (Real) IN(0)[i];
        unit->audioBuffer->at(unit->writePos) = value;
        unit->writePos++;
        if(unit->writePos >= unit->frameSize){
            unit->writePos = 0;
            bCalculate = true;
        }
    }
    
    
    if(bCalculate){
        unit->dcremoval->compute();
        unit->window->compute();
        unit->spectrum->compute();
        unit->SpectralComplexity->compute();
        unit->bComputed = true;
    }
    
    for(int i=0; i<inNumSamples; i++){
        if(unit->bComputed){
            OUT(0)[i] = unit->spectralComplexityValue;
        } else{
            OUT(0)[i] = 0;
        }
    }
}

void EssentiaSpectralComplexity_Dtor(EssentiaSpectralComplexity* unit){
    unit->audioBuffer->clear();
    unit->audioBuffer_dc->clear();
    unit->windowedframe->clear();
    unit->spec->clear();
//    RTFree(unit->mWorld, unit->historyBuffer);
}

// SuperCollider wants this function to load the plug-in
PluginLoad(EssentiaSpectralComplexity){
    ft = inTable;
    DefineDtorUnit(EssentiaSpectralComplexity);
}
