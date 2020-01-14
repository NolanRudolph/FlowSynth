#!/bin/bash


ARGC=$#

# Argument Validation
if [ $ARGC -ne 3 ]; then
	echo "Please use as $ ./$0 [Node 1 Addr] [Node 2 Addr] [Interface]"
	exit 1;
fi


NODE1=$1
NODE2=$2
IF=$3

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
pr12=380000
pr13=420000
pr14=460000
pr15=510000
pr16=550000
pr17=590000
pr18=625000
pr19=675000
pr20=725000
pr21=775000
pr22=800000
pr23=850000
pr24=875000
pr25=950000
pr26=975000
pr27=1000000
pr28=1050000
pr29=1100000
pr30=1125000
pr31=1175000
pr32=1225000
pr33=1275000

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
br24=10500000000
br25=11000000000
br26=11500000000
br27=12000000000
br28=12500000000
br29=13000000000
br30=13500000000
br31=14000000000
br32=14500000000
br33=15000000000
# Modify me when removing/adding new packet rates
printf "$br1\n$br2\n$br3\n$br4\n$br5\n$br6\n$br7\n$br8\n" > bRates.txt
printf "$br9\n$br10\n$br11\n$br12\n$br13\n$br14\n$br15\n$br16\n" >> bRates.txt
printf "$br17\n$br18\n$br19\n$br20\n$br21\n$br22\n$br23" >> bRates.txt


echo "Preparing Node 1..."
ssh -t $NODE1 "
cd $MAKE_DIR;
make &> /dev/null;
sudo mkdir csv;
cd $TEST_DIR;
sudo python createTest.py 10 $INFO $pr1 $br1 $CSV_DIR/test1.csv;
sudo python createTest.py 10 $INFO $pr2 $br2 $CSV_DIR/test2.csv;
sudo python createTest.py 10 $INFO $pr3 $br3 $CSV_DIR/test3.csv;
sudo python createTest.py 10 $INFO $pr4 $br4 $CSV_DIR/test4.csv;
sudo python createTest.py 10 $INFO $pr5 $br5 $CSV_DIR/test5.csv;
sudo python createTest.py 10 $INFO $pr6 $br6 $CSV_DIR/test6.csv;
sudo python createTest.py 10 $INFO $pr7 $br7 $CSV_DIR/test7.csv;
sudo python createTest.py 10 $INFO $pr8 $br8 $CSV_DIR/test8.csv;
sudo python createTest.py 10 $INFO $pr9 $br9 $CSV_DIR/test9.csv;
sudo python createTest.py 10 $INFO $pr10 $br10 $CSV_DIR/test10.csv;
sudo python createTest.py 10 $INFO $pr11 $br11 $CSV_DIR/test11.csv;
sudo python createTest.py 10 $INFO $pr12 $br12 $CSV_DIR/test12.csv;
sudo python createTest.py 10 $INFO $pr13 $br13 $CSV_DIR/test13.csv;
sudo python createTest.py 10 $INFO $pr14 $br14 $CSV_DIR/test14.csv;
sudo python createTest.py 10 $INFO $pr15 $br15 $CSV_DIR/test15.csv;
sudo python createTest.py 10 $INFO $pr16 $br16 $CSV_DIR/test16.csv;
sudo python createTest.py 10 $INFO $pr17 $br17 $CSV_DIR/test17.csv;
sudo python createTest.py 10 $INFO $pr18 $br18 $CSV_DIR/test18.csv;
sudo python createTest.py 10 $INFO $pr19 $br19 $CSV_DIR/test19.csv;
sudo python createTest.py 10 $INFO $pr20 $br20 $CSV_DIR/test20.csv;
sudo python createTest.py 10 $INFO $pr21 $br21 $CSV_DIR/test21.csv;
sudo python createTest.py 10 $INFO $pr22 $br22 $CSV_DIR/test22.csv;
sudo python createTest.py 10 $INFO $pr23 $br23 $CSV_DIR/test23.csv;
sudo python createTest.py 10 $INFO $pr24 $br24 $CSV_DIR/test24.csv;
sudo python createTest.py 10 $INFO $pr25 $br25 $CSV_DIR/test25.csv;
sudo python createTest.py 10 $INFO $pr26 $br26 $CSV_DIR/test26.csv;
sudo python createTest.py 10 $INFO $pr27 $br27 $CSV_DIR/test27.csv;
sudo python createTest.py 10 $INFO $pr28 $br28 $CSV_DIR/test28.csv;
sudo python createTest.py 10 $INFO $pr29 $br29 $CSV_DIR/test29.csv;
sudo python createTest.py 10 $INFO $pr30 $br30 $CSV_DIR/test30.csv;
sudo python createTest.py 10 $INFO $pr31 $br31 $CSV_DIR/test31.csv;
sudo python createTest.py 10 $INFO $pr32 $br32 $CSV_DIR/test32.csv;
sudo python createTest.py 10 $INFO $pr33 $br33 $CSV_DIR/test33.csv;
" # > /dev/null 2> /dev/null


