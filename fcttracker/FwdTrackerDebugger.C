#if !defined(__CLING__) || defined(__ROOTCLING__)

#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TH3F.h"
#include "TCanvas.h"
#include "TRandom3.h"
#include "TSystem.h"
#include "MFTTracking/Cluster.h"
#include "MFTTracking/MFTTrackingParam.h"
#include "ITSMFTSimulation/Hit.h"
#include "Math/SMatrix.h"
#endif

#include "FCTTrack.h"
#include "TrackFitter.h"

#pragma link C++ class o2::fct::FCTTrack + ;

using SMatrix55Sym = ROOT::Math::SMatrix<double, 5, 5, ROOT::Math::MatRepSym<double, 5>>;
using o2::fct::FCTTrack;

enum TH3HistosCodes {
  kFCTTrackDeltaXVertexPtEta = 0,
  kFCTTrackDeltaYVertexPtEta = 1,
  kFCTTrackInvQPtResolutionPtEta = 2,
  kFCTTrackInvQPtPullPtEta = 3
};

bool DEBUG_VERBOSE = !true;
bool DEBUG_qpt = !true;
bool DEBUG_fitter = !true;

Float_t EtaToTheta(Float_t arg);
Float_t EtaToTanl(Float_t arg);
std::vector<float_t> loadx2X0fromFile(std::string configFileName);

void DrawTrees(const char* treefile = "fwdtrackdebugger.root");
void printCanvas(TCanvas* c, const char* filename);
void exportHisto(TH2F* histo, const char* filename);
void exportHisto(TH1F* histo, const char* filename);
void setSeedCovariances(FCTTrack& track);

void simFwdTracks(size_t nTracks, float ptMinCut, float ptMax, float etaMin, float etaMax, float zField, std::vector<float> zPositionsVec, std::vector<float> layResVec, TTree* trkDBGTree, std::vector<std::unique_ptr<TH3F>>& TH3Histos);
void simFCTTracks(size_t nTracks, float ptMinCut, float ptMax, float etaMin, float etaMax, float zField, TTree* trkDBGTree, std::vector<std::unique_ptr<TH3F>>& TH3Histos);

o2::fct::TrackFitter fitter;
o2::track::TrackParFwd MCTrack_;
o2::mft::TrackMFT probe;
TRandom3 rnd(0);
float pixelPitch;
float clSigma;
bool enableClusterSmearing;

