# SuperColliderEssentiaUGens
Running Essentia algorithms in SuperCollider

# Installation
(Currently only tested on OSX 10.15.7 w/ SuperCollider 3.11)

Install [Essentia](https://github.com/MTG/essentia) with Homebrew.
~~~
brew install essentia --HEAD
~~~
I also needed to install Eigen (otherwise I had this error: “unsupported/Eigen/CXX11/Tensor: No such file or directory”)
~~~
brew install eigen
~~~
To compile the UGen you need to have a copy of the SC source on your computer, in the example below you can see my copy is in ~/Downloads/

Then clone this repository into your Extensions folder. 

Then create a directory called build (CMake wants this), go there, and install w/ CMake:
~~~
mkdir build && cd build
cmake -DESSENTIA_PATH=/usr/local/opt/essentia -DEIGEN_PATH=/usr/local/opt/eigen -DSC_PATH=~/Downloads/supercollider ..
make
~~~
# Disclaimer
The C++ code is quite old, experimental and messy. Hopefully I'll change this. However, this example still demonstrates the succesfull implementation of Essentia within SuperCollider
# Example
~~~
(
{
	var hfc, sin;
	sin = SinOsc.ar(LFNoise1.kr(1, 400, 1000), mul: 1);
	hfc = EssentiaHFC.kr(sin).poll;
	sin!2; // Only play the sine oscillator, show the HFC value with .poll
}.play;
)

// Or try this

(
{
    var sc, osc = WhiteNoise.ar(1) * LFPulse.kr(1);
    sc = EssentiaSpectralComplexity.kr(osc).poll;
    osc!2;
}.play;
)
~~~
