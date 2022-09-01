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
#include "TRandom3.h"


// This macro plots the production process of the pointing angle distribution

void V0_PaProcPlot(){
	
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
	std::vector<Int_t> allV0PaPP; // All V0 Pointing Angle Production Processes. A distribution for every process will be made
	std::map<Int_t, std::vector<Double_t>> allPhotons; // map containing all photons with their respective pointing angle

	TString input = "InAndOutput_UpO2/V0sFCT.root";
	TFile *V0File = new TFile(input, "read");
    TTree *V0Tree = (TTree*)V0File->Get("o2sim");
	Int_t nEvents = V0Tree->GetEntries();

	std::vector<V0_mci> V0s, *V0sP = &V0s;
	V0Tree->SetBranchAddress("V0", &V0sP);

	// Smear the pointing angle
	Bool_t pasmearing = true;
	Int_t totalV0 = 0;
	Int_t afterCutV0 = 0;
	Int_t totalPrim = 0;
	Int_t afterCutV0Prim = 0;
	Double_t paCut = 0.05;

	for(Int_t event = 0; event < nEvents; event++){
		V0Tree->GetEntry(event);
		for(auto& V0 : V0s){
			if(V0.GetBothInFCT()){
				if(V0.GetCZ() < -440){
					Double_t thisE = 1000 * V0.GetPhotonE();
					Double_t thisPt = 1000. * V0.GetPhotonPt();
					if(thisE < 10 || thisPt > 10){continue;}
					totalV0++;
					Double_t thisPa = V0.GetPointingAngle();
					Double_t newPa;
					if(pasmearing){
						Double_t sigma;
						TRandom3 rnd(2134);
						if(thisE < 50){
							sigma = 0.275;
						} else if(thisE > 50 && thisE < 100){
							sigma = 0.05;
						} else if(thisE > 100){
							sigma = 0.025;
						}
						newPa = thisPa + (pasmearing ? rnd.Gaus(0, sigma) : 0);
						if(newPa < paCut){
							afterCutV0++;
							if(V0.GetPhotonProductionProcess() == 0){
								afterCutV0Prim++;
							}
						}
					}
					if(V0.GetPhotonProductionProcess() == 0){
						totalPrim++;
					}

					Double_t thisPP = V0.GetPhotonProductionProcess();
					if(!(std::find(allV0PaPP.begin(), allV0PaPP.end(), thisPP) != allV0PaPP.end())){
						allV0PaPP.push_back(thisPP);
					}
					if(pasmearing){
						allPhotons[thisPP].push_back(newPa);
					} else {
						allPhotons[thisPP].push_back(thisPa);
					}
				}
			}
		}
	}

	std::cout << "This many V0s for " << nEvents << " events" << std::endl;
	std::cout << totalV0 << std::endl;
	std::cout << "This many V0's are left after the cut " << paCut << " < pa: " << std::endl;
	std::cout << afterCutV0 << std::endl;
	std::cout << "This many V0s were made by primary photons" << std::endl;
	std::cout << totalPrim << std::endl;
	std::cout << "This many primary photons are left" << std::endl;
	std::cout << afterCutV0Prim << std::endl;


	// Define Canvas
	TCanvas *c1 = new TCanvas("c1", "My Root Plots", 2000, 1000);
    c1->DrawFrame(0, 0, 1, 1);
    c1->SetLogy();
    c1->SetGrid();

	// Define TLegend
    TLegend* legend = new TLegend(0.65, 0.65, 0.85, 0.85);
    legend->SetBorderSize(0);

	// Fill the histograms
    std::sort(allV0PaPP.begin(), allV0PaPP.end());
    Int_t nDistributions = allV0PaPP.size();
    TH1F** V0Distributions = new TH1F*[nDistributions];
    auto allV0PaPP_iter = allV0PaPP.begin();
    Int_t distCounter = 0;
    while(allV0PaPP_iter != allV0PaPP.end()){
        Int_t prodProcNumber = *allV0PaPP_iter;

        // Define a new histogram
        TString histnumber = Form("hist%d", distCounter);
        V0Distributions[distCounter] = new TH1F(histnumber, "", 100, 0, 2);
        V0Distributions[distCounter]->SetLineColor(distCounter + 1);
        V0Distributions[distCounter]->SetStats(0);
        V0Distributions[distCounter]->GetXaxis()->SetTitle("Pointing angle (a.u.)");
		//V0Distributions[distCounter]->GetYaxis()->SetTitle("Counts (a.u.)");
        V0Distributions[distCounter]->GetYaxis()->SetTitle("1/N_{evt} dN/dpa (a.u.)");
        
        // Add the histogram to the legend
        TString prodProcName = TMCProcessName[prodProcNumber];
        legend->AddEntry(V0Distributions[distCounter], prodProcName, "l");

        // Fill and the histogram
        auto photDist_iter = allPhotons[prodProcNumber].begin();
        while(photDist_iter != allPhotons[prodProcNumber].end()){
            V0Distributions[distCounter]->Fill(*photDist_iter);
            photDist_iter++;
        }  
        distCounter++;
        allV0PaPP_iter++;
    }

    // Scale and draw the histograms
    Double_t scaleFactor = 5e-4;

    V0Distributions[0]->Scale(scaleFactor);
	//V0Distributions[0]->GetYaxis()->SetRangeUser(1e-1, 1e5);
    V0Distributions[0]->GetYaxis()->SetRangeUser(1e-5, 1e2);
    //V0Distributions[0]->SetTitle("Pointing angle Distribution in FCT");
	V0Distributions[0]->SetTitle("Normalized Pointing angle Distribution in FCT");
    V0Distributions[0]->Draw("hist");

    for(int i = 1; i < nDistributions; i++){
        V0Distributions[i]->Scale(scaleFactor);
        V0Distributions[i]->Draw("hist same");
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
	t.DrawLatexNDC(.2, .685, "p_{T} < 10 MeV/c");

    c1->Print("InAndOutput_UpO2/PaProcDistribution_Ef_pTf_test.pdf");	

}