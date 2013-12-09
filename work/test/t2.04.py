#!/usr/bin/scapy -c

IF1='eth1'
IF2='eth0'
MAC='cc:cc:81:0f:a4:f0'

pkt =  Ether(src='00:01:00:02:00',dst='00:02:00:02:00')/Dot1Q(vlan=200)/IP(src="192.168.1.200",dst="192.168.2.200")/Padding(load="test2.3")
sendp(pkt,iface=IF1,verbose=1,loop=0,count=1)

pkt =  Ether(src='00:02:00:03:00',dst='00:03:00:03:00')/Dot1Q(vlan=300)/IP(src="192.168.2.30",dst="192.168.3.30")/Padding(load="test2.3")
sendp(pkt,iface=IF2,verbose=1,loop=0,count=1)

pkt =  Ether(src='00:01:00:02:00',dst=MAC)/Dot1Q(vlan=200)/IP(src="192.168.1.1",dst="192.168.1.2")/ICMP()
#pkt =  Dot1Q(vlan=200)/IP(src="192.168.1.1",dst="192.168.1.2")/ICMP()
sendp(pkt,iface=IF1,verbose=1,loop=0,count=1)

quit()
