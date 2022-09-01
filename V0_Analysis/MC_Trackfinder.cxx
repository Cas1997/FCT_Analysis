#if !defined(__CLING__) || defined(__ROOTCLING__)

// #include "SimulationDataFormat/MCTrack.h"
// #include "SimulationDataFormat/MCEventHeader.h"
// #include "SimulationDataFormat/MCUtils.h"
// #include "ITSMFTSimulation/Hit.h"

#endif

#include "TString.h"
#include "TFile.h"
#include "TTree.h"
#include "TMath.h"
#include "XXXTrack.h"
#include "FULLTrack.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TCanvas.h"
//#include <vector>
#include <map>

using o2::xxx::XXXTrack;
using o2::full::FULLTrack;
// using o2::MCTrackT;
// using o2::mcutils::MCTrackNavigator;
using o2::itsmft::Hit;

void fill_tracksmap(std::vector<Hit>* hitXXX, int nEvents, std::vector<std::map<int, XXXTrack>>& allXXXTracks, std::vector<std::map<int, FULLTrack>>& fullTracks, TTree* hitTreeXXX, int type);

int MC_Trackfinder(){
    // User input (place file in "<YourAnalysisFolder>" folder)
    TString FCT_hits_file = "o2sim_HitsFCT_AD.root";
    TString FT3_hits_file = "o2sim_HitsFT3_AD.root";
    // TString Kine_file = "o2sim_Kine_AD.root";
    TString Analysis_Folder = "InAndOutput_UpO2/";

    // Construct paths
    TString FCT_path = Form(Analysis_Folder + FCT_hits_file);
    TString FT3_path = Form(Analysis_Folder + FT3_hits_file);
    // TString Kine_path = Form(Analysis_Folder + Kine_file);
    
    // Get TTrees
    // FCT
    TFile *inputFCT_hits = new TFile(FCT_path, "read");
    TTree *hitTreeFCT = (TTree*)inputFCT_hits->Get("o2sim");
    // FT3
    TFile *inputFT3_hits = new TFile(FT3_path, "read");
    TTree *hitTreeFT3 = (TTree*)inputFT3_hits->Get("o2sim");
    // Kine
    // TFile *input_kine = new TFile(Kine_path, "read");
    // TTree* o2SimKineTree = (TTree*)input_kine->Get("o2sim");

    // Set branch addresses
    // FCT
    std::vector<Hit>* hitFCT = nullptr;
    hitTreeFCT->SetBranchAddress("FCTHit", &hitFCT);
    Int_t nFCTEvents = hitTreeFCT->GetEntries();
    // FT3
    std::vector<Hit>* hitFT3 = nullptr;
    hitTreeFT3->SetBranchAddress("FT3Hit", &hitFT3);
    Int_t nFT3Events = hitTreeFT3->GetEntries();
    // // Kine
    // std::vector<o2::MCTrack>* mcTr = nullptr;
    // o2SimKineTree->SetBranchAddress("MCTrack", &mcTr);
    // o2::dataformats::MCEventHeader* eventHeader = nullptr;
    // o2SimKineTree->SetBranchAddress("MCEventHeader.", &eventHeader);
    // Int_t numberOfEvents = 10; //o2SimKineTree->GetEntries();      

    // Check if number of events match
    if (nFCTEvents == nFT3Events){
        std::cout << "numberOfEvents = " << nFCTEvents << std::endl;
    } else {
        std::cout << "ERROR: Inconsistent number of entries on " << std::endl;
        // std::cout << Kine_path << std::endl;
        // std::cout << " and " << std::endl;
        std::cout << FCT_path << std::endl;
        std::cout << " and " << std::endl;
        std::cout << FT3_path << std::endl;
        return -1;
    }

    // Define trackmaps
    std::vector<std::map<int, FULLTrack>> allFullTracks;
    std::vector<std::map<int, XXXTrack>> allFCTTracks;
    std::vector<std::map<int, XXXTrack>> allFT3Tracks;

    // Resize track maps
    allFullTracks.resize(nFCTEvents);
    allFCTTracks.resize(nFCTEvents);
    allFT3Tracks.resize(nFT3Events);

    // Fill track maps (Full Tracks Map gets filled during this process)
    // FCT
    fill_tracksmap(hitFCT, nFCTEvents, allFCTTracks, allFullTracks, hitTreeFCT, 0);
    // FT3
    fill_tracksmap(hitFT3, nFT3Events, allFT3Tracks, allFullTracks, hitTreeFT3, 0);

    // Use these collection proxies to store tracks in braches
    std::vector<std::vector<o2::full::FULLTrack>> fullTracksColProxy;
    fullTracksColProxy.resize(nFCTEvents);
    std::vector<std::vector<Int_t>> fullTrackIDsColProxy;
    fullTrackIDsColProxy.resize(nFCTEvents);
    std::vector<std::vector<o2::xxx::XXXTrack>> fctTracksColProxy;
    fctTracksColProxy.resize(nFCTEvents);
    std::vector<std::vector<Int_t>> fctTrackIDsColProxy;
    fctTrackIDsColProxy.resize(nFCTEvents);
    std::vector<std::vector<o2::xxx::XXXTrack>> ft3TracksColProxy;
    ft3TracksColProxy.resize(nFCTEvents);
    std::vector<std::vector<Int_t>> ft3TrackIDsColProxy;
    ft3TrackIDsColProxy.resize(nFCTEvents);

    // Define output file
    TString output_path = Form(Analysis_Folder + "MCTracks.root");
    TFile* MCTracks = new TFile(output_path, "RECREATE");
    TTree* MCTrackTree = new TTree("o2sim", "Tree with MC Tracks");
    std::vector<o2::full::FULLTrack>* fullTracks;
    std::vector<Int_t>* fullTrackIDs;
    std::vector<o2::xxx::XXXTrack>* fctTracks;
    std::vector<Int_t>* fctTrackIDs;
    std::vector<o2::xxx::XXXTrack>* ft3Tracks;
    std::vector<Int_t>* ft3TrackIDs;
    // Create branches
    MCTrackTree->Branch("FullTrack", &fullTracks);
    MCTrackTree->Branch("FullTrackID", &fullTrackIDs);
    MCTrackTree->Branch("FCTTrack", &fctTracks);
    MCTrackTree->Branch("FCTTrackID", &fctTrackIDs);
    MCTrackTree->Branch("FT3Track", &ft3Tracks);
    MCTrackTree->Branch("FT3TrackID", &ft3TrackIDs);

    for(Int_t event = 0; event < nFCTEvents; event++){
        fullTracks = &fullTracksColProxy[event];
        fullTrackIDs = &fullTrackIDsColProxy[event];
        auto& fullTracksMap = allFullTracks.at(event);
        auto fullIter = fullTracksMap.begin();
        while(fullIter != fullTracksMap.end()){
            auto& trackID = fullIter->first;
            auto& track = fullIter->second;
            fullTrackIDs->emplace_back(trackID);
            fullTracks->emplace_back(track);
            fullIter++;
        }

        fctTracks = &fctTracksColProxy[event];
        fctTrackIDs = &fctTrackIDsColProxy[event];
        auto& fctTracksMap = allFCTTracks.at(event);
        auto fctIter = fctTracksMap.begin();
        while(fctIter != fctTracksMap.end()){
            auto& trackID = fctIter->first;
            auto& track = fctIter->second;
            fctTrackIDs->emplace_back(trackID);
            fctTracks->emplace_back(track);
            fctIter++;
        }

        ft3Tracks = &ft3TracksColProxy[event];
        ft3TrackIDs = &ft3TrackIDsColProxy[event];
        auto& ft3TracksMap = allFT3Tracks.at(event);
        auto ft3Iter = ft3TracksMap.begin();
        while(ft3Iter != ft3TracksMap.end()){
            auto& trackID = ft3Iter->first;
            auto& track = ft3Iter->second;
            ft3TrackIDs->emplace_back(trackID);
            ft3Tracks->emplace_back(track);
            ft3Iter++;
        }
        MCTrackTree->Fill();
    }
    MCTracks->Write();
    
    return 0;
}

