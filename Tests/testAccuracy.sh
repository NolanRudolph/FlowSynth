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

echo "Preparing Node 1..."
ssh -t $NODE1 "
cd $MAKE_DIR;
make &> /dev/null;
mkdir csv;
cd $TEST_DIR;
python createTest.py 10 $INFO 1 1000 $CSV_DIR/test1.csv;
python createTest.py 10 $INFO 10 10000 $CSV_DIR/test2.csv;
python createTest.py 10 $INFO 100 100000 $CSV_DIR/test3.csv;
python createTest.py 10 $INFO 1000 1000000 $CSV_DIR/test4.csv;
python createTest.py 10 $INFO 10000 10000000 $CSV_DIR/test5.csv;
python createTest.py 10 $INFO 100000 100000000 $CSV_DIR/test6.csv;
python createTest.py 10 $INFO 1000000 1000000000 $CSV_DIR/test7.csv;
python createTest.py 10 $INFO 10000000 10000000000 $CSV_DIR/test8.csv;
" 2> /dev/null

echo "Preparing Node 2..."
ssh -t $NODE2 "
cd $TEST_DIR;
cd get_net_usage;
make &> /dev/null;
" 2> /dev/null

echo "Evaluating Test 1 ~ Packet Rate: 1 | Bit Rate: 1000"
ssh $NODE1 "
cd $MAKE_DIR;
sudo ./packetize ./csv/test1.csv 00:01:02:03:04:05 00:06:07:08:09:10 eno1d1;
" &

ssh $NODE2 "
cd $TEST_DIR;
python evalMace.py 10 1 1000;
" 



