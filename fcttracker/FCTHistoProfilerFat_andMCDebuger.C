#include <TFile.h>
#include <TH3.h>
#include <TCanvas.h>
#include <TH2.h>

#include "TEfficiency.h"
#include "TFile.h"
#include "TLatex.h"
#include "TROOT.h"
#include "TTree.h"
#include <TGeoGlobalMagField.h>
#include <TGraph.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TH3F.h>
#include <TMath.h>
#include <TProfile.h>
#include <TStyle.h>
#include <TProfile2D.h>
#include <TMultiGraph.h>

#include "FAT/FWDFat.C"

double etaWindow = 0.1;
double ptWindow = 0.2;

//FAT configuration
float etamin = 3.8;
float etamax = 2.6;
int nEtaPoints = 10;
float ptmin = 0.1;
float ptmax = 20;
int nPtPoints = 200;
bool FAT_ENABLED = true;

// Estimages pt and vertexing resolution from TH3 histograms produced by FCTTrackerChecker
//
//_________________________________________________________________________________________
void FCTHistoProfilerFat_andMCDebuger(float sigma = 8.44e-4) // ; //1e-6;//  8.44e-4
{

  gStyle->SetHistLineWidth(3);
  gStyle->SetFrameLineWidth(3);
  gStyle->SetLineWidth(3);
  gStyle->SetPalette(kRainBow);

  // std::vector<double> etaList({3.7, 3.4, 3.1, 2.8});
  // std::vector<double> etaListFAT({3.7 + 0.5 * etaWindow, 3.4 + 0.5 * etaWindow, 3.1 + 0.5 * etaWindow, 2.8 + 0.5 * etaWindow});
  // std::vector<double> ptList({0.2, 1., 4., 9.});
  // std::vector<double> ptListFAT({0.2 + 0.5 * ptWindow, 1. + 0.5 * ptWindow, 4. + 0.5 * ptWindow, 9. + 0.5 * ptWindow});

  std::vector<double> etaList({3.6, 3.2, 2.7});
  std::vector<double> etaListFAT({3.6 + 0.5 * etaWindow, 3.2 + 0.5 * etaWindow, 2.7 + 0.5 * etaWindow});
  std::vector<double> ptList({0.2, 1.0, 9., 20});
  std::vector<double> ptListFAT({0.2 + 0.5 * ptWindow, 1.0 + 0.5 * ptWindow, 9. + 0.5 * ptWindow, 20. + 0.5 * ptWindow});

  // Tracks with 7 hits minium
  bool Min7Hits = false;
  if (Min7Hits) {
    etaList = {4., 3.5, 2.75, 2.};
    etaListFAT = {4. + 0.5 * etaWindow, 3.5 + 0.5 * etaWindow, 2.75 + 0.5 * etaWindow, 2. + 0.5 * etaWindow};
    etamin = 4.;
    etamax = 2.0;
  }

  std::vector<float> etas;
  for (auto i = 0; i < nEtaPoints; i++) {
    etas.emplace_back(i * (etamax - etamin) / nEtaPoints + etamin);
  }

  std::vector<float> pts;
  for (auto i = 0; i < nPtPoints; i++) {
    pts.emplace_back(i * (ptmax - ptmin) / nPtPoints + ptmin);
  }

  TFile* chkFileIn = new TFile("Fittercheck_fcttracks.root");
  TFile* debugFileIn = new TFile("fwdtrackdebugger.root");
  TFile* profilerFileOut = new TFile("FCTProfiles.root", "RECREATE");

  // FAT pt resolution vs. pt
  auto CPtResInvPt = new TCanvas("PtResVsPt", "PtResVsPt", 1080, 1080);
  TMultiGraph* ptResGraphs = new TMultiGraph("FCTPtRes", "FCT q/Pt Resolution");

  for (auto eta : etaListFAT) {

    auto ptres = getFATPtRes_pts_at_eta(pts, eta, sigma);
    TGraph* ptResolution = new TGraph(pts.size(), &pts[0], &ptres[0]);
    ptResolution->SetTitle(Form("\\text{FAT: }\\eta = %1.3f", eta));
    ptResolution->SetDrawOption("A SAME PLC PMC");
    ptResGraphs->Add(ptResolution);
  }
  ptResGraphs->GetYaxis()->SetTitle("(q/pt) resolution");
  ptResGraphs->GetXaxis()->SetTitle("p_t (GeV/c)");
  ptResGraphs->Draw("A PLC");

  // FAT: no MCS
  if (sigma > 1.e-4)
    for (auto eta : etaListFAT) {

      auto ptres = getFATPtRes_pts_at_eta(pts, eta, sigma, false);
      TGraph* ptResolution = new TGraph(pts.size(), &pts[0], &ptres[0]);
      ptResolution->SetTitle(Form("\\text{FAT: } \\eta = %1.3f \\text{ (w.o. MCS)}", eta));
      ptResolution->SetDrawOption("A SAME PMC");
      ptResolution->SetLineStyle(kDashed);
      ptResGraphs->Add(ptResolution);
    }

  // InvPtResolution vs pt full simulation
  auto FCTTrackInvQPtResolutionPtEta = (TH3F*)chkFileIn->Get("MoreHistos/FCTTrackInvQPtResolutionPtEta");
  int marker = kFullCircle;

  for (auto etamin : etaList) {

    auto etamax = etamin + etaWindow;

    FCTTrackInvQPtResolutionPtEta->GetYaxis()->SetRangeUser(etamin, etamax);

    auto title = Form("InvQPtRes_%1.1f_%1.1f_xz", etamin, etamax);
    auto a = (TH2F*)FCTTrackInvQPtResolutionPtEta->Project3D(title);
    a->GetXaxis()->SetRangeUser(-20, 20);

    a->FitSlicesX(0, 0, -1, 1);
    auto th2InvPtResolution = (TH1F*)gDirectory->Get((std::string(a->GetName()) + std::string("_2")).c_str());
    th2InvPtResolution->SetTitle(Form("\\text{O2Sim: } %1.2f < \\eta < %1.2f", etamin, etamax));
    th2InvPtResolution->SetMarkerStyle(marker++);
    th2InvPtResolution->SetStats(0);
    th2InvPtResolution->Draw("PLC PMC same");
    th2InvPtResolution->Write();
  }

  // InvPtResolution vs pt MC Debuger
  auto FCTTrackInvQPtResolutionPtEtaDBG = (TH3F*)debugFileIn->Get("MoreHistos/FCTDBGTrackInvQPtResolutionPtEta");
  FCTTrackInvQPtResolutionPtEtaDBG->GetXaxis()->SetRange(0, 0);
  marker = kFullCircle;
  if (sigma > 5.e-4)
    for (auto etamin : etaList) {
      auto etamax = etamin + etaWindow;

      FCTTrackInvQPtResolutionPtEtaDBG->GetYaxis()->SetRangeUser(etamin, etamax);

      auto title = Form("__%1.2f_%1.2f_xz", etamin, etamax);
      auto aDBG = (TH2F*)FCTTrackInvQPtResolutionPtEtaDBG->Project3D(title);
      aDBG->GetXaxis()->SetRange(0, 0);

      aDBG->FitSlicesX(0, 0, -1, 1);
      auto th2InvPtResolutionEtaDBG = (TH1F*)gDirectory->Get((std::string(aDBG->GetName()) + std::string("_2")).c_str());
      th2InvPtResolutionEtaDBG->SetTitle(Form("\\text{MC_DBG: }%1.2f < \\eta < %1.2f \\text{ (w.o.MCS)}", etamin, etamax));
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

  // FAT pt resolution vs. eta
  auto CPtResInvPtEta = new TCanvas("PtResVsEta", "PtResVsEta", 1080, 1080);
  TMultiGraph* ptResGraphsEta = new TMultiGraph("FCTPtRes", "FCT q/Pt Resolution");

  for (auto pt : ptListFAT) {
    auto ptres = getFATPtRes_etas_at_pt(etas, pt, sigma);
    TGraph* ptResolution = new TGraph(etas.size(), &etas[0], &ptres[0]);
    ptResolution->SetTitle(Form("\\text{FAT: } pt = %1.2f", pt));
    ptResolution->SetDrawOption("A SAME PLC PMC");
    ptResGraphsEta->Add(ptResolution);
  }
  ptResGraphsEta->GetYaxis()->SetTitle("(q/pt) resolution");
  ptResGraphsEta->GetXaxis()->SetTitle("\\eta");
  ptResGraphsEta->Draw("A PLC");
  ptResGraphsEta->SetMinimum(0);

  // FAT: no MCS
  if (sigma > 5.e-4)
    for (auto pt : ptListFAT) {

      auto ptres = getFATPtRes_etas_at_pt(etas, pt, sigma, false);
      TGraph* ptResolution = new TGraph(etas.size(), &etas[0], &ptres[0]);
      ptResolution->SetTitle(Form("\\text{FAT: } p_t = %1.2f (w.o. MCS)", pt));
      ptResolution->SetDrawOption("A SAME PMC");
      ptResolution->SetLineStyle(kDashed);
      ptResGraphsEta->Add(ptResolution);
    }

  // InvPtResolution vs eta Full Simulation
  FCTTrackInvQPtResolutionPtEta = (TH3F*)chkFileIn->Get("MoreHistos/FCTTrackInvQPtResolutionPtEta");
  FCTTrackInvQPtResolutionPtEta->GetYaxis()->SetRange(0, 0);

  marker = kFullCircle;
  for (auto ptmin : ptList) {
    auto ptmax = ptmin + ptWindow;

    FCTTrackInvQPtResolutionPtEta->GetXaxis()->SetRangeUser(ptmin, ptmax);

    auto title = Form("InvQPtResEta_%1.1f_%1.1f_yz", ptmin, ptmax);
    auto a = (TH2F*)FCTTrackInvQPtResolutionPtEta->Project3D(title);
    a->GetXaxis()->SetRangeUser(-1, 10);

    a->FitSlicesX(0, 0, -1, 1);
    auto th2InvPtResolutionEta = (TH1F*)gDirectory->Get((std::string(a->GetName()) + std::string("_2")).c_str());
    th2InvPtResolutionEta->SetTitle(Form("O2Sim: %1.2f < p_t < %1.2f", ptmin, ptmax));
    th2InvPtResolutionEta->SetMarkerStyle(marker++);
    th2InvPtResolutionEta->SetStats(0);
    th2InvPtResolutionEta->Draw("PLC PMC same");
    th2InvPtResolutionEta->Write();
  }

  // InvPtResolution vs eta MC Debuger
  FCTTrackInvQPtResolutionPtEtaDBG = (TH3F*)debugFileIn->Get("MoreHistos/FCTDBGTrackInvQPtResolutionPtEta");
  FCTTrackInvQPtResolutionPtEtaDBG->GetYaxis()->SetRange(0, 0);

  marker = kFullCircle;
  if (sigma > 5.e-4)
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

  { // FAT: Line at infinite sensor position resolution
    auto pt = 1.0;

    auto ptres = getFATPtRes_etas_at_pt(etas, pt, 1e-5);
    TGraph* ptResolution = new TGraph(etas.size(), &etas[0], &ptres[0]);
    ptResolution->SetTitle(Form("FAT: MCS only"));
    ptResolution->SetDrawOption("SAME");
    ptResolution->SetLineColor(kBlack);
    ptResolution->SetLineStyle(kDotted);
    ptResolution->Draw("same");
  }
  CPtResInvPtEta->SetTicky();
  CPtResInvPtEta->SetGridy();
  CPtResInvPtEta->BuildLegend();
  CPtResInvPtEta->Write();
  CPtResInvPtEta->Print("PtResVsEta.png");
  //profilerFileOut->Close();

  // FAT X vertexing resolution vs. eta
  auto CPtResVertEta = new TCanvas();
  if (FAT_ENABLED) {
    TMultiGraph* vtxXResGraphsEta = new TMultiGraph("PtRes", "PtRes");

    for (auto pt : ptListFAT) {

      auto ptres = getFATvtxXRes_etas_at_pt(etas, pt, sigma);
      TGraph* ptResolution = new TGraph(etas.size(), &etas[0], &ptres[0]);

      ptResolution->SetTitle(Form("\\text{FAT: } pt = %1.2f", pt));
      ptResolution->SetDrawOption("A SAME PLC PMC");

      vtxXResGraphsEta->Add(ptResolution);
    }
    vtxXResGraphsEta->GetYaxis()->SetTitle("\\sigma_x \\text{ @ vertex} (\\mu m) ");
    vtxXResGraphsEta->GetXaxis()->SetTitle("\\eta");
    vtxXResGraphsEta->Draw("A PLC");
  }

  /*
  // Vertexing resolution vs eta
  auto FCTTrackDeltaXVertexPtEta = (TH3F*)chkFileIn->Get("MoreHistos/FCTTrackDeltaXVertexPtEta");
  FCTTrackDeltaXVertexPtEta->GetYaxis()->SetRange(0, 0);

  FCTTrackDeltaXVertexPtEta->RebinY(2);

  marker = kFullCircle;
  for (auto ptmin : ptList) {

    auto ptmax = ptmin + ptWindow;

    FCTTrackDeltaXVertexPtEta->GetXaxis()->SetRangeUser(ptmin, ptmax);

    auto title = Form("VertXResEta_%1.1f_%1.1f_yz", ptmin, ptmax);
    auto a = (TH2F*)FCTTrackDeltaXVertexPtEta->Project3D(title);
    a->GetXaxis()->SetRangeUser(-1, 10);
    a->FitSlicesX(0, 0, -1, 1);
    auto th2VertEta = (TH1F*)gDirectory->Get((std::string(a->GetName()) + std::string("_2")).c_str());
    th2VertEta->SetTitle(Form("%1.2f < p_t < %1.2f", ptmin, ptmax));
    th2VertEta->SetMarkerStyle(marker++);
    th2VertEta->SetStats(0);
    th2VertEta->Draw("PLC PMC same");
    th2VertEta->Write();
  }
  CPtResVertEta->BuildLegend();
  */
  auto CVertexResEta = new TCanvas();

  // FAT vertexing resolution vs. pt
  TMultiGraph* vtxXResGraphsPt = new TMultiGraph("VtxXRes", "VtxXRes");
  if (FAT_ENABLED) {

    for (auto eta : etaListFAT) {
      std::vector<float> pts;

      for (auto i = 0; i < nPtPoints; i++) {
        pts.emplace_back(i * (ptmax - ptmin) / nPtPoints + ptmin);
      }

      auto ptres = getFATvtxXRes_pts_at_eta(pts, eta, sigma);
      TGraph* ptResolution = new TGraph(pts.size(), &pts[0], &ptres[0]);
      ptResolution->SetTitle(Form("\\text{FAT: } \\eta = %1.3f", eta));
      ptResolution->SetDrawOption("A SAME PLC PMC");
      vtxXResGraphsPt->Add(ptResolution);
    }
    vtxXResGraphsPt->GetYaxis()->SetTitle("\\sigma_x \\text{ @ Vertex } (\\mu m)");
    vtxXResGraphsPt->GetXaxis()->SetTitle("p_t (GeV/c)");
    vtxXResGraphsPt->Draw("A PLC");
  }

  // Vertexing resolution vs pt
  auto FCTTrackDeltaXVertexPtEta = (TH3F*)chkFileIn->Get("MoreHistos/FCTTrackDeltaXVertexPtEta");
  FCTTrackDeltaXVertexPtEta->GetYaxis()->SetRange(0, 0);
  FCTTrackDeltaXVertexPtEta->GetXaxis()->SetRange(0, 0);

  marker = kFullCircle;
  for (auto etamin : etaList) { // 10 Hits
    auto etamax = etamin + etaWindow;
    FCTTrackDeltaXVertexPtEta->GetYaxis()->SetRangeUser(etamin, etamax);
    auto title = Form("VertResPt_%1.1f_%1.1f_xz", etamin, etamax);
    auto a = (TH2F*)FCTTrackDeltaXVertexPtEta->Project3D(title);
    a->SetTitle(title);
    a->FitSlicesX(0, 0, -1, 1);
    auto th2VertResolutionPt = (TH1F*)gDirectory->Get((std::string(a->GetName()) + std::string("_2")).c_str());
    th2VertResolutionPt->SetTitle(Form("%1.3f < \\eta < %1.3f", etamin, etamax));
    th2VertResolutionPt->SetMarkerStyle(marker++);
    th2VertResolutionPt->SetStats(0);
    th2VertResolutionPt->Draw("PLC PMC same");
    th2VertResolutionPt->Write();
  }
  CVertexResEta->BuildLegend();
}
