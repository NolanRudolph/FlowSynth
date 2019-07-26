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
python createTest.py 10 $INFO 5 6000 $CSV_DIR/test1.csv;
python createTest.py 10 $INFO 50 18400 $CSV_DIR/test2.csv;
python createTest.py 10 $INFO 250 140000 $CSV_DIR/test3.csv;
python createTest.py 10 $INFO 500 736000 $CSV_DIR/test4.csv;
python createTest.py 10 $INFO 2500 1400000 $CSV_DIR/test5.csv;
python createTest.py 10 $INFO 5000 7360000 $CSV_DIR/test6.csv;
python createTest.py 10 $INFO 25000 14000000 $CSV_DIR/test7.csv;
python createTest.py 10 $INFO 50000 73600000 $CSV_DIR/test8.csv;
python createTest.py 10 $INFO 12500 100000000 $CSV_DIR/test9.csv;
python createTest.py 10 $INFO 12500 120000000 $CSV_DIR/test10.csv;
python createTest.py 10 $INFO 25000 140000000 $CSV_DIR/test11.csv;
" 2> /dev/null


echo "Preparing Node 2..."
ssh -t $NODE2 "
cd $TEST_DIR/get_net_usage;
make &> /dev/null;
" 2> /dev/null


echo "Resolving MAC Addresses."
IF="enp1s0d1"

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


printf "\nEvaluating Test 1 ~ Packet Rate: 5 | Bit Rate: 6,000\n"

ssh $NODE1 "
cd $MAKE_DIR;
sudo ./packetize ./csv/test1.csv $S_ETHER $D_ETHER enp1s0d1;
" & 

ssh $NODE2 "
cd $TEST_DIR;
python evalMace.py 10 5 6000 enp1s0d1;
"


printf "\nEvaluating Test 2 ~ Packet Rate: 50 | Bit Rate: 18,400\n"

ssh $NODE1 "
cd $MAKE_DIR;
sudo ./packetize ./csv/test2.csv $S_ETHER $D_ETHER enp1s0d1;
" &

ssh $NODE2 "
cd $TEST_DIR;
python evalMace.py 10 50 18400 enp1s0d1;
"


printf "\nEvaluating Test 3 ~ Packet Rate: 250 | Bit Rate: 140,000\n"

ssh $NODE1 "
cd $MAKE_DIR;
sudo ./packetize ./csv/test3.csv $S_ETHER $D_ETHER enp1s0d1;
" &

ssh $NODE2 "
cd $TEST_DIR;
python evalMace.py 10 250 140000 enp1s0d1;
"


printf "\nEvaluating Test 4 ~ Packet Rate: 500 | Bit Rate: 736,000\n"

ssh $NODE1 "
cd $MAKE_DIR;
sudo ./packetize ./csv/test4.csv $S_ETHER $D_ETHER enp1s0d1;
" &

ssh $NODE2 "
cd $TEST_DIR;
python evalMace.py 10 500 736000 enp1s0d1;
"


printf "\nEvaluating Test 5 ~ Packet Rate: 2,500 | Bit Rate: 1,400,000\n"

ssh $NODE1 "
cd $MAKE_DIR;
sudo ./packetize ./csv/test5.csv $S_ETHER $D_ETHER enp1s0d1;
" &

ssh $NODE2 "
cd $TEST_DIR;
python evalMace.py 10 2500 1400000 enp1s0d1;
"

printf "\nEvaluating Test 6 ~ Packet Rate: 5,000 | Bit Rate: 7,360,000\n"

ssh $NODE1 "
cd $MAKE_DIR;
sudo ./packetize ./csv/test6.csv $S_ETHER $D_ETHER enp1s0d1;
" &

ssh $NODE2 "
cd $TEST_DIR;
python evalMace.py 10 5000 7360000 enp1s0d1;
"


printf "\nEvaluating Test 7 ~ Packet Rate: 25,000 | Bit Rate: 14,000,000\n"

ssh $NODE1 "
cd $MAKE_DIR;
sudo ./packetize ./csv/test7.csv $S_ETHER $D_ETHER enp1s0d1;
" &

ssh $NODE2 "
cd $TEST_DIR;
python evalMace.py 10 25000 14000000 enp1s0d1;
"


printf "\nEvaluating Test 8 ~ Packet Rate: 50,000 | Bit Rate: 73,600,000\n"

ssh $NODE1 "
cd $MAKE_DIR;
sudo ./packetize ./csv/test8.csv $S_ETHER $D_ETHER enp1s0d1;
" &

ssh $NODE2 "
cd $TEST_DIR;
python evalMace.py 10 50000 73600000 enp1s0d1;
"


printf "\nEvaluating Test 9 ~ Packet Rate: 12,500 | Bit Rate: 100,000,000\n"

ssh $NODE1 "
cd $MAKE_DIR;
sudo ./packetize ./csv/test9.csv $S_ETHER $D_ETHER enp1s0d1;
" &

ssh $NODE2 "
cd $TEST_DIR;
python evalMace.py 10 12500 100000000 enp1s0d1;
"

printf "\n\nThis is where it gets interesting. Bit Rate cannot exceed 100,000,000?\n\n"

printf "\nEvaluating Test 10 ~ Packet Rate: 12,500 | Bit Rate: 120,000,000\n"

ssh $NODE1 "
cd $MAKE_DIR;
sudo ./packetize ./csv/test10.csv $S_ETHER $D_ETHER enp1s0d1;
" &

ssh $NODE2 "
cd $TEST_DIR;
python evalMace.py 10 12500 120000000 enp1s0d1;
"


printf "\nEvaluating Test 11 ~ Packet Rate: 25,000 | Bit Rate: 140,000,000\n"

ssh $NODE1 "
cd $MAKE_DIR;
sudo ./packetize ./csv/test11.csv $S_ETHER $D_ETHER enp1s0d1;
" &

ssh $NODE2 "
cd $TEST_DIR;
python evalMace.py 10 25000 140000000 enp1s0d1;
"
