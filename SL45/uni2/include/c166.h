/* Remove variable attributes */
unsigned int            _rol(unsigned int,unsigned int);
unsigned int            _ror(unsigned int,unsigned int);
void                    _bfld(unsigned int,unsigned int,unsigned int);
//unsigned char           _getbit(unsigned int,unsigned int);
//void                    _putbit(unsigned char,unsigned int,unsigned int);
void                    _int166(unsigned int);
void                    _idle(void);
void                    _nop(void);
void                    _pwrdn(void);
void                    _srvwdt(void);
void                    _diswdt(void);
void                    _einit(void);
unsigned int            _prior(unsigned int);
long                    _mul32(int,int);
unsigned long           _mulu32(unsigned int,unsigned int);
int                     _div32(long,int);
unsigned int            _divu32(unsigned long,unsigned int);
int                     _mod32(long,int);
unsigned int            _modu32(unsigned long,unsigned int);

/* Pointer conversions */
unsigned int            _sof(void _huge *);
unsigned int            _pof(void _huge *);
unsigned int            _seg(void _far *);
unsigned int            _pag(void _far *);
void huge *             _mkhp(unsigned int,unsigned int);
void far *              _mkfp(unsigned int,unsigned int);
void                    _atomic(unsigned int);
