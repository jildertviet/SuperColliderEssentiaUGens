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

struct EssentiaHFC : public Unit {
    float value;
    Algorithm* hfc;
    Algorithm* spectrum;
    Algorithm* window;
    Algorithm* dcremoval;
    
    vector<Real>* spec;
    vector<Real>* windowedframe;
    vector<Real>* audioBuffer_dc;
    vector<Real>* audioBuffer;
    Real hfcValue;
    int frameSize;
    int sampleRate;
    int zeropadding;
    int bufferSize;
};

extern "C"{
    void EssentiaHFC_Ctor(EssentiaHFC* unit);
    void processs(EssentiaHFC *unit, int inNumSamples);
    void EssentiaHFC_Dtor(EssentiaHFC* unit);
}

void EssentiaHFC_Ctor(EssentiaHFC *unit){
    unit->frameSize = 1024;
    unit->sampleRate = 441000;
    unit->zeropadding = 0;
    
    unit->spec = new vector<Real>;
    unit->windowedframe = new vector<Real>;
    unit->audioBuffer_dc = new vector<Real>;
    unit->audioBuffer = new vector<Real>;

//    int firstArg = IN0(1);
    
    essentia::init();
    AlgorithmFactory& factory = AlgorithmFactory::instance();

    // Initiate algorithms
    unit->dcremoval = factory.create("DCRemoval", "sampleRate", unit->sampleRate);
    unit->window = factory.create("Windowing",
                            "type", "hann",
                            "zeroPadding", unit->zeropadding);
    unit->spectrum = factory.create("Spectrum",
                              "size", unit->frameSize);
    unit->hfc = factory.create("HFC", "sampleRate", unit->sampleRate);

    // Link
    unit->dcremoval->input("signal").set(*(unit->audioBuffer));
    unit->dcremoval->output("signal").set(*(unit->audioBuffer_dc));
    
//    cout << "FrameSize: " << (*(unit->audioBuffer_dc)).size() << endl;
    unit->window->input("frame").set(*(unit->audioBuffer_dc));
    unit->window->output("frame").set(*(unit->windowedframe));
    
    unit->spectrum->input("frame").set(*(unit->windowedframe));
    unit->spectrum->output("spectrum").set(*(unit->spec));
    
    unit->hfc->input("spectrum").set(*(unit->spec));
    unit->hfc->output("hfc").set(unit->hfcValue);
    
    cout << "EssentiaHFC object made" << endl;
    SETCALC(processs);
    processs(unit, 1);
}

void processs(EssentiaHFC *unit, int inNumSamples){
    unit->audioBuffer->clear();
    
    for (int i=0; i<inNumSamples;i++){ // Read the incoming signal
        Real value = (Real) IN(0)[i];
        unit->audioBuffer->push_back(value);
    }

    unit->dcremoval->compute();
//    cout << (*(unit->audioBuffer))[0] << endl;
//    cout << (*(unit->audioBuffer_dc))[0] << endl;
    if(inNumSamples>1){
        unit->window->compute();
        unit->spectrum->compute();
        unit->hfc->compute();
//        cout << unit->hfcValue << endl;
    }
    
//    unit->hfcValue
    
    for(int i=0; i<inNumSamples; i++){
        OUT(0)[i] = unit->hfcValue;
    }
}

void EssentiaHFC_Dtor(EssentiaHFC* unit){
    unit->audioBuffer->clear();
    unit->audioBuffer_dc->clear();
    unit->windowedframe->clear();
    unit->spec->clear();
//    RTFree(unit->mWorld, unit->historyBuffer);
}

// SuperCollider wants this function to load the plug-in
PluginLoad(EssentiaHFC){
    ft = inTable;
    DefineDtorUnit(EssentiaHFC);
}
