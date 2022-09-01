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
This finds all the V0's and stores iomportant quantities (found in V0_mci.h) in a root file
