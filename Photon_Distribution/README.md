# What photons reach the FCT?

The macro to_reach_FCT.cpp plots the photon distriubtion of each production process to reach the first layer of the FCT as function of the transverse momentum of the FCT. 
An energy filter can be toggled such that photons with an energy of 10 MeV or less will not be considered (these photons are very unlikely to pair produce e+e- anyway).

This program checks for all of the particles to reach the FCT whether it is a photon or not. This is a bit tricky, since the only particles that get measured by the FCT are charged particles of which the hits are stored in the o2sim_HitsFCT.root file.
You also have access to the o2sim_Kine.root file which tells you the path of all the particles in the whole setup, but this does not tell you which particles have actually reached the FCT.
I came up with a workaround for this such that I do not have to set up a complicated filter for the o2sim_Kine.root file and that is to let the FCT measure all particles.
This is how it is done:

In the file FCTBaseParam.h, found [here](https://github.com/AliceO2Group/AliceO2/blob/dev/Detectors/Upgrades/ALICE3/FCT/base/include/FCTBase/FCTBaseParam.h), add the variable 
```cpp
Bool_t OnlyChargedParticles = true; // When true, only charged partciles are saved. When false, all particles are saved.
```

In the file Detector.cxx, found [here](https://github.com/AliceO2Group/AliceO2/blob/dev/Detectors/Upgrades/ALICE3/FCT/simulation/src/Detector.cxx), replace the following line in the funciton void Detector::ProcessHits(FairVolume* vol)
```cpp
  // This method is called from the MC stepping
  if(mOnlyChargedParticles){
    return kFALSE;
  }
```
With
```cpp
  // This method is called from the MC stepping
  if(mOnlyChargedParticles && !(fMC->TrackCharge())){
    return kFALSE;
  }
```

and add the following line to the function void Detector::ConstructGeometry()

```cpp
mOnlyChargedParticles = fctBaseParam.OnlyChargedParticles;
```
just before the lines
```cpp
  exportLayout();

  // Create detector materials
  createMaterials();

  // Construct the detector geometry
  createGeometry();
```
In the file Detector.h, found [here](https://github.com/AliceO2Group/AliceO2/blob/dev/Detectors/Upgrades/ALICE3/FCT/simulation/include/FCTSimulation/Detector.h) add the variable
```cpp
Bool_t mOnlyChargedParticles;
```
To the list of variables
```cpp
private:
  std::vector<Int_t> mLayerID;
  std::vector<Int_t> mConverterLayerId;
  std::vector<TString> mLayerName;
  std::vector<TString> mConverterLayerName;
  Int_t mNumberOfLayers;
  Int_t mNumberOfConverterLayers;
  Bool_t mOnlyChargedParticles;
```
Rebuild your O2 with
```sh
ENABLE_UPGRADES=ON aliBuild build O2 --defaults o2
```

Doing all these steps allows you to run the FCT simulation such that it will detect all particles instead of just charged particles, like so
```sh
o2-sim -j 1 -m FCT FT3 TRK -e TGeant4 -g pythia8pp -n 5000 --configKeyValues "FCTBase.OnlyChargedParticles=false"
```
This way, the o2sim_HitsFCT.root effectively functions as a filter for all the particles to reach the FCT.

# How to use the to_reach_FCT.cpp program
Provide the input path to the o2sim_HitsFCT file for the variable
```cpp
TString input_path_HitsFCT = <YourPath>
```
The input path to the o2sim_Kine.root file for the variable
```cpp
TString input_path_kine = <YourPath2>
```
The output path for the .pdf file at the bottom of the program
```cpp
c1->Print("MyOutputPath.pdf");
```
