#include "TH2F.h"
#include "TH1F.h"
#include "TFile.h"
#include "THStack.h"
#include "TLine.h"
#include "TCanvas.h"
#include "TLegend.h"

void matbudget_plot(){

    TFile *A3IP = new TFile("o2sim_matbudget_A3IP.root", "READ");
    TFile *AIR = new TFile("o2sim_matbudget_AIR.root", "READ");
    TFile *ALL = new TFile("o2sim_matbudget_ALL.root", "READ");
    TFile *FT3 = new TFile("o2sim_matbudget_FT3.root", "READ");
    TFile *TRK = new TFile("o2sim_matbudget_TRK.root", "READ");

    TH2F *hist_AIR = (TH2F*)AIR->Get("hradl");
    TH2F *hist_A3IP = (TH2F*)A3IP->Get("hradl");
    TH2F *hist_ALL = (TH2F*)ALL->Get("hradl");
    TH2F *hist_FT3 = (TH2F*)FT3->Get("hradl");
    TH2F *hist_TRK = (TH2F*)TRK->Get("hradl");

    TH1F *hist_AIR_1D = new TH1F("hist_AIR", "", 100, -5, -3);
    hist_AIR_1D->SetFillColor(kOrange-9);
    TH1F *hist_A3IP_1D = new TH1F("hist_A3IP", "", 100, -5, -3);
    hist_A3IP_1D->SetFillColor(kRed-9);
    TH1F *hist_ALL_1D = new TH1F("hist_ALL", "", 100, -5, -3);
    hist_ALL_1D->SetFillColor(kYellow-9);
    TH1F *hist_FT3_1D = new TH1F("hist_FT3", "", 100, -5, -3);
    hist_FT3_1D->SetFillColor(kBlue-9);
    TH1F *hist_TRK_1D = new TH1F("hist_TRK", "", 100, -5, -3);
    hist_TRK_1D->SetFillColor(kGreen-9);


    // Fill histograms
    int nbins = 100;
    double air_contr; // air contribution
    double all_evalmat;
    double all_sum;
    double ft3;
    double a3ip;
    double trk;
    for(int i = 0; i < nbins; i++){
        air_contr = hist_AIR->GetBinContent(i, 30);
        std::cout << "air: " << air_contr << std::endl;
        hist_AIR_1D->SetBinContent(i, air_contr);

        a3ip = hist_A3IP->GetBinContent(i, 30);
        std::cout << "a3ip: " << a3ip - air_contr << std::endl;
        hist_A3IP_1D->SetBinContent(i, a3ip - air_contr);

        ft3 = hist_FT3->GetBinContent(i, 30);
        std::cout << "ft3: " << ft3 - air_contr << std::endl;
        hist_FT3_1D->SetBinContent(i, ft3 - air_contr);

        trk = hist_TRK->GetBinContent(i, 30);
        std::cout << "trk: " << trk - air_contr << std::endl;
        hist_TRK_1D->SetBinContent(i, trk - air_contr);

        all_evalmat = hist_ALL->GetBinContent(i, 30);
        std::cout << "all: " << all_evalmat << std::endl;
        hist_ALL_1D->SetBinContent(i, all_evalmat);

        all_sum = - 2 * air_contr + a3ip + ft3 + trk;
        std::cout << "sum: " << all_sum << std::endl;

        std::cout << "" << std::endl;
    }

    THStack *hstack = new THStack("hstack", "Material budget;Pseudorapidity;Radiation length");
    // hstack->Add(hist_A3IP_1D);
    hstack->Add(hist_FT3_1D);
    // hstack->Add(hist_TRK_1D);
    // hstack->Add(hist_AIR_1D);
    // hstack->Add(hist_ALL_1D);
    hstack->SetMinimum(0.);
    hstack->SetMaximum(0.3);

    TLine *line = new TLine(-5, 0.14, -3, 0.14); 

    TLegend *leg = new TLegend(0.7, 0.7, 0.88, 0.88);
    leg->SetBorderSize(0);
    // leg->AddEntry(hist_A3IP_1D, "A3IP", "f");
    leg->AddEntry(hist_FT3_1D, "FT3", "f");
    // leg->AddEntry(hist_TRK_1D, "TRK", "f");
    // leg->AddEntry(hist_AIR_1D, "Air", "f");
    // leg->AddEntry(hist_ALL_1D, "All", "f");
    leg->Draw();

    TCanvas *c1 = new TCanvas();
    hstack->Draw();
    line->Draw();
    leg->Draw();

    c1->Print("Matbudget_FT3.pdf");

}