#include "TFile.h"
#include "TCanvas.h"
#include "TTree.h"
#include "TFile.h"
#include "TH3F.h"
#include "TPolyLine3D.h"
#include "XXXTrack.h"

#include <map>

using o2::xxx::XXXTrack;
using o2::itsmft::Hit;

double round_up_to_mult(double numToRound, double multiple);
double round_down_to_mult(double numToRound, double multiple);
void fill_tracksmap(std::vector<Hit>* hitXXX, int nEvents, std::vector<std::map<int, XXXTrack>>& allXXXTracks, TTree* hitTreeXXX, int type);
void fill_connecting_line(TPolyLine3D* line, std::vector<std::map<int, XXXTrack>>& allXXXTracks1, std::vector<std::map<int, XXXTrack>>& allXXXTracks2, Int_t track_ID, int disp_event);

void Track_plot_all_improved(int display_event){

    // vector of all forward tracks. The values of the map are the hits
    std::vector<std::map<int, XXXTrack>> allFCTTracks;
    std::vector<std::map<int, XXXTrack>> allFT3Tracks;
    std::vector<std::map<int, XXXTrack>> allTRKTracks;

    // Input files
    TFile *inputFCT = new TFile("o2sim_HitsFCT.root", "read");
    TTree *hitTreeFCT = (TTree*)inputFCT->Get("o2sim");

    TFile *inputFT3 = new TFile("o2sim_HitsFT3.root", "read");
    TTree *hitTreeFT3 = (TTree*)inputFT3->Get("o2sim");

    TFile *inputTRK = new TFile("o2sim_HitsTRK.root", "read");
    TTree *hitTreeTRK = (TTree*)inputTRK->Get("o2sim");

    // Set branch address
    std::vector<Hit>* hitFCT = nullptr;
    hitTreeFCT->SetBranchAddress("FCTHit", &hitFCT);
    int nFCTEvents = hitTreeFCT->GetEntries();
    allFCTTracks.resize(nFCTEvents);

    std::vector<Hit>* hitFT3 = nullptr;
    hitTreeFT3->SetBranchAddress("FT3Hit", &hitFT3);
    int nFT3Events = hitTreeFT3->GetEntries();
    allFT3Tracks.resize(nFT3Events);

    std::vector<Hit>* hitTRK = nullptr;
    hitTreeTRK->SetBranchAddress("TRKHit", &hitTRK);
    int nTRKEvents = hitTreeTRK->GetEntries();
    allTRKTracks.resize(nTRKEvents);

    // Fill the track maps. Fills allXXXTracks with the tracks based on the hits per event
    fill_tracksmap(hitFCT, nFCTEvents, allFCTTracks, hitTreeFCT, 0);
    fill_tracksmap(hitFT3, nFT3Events, allFT3Tracks, hitTreeFT3, 0);
    fill_tracksmap(hitTRK, nTRKEvents, allTRKTracks, hitTreeTRK, 1);


    // Make as many TPolyLine3D as there are tracks through FCT, FT3 and TRK
    int nFCTTracks = allFCTTracks.at(display_event).size();
    std::cout << "nFCTTracks: " << nFCTTracks << std::endl;
    TPolyLine3D** FCTtrackLines = new TPolyLine3D*[nFCTTracks];

    int nFT3Tracks = allFT3Tracks.at(display_event).size();
    std::cout << "nFT3Tracks: " << nFT3Tracks << std::endl;
    TPolyLine3D** FT3trackLines = new TPolyLine3D*[nFT3Tracks];

    int nTRKTracks = allTRKTracks.at(display_event).size();
    std::cout << "nTRKTracks: " << nTRKTracks << std::endl;
    TPolyLine3D** TRKtrackLines = new TPolyLine3D*[nTRKTracks];

    
    // Loop through all events and plot the tracks

    std::vector<Int_t> FCTTrackIDs;
    std::vector<Int_t> FT3TrackIDs;
    std::vector<Int_t> TRKTrackIDs;

    for (Int_t event = display_event; event < display_event + 1; event++) {
        
        // FCT
        auto& FCTTracksMap = allFCTTracks.at(event);

        auto FCTiter = FCTTracksMap.begin();
        int FCTcounter = 0;
        while (FCTiter != FCTTracksMap.end()) {
            auto& trackID = FCTiter->first;
            FCTTrackIDs.push_back(trackID);
            auto& track = FCTiter->second;
            auto nHits = track.getNumberOfPoints();
            // Now fill the trackLines
            std::vector<Float_t> xCordHits = track.getXCoordinates();
            std::vector<Float_t> yCordHits = track.getYCoordinates();
            std::vector<Float_t> zCordHits = track.getZCoordinates();
            FCTtrackLines[FCTcounter] = new TPolyLine3D(nHits);
            for(int i = 0; i < nHits; i++) {
                FCTtrackLines[FCTcounter]->SetPoint(i, zCordHits[i], xCordHits[i], yCordHits[i]);
            }
            FCTtrackLines[FCTcounter]->SetLineColor(kBlue);
            FCTcounter++;
            ++FCTiter;
        }

        // FT3
        auto& FT3TracksMap = allFT3Tracks.at(event);

        auto FT3iter = FT3TracksMap.begin();
        int FT3counter = 0;
        while (FT3iter != FT3TracksMap.end()) {
            auto& trackID = FT3iter->first;
            FT3TrackIDs.push_back(trackID);
            auto& track = FT3iter->second;
            auto nHits = track.getNumberOfPoints();
            // Now fill the trackLines
            std::vector<Float_t> xCordHits = track.getXCoordinates();
            std::vector<Float_t> yCordHits = track.getYCoordinates();
            std::vector<Float_t> zCordHits = track.getZCoordinates();
            FT3trackLines[FT3counter] = new TPolyLine3D(nHits);
            for(int i = 0; i < nHits; i++) {
                FT3trackLines[FT3counter]->SetPoint(i, zCordHits[i], xCordHits[i], yCordHits[i]);
            }
            FT3trackLines[FT3counter]->SetLineColor(kRed);
            FT3counter++;
            ++FT3iter;
        }

        // TRK
        auto& TRKTracksMap = allTRKTracks.at(event);

        auto TRKiter = TRKTracksMap.begin();
        int TRKcounter = 0;
        while (TRKiter != TRKTracksMap.end()) {
            auto& trackID = TRKiter->first;
            TRKTrackIDs.push_back(trackID);
            auto& track = TRKiter->second;
            auto nHits = track.getNumberOfPoints();
            // Now fill the trackLines
            std::vector<Float_t> xCordHits = track.getXCoordinates();
            std::vector<Float_t> yCordHits = track.getYCoordinates();
            std::vector<Float_t> zCordHits = track.getZCoordinates();
            TRKtrackLines[TRKcounter] = new TPolyLine3D(nHits);
            for(int i = 0; i < nHits; i++) {
                TRKtrackLines[TRKcounter]->SetPoint(i, zCordHits[i], xCordHits[i], yCordHits[i]);
            }
            TRKtrackLines[TRKcounter]->SetLineColor(kGreen);
            TRKcounter++;
            ++TRKiter;
        }



    }


    // Check if track IDs match
    auto FCTIDiter = FCTTrackIDs.begin();
    auto FT3IDiter = FT3TrackIDs.begin();
    auto TRKIDiter = TRKTrackIDs.begin();
    
    // Match containers
    std::vector<Int_t> FCT_FT3;
    std::vector<Int_t> FCT_TRK;
    std::vector<Int_t> FT3_TRK;

    int connecting_lines = 0;

    // Compare FCT with FT3
    std::cout << "Displaying matches" << std::endl;
    while (FCTIDiter != FCTTrackIDs.end()) {
        if(std::find(FT3TrackIDs.begin(), FT3TrackIDs.end(), *FCTIDiter) != FT3TrackIDs.end()){
            FCT_FT3.push_back(*FCTIDiter);
            connecting_lines++;
            std::cout << "Match for FCT and FT3 at Track ID " << *FCTIDiter << std::endl;
        }
        FCTIDiter++;
    }
    std::cout << "Number of connecting lines: " << connecting_lines << std::endl;
    std::cout << "" << std::endl;

    // Compare FT3 with TRK
    while (FT3IDiter != FT3TrackIDs.end()) {
        if(std::find(TRKTrackIDs.begin(), TRKTrackIDs.end(), *FT3IDiter) != TRKTrackIDs.end()){
            FT3_TRK.push_back(*FT3IDiter);
            connecting_lines++;
            std::cout << "Match for FT3 and TRK at Track ID " << *FT3IDiter << std::endl;
        }
        FT3IDiter++;
    }
    std::cout << "Number of connecting lines: " << connecting_lines << std::endl;
    int current_n_clines = connecting_lines;
    std::cout << "" << std::endl;

    FCTIDiter = FCTTrackIDs.begin();
    // Compare FCT with TRK
    while (FCTIDiter != FCTTrackIDs.end()) {
        if(std::find(TRKTrackIDs.begin(), TRKTrackIDs.end(), *FCTIDiter) != TRKTrackIDs.end()){
            FCT_TRK.push_back(*FCTIDiter);
            std::cout << "Match for FCT and TRK at Track ID " << *FCTIDiter << std::endl;
            if(!(std::find(FT3TrackIDs.begin(), FT3TrackIDs.end(), *FCTIDiter) != FT3TrackIDs.end())){
                connecting_lines++;
            }
        }
        FCTIDiter++;
    }
    std::cout << "Number of connecting lines: " << connecting_lines << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Number of FCT_FT3 lines " << FCT_FT3.size() << std::endl;



    // Make the connecting lines between tracks for each track that occurs in multiple detectors
    TPolyLine3D** ConnectingLines = new TPolyLine3D*[connecting_lines];
    int connecting_lines_counter = 0;

    auto FCT_FT3_iter = FCT_FT3.begin();
    while(FCT_FT3_iter != FCT_FT3.end()) {
        
        ConnectingLines[connecting_lines_counter] = new TPolyLine3D(2);
        fill_connecting_line(ConnectingLines[connecting_lines_counter], allFCTTracks, allFT3Tracks, *FCT_FT3_iter, display_event);
        ConnectingLines[connecting_lines_counter]->SetLineColor(kBlack);
        connecting_lines_counter++;
        std::cout << "Found match number" << connecting_lines_counter << std::endl; 
        FCT_FT3_iter++;

    }

    auto FT3_TRK_iter = FT3_TRK.begin();
    while(FT3_TRK_iter != FT3_TRK.end()) {
        
        ConnectingLines[connecting_lines_counter] = new TPolyLine3D(2);
        fill_connecting_line(ConnectingLines[connecting_lines_counter], allFT3Tracks, allTRKTracks, *FT3_TRK_iter, display_event);
        ConnectingLines[connecting_lines_counter]->SetLineColor(kBlack);
        connecting_lines_counter++;
        std::cout << "Found match number" << connecting_lines_counter << std::endl; 
        FT3_TRK_iter++;

    }

    if(!(current_n_clines == connecting_lines)){
        std::cout << "You have some work to do" << std::endl;
    }


    // Set values for "detector box" to be drawn
    double z_begin = -500;
    double z_end = 0.;
    double side_max = 100;

    TCanvas *c1 = new TCanvas("c1", "c1", 800, 600);
    // Hack of sorts to provide a "box" where the TPolyLine3D's can be drawn
    TH3F* h3 = new TH3F("h3", "FCT, FT3 and TRK tracks", 10, z_begin, z_end, 10, -side_max, side_max, 10, -side_max, side_max);
    h3->GetXaxis()->SetTitle("Z (cm)");
    h3->GetYaxis()->SetTitle("X (cm)");
    h3->GetZaxis()->SetTitle("Y (cm)");
    h3->SetStats(0);

    h3->Draw();
    for( int i = 0; i < nFCTTracks; i++){
        FCTtrackLines[i]->Draw();
    }
    for( int i = 0; i < nFT3Tracks; i++){
        FT3trackLines[i]->Draw();
    }
    for( int i = 0; i < nTRKTracks; i++){
        TRKtrackLines[i]->Draw();
    }
    for( int i = 0; i < connecting_lines; i++){
        ConnectingLines[i]->Draw();
    }

    c1->Print(Form("TracksInFCTFT3TRK_connect_ev%d.pdf", display_event));


}

