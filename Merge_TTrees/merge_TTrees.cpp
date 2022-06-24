#include "TFile.h"
#include "TString.h"
#include "TTree.h"


void merge_TTrees(){
    // This macro merges TTrees. The TTrees are all located in their seperate directory.
    // This macro will access all the subdirectories of that particular job, e.g. AllParticles1LayerFCT/jobx/o2sim_FCTHits.root
    // extract the trees and merge them in one file, "output_name"
    TString path_base = "/misc/alidata131/alice_u/cas/O2/run_fct/fct_jobs/AllParticles1SenLayerFCT";
    TString file_name = "o2sim_HitsFCT.root";
    TString output_name = Form(%s + "/" + "o2sim_HitsFCT_AP1SL.root", path_base);
    Int_t nJobs = 20;

    // Read in all trees and add their pointers to the list
    TList *list = new TList;
    for(Int_t i = 0; i < nJobs; i++){
        TString tree_path = Form(path_base + "/job%d/" + file_name, i);
        TFile *input = new TFile(tree_path, "read");
        TTree *tree = (TTree*)input->Get("o2sim");
        list->Add(tree);
    }

    // Create output file and write the merged tree to it
    TFile *output = new TFile(output_name, "recreate");
    TTree *newtree = TTree::MergeTrees(list);
    newtree->SetName("o2sim");
    output->Write();
    output->Close();

}