<<multiFlows
packrate=833334
bitrate=1000000000

# Testing Multiple Flows
ssh -t $NODE1 "
cd $MAKE_DIR;
make &> /dev/null;
mkdir csv;
cd $TEST_DIR;
python createNTest.py 1 10 $INFO $packrate $bitrate $CSV_DIR/test1.csv;
python createNTest.py 2 10 $INFO $packrate $bitrate $CSV_DIR/test2.csv;
python createNTest.py 3 10 $INFO $packrate $bitrate $CSV_DIR/test3.csv;
python createNTest.py 4 10 $INFO $packrate $bitrate $CSV_DIR/test4.csv;
python createNTest.py 5 10 $INFO $packrate $bitrate $CSV_DIR/test5.csv;
python createNTest.py 6 10 $INFO $packrate $bitrate $CSV_DIR/test6.csv;
python createNTest.py 7 10 $INFO $packrate $bitrate $CSV_DIR/test7.csv;
python createNTest.py 8 10 $INFO $packrate $bitrate $CSV_DIR/test8.csv;
python createNTest.py 9 10 $INFO $packrate $bitrate $CSV_DIR/test9.csv;
python createNTest.py 10 10 $INFO $packrate $bitrate $CSV_DIR/test10.csv;
python createNTest.py 11 10 $INFO $packrate $bitrate $CSV_DIR/test11.csv;
python createNTest.py 12 10 $INFO $packrate $bitrate $CSV_DIR/test12.csv;
python createNTest.py 13 10 $INFO $packrate $bitrate $CSV_DIR/test13.csv;
python createNTest.py 14 10 $INFO $packrate $bitrate $CSV_DIR/test14.csv;
python createNTest.py 15 10 $INFO $packrate $bitrate $CSV_DIR/test15.csv;
python createNTest.py 16 10 $INFO $packrate $bitrate $CSV_DIR/test16.csv;
python createNTest.py 17 10 $INFO $packrate $bitrate $CSV_DIR/test17.csv;
python createNTest.py 18 10 $INFO $packrate $bitrate $CSV_DIR/test18.csv;
python createNTest.py 19 10 $INFO $packrate $bitrate $CSV_DIR/test19.csv;
python createNTest.py 20 10 $INFO $packrate $bitrate $CSV_DIR/test20.csv;
python createNTest.py 21 10 $INFO $packrate $bitrate $CSV_DIR/test21.csv;
python createNTest.py 22 10 $INFO $packrate $bitrate $CSV_DIR/test22.csv;
python createNTest.py 23 10 $INFO $packrate $bitrate $CSV_DIR/test23.csv;
python createNTest.py 24 10 $INFO $packrate $bitrate $CSV_DIR/test24.csv;
python createNTest.py 25 10 $INFO $packrate $bitrate $CSV_DIR/test25.csv;
python createNTest.py 26 10 $INFO $packrate $bitrate $CSV_DIR/test26.csv;
python createNTest.py 27 10 $INFO $packrate $bitrate $CSV_DIR/test27.csv;
python createNTest.py 28 10 $INFO $packrate $bitrate $CSV_DIR/test28.csv;
python createNTest.py 29 10 $INFO $packrate $bitrate $CSV_DIR/test29.csv;
python createNTest.py 30 10 $INFO $packrate $bitrate $CSV_DIR/test30.csv;
python createNTest.py 31 10 $INFO $packrate $bitrate $CSV_DIR/test31.csv;
python createNTest.py 32 10 $INFO $packrate $bitrate $CSV_DIR/test32.csv;
python createNTest.py 33 10 $INFO $packrate $bitrate $CSV_DIR/test33.csv;
python createNTest.py 34 10 $INFO $packrate $bitrate $CSV_DIR/test34.csv;
python createNTest.py 35 10 $INFO $packrate $bitrate $CSV_DIR/test35.csv;
python createNTest.py 36 10 $INFO $packrate $bitrate $CSV_DIR/test36.csv;
python createNTest.py 37 10 $INFO $packrate $bitrate $CSV_DIR/test37.csv;
python createNTest.py 38 10 $INFO $packrate $bitrate $CSV_DIR/test38.csv;
python createNTest.py 39 10 $INFO $packrate $bitrate $CSV_DIR/test39.csv;
python createNTest.py 40 10 $INFO $packrate $bitrate $CSV_DIR/test40.csv;
python createNTest.py 41 10 $INFO $packrate $bitrate $CSV_DIR/test41.csv;
python createNTest.py 42 10 $INFO $packrate $bitrate $CSV_DIR/test42.csv;
python createNTest.py 43 10 $INFO $packrate $bitrate $CSV_DIR/test43.csv;
python createNTest.py 44 10 $INFO $packrate $bitrate $CSV_DIR/test44.csv;
python createNTest.py 45 10 $INFO $packrate $bitrate $CSV_DIR/test45.csv;
python createNTest.py 46 10 $INFO $packrate $bitrate $CSV_DIR/test46.csv;
python createNTest.py 47 10 $INFO $packrate $bitrate $CSV_DIR/test47.csv;
python createNTest.py 48 10 $INFO $packrate $bitrate $CSV_DIR/test48.csv;
python createNTest.py 49 10 $INFO $packrate $bitrate $CSV_DIR/test49.csv;
python createNTest.py 50 10 $INFO $packrate $bitrate $CSV_DIR/test50.csv;
" >> /dev/null
multiFlows


