# ALICE 3 Forward Tracker

### 1. Simulation

`$ o2-sim -m FCT -e TGeant3 -g pythia8 -n 1000`

Output: `o2sim_HitsFCT.root`

### 2. Tracking
`$ root.exe -q fctTracker.C+`

Output: `fcttracks.root`

### 3. Assessment histograms
#### Layer hit densities

`$ root.exe -b -q fctOccupancy.C+`

Output: `fctOccupancy.root`

#### Tracking evalutaion
`$ root.exe -q -b FCTTrackerChecker.C+`

Output: `Fittercheck_fcttracks.root`

# This is the adaptation of the [FT3 Tracker](https://github.com/rpezzi/ft3tracker) by Raffael Pezzi. Only names are changed such that it works for the FCT.

Caution, this tracker just serves as an example. The FCT will get its magnetic field from a dipole magnet, whereas the FT3 gets its from a solenoid. Tracking is different for the detectors, but this nonetheless shows you a little of what can be done and how to access inforamtion of the TTrees produced by o2sim.
