EssentiaSpectralComplexity : UGen {
    *kr { arg in;
		^this.multiNew('audio', in)
    }
}