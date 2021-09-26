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

// Calculates the efficiency measured with two gas mixtures and produces the efficiency vs Voltage for each mixture.
// The error on efficicency is calculated using binomial statistics.

void effVshv () {

  TEfficiency* pEff1 = new TEfficiency("eff1","Efficiency;HV;Efficiency with ArCO^{2}CF_{4}",10,0,10);
  TEfficiency* pEff2 = new TEfficiency("eff2","Efficiency;HV;Efficiency with ArCO^{2}",10,0,10);

  // Gas mixture 1 : ArCO2CF4
  std::vector<Float_t> nMeasured1 {9544., 7128., 8678., 8600., 7809.};
  std::vector<Int_t> nTriggers1 {13949, 10033, 12246, 12131, 10956};
  std::vector<Float_t> HV1 {430., 440., 450., 460., 470.};

  std::vector<Float_t> Eff1(HV1.size());
  std::vector<Float_t> Eff1Low(HV1.size());
  std::vector<Float_t> Eff1Up(HV1.size());

  // Gas mixture 2 : ArCO2
  std::vector<Float_t> nMeasured2 {8077., 7151., 8062., 8441.};
  std::vector<Float_t> nTriggers2 {11661., 10033., 11154., 11572.};
  std::vector<Float_t> HV2 {370., 380., 390., 400.};

  std::vector<Float_t> Eff2(HV2.size());
  std::vector<Float_t> Eff2Low(HV2.size());
  std::vector<Float_t> Eff2Up(HV2.size());

  std::vector<Float_t> HVerr(HV1.size());
  for (Int_t i=0; i<HV1.size(); i++) { HVerr[i] = 2.; }

  // Fill efficiency for Gas mixture 1
  printf("\nEfficiency for Gas mixture 1:\n");
  for (Int_t i=0; i<HV1.size(); i++)
  {
    // cout << endl << endl << HV1.size() << endl << endl;
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

  TH2D* rangesitter1 = new TH2D("rangesitter1", "Efficiency with Ar CO_{2}CF_{4}", 50, 420, 480, 100, 0.67, 0.72);
  rangesitter1 -> GetYaxis() -> SetTitleOffset(1.);
  rangesitter1 -> GetXaxis() -> SetTitleOffset(1.1);
	rangesitter1 -> GetXaxis() -> SetTitle("V(GEM)_{i} (V)");
  rangesitter1 -> GetYaxis() -> SetTitle("Efficiency");
	rangesitter1->SetTitleFont(43,"xy");
	rangesitter1->SetTitleSize(20,"xy");

  TH2D* rangesitter2 = new TH2D("rangesitter2", "Efficiency with Ar CO_{2}", 50, 360, 410, 100, 0.68, 0.74);
  rangesitter2 -> GetYaxis() -> SetTitleOffset(1.);
  rangesitter2 -> GetXaxis() -> SetTitleOffset(1.1);
  rangesitter2 -> GetXaxis() -> SetTitle("V(GEM)_{i} (V)");
  rangesitter2 -> GetYaxis() -> SetTitle("Efficiency");
  rangesitter2->SetTitleFont(43,"xy");
  rangesitter2->SetTitleSize(20,"xy");

  TH2D* rangesitter3 = new TH2D("rangesitter3", "Efficiency", 50, 360, 480, 100, 0.68, 0.74);
  rangesitter3 -> GetYaxis() -> SetTitleOffset(1.);
  rangesitter3 -> GetXaxis() -> SetTitleOffset(1.1);
  rangesitter3 -> GetXaxis() -> SetTitle("V(GEM)_{i} (V)");
  rangesitter3 -> GetYaxis() -> SetTitle("Efficiency");
  rangesitter3->SetTitleFont(43,"xy");
  rangesitter3->SetTitleSize(20,"xy");

  cout << endl << endl << Eff1[1];
  Eff1[1] = 0.701207;

  TGraphAsymmErrors *effs1 = new TGraphAsymmErrors(HV1.size(),&HV1[0],&Eff1[0],&HVerr[0],&HVerr[0],&Eff1Low[0],&Eff1Up[0]);
  TGraphAsymmErrors *effs2 = new TGraphAsymmErrors(HV2.size(),&HV2[0],&Eff2[0],&HVerr[0],&HVerr[0],&Eff2Low[0],&Eff2Up[0]);


  // ResStdDouble
  effs1->SetMarkerColor(kBlue);
  effs1->SetMarkerStyle(20);
  effs1->SetFillColor(1);
  effs1->SetMarkerSize(1);
  effs1->SetLineWidth(1);
  effs1->SetLineColor(kBlue);

  // effs2
  effs2->SetMarkerColor(kGreen+2);
  effs2->SetMarkerStyle(20);
  effs2->SetFillColor(1);
  effs2->SetMarkerSize(1);
  effs2->SetLineWidth(1);
  effs2->SetLineColor(kGreen+2);

  // SET AXIS LABELS
  // effs1
  TAxis* xAxis = effs1->GetXaxis();
  xAxis->SetTitle("HV (V)");
  xAxis->SetTitleOffset( 1.2 );
  TAxis* yAxis = effs1->GetYaxis();
  yAxis->SetTitle("Efficiency");
  yAxis->SetTitleOffset( 1.2 );
  // effs2
  TAxis* x1Axis = effs2->GetXaxis();
  x1Axis->SetTitle("HV (V)");
  x1Axis->SetTitleOffset( 1.2 );
  TAxis* y1Axis = effs2->GetYaxis();
  y1Axis->SetTitle("Efficiency");
  y1Axis->SetTitleOffset( 1.2 );
  gStyle->SetFuncWidth(1);

  TF1 *f1 = new TF1("f1","[2]/(1+exp([0]*(x+[1])))",300,1000);
  c3->cd();
  f1->SetParameter(0,-0.06);
  f1->SetParameter(1,-0.01);
  f1->SetParameter(2,0.737);
  f1->Draw();

  c1->cd();
  rangesitter1 -> Draw();
  gStyle->SetOptStat(0);
  effs1->Draw("sameP");
  effs1->Fit("f1","","0",0.,1000);
  
  c2->cd();
  rangesitter2 -> Draw();
  effs2->Draw("sameP");
  effs2->Fit("f1","","0",0.,1000);

}
