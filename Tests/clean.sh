#!/bin/bash

ARGC=$#

# Argument Validation
if [ $ARGC -ne 2 ]; then
	echo "Please use as $ ./$0 [Node 1 Addr] [Node 2 Addr]"
	exit 1;
fi

NODE1=$1
NODE2=$2

# Node Validation
if [ $(ssh $NODE1 echo Hello) ] && [ $(ssh $NODE2 echo Hello) ]; then
	echo "Nodes appear to be functional."
else
	echo "Nodes are not accessible. Exiting."
	exit 1;
fi

# Node1 will be the issuer, Node 2 will be the listener
# Node Setups
echo "Node 1: Removing Repository."
ssh $NODE1 sudo rm -rf UONetflowC
echo "Node 2: Removing Repository."
ssh $NODE2 sudo rm -rf UONetflowC

# Remove unneeded text files
echo "Cleaning workspace."
rm pRates.txt
rm packetResults.txt
rm bRates.txt
rm bitResults.txt
rm results.txt
rm testResults.txt
