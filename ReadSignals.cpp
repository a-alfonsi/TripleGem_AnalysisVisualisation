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

// Reads the plot produced by ShowSignals.cpp and prints it in good fashion


void set_plot_style()
{
  const int NRGBs = 7;
  const int NCont = 999;
  gStyle->SetNumberContours(NCont);
  Double_t stops[NRGBs] = { 0.00, 0.10, 0.25, 0.45, 0.60, 0.75, 1.00 };
  Double_t red[NRGBs]   = { 0.50, 0.00, 0.00, 0.00, 0.97, 0.97, 0.10 };
  Double_t green[NRGBs] = { 0.50, 0.97, 0.30, 0.40, 0.97, 0.00, 0.00 };
  Double_t blue[NRGBs]  = { 0.50, 0.97, 0.97, 0.00, 0.00, 0.00, 0.00 };
  TColor::CreateGradientColorTable(NRGBs, stops, red, green, blue, NCont);
  gStyle->SetNumberContours(NCont);
}




void ReadSignals()
{
  // OPEN RUN FILE AND GET TH2 OF ALL SIGNALS FROM THE OSCILLOSCOPE
  TString dummy, stringa;
  // const char *dirname = "GEM470volt/";
  const char *dirname = "Gem7030_380V_BSC/";
  TString filename = (TString)dirname;
  filename.Strip();
  filename.ReplaceAll("/","");

  TString TH2name = filename;
  TH2name.ReplaceAll("Gem7030_", "V(GEM)_{i} = ");
  TH2name.ReplaceAll("V", " V");
  TH2name.ReplaceAll("_BSC", " (Baseline Corrected)");

  filename = filename + ".root";
  cout << endl << filename << endl;
  TFile *f = new TFile(filename);
  f->cd();
  f->ls();
  TH2F *Signals = (TH2F*)f -> Get("Signals");

  // SET CANVAS
  TCanvas *c1 = new TCanvas("c1","GEM results in ASTRA LAB",0,0,800,500);
  // c1->SetGrid();
  gStyle -> SetOptStat(0);
  c1->SetRightMargin(0.12);
  set_plot_style();

  // SET TH2 STYLE AD DRAW
  Signals -> GetXaxis() -> SetLabelOffset(0.015);
  Signals -> GetYaxis() -> SetLabelOffset(0.015);
  Signals -> GetYaxis() -> SetTitleOffset(1.2);
  Signals -> GetXaxis() -> SetTitleOffset(1.2);
  Signals -> GetYaxis() -> SetTitle("Amplitude (mV)");
  Signals -> GetXaxis() -> SetTitle("Time (ns)");
  Signals -> SetTitle(TH2name);
  Signals -> Draw("colz2");
  // Signals -> Draw("sameaxis");

  // SET AND DRAW LOW THRESHOLD
  Float_t LowThr = -0.03;
  TLine *LowThreshold = new TLine(-140,LowThr,350,LowThr);
  LowThreshold -> SetLineColor(kBlue+2);
  LowThreshold -> SetLineWidth(1);
  LowThreshold -> Draw();

  TLatex *Tl = new TLatex();
  Tl -> SetTextAlign(12);
  Tl -> SetTextSize(0.03);
  Tl -> SetTextFont(42);
  Tl -> DrawLatex(250,-0.01,"low threshold");
  TLatex *Th = new TLatex();
  Th -> SetTextAlign(12);
  Th -> SetTextSize(0.03);
  Th -> SetTextFont(42);
  Th -> DrawLatex(250,-0.08,"high threshold");

  // SET AND DRAW HIGH THRESHOLD
  Float_t HighThr = -0.1;
  TLine *HighThreshold = new TLine(-140,HighThr,350,HighThr);
  HighThreshold -> SetLineColor(kRed+2);
  HighThreshold -> SetLineWidth(1);
  HighThreshold -> Draw("same");

  return;
}
