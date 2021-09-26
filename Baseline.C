#include "TF1.h"
#include "TF2.h"
#include "TString.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TStyle.h"
#include "TLegend.h"
#include <TH2.h>
#include "TH2F.h"
#include "TROOT.h"
#include "TApplication.h"
#include <TLorentzVector.h>
#include <string>


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


void Baseline ()
{
  std::cout << "\n-------------------------------------------------\n";
  Float_t uTime[3000];
  Float_t volt[3000];
  Float_t BaseLine = 0.00 ;
  Float_t avBaseLine = 0.00 ;
  Float_t ThrCFD = -0.001; // mV
  Float_t Threshold = -0.005; // mV
  Float_t Thrlow = -0.002; // mV
  Float_t Thrhigh = -0.01; // mV
  Float_t DelayCFD;
  TString dummy;
  TString stringa;

  TCanvas *c1 = new TCanvas("c1","ASTRA GGM Efficiency plots",0,0,700,500);

  TH1F *h10 = new TH1F("h10","Single Thr",200,-0.1,0.1);
  TH1F *h20 = new TH1F("h20","Double Thr",200,-0.1,0.1);

  TH2F *frame = new TH2F ("frame"," ",100, -150., 400, 100, -0.4, 0.05);
  // frame->Draw();

  TF1 *signal_ = new TF1("signal"," [0]/(1 + exp( [1] * ([2] - x) ) )",1,3);
  signal_->SetParameters(100.0, 0.015, 9750);
  signal_->SetParNames("Efficiency","Slope","HV @ 50 efficiency");

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

  // Define output file for selected analyzed set
  TString OutFileName = SignalToAnalyze;
  OutFileName.ReplaceAll("listsignals","baseline");
  std::cout << "Writing output in " << OutFileName << std::endl;
  ofstream outfile;
  outfile.open(OutFileName);

  // ---------------------------------------------------------------------
  // ANALYZE CHOSEN MEASUREMENT SET
  // ---------------------------------------------------------------------
  ifstream runlist(SignalToAnalyze);
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
  while(!runlist.eof()) // Read the list of signal files
  {
    runlist >> run;
    if( !run.CompareTo("") ) continue;    // Avoid empty lines
    // cout << endl << run << endl;
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
    int nRead = 0;
    int passed = 0;
    int passedCFD = 0;
    int passedlow = 0;
    int passedhigh = 0;
    Float_t timesingle = -999.0;
    Float_t timedouble = -999.0;
    BaseLine = 0;
    for (Int_t i=0; i<6; i++) DSTfile >> dummy;   // Ignore no-signal lines (oscilloscope settings summary)
    while(!DSTfile.eof())   // Read every signal
    {
      DSTfile >> stringa;
      if( !stringa.CompareTo("") ) continue;    // Avoid empty lines
      // std::cout << "stringa: " << stringa << std::endl;
      if (!stringa.Contains(","))
      {
        std::cout << "Check your file, something wrong in its (time,amp) values pattern.\n";
        return;
      }
      TObjArray *tx = stringa.Tokenize(",");
      uTime[nRead] = ((TObjString *)(tx->At(0)))->String().Atof()*1000000000.0;
      volt[nRead] = ((TObjString *)(tx->At(1)))->String().Atof();
      // std::cout << "volt: " << volt[nRead] << std::endl;
      // std::cout << "utime: " << uTime[nRead] << std::endl;
      if (uTime[nRead] <= -20.0)
      {
        BaseLine = BaseLine + volt[nRead];
        nRead++;
        avBaseLine = BaseLine / nRead;
        // std::cout << BaseLine << std::endl;
        // std::cout << avBaseLine << " average " << nRead << " nRead "<< nRun << std::endl;
      }
    }
    DSTfile.close();
    outfile << avBaseLine <<" "<< nRun << endl;
    nRun++;
    // PROGRESS BAR
    TString sNevents = SignalToAnalyze;
    sNevents.ReplaceAll("listsignals","Nevents");
    ifstream Nevents(sNevents);
    Float_t Nev;
    Nevents >> Nev;
    ProgressBar(nRun/Nev);
    avBaseLine = 0.0;
  }
  std::cout << std::endl;
  runlist.close();
  outfile.close();
}