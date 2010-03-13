#ifndef _SHORTCUTS_H_
#define _SHORTCUTS_H_

#define IDN_DIGIT_0_ICON _T( "FM_RADIO_MHZ_DIGIT_0_ICN" )
#define IDN_DIGIT_1_ICON _T( "FM_RADIO_MHZ_DIGIT_1_ICN" )
#define IDN_DIGIT_2_ICON _T( "FM_RADIO_MHZ_DIGIT_2_ICN" )
#define IDN_DIGIT_3_ICON _T( "FM_RADIO_MHZ_DIGIT_3_ICN" )
#define IDN_DIGIT_4_ICON _T( "FM_RADIO_MHZ_DIGIT_4_ICN" )
#define IDN_DIGIT_5_ICON _T( "FM_RADIO_MHZ_DIGIT_5_ICN" )
#define IDN_DIGIT_6_ICON _T( "FM_RADIO_MHZ_DIGIT_6_ICN" )
#define IDN_DIGIT_7_ICON _T( "FM_RADIO_MHZ_DIGIT_7_ICN" )
#define IDN_DIGIT_8_ICON _T( "FM_RADIO_MHZ_DIGIT_8_ICN" )
#define IDN_DIGIT_9_ICON _T( "FM_RADIO_MHZ_DIGIT_9_ICN" )

typedef struct
{
	wchar_t* name;
	wchar_t* fullpath;
	int appID;
	wchar_t imageID;
}java_list_elem;



void Shortcuts( BOOK* book, GUI* );
wchar_t* get_path();
int elem_filter(void* elem);
void elem_free(void* elem);
int get_file( wchar_t* fname, char** buf_set );
int CreateButtonList( void* data, BOOK* book );

#endif