double round_up_to_mult(double numToRound, double multiple){
    if(multiple == 0) {
        return numToRound;
    }

    double roundUp;
    double roundDown = ((double)((int) (numToRound) / (int) (multiple))) * multiple;
    if(numToRound > 0) {
        roundUp = roundDown + multiple;
    } else {
        roundUp = roundDown;
    }
    return roundUp;
}

double round_down_to_mult(double numToRound, double multiple){
    if(multiple == 0) {
        return numToRound;
    }

    double roundUp;
    double roundDown = ((double)((int) (numToRound) / (int) (multiple))) * multiple;
    if(numToRound > 0) {
        roundUp = roundDown;
    } else {
        roundUp = roundDown - multiple;
    }
    return roundUp;
}

void fill_tracksmap(std::vector<Hit>* hitXXX, int nEvents, std::vector<std::map<int, XXXTrack>>& allXXXTracks, TTree* hitTreeXXX, int type){
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
            XXXTracksMap[myID] = XXXTracksMap[myID];
            if(type == 0){
                XXXTracksMap[myID].addHit(*thisHit, iHit, clResolution, true);
            } else if(type == 1){
                XXXTracksMap[myID].addTRKHit(*thisHit, iHit, clResolution, true);
            }
            XXXTracksMap[myID].setEventID(event);
            XXXTracksMap[myID].setTrackID(myID);
        }
    }
}

