#include "TFile.h"
#include "TCanvas.h"
#include "TTree.h"
#include "TFile.h"
#include "TH3F.h"
#include "TPolyLine3D.h"
#include "XXXTrack.h"


#include <map>

//using o2::fct::FCTTrack;
using o2::xxx::XXXTrack;
using o2::itsmft::Hit;

double round_up_to_mult(double numToRound, double multiple);
double round_down_to_mult(double numToRound, double multiple);

void Track_plot_all(){

    // Ouput file
    std::string tr3Tracksfile = "fcttracks.root";

    // vector of all forward tracks, will be stored in output file
    std::vector<std::map<int, XXXTrack>> allFCTTracks;
    std::vector<std::map<int, XXXTrack>> allFT3Tracks;

    // Input files
    TFile *inputFCT = new TFile("o2sim_HitsFCT.root", "read");
    TTree *hitTreeFCT = (TTree*)inputFCT->Get("o2sim");

    TFile *inputFT3 = new TFile("o2sim_HitsFT3.root", "read");
    TTree *hitTreeFT3 = (TTree*)inputFT3->Get("o2sim");

    // Set branch address
    std::vector<Hit>* hitFCT = nullptr;
    hitTreeFCT->SetBranchAddress("FCTHit", &hitFCT);

    std::vector<Hit>* hitFT3 = nullptr;
    hitTreeFT3->SetBranchAddress("FT3Hit", &hitFT3);

    int nFCTEvents = hitTreeFCT->GetEntries();
    allFCTTracks.resize(nFCTEvents);

    int nFT3Events = hitTreeFT3->GetEntries();
    allFT3Tracks.resize(nFT3Events);


    // What is this clResolution?
    Double_t clResolution = 8.44e-4;
    // Fill the tracksmap and gather all the individual tracks together
    // A track is one single particle that passes through the detector. 
    // The trackcomponents are the hits in the layers that particle left behind.
    // This is still simulation information and not reconstruction information.
    for(int i = 0; i < nFCTEvents; i++){
        hitTreeFCT->GetEntry(i);
        int nFCTHits = hitFCT->size();

        auto& FCTTracksMap = allFCTTracks.at(i);
        for (Int_t iHit = 0; iHit < nFCTHits; iHit++) {
            Hit* thisHit = &(*hitFCT)[iHit];
            Int_t myID = thisHit->GetTrackID();
            FCTTracksMap[myID] = FCTTracksMap[myID];
            FCTTracksMap[myID].addHit(*thisHit, iHit, clResolution, true);
            FCTTracksMap[myID].setEventID(i);
            FCTTracksMap[myID].setTrackID(myID);
        }
    }

    for(int i = 0; i < nFT3Events; i++){
        hitTreeFT3->GetEntry(i);
        int nFT3Hits = hitFT3->size();

        auto& FT3TracksMap = allFT3Tracks.at(i);
        for (Int_t iHit = 0; iHit < nFT3Hits; iHit++) {
            Hit* thisHit = &(*hitFT3)[iHit];
            Int_t myID = thisHit->GetTrackID();
            FT3TracksMap[myID] = FT3TracksMap[myID];
            FT3TracksMap[myID].addHit(*thisHit, iHit, clResolution, true);
            FT3TracksMap[myID].setEventID(i);
            FT3TracksMap[myID].setTrackID(myID);
        }
    }


    // Read in detector configuration file to decide the size of the box
    // std::string configFileName = "FCT_layout.cfg";
    // std::ifstream ifs(configFileName.c_str());
    // std::string tempstr;
    // double z_layer, r_in, r_out_l_side, Layerx2X0;

    // std::vector<double> distance;
    // std::vector<double> halfSide;
    // while (std::getline(ifs, tempstr)) {
    //     if (tempstr[0] == '#') {
    //         continue;
    //     }
    //     std::istringstream iss(tempstr);
    //     iss >> z_layer;
    //     distance.push_back(z_layer);
    //     iss >> r_in;
    //     iss >> r_out_l_side;
    //     halfSide.push_back(r_out_l_side);
    //     iss >> Layerx2X0;
    // }

    // double z_begin = *std::min_element(distance.begin(), distance.end());
    // double z_end = *std::max_element(distance.begin(), distance.end());
    // double side_max = *std::max_element(halfSide.begin(), halfSide.end());

    // z_begin = round_down_to_mult(z_begin, 100.);
    // z_end = round_up_to_mult(z_end, 100.);
    // side_max = round_up_to_mult(side_max, 50.);

    double z_begin = -500;
    double z_end = 0.;
    double side_max = 100;


    // Make as many TPolyLine3D as there are tracks through FCT and FT3
    int nFCTTracks = allFCTTracks.at(0).size();
    std::cout << nFCTTracks << std::endl;
    TPolyLine3D** FCTtrackLines = new TPolyLine3D*[nFCTTracks];

    int nFT3Tracks = allFT3Tracks.at(0).size();
    std::cout << nFT3Tracks << std::endl;
    TPolyLine3D** FT3trackLines = new TPolyLine3D*[nFT3Tracks];

    // Loop through all events and plot the tracks

    for (Int_t event = 0; event < 1; event++) {
        
        auto& FCTTracksMap = allFCTTracks.at(event);

        auto FCTiter = FCTTracksMap.begin();
        int FCTcounter = 0;
        while (FCTiter != FCTTracksMap.end()) {
            auto& trackID = FCTiter->first;
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

        auto& FT3TracksMap = allFT3Tracks.at(event);

        auto FT3iter = FT3TracksMap.begin();
        int FT3counter = 0;
        while (FT3iter != FT3TracksMap.end()) {
            auto& trackID = FT3iter->first;
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



    }


    TCanvas *c1 = new TCanvas("c1", "c1", 800, 600);
    // Hack of sorts to provide a "box" where the TPolyLine3D's can be drawn
    TH3F* h3 = new TH3F("h3", "h2", 10, z_begin, z_end, 10, -side_max, side_max, 10, -side_max, side_max);
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

    c1->Print("TracksInFCTFT3_testxxx.pdf");

    // for(int i = 0; i < nTracks; i++){
    //     delete[] trackLines[i];
    // }
    // delete[] trackLines;

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