# Vacuum Cave in ALICE O2

This file is an adaptation of the cave of O2 found at 
https://github.com/AliceO2Group/AliceO2/blob/dev/Detectors/Passive/src/Cave.cxx

The 5 adaptations can be found with the tag "Cas add-on"
These adaptations "fill" the cave with a vacuum isntead of with air. 


This only works if no overlapping detector-volumes are palced inside of the cave that have air too
Some detector have a "top" volume which gets filled with air. This top volume (which contains the detector) will then get placed inside of the cave volume.
It is up to the user to know what they are doing before they attempt to change the cave volume.