void fill_connecting_line(TPolyLine3D* line, std::vector<std::map<int, XXXTrack>>& allXXXTracks1, std::vector<std::map<int, XXXTrack>>& allXXXTracks2, Int_t track_ID, int disp_event){
    // Function to find the two points to connect two tracks
    // Match track ID with the given value
    // And store the xyz values of the track

    auto& XXXTracksMap1 = allXXXTracks1.at(disp_event);
    auto XXX1iter = XXXTracksMap1.begin();
    std::vector<Float_t> XXX1xCordHits;
    std::vector<Float_t> XXX1yCordHits;
    std::vector<Float_t> XXX1zCordHits;
    bool found = false;
    while(!found){
        if(XXX1iter->first == track_ID){
            found = true;
            auto& track = XXX1iter->second;
            XXX1xCordHits = track.getXCoordinates();
            XXX1yCordHits = track.getYCoordinates();
            XXX1zCordHits = track.getZCoordinates();
        }
        XXX1iter++;
    }

    auto& XXXTracksMap2 = allXXXTracks2.at(disp_event);
    auto XXX2iter = XXXTracksMap2.begin();
    std::vector<Float_t> XXX2xCordHits;
    std::vector<Float_t> XXX2yCordHits;
    std::vector<Float_t> XXX2zCordHits;
    found = false;
    while(!found){
        if(XXX2iter->first == track_ID){
            found = true;
            auto& track = XXX2iter->second;
            XXX2xCordHits = track.getXCoordinates();
            XXX2yCordHits = track.getYCoordinates();
            XXX2zCordHits = track.getZCoordinates();
        }
        XXX2iter++;
    }

    if(*XXX1zCordHits.begin() < *XXX2zCordHits.end()){
        line->SetPoint(0, *XXX1zCordHits.begin(), *XXX1xCordHits.begin(), *XXX1yCordHits.begin());
        line->SetPoint(1, *(XXX2zCordHits.end() - 1), *(XXX2xCordHits.end() - 1), *(XXX2yCordHits.end() - 1));
        auto iter = XXX2xCordHits.begin();
    } else if(*XXX1zCordHits.end() > *XXX2zCordHits.begin()){
        line->SetPoint(0, *(XXX1zCordHits.end() - 1), *(XXX1xCordHits.end() - 1), *(XXX1yCordHits.end() - 1));
        line->SetPoint(1, *XXX2zCordHits.begin(), *XXX2xCordHits.begin(), *XXX2yCordHits.begin());
    }
    return;
}