#include "TFile.h"
#include "TCanvas.h"
#include "TTree.h"
#include "TFile.h"
#include "TClonesArray.h"



void TrackID_plot(){

    TFile *input = new TFile("o2sim_HitsFCT_py.root", "read");
    TTree *tree = (TTree*)input->Get("o2sim");

    TClonesArray *FCTHitVars = 0;
    tree->SetBranchAddress("FCTHit", &FCTHitVars);

    int entries = tree->GetEntries();
    std::cout << "Entries " << entries << std::endl;

    TH1F *hist = new TH1F("hist", "MyHistogram", 500, 0, 500);

    for(int i = 0; i < entries; i++){
        tree->GetEntry(i);
        int nentries = FCTHitVars->GetEntries();
        for(int j = 0; j < nentries; j++){
            int mtrackid = tree->GetLeaf("FCTHit.mTrackID")->GetValue(j);
            hist->Fill(mtrackid);

        }
    }

    TCanvas *c1 = new TCanvas();
    hist->Draw();

}
 