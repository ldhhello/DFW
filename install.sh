#!/bin/sh

if [[ $EUID -ne 0 ]]; then
   echo "This script must be run as root" 
   exit 1
fi

if ! command -v g++ &> /dev/null
then
    echo "g++ could not be found."
    echo "Please install g++ to install it."
    exit 1
fi

echo 'Installing DFW Firewall...'
echo "Please make sure you have installed g++ Compiler and Boost Library. If not, you can't install it."

cd DFW

echo 'Compiling DFW...'
g++ -o dfw -Wall -O2 -std=c++11 *.cpp -I. -lboost_system -lpthread -lboost_program_options 

cd ..
cd DFWDaemon

echo 'Compiling DFW Daemon...'
g++ -o dfw_daemon -Wall -O2 -std=c++11 *.cpp -I. -lboost_system -lpthread

cd ..
echo 'Finished to compile!'

echo 'Installing DFW...'
cp DFW/dfw /bin/dfw -f
cp DFWDaemon/dfw_daemon /bin/dfw-daemon -f

cp dfw.service /etc/systemd/system/dfw.service
systemctl daemon-reload

echo 'Finished to install.'
echo "To start the daemon, please run 'systemctl start dfw'"
echo "To make the daemon start at boot, please run 'systemctl enable dfw'"
