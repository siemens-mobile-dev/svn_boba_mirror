#!/usr/bin/scapy -c

IF1='eth1'
IF2='eth0'

pkt =  Ether(src='00:01:00:01:00',dst='00:02:00:01:00')/Dot1Q(vlan=100)/IP(src="192.168.1.100",dst="192.168.2.100")/Padding(load="test2.3")
sendp(pkt,iface=IF1,verbose=1,loop=0,count=1)
pkt =  Ether(src='00:01:00:01:01',dst='00:02:00:01:01')/Dot1Q(vlan=101)/IP(src="192.168.1.101",dst="192.168.2.101")/Padding(load="test2.3")
sendp(pkt,iface=IF1,verbose=1,loop=0,count=1)
pkt =  Ether(src='00:01:00:01:02',dst='00:02:00:01:02')/Dot1Q(vlan=102)/IP(src="192.168.1.102",dst="192.168.2.102")/Padding(load="test2.3")
sendp(pkt,iface=IF1,verbose=1,loop=0,count=1)
pkt =  Ether(src='00:01:00:01:03',dst='00:02:00:01:03')/Dot1Q(vlan=103)/IP(src="192.168.1.103",dst="192.168.2.103")/Padding(load="test2.3")
sendp(pkt,iface=IF1,verbose=1,loop=0,count=1)

pkt =  Ether(src='00:01:00:10:00',dst='00:02:00:10:00')/Dot1Q(vlan=1000)/IP(src="192.168.1.250",dst="192.168.2.250")/Padding(load="test2.3")
sendp(pkt,iface=IF1,verbose=1,loop=0,count=1)

pkt =  Ether(src='00:01:00:12:34',dst='00:02:00:12:34')/Dot1Q(vlan=1234)/Dot1Q(vlan=2345)/IP(src="192.168.1.123",dst="192.168.2.123")/Padding(load="test2.3")
sendp(pkt,iface=IF1,verbose=1,loop=0,count=1)


pkt =  Ether(src='00:02:00:01:00',dst='00:03:00:01:00')/Dot1Q(vlan=999)/Dot1Q(vlan=100)/IP(src="192.168.2.100",dst="192.168.3.100")/Padding(load="test2.3")
sendp(pkt,iface=IF2,verbose=1,loop=0,count=1)
pkt =  Ether(src='00:02:00:01:01',dst='00:03:00:01:01')/Dot1Q(vlan=999)/Dot1Q(vlan=101)/IP(src="192.168.2.101",dst="192.168.3.101")/Padding(load="test2.3")
sendp(pkt,iface=IF2,verbose=1,loop=0,count=1)
pkt =  Ether(src='00:02:00:01:02',dst='00:03:00:01:02')/Dot1Q(vlan=999)/Dot1Q(vlan=102)/IP(src="192.168.2.102",dst="192.168.3.102")/Padding(load="test2.3")
sendp(pkt,iface=IF2,verbose=1,loop=0,count=1)
pkt =  Ether(src='00:02:00:01:03',dst='00:03:00:01:03')/Dot1Q(vlan=999)/Dot1Q(vlan=103)/IP(src="192.168.2.103",dst="192.168.3.103")/Padding(load="test2.3")
sendp(pkt,iface=IF2,verbose=1,loop=0,count=1)

pkt =  Ether(src='00:02:00:10:00',dst='00:03:00:10:00')/Dot1Q(vlan=1000)/IP(src="192.168.2.250",dst="192.168.3.250")/Padding(load="test2.3")
sendp(pkt,iface=IF2,verbose=1,loop=0,count=1)

pkt =  Ether(src='00:02:00:12:34',dst='00:03:00:12:34')/Dot1Q(vlan=1234)/Dot1Q(vlan=2345)/IP(src="192.168.2.123",dst="192.168.3.123")/Padding(load="test2.3")
sendp(pkt,iface=IF2,verbose=1,loop=0,count=1)

quit()