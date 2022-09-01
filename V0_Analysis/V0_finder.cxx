#if !defined(__CLING__) || defined(__ROOTCLING__)

#include "SimulationDataFormat/MCTrack.h"
#include "SimulationDataFormat/MCEventHeader.h"
#include "SimulationDataFormat/MCUtils.h"

#endif

#include "V0_mci.h"
#include "TString.h"
#include "TFile.h"
#include "TTree.h"
#include "TMath.h"
#include "XXXTrack.h"
#include "FULLTrack.h"
//#include <vector>
#include <map>

using o2::xxx::XXXTrack;
using o2::full::FULLTrack;
using o2::MCTrackT;
using o2::mcutils::MCTrackNavigator;
using o2::itsmft::Hit;

bool is_close(Double_t vertexCord0, Double_t vertexCord1, Double_t check_range);

// This program will find all V0's per event and save them in V0_mci

int V0_finder(){

    Bool_t verbose = false;

	// Load in tree with tracks from FCT, FT3 and all tracks
	TString input = "InAndOutput_UpO2/MCTracks.root";
	TFile *tracksFile = new TFile(input, "read");
    TTree *trackTree = (TTree*)tracksFile->Get("o2sim");
	Int_t nFCTEvents = trackTree->GetEntries();

	// Set branch address - TrackTree
    std::vector<o2::xxx::XXXTrack> fctTracks, *fctTracksP = &fctTracks;
    std::vector<Int_t>* fctTrackIDs = nullptr;
    std::vector<o2::xxx::XXXTrack> ft3Tracks, *ft3TracksP = &ft3Tracks;
    std::vector<Int_t>* ft3TrackIDs = nullptr;

    trackTree->SetBranchAddress("FCTTrack", &fctTracksP);
    trackTree->SetBranchAddress("FCTTrackID", &fctTrackIDs);
    trackTree->SetBranchAddress("FT3Track", &ft3TracksP);
    trackTree->SetBranchAddress("FT3TrackID", &ft3TrackIDs);

	// Input kine file
    TString input_path_kine = "InAndOutput_UpO2/o2sim_Kine_AD.root";
    TFile *input_kine = new TFile(input_path_kine, "read");
    TTree* o2SimKineTree = (TTree*)input_kine->Get("o2sim");

    // Set branch address - kine tree
    std::vector<o2::MCTrack>* mcTr = nullptr;
    o2SimKineTree->SetBranchAddress("MCTrack", &mcTr);
    o2::dataformats::MCEventHeader* eventHeader = nullptr;
    o2SimKineTree->SetBranchAddress("MCEventHeader.", &eventHeader);
	Int_t nEvents = o2SimKineTree->GetEntries();	
	
	// Debugging variables
    int v0counter = 0;
    int ev0counter = 0;
    int mv0counter = 0;
    int kPNullCounter = 0;
    int otherProcessCounter = 0;
    int v0_inside_fct = 0;
    int v0_outside_fct = 0;

    // Make output TFile and tree
    std::vector<std::vector<V0_mci>> V0Col;
    V0Col.resize(nFCTEvents);
    TString output = "InAndOutput_UpO2/V0sFCT.root";
    TFile* V0File = new TFile(output, "RECREATE");
    TTree* V0Tree = new TTree("o2sim", "V0sFCT");
    std::vector<V0_mci>* V0List;
    V0Tree->Branch("V0", &V0List);
	
	// Loop through all events
	for(Int_t event = 0; event < nFCTEvents; event++){
        if(verbose){
            std::cout << "--------------------------------------------------------" << std::endl;
            std::cout << "Event: " << event << std::endl;
            std::cout << std::endl;
        }
		trackTree->GetEntry(event);
        o2SimKineTree->GetEntry(event);
		V0List = &V0Col[event];

        std::vector<int> foundV0s; // found V0s by motherTrackID per event

		for(auto& fctTrack : fctTracks){
			Int_t thisTrackID = fctTrack.getTrackID();
			
			// True MC Track
			MCTrackT<float>* trueTrack = &(*mcTr).at(thisTrackID);
			int pdgTrueTrack = trueTrack->GetPdgCode();
			
			// Check if this particle is an e- or e+ or mu+ or mu-
			if(pdgTrueTrack == 11 || pdgTrueTrack == -11 || pdgTrueTrack == 13 || pdgTrueTrack == -13){
				int motherTrackID = trueTrack->getMotherTrackId();

                // If V0 already found, continue with next track
                if(std::find(foundV0s.begin(), foundV0s.end(), motherTrackID) != foundV0s.end()){
                    continue;
                }

				MCTrackT<float>* motherTrack = &(*mcTr).at(motherTrackID);
				
				// Check if mother is photon (22)
				if(motherTrack->GetPdgCode() == 22){
					// Get both daughter tracks
					Int_t d0ID = motherTrack->getFirstDaughterTrackId();
                    MCTrackT<float>* d0Track = &(*mcTr).at(d0ID);
                    Int_t d1ID = motherTrack->getLastDaughterTrackId();
                    MCTrackT<float>* d1Track = &(*mcTr).at(d1ID);
					
					// Check to make sure they are not the same particle
                    if(d0ID != d1ID){
						Double_t cr = 0.01; // check range
						Double_t d0Vx = d0Track->Vx();
						Double_t d0Vy = d0Track->Vy();
						Double_t d0Vz = d0Track->Vz();
						Double_t d1Vx = d1Track->Vx();
						Double_t d1Vy = d1Track->Vy();
						Double_t d1Vz = d1Track->Vz();
						// Check if daughter particles are produced close to each other 
						// since they could technically be produced by two bremsstrahlungs processes
						if(is_close(d0Vx, d1Vx, cr) && is_close(d0Vy, d1Vy, cr) && is_close(d0Vz, d1Vz, cr)){
							// Daughter particles originate from the same vertex point
                            foundV0s.push_back(motherTrackID);
                            if(d0Track->GetPdgCode() == 11 || d0Track->GetPdgCode() == 13){
                                V0List->emplace_back(motherTrack->GetEnergy(), motherTrackID, motherTrack->getProcess(), d0Track->GetEnergy(), d0ID, d0Track->GetPdgCode(), d1Track->GetEnergy(), d1ID, d1Track->GetPdgCode());
                                if(d0Track->GetPdgCode() == 11){
                                    ev0counter++;
                                } else {
                                    mv0counter++;
                                }
                            } else {
                                V0List->emplace_back(motherTrack->GetEnergy(), motherTrackID, motherTrack->getProcess(), d1Track->GetEnergy(), d1ID, d1Track->GetPdgCode(), d0Track->GetEnergy(), d0ID, d0Track->GetPdgCode());
                                if(d0Track->GetPdgCode() == -11){
                                    ev0counter++;
                                } else {
                                    mv0counter++;
                                }
                            }
                            if(verbose){
                                printf("V0Energy mTrackID mTraProc d0TrackE d0TrackID d0TrackPDG d1TrackE d1TrackID d1TrackPDG\n");
                                printf("%8.2f %8i %8i %8.2f %9i %10i %8.2f %9i %10i\n", motherTrack->GetEnergy(), motherTrackID, motherTrack->getProcess(), d0Track->GetEnergy(), d0ID, d0Track->GetPdgCode(), d1Track->GetEnergy(), d1ID, d1Track->GetPdgCode());
                            }
                            // Set position vector
							V0List->back().SetPosVec(d0Vx, d0Vy, d0Vz);
                            // Set momentum vector
                            TVector3 mom;
                            motherTrack->GetMomentum(mom);
                            V0List->back().SetMomVec(mom);
                            V0List->back().SetPhotonPt(TMath::Sqrt( mom.X() * mom.X() + mom.Y() * mom.Y()));
                            // Calculate pointing angle
                            V0List->back().CalculatePA();
                            // Set number of hits of the l+ and l- track if both tracks can be found in the FCT
                            XXXTrack otherTrack;
                            Bool_t otherInFCT = false;
                            for(auto& checkTrack : fctTracks){
                                if(checkTrack.getTrackID() != thisTrackID){
                                    MCTrackT<float>* otherTrueTrack = &(*mcTr).at(checkTrack.getTrackID());
                                    if(otherTrueTrack->getMotherTrackId() == motherTrackID){
                                        otherInFCT = true;
                                        V0List->back().SetBothInFCT(true);
                                        otherTrack = checkTrack;
                                        break;
                                    }
                                }
                            }

                            Int_t thisTrackHits = 0;
                            Int_t otherTrackHits = 0;
                            thisTrackHits = fctTrack.getNXXXHits();
                            otherTrackHits = otherTrack.getNXXXHits();
                            if(pdgTrueTrack == 11 || pdgTrueTrack == 13){
                                V0List->back().SetNenHits(thisTrackHits);
                                V0List->back().SetNepHits(otherTrackHits);
                            } else {
                                V0List->back().SetNenHits(otherTrackHits);
                                V0List->back().SetNepHits(thisTrackHits);
                            }

                            if(!otherInFCT) {
                                V0List->back().SetBothInFCT(false);
                            }

                            if(verbose){
                                printf("V0PosX V0PosY  V0PosZ V0MomX V0MomY V0MomZ BothInFCT\n");
                                if(otherInFCT){
                                    printf("%6.2f %6.2f %7.2f %6.2f %6.2f %6.2f %9s\n", d0Vx, d0Vy, d0Vz, mom.X(), mom.Y(), mom.Z(), "True");
                                } else {
                                    printf("%6.2f %6.2f %7.2f %6.2f %6.2f %6.2f %9s\n", d0Vx, d0Vy, d0Vz, mom.X(), mom.Y(), mom.Z(), "False");
                                }
                                if(pdgTrueTrack == 11 || pdgTrueTrack == 13){
                                    printf("PointingA ElecHits PosiHits\n");
                                    printf("%9f %8i %8i\n", V0List->back().GetPointingAngle(), thisTrackHits, otherTrackHits);
                                } else {
                                    printf("PointingA ElecHits PosiHits\n");
                                    printf("%9f %8i %8i\n", V0List->back().GetPointingAngle(), otherTrackHits, thisTrackHits);                                
                                }
                                printf("HND0 HitPosX HitPosY HitPosZ\n");
                                if(d0ID == thisTrackID){
                                    std::vector<Float_t> xd0 = fctTrack.getXCoordinates();
                                    std::vector<Float_t> yd0 = fctTrack.getYCoordinates();
                                    std::vector<Float_t> zd0 = fctTrack.getZCoordinates();

                                    for(int i = 0; i < thisTrackHits; i++){
                                        printf("%4i %7.2f %7.2f %7.2f\n", i, xd0[i], yd0[i], zd0[i]);
                                    }
                                    std::vector<Float_t> xd1 = otherTrack.getXCoordinates();
                                    std::vector<Float_t> yd1 = otherTrack.getYCoordinates();
                                    std::vector<Float_t> zd1 = otherTrack.getZCoordinates();
                                    printf("HND1 HitPosX HitPosY HitPosZ\n");
                                    for(int i = 0; i < otherTrackHits; i++){
                                        printf("%4i %7.2f %7.2f %7.2f\n", i, xd1[i], yd1[i], zd1[i]);
                                    }
                                } else {
                                    std::vector<Float_t> xd0 = otherTrack.getXCoordinates();
                                    std::vector<Float_t> yd0 = otherTrack.getYCoordinates();
                                    std::vector<Float_t> zd0 = otherTrack.getZCoordinates();                                
                                    for(int i = 0; i < otherTrackHits; i++){
                                        printf("%4i %7.2f %7.2f %7.2f\n", i, xd0[i], yd0[i], zd0[i]);
                                    }
                                    std::vector<Float_t> xd1 = fctTrack.getXCoordinates();
                                    std::vector<Float_t> yd1 = fctTrack.getYCoordinates();
                                    std::vector<Float_t> zd1 = fctTrack.getZCoordinates();
                                    printf("HND1 HitPosX HitPosY HitPosZ\n");
                                    for(int i = 0; i < thisTrackHits; i++){
                                        printf("%4i %7.2f %7.2f %7.2f\n", i, xd1[i], yd1[i], zd1[i]);
                                    }
                                }
                                printf("\n");
                            }
						}
					}
				}
			}
		}
        V0Tree->Fill();
	}
    V0Tree->Write();
	
	return 0;
}

bool is_close(Double_t vertexCord0, Double_t vertexCord1, Double_t check_range){
    if(vertexCord0 + check_range > vertexCord0 && vertexCord0 - check_range < vertexCord1){
        return true;
    } else {
        return false;
    }
}

