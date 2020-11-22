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
    int sampleRate;
    int zeropadding;
    int bufferSize;
    int writePos;
};

extern "C"{
    void EssentiaSpectralComplexity_Ctor(EssentiaSpectralComplexity* unit);
    void processs(EssentiaSpectralComplexity *unit, int inNumSamples);
    void EssentiaSpectralComplexity_Dtor(EssentiaSpectralComplexity* unit);
}

void EssentiaSpectralComplexity_Ctor(EssentiaSpectralComplexity *unit)
{
    unit->frameSize = 1024;
    unit->sampleRate = 44100;
    unit->zeropadding = 0;
    unit->writePos = 0;
    
    unit->spec = new vector<Real>;
    unit->windowedframe = new vector<Real>;
    unit->audioBuffer_dc = new vector<Real>;
    unit->audioBuffer = new vector<Real>(unit->frameSize, 0);
//    cout << unit->audioBuffer->size() << endl;

    
    essentia::init();
    AlgorithmFactory& factory = AlgorithmFactory::instance();

    // Initiate algorithms
    unit->dcremoval = factory.create("DCRemoval", "sampleRate", 44100);
    unit->window = factory.create("Windowing",
                            "type", "hann",
                            "zeroPadding", 0);
    unit->spectrum = factory.create("Spectrum",
                              "size", unit->frameSize);
    unit->SpectralComplexity = factory.create("SpectralComplexity", "sampleRate", 44100);

    // Link
    unit->dcremoval->input("signal").set(*(unit->audioBuffer));
    unit->dcremoval->output("signal").set(*(unit->audioBuffer_dc));
    
    unit->window->input("frame").set(*(unit->audioBuffer_dc));
    unit->window->output("frame").set(*(unit->windowedframe));
    
    unit->spectrum->input("frame").set(*(unit->windowedframe));
    unit->spectrum->output("spectrum").set(*(unit->spec));
    
    unit->SpectralComplexity->input("spectrum").set(*(unit->spec));
    unit->SpectralComplexity->output("spectralComplexity").set(unit->spectralComplexityValue);
    
    cout << "JildertSpectralComplexity object made" << endl;
    SETCALC(processs);
//    processs(unit, 1);
}




void processs(EssentiaSpectralComplexity *unit, int inNumSamples){
    if(unit->writePos >= unit->frameSize){
        unit->writePos = 0;
//        unit->audioBuffer->clear();
    }
//    cout << unit->writePos << endl;
    
    for (int i=0; i<inNumSamples;i++){
        Real value = (Real) IN(0)[i];
        unit->audioBuffer->at((unit->writePos)+i) = value;
    }
    
    unit->dcremoval->compute();
    
    if(inNumSamples>1){
        unit->window->compute();
        unit->spectrum->compute();
        unit->SpectralComplexity->compute();
    }
    
//    cout << unit->spectralComplexityValue << endl;
    
    for(int i=0; i<inNumSamples; i++){
        OUT(0)[i] = unit->spectralComplexityValue;
    }
    
    unit->writePos+=inNumSamples;
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
