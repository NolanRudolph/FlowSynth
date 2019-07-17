<p align="center">
  <b>At Scale Packet Synthesizer for University of Oregon Netflow</b>
</p>
</br>
<p>
  The project should be compiled and run by the following steps:
  <ol>
    <li>Clone the repository</li>
    <li>Change directory to "TimelySend" for real-time based synthesization</li>
    <li>Issue "$ make" within the new directory via command line interface</li>
    <li>Setup a tcpdump in a new terminal to visualize functionality</li> 
    <li>Run the program via "$ sudo ./packetize [Netflow CSV File] [Ethernet sHost] [Ethernet dHost] [Network Interface]"</li>
  </ol>
</p>
</br>
<p>
  <u>Note that the "Netflow CSV File" should have the following sections delimited by commas:</u>
  <ol>
    <li>Start Timestamp in Epoch Format</li>
    <li>End Timestamp in Epoch Formatt</li>
    <li>Source IP Address</li>
    <li>Destination IP Address</li>
    <li>Source Port Number</li>
    <li>Destination Port Number</li>
    <li>IP Header Protocol Number</li>
    <li>Type of Service Value (from IP Header)</li>
    <li>TCP Flags</li>
    <li>Number of Packets</li>
    <li>Number of Bytes</li>
    <li>Router Ingress Port</li>
    <li>Router Egress Port</li>
    <li>Source Autonomous System</li>
    <li>Destination Autonomous System</li>
  </ol>
</p>
    
    
