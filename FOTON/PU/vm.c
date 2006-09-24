//------------------------------
// Виртуальная машина
//------------------------------

void ConstructVMPC(void)
{
  unsigned int *p;
  unsigned int adr;
  unsigned long l;
  char c;

  p=&VMPC[0];
  c=16;
  do
  {
    *p=0; p++;
  }
  while (--c);

  p=&VMPC[0];

  adr=0;

  do
  {
    FlashReadVM(adr,&l,4);
    if (!(adr+=32)) return;
  }
  while(l!=0xFFFFFFFF);

  FlashReadVM(adr,&l,4);

  if (l==0xFFFFFFFF) return;

  c=16;
  do
  {
    *p=adr;
    VMBASE[16-c]=adr;
    p++;
    do
    {
      FlashReadVM(adr,&l,4);
      if (!(adr+=4)) return;
      if (l==0xFFFFFFFF) return;
    }
    while(l);
  }
  while(--c);
}

void vm_sendcmdtodevice(char ppkp, char sensor, char cmd)
{
  char s0;
  char s1;
  char s2;

  if (!ISMASTER) return;

  while (SysPPKP_rxsize==0); // Ожидаем окончание сервисного обмена
  _CLI();
  s0=SysPPKP_rxsize; // Сохраняем принятые данные
  s1=SysPPKP_RXB[0];
  s2=SysPPKP_RXB[1];
  // Подготовка данных для передачи комманды
  SysPPKP_rxsize=0;
  SysPPKP_TXB[0]=ppkp&0x3f;
  SysPPKP_TXB[1]=sensor&0x3f;
  SysPPKP_TXB[2]=cmd;
  SysPPKP_txsize=3;
  //NeedNoError=1;
  _SEI();
  while (SysPPKP_rxsize==0); // Ожидаем окончание сервисного обмена
  SysPPKP_rxsize=s0; // Восстанавливаем старые данные
  SysPPKP_RXB[0]=s1;
  SysPPKP_RXB[1]=s2;

  // EPPKP[ppkp]&=0x7F;
}

void DoVirtualMachines(void)
{
  struct
  {
    char cmd;
    char p1;
    char p2;
    char p3;
  } vc;

  char vm_number;

  unsigned int *pc;
  char *pp;

  pc=&VMPC[0];
  vm_number=0;
  do
  {
    if ((*pc)==0) goto L_NOVM;

    FlashReadVM(*pc,&vc,4);

    (*pc)+=4;

    switch(vc.cmd)
    {
    case 0:	
      //Stop
      (*pc)=0;
      break;
    case 1:
      //if Sxxx
      if (!( BitFlags[vc.p1>>3]&(1<<(vc.p1&7)) ))
      {
        (*pc)+=4;
      }
      break;
    case 2:
      //if Cxxx
      if ( BitFlags[vc.p1>>3]&(1<<(vc.p1&7)) )
      {
        (*pc)+=4;
      }
      break;
    case 3:
      //goto label
      (*pc)=(vc.p1|(vc.p2<<8))*4+VMBASE[vm_number];
      break;
    case 4:
      //deviceledon ppkp,sensor
      vm_sendcmdtodevice(vc.p1,vc.p2,1);
      break;
    case 5:
      //deviceledoff ppkp,sensor
      vm_sendcmdtodevice(vc.p1,vc.p2,0);
      break;
    case 6:
      //timer n,time,bit
      pp=&MacroTimers[vc.p1<<1];
      _CLI();
      *pp=vc.p2;
      pp++;
      if (vc.p2)
      {
        *pp=vc.p3;
        BitFlags[vc.p3>>3]&=~(1<<(vc.p3&7));
      }
      _SEI();
      break;
    case 7:
      //relay zone,point
      //ZoneDelays[vc.p1<<2|(vc.p2)]=0xFF;
      {
        unsigned int z;
        z=(vc.p1<<2)|vc.p2;
        if (ZoneDelays[z]==0xFE) ZoneDelays[z]=ZONEDELAY_AREA[z];
      }
      break;
    case 8:
      //event c,c1
      Event2FIFO(0xFF,0xFF,vc.p1,vc.p2);
      break;
    default:	
      //Ошибка кода
      *pc=0;
      Event2FIFO(0xFF,0xFF,33,vm_number);
      break;
    }
  L_NOVM:
    pc++;
    vm_number++;
  }
  while(vm_number<16);
}