echo "Preparing Node 2..."
ssh -t $NODE2 "
cd $TEST_DIR/get_net_usage;
make &> /dev/null;
" 2> /dev/null


echo "Resolving MAC Addresses."

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
		sudo ./packetize $4 $S_ETHER $D_ETHER $IF;
	" &

	if [ $1 -eq 1 ]; then
		ssh $NODE2 "
			cd $TEST_DIR;
			sudo python evalMace.py 10 $2 $3 $IF;
		" | tee testResults.txt
	else
		ssh $NODE2 "
			cd $TEST_DIR;
			sudo python evalMace.py 10 $2 $3 $IF;
		" | tee -a testResults.txt 
	fi
}

<<mFlows
testCase 1 $packrate $bitrate ./csv/test1.csv
testCase 2 $packrate $bitrate ./csv/test2.csv
testCase 3 $packrate $bitrate ./csv/test3.csv
testCase 4 $packrate $bitrate ./csv/test4.csv
testCase 5 $packrate $bitrate ./csv/test5.csv
testCase 6 $packrate $bitrate ./csv/test6.csv
testCase 7 $packrate $bitrate ./csv/test7.csv
testCase 8 $packrate $bitrate ./csv/test8.csv
testCase 9 $packrate $bitrate ./csv/test9.csv
testCase 10 $packrate $bitrate ./csv/test10.csv
testCase 11 $packrate $bitrate ./csv/test11.csv
testCase 12 $packrate $bitrate ./csv/test12.csv
testCase 13 $packrate $bitrate ./csv/test13.csv
testCase 14 $packrate $bitrate ./csv/test14.csv
testCase 15 $packrate $bitrate ./csv/test15.csv
testCase 16 $packrate $bitrate ./csv/test16.csv
testCase 17 $packrate $bitrate ./csv/test17.csv
testCase 18 $packrate $bitrate ./csv/test18.csv
testCase 19 $packrate $bitrate ./csv/test19.csv
testCase 20 $packrate $bitrate ./csv/test20.csv
testCase 21 $packrate $bitrate ./csv/test21.csv
testCase 22 $packrate $bitrate ./csv/test22.csv
testCase 23 $packrate $bitrate ./csv/test23.csv
testCase 24 $packrate $bitrate ./csv/test24.csv
testCase 25 $packrate $bitrate ./csv/test25.csv
testCase 26 $packrate $bitrate ./csv/test26.csv
testCase 27 $packrate $bitrate ./csv/test27.csv
testCase 28 $packrate $bitrate ./csv/test28.csv
testCase 29 $packrate $bitrate ./csv/test29.csv
testCase 30 $packrate $bitrate ./csv/test30.csv
testCase 31 $packrate $bitrate ./csv/test31.csv
testCase 32 $packrate $bitrate ./csv/test32.csv
testCase 33 $packrate $bitrate ./csv/test33.csv
testCase 34 $packrate $bitrate ./csv/test34.csv
testCase 35 $packrate $bitrate ./csv/test35.csv
testCase 36 $packrate $bitrate ./csv/test36.csv
testCase 37 $packrate $bitrate ./csv/test37.csv
testCase 38 $packrate $bitrate ./csv/test38.csv
testCase 39 $packrate $bitrate ./csv/test39.csv
testCase 40 $packrate $bitrate ./csv/test40.csv
testCase 41 $packrate $bitrate ./csv/test41.csv
testCase 42 $packrate $bitrate ./csv/test42.csv
testCase 43 $packrate $bitrate ./csv/test43.csv
testCase 44 $packrate $bitrate ./csv/test44.csv
testCase 45 $packrate $bitrate ./csv/test45.csv
testCase 46 $packrate $bitrate ./csv/test46.csv
testCase 47 $packrate $bitrate ./csv/test47.csv
testCase 48 $packrate $bitrate ./csv/test48.csv
testCase 49 $packrate $bitrate ./csv/test49.csv
testCase 50 $packrate $bitrate ./csv/test50.csv
mFlows

