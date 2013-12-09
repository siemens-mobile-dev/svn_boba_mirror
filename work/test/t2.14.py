#!/usr/bin/scapy -c

IF1='eth1'

pkt =  Ether(src='34:08:04:00:00:00',dst='00:02:00:02:00')/Dot1Q(vlan=100,prio=5,type=0x8863)/IP(src="192.168.1.200",dst="192.168.2.200")/Padding(load="test2.3")
sendp(pkt,iface=IF1,verbose=1,loop=0,count=1)

pkt =  Ether(src='34:08:04:00:00:01',dst='00:02:00:02:00')/Dot1Q(vlan=100,prio=5,type=0x8863)/IP(src="192.168.1.200",dst="192.168.2.200")/Padding(load="test2.3")
sendp(pkt,iface=IF1,verbose=1,loop=0,count=1)

pkt =  Ether(src='34:08:14:00:00:00',dst='00:02:00:02:00')/Dot1Q(vlan=100,prio=5,type=0x8863)/IP(src="192.168.1.200",dst="192.168.2.200")/Padding(load="test2.3")
sendp(pkt,iface=IF1,verbose=1,loop=0,count=1)

pkt =  Ether(src='34:08:04:00:00:00',dst='00:02:00:02:00')/Dot1Q(vlan=100,prio=6,type=0x8863)/IP(src="192.168.1.200",dst="192.168.2.200")/Padding(load="test2.3")
sendp(pkt,iface=IF1,verbose=1,loop=0,count=1)

pkt =  Ether(src='34:08:04:00:00:00',dst='00:02:00:02:00')/Dot1Q(vlan=100,prio=5,type=0x8864)/IP(src="192.168.1.200",dst="192.168.2.200")/Padding(load="test2.3")
sendp(pkt,iface=IF1,verbose=1,loop=0,count=1)

quit()