void fill_tracksmap(std::vector<Hit>* hitXXX, int nEvents, std::vector<std::map<int, XXXTrack>>& allXXXTracks, std::vector<std::map<int, FULLTrack>>& fullTracks, TTree* hitTreeXXX, int type){
    // Function to get all the tracks inside of a single detector
    // Add it to the detector specific tracks map
    // and to the full tracks map
    // Type of the Hit. 0: FCT. 1: FT3. 2: TRK
    
    // What is this clResolution?
    Double_t clResolution = 8.44e-4;
    // Fill the tracksmap and gather all the individual hits together in a track
    // A track is one single particle that passes through the detector. 
    // The trackcomponents are the hits in the layers that particle left behind.
    // This is still simulation information and not reconstruction information.
    for(int event = 0; event < nEvents; event++){
        hitTreeXXX->GetEntry(event);
        int nXXXHits = hitXXX->size();

        auto& XXXTracksMap = allXXXTracks.at(event);
        auto& FULLTracksMap = fullTracks.at(event);
        for (Int_t iHit = 0; iHit < nXXXHits; iHit++) {
            Hit* thisHit = &(*hitXXX)[iHit];
            Int_t myID = thisHit->GetTrackID();
            XXXTracksMap[myID] = XXXTracksMap[myID];
            fullTracks[myID] = fullTracks[myID];
            if(type == 0){
                XXXTracksMap[myID].addHit(*thisHit, iHit, clResolution, true);
                FULLTracksMap[myID].addFCTHit(*thisHit, iHit, clResolution, true);
            } else if(type == 1){
                XXXTracksMap[myID].addHit(*thisHit, iHit, clResolution, true);
                FULLTracksMap[myID].addFT3Hit(*thisHit, iHit, clResolution, true);                
            } else if(type == 2){
                XXXTracksMap[myID].addTRKHit(*thisHit, iHit, clResolution, true);
                FULLTracksMap[myID].addTRKHit(*thisHit, iHit, clResolution, true);
            }
            XXXTracksMap[myID].setEventID(event);
            XXXTracksMap[myID].setTrackID(myID);
            FULLTracksMap[myID].setEventID(event);
            FULLTracksMap[myID].setTrackID(myID);
        }
    }
}