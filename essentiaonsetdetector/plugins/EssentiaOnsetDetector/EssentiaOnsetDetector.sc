EssentiaOnsetDetector : UGen {
	*ar { arg in, frameSize = 1024;
		^this.multiNew('audio', in, frameSize, Server.local.sampleRate)
  }


	checkInputs {
		/* TODO */
		^this.checkValidInputs;
	}
}
