#include "softkeyutil.hpp"

static bool isallocatedstrid(STRID strid)
{
	return (strid!=EMPTY_SID) && ((strid&0xFFFF0000)!=0);
}

enum
{
	STRUCTTYPE_NONE,
	STRUCTTYPE_OLD,
	STRUCTTYPE_NEW
};

int CSoftKeyDesc::structtype = STRUCTTYPE_NONE;

int CSoftKeyDesc::getstructtype()
{
	if( structtype == STRUCTTYPE_NONE )
	{
		switch( GetChipID() & CHIPID_MASK )
		{
		case CHIPID_DB2000:
		case CHIPID_DB2010:
		case CHIPID_DB2020:
			structtype = STRUCTTYPE_OLD;
			break;
		default:
			structtype = STRUCTTYPE_NEW;
		}
	}
	return structtype;
}

bool CSoftKeyDesc::IsVisible()
{
	if( !this )
		return false;

	if( getstructtype() == STRUCTTYPE_OLD )
		return reinterpret_cast<SOFTKEY_DESC*>( this )->visible;

	return reinterpret_cast<SOFTKEY_DESC_A2*>( this )->visible;
}

u16 CSoftKeyDesc::GetAction()
{
	if( !this )
		return 0xFFFF;

	return reinterpret_cast<SOFTKEY_DESC*>( this )->action;
}

STRID CSoftKeyDesc::GetButtonText()
{
	if( !this )
		return EMPTY_SID;

	return reinterpret_cast<SOFTKEY_DESC*>( this )->ButtonText;
}

CSoftKeyDesc* FindSoftkey( DISP_OBJ* disp, BOOK* book, STRID strid )
{
	wchar_t temp[32];
	temp[0]=0;

	LIST* softkeys = DispObject_SoftKeys_GetList( disp, book, 0 );
	if( softkeys )
	{
		for( int i=0; i< softkeys->FirstFree; i++ )
		{
			CSoftKeyDesc* sk = reinterpret_cast<CSoftKeyDesc*>( List_Get( softkeys, i ) );
			if( sk->IsVisible() )
			{
				if( isallocatedstrid(sk->GetButtonText()) || isallocatedstrid(strid) )
				{
					if( !temp[0] )
						TextID2wstr( strid, temp, MAXELEMS( temp ) );

					wchar_t temp2[32];
					TextID2wstr( sk->GetButtonText(), temp2, MAXELEMS( temp2 ) );

					if( !wstrcmp( temp, temp2 ) )
						return sk;
				}else
				{
					if( sk->GetButtonText() == strid )
						return sk;
				}
			}
		}
	}
	return NULL;
}
