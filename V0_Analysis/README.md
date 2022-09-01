# V0 analysis for the FCT

This analysis is based on true MC information. To tag V0's, use the following macros in order

- MC_TrackFinder.cxx 

To execute this file, in Root do
```sh
.L XXXTrack.cxx+
.L FULLTrack.cxx+
```
Then do
```
.x MC_TrackFinder.cxx
```
Fro this macro you need to change the variables
```cpp
TString FCT_hits_file
TString FT3_hits_file
```
to be the paths to your o2sim_HitsXXX.root files (XXX being FCT, TRK or FT3)

This finds all the tracks in the FCT (XXXTrack, can also be used for the FT3) and all the "Full" tracks that pass through the FCT. Full tracks are those that have hits in the FCT, but thweir hits in other detectors (FT3 and TRK) also get stored in one track, the Full track.

This outputs the "MCTrack.root" file.

Now use the 
- V0_finder.cxx

To execute this, do (these are all custom classes and so root does not know of them and need to be compiled)
```sh
.L XXXTrack.cxx+
.L FULLTrack.cxx+
.L V0_mci.cxx+
```
Then do 
```sh
.x V0_finder.cxx
```
For this macro, you need to change the variable
```cp
TString input
```
to be the path to the MCTrack.root file that was outputted by MC_TrackFinder.cxx 
This finds all the V0's and stores important quantities (found in V0_mci.h) in a root file called V0sFCT.root

A V0 is defined as follows: When either of the pair produced particles from a photon is measured, this photon, together with its lepton+ and lepton- is tagged as a V0.
This means that V0's are saved that
- Have only one of their l+ or l- leave a hit in the FCT
- The leptons leave only one hit in the FCT (so only pass 1 layer of the FCT)
- Find their conversion point to be outside of the volume of the FCT

# V0_LocationPlot.cxx
This macro plots the location of all V0's found in the FCT. 

To use this macro, do
```sh
.L V0_mci.cxx+
.x V0_LocationPlot.cxx
```

You need to change the variables
```cpp
TString input
```
to be the path to the V0sFCT.root file outputted by the macro V0_finder.cxx
and
```cpp
c1->Print("YourOutputPath.pdf");
```
to be your output path

# V0_PaPlot.cxx
This macro produces a histogram of the pointing angles of all the V0's found inside of the FCT that have both of the lepton tracks measured by the FCT.

To use this macro, do
```sh
.L V0_mci.cxx+
.x PaPlot.cxx
```

You need to change the variables
```cpp
TString input
```
to be the path to the V0sFCT.root file outputted by the macro V0_finder.cxx
and
```cpp
c1->Print("YourOutputPath.pdf");
```
