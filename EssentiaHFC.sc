EssentiaHFC : UGen {
	*kr { arg in, type="Masri";
		var typeInt = ["Masri", "Jensen", "Brossier"].detectIndex({arg item, i; item==type});
		^this.multiNew('audio', in, typeInt)
    }
}