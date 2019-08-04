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


pr1=1
pr2=10
pr3=100
pr4=1000
pr5=10000
pr6=100000
pr7=100000
pr8=200000
pr9=300000
pr10=350000
pr11=350000
pr12=375000
pr13=416666

br1=1000
br2=10000
br3=100000
br4=1000000
br5=10000000
br6=100000000
br7=1000000000
br8=2000000000
br9=3000000000
br10=3500000000
br11=4000000000
br12=4500000000
br13=5000000000

echo "Preparing Node 1..."
ssh -t $NODE1 "
cd $MAKE_DIR;
make &> /dev/null;
mkdir csv;
cd $TEST_DIR;
python createTest.py 10 $INFO $pr1 $br1 $CSV_DIR/test1.csv;
python createTest.py 10 $INFO $pr2 $br2 $CSV_DIR/test2.csv;
python createTest.py 10 $INFO $pr3 $br3 $CSV_DIR/test3.csv;
python createTest.py 10 $INFO $pr4 $br4 $CSV_DIR/test4.csv;
python createTest.py 10 $INFO $pr5 $br5 $CSV_DIR/test5.csv;
python createTest.py 10 $INFO $pr6 $br6 $CSV_DIR/test6.csv;
python createTest.py 10 $INFO $pr7 $br7 $CSV_DIR/test7.csv;
python createTest.py 10 $INFO $pr8 $br8 $CSV_DIR/test8.csv;
python createTest.py 10 $INFO $pr9 $br9 $CSV_DIR/test9.csv;
python createTest.py 10 $INFO $pr10 $br10 $CSV_DIR/test10.csv;
python createTest.py 10 $INFO $pr11 $br11 $CSV_DIR/test11.csv;
python createTest.py 10 $INFO $pr12 $br12 $CSV_DIR/test12.csv;
python createTest.py 10 $INFO $pr13 $br13 $CSV_DIR/test13.csv;
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


printf "\nEvaluating Test 1 ~ Packet Rate: $pr1 | Bit Rate: $br1\n"

ssh $NODE1 "
cd $MAKE_DIR;
sudo ./packetize ./csv/test1.csv $S_ETHER $D_ETHER enp1s0d1;
" & 

ssh $NODE2 "
cd $TEST_DIR;
python evalMace.py 10 $pr1 $br1 enp1s0d1;
" | tee testResults.txt


printf "\nEvaluating Test 2 ~ Packet Rate: $pr2 | Bit Rate: $br2\n"

ssh $NODE1 "
cd $MAKE_DIR;
sudo ./packetize ./csv/test2.csv $S_ETHER $D_ETHER enp1s0d1;
" &

ssh $NODE2 "
cd $TEST_DIR;
python evalMace.py 10 $pr2 $br2 enp1s0d1;
" | tee -a testResults.txt


printf "\nEvaluating Test 3 ~ Packet Rate: $pr3 | Bit Rate: $br3\n"

ssh $NODE1 "
cd $MAKE_DIR;
sudo ./packetize ./csv/test3.csv $S_ETHER $D_ETHER enp1s0d1;
" &

ssh $NODE2 "
cd $TEST_DIR;
python evalMace.py 10 $pr3 $br3 enp1s0d1;
" | tee -a testResults.txt


printf "\nEvaluating Test 4 ~ Packet Rate: $pr4 | Bit Rate: $br4\n"

ssh $NODE1 "
cd $MAKE_DIR;
sudo ./packetize ./csv/test4.csv $S_ETHER $D_ETHER enp1s0d1;
" &

ssh $NODE2 "
cd $TEST_DIR;
python evalMace.py 10 $pr4 $br4 enp1s0d1;
" | tee -a testResults.txt


printf "\nEvaluating Test 5 ~ Packet Rate: $pr5 | Bit Rate: $br5\n"

ssh $NODE1 "
cd $MAKE_DIR;
sudo ./packetize ./csv/test5.csv $S_ETHER $D_ETHER enp1s0d1;
" &

ssh $NODE2 "
cd $TEST_DIR;
python evalMace.py 10 $pr5 $br5 enp1s0d1;
" | tee -a testResults.txt

