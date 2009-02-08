#include "inc\mc.h"
#include "inc\krnl.h"

typedef struct{
	const int *key;
	char* name;
} KEYNM_MAP;

const KEYNM_MAP keynames[] = 
{
	{&KEY_LEFT_SOFT, "lsoft"},  // 0
	{&KEY_RIGHT_SOFT, "rsoft"},  // 1
	{&KEY_ENTER, "enter"},  // 2
	{&KEY_UP, "up"},  // 3
	{&KEY_DOWN, "down"},  // 4
	{&KEY_LEFT, "left"},  // 5
	{&KEY_RIGHT, "right"},  //6
	{&KEY_VOL_UP, "volup"},  // 7
	{&KEY_VOL_DOWN, "voldown"},   //8
        {&KEY_ESC, "esc"},  //9
	{&KEY_STAR, "*"},  //10
	{&KEY_DIEZ, "#"}, //11
        
	{&KEY_DIGITAL_0, "0"}, 
	{&KEY_DIGITAL_0, "1"},
	{&KEY_DIGITAL_0, "2"},
	{&KEY_DIGITAL_0, "3"},
	{&KEY_DIGITAL_0, "4"},
	{&KEY_DIGITAL_0, "5"},
	{&KEY_DIGITAL_0, "6"},
	{&KEY_DIGITAL_0, "7"},
	{&KEY_DIGITAL_0, "8"},
	{&KEY_DIGITAL_0, "9"},
};

#define KEYS_COUNT (sizeof(keynames)/sizeof(keynames[0]))

char keyMap[0xFF];

KEY_PROC procmap[] = 
{
	NULL,
	DoMenu,
	DoOpen,
	DoUp,
	DoDwn,
	DoPrvDrv,
	DoNxtDrv,
	NULL,
	NULL,
	DoBackK,
	NULL,
	NULL,
	NULL,
	DoSwapTab,
	DoSetSort,
	NULL,
	DoChk,
	DoOpen,
	DoDel,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL, 
};

// long press
KEY_PROC procmapl[] = 
{
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL, 
};


typedef struct{
	char* name;
	KEY_PROC proc;
} PROC_MAP;

const PROC_MAP keyprocs[]=
{
  {"none", NULL}  
};


void KeysProc(char *name, char *value)
{
  strtolower(name, name, -1);
  strtolower(value, value, -1);
  
  int inkey = -1;
  int islong = 0;
  KEY_PROC keyproc = DoErrKey;

	int nameLen = strlen(name);
	if (nameLen > 0 && name[nameLen - 1] == '+')
	{
		// Последний символ +, значит долгое нажатие
		islong = 1;
		name[nameLen - 1] = 0; // Убираем плюс
	}

	for(int cc=0; cc < KEYS_COUNT; cc++)
	{
		if (strcmp(keynames[cc].name, name)==0)
		{
			inkey = cc;
			break;
		}
	}
	if (inkey == -1) return;

	for(int cc = 0; cc < sizeof(keyprocs) / sizeof(keyprocs[0]); cc++)
	{
		if (strcmp(keyprocs[cc].name, value) == 0)
		{
			keyproc = keyprocs[cc].proc;
			break;
		}
	}

	if (islong)
		procmapl[inkey] = keyproc;
	else
		procmap[inkey] = keyproc;
}

void InitializeKeyMap()
{
  int k;
  // Инициируем массив, 0xFF - значит нет такой клавиши
  memset(keyMap, 0xFF, sizeof(keyMap));
  // Сохраняем индексы
  for (int i = 0; i < 12; i++)
  {
    k=*keynames[i].key;
    if (k<0xFF)
      keyMap[k] = i;
  }
  for (int i = 0; i < 10; i++)
  {
    k=KEY_DIGITAL_0+i;
    if (k<0xFF)
      keyMap[k] = i+12;
  }  
}

char GetKeynameIndexByKey(char key)
{
	if (key >= 0xFF) return 0xFF;
	else return keyMap[key];
}

KEY_PROC GetKeyprocByKey(char key)
{
	char idx = GetKeynameIndexByKey(key);
	if (idx != 0xFF)
		return procmap[idx];
	else
		return NULL;
}

KEY_PROC GetKeyprocLongByKey(char key)
{
	char idx = GetKeynameIndexByKey(key);
	if (idx != 0xFF)
		return procmapl[idx];
	else
		return NULL;
}

void DoKey(int isLongPress, int key)
{
  char idx = GetKeynameIndexByKey(key);
  if (idx != 0xFF)
  {
    if (isLongPress)
    {
      if (procmapl[idx]) procmapl[idx]();
    }
    else
    {
      if (procmap[idx]) procmap[idx]();
    }
  }
}

void LoadKeys()
{
  if (CONFIG_LOAD_KEYS)
    EnumIni(1,keys_file,KeysProc);
  InitializeKeyMap();
}
