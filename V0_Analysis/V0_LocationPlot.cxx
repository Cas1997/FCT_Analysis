#include "TH1F.h"
#include "TCanvas.h"
#include "V0_mci.h"
#include "TString.h"
#include "TFile.h"
#include "TTree.h"
#include "TMath.h"
//#include <vector>
#include <map>

void V0_LocationPlot(){
	TString input = "InAndOutput_UpO2/V0sFCT.root";
	TFile *V0File = new TFile(input, "read");
    TTree *V0Tree = (TTree*)V0File->Get("o2sim");
	Int_t nEvents = V0Tree->GetEntries();

	std::vector<V0_mci> V0s, *V0sP = &V0s;
	V0Tree->SetBranchAddress("V0", &V0sP);

	TH1F *hist = new TH1F("hist", "V0 conversion location z", 500, -499.5, -0.5);
    hist->GetXaxis()->SetTitle("V0 location z (cm)");
    hist->GetYaxis()->SetTitle("Counts (a.u.)");
	
	Int_t v0_counter = 0;
	Int_t v0bt_counter = 0;
	Int_t v0btFCT_counter = 0;
	for(Int_t event = 0; event < nEvents; event++){
		V0Tree->GetEntry(event);
		
		for(auto& V0 : V0s){
			hist->Fill(V0.GetCZ());
			v0_counter++;
			if(V0.GetBothInFCT()){
				v0bt_counter++;
				if(V0.GetCZ() < -440){
					v0btFCT_counter++;
				}
			}
		}
	}
	std::cout << "This many V0's were measured by the FCT" << std::endl;
	std::cout << v0_counter << std::endl;
	std::cout << "This many V0's had both tracks in the FCT" << std::endl;
	std::cout << v0bt_counter << std::endl;
	std::cout << "This many V0's were created in the FCT and had both tracks in the FCT" << std::endl;
	std::cout << v0btFCT_counter << std::endl;
	TCanvas *c1 = new TCanvas();
	hist->SetStats(0);
	hist->Draw();
	c1->Print("InAndOutput_UpO2/V0Location.pdf");

}