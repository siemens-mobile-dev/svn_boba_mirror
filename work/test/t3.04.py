#!/usr/bin/scapy -c

IF1='eth1'

pkt =  Ether(src='34:08:04:00:00:01',dst='00:02:00:02:01')/Dot1Q(vlan=100)/Padding(load="test3.4")
sendp(pkt,iface=IF1,verbose=1,loop=0,count=1)

pkt =  Ether(src='34:08:04:00:00:02',dst='00:02:00:02:02')/Dot1Q(vlan=101)/IP(src="192.168.1.200",dst="192.168.2.200")/Padding(load="test3.4")
sendp(pkt,iface=IF1,verbose=1,loop=0,count=1)

pkt =  Ether(src='34:08:04:00:00:03',dst='00:02:00:02:03')/Dot1Q(vlan=100,prio=3)/IP(src="192.168.1.200",dst="192.168.2.200")/Padding(load="test3.4")
sendp(pkt,iface=IF1,verbose=1,loop=0,count=1)

pkt =  Ether(src='00:00:00:00:01:04',dst='00:02:00:02:04')/Dot1Q(vlan=100)/IP(src="192.168.1.200",dst="192.168.2.200")/Padding(load="test3.4")
sendp(pkt,iface=IF1,verbose=1,loop=0,count=1)

pkt =  Ether(src='34:08:04:00:00:05',dst='00:00:00:00:02:05')/Dot1Q(vlan=100)/IP(src="192.168.1.200",dst="192.168.2.200")/Padding(load="test3.4")
sendp(pkt,iface=IF1,verbose=1,loop=0,count=1)

pkt =  Ether(src='34:08:04:00:00:06',dst='00:02:00:02:06')/Dot1Q(vlan=100)/IP(src="192.168.1.1",dst="192.168.2.200")/Padding(load="test3.4")
sendp(pkt,iface=IF1,verbose=1,loop=0,count=1)

pkt =  Ether(src='34:08:04:00:00:07',dst='00:02:00:02:07')/Dot1Q(vlan=100)/IP(src="192.168.1.200",dst="192.168.2.2")/Padding(load="test3.4")
sendp(pkt,iface=IF1,verbose=1,loop=0,count=1)

pkt =  Ether(src='34:08:04:00:00:08',dst='00:02:00:02:08')/Dot1Q(vlan=100)/IP(src="192.168.1.200",dst="192.168.2.200",tos=40)/Padding(load="test3.4")
sendp(pkt,iface=IF1,verbose=1,loop=0,count=1)

pkt =  Ether(src='34:08:04:00:00:08',dst='00:02:00:02:08')/Dot1Q(vlan=100)/IPv6()/Padding(load="test3.4")
sendp(pkt,iface=IF1,verbose=1,loop=0,count=1)

pkt =  Ether(src='34:08:04:00:00:08',dst='00:02:00:02:08')/Dot1Q(vlan=100)/IP(src="192.168.1.200",dst="192.168.2.200")/TCP(dport=1000)/Padding(load="test3.4")
sendp(pkt,iface=IF1,verbose=1,loop=0,count=1)

pkt =  Ether(src='34:08:04:00:00:08',dst='00:02:00:02:08')/Dot1Q(vlan=100)/IP(src="192.168.1.200",dst="192.168.2.200")/TCP(sport=1001)/Padding(load="test3.4")
sendp(pkt,iface=IF1,verbose=1,loop=0,count=1)

pkt =  Ether(src='34:08:04:00:00:08',dst='00:02:00:02:08')/Dot1Q(vlan=100)/IP(src="192.168.1.200",dst="192.168.2.200")/UDP(dport=1002)/Padding(load="test3.4")
sendp(pkt,iface=IF1,verbose=1,loop=0,count=1)

pkt =  Ether(src='34:08:04:00:00:08',dst='00:02:00:02:08')/Dot1Q(vlan=100)/IP(src="192.168.1.200",dst="192.168.2.200")/UDP(sport=1003)/Padding(load="test3.4")
sendp(pkt,iface=IF1,verbose=1,loop=0,count=1)

quit()
