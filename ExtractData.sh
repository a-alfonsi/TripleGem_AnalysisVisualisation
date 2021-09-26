#!/bin/bash
# To make the script executable: chmod u+x ExtractData.sh
# To run the script: ./ExtractData.sh
#
# Opens each directory (containing a set of measurements for a given value of V)
# and produces the list of signal files in each directoy, reads the number of
# signal files, and produces the list of measurement sets
#
echo
echo "-------------------------------------------------"
echo "Opening each measurement set directory:"
for f in *; do
    if [[ -d "$f" && ! -L "$f" ]]; then
        echo
        echo $f
        echo "Producing the list of signal files;"
        find "$PWD/$f" -name '*.txt' > listsignals_$f.dat
        echo "Reading the number of signal files;"
        NUMOFLINES=$(wc -l < listsignals_$f.dat)
        echo $NUMOFLINES > Nevents_$f.dat
    fi
done
echo
echo "Producing the list of measurement sets"
find . -name 'listsignals_*.dat' | sed s/'.\/'//g > HVSets.dat
echo "-------------------------------------------------"
echo