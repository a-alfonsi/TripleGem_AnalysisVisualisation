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
// Baseline correction is applied to the signals.


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



void ShowSignals_BSC()
{
  Float_t BaseLine = 0.00 ;
  Float_t avBaseLine = 0.00 ;

  TH2F* Signals = new TH2F("Signals", "", 1500, -140, 350, 1000, -0.7, 0.1);
  TString dummy, stringa;
  const char *dirname = "Gem7030_370V/";
  const char *ext = "txt";
  TString filename = (TString)dirname;

  TString BaselineFile = "baseline_" + filename;
  BaselineFile.ReplaceAll("/",".dat");
  cout << BaselineFile;
  ifstream baseline(BaselineFile);

  filename.ReplaceAll("/","");
  filename = filename + "_BSC.root";
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
  Int_t nRunb=0;
  // std::cout << "Loaded files: " << std::endl;
  for (it = filenamesMap.begin(); it != filenamesMap.end(); ++it)
  {
    baseline >> BaseLine >> nRunb;

    Char_t currentfile[500];
    TString strfile = dirname + it->second;
    strcpy(currentfile, strfile);
    ifstream filearray(currentfile);
    for (Int_t j=0; j<6; j++) filearray >> dummy;   // Ignore no-signal lines (oscilloscope settings summary)
    Int_t j = 0;
    while (!filearray.eof())
    {
      filearray >> stringa;
      if( !stringa.CompareTo("") ) continue;    // Avoid empty lines
      TObjArray *tx = stringa.Tokenize(",");
      // Time.push_back(((TObjString *)(tx->At(0)))->String().Atof()*1000000000.0);
      Time[j] = ((TObjString *)(tx->At(0)))->String().Atof()*1000000000.0;
      // Ampl.push_back(((TObjString *)(tx->At(1)))->String().Atof());
      Ampl[j] = ((TObjString *)(tx->At(1)))->String().Atof();
      // std::cout << "time:amp = " << Time[j] << ":" << Ampl[j] << endl;
      Ampl[j] = Ampl[j] - BaseLine; // Correct the BaseLine of the signal due to slow frequency noise

      Signals -> Fill(Time[j], Ampl[j]);
      j++;
    }
    TGraphArray[i] = new TGraph(Time.size(), &Time[0], &Ampl[0]);
    SingleSignals -> cd();
    TGraphArray[i] -> Write("Signal"); // save the graph as "Signals"

    filearray.close();
    // std::cout << i << " - " << currentfile << std::endl;
    i++;
  }
  baseline.close();
  f->cd(); // make sure that we will write to this ROOT file
  Signals->Write("Signals"); // save the TH2 as "Signals"

  f->ls(); // show the contents of the ROOT file
  delete f; // close the ROOT file
  cout << n;


  TCanvas *c1 = new TCanvas("c1","GEM results in ASTRA LAB",0,0,700,500);
  c1->SetGrid();
  gStyle -> SetOptStat(0);
  // gStyle->SetPalette(107);
  // gStyle->SetPalette(55);
  // Signals->SetContour(200);
  set_plot_style();
  TH2D* rangesitter = new TH2D("rangesetter", "", 100, -140, 350, 100, -0.7, 0.1);
  // rangesitter -> GetXaxis() -> SetTickLength(0);
  rangesitter -> GetXaxis() -> SetLabelOffset(0.015);
  rangesitter -> GetYaxis() -> SetLabelOffset(0.015);
  rangesitter -> GetYaxis() -> SetTitleOffset(1.2);
  rangesitter -> GetXaxis() -> SetTitleOffset(1.2);

  rangesitter -> GetYaxis() -> SetTitle("Ampl. (mV)");
  rangesitter -> GetXaxis() -> SetTitle("Time (ns)");
  rangesitter -> Draw();
  Int_t a = 0;
  // Int_t b = 130;
  TGraphArray[a]->Draw("P");
  // TGraphArray[b]->SetMarkerColor(kGray);
  // TGraphArray[b]->Draw("P");
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
  // gPad->SetFillColor(kGray);
  // c1->SetFrameFillColor(50);
  Signals -> Draw("colz2");


  return;
}
