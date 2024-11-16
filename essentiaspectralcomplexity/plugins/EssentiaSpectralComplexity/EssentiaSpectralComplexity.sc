EssentiaSpectralComplexity : UGen {
  *kr { arg in, magnitudeThreshold=0.005, frameSize=2048;
		^this.multiNew('audio', in, magnitudeThreshold, frameSize)
  }

	checkInputs {
		/* TODO */
		^this.checkValidInputs;
	}
}
