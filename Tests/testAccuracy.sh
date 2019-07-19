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
python createTest.py 10 $INFO 100000 300000000 $CSV_DIR/test1.csv;
python createTest.py 10 $INFO 200000 120000000 $CSV_DIR/test2.csv;
python createTest.py 10 $INFO 100000 600000000 $CSV_DIR/test3.csv;
python createTest.py 10 $INFO 200000 600000000 $CSV_DIR/test4.csv;
python createTest.py 10 $INFO 200000 1200000000 $CSV_DIR/test5.csv;
" 2> /dev/null


echo "Preparing Node 2..."
ssh -t $NODE2 "
cd $TEST_DIR/get_net_usage;
make &> /dev/null;
" 2> /dev/null


echo "Resolving MAC Addresses."
IF="eno1d1"

P="[0-9a-fA-F]{2}"
# Source Ethernet
ETHERS=($(ssh $NODE1 ip a | egrep -o "$P:$P:$P:$P:$P:$P"))

i=0
I_S_ETHER=-1
for iface in $(ssh $NODE1 ip a | cut -d ' ' -f2 | tr -d ':'); do
		if [ $IF == $iface ]; then
            I_S_ETHER=$[$i * 2]
			break
        fi
        ((++i))
done

if [ $I_S_ETHER -eq -1 ]; then
	echo "Could not resolve Ethernet for interface $IF on Node 1."
	exit 1;
fi

S_ETHER=${ETHERS[$I_S_ETHER]:0:17}

# Destination Ethernet
ETHERS=($(ssh $NODE2 ip a | egrep -o "$P:$P:$P:$P:$P:$P"))

i=0
I_D_ETHER=-1
for iface in $(ssh $NODE2 ip a | cut -d ' ' -f2 | tr -d ':'); do
        if [ $IF == $iface ]; then
            I_D_ETHER=$[$i * 2]
            break
        fi
        ((++i))
done

if [ $I_D_ETHER -eq -1 ]; then
    echo "Could not resolve Ethernet for interface $IF on Node 2."
    exit 1;
fi

D_ETHER=${ETHERS[$I_D_ETHER]:0:17}


printf "\nEvaluating Test 1 ~ Packet Rate: 100,000 | Bit Rate: 300,000,000\n"

ssh $NODE1 "
cd $MAKE_DIR;
sudo ./packetize ./csv/test1.csv $S_ETHER $D_ETHER eno1d1;
" & 

ssh $NODE2 "
cd $TEST_DIR;
python evalMace.py 10 100000 300000000 eno1d1;
"


printf "\nEvaluating Test 2 ~ Packet Rate: 200,000 | Bit Rate: 120,000,000\n"

ssh $NODE1 "
cd $MAKE_DIR;
sudo ./packetize ./csv/test2.csv $S_ETHER $D_ETHER eno1d1;
" &

ssh $NODE2 "
cd $TEST_DIR;
python evalMace.py 10 200000 120000000 eno1d1;
"


printf "\nEvaluating Test 3 ~ Packet Rate: 100,000 | Bit Rate: 600,000,000\n"

ssh $NODE1 "
cd $MAKE_DIR;
sudo ./packetize ./csv/test3.csv $S_ETHER $D_ETHER eno1d1;
" &

ssh $NODE2 "
cd $TEST_DIR;
python evalMace.py 10 100000 600000000 eno1d1;
"


printf "\nEvaluating Test 4 ~ Packet Rate: 200,000 | Bit Rate: 600,000,000\n"

ssh $NODE1 "
cd $MAKE_DIR;
sudo ./packetize ./csv/test4.csv $S_ETHER $D_ETHER eno1d1;
" &

ssh $NODE2 "
cd $TEST_DIR;
python evalMace.py 10 200000 600000000 eno1d1;
"


printf "\nEvaluating Test 5 ~ Packet Rate: 200,000 | Bit Rate: 1,200,000,000\n"

ssh $NODE1 "
cd $MAKE_DIR;
sudo ./packetize ./csv/test5.csv $S_ETHER $D_ETHER eno1d1;
" &

ssh $NODE2 "
cd $TEST_DIR;
python evalMace.py 10 200000 1200000000 eno1d1;
"

<<notnow
printf "\nEvaluating Test 6 ~ Packet Rate: 5000 | Bit Rate: 1,200,000\n"

ssh $NODE1 "
cd $MAKE_DIR;
sudo ./packetize ./csv/test6.csv $S_ETHER $D_ETHER eno1d1;
" &

ssh $NODE2 "
cd $TEST_DIR;
python evalMace.py 10 5000 1200000 eno1d1;
"


printf "\nEvaluating Test 7 ~ Packet Rate: 5000 | Bit Rate: 4,800,000\n"

