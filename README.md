# Triple GEM Analysis and Visualisation package

Framework that reads, processes and plots the oscilloscope data collected in the testing of a Triple-GEM detector prototype.
The original data are not shared here, but the final plots can be seen in the _[official report](https://www.lnf.infn.it/sis/preprint/detail-new.php?id=5396)_.
&nbsp;

## :triangular_flag_on_post: List of scripts
Macros are made to be run through ROOT CERN, simply doing `root -l macro.cxx`:

* __Baseline.C__
Reads list of measurements from different files and calculates the average baseline

* __ADC.C__
Reads and plots ADC entries

* __ShowSignals.cpp__
Produces a plot resembling the oscilloscope output and saves it into a root file, to be opened with ReadSignals.cpp
The signals shown are without baseline correction

* __ShowSignalsBSC.cpp__
Produces a plot resembling the oscilloscope output and saves it into a root file, to be opened with ReadSignals.cpp
Baseline correction is applied to the signals.

* __ReadSignals.cpp__
Reads the plot produced by ShowSignals.cpp and prints it in good fashion

* __ExtractData.sh__
Opens each directory (containing a set of measurements for a given value of V) and produces the list of signal files in each directoy, reads the number of signal files, and produces the list of measurement sets

* __Sigma_VsHV.cpp__
Produces the sigma and std vs HV plot for mixture 1 and mixture 2

* __Sigma_VsEff.cpp__
Produces the sigma vs eff plot for mixture 1, mixture 2 and both mixtures together

* __EffVshv.cpp__
Calculates the efficiency measured with two gas mixtures and produces the efficiency vs Voltage for each mixture. The error on efficicency is calculated using binomial statistics.

* __effs.cpp__
Measures the efficiency from data, estimated as the number of triple coincidences / number of double coincidences. The uncertainty is estimated in the script "sigma_VsEff", using the TEfficiency class of root

* __montecarlomiomultieff.cpp__
Simulates triple and double concidences with triple GEM detector setup using random numbers of cosmic muons.
The simulted efficiency is estimated as N(Triple coinc.)/N(Double coinc.)
Plots the estimated efficiency for each number of simulated muons.

