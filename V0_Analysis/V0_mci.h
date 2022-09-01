// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.
///

#ifndef O2_V0_MCI_H_
#define O2_V0_MCI_H_

// V0 class for when MC information is provided.
// Now only contains basic info, but it is nice to have this together. Makes searching through them easier
// This class stores the TrackID of the particles involved in the V0 production.
// The TrackID is the same as that of the particle stack, so this can be used to loop through the V0's

#include "TVector3.h"

class V0_mci
{
    public:
        V0_mci() = default;
        V0_mci(Double_t photE, Int_t photId, Int_t photPP, Double_t lepNegE, Int_t lepNegID, Int_t lepNegPID, Double_t lepPosE, Int_t lepPosID, Int_t lepPosPID) : 
            mPhotonE(photE),
            mPhotonTrackID(photId),
            mPhotonProductionProcess(photPP),
            mLeptonNegE(lepNegE),
            mLeptonNegTrackID(lepNegID),
            mLeptonNegPID(lepNegPID),
            mLeptonPosE(lepPosE),
            mLeptonPosTrackID(lepPosID),
            mLeptonPosPID(lepPosPID) {
            // constructor body
        }
        ~V0_mci() = default;
        // Photon get and set
        void SetPhotonE(Double_t Eg) { mPhotonE = Eg; }
        void SetPhotonRecE(Double_t EgRec) { mPhotonRecE = EgRec; }
        void SetPhotonPt(Double_t pt) { mPt = pt; }
        void SetPhotonTrackID(Int_t PhotId) { mPhotonTrackID = PhotId; }
        void SetPhotonProductionProcess(Int_t PhotProd) { mPhotonProductionProcess = PhotProd; }
        Double_t GetPhotonE() { return mPhotonE; }
        Double_t GetPhotonRecE() { return mPhotonRecE; }
        Double_t GetPhotonPt() { return mPt; }
        Int_t GetPhotonTrackID() { return mPhotonTrackID; }
        Int_t GetPhotonProductionProcess() { return mPhotonProductionProcess; }

        // Lepton - get and set
        void SetLNegE(Double_t LNegE) { mLeptonNegE = LNegE; }
        void SetLNegTrackID(Int_t LNId) { mLeptonNegTrackID = LNId; }
        void SetLNegPID(Int_t LNPId) { mLeptonNegPID = LNPId;}
        Double_t GetLNegE() { return mLeptonNegE; }
        Int_t GetLNegPID() { return mLeptonNegPID; }
        Int_t GetLNegTrackID() { return mLeptonNegTrackID; }

        // Lepton + get and set
        void SetLPosE(Double_t LPosE) { mLeptonPosE = LPosE; }
        Double_t GetLPosE() { return mLeptonPosE; }
        void SetLPosTrackID(Int_t LPId) { mLeptonPosTrackID = LPId; }
        Int_t GetLPosTrackID() { return mLeptonPosTrackID; }
        void SetLPosPID(Int_t LPPId) { mLeptonPosPID = LPPId;}
        Int_t GetLPosPID() { return mLeptonPosPID; }

        // V0 get and set
        void SetPosVec(Double_t x, Double_t y, Double_t z){
            mPosVec.SetXYZ(x, y, z);
        }
        void SetPosVec(TVector3 loc){
            mPosVec.SetXYZ(loc.X(), loc.Y(), loc.Z());
        }
        void SetMomVec(TVector3 mom){
            mMomVec.SetXYZ(mom.X(), mom.Y(), mom.Z());     
        }
        void SetNepHits(Int_t nHits) { nepHits = nHits; }
        void SetNenHits(Int_t nHits) { nenHits = nHits; }
        void SetPointingAngle(Double_t pointA) { mPA = pointA; }
        void SetBothInFCT(Bool_t value) { mBothInFCT = value; }
        Double_t GetCX() { return mPosVec.X(); }
        Double_t GetCY() { return mPosVec.Y(); }
        Double_t GetCZ() { return mPosVec.Z(); }
        TVector3 GetPosVec() { return mPosVec; }
        TVector3 GetMomVec() { return mMomVec; }
        Int_t GetNepHits() { return nepHits; }
        Int_t GetNenHits() { return nenHits; }
        Double_t GetPointingAngle() { return mPA; }
        Bool_t GetBothInFCT() { return mBothInFCT; }

        // V0 functions
        void CalculatePA() { mPA = mMomVec.Angle(mPosVec); } // Calculate pointing angle


    private:
        // Photon properties
        Double_t mPhotonE; // GeV
        Double_t mPhotonRecE; // GeV
        Double_t mPt; // GeV
        Int_t mPhotonTrackID;
        Int_t mPhotonProductionProcess;
        

        // Lepton - properties
        Double_t mLeptonNegE; // GeV
        Int_t mLeptonNegTrackID;
        Int_t mLeptonNegPID;

        // Lepton + properties
        Double_t mLeptonPosE;
        Int_t mLeptonPosTrackID;
        Int_t mLeptonPosPID;

        // V0 properties
        Double_t mPA; // Pointing angle
        Int_t nepHits; // Number of positron hits in detector
        Int_t nenHits; // Number of electron hits in detector
        TVector3 mPosVec; // V0 position vector
        TVector3 mMomVec; // V0 momentum vector
        Bool_t mBothInFCT; // Both tracks of V0 are detected in the FCT


        // Same event properties (this is so we can cut on these variables). All V0's in the same event share these properties
        Bool_t V0_in_FT3 = false; // V0 in FT3 in same event?
        Bool_t prim_e_in_ev = false; // Primary e+ or e- in same event in direction of FCT?


};

#pragma link C++ class V0_mci + ;
#pragma link C++ class std::vector < V0_mci> + ;


#endif /* O2_V0_MCI_H_ */