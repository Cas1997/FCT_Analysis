#How to plot the material budget in front of the FCT?

Enter alienv with (if that is your branch)
```sh
alienv enter O2/latest-dev-o2
```
Run O2 with
```
o2-sim-evalmat -m FT3 TRK A3IP --configKeyValues 'matm.nphi=360;matm.phimin=0.;matm.phimax=360.;matm.neta=100;matm.etamin=-5.;matm.etamax=-3;matm.rmax=100.;matm.zmax=450'
```

This will send geantinos, non-interacting particles, through the detector setup. Geantinos do collect information of the material they pass through, but they never interact.
The aprameters of this command are:

- matm.nphi
- matm.phimin
- matm.phimax
In the example this will cover the phi range from min (0) to max (360) with 360 bins

- matm.neta
- matm.etamin
- matm.etamax
In the example this will cover the pseudorapidity range from -5 to -3 with 360 bins

- matm.rmax
- matm.zmax
In the example this will cover the radius up to 100 cm and the z up to 450 cm

The program 
