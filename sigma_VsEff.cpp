#include "TF1.h"
#include "TString.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TStyle.h"
#include "TLegend.h"
#include <TH2.h>
#include "TROOT.h"
#include "TApplication.h"
#include <TLorentzVector.h>
#include <string>
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TLatex.h"

// Produces the sigma vs eff plot for mixture 1, mixture 2 and both mixtures together



void sigma_VsEff () {

  // Mixture 1
  //             Std      Sigma
  // 0.684207    7,211    5,771
  // 0.710455    6,927    5,219
  // 0.708640    6,481    4,806
  // 0.708928    6,139    4,487
  // 0.712760    6,274    4,208
  //
  // Mixture 2
  //             Std      Sigma
  // 0.692651    10,99    9,715
  // 0.712748    9,963    8,488
  // 0.722790    9,133    7,193
  // 0.729433    8,831    6,418

  TEfficiency* pEff1 = new TEfficiency("eff1","Efficiency;HV;Efficiency with ArCO^{2}CF_{4}",10,0,10);
  TEfficiency* pEff2 = new TEfficiency("eff2","Efficiency;HV;Efficiency with ArCO^{2}",10,0,10);

  // Gas mixture 1 : ArCO2CF4
  std::vector<Float_t> nMeasured1 {9544., 7128., 8678., 8600., 7809.};
  std::vector<Float_t> nTriggers1 {13949., 10033., 12246., 12131., 10956.};
  std::vector<Float_t> HV1 {430., 440., 450., 460., 470.};
  std::vector<Float_t> Std1 {7.211, 6.927, 6.481, 6.139, 6.274};
  std::vector<Float_t> Sigma1 {5.771, 5.219, 4.806, 4.487, 4.208};
  // corrette
  // std::vector<Float_t> Sigma1 {5.771, 5.200, 4.806, 4.487, 4.228};
  std::vector<Float_t> Sigma1Err {0.090,0.201,0.073,0.100,0.062};

  std::vector<Float_t> Eff1(HV1.size());
  std::vector<Float_t> Eff1Low(HV1.size());
  std::vector<Float_t> Eff1Up(HV1.size());

  // Gas mixture 2 : ArCO2
  std::vector<Float_t> nMeasured2 {8077., 7151., 8062., 8441.};
  std::vector<Float_t> nTriggers2 {11661., 10033., 11154., 11572.};
  std::vector<Float_t> HV2 {370., 380., 390., 400.};
  // std::vector<Float_t> Std2 {7.211, 6.927, 6.481, 6.139, 6.274};
  std::vector<Float_t> Sigma2 {9.715, 8.400, 7.193, 6.418 };
  std::vector<Float_t> Sigma2Err {0.164,0.162,0.097,0.103};

  std::vector<Float_t> Eff2(HV2.size());
  std::vector<Float_t> Eff2Low(HV2.size());
  std::vector<Float_t> Eff2Up(HV2.size());


  std::vector<Float_t> HVerr(HV1.size());
  for (Int_t i=0; i<HV1.size(); i++) { HVerr[i] = 2.; }

  // Fill efficiency for Gas mixture 1
  printf("\nEfficiency for Gas mixture 1:\n");
  for (Int_t i=0; i<HV1.size(); i++)
  {
    pEff1->SetTotalEvents(i+1,nTriggers1[i]);
    pEff1->SetPassedEvents(i+1,nMeasured1[i]);
    Eff1[i] = pEff1->GetEfficiency(i+1);
    Eff1Low[i] = pEff1 -> GetEfficiencyErrorLow(i+1);
    Eff1Up[i] = pEff1 -> GetEfficiencyErrorUp(i+1);
    printf("%3.0f => %f +- %f (up) +- %f (low)\n", HV1[i], Eff1[i], Eff1Up[i], Eff1Low[i]);
  }

  // Fill efficiency for Gas mixture 2
  printf("\nEfficiency for Gas mixture 2:\n");
  for (Int_t i=0; i<HV2.size(); i++)
  {
    pEff2->SetTotalEvents(i+1,nTriggers2[i]);
    pEff2->SetPassedEvents(i+1,nMeasured2[i]);
    Eff2[i] = pEff2->GetEfficiency(i+1);
    Eff2Low[i] = pEff2 -> GetEfficiencyErrorLow(i+1);
    Eff2Up[i] = pEff2 -> GetEfficiencyErrorUp(i+1);
    printf("\%3.0f => %f +- %f (up) +- %f (low)\n", HV2[i], Eff2[i], Eff2Up[i], Eff2Low[i]);
  }

  gStyle->SetOptFit(1);

  TCanvas *c1 = new TCanvas("c1","GEM results in ASTRA LAB",0,0,700,500);
  TCanvas *c2 = new TCanvas("c2","GEM results in ASTRA LAB",0,0,700,500);
  TCanvas *c3 = new TCanvas("c3","GEM results in ASTRA LAB",0,0,700,500);

  TH2D* rangesitter1 = new TH2D("rangesitter1", "Time resolution with Ar CO_{2}CF_{4}", 50, 0.67, 0.73, 100, 4, 6);
  rangesitter1 -> GetYaxis() -> SetTitleOffset(1.);
  rangesitter1 -> GetXaxis() -> SetTitleOffset(1.2);
	rangesitter1 -> GetXaxis() -> SetTitle("Efficiency");
  rangesitter1 -> GetYaxis() -> SetTitle("Sigma (ns)");
	rangesitter1->SetTitleFont(43,"xy");
	rangesitter1->SetTitleSize(20,"xy");

  TH2D* rangesitter2 = new TH2D("rangesitter2", "Time resolution with Ar CO_{2}", 50, 0.68, 0.74, 100, 6, 10);
  rangesitter2 -> GetYaxis() -> SetTitleOffset(1.);
  rangesitter2 -> GetXaxis() -> SetTitleOffset(1.2);
  rangesitter2 -> GetXaxis() -> SetTitle("Efficiency");
  rangesitter2 -> GetYaxis() -> SetTitle("Sigma (ns)");
  rangesitter2->SetTitleFont(43,"xy");
  rangesitter2->SetTitleSize(20,"xy");

  TH2D* rangesitter3 = new TH2D("rangesitter3", "Time resolution comparison", 50, 0.67, 0.74, 100, 3, 11);
  rangesitter3 -> GetYaxis() -> SetTitleOffset(1.);
  rangesitter3 -> GetXaxis() -> SetTitleOffset(1.2);
  rangesitter3 -> GetXaxis() -> SetTitle("Efficiency");
  rangesitter3 -> GetYaxis() -> SetTitle("Sigma (ns)");
  rangesitter3->SetTitleFont(43,"xy");
  rangesitter3->SetTitleSize(20,"xy");

  Eff1[1] = 0.701207;

  TGraphAsymmErrors *ArCoCF = new TGraphAsymmErrors(Eff1.size(),&Eff1[0],&Sigma1[0],&Eff1Low[0],&Eff1Up[0],&Sigma1Err[0],&Sigma1Err[0]);
  TGraphAsymmErrors *ArCo = new TGraphAsymmErrors(Eff2.size(),&Eff2[0],&Sigma2[0],&Eff2Low[0],&Eff2Up[0],&Sigma2Err[0],&Sigma2Err[0]);

  // SET AXIS LABELS
  // ResStdDouble
  ArCoCF->SetMarkerColor(kAzure+7);
  ArCoCF->SetMarkerStyle(20);
  ArCoCF->SetFillColor(1);
  ArCoCF->SetMarkerSize(1);
  ArCoCF->SetLineWidth(1);
  ArCoCF->SetLineColor(kAzure+7);

  // effs2
  ArCo->SetMarkerColor(kTeal+2);
  ArCo->SetMarkerStyle(20);
  ArCo->SetFillColor(1);
  ArCo->SetMarkerSize(1);
  ArCo->SetLineWidth(1);
  ArCo->SetLineColor(kTeal+2);

  gStyle->SetOptStat(0);

  c1->cd();
  rangesitter1->Draw();
  ArCoCF->Draw("sameP");

  c2->cd();
  rangesitter2 -> Draw();
  ArCo->Draw("sameP");

  c3->cd();
  rangesitter3 -> Draw();
  ArCoCF->Draw("sameP");
  ArCo->Draw("sameP");

  TLegend *leg = new TLegend(0.72,0.76,0.88,0.87);
  leg -> SetTextFont(43);
  leg -> SetTextSizePixels(16);
  leg -> SetBorderSize(1);
  leg -> SetFillColorAlpha(18, 1);
  leg -> AddEntry(ArCoCF, "ArCO_{2}CF_{4}",  "lp");
  leg -> AddEntry(ArCo, "ArCO_{2}",  "lp");
  leg -> Draw();

}
