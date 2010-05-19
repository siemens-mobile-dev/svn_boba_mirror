#include "..\\include\Lib_Clara.h"
#include "..\\include\Types.h"
#include "vars.h"

#define IDN_ELF_SMALL_ICON _T("CALE_LUNAR_12ANIMALS_1ST_MOUSE_ICN")
#define IDN_ELF_BIG_ICON _T("CALE_LUNAR_12ANIMALS_10TH_ROOSTER_ICN")

#define IDN_FOLDERNAME _T("DB_OTHER_TXT") //old value: _T("DB_APPLICATIONS_TXT")
#define IDN_SAVED_ON_MS _T("DB_SAVED_IN_OTHER_ON_MS_TXT")
#define IDN_SAVED_ON_PH _T("DB_SAVED_IN_OTHER_IN_PH_TXT")
#define IDN_START _T("GUI_SELECT_SK") //old values: _T("OAF_START_SK"), _T("MENU_SYNCH_START_REMOTE_TXT")

#ifdef EP_EXTICON14x14
#include "exticons\14x14.inc"
#endif

#ifdef EP_EXTICON16x20
#include "exticons\16x20.inc"
#endif

#ifdef EP_EXTICON24x24
#include "exticons\24x24.inc"
#endif

#ifdef EP_EXTICON42x32
#include "exticons\42x32.inc"
#endif

#ifdef EP_EXTICON48x36
#include "exticons\48x36.inc"
#endif

#ifdef EP_EXTICON56x42
#include "exticons\56x42.inc"
#endif

extern __thumb long elfload(const unsigned short *filename, void *param1, void *param2, void *param3);
extern EP_DATA * getepd(void);


const u16 ext[]={L"elf"};
const char ctype[]={"application/elf"};

const int ex[2]={(int)ext,0};
const int ct[2]={(int)ctype,0};

const char ers[]={"Elf_Run_Subroutine"};

DB_EXT * OtherDbExt()
{
	__get_epd;
	int i = List_IndexOf(epd->DBExtList, LastExtDB());
	return (DB_EXT*)List_Get(epd->DBExtList, i-1);
}

DB_EXT * CreateDbExt()
{
	DB_EXT * other_db_ext = OtherDbExt();
#if (DB2020 || A2)
	DB_EXT * db_ext = malloc(sizeof(DB_EXT));
	db_ext->type_group = other_db_ext->type_group;
#else
	DB_EXT_2010 * db_ext = malloc(sizeof(DB_EXT_2010));
#endif
	db_ext->content_type = 0;
	db_ext->ext_list = 0;
	db_ext->GetMethods = 0;
	db_ext->sub_execute_size = other_db_ext->sub_execute_size;
	db_ext->dbf = 0;
	db_ext->drm_flags = 0;
	db_ext->unk2 = 0;
	db_ext->unk3 = 0;
	return (DB_EXT*)db_ext;
}

__root void GetMethods(SUB_EXECUTE * sub_execute)
{
	sub_execute->filesub = getepd()->elf_ext_m;
}

__arm int SetSmallIcon(SUB_EXECUTE * sub_execute, wchar_t * iconid)
{
	__get_epd;
	if (epd->IconSmall == NOIMAGE)
	{
#ifdef USESMALLICON
		ImageID_GetIndirect( (void*)exticon_small, sizeof(exticon_small),0, L"png", &epd->IconSmall);
#else
		int iconsmall;
		iconidname2id(IDN_ELF_SMALL_ICON,-1,&iconsmall);
		epd->IconSmall = iconsmall;
#endif
	}
	*iconid = epd->IconSmall;
	return 0;
}

__arm int SetThumbnailIcon(SUB_EXECUTE * sub_execute, wchar_t * iconid)
{
	__get_epd;
	if (epd->IconBig == NOIMAGE)
	{
#ifdef USEBIGICON
		ImageID_GetIndirect( (void*)exticon_big, sizeof(exticon_big),0, L"png", &epd->IconBig);
#else
		int iconbig;
		iconidname2id(IDN_ELF_BIG_ICON,-1,&iconbig);
		epd->IconBig = iconbig;
#endif
	}
	*iconid = epd->IconBig;
	return 0;
}

__arm int Elf_Run_Page_PAGE_ENTER_EVENT(void * r0, BOOK * book)
{
	SUB_EXECUTE * data = BrowserItem_Get_SUB_EXECUTE(book);
	
	wchar_t * fpath = FILEITEM_GetPath(data->file_item);
	wchar_t * fname = FILEITEM_GetFname(data->file_item);
	u16 * filename = malloc((wstrlen(fpath)+wstrlen(fname)+2)*2);
	
	wstrcpy(filename,fpath);
	wstrcat(filename,L"/");
	wstrcat(filename,fname);
	
	_printf("Starting %ls",filename);
	
	elfload(filename,0,0,0);
	
	mfree(filename);
	BookObj_ReturnPage(book,PREVIOUS_EVENT);
	return(1);
};

int Elf_Run_Page_PREVIOUS_EVENT(void * r0, BOOK * book)
{
	BookObj_ReturnPage(book,PREVIOUS_EVENT);
	return(1);
}

