static const unsigned char far bmp_bat_persent0[]   = {
 0x00,0x00,0x00,0x7F,
 0x00,0x00,0x04,0xFF,
 0x00,0x00,0x00,0xFF,
 0x00,0x00,0x01,0x7F,
 0x00,0x00,0x02,0x7F,
 0x00,0x00,0x04,0xFF,
 0x00,0x00,0x00,0x7F
};
static const unsigned char far bmp_bat_persent1[]   = {
 0x80,0x00,0x00,0x7F,
 0x80,0x00,0x04,0xFF,
 0x80,0x00,0x00,0xFF,
 0x80,0x00,0x01,0x7F,
 0x80,0x00,0x02,0x7F,
 0x80,0x00,0x04,0xFF,
 0x00,0x00,0x00,0x7F
};
static const unsigned char far bmp_bat_persent2[]   = {
 0x00,0x00,0x00,0x7F,
 0x00,0x00,0x04,0xFF,
 0x00,0x00,0x00,0xFF,
 0x80,0x00,0x01,0x7F,
 0x80,0x00,0x02,0x7F,
 0x80,0x00,0x04,0xFF,
 0x00,0x00,0x00,0x7F
};
static const unsigned char far bmp_bat_persent3[]   = {
 0x80,0x00,0x00,0x7F,
 0x80,0x00,0x04,0xFF,
 0x80,0x00,0x00,0xFF,
 0x00,0x00,0x01,0x7F,
 0x00,0x00,0x02,0x7F,
 0x00,0x00,0x04,0xFF,
 0x00,0x00,0x00,0x7F
};
static const unsigned char far bmp_mA[]             = {0x27,0x47,0xF7,0x27,0x47,0};
static const unsigned char far bmp_minus[]          = {0x07,0x07,0x77,0x07,0x07,0};
static const unsigned char far bmp_temp[]           = {0x00,0x00,0x2F,0x00,0x00,0x5F,0x00,0x00,0x2F,0x00,0x00,0x0F,0x00,0x20,0x0F,0};

static const IMGHDR far hdr_bat_persent[4] = {
 25,7,0x1,&bmp_bat_persent0,
 25,7,0x1,&bmp_bat_persent1,
 25,7,0x1,&bmp_bat_persent2,
 25,7,0x1,&bmp_bat_persent3
};
static const IMGHDR far hdr_mA               = {5,5,0x1,&bmp_mA};
static const IMGHDR far hdr_minus            = {5,5,0x1,&bmp_minus};
static const IMGHDR far hdr_temp             = {20,5,0x1,&bmp_temp};
