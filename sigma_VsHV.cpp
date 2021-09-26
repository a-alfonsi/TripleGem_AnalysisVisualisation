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

// Produces the sigma and std vs HV plot for mixture 1 and mixture 2


void sigma_VsHV () {

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

  // Gas mixture 1 : ArCO2CF4
  std::vector<Float_t> HV1 {430., 440., 450., 460., 470.};
  std::vector<Float_t> Std1 {7.211, 6.927, 6.481, 6.139, 6.274};
  std::vector<Float_t> Sigma1 {5.771, 5.219, 4.806, 4.487, 4.208};
  std::vector<Float_t> Sigma1Err {0.090,0.201,0.073,0.100,0.062};

  // Gas mixture 2 : ArCO2
  std::vector<Float_t> HV2 {370., 380., 390., 400.};
  std::vector<Float_t> Std2 {10.99, 9.963, 9.133, 8.831};
  std::vector<Float_t> Sigma2 {9.715, 8.400, 7.193, 6.418 };
  std::vector<Float_t> Sigma2Err {0.164,0.162,0.097,0.103};

  std::vector<Float_t> HVerr(HV1.size());
  std::vector<Float_t> StdErr(HV1.size());

  for (Int_t i=0; i<HV1.size(); i++)
  {
    HVerr[i] = 2.;
    StdErr[i] = 0.;
  }

  gStyle->SetOptFit(1);

  TCanvas *c1 = new TCanvas("c1","GEM results in ASTRA LAB",0,0,700,500);
  TCanvas *c2 = new TCanvas("c2","GEM results in ASTRA LAB",0,0,700,500);
  TH2D* rangesitter1 = new TH2D("rangesitter1", "Time resolution with Ar CO_{2}CF_{4}", 50, 420, 480, 100, 4, 7.5);
  rangesitter1 -> GetYaxis() -> SetTitleOffset(0.8);
  rangesitter1 -> GetXaxis() -> SetTitleOffset(1.2);
	rangesitter1 -> GetXaxis() -> SetTitle("V(GEM)_{i} (V)");
  rangesitter1 -> GetYaxis() -> SetTitle("Time resolution (ns)");
	// rangesitter1->SetLabelFont(62, "xy");
	rangesitter1->SetTitleFont(43,"xy");
	rangesitter1->SetTitleSize(20,"xy");

  TH2D* rangesitter2 = new TH2D("rangesitter2", "Time resolution with Ar CO_{2}", 50, 360, 410, 100, 6, 11.5);
  rangesitter2 -> GetYaxis() -> SetTitleOffset(0.8);
  rangesitter2 -> GetXaxis() -> SetTitleOffset(1.2);
  rangesitter2 -> GetXaxis() -> SetTitle("V(GEM)_{i} (V)");
  rangesitter2 -> GetYaxis() -> SetTitle("Time resolution (ns)");
  rangesitter2->SetTitleFont(43,"xy");
  rangesitter2->SetTitleSize(20,"xy");

  TGraphErrors *ArCoCF = new TGraphErrors(HV1.size(),&HV1[0],&Sigma1[0],&HVerr[0],&Sigma1Err[0]);
  TGraphErrors *ArCoCFStd = new TGraphErrors(HV1.size(),&HV1[0],&Std1[0],&HVerr[0],&StdErr[0]);

  TGraphErrors *ArCo = new TGraphErrors(HV2.size(),&HV2[0],&Sigma2[0],&HVerr[0],&Sigma2Err[0]);
  TGraphErrors *ArCoStd = new TGraphErrors(HV2.size(),&HV2[0],&Std2[0],&HVerr[0],&StdErr[0]);

  // SET AXIS LABELS
  // ResStdDouble
  ArCoCF->SetMarkerColor(kAzure+7);
  ArCoCF->SetMarkerStyle(20);
  ArCoCF->SetFillColor(1);
  ArCoCF->SetMarkerSize(1);
  ArCoCF->SetLineWidth(1);
  ArCoCF->SetLineColor(kAzure+7);

  ArCoCFStd->SetMarkerColor(kCyan+1);
  ArCoCFStd->SetMarkerStyle(20);
  ArCoCFStd->SetFillColor(1);
  ArCoCFStd->SetMarkerSize(1);
  ArCoCFStd->SetLineWidth(1);
  ArCoCFStd->SetLineColor(kCyan+1);

  // effs2
  ArCo->SetMarkerColor(kTeal+2);
  ArCo->SetMarkerStyle(20);
  ArCo->SetFillColor(1);
  ArCo->SetMarkerSize(1);
  ArCo->SetLineWidth(1);
  ArCo->SetLineColor(kTeal+2);

  ArCoStd->SetMarkerColor(kSpring+9);
  ArCoStd->SetMarkerStyle(20);
  ArCoStd->SetFillColor(1);
  ArCoStd->SetMarkerSize(1);
  ArCoStd->SetLineWidth(1);
  ArCoStd->SetLineColor(kSpring+9);

  gStyle->SetOptStat(0);

  c1->cd();
  rangesitter1->Draw();
  ArCoCF->Draw("sameP");
  ArCoCFStd->Draw("sameP");

  TLegend *leg1 = new TLegend(0.72,0.76,0.88,0.87);
  leg1 -> SetTextFont(43);
  leg1 -> SetTextSizePixels(16);
  leg1 -> SetBorderSize(1);
  leg1 -> SetFillColorAlpha(18, 1);
  leg1 -> AddEntry(ArCoCF, "Sigma",  "lp");
  leg1 -> AddEntry(ArCoCFStd, "Std",  "lp");
  leg1 -> Draw();

  c2->cd();
  rangesitter2 -> Draw();
  ArCo->Draw("sameP");
  ArCoStd->Draw("sameP");

  TLegend *leg2 = new TLegend(0.72,0.76,0.88,0.87);
  leg2 -> SetTextFont(43);
  leg2 -> SetTextSizePixels(16);
  leg2 -> SetBorderSize(1);
  leg2 -> SetFillColorAlpha(18, 1);
  leg2 -> AddEntry(ArCo, "Sigma",  "lp");
  leg2 -> AddEntry(ArCoStd, "Std",  "lp");
  leg2 -> Draw();

}
