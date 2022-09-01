# How to merge TTrees?

The program merge_TTrees.cpp merges TTrees by reading them, putting them in a TList and merging them.
Provide the base folder like is shown in the example. That folder should contain folders that contain the individual runs (in the case of the example "job" 0 to 19).
Then provide the file name of the files that contain the trees (all have to be the same).
Then provide the output name of the file (your choice).

This is quite a memory intensive way of combining TTrees. This is especially the case when you want to read in the output in a new program.
Therefore it is recommended to work with TChain. More on that will follow later