void FwdTrackerDebugger(size_t nTracks = 1000,
                        float zField = -5.0,
                        float clSigma_ = 8.44e-4, //~( o2::itsmft::SegmentationAlpide::PitchCol / std::sqrt(12))
                        bool enableClusterSmearing_ = true,
                        float pixelPitch_ = 0.003, // o2::itsmft::SegmentationAlpide::PitchCol = 0.00292400f
                        const char* treeFile = "fwdtrackdebugger.root")
{
  pixelPitch = pixelPitch_;
  clSigma = clSigma_;
  enableClusterSmearing = enableClusterSmearing_;

  Double_t ptMax = 21.0;
  Double_t ptMin = 0.1;
  Double_t etaMin = -3.8;
  Double_t etaMax = -0.85;
  auto ptmincut = 0.0001;
  fitter.setBz(zField);
  fitter.setLayersx2X0(std::vector<double>(10, 0.0)); // Disable MCS effects
  fitter.mVerbose = DEBUG_fitter;
  //fitter.setTrackModel(o2::mft::MFTTrackModel::Helix);
  //fitter.setTrackModel(o2::mft::MFTTrackModel::Quadratic);

  auto fT = TFile::Open(treeFile, "RECREATE");
  TTree* trkDBGTree = new TTree("fwdTrackDBG", "FwDTrackDBGTree");
  trkDBGTree->Branch("MCTrack", &MCTrack_);
  trkDBGTree->Branch("probe", &probe);

  auto etaInner = -3.6;
  auto etaCenter = -3.06;
  auto etaOuter = -2.45;
  auto etaTransition = -0.881;

  std::vector<std::unique_ptr<TH3F>> TH3Histos;

  std::map<int, const char*> TH3Names{
    {kFCTTrackDeltaXVertexPtEta, "FCTDBGTrackDeltaXVertexPtEta"},
    {kFCTTrackDeltaYVertexPtEta, "FCTDBGTrackDeltaYVertexPtEta"},
    {kFCTTrackInvQPtResolutionPtEta, "FCTDBGTrackInvQPtResolutionPtEta"},
    {kFCTTrackInvQPtPullPtEta, "FCTDBGTrackInvQPtPullPtEta"}};

  std::map<int, const char*> TH3Titles{
    {kFCTTrackDeltaXVertexPtEta, "FCTDBGTrackDeltaXVertexPtEta"},
    {kFCTTrackDeltaYVertexPtEta, "FCTDBGTrackDeltaYVertexPtEta"},
    {kFCTTrackInvQPtPullPtEta, "FCTDBGTrackInvQPtPullPtEta"},
    {kFCTTrackInvQPtResolutionPtEta, "FCTDBGTrackInvQPtResolutionPtEta"}};

  std::map<int, std::array<double, 9>> TH3Binning{
    {kFCTTrackDeltaXVertexPtEta, {20, 0, 10, 84, 1.7, 4.5, 500, -500, 500}},
    {kFCTTrackDeltaYVertexPtEta, {50, 0, 10, 84, 1.7, 4.5, 500, -500, 500}},
    {kFCTTrackInvQPtPullPtEta, {200, 0, 20, 84, 1.7, 4.5, 200, -5, 5}},
    {kFCTTrackInvQPtResolutionPtEta, {200, 0, 20, 84, 1.7, 4.5, 100, -1, 1}}};

  std::map<int, const char*> TH3XaxisTitles{
    {kFCTTrackDeltaXVertexPtEta, "p_t"},
    {kFCTTrackDeltaYVertexPtEta, "p_t"},
    {kFCTTrackInvQPtPullPtEta, "p_t"},
    {kFCTTrackInvQPtResolutionPtEta, "p_t"}};

  std::map<int, const char*> TH3YaxisTitles{
    {kFCTTrackDeltaXVertexPtEta, "\\eta"},
    {kFCTTrackDeltaYVertexPtEta, "\\eta"},
    {kFCTTrackInvQPtPullPtEta, "\\eta"},
    {kFCTTrackInvQPtResolutionPtEta, "\\eta"}};

  std::map<int, const char*> TH3ZaxisTitles{
    {kFCTTrackDeltaXVertexPtEta, "X residual at vertex (um)"},
    {kFCTTrackDeltaYVertexPtEta, "Y residual at vertex (um)"},
    {kFCTTrackInvQPtPullPtEta, "(\\Delta q/p_t)/\\sigma_{q/pt}"},
    {kFCTTrackInvQPtResolutionPtEta, "(q/p_t residual)/(q/pt)"}};

  const int nTH3Histos = TH3Names.size();

  std::cout << " nTH3Histos = " << nTH3Histos << std::endl;
  //for (auto& h : TH3Histos) {
  for (auto n3Histo = 0; n3Histo < 4; n3Histo++) {

    TH3Histos.emplace_back(std::make_unique<TH3F>(TH3Names[n3Histo], TH3Titles[n3Histo],
                                                  (int)TH3Binning[n3Histo][0],
                                                  TH3Binning[n3Histo][1],
                                                  TH3Binning[n3Histo][2],
                                                  (int)TH3Binning[n3Histo][3],
                                                  TH3Binning[n3Histo][4],
                                                  TH3Binning[n3Histo][5],
                                                  (int)TH3Binning[n3Histo][6],
                                                  TH3Binning[n3Histo][7],
                                                  TH3Binning[n3Histo][8]));
  }

  simFCTTracks(nTracks, ptmincut, ptMax, etaMin, etaMax, zField, trkDBGTree, TH3Histos);

  fT->mkdir("MoreHistos");
  fT->cd("MoreHistos");

  for (auto& h : TH3Histos) {
    h->Write();
  }

  fT->Write();
}

Float_t EtaToTheta(Float_t arg)
{
  return (180. / TMath::Pi()) * 2. * atan(exp(-arg));
}

Float_t EtaToTanl(Float_t eta)
{
  return tan(TMath::Pi() / 2 - 2 * atan(exp(-eta)));
}

void simFCTTracks(size_t nTracks, float ptMinCut, float ptMax, float etaMin, float etaMax, float zField, TTree* trkDBGTree, std::vector<std::unique_ptr<TH3F>>& TH3Histos)
{
  std::vector<float> zPositionsVec{-16., -20., -24., -77., -100., -122., -150., -180., -220., -279.};
  std::vector<float> layResVec(zPositionsVec.size(), clSigma);
  simFwdTracks(nTracks, ptMinCut, ptMax, etaMin, etaMax, zField, zPositionsVec, layResVec, trkDBGTree, TH3Histos);
}

