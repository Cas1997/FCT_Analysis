#include "TH1F.h"
#include "TCanvas.h"
#include "V0_mci.h"
#include "TString.h"
#include "TFile.h"
#include "TTree.h"
#include "TMath.h"
#include "TLegend.h"
//#include <vector>
#include <map>
#include "TLatex.h"


// This macro plots the production process of the V0's to reach the FCT
// as function of their pT

void V0_FCTBackground(){
	
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
	std::vector<Int_t> allPPP; // All photon production processes. A distribution for every process will be made
	std::map<Int_t, std::vector<Double_t>> allPhotons; // map containing all photons with their respective pT

	TString input = "InAndOutput_UpO2/V0sFCT.root";
	TFile *V0File = new TFile(input, "read");
    TTree *V0Tree = (TTree*)V0File->Get("o2sim");
	Int_t nEvents = V0Tree->GetEntries();

	std::vector<V0_mci> V0s, *V0sP = &V0s;
	V0Tree->SetBranchAddress("V0", &V0sP);

	for(Int_t event = 0; event < nEvents; event++){
		V0Tree->GetEntry(event);
		for(auto& V0 : V0s){
			if(V0.GetBothInFCT()){
				if(V0.GetCZ() < -440){
					Double_t thisE = 1000 * V0.GetPhotonE();
					if(thisE < 10){continue;}
					Double_t thisPt = 1000. * V0.GetPhotonPt();
					Double_t thisPP = V0.GetPhotonProductionProcess();
					if(!(std::find(allPPP.begin(), allPPP.end(), thisPP) != allPPP.end())){
						allPPP.push_back(thisPP);
					}
					allPhotons[thisPP].push_back(thisPt);
				}
			}
		}
	}

	// Define Canvas
	TCanvas *c1 = new TCanvas("c1", "My Root Plots", 2000, 1000);
    c1->DrawFrame(0, 0, 1, 1);
    c1->SetLogy();
    c1->SetGrid();

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
        
        // Add the histogram to the legend
        TString prodProcName = TMCProcessName[prodProcNumber];
        legend->AddEntry(PhotonDistributions[distCounter], prodProcName, "l");

        // Fill and the histogram
        auto photDist_iter = allPhotons[prodProcNumber].begin();
        while(photDist_iter != allPhotons[prodProcNumber].end()){
            PhotonDistributions[distCounter]->Fill(*photDist_iter);
            photDist_iter++;
        }  
        distCounter++;
        allPPP_iter++;
    }

    // Scale and draw the histograms + Low Signal Graph
    Double_t scaleFactor = 1e-4;

    PhotonDistributions[0]->Scale(scaleFactor);
    PhotonDistributions[0]->GetYaxis()->SetRangeUser(1e-5, 1e2);
    PhotonDistributions[0]->SetTitle("Normalized V0 Distribution in FCT");
    PhotonDistributions[0]->Draw("hist l");

    for(int i = 1; i < nDistributions; i++){
        PhotonDistributions[i]->Scale(scaleFactor);
        PhotonDistributions[i]->Draw("hist same l");
    }

	legend->Draw();
    TLatex t;
    t.SetTextSize(0.025);
    t.DrawLatexNDC(.2, .865, "ALICE 3 FCT Study");
    t.DrawLatexNDC(.2, .835, "pp, #sqrt{s} = 14 TeV");
    t.DrawLatexNDC(.2, .805, "PYTHIA 8.304, 100K events");
    t.DrawLatexNDC(.2, .775, "Geant4 FTFP_BERT_EMV+optical");
    t.DrawLatexNDC(.2, .745, "-5 < #eta < -3");
    t.DrawLatexNDC(.2, .715, "E_{#gamma} > 10 MeV");

    c1->Print("InAndOutput_UpO2/V0Distribution_Ef.pdf");
}