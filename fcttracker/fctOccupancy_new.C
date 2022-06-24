#include "TFile.h"
#include "TTree.h"
#include <TH2F.h>
#include <TH3F.h>

#include "ITSMFTSimulation/Hit.h"
#include "fcttools/HistosHelpers.C"

#include <map>
#include <set>
#include <cstdio> // for NULL, snprintf
#include <fstream>

using o2::itsmft::Hit;

Int_t roundUp(Int_t numToRound, Int_t multiple);

bool DEBUG_VERBOSE = false;

// Hit density scaled according to QED cross section on pureQED events
Double_t xSectionQED = 29533.4;
Double_t xSectionHad = 8;
Bool_t pureQED = false;
Double_t QEDScale = pureQED ? xSectionQED / xSectionHad : 1.0;

void fctOccupancy_new(){
    std::string hitfile = "o2sim_HitsFCT.root";
    std::string tr3Occupancyfile = "fctOccupancy_new.root";
    std::string layoutfile = "FCT_layout.cfg";
    std::string annotation = "FCT occupancy new";

    // Load configFile to check amount of layers
    int numberOfLayers = 0;
    std::ifstream ifs(layoutfile.c_str());
    if (!ifs.good()) {
        std::cout << " Could not find " << layoutfile << std::endl;;
    }
    std::string tempstr;
    Double_t z_layer, r_in, r_out_l_side, Layerx2X0;
    std::vector<Double_t> layerWidth;
    std::vector<Double_t> layerDistance;
    while (std::getline(ifs, tempstr)) {
        if (tempstr[0] == '#') {
            std::cout << " Comment: " << tempstr << std::endl;
            continue;
        }
        std::istringstream iss(tempstr);
        // Don't know if I will use these values yet, but here they are
        iss >> z_layer;
        layerDistance.push_back(z_layer);
        iss >> r_in;
        iss >> r_out_l_side;
        layerWidth.push_back(r_out_l_side);
        iss >> Layerx2X0;
        numberOfLayers++;
    }

    // Make numberOfLayers amount of histograms
    TH2F** occupancy_hists = new TH2F*[numberOfLayers];
    std::vector<TString> histnames;
    int nBinsPerCm = 3;
    for(int i = 0; i < numberOfLayers; i++){
        histnames.push_back(Form("Layer%dOccupancy", i));
        int rLW = roundUp(TMath::Ceil(layerWidth[i]), 100); // Rounded Layer Width
        int nBins = rLW * nBinsPerCm * 2;
        occupancy_hists[i] = new TH2F(histnames[i], histnames[i], nBins, -rLW, rLW, nBins, -rLW, rLW);
    }

    // Make HitMap3D
    std::vector<Double_t>::iterator z_min_it = std::min_element(layerDistance.begin(), layerDistance.end());
    int pos_z_min = std::distance(layerDistance.begin(), z_min_it);
    Double_t z_min = layerDistance[pos_z_min];

    std::vector<Double_t>::iterator z_max_it = std::max_element(layerDistance.begin(), layerDistance.end());
    int pos_z_max = std::distance(layerDistance.begin(), z_max_it);
    Double_t z_max = layerDistance[pos_z_max];

    std::vector<Double_t>::iterator r_max_it = std::max_element(layerWidth.begin(), layerWidth.end());
    int pos_r_max = std::distance(layerWidth.begin(), r_max_it);
    Double_t r_max = layerWidth[pos_r_max];
    r_max = roundUp(TMath::Ceil(r_max), 100);
    
    TH3F* HitMap3D = new TH3F("HitMap3D", "HitMap3D", r_max * nBinsPerCm, -r_max, r_max, r_max * nBinsPerCm, -r_max, r_max, (z_max - z_min) * nBinsPerCm, z_min, z_max);

    // Load in tree
    TFile *HitFileIn = new TFile(hitfile.c_str());
    TTree *hitTree = (TTree *)HitFileIn->Get("o2sim");

    std::vector<Hit> *hit = nullptr;
    hitTree->SetBranchAddress("FCTHit", &hit);
    Int_t nEvents = hitTree->GetEntries();
    std::cout << " Computing hits from " << nEvents << " events" << std::endl;
    for (Int_t event = 0; event < nEvents; event++) {
        hitTree->GetEntry(event);
        Int_t nHits = hit->size();
        
        if (DEBUG_VERBOSE) {
            std::cout << " Processing event " << event << std::endl;
        }

        for (Int_t iHit = 0; iHit < nHits; iHit++) {
            Hit *thisHit = &(*hit)[iHit];
            Int_t nLayer = thisHit->GetDetectorID();
            if (DEBUG_VERBOSE) {
                std::cout << " Layer: " << nLayer << " z = " << thisHit->GetStartZ() << std::endl;
            }
            occupancy_hists[nLayer]->Fill(thisHit->GetStartX(),thisHit->GetStartY());
            HitMap3D->Fill(thisHit->GetStartX(),thisHit->GetStartY(),thisHit->GetStartZ());
        }
    }

    TFile outFile(tr3Occupancyfile.c_str(), "RECREATE");
    
    for(int i = 0; i < numberOfLayers; i++){
        occupancy_hists[i]->SetStats(0);
        occupancy_hists[i]->Scale(QEDScale*1.0/nEvents);
        std::cout << " Layer " << i << " Max " <<  occupancy_hists[i]->GetMaximum() << " hits/event/bin " << std::endl;
    occupancy_hists[i]->Write();
    }

    TCanvas occup_summary = summary_report_gen(occupancy_hists, numberOfLayers, "Occupancy summary", annotation);
    HitMap3D->Write();
    outFile.Close();

}

Int_t roundUp(Int_t numToRound, Int_t multiple){
    if(multiple == 0){ return numToRound;}

    int remainder = numToRound % multiple;
    if(remainder == 0){
        return numToRound;
    }

    return numToRound + multiple - remainder;
}