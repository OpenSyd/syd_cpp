#!/bin/sh

sydTestSpectImage --noise 0

sydCreateTimeIntegratedSpectImage --images test-spect0.mhd,test-spect1.mhd,test-spect2.mhd,test-spect3.mhd,test-spect4.mhd,test-spect5.mhd --times 1.0,4.7,23.9,47.9,71.8,143.8 -o bidon.mhd

clitkImageArithm -i integral.mhd -j bidon.mhd -t 8 -o diff.mhd
clitkImageArithm -i diff.mhd -s 100 -t 1 -o diff.mhd

clitkImageStatistics -i diff.mhd -v

clitkImageStatistics -i diff.mhd > last_output.txt

# compare results with reference
r=`diff last_output.txt reference_output.txt`
echo $r
if [ -z "$r" ]
then
    echo "OK"
    exit 0
else
    echo "Error !!"
    exit 1
fi
