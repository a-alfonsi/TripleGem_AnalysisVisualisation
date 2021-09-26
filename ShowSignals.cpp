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


// Produces a plot resembling the oscilloscope output and saves it into a root file, to be opened with ReadSignals.cpp
// The signals shown are without baseline correction


void set_plot_style()
{
  const int NRGBs = 7;
  const int NCont = 999;
  gStyle->SetNumberContours(NCont);
  Double_t stops[NRGBs] = { 0.00, 0.10, 0.25, 0.45, 0.60, 0.75, 1.00 };
  Double_t red[NRGBs]   = { 0.90, 0.00, 0.00, 0.00, 0.97, 0.97, 0.10 };
  Double_t green[NRGBs] = { 0.90, 0.97, 0.30, 0.40, 0.97, 0.00, 0.00 };
  Double_t blue[NRGBs]  = { 0.90, 0.97, 0.97, 0.00, 0.00, 0.00, 0.00 };

  TColor::CreateGradientColorTable(NRGBs, stops, red, green, blue, NCont);
  gStyle->SetNumberContours(NCont);
}



void DrawGrid()
{
  TPad *grid = new TPad("grid","",0,0,1,1);
  grid->Draw();
  grid->cd();
  grid->SetGrid();
  grid->SetFillStyle(4000);
  grid->SetFrameFillStyle(0);
  TH2 *hgrid = new TH2C("hgrid","",208,-196,404.,60,-0.91,0.12);
  hgrid->Draw();
  hgrid->GetXaxis()->SetNdivisions(208);
  hgrid->GetYaxis()->SetNdivisions(64);
  hgrid->GetYaxis()->SetLabelOffset(999.);
  hgrid->GetXaxis()->SetLabelOffset(999.);
}



void ShowSignals()
{
  TH2F* Signals = new TH2F("Signals", "", 1500, -140, 350, 1000, -0.7, 0.1);
  TString dummy, stringa;
  const char *dirname = "Gem7030_400V/";
  const char *ext = "txt";
  TString filename = (TString)dirname;
  filename.Strip();
  filename.ReplaceAll("/","");
  filename = filename + ".root";
  cout << endl << filename << endl;
  TFile *f = TFile::Open(filename, "RECREATE");
  TDirectory *SingleSignals = f->mkdir("SingleSignals");   // create a subdirectory "SingleSignals" in this file

  std::map<TString,TString> filenamesMap;
  // LIST FILES WITH THE CHOSEN EXTENSION AND ORDER THEM ALPHABETICALLY WITH A MAP
  TSystemDirectory dir(dirname, dirname);
  TList *files = dir.GetListOfFiles();
  n=0;
  if (files)
  {
    TSystemFile *file;
    TString tfname;
    TString tfnamel = "empty";
    TIter next(files);
    while ( (file=(TSystemFile*)next()) )
    {
      tfname = file->GetName();
      // std::cout << tfname << std::endl;
      if (!file->IsDirectory() && tfname.EndsWith(ext))
      {
        tfnamel = tfname;
        tfnamel.ToLower();
        filenamesMap.insert(std::pair<TString, TString>(tfnamel, tfname));
        n++;
      }
    }
  }
  // OPEN LISTED SIGNAL FILES IN THE RIGHT ORDER AND FILL TGRAPH FOR EACH SIGNAL
  std::vector<Float_t> Time(5000);
  std::vector<Float_t> Ampl(5000);
  TGraph *TGraphArray[n];
  std::map<TString,TString>::iterator it;
  Int_t i = 0;
  // std::cout << "Loaded files: " << std::endl;
  for (it = filenamesMap.begin(); it != filenamesMap.end(); ++it)
  {
    Char_t currentfile[500];
    TString strfile = dirname + it->second;
    strcpy(currentfile, strfile);
    ifstream SignalFile(currentfile);
    // SignalFile[i](currentfile);

    for (Int_t j=0; j<6; j++) SignalFile >> dummy;   // Ignore no-signal lines (oscilloscope settings summary)
    Int_t j = 0;
    while (!SignalFile.eof())
    {
      SignalFile >> stringa;
      if( !stringa.CompareTo("") ) continue;    // Avoid empty lines
      TObjArray *tx = stringa.Tokenize(",");
      // Time.push_back(((TObjString *)(tx->At(0)))->String().Atof()*1000000000.0);
      Time[j] = ((TObjString *)(tx->At(0)))->String().Atof()*1000000000.0;
      // Ampl.push_back(((TObjString *)(tx->At(1)))->String().Atof());
      Ampl[j] = ((TObjString *)(tx->At(1)))->String().Atof();
      // std::cout << "time:amp = " << Time[j] << ":" << Ampl[j] << endl;
      Signals -> Fill(Time[j], Ampl[j]);
      j++;
    }
    // TGraph *g_mod = new TGraph(Time.size(), &Time[0], &Ampl[0]);
    TGraphArray[i] = new TGraph(Time.size(), &Time[0], &Ampl[0]);
    SingleSignals -> cd();
    TGraphArray[i] -> Write("Signal"); // save the graph as "Signals"

    SignalFile.close();
    i++;
  }
  f->cd(); // make sure that we will write to this ROOT file
  Signals->Write("Signals"); // save the TH2 as "Signals"

  f->ls(); // show the contents of the ROOT file
  delete f; // close the ROOT file

  cout << n;

  TCanvas *c1 = new TCanvas("c1","GEM results in ASTRA LAB",0,0,700,500);
  c1->SetGrid();
  gStyle -> SetOptStat(0);
  set_plot_style();
  TH2D* rangesitter = new TH2D("rangesetter", "", 100, -140, 350, 100, -0.7, 0.1);
  rangesitter -> GetXaxis() -> SetLabelOffset(0.015);
  rangesitter -> GetYaxis() -> SetLabelOffset(0.015);
  rangesitter -> GetYaxis() -> SetTitleOffset(1.2);
  rangesitter -> GetXaxis() -> SetTitleOffset(1.2);

  rangesitter -> GetYaxis() -> SetTitle("Ampl. (mV)");
  rangesitter -> GetXaxis() -> SetTitle("Time (ns)");
  rangesitter -> Draw();
  Int_t a = 0;
  TGraphArray[a]->Draw("P");
  for (Int_t i=1; i<n; i++)
  {
    TGraphArray[i]->Draw("P");
  }

  TLine *LowThreshold = new TLine(-200,-0.03,350,-0.03);
  LowThreshold -> SetLineColor(kGreen+1);
  LowThreshold -> SetLineWidth(1);
  LowThreshold -> Draw();

  TLine *HighThreshold = new TLine(-200,-0.1,350,-0.1);
  HighThreshold -> SetLineColor(kRed);
  HighThreshold -> SetLineWidth(1);
  HighThreshold -> Draw();


  TCanvas *c2 = new TCanvas("c2","GEM results in ASTRA LAB",0,0,700,500);
  c2->cd();
  Signals -> Draw("colz2");
}
