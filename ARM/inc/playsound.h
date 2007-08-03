#ifdef NEWSGOLD
typedef struct{
  int pid_from;
  int msg; //MSG_PLAYFILE_REPORT
  unsigned short cmd;
  short handler;
  void *data0;
  void *data1;
} GBS_PSOUND_MSG;
#else
typedef struct{
  short pid_from;
  short msg; //MSG_PLAYFILE_REPORT
  unsigned short cmd;
  short handler;
  void *data0;
  void *data1;
} GBS_PSOUND_MSG;
#endif


//cmd
#define M_SAE_EVENT_STARTED 0
#define M_SAE_EVENT_STOPPED 1
#define M_SAE_EVENT_DONE 2
#define M_SAE_EVENT_ERROR 3
#define M_SAE_PLAYBACK_STARTED 4
#define M_SAE_PLAYBACK_ERROR 5
#define M_SAE_PLAYBACK_NEXTCYCLE 6
#define M_SAE_PLAYBACK_DONE 7
#define M_SAE_VOLUME_UPDATED 8
#define M_SAE_VOLUME_ERROR 9
#define M_SAE_PLAYTIME_UPDATED 0xA
#define M_SAE_PLAYTIME_ERROR 0xB
#define M_SAE_HANDSFREE_UPDATED 0xC
#define M_SAE_HANDSFREE_ERROR 0xD
#define M_SAE_RECORD_STARTED 0xE
#define M_SAE_RECORD_ERROR 0xF
#define M_SAE_RECORD_DONE 0x10
#define M_SAE_STOP_OK 0x11
#define M_SAE_STOP_FAIL 0x12
#define M_SAE_CANCEL_OK 0x13
#define M_SAE_CANCEL_FAIL 0x14
#define M_SAE_PAUSE_OK 0x15
#define M_SAE_PAUSE_FAIL 0x16
#define M_SAE_RESUME_OK 0x17
#define M_SAE_RESUME_FAIL 0x18
#define M_SAE_INFO_DATA_AVAILABLE 0x19
#define M_SAE_INFO_REQUEST_FAIL 0x1A
#define M_SAE_MAX_RECORDING_TIME_AVAILABLE 0x1B
#define M_SAE_MAX_RECORDING_TIME_ERROR 0x1C
#define M_SAE_AUDIO_EVENT_UPDATED 0x1D
#define M_SAE_AUDIO_EVENT_UPDATED_MS 0x1E
#define M_SAE_AUDIO_EVENT_STOPPED 0x1F
#define M_SAE_AUDIO_EVENT_PAUSED 0x20
#define M_SAE_AUDIO_EVENT_RESUMED 0x21