void simFwdTracks(size_t nTracks, float ptMinCut, float ptMax, float etaMin, float etaMax, float zField, std::vector<float> zPositionsVec, std::vector<float> layResVec, TTree* trkDBGTree, std::vector<std::unique_ptr<TH3F>>& TH3Histos)
{

  for (size_t i = 0; i < nTracks; i++) {
    if (i > 10)
      fitter.mVerbose = !true;
    o2::track::TrackParFwd MCTrack;
    MCTrack.setX(0);
    MCTrack.setY(0);
    MCTrack.setPhi(rnd.Uniform(-TMath::Pi(), TMath::Pi()));
    //MCTrack.setPhi(TMath::Pi() / 4);

    auto eta = rnd.Uniform(etaMin, etaMax);

    MCTrack.setTanl(EtaToTanl(eta));
    auto qpt = 0.0;
    while (1) {
      qpt = rnd.Uniform(-ptMax, ptMax);
      if (std::abs(qpt) > ptMinCut)
        break;
    }
    MCTrack.setInvQPt(1.0 / qpt);
    MCTrack_ = MCTrack;
    if (DEBUG_VERBOSE) {
      o2::track::TrackParFwd tempTrk = MCTrack;
      auto z = zPositionsVec.back();
      tempTrk.propagateParamToZhelix(z, zField);
      std::cout << "\n\n *** New gen track ***\n  MCTrack: X = " << MCTrack.getX() << " Y = " << MCTrack.getY() << " Z = " << MCTrack.getZ() << " Tgl = " << MCTrack.getTanl() << "  Phi = " << MCTrack.getPhi() << "  q/pt = " << MCTrack.getInvQPt() << std::endl;
      std::cout << "    MCTrack at last disk: X = " << tempTrk.getX() << " Y = " << tempTrk.getY() << " Z = " << tempTrk.getZ() << " Tgl = " << tempTrk.getTanl() << "  Phi = " << tempTrk.getPhi() << "  (" << o2::math_utils::toPMPiGen(tempTrk.getPhi()) << ") q/pt = " << tempTrk.getInvQPt() << std::endl;
    }
    FCTTrack fctProbeTr;
    int nLayer = 0;
    for (auto z : zPositionsVec) {
      MCTrack.propagateParamToZhelix(z, zField);
      //std::cout << " AddHit: MCTrack.getX() = " << MCTrack.getX() << " ; MCTrack.getY() =  " << MCTrack.getY() << "  MCTrack.getZ() = " << MCTrack.getZ() << std::endl;

      o2::itsmft::Hit hit(0, 0, {MCTrack.getX(), MCTrack.getY(), MCTrack.getZ()}, {0, 0, 0}, {0, 0, 0}, 0, 0, 0, 0, 0);
      fctProbeTr.addHit(hit, nLayer, clSigma);
      nLayer++;
    }
    //std::cout << std::endl;
    //if (DEBUG_VERBOSE)
    // std::cout << std::endl;
    fctProbeTr.sort();

    fitter.initTrack(fctProbeTr);
    setSeedCovariances(fctProbeTr);
    //fitter.MinuitFit(fctProbeTr);
    fitter.fit(fctProbeTr);
    if (DEBUG_qpt)
      std::cout << "Track " << i << ": \n    q/pt_MC = " << MCTrack.getInvQPt() << "\n    q/pt_Seed = " << fctProbeTr.getInvQPtSeed() << "\n    q/pt_fit = " << fctProbeTr.getInvQPt() << std::endl;
    fctProbeTr.propagateToZhelix(0.0, zField);
    if (DEBUG_VERBOSE)
      std::cout << "  Track at vertex: X = " << fctProbeTr.getX() << " Y = " << fctProbeTr.getY() << " Z = " << fctProbeTr.getZ() << " Tgl = " << fctProbeTr.getTanl() << "  Phi = " << fctProbeTr.getPhi() << "  (" << o2::math_utils::toPMPiGen(fctProbeTr.getPhi()) << ")  q/pt = " << fctProbeTr.getInvQPt() << std::endl;

    probe = (o2::mft::TrackMFT)fctProbeTr;
    trkDBGTree->Fill();

    auto vx_MC = MCTrack_.getX();
    auto vy_MC = MCTrack_.getY();
    auto invQPt_MC = MCTrack_.getInvQPt();
    auto Pt_MC = MCTrack_.getPt();
    auto eta_MC = MCTrack_.getEta();

    auto dx = fctProbeTr.getX() - vx_MC;
    auto dy = fctProbeTr.getY() - vy_MC;
    auto invQPt_Fit = fctProbeTr.getInvQPt();
    auto d_invQPt = invQPt_Fit - invQPt_MC;

    TH3Histos[kFCTTrackDeltaXVertexPtEta]->Fill(Pt_MC, std::abs(eta_MC), 1e4 * dx);
    TH3Histos[kFCTTrackDeltaYVertexPtEta]->Fill(Pt_MC, std::abs(eta_MC), 1e4 * dy);
    TH3Histos[kFCTTrackInvQPtPullPtEta]->Fill(Pt_MC, std::abs(eta_MC), d_invQPt / sqrt(fctProbeTr.getCovariances()(4, 4)));
    TH3Histos[kFCTTrackInvQPtResolutionPtEta]->Fill(Pt_MC, std::abs(eta_MC), (invQPt_Fit - invQPt_MC) / invQPt_MC);
  }

  // InvPtResolution vs eta MC Debuger
  auto CPtResInvPt = new TCanvas("PtResVsPt", "PtResVsPt", 1080, 1080);

  auto& FCTTrackInvQPtResolutionPtEtaDBG = TH3Histos[kFCTTrackInvQPtPullPtEta];
  FCTTrackInvQPtResolutionPtEtaDBG->GetYaxis()->SetRange(0, 0);

  int marker = kFullCircle;
  std::vector<double> ptList({1, 9.});
  double ptWindow = 1.2;

  for (auto ptmin : ptList) {
    auto ptmax = ptmin + ptWindow;

    FCTTrackInvQPtResolutionPtEtaDBG->GetXaxis()->SetRangeUser(ptmin, ptmax);

    auto title = Form("_%1.2f_%1.2f_yz", ptmin, ptmax);
    auto aDBG = (TH2F*)FCTTrackInvQPtResolutionPtEtaDBG->Project3D(title);
    aDBG->GetXaxis()->SetRangeUser(0, 0);

    aDBG->FitSlicesX(0, 0, -1, 1);
    auto th2InvPtResolutionEtaDBG = (TH1F*)gDirectory->Get((std::string(aDBG->GetName()) + std::string("_2")).c_str());
    th2InvPtResolutionEtaDBG->SetTitle(Form("MC_DBG: %1.2f < p_t < %1.2f (w.o.MCS)", ptmin, ptmax));
    th2InvPtResolutionEtaDBG->SetMarkerStyle(marker++);
    th2InvPtResolutionEtaDBG->SetLineStyle(kDashed);
    th2InvPtResolutionEtaDBG->SetStats(0);
    th2InvPtResolutionEtaDBG->Draw("PLC PMC same");
  }
  CPtResInvPt->BuildLegend();
  CPtResInvPt->SetTicky();
  CPtResInvPt->SetGridy();
  CPtResInvPt->Write();
  CPtResInvPt->Print("PtResVsPt.png");
}

