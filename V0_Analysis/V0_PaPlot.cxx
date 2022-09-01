#include "TH1F.h"
#include "TCanvas.h"
#include "V0_mci.h"
#include "TString.h"
#include "TFile.h"
#include "TTree.h"
#include "TMath.h"
//#include <vector>
#include <map>

// Plots the pointing angle of the V0's

void V0_PaPlot(){
	TString input = "InAndOutput_UpO2/V0sFCT.root";
	TFile *V0File = new TFile(input, "read");
    TTree *V0Tree = (TTree*)V0File->Get("o2sim");
	Int_t nEvents = V0Tree->GetEntries();

	std::vector<V0_mci> V0s, *V0sP = &V0s;
	V0Tree->SetBranchAddress("V0", &V0sP);

	TH1F *hist = new TH1F("hist", "Pointing angle V0's in FCT", 100, 0, 2);
    hist->GetXaxis()->SetTitle("Pointing angle (a.u.)");
    hist->GetYaxis()->SetTitle("Counts (a.u.)");

	for(Int_t event = 0; event < nEvents; event++){
		V0Tree->GetEntry(event);

		for(auto& V0 : V0s){
			if(V0.GetBothInFCT()){
				if(V0.GetCZ() < -440){
					
					hist->Fill(V0.GetPointingAngle());
				}
			}
		}
	}
	
	TCanvas *c1 = new TCanvas();
	c1->SetLogy();
	hist->SetStats(0);
	hist->Draw();
	c1->Print("InAndOutput_UpO2/V0_PointingAngle.pdf");
}