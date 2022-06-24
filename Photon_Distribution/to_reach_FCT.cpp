#include "TFile.h"
#include "TTree.h"
#include "TFile.h"
#include "TMath.h"
#include "TH1F.h"
#include "TLegend.h"
#include "TString.h"
#include "TCanvas.h"
#include "FCTTrack.h"

#include <map>

// Macro to count how many photons, and by which production process, reach the first layer of the FCT

using o2::fct::FCTTrack;
using o2::fct::Hit;

void to_reach_FCT(){

    // Could not get TMCProcess to work
    TString TMCProcessName[50] = {
        "Primary particle emission",
        "Multiple scattering",
        "Energy loss",
        "Bending in magnetic field",
        "Decay",
        "Lepton pair production",
        "Compton scattering",
        "Photoelectric effect",
        "Bremstrahlung",
        "Delta ray",
        "Positron annihilation",
        "Positron annihilation at rest",
        "Positron annihilation in flight",
        "Hadronic interaction",
        "Nuclear evaporation",
        "Nuclear fission",
        "Nuclear absorbtion",
        "Antiproton annihilation",
        "Antineutron annihilation",
        "Neutron capture",
        "Hadronic elastic",
        "Hadronic incoherent elastic",
        "Hadronic coherent elastic",
        "Hadronic inelastic",
        "Photon inelastic",
        "Muon nuclear interaction",
        "Electron nuclear interaction",
        "Positron nuclear interaction",
        "Time of flight limit",
        "Nuclear photofission",
        "Rayleigh effect",
        "No active process",
        "Energy threshold",
        "Light absorption",
        "Light detection",
        "Light scattering",
        "Maximum allowed step",
        "Cerenkov production",
        "Cerenkov feed back photon",
        "Cerenkov photon reflection",
        "Cerenkov photon refraction",
        "Synchrotron radiation",
        "Scintillation",
        "Transportation",
        "Unknown process",
        "Coulomb scattering",
        "Photo nuclear interaction",
        "User defined process",
        "Optical photon wavelength shifting",
        "Transition radiation"
    };

    std::map<Int_t, Int_t> color_map =  {std::make_pair(0, 1), std::make_pair(4, 2), std::make_pair(8, 3), std::make_pair(10, 4), std::make_pair(13, 5), std::make_pair(20, 6), std::make_pair(27, 7), std::make_pair(44, 8)};// Links production process to color so in the graph you will always get the same color for same the entry

    Float_t firstLayerZ = -442; // cm

    // vector of all forward tracks. The values of the map are the hits
    std::vector<std::map<int, FCTTrack>> allFCTTracks;

    std::vector<Int_t> allPPP; // All photon production processes. A distribution for every process will be made

    std::map<Int_t, std::vector<Double_t>> allPhotons; // map containing all photons with their respective pT


    // Input files
    TFile *inputFCT = new TFile("/misc/alidata131/alice_u/cas/O2/run_fct/fct_jobs/AllParticles1SenLayerFCT/o2sim_HitsFCT_AP1SL.root", "read");
    TTree *hitTreeFCT = (TTree*)inputFCT->Get("o2sim");

    // Set branch address
    std::vector<Hit>* hitFCT = nullptr;
    hitTreeFCT->SetBranchAddress("FCTHit", &hitFCT);
    int nFCTEvents = hitTreeFCT->GetEntries();
    allFCTTracks.resize(nFCTEvents);

    // Save only those photons that appear in the first layer
    // Save them under production process in a map with the corresponding vector which contains the pT
    for(int event = 0; event < nFCTEvents; event++){
        hitTreeFCT->GetEntry(event);
        int nFCTHits = hitFCT->size();
        for (Int_t iHit = 0; iHit < nFCTHits; iHit++) {
            Hit* thisHit = &(*hitFCT)[iHit];
            Int_t myPid = thisHit->GetTrackPid();
            if(myPid == 22){
                short detID = thisHit->GetDetectorID();
                // std::cout << "Detector ID: " << detID << std::endl; // Debug output
                Float_t thisZ = thisHit->GetStartZ();
                // std::cout << "Position: " << thisZ << std::endl; // Debug output
                if(firstLayerZ - 0.5 < thisZ && firstLayerZ + 0.5 > thisZ){
                    // std::cout << "In first layer" << std::endl; // Debug output
                    // Int_t myID = thisHit->GetTrackID();
                    Int_t thisProdProcess = thisHit->GetProdProcess();
                    if(!(std::find(allPPP.begin(), allPPP.end(), thisProdProcess) != allPPP.end())){
                        allPPP.push_back(thisProdProcess);
                    }
                    Double_t pT = 1000. * TMath::Sqrt(thisHit->GetPx() * thisHit->GetPx() + thisHit->GetPy() * thisHit->GetPy());
                    allPhotons[thisProdProcess].push_back(pT);
                }
                // std::cout << "" << std::endl; // Debug output
            }
        }
    }

    // Define TLegend
    TLegend* legend = new TLegend(0.65, 0.65, 0.85, 0.85);
    legend->SetBorderSize(0);


    // Fill the histograms
    std::sort(allPPP.begin(), allPPP.end());
    Int_t nDistributions = allPPP.size();
    TH1F** PhotonDistributions = new TH1F*[nDistributions];
    auto allPPP_iter = allPPP.begin();
    Int_t distCounter = 0;
    while(allPPP_iter != allPPP.end()){
        Int_t prodProcNumber = *allPPP_iter;

        // Define a new histogram
        TString histnumber = Form("hist%d", distCounter);
        PhotonDistributions[distCounter] = new TH1F(histnumber, "", 100, 0, 10);
        PhotonDistributions[distCounter]->SetLineColor(distCounter + 1);
        PhotonDistributions[distCounter]->SetStats(0);
        PhotonDistributions[distCounter]->GetXaxis()->SetTitle("p_{T} (MeV/c)");
        PhotonDistributions[distCounter]->GetYaxis()->SetTitle("1/N_{evt} dN/dp_{T} (MeV^{-1})");
        PhotonDistributions[distCounter]->GetYaxis()->SetRangeUser(0, 100);
        
        // Add the histogram to the legend
        TString prodProcName = TMCProcessName[prodProcNumber];
        legend->AddEntry(PhotonDistributions[distCounter], prodProcName, "l");

        // Fill the histograms
        auto photDist_iter = allPhotons[prodProcNumber].begin();
        while(photDist_iter != allPhotons[prodProcNumber].end()){
            PhotonDistributions[distCounter]->Fill(*photDist_iter);
            photDist_iter++;
        }  
        distCounter++;
        allPPP_iter++;
    }
    
    // Scale and draw the histograms
    Double_t scaleFactor = 1e-4;
    TCanvas *c1 = new TCanvas("c1", "My Root Plots", 2000, 1000);
    c1->SetLogy();
    PhotonDistributions[0]->Scale(scaleFactor);
    PhotonDistributions[0]->Draw("hist l");

    for(int i = 1; i < nDistributions; i++){
        PhotonDistributions[i]->Scale(scaleFactor);
        PhotonDistributions[i]->Draw("hist same l");
    }
    legend->Draw();

    c1->Print("FCTPhotonDistributions_AP1SL.pdf");


}

