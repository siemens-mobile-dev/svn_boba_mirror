#!/usr/bin/scapy -c

IF1='eth1'
IF2='eth0'

pkt =  Ether(src='00:01:00:01:00',dst='01:80:c2:00:00')/LLC(dsap=0x42,ssap=0x42,ctrl=0x03)/STP(version=2,bpdutype=2,bpduflags=0x0e,rootid=0x8000,rootmac='11:11:11:11:11:11',bridgeid=0x8000,bridgemac='22:22:22:22:22:22')/'\x00\x00\x00\x00\x00\x00\x00\x00'
sendp(pkt,iface=IF1,verbose=1,loop=0,count=1)

pkt =  Ether(src='00:02:00:01:00',dst='01:80:c2:00:00')/LLC(dsap=0x42,ssap=0x42,ctrl=0x03)/STP(version=2,bpdutype=2,bpduflags=0x0e,rootid=0x8000,rootmac='11:11:11:11:11:11',bridgeid=0x8000,bridgemac='22:22:22:22:22:22')/'\x00\x00\x00\x00\x00\x00\x00\x00'
sendp(pkt,iface=IF2,verbose=1,loop=0,count=1)

quit()
