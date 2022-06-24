#include "TFile.h"
#include "TTree.h"
#include "TFile.h"
#include "FCTTrack.h"

#include <map>

using o2::fct::FCTTrack;
using o2::fct::Hit;

void fill_tracksmap(std::vector<Hit>* hitXXX, int nEvents, std::vector<std::map<int, FCTTrack>>& allXXXTracks, TTree* hitTreeXXX);

void print_hit_prop(){

    // vector of all forward tracks. The values of the map are the hits
    std::vector<std::map<int, FCTTrack>> allFCTTracks;


    // Input files
    TFile *inputFCT = new TFile("o2sim_HitsFCT_OwnHitsClassNoCharge.root", "read");
    TTree *hitTreeFCT = (TTree*)inputFCT->Get("o2sim");

    // Set branch address
    std::vector<Hit>* hitFCT = nullptr;
    hitTreeFCT->SetBranchAddress("FCTHit", &hitFCT);
    int nFCTEvents = hitTreeFCT->GetEntries();
    allFCTTracks.resize(nFCTEvents);

    fill_tracksmap(hitFCT, nFCTEvents, allFCTTracks, hitTreeFCT);

    std::vector<Int_t> FCTTrackIDs;

    int nEvents_display;
    if(nFCTEvents > 10){
        nEvents_display = 10;
    } else {
        nEvents_display = nFCTEvents;
    }

    for (Int_t event = 0; event < nEvents_display; event++) {
        
        auto& FCTTracksMap = allFCTTracks.at(event);

        auto FCTiter = FCTTracksMap.begin();
        std::cout << "---------------------" << std::endl;
        std::cout << "Event number " << event << std::endl;
        Int_t trackCounter = 0;
        while (FCTiter != FCTTracksMap.end()) {
            auto& trackID = FCTiter->first;
            FCTTrackIDs.push_back(trackID);
            auto& track = FCTiter->second;
            auto nHits = track.getNumberOfPoints();
            Int_t prodProcess = track.getProdProcess();
            Int_t trackPid = track.getTrackPid();
            std::cout << "Track number: " << trackCounter << std::endl;
            std::cout << "PDGCode: " << trackPid << std::endl;
            std::cout << "Production process: " << prodProcess << std::endl;
            std::cout << "" << std::endl;
            ++FCTiter;
            trackCounter++;
        }



    }

}

void fill_tracksmap(std::vector<Hit>* hitXXX, int nEvents, std::vector<std::map<int, FCTTrack>>& allXXXTracks, TTree* hitTreeXXX){
    // type: 0: FCT & FT3, 1: TRK
    
    // What is this clResolution?
    Double_t clResolution = 8.44e-4;
    // Fill the tracksmap and gather all the individual tracks together
    // A track is one single particle that passes through the detector. 
    // The trackcomponents are the hits in the layers that particle left behind.
    // This is still simulation information and not reconstruction information.
    for(int event = 0; event < nEvents; event++){
        hitTreeXXX->GetEntry(event);
        int nXXXHits = hitXXX->size();

        auto& XXXTracksMap = allXXXTracks.at(event);
        for (Int_t iHit = 0; iHit < nXXXHits; iHit++) {
            Hit* thisHit = &(*hitXXX)[iHit];
            Int_t myID = thisHit->GetTrackID();
            Int_t myPid = thisHit->GetTrackPid();
            Int_t myProdProcess = thisHit->GetProdProcess();
            if(myProdProcess != XXXTracksMap[myID].getProdProcess() && XXXTracksMap[myID].getProdProcess() != 0){
                std::cout << "That is odd. The production processes of the same track do not match" << std::endl;
                std::cout << "myProdProcess: " << myProdProcess << std::endl;
                std::cout << "Tracksmap: " << XXXTracksMap[myID].getProdProcess() << std::endl;
            }
            XXXTracksMap[myID] = XXXTracksMap[myID];
            XXXTracksMap[myID].addHit(*thisHit, iHit, clResolution, true);
            XXXTracksMap[myID].setEventID(event);
            XXXTracksMap[myID].setTrackID(myID);
            XXXTracksMap[myID].setTrackPid(myPid);
            XXXTracksMap[myID].setProdProcess(myProdProcess);
        }
    }
}