void setSeedCovariances(FCTTrack& track)
{

  auto tan_k = 10.0;
  auto q2pt_c = 10.;
  auto phi_c = 1 / 16.;

  SMatrix55Sym Covariances{}; ///< \brief Covariance matrix of track parameters
  float q2ptcov = TMath::Max(std::abs(track.getInvQPt()), .5);
  float tanlerr = TMath::Max(std::abs(track.getTanl()), .5);

  Covariances(0, 0) = 1;
  Covariances(1, 1) = 1;
  Covariances(2, 2) = phi_c * TMath::Pi() * TMath::Pi();
  Covariances(3, 3) = tan_k * tanlerr * tanlerr;
  Covariances(4, 4) = q2pt_c * q2ptcov * q2ptcov;
  track.setCovariances(Covariances);
}

void printCanvas(TCanvas* c, const char* filename)
{
  //gStyle->SetImageScaling(3.);
  c->SetBatch();
  gSystem->ProcessEvents();
  c->Update();
  c->Print(filename);
}

void exportHisto(TH2F* histo, const char* filename)
{
  TCanvas* C1_ = new TCanvas(filename, filename, 1080, 1080);
  histo->Draw("colz");
  gSystem->ProcessEvents();
  C1_->Update();
  C1_->Print(filename);
}

void exportHisto(TH1F* histo, const char* filename)
{
  TCanvas* C1_ = new TCanvas(filename, filename, 1080, 1080);
  histo->Draw("");
  gSystem->ProcessEvents();
  C1_->Update();
  C1_->Print(filename);
}

std::vector<float_t> loadx2X0fromFile(std::string configFileName = "FCT_layout.cfg")
{
  std::vector<float_t> Layersx2X0;
  std::ifstream ifs(configFileName.c_str());
  if (!ifs.good()) {
    LOG(FATAL) << " Invalid FCTBase.configFile!";
  }
  std::string tempstr;
  float z_layer, r_in, r_out, Layerx2X0;
  char delimiter;
  int layerNumber = 0;
  while (std::getline(ifs, tempstr)) {
    if (tempstr[0] == '#') {
      LOG(INFO) << " Comment: " << tempstr;
      continue;
    }
    LOG(INFO) << " Line: " << tempstr;
    std::istringstream iss(tempstr);
    iss >> z_layer;
    iss >> r_in;
    iss >> r_out;
    iss >> Layerx2X0;

    Layersx2X0.push_back(Layerx2X0);
    LOG(INFO) << " loadx2X0fromFile z =  " << z_layer << " ; x/X0 = " << Layerx2X0 << std::endl;
  }
  return Layersx2X0;
}