#<<singleFlows
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
testCase 24 $pr24 $br24 ./csv/test24.csv
testCase 25 $pr25 $br25 ./csv/test25.csv
testCase 26 $pr26 $br26 ./csv/test26.csv
testCase 27 $pr27 $br27 ./csv/test27.csv
testCase 28 $pr28 $br28 ./csv/test28.csv
testCase 29 $pr29 $br29 ./csv/test29.csv
testCase 30 $pr30 $br30 ./csv/test30.csv
testCase 31 $pr31 $br31 ./csv/test31.csv
testCase 32 $pr32 $br32 ./csv/test32.csv
testCase 33 $pr33 $br33 ./csv/test33.csv
#singleFlows

# Truncating results for easier reading
sudo cat testResults.txt | egrep -o "[0-9]+.[0-9]+%" > results.txt

# Creating Text files for graphing
sudo touch packetResults.txt
sudo touch bitResults.txt
lineN=0
while read p; do
    if [ $[$lineN % 2] -eq 0 ]; then
        sudo echo "${p//%}" >> packetResults.txt
    else
        sudo echo "${p//%}" >> bitResults.txt
    fi
    ((++lineN))
done < results.txt

<<whengraphs
# Graph Generation
printf "Generating Performance Graph by Bit Rate\n"
python graph.py bits bRates.txt bitResults.txt > /dev/null 2> /dev/null

printf "Generating Performance Graph by Packet Rate\n"
python graph.py packets pRates.txt packetResults.txt > /dev/null 2> /dev/null
whengraphs
