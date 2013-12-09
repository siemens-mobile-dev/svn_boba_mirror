#!/usr/bin/scapy -c

MACFILE='perm.mac'

import threading,time,sys,pprint

gen_iface ='eth1'
list_iface='eth2'

firstvlan=21
lastvlan =22

v2p={21:21, 22:21, 23:23, 24:23, 25:25, 26:25, 27:25, 28:25}

dstMAC = '00:00:b0:ba:00:02'

random.seed(3)


txp=0
rxp=0
err=0
tim=time.time()

MACSc=0
if (MACFILE):
    MACS=open(MACFILE).readlines()
    MACSc=len(MACS)
    print 'Read MACs:%d'%MACSc

def getMAC():
    if MACSc:
	return MACS[random.randint(0,MACSc)]
    return str(RandMAC(template='0:*:*:*:*:*'))

def generator(e1):
 global firstvlan,lastvlan,gen_iface,dstMAC,txp
 vc=dict.fromkeys(range(firstvlan,lastvlan+1),0)
 macs=set()
 while True:
    global txp
    srcMAC = str(RandMAC(template='0:*:*:*:*:*'))
    vlan = random.randint(firstvlan,lastvlan)
#    vc[vlan]+=1
#    macs=macs.union(set([srcMAC]))
    e1.wait(1)
    e1.clear()
    pkt = Ether(src=srcMAC,dst=dstMAC)/Dot1Q(vlan=vlan)/IP(src='1.1.1.1',dst='2.2.2.2')/Padding(load=vlan)
    sendp(pkt,iface=gen_iface,verbose=0,loop=0,count=1)
#    print 'gen1:',vc,len(macs),txp,rxp,srcMAC
    txp+=1
#    if txp==23186:
#	print 'STOP!!!'
#	break

def listener1(e1):
 global firstvlan,lastvlan,gen_iface,list_iface,dstMAC,v2p
 vc=dict.fromkeys(range(firstvlan,lastvlan+1),0)
 global macs
 macs=set()
 def list1(pkt):
    global macs
    srcMAC=pkt[0].src
    vlan  =pkt[0].vlan
    load  =int(pkt[0].load[0]+pkt[0].load[1])
#    vc[vlan]+=1
#    macs=macs.union(set([srcMAC]))
#    if vlan!=v2p[load]:
#	print '\nlist1 Vlan tx:%d Vlan rx:%d MAC:%s'%(load,vlan,srcMAC)
#    print 'list1: ',vc,len(macs),txp,rxp,srcMAC
    pkt2 = Ether(src=dstMAC,dst=srcMAC)/Dot1Q(vlan=vlan)/IP(src='2.2.2.2',dst='1.1.1.1')/Padding(load=load)
    sendp(pkt2,iface=list_iface,verbose=0,loop=0,count=1)
    e1.set()

 pkt=sniff(iface=list_iface,filter='ether dst host '+dstMAC, count=0, prn=list1, store=0)

def listener2(e1):
 global firstvlan,lastvlan,gen_iface,list_iface,dstMAC,txp,rxp
 vc=dict.fromkeys(range(firstvlan,lastvlan+1),0)
 global macs
 macs=set()
 def list2(pkt):
    global macs,rxp,txp,err,tim
    rxp+=1
    vlan  =pkt[0].vlan
    load  =int(pkt[0].load[0]+pkt[0].load[1])
    srcMAC=pkt[0].dst
    if (err!=0 and txp in [1000,2000,4000,8000,16000,32000,64000,128000]):
	print '\rTx:%d Rx:%d Err:%d Spd:%d'%(txp,rxp,err,rxp/(time.time()-tim))
    if vlan!=load:
	err+=1
#	print '\rlist2 (%d/%d)  Vlan tx:%d Vlan rx:%d MAC:%s'%(err,rxp,load,vlan,srcMAC)
	if err in [1,10,100,1000,10000]:
	    print '\rTx:%d Rx:%d Err:%d Spd:%d'%(txp,rxp,err,rxp/(time.time()-tim))
#	    if err==1:
#		print srcMAC
#    vc[vlan]+=1
#    macs=macs.union(set([srcMAC]))
#    print 'list2: ',vc,len(macs),txp,rxp,srcMAC
#    if rxp % 16 == 0:
    print '\rTx:%d Rx:%d Err:%d Spd:%d'%(txp,rxp,err,rxp/(time.time()-tim)),

 pkt=sniff(iface=gen_iface,filter='ether src host '+dstMAC, count=0,prn=list2,store=0)


e1 = threading.Event()

t1=threading.Thread(name='gen1', target=generator, args=(e1,))
t2=threading.Thread(name='list1',target=listener1, args=(e1,))
t3=threading.Thread(name='list2',target=listener2, args=(e1,))

t3.start()
t2.start()
t1.start()

t3.join()
t2.join()
t1.join()

quit()
