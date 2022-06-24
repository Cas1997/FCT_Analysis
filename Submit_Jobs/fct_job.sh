#!/bin/bash

export ALIBUILD_WORK_DIR=/misc/alidata131/alice_u/cas/O2/alice/sw
eval `/usr/local/bin/alienv shell-helper`
export PATH=${PATH}

source o2_env.sh

o2-sim -m FCT -e TGeant4 -g pythia8pp -n 5 --configKeyValues "FCTBase.OnlyChargedParticles = false"