ssh $NODE1 "
cd $MAKE_DIR;
sudo ./packetize ./csv/test7.csv $S_ETHER $D_ETHER eno1d1;
" &

ssh $NODE2 "
cd $TEST_DIR;
python evalMace.py 10 5000 4800000 eno1d1;
"


printf "\nEvaluating Test 8 ~ Packet Rate: 50,000 | Bit Rate: 12,000,000\n"

ssh $NODE1 "
cd $MAKE_DIR;
sudo ./packetize ./csv/test8.csv $S_ETHER $D_ETHER eno1d1;
" &

ssh $NODE2 "
cd $TEST_DIR;
python evalMace.py 10 50000 12000000 eno1d1;
"


printf "\nEvaluating Test 9 ~ Packet Rate: 50,000 | Bit Rate: 30,000,000\n"

ssh $NODE1 "
cd $MAKE_DIR;
sudo ./packetize ./csv/test9.csv $S_ETHER $D_ETHER eno1d1;
" &

ssh $NODE2 "
cd $TEST_DIR;
python evalMace.py 10 200000 30000000 eno1d1;
"


printf "\nEvaluating Test 10 ~ Packet Rate: 50,000 | Bit Rate: 60,000,000\n"

ssh $NODE1 "
cd $MAKE_DIR;
sudo ./packetize ./csv/test10.csv $S_ETHER $D_ETHER eno1d1;
" &

ssh $NODE2 "
cd $TEST_DIR;
python evalMace.py 10 200000 60000000 eno1d1;
"


printf "\nEvaluating Test 11 ~ Packet Rate: 500,000 | Bit Rate: 120,000,000\n"

ssh $NODE1 "
cd $MAKE_DIR;
sudo ./packetize ./csv/test11.csv $S_ETHER $D_ETHER eno1d1;
" &

ssh $NODE2 "
cd $TEST_DIR;
python evalMace.py 10 200000 120000000 eno1d1;
"


printf "\nEvaluating Test 12 ~ Packet Rate: 500,000 | Bit Rate: 180,000,000\n"

ssh $NODE1 "
cd $MAKE_DIR;
sudo ./packetize ./csv/test12.csv $S_ETHER $D_ETHER eno1d1;
" &

ssh $NODE2 "
cd $TEST_DIR;
python evalMace.py 10 200000 180000000 eno1d1;
"


printf "\nEvaluating Test 13 ~ Packet Rate: 500,000 | Bit Rate: 300,000,000\n"

ssh $NODE1 "
cd $MAKE_DIR;
sudo ./packetize ./csv/test13.csv $S_ETHER $D_ETHER eno1d1;
" &

ssh $NODE2 "
cd $TEST_DIR;
python evalMace.py 10 200000 300000000 eno1d1;
"


printf "\nEvaluating Test 14 ~ Packet Rate: 500,000 | Bit Rate: 600,000,000\n"

ssh $NODE1 "
cd $MAKE_DIR;
sudo ./packetize ./csv/test14.csv $S_ETHER $D_ETHER eno1d1;
" &

ssh $NODE2 "
cd $TEST_DIR;
python evalMace.py 10 200000 600000000 eno1d1;
"


printf "\nEvaluating Test 15 ~ Packet Rate: 500,000 | Bit Rate: 1,200,000,000\n"

ssh $NODE1 "
cd $MAKE_DIR;
sudo ./packetize ./csv/test15.csv $S_ETHER $D_ETHER eno1d1;
" &

ssh $NODE2 "
cd $TEST_DIR;
python evalMace.py 10 200000 1200000000 eno1d1;
"


printf "\nEvaluating Test 16 ~ Packet Rate: 500,000 | Bit Rate: 2,400,000,000\n"

ssh $NODE1 "
cd $MAKE_DIR;
sudo ./packetize ./csv/test16.csv $S_ETHER $D_ETHER eno1d1;
" &

ssh $NODE2 "
cd $TEST_DIR;
python evalMace.py 10 200000 2400000000 eno1d1;
"


printf "\nEvaluating Test 17 ~ Packet Rate: 500,000 | Bit Rate: 4,800,000,000\n"

ssh $NODE1 "
cd $MAKE_DIR;
sudo ./packetize ./csv/test17.csv $S_ETHER $D_ETHER eno1d1;
" &

ssh $NODE2 "
cd $TEST_DIR;
python evalMace.py 10 200000 4800000000 eno1d1;
"


printf "\nEvaluating Test 18 ~ Packet Rate: 800,000 | Bit Rate: 9,600,000,000\n"

ssh $NODE1 "
cd $MAKE_DIR;
sudo ./packetize ./csv/test18.csv $S_ETHER $D_ETHER eno1d1;
" &

ssh $NODE2 "
cd $TEST_DIR;
python evalMace.py 10 200000 9600000000 eno1d1;
"
notnow
