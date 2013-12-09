#!/usr/bin/scapy -c

IF1='eth0'

while 1:
    pkt=sniff(iface=IF1, count=1)
    pkt.show()
    sendp(pkt,iface=IF1,verbose=1,loop=0,count=1)

quit()