int Elf_Run_Page_ACCEPT_EVENT(void * r0, BOOK * book)
{
	BookObj_ReturnPage(book,PREVIOUS_EVENT);
	return(1);
}

int Elf_Run_Page_CANCEL_EVENT(void * r0, BOOK * book)
{
	BookObj_ReturnPage(book,CANCEL_EVENT);
	return(1);
}

int Elf_Run_Page_PAGE_EXIT_EVENT(void * r0, BOOK * book)
{
	return(1);
}

const PAGE_MSG erp_msg[]={
	PAGE_ENTER_EVENT,  (int*)Elf_Run_Page_PAGE_ENTER_EVENT,
	PREVIOUS_EVENT,    (int*)Elf_Run_Page_PREVIOUS_EVENT,
	ACCEPT_EVENT,      (int*)Elf_Run_Page_ACCEPT_EVENT,
	CANCEL_EVENT,      (int*)Elf_Run_Page_CANCEL_EVENT,
	PAGE_EXIT_EVENT,   (int*)Elf_Run_Page_PAGE_EXIT_EVENT,
	0,0
};

const PAGE_DESC erun_page={"Elf_Run_Page",0,erp_msg};

const int subrout[]={(int)&ers,(int)&erun_page,0,0};

__arm int Elf_Run_Subroutine(SUB_EXECUTE * sub_execute)
{
	BookObj_CallSubroutine(sub_execute->BrowserItemBook,(void*)subrout);
	return(1);
};

int Elf_Run_Check(SUB_EXECUTE * sub_execute)
{
	return 2;
}



__arm void ELFExtrRegister(EP_DATA * epd)
{
	int ofs_count = 0;
	FILESUBROUTINE * OtherFileSub;
	FILESUBROUTINE * NewFileSub;
	
	GetOtherExtMethods(&OtherFileSub);
	while (OtherFileSub[ofs_count++].ON_CMD);
	epd->elf_ext_m = NewFileSub = (FILESUBROUTINE*)malloc((ofs_count+1)*sizeof(FILESUBROUTINE));
	memcpy(NewFileSub+1, OtherFileSub, ofs_count*sizeof(FILESUBROUTINE));
	
	NewFileSub->cmd = 1;
	NewFileSub->ON_CMD_RUN = Elf_Run_Subroutine;
	NewFileSub->ON_CMD_RUN_CHECK = Elf_Run_Check;
	textidname2id(IDN_START,-1,&NewFileSub->StrID);
	
	
	while (NewFileSub->ON_CMD)
	{
		int cmd = NewFileSub->cmd;
		if (cmd == DB_CMD_SETSMALLICON)
		{
			NewFileSub->ON_CMD_ICON = SetSmallIcon;
		}
#ifdef DB_CMD_SETTHUMBNAILICON
		else if(cmd == DB_CMD_SETTHUMBNAILICON)
		{
			NewFileSub->ON_CMD_ICON = SetThumbnailIcon;
		}
#endif
#ifdef DB_CMD_SETSAVEDICON
		else if(cmd == DB_CMD_SETSAVEDICON)
		{
			NewFileSub->ON_CMD_ICON = SetThumbnailIcon;
		}
#endif
		NewFileSub++;
	}
	NewFileSub = epd->elf_ext_m;
	
#ifndef DAEMONS_INTERNAL
	int efnum=3;
#else
	int efnum=2;
#endif
	DB_EXT_FOLDERS * dbfolders = malloc(sizeof(DB_EXT_FOLDERS)*efnum--);
	//last empty record
	dbfolders[efnum].StrID_FolderName = EMPTY_SID;
	dbfolders[efnum].StrID_SavedTo = EMPTY_SID;
	dbfolders[efnum].Path = NULL;
	dbfolders[efnum--].isInternal = NULL;
	//internal folder
	textidname2id(IDN_FOLDERNAME, -1, &dbfolders[efnum].StrID_FolderName);
	textidname2id(IDN_SAVED_ON_PH, -1, &dbfolders[efnum].StrID_SavedTo);
	dbfolders[efnum].Path = PATH_ELF_ROOT_INT;
	dbfolders[efnum--].isInternal = 1;
#ifndef DAEMONS_INTERNAL
	//external folder
	textidname2id( IDN_FOLDERNAME, -1, &dbfolders[efnum].StrID_FolderName);
	textidname2id( IDN_SAVED_ON_MS, -1, &dbfolders[efnum].StrID_SavedTo);
	dbfolders[efnum].Path = PATH_ELF_ROOT_EXT;
	dbfolders[efnum].isInternal = 0;
#endif
	
	DB_EXT * db_ext = CreateDbExt();
	db_ext->content_type = (char**)ct;
	db_ext->ext_list = (wchar_t**)ex;
	db_ext->GetMethods = GetMethods;
	db_ext->dbf = dbfolders;
	
	List_InsertFirst(epd->DBExtList, db_ext);
}

__arm LIST *CreateDBExtList()
{
	LIST * dbe = List_Create();
	DB_EXT ** old = (DB_EXT**)EXT_TABLE;
	int db_ext_count = 0;
	do
	{
		List_InsertLast(dbe, old[db_ext_count]);
	}while(old[db_ext_count++] != LastExtDB());
	return dbe;
}
