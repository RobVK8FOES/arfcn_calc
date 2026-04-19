arfcn_calc

A simple C program to convert GSM ARFCN numbers to downlink frequency, and vice versa.
Prevents the laborious process of browsing to an online ARFCN list webpage. 
Tested and working on DragonOS FocalX R37.


BUILD AND INSTALL:

cd ~/

git clone https://github.com/RobVK8FOES/arfcn_calc.git

cd ~/arfcn_calc

g++ -O3 arfcn_calc.cpp -o arfcn_calc

sudo cp ~/arfcn_calc/arfcn_calc /usr/local/bin/arfcn_calc


CLEANUP (OPTIONAL):

sudo rm -r ~/arfcn_calc/


EXAMPLE USAGE:

arfcn_calc 122 OR arfcn_calc 959.4
