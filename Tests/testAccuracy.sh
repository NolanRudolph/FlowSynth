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
pr13=416667
pr14=458334
pr15=500000
pr16=541667
pr17=583334
pr18=625000
pr19=666667
pr20=708334
pr21=750000
pr22=791667
pr23=833334

# Modify me when removing/adding new packet rates
printf "$pr1\n$pr2\n$pr3\n$pr4\n$pr5\n$pr6\n$pr7\n$pr8\n" > pRates.txt
printf "$pr9\n$pr10\n$pr11\n$pr12\n$pr13\n$pr14\n$pr15\n$pr16\n" >> pRates.txt
printf "$pr17\n$pr18\n$pr19\n$pr20\n$pr21\n$pr22\n$pr23" >> pRates.txt

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
br14=5500000000
br15=6000000000
br16=6500000000
br17=7000000000
br18=7500000000
br19=8000000000
br20=8500000000
br21=9000000000
br22=9500000000
br23=10000000000
# Modify me when removing/adding new packet rates
printf "$br1\n$br2\n$br3\n$br4\n$br5\n$br6\n$br7\n$br8\n" > bRates.txt
printf "$br9\n$br10\n$br11\n$br12\n$br13\n$br14\n$br15\n$br16\n" >> bRates.txt
printf "$br17\n$br18\n$br19\n$br20\n$br21\n$br22\n$br23" >> bRates.txt


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
python createTest.py 10 $INFO $pr14 $br14 $CSV_DIR/test14.csv;
python createTest.py 10 $INFO $pr15 $br15 $CSV_DIR/test15.csv;
python createTest.py 10 $INFO $pr16 $br16 $CSV_DIR/test16.csv;
python createTest.py 10 $INFO $pr17 $br17 $CSV_DIR/test17.csv;
python createTest.py 10 $INFO $pr18 $br18 $CSV_DIR/test18.csv;
python createTest.py 10 $INFO $pr19 $br19 $CSV_DIR/test19.csv;
python createTest.py 10 $INFO $pr20 $br20 $CSV_DIR/test20.csv;
python createTest.py 10 $INFO $pr21 $br21 $CSV_DIR/test21.csv;
python createTest.py 10 $INFO $pr22 $br22 $CSV_DIR/test22.csv;
python createTest.py 10 $INFO $pr23 $br23 $CSV_DIR/test23.csv;
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

function testCase {
	# Arg 1: Test Number
	# Arg 2: Packet Rate
	# Arg 3: Bit Rate
	# Arg 4: CSV File
	if [ $# -ne 4 ]; then
		echo "Invalid call to testCase()."
		exit 1
	fi

	printf "\nEvaluating Test $1 ~ Packet Rate: $2 | Bit Rate: $3\n"

	ssh $NODE1 "
		cd $MAKE_DIR;
		sudo ./packetize $4 $S_ETHER $D_ETHER enp1s0d1;
	" &

	if [ $1 -eq 1 ]; then
		ssh $NODE2 "
			cd $TEST_DIR;
			python evalMace.py 10 $2 $3 enp1s0d1;
		" | tee testResults.txt
	else
		ssh $NODE2 "
			cd $TEST_DIR;
			python evalMace.py 10 $2 $3 enp1s0d1;
		" | tee -a testResults.txt 
	fi
}

testCase 1 $pr1 $br1 ./csv/test1.csv
testCase 2 $pr2 $br2 ./csv/test2.csv
testCase 3 $pr3 $br3 ./csv/test3.csv
testCase 4 $pr4 $br4 ./csv/test4.csv
testCase 5 $pr5 $br5 ./csv/test5.csv
testCase 6 $pr6 $br6 ./csv/test6.csv
testCase 7 $pr7 $br7 ./csv/test7.csv
testCase 8 $pr8 $br8 ./csv/test8.csv
testCase 9 $pr9 $br9 ./csv/test9.csv
testCase 10 $pr10 $br10 ./csv/test10.csv
testCase 11 $pr11 $br11 ./csv/test11.csv
testCase 12 $pr12 $br12 ./csv/test12.csv
testCase 13 $pr13 $br13 ./csv/test13.csv
testCase 14 $pr14 $br14 ./csv/test14.csv
testCase 15 $pr15 $br15 ./csv/test15.csv
testCase 16 $pr16 $br16 ./csv/test16.csv
testCase 17 $pr17 $br17 ./csv/test17.csv
testCase 18 $pr18 $br18 ./csv/test18.csv
testCase 19 $pr19 $br19 ./csv/test19.csv
testCase 20 $pr20 $br20 ./csv/test20.csv
testCase 21 $pr21 $br21 ./csv/test21.csv
testCase 22 $pr22 $br22 ./csv/test22.csv
testCase 23 $pr23 $br23 ./csv/test23.csv


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

<<testing
# Graph Generation
printf "Generating Performance Graph\n"
python graph.py pRates.txt packetResults.txt bRates.txt bitResults.txt
testing
