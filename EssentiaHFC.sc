EssentiaHFC : UGen {
	*kr { arg in, type="Masri", frameSize=2048;
		var typeInt = ["Masri", "Jensen", "Brossier"].detectIndex({arg item, i; item==type});
		^this.multiNew('audio', in, typeInt, frameSize)
    }
}