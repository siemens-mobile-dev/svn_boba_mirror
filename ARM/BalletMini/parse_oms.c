
#include "../inc/swilib.h"
#include "parse_oms.h"
#include "additems.h"
//#include <stdlib.h>
//#include <string.h>
//#include <stdio.h>

#pragma inline
unsigned int _rbyte(VIEWDATA *vd)
{
  unsigned char *p=(unsigned char *)(vd->oms+vd->oms_pos);
  unsigned int r=*p++;
  vd->oms_pos+=1;
  return r;
}

unsigned int _rshort(VIEWDATA *vd)
{
  unsigned char *p=(unsigned char *)(vd->oms+vd->oms_pos);
  unsigned int r=*p++;
  r<<=8;
  r|=*p;
  vd->oms_pos+=2;
  return r;
}

unsigned int _rlong(VIEWDATA *vd)
{
  unsigned char *p=(unsigned char *)(vd->oms+vd->oms_pos);
  unsigned int r=*p++;
  r<<=8;r|=*p++;
  r<<=8;r|=*p++;
  r<<=8;r|=*p;
  vd->oms_pos+=4;
  return r;
}

//static const char slash[]="/";

void OMS_DataArrived(VIEWDATA *vd, const char *buf, int len)
{
  int i;
  unsigned int k;
  //  unsigned int iw;
  //  unsigned int ih;
  char s[128];
  if ((len==0)||(buf==0))
  {
    vd->parse_state=OMS_STOP;
    return;
  }
  memcpy((vd->oms=realloc(vd->oms,i=vd->oms_size+len))+vd->oms_size,buf,len);
  vd->oms_size=i;
  if (vd->parse_state==OMS_BEGIN)
  {
    vd->oms_wanted=sizeof(OMS_HEADER);
    vd->parse_state=OMS_HDR;
  }
  while(vd->oms_size>=vd->oms_wanted)
  {
    switch(vd->parse_state)
    {
    case OMS_HDR:
      vd->oms_pos=vd->oms_wanted;
      vd->oms_wanted+=2;
      vd->parse_state=OMS_PAGEURL_SIZE;
      break;
    case OMS_PAGEURL_SIZE:
      vd->oms_wanted+=_rshort(vd);
      vd->parse_state=OMS_PAGEURL_DATA;
      break;
    case OMS_PAGEURL_DATA:
      i=vd->oms_wanted-vd->oms_pos;
//      AddTextItem(vd,vd->oms+vd->oms_pos,i);
//      AddBrItem(vd);
      vd->oms_pos=vd->oms_wanted;
      vd->oms_wanted++;
      vd->parse_state=OMS_TAG_NAME;
      break;
    case OMS_TAG_NAME:
      //Stage 1
      switch(i=vd->oms[vd->oms_pos])
      {
      case '+':
        vd->oms_pos++;
	goto L_NOSTAGE2;
      case '$':
        vd->oms_pos++; //Form end
	goto L_NOSTAGE2;
      case 'D':
	vd->oms_wanted+=2;
	break;
      case 'S':
	vd->oms_wanted+=4;
	break;
      case 'T':
	vd->oms_wanted+=2;
	break;
      case 'Y':
	vd->oms_wanted+=1;
	break;
      case 'B':
	AddBrItem(vd);
        vd->oms_pos++;
	goto L_NOSTAGE2;
      case 'V':
	AddPItem(vd);
        vd->oms_pos++;
	goto L_NOSTAGE2;
      case 'I':
	vd->oms_wanted+=8;
	break;
      case 'J':
	vd->oms_wanted+=4;
	break;
      case 'K':
	vd->oms_wanted+=6;
	break;
      case 'X':
	vd->oms_wanted+=4;
	break;
      case 'h':
	vd->oms_wanted+=2;
	break;
      case 'e':
      case 'p':
      case 'u':
      case 'x':
	vd->oms_wanted+=2;
	break;
      case 'c':
      case 'r':
	vd->oms_wanted+=2;
	break;
      case 's':
	vd->oms_wanted+=2;
	break;
      case 'o':
	vd->oms_wanted+=2;
	break;
      case 'l':
	AddTextItem(vd,"<l>",3);
        vd->oms_pos++;
	goto L_NOSTAGE2;
      case 'i':
	vd->oms_wanted+=2;
	break;
      case 'L':
	vd->oms_wanted+=2;
	break;
      case 'P':
        vd->oms_wanted+=2;
        break;
      case 'E':
	AddEndRef(vd);
        vd->ref_mode=0;
        vd->oms_pos++;
	goto L_NOSTAGE2;
      case 'Q':
	AddTextItem(vd,"\n<Q>",4);
	vd->parse_state=OMS_STOP;
	return;
      default:
	sprintf(s,"Unknown tag %c",i);
	AddTextItem(vd,s,strlen(s));
	vd->parse_state=OMS_STOP;
	return;
      }
      vd->parse_state=OMS_TAG_DATA;
      break;
    case OMS_TAG_DATA:
      i=vd->oms[vd->oms_pos];
      vd->oms_pos++;
      switch(i)
      {
      case 'D':
	*((unsigned short *)(&(vd->current_tag_d)))=_rshort(vd);
	AddNewStyle(vd);
	break;
      case 'S':
	*((unsigned int *)(&(vd->current_tag_s)))=k=_rlong(vd);
	((unsigned int *)(vd->S_cache=realloc(vd->S_cache,(vd->S_cache_size+1)*sizeof(TAG_S))))[vd->S_cache_size]=k;
	vd->S_cache_size++;
	AddNewStyle(vd);
	break;
      case 'T':
	i=_rshort(vd);
	vd->oms_wanted+=i;
	vd->parse_state=OMS_TAGT_STAGE3;
	goto L_STAGE3_WANTED;
      case 'Y':
	i=_rbyte(vd);
	vd->current_tag_s=vd->S_cache[i];
	AddNewStyle(vd);
	break;
      case 'I':
	vd->iw=_rshort(vd); //width
	vd->ih=_rshort(vd); //heigth
	i=_rshort(vd);
	_rshort(vd);
	vd->oms_wanted+=i;
	vd->parse_state=OMS_TAGI_STAGE3;
	goto L_STAGE3_WANTED;
      case 'J':
	_rshort(vd); //width
	_rshort(vd); //height
	AddPictureItem(vd,NULL);
	break;
      case 'K':
	_rshort(vd); //width
	_rshort(vd); //height
	_rshort(vd); //index
	AddPictureItem(vd,NULL);
	break;
      case 'X':
	vd->iw=_rbyte(vd); //width
	vd->ih=_rbyte(vd); //heigth
	i=_rshort(vd);
	vd->oms_wanted+=i;
	vd->parse_state=OMS_TAGX_STAGE3;
	goto L_STAGE3_WANTED;
      case 'h':
	i=(vd->iw=_rshort(vd))+2;
//	AddTextItem(vd,"<h>",3);
	vd->oms_wanted+=i;
	vd->parse_state=OMS_TAGh_STAGE3;
	goto L_STAGE3_WANTED;
      case 'e':
	i=(vd->iw=_rshort(vd))+2;
//	AddTextItem(vd,"<e>",3);
	vd->oms_wanted+=i;
	vd->parse_state=OMS_TAGe_STAGE3;
	goto L_STAGE3_WANTED;
      case 'p':
	i=(vd->iw=_rshort(vd))+2;
//	AddTextItem(vd,"{",3);
	vd->oms_wanted+=i;
	vd->parse_state=OMS_TAGp_STAGE3;
	goto L_STAGE3_WANTED;
      case 'u':
	i=(vd->iw=_rshort(vd))+2;
//	AddTextItem(vd,"<u>",3);
	vd->oms_wanted+=i;
	vd->parse_state=OMS_TAGu_STAGE3;
	goto L_STAGE3_WANTED;
      case 'x':
	i=(vd->iw=_rshort(vd))+2;
//	AddTextItem(vd,"{",1);
	vd->oms_wanted+=i;
	vd->parse_state=OMS_TAGx_STAGE3;
	goto L_STAGE3_WANTED;
      case 'c':
	i=(vd->iw=_rshort(vd))+2;
	AddCheckBoxItem(vd);
//	AddTextItem(vd,"[v]",3);
	vd->oms_wanted+=i;
	vd->parse_state=OMS_TAGc_STAGE3;
	goto L_STAGE3_WANTED;
      case 'r':
	i=(vd->iw=_rshort(vd))+2;
	AddRadioButton(vd);
//	AddTextItem(vd,"[_]",3);
	vd->oms_wanted+=i;
	vd->parse_state=OMS_TAGr_STAGE3;
	goto L_STAGE3_WANTED;
      case 's':
	i=(vd->iw=_rshort(vd))+3;
	AddTextItem(vd,"<s>",3);
	vd->oms_wanted+=i;
	vd->parse_state=OMS_TAGs_STAGE3;
	goto L_STAGE3_WANTED;
      case 'o':
	if (vd->tag_o_count)
	{
	  vd->tag_o_count--;
	  i=(vd->iw=_rshort(vd))+2;
	  AddTextItem(vd,"<o>",3);
	  vd->oms_wanted+=i;
	  vd->parse_state=OMS_TAGo_STAGE3;
	  goto L_STAGE3_WANTED;
	}
	else
	{
	  AddTextItem(vd,"!Illegal <o>!",13);
	  vd->parse_state=OMS_STOP;
	  return;
	}
      case 'i':
	i=(vd->iw=_rshort(vd))+2;
	AddTextItem(vd,"<i>",3);
	vd->oms_wanted+=i;
	vd->parse_state=OMS_TAGi_STAGE3;
	goto L_STAGE3_WANTED;
      case 'L':
	i=_rshort(vd);
        vd->ref_mode=1;
	AddBeginRef(vd);
	vd->oms_wanted+=i;
	vd->parse_state=OMS_TAGL_STAGE3;
	goto L_STAGE3_WANTED;
      case 'P':
	i=_rshort(vd);
//	AddTextItem(vd,"<L>",3);
	vd->oms_wanted+=i;
	vd->parse_state=OMS_TAGP_STAGE3;
	goto L_STAGE3_WANTED;
      default:
	//	vd->parse_state=OMS_STOP;
	break;
      }
    L_NOSTAGE2:
      vd->oms_wanted++;
      vd->parse_state=OMS_TAG_NAME;
    L_STAGE3_WANTED:
      break;
    case OMS_TAGT_STAGE3:
      i=vd->oms_wanted-vd->oms_pos;
      AddTextItem(vd,vd->oms+vd->oms_pos,i);
      vd->oms_pos=vd->oms_wanted;
      vd->oms_wanted++;
      vd->parse_state=OMS_TAG_NAME;
      break;
    case OMS_TAGI_STAGE3:
      //i=vd->oms_wanted-vd->oms_pos; //Size of picture
      AddPictureItem(vd,NULL);
      vd->oms_pos=vd->oms_wanted;
      vd->oms_wanted++;
      vd->parse_state=OMS_TAG_NAME;
      break;
    case OMS_TAGX_STAGE3:
      //i=vd->oms_wanted-vd->oms_pos; //Size of picture
      AddPictureItem(vd,NULL);
      vd->oms_pos=vd->oms_wanted;
      vd->oms_wanted++;
      vd->parse_state=OMS_TAG_NAME;
      break;
    case OMS_TAGh_STAGE3:
      i=vd->iw;
//      AddTextItem(vd,vd->oms+vd->oms_pos,i);
//      AddTextItem(vd,slash,1);
      vd->oms_pos+=i;
      i=(vd->ih=_rshort(vd));
      vd->oms_wanted+=i;
      vd->parse_state=OMS_TAGh_STAGE4;
      break;
    case OMS_TAGh_STAGE4:
      i=vd->ih;
//      AddTextItem(vd,vd->oms+vd->oms_pos,i);
//      AddTextItem(vd,slash,1);
      vd->oms_pos+=i;
      vd->oms_wanted++;
      vd->parse_state=OMS_TAG_NAME;
      break;
    case OMS_TAGe_STAGE3:
      i=vd->iw;
//      AddTextItem(vd,vd->oms+vd->oms_pos,i);
//      AddTextItem(vd,slash,1);
      vd->oms_pos+=i;
      i=(vd->ih=_rshort(vd));
      vd->oms_wanted+=i;
      vd->parse_state=OMS_TAGe_STAGE4;
      break;
    case OMS_TAGe_STAGE4:
      i=vd->ih;
//      AddTextItem(vd,vd->oms+vd->oms_pos,i);
//      AddTextItem(vd,slash,1);
      vd->oms_pos+=i;
      vd->oms_wanted++;
      vd->parse_state=OMS_TAG_NAME;
      break;
    case OMS_TAGp_STAGE3:
      i=vd->iw;
//      AddTextItem(vd,vd->oms+vd->oms_pos,i);
//      AddTextItem(vd,"[",1);
      vd->oms_pos+=i;
      i=(vd->ih=_rshort(vd));
      vd->oms_wanted+=i;
      vd->parse_state=OMS_TAGp_STAGE4;
      break;
    case OMS_TAGp_STAGE4:
      i=vd->ih;
      AddPassInputItem(vd,vd->oms+vd->oms_pos,i);
      vd->oms_pos+=i;
      vd->oms_wanted++;
      vd->parse_state=OMS_TAG_NAME;
      break;
    case OMS_TAGu_STAGE3:
      i=vd->iw;
//      AddTextItem(vd,vd->oms+vd->oms_pos,i);
//      AddTextItem(vd,"[",1);
      vd->oms_pos+=i;
      i=(vd->ih=_rshort(vd));
      vd->oms_wanted+=i;
      vd->parse_state=OMS_TAGu_STAGE4;
      break;
    case OMS_TAGu_STAGE4:
      i=vd->ih;
      AddButtonItem(vd,vd->oms+vd->oms_pos,i);
//      AddTextItem(vd,"[send]",1);
      vd->oms_pos+=i;
      vd->oms_wanted++;
      vd->parse_state=OMS_TAG_NAME;
      break;
    case OMS_TAGx_STAGE3:
      i=vd->iw;
//      AddTextItem(vd,vd->oms+vd->oms_pos,i);
//      AddTextItem(vd,slash,1);
      vd->oms_pos+=i;
      i=(vd->ih=_rshort(vd));
      vd->oms_wanted+=i;
      vd->parse_state=OMS_TAGx_STAGE4;
      break;
    case OMS_TAGx_STAGE4:
      i=vd->ih;
      AddInputItem(vd,vd->oms+vd->oms_pos,i);
      vd->oms_pos+=i;
      vd->oms_wanted++;
      vd->parse_state=OMS_TAG_NAME;
      break;
    case OMS_TAGc_STAGE3:
      i=vd->iw;
//      AddTextItem(vd,vd->oms+vd->oms_pos,i);
//      AddTextItem(vd,slash,1);
      vd->oms_pos+=i;
      i=(vd->ih=_rshort(vd))+1;
      vd->oms_wanted+=i;
      vd->parse_state=OMS_TAGc_STAGE4;
      break;
    case OMS_TAGc_STAGE4:
      i=vd->ih;
//      AddTextItem(vd,vd->oms+vd->oms_pos,i);
//      AddTextItem(vd,slash,1);
      vd->oms_pos+=i;
      _rbyte(vd); //group id
      vd->oms_wanted++;
      vd->parse_state=OMS_TAG_NAME;
      break;
    case OMS_TAGr_STAGE3:
      i=vd->iw;
//      AddTextItem(vd,vd->oms+vd->oms_pos,i);
//      AddTextItem(vd,slash,1);
      vd->oms_pos+=i;
      i=(vd->ih=_rshort(vd))+1;
      vd->oms_wanted+=i;
      vd->parse_state=OMS_TAGr_STAGE4;
      break;
    case OMS_TAGr_STAGE4:
      i=vd->ih;
//      AddTextItem(vd,vd->oms+vd->oms_pos,i);
//      AddTextItem(vd,slash,1);
      vd->oms_pos+=i;
      _rbyte(vd); //group id
      vd->oms_wanted++;
      vd->parse_state=OMS_TAG_NAME;
      break;
    case OMS_TAGs_STAGE3:
      i=vd->iw;
//      AddTextItem(vd,vd->oms+vd->oms_pos,i);
//      AddTextItem(vd,slash,1);
      vd->oms_pos+=i;
      _rbyte(vd);
      vd->tag_o_count=_rshort(vd);
      vd->oms_wanted++;
      vd->parse_state=OMS_TAG_NAME;
      break;
    case OMS_TAGo_STAGE3:
      i=vd->iw;
//      AddTextItem(vd,vd->oms+vd->oms_pos,i);
//      AddTextItem(vd,slash,1);
      vd->oms_pos+=i;
      i=(vd->ih=_rshort(vd))+1;
      vd->oms_wanted+=i;
      vd->parse_state=OMS_TAGo_STAGE4;
      break;
    case OMS_TAGo_STAGE4:
      i=vd->ih;
//      AddTextItem(vd,vd->oms+vd->oms_pos,i);
//      AddTextItem(vd,slash,1);
      vd->oms_pos+=i;
      _rbyte(vd); //checked/unchecked
      vd->oms_wanted++;
      vd->parse_state=OMS_TAG_NAME;
      break;
    case OMS_TAGi_STAGE3:
      i=vd->iw;
//      AddTextItem(vd,vd->oms+vd->oms_pos,i);
//      AddTextItem(vd,slash,1);
      vd->oms_pos+=i;
      _rshort(vd); //unk
      vd->oms_wanted++;
      vd->parse_state=OMS_TAG_NAME;
      break;
    case OMS_TAGL_STAGE3:
      i=vd->oms_wanted-vd->oms_pos;
//      AddTextItem(vd,vd->oms+vd->oms_pos,i);
//      AddTextItem(vd,slash,1);
      vd->oms_pos=vd->oms_wanted;
      vd->oms_wanted++;
      vd->parse_state=OMS_TAG_NAME;
      break;
    case OMS_TAGP_STAGE3:
      i=vd->oms_wanted-vd->oms_pos;
//      AddTextItem(vd,vd->oms+vd->oms_pos,i);
//      AddTextItem(vd,slash,1);
      vd->oms_pos=vd->oms_wanted;
      vd->oms_wanted++;
      vd->parse_state=OMS_TAG_NAME;
      break;
    default:
      sprintf(s,"StateMachine failed %u",i);
      AddTextItem(vd,s,strlen(s));
      vd->parse_state=OMS_STOP;
      return;
    }
  }
}