printf "\nEvaluating Test 6 ~ Packet Rate: $pr6 | Bit Rate: $br6\n"

ssh $NODE1 "
cd $MAKE_DIR;
sudo ./packetize ./csv/test6.csv $S_ETHER $D_ETHER enp1s0d1;
" &

ssh $NODE2 "
cd $TEST_DIR;
python evalMace.py 10 $pr6 $br6 enp1s0d1;
" | tee -a testResults.txt


printf "\nEvaluating Test 7 ~ Packet Rate: $pr7 | Bit Rate: $br7\n"

ssh $NODE1 "
cd $MAKE_DIR;
sudo ./packetize ./csv/test7.csv $S_ETHER $D_ETHER enp1s0d1;
" &

ssh $NODE2 "
cd $TEST_DIR;
python evalMace.py 10 $pr7 $br7 enp1s0d1;
" | tee -a testResults.txt


printf "\nEvaluating Test 8 ~ Packet Rate: $pr8 | Bit Rate: $br8\n"

ssh $NODE1 "
cd $MAKE_DIR;
sudo ./packetize ./csv/test8.csv $S_ETHER $D_ETHER enp1s0d1;
" &

ssh $NODE2 "
cd $TEST_DIR;
python evalMace.py 10 $pr8 $br8 enp1s0d1;
" | tee -a testResults.txt


printf "\nEvaluating Test 9 ~ Packet Rate: $pr9 | Bit Rate: $br9\n"

ssh $NODE1 "
cd $MAKE_DIR;
sudo ./packetize ./csv/test9.csv $S_ETHER $D_ETHER enp1s0d1;
" &

ssh $NODE2 "
cd $TEST_DIR;
python evalMace.py 10 $pr9 $br9 enp1s0d1;
" | tee -a testResults.txt


printf "\nEvaluating Test 10 ~ Packet Rate: $pr10 | Bit Rate: $br10\n"

ssh $NODE1 "
cd $MAKE_DIR;
sudo ./packetize ./csv/test10.csv $S_ETHER $D_ETHER enp1s0d1;
" &

ssh $NODE2 "
cd $TEST_DIR;
python evalMace.py 10 $pr10 $br10 enp1s0d1;
" | tee -a testResults.txt


printf "\nEvaluating Test 11 ~ Packet Rate: $pr11 | Bit Rate: $br11\n"

ssh $NODE1 "
cd $MAKE_DIR;
sudo ./packetize ./csv/test11.csv $S_ETHER $D_ETHER enp1s0d1;
" &

ssh $NODE2 "
cd $TEST_DIR;
python evalMace.py 10 $pr11 $br11 enp1s0d1;
" | tee -a testResults.txt


printf "\nEvaluating Test 12 ~ Packet Rate: $pr12 | Bit Rate: $br12\n"

ssh $NODE1 "
cd $MAKE_DIR;
sudo ./packetize ./csv/test12.csv $S_ETHER $D_ETHER enp1s0d1;
" &

ssh $NODE2 "
cd $TEST_DIR;
python evalMace.py 10 $pr12 $br12 enp1s0d1;
" | tee -a testResults.txt


printf "\nEvaluating Test 13 ~ Packet Rate: $pr13 | Bit Rate: $br13\n"

ssh $NODE1 "
cd $MAKE_DIR;
sudo ./packetize ./csv/test13.csv $S_ETHER $D_ETHER enp1s0d1;
" &

ssh $NODE2 "
cd $TEST_DIR;
python evalMace.py 10 $pr13 $br13 enp1s0d1;
" | tee -a testResults.txt


# Truncating results for easier reading
cat testResults.txt | egrep -o "[0-9]+.[0-9]+%" > results.txt

# Creating Text files for graphing
touch packetResults.txt
touch bitResults.txt
lineN=0
while read p; do
    if [ $[$lineN % 2] -eq 0 ]; then
        echo "${p//%}" >> packetResults.txt
    else
        echo "${p//%}" >> bitResults.txt
    fi
    ((++lineN))
done < results.txt
