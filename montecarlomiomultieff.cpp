/***************************************************
MonteCarlo simulation for triple and double concidences with triple GEM detector setup at Astra lab.
Simulates efficiency = # Triple coinc. / # Double Coinc.
Using different numbers of simulated cosmic muons.
Produces a lot with the estimated efficiency for each number of simulated muons.
The error on the efficiency is calculated using binomial statistics, 68% C.L.
 ***************************************************/


void montecarlomiomultieff() {

	Int_t N; // Number of muons to simulate
	Int_t Dcoinc = 0, Tcoinc = 0; // Number of coincidences
	Double_t eff = 0.;
	Double_t x;
	Double_t enne=0.;
	/***************************************************
	 * Muon coordinates
	 ***************************************************/
	// Muon coordinates of top scintillator
	Double_t xT, yT, zT = 5.;
	// Muon coordinates on GEM detector
	Double_t xG, yG, zG = 0.;
	// Muon coordinates of bottom scintillator
	Double_t xB, yB, zB = -5.;
	// Muon direction coordinates
	Double_t costheta, phi, tantheta;

	/***************************************************
	 * Detectors coordinates
	 ***************************************************/
	//  Top scintillator
	Double_t x1_T = 1.75, x2_T = 8.25;
	Double_t y1_T = 0., y2_T = 7.5;
	Double_t z_T = 5.;
	// GEM detector
	Double_t x1_G = 0., x2_G = 10.;
	Double_t y1_G = 0., y2_G = 3.5;
	Double_t z_G = 0.;
	// Bottom scintillator
	Double_t x1_B = 0., x2_B = 10.;
	Double_t y1_B = 0., y2_B = 3.5;
	Double_t z_B = -5.;

	TEfficiency* pEff = new TEfficiency("eff","Efficiency;# simulated events;Efficiency",1000,500000,5500000);
	TFile* pFile = new TFile("myfile.root","recreate");
	bool bPassed;


	/***************************************************
	 * Data input
	 ***************************************************/
	printf("\n ESTIMATING GEM EFFICIENCY");
	printf(" Number of muons to simulate: ");
	scanf("%d", &N);
	getchar();

	/***************************************************
	 * SIMULATION
	 ***************************************************/
	TRandom3 *random = new TRandom3();
	random -> SetSeed(0);

	for (Int_t j=1; j<=10; j++)
	{
		enne =(double)N*j;

	for (Int_t i=1; i<=enne; i++)
	{
		/***************************************************
	 	 * Muon coordinates at top scintillator
		 ***************************************************/
		 xT = random -> Uniform(x1_T,x2_T);
		 yT = random -> Uniform(y1_T,y2_T);

		/***************************************************
	 	 * Muon incident direction (theta, phi)
		 * Simulation of the angular distribution ~cos^2
		 ***************************************************/
		 costheta = sqrt(random -> Uniform(0.,1.));
		 phi = random -> Uniform(0.,2.*TMath::Pi());
		 tantheta = sqrt(1. - costheta*costheta) / costheta;

		 /***************************************************
			* Propagation of muon coordinates at lower scintillator
			***************************************************/
		 xB = xT - (z_B - z_T)*tantheta*cos(phi);
		 yB = yT - (z_B - z_T)*tantheta*sin(phi);
		//  z = z_B;
		// cout << "tantheta, xB, yB = " << tantheta << ", " << xB << ", " << yB << endl;
		// cout << "x1_B, x2_B = " << x1_B << ", " << x2_B << endl;
		 if( (x1_B<=xB)&&(xB<=x2_B) && (y1_B<=yB)&&(yB<=y2_B) )
		 {
			//  cout << "xB, yB = " << xB << ", " << yB << endl;
			 Dcoinc = Dcoinc + 1;

			 /***************************************************
				* Propagation of muon coordinates at GEM detector
				***************************************************/
			 xG = xT - (z_G - z_T)*tantheta*cos(phi);
			 yG = yT - (z_G - z_T)*tantheta*sin(phi);

			 if( (x1_G<=xG)&&(xG<=x2_G) && (y1_G<=yG)&&(yG<=y2_G) )
			 {
				 Tcoinc = Tcoinc + 1;
				 bPassed = true;
				 x=1.;
				 pEff->Fill(bPassed,enne);
			 }
			 else
			 {
				 bPassed = false;
				 x=0.;
				 pEff->Fill(bPassed,enne);
			 }
		 }

	}
	eff = (double)Tcoinc/(double)Dcoinc;
	printf("\n Number of simulated muons = %f", enne);
	printf("\n Number of double coincidences = %d", Dcoinc);
	printf("\n Number of triple coincidences = %d", Tcoinc);
	printf("\n Efficiency  = %3.1lf %%\n\n", eff*100.);
	}

	TCanvas *c1 = new TCanvas("c1","GEM results in ASTRA LAB",0,0,700,500);
	c1->SetRightMargin( 0.05 );
	c1->SetTopMargin( 0.05 );
	TH2D* rangesitter = new TH2D("rangesitter", "", 50, 100*1E03, 5500*1E03, 100, 0.72, 0.75);
  rangesitter -> GetYaxis() -> SetTitleOffset(1.2);
	rangesitter -> GetXaxis() -> SetTitle("# simulated events");
  rangesitter -> GetYaxis() -> SetTitle("Simulated efficiency");
	rangesitter->SetTitleFont(43,"xy");
	rangesitter->SetTitleSize(20,"xy");

	gStyle->SetMarkerColor(kRed);
	pEff->SetLineColor(kRed);
	pEff->SetMarkerSize(0.8);
	pEff->SetMarkerStyle(20);
	rangesitter -> Draw();
	gStyle->SetOptStat(0);
	gStyle->SetLabelSize(0.4,"X");
	pEff->Draw("sameP");

	Double_t eff2 = pEff->GetEfficiency(1);
	Double_t ErLow = pEff -> GetEfficiencyErrorLow(1);
	Double_t ErUp = pEff -> GetEfficiencyErrorUp(1);
	printf("\nEfficiency = %f +- %f (up) +- %f (low)\n", eff2, ErUp, ErLow);
}
