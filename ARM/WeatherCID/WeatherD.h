extern const unsigned int TID;

extern const char ICON_PATH[];
extern const char DATA_PATH[];
extern const char FONT_FILE[];

extern const unsigned int FONT_SIZE;
extern const char FONT_COLOR[];
extern const char BORDER_COLOR[];

extern const unsigned int DATA_X;
extern const unsigned int DATA_Y;

extern const unsigned int PICT_X;
extern const unsigned int PICT_Y;

extern const int SHOW_TEMP;
extern const int SHOW_PRESSURE;
extern const int SHOW_WIND;
extern const int SHOW_REWLET;
extern const int SHOW_PIC;
extern const int SHOW_CITY;

typedef struct{
  char path[256];
  int  height,width;
}TPic;

typedef struct{
  TPic dt, c, r, s, st, WindPic;
  char City[16],
       Temp[16],
       Pressure[16],
       Wind[16],
       Rewlet[16];
  char daytime,
       cloudness,
       rain,
       storm,
       snow;
}WEATHER;

GBSTMR update_tmr;
GBSTMR reconnect_tmr;

unsigned int old_gprs_state[2]={0,0};
WEATHER weath;
