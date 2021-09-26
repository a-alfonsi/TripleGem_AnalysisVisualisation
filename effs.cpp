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

// Measure the efficiency from data
// Efficiency is estimated as the number of triple coincidences / number of double coincidences
// The uncertainty is estimated in the script "sigma_VsEff", using the TEfficiency class of root

void ProgressBar (double percentage)
{
    const char PBSTR[61] = "||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||";
    int PBWIDTH = 60;
    int val = (int) (percentage * 100);
    int lpad = (int) (percentage * PBWIDTH);
    int rpad = PBWIDTH - lpad;
    printf ("\r%3d%% [%.*s%*s]", val, lpad, PBSTR, rpad, "");
    fflush (stdout);
}



void effs () {

  std::cout << "\n-------------------------------------------------\n";
  gStyle->SetOptFit(1);
  Double_t x=0.; // dummy variable
  TEfficiency* pEff = new TEfficiency("eff","Efficiency;# simulated events;Efficiency",100,0,100);
  TFile* pFile = new TFile("MeasuredEff.root","recreate");
  bool bPassed;

  Float_t uTime[30000];
  Float_t volt[30000];
  Float_t voltc[30000];
  Float_t ADC;
  Float_t BaseLine;
  Float_t ThrCFD = -0.001; // mV
  Float_t Threshold = -0.1; // mV
  Float_t Thrlow = -0.03; // mV
  Float_t Thrhigh = -0.1; // mV
  Float_t Nmeasured = 0.;

  Float_t DelayCFD;
  TString dummy;
  TString stringa;

  TCanvas *c1 = new TCanvas("c1","GEM results in ASTRA LAB",0,0,700,500);
  TCanvas *c2 = new TCanvas("c2","GEM results in ASTRA LAB",0,0,700,500);
  TCanvas *c3 = new TCanvas("c3","GEM results in ASTRA LAB",0,0,700,500);


  TH1F *h10 = new TH1F("h10"," ADC ",100,-1.0,80.0);
  TH1F *h20 = new TH1F("h20","Single Thr",200,-55.,45.);
  TH1F *h30 = new TH1F("Double Threshold","Double Threshold",200,-55.,45.);

  TH2F *frame = new TH2F ("frame"," ",100, -45., 45., 100, 0., 420.);
  frame->Draw();
  TString run;

  // ---------------------------------------------------------------------
  // CHOOSE MEASUREMENT SET (AT GIVEN VOLTAGE) TO ANALYZE
  // ---------------------------------------------------------------------
  TString SignalToAnalyze;
  std::cout << "Select set of signals to analyze. Choose from:\n";
  TString List;
  ifstream HVSets("HVSets.dat");
  HVSets.seekg(0, ios::end);
  if(!HVSets || HVSets.tellg() == 0)   // Check for empty or missing file
  {
    std::cout<<"Error opening file HVSets.dat" << std::endl;
    std::cout<<"File not found or empty...Please check it\n" << std::endl;
    return;
  }
  HVSets.clear();
  HVSets.seekg(0, ios::beg);
  while(!HVSets.eof())    // Read the list of measurement sets
  {
    HVSets >> List;
    if( !List.CompareTo("") ) continue;
    std::cout << List << endl;
  }
  std::cout << "Your choice: ";
  std::cin >> SignalToAnalyze;
  std::cout << "\nProcessing " << SignalToAnalyze << std::endl;

  // CREATE ROOT FILE TO SAVE HISTOGRAMS
  // listsignals_GEM430volt.dat
  TString rootfile = "Plots" + SignalToAnalyze + ".root";
  rootfile.ReplaceAll("listsignals","");
  rootfile.ReplaceAll(".dat","");
  cout << endl << rootfile << endl;
  TFile *f = TFile::Open(rootfile, "RECREATE");

  TString Hname = rootfile;
  Hname.ReplaceAll("Plots_","");
  Hname.ReplaceAll(".root","");
  Hname.ReplaceAll("GEM","V(GEM)_{i} = ");
  Hname.ReplaceAll("volt"," V");


  // ---------------------------------------------------------------------
  // ANALYZE CHOSEN MEASUREMENT SET
  // ---------------------------------------------------------------------
  // FIXME: open all .txt files in given directory just using TDirectory
  // no need for runlist file
  ifstream runlist(SignalToAnalyze);
  TString BaselineFile = SignalToAnalyze;
  BaselineFile.ReplaceAll("listsignals","baseline");
  ifstream baseline(BaselineFile);
  std::cout << "Using " << BaselineFile << std::endl;
  baseline.seekg(0, ios::end);
  if(!baseline || baseline.tellg() == 0)   // Check for empty or missing file
  {
    cout<<"Error opening file " << BaselineFile << std::endl;
    cout<<"file not found or empty...Please check it" <<endl;
    return;
  }
  baseline.clear();
  baseline.seekg(0, ios::beg);
  runlist.seekg(0, ios::end);
  if(!runlist || runlist.tellg() == 0)   // Check for empty or missing file
  {
    cout<<"Error opening file " << SignalToAnalyze << std::endl;
    cout<<"file not found or empty...Please check it" <<endl;
    return;
  }
  runlist.clear();
  runlist.seekg(0, ios::beg);
  int nRun=0;
  int nRunb=0;
  while(!runlist.eof()) // Reads the list of signal files to process
    {
      runlist >> run;
      baseline >> BaseLine >> nRunb;
      if( !run.CompareTo("") ) continue;    // Avoid empty lines
      if( !BaseLine ) continue;    // Avoid empty lines
      ifstream DSTfile(run);
      DSTfile.seekg(0, ios::end);
      if( !DSTfile ||  DSTfile.tellg()==0 )   // Check for empty or missing file
      {
        std::cout<<"Error opening signal file. "<< run << std::endl;
        std::cout<<"File not found or empty...Please check it." << std::endl;
  	    return;
      }
      DSTfile.clear();
      DSTfile.seekg(0, ios::beg);

      ADC = 0;
      Float_t timesingle = -999.0;
      Float_t timedouble = -999.0;
      int passed = 0;
      int passedlow = 0;
      int passedhigh = 0;
      int nRead = 0;
      for (Int_t i=0; i<6; i++) DSTfile >> dummy;   // Ignore no-signal lines (oscilloscope settings summary)
      while(!DSTfile.eof())   // Read every signal
      {
        DSTfile >> stringa;
        if( !stringa.CompareTo("") ) continue;    // Avoid empty lines
        if (!stringa.Contains(","))
        {
          std::cout << "Check your file, something wrong in its (time,amp) values pattern.\n";
          return;
        }
        TObjArray *tx = stringa.Tokenize(",");
        uTime[nRead] = ((TObjString *)(tx->At(0)))->String().Atof()*1000000000.0;
        volt[nRead] = ((TObjString *)(tx->At(1)))->String().Atof();
        volt[nRead] = volt[nRead] - BaseLine; // Correct the BaseLine of the signal due to slow frequency noise
        // ---------------------------------------------------------------------
        // Single threshold discriminator Simulation
        // ---------------------------------------------------------------------
        if (volt[nRead] <= Threshold && passed == 0)
        {
          passed = 1;
          timesingle = uTime[nRead];
          Nmeasured = Nmeasured + 1;
          bPassed = true;
          pEff->Fill(bPassed,x);
          h20->Fill(timesingle);
        }
        else
        {
          bPassed = false;
          pEff->Fill(bPassed,x);
        }
        // ---------------------------------------------------------------------
        // Double threshold discriminator Simulation
        // ---------------------------------------------------------------------
	      if (volt[nRead] <= Thrlow && passedlow == 0)
        {
          passedlow = 1;
          timedouble = uTime[nRead];
		    }
	      if (volt[nRead] <= Thrhigh && passedhigh == 0)
		    {
		      if ( (uTime[nRead] - timedouble) <= 10)
          {
            passedhigh = 1;
            h30->Fill(timedouble);
		      }
		    }
        // ---------------------------------------------------------------------
        // Calculate the signal charge
        // ---------------------------------------------------------------------
        if (uTime[nRead] >= -200)
        {
          ADC = ADC + volt[nRead];
        }
        nRead++;
      }

      h10->Fill(-ADC);
      DSTfile.close();
      nRun++;
      // PROGRESS BAR
      TString sNevents = SignalToAnalyze;
      sNevents.ReplaceAll("listsignals","Nevents");
      ifstream Nevents(sNevents);
      Float_t Nev;
      Nevents >> Nev;
      ProgressBar(nRun/Nev);
    }
  runlist.close();

  h20 -> GetYaxis() -> SetTitle("Entries/ns");
  h20 -> GetXaxis() -> SetTitle("t (ns)");
  h30 -> GetYaxis() -> SetTitle("Entries/ns");
  h30 -> GetXaxis() -> SetTitle("t (ns)");
  h30 -> SetLineColor(kBlue);
  h30 -> SetTitle(Hname);

  h10->Write("ADC");
  h30->Write("DoubleThreshold");
  h20->Write("SingleThreshold");
  c3->cd();

  std::cout << std::endl << "nTriggers, Nmeasured:   " << nRun << ", " << Nmeasured << std::endl;
  Float_t Efficiency = Nmeasured/nRun;
  std::cout << "Efficiency:" << Nmeasured/nRun << std::endl;

  pEff->Draw("sameP");
  pEff->SetPassedEvents(1,Nmeasured);
  pEff->SetTotalEvents(1,nRun);
  Double_t eff2 = pEff->GetEfficiency(1);
  Double_t ErLow = pEff -> GetEfficiencyErrorLow(1);
  Double_t ErUp = pEff -> GetEfficiencyErrorUp(1);
  printf("\nEfficiency = %f +- %f (up) +- %f (low)\n", eff2, ErUp, ErLow);

}
