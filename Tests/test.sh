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
printf "Node 1: "; ssh $NODE1 git clone https://github.com/NolanRudolph/UONetflowC
printf "Node 2: "; ssh $NODE2 git clone https://github.com/NolanRudolph/UONetflowC

INFO="127.0.0.1 127.0.0.2 443 57192 6"
TEST_DIR="~/UONetflowC/Tests"
MAKE_DIR="~/UONetflowC/TimelySend"
CSV_DIR="~/UONetflowC/TimelySend/csv"

ssh -t $NODE1 "
cd $MAKE_DIR;
make;
cd $TEST_DIR;
python createTest.py 20 $INFO 1 1000 $CSV_DIR/test1.csv;
python createTest.py 20 $INFO 10 10000 $CSV_DIR/test2.csv;
python createTest.py 20 $INFO 100 100000 $CSV_DIR/test3.csv;
python createTest.py 20 $INFO 1000 1000000 $CSV_DIR/test4.csv;
" 2> /dev/null

echo $(ssh $NODE1 cat ./UONetflowC/TimelySend/csv/test3.csv)


