#include "TString.h"
#include "TFile.h"
#include "TTree.h"
#include "TMath.h"
#include "XXXTrack.h"
#include "FULLTrack.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TCanvas.h"
//#include <vector>
#include <map>

using o2::xxx::XXXTrack;
using o2::full::FULLTrack;
// using o2::MCTrackT;
// using o2::mcutils::MCTrackNavigator;
using o2::itsmft::Hit;

// Small program to test MC_Trackfinder.cxx

void MCTrack_Reader(){
	TString input = "InAndOutput_UpO2/MCTracks.root";
	TFile *tracksFile = new TFile(input, "read");
    TTree *tree = (TTree*)tracksFile->Get("o2sim");
	Int_t nEvents = 10; // tree->GetEntries();

	std::vector<o2::full::FULLTrackExt> fullTracks, *fullTracksP = &fullTracks;
    std::vector<Int_t>* fullTrackIDs = nullptr;
    std::vector<o2::xxx::XXXTrackExt> fctTracks, *fctTracksP = &fctTracks;
    std::vector<Int_t>* fctTrackIDs = nullptr;
    std::vector<o2::xxx::XXXTrackExt> ft3Tracks, *ft3TracksP = &ft3Tracks;
    std::vector<Int_t>* ft3TrackIDs = nullptr;

	tree->SetBranchAddress("FullTrack", &fullTracksP);
    tree->SetBranchAddress("FullTrackID", &fullTrackIDs);
    tree->SetBranchAddress("FCTTrack", &fctTracksP);
    tree->SetBranchAddress("FCTTrackID", &fctTrackIDs);
    tree->SetBranchAddress("FT3Track", &ft3TracksP);
    tree->SetBranchAddress("FT3TrackID", &ft3TrackIDs);

	for(Int_t event = 0; event < nEvents; event++){
		tree->GetEntry(event);
		Int_t iTrack = 0;
		std::cout << "event: " << event << std::endl;
		for(auto& fullTrack : fullTracks){
			Int_t fulltrackID = fullTrackIDs->at(iTrack);
			Int_t nHits = fullTrack.getNFULLHits();
			std::cout << "nHits: " << nHits << std::endl;
		}
	}
}