IF="lo"

P="[0-9a-fA-F]{2}"
ETHERS=$(ip a | egrep -o "$P:$P:$P:$P:$P:$P")

i=1
DATHER=0
for iface in $(ip a | cut -d ' ' -f1); do
        if [ $IF == $iface ]; then
			$DATHER=$i
		fi
		((++i))			
done


IETHER=$[$DATHER * 2]
echo "Correct IF: ${ETHERS[$IETHER]:0:17}"
