#include "../inc/swilib.h"
#include "parse_oms.h"
#include "additems.h"
#include "string_works.h"
#include "..\inc\zlib.h"
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

// Функции-заглушки для ZLib
void* zcalloc(int unk,size_t nelem, size_t elsize)
{
  return (malloc(nelem*elsize));
}

void zcfree(int unk, void* ptr)
{
  mfree(ptr);
}

//static const char slash[]="/";

void OMS_DataArrived(VIEWDATA *vd, const char *buf, int len)
{
  int i;
  unsigned int k;
  int err;
  //  unsigned int iw;
  //  unsigned int ih;
  char s[128];
  if ((len==0)||(buf==0))
  {
    vd->parse_state=OMS_STOP;
    return;
  }
  if (vd->zs)
  {
  L_ZBEGIN:
    vd->zs->next_in=(Byte *)buf;
    vd->zs->avail_in=len;
  L_ZNEXT:
    vd->zs->next_out=(Byte *)(vd->oms=realloc(vd->oms,vd->oms_size+len))+vd->oms_size;
    vd->zs->avail_out=len;
    err=inflate(vd->zs, Z_NO_FLUSH);
    switch (err)
    {
    case Z_NEED_DICT:
    case Z_DATA_ERROR:
    case Z_MEM_ERROR:
      sprintf(s,"ZLib Err %d\n",err);
      AddTextItem(vd,s,strlen(s));
      AddBrItem(vd);
      vd->parse_state=OMS_STOP;
      return;
    }
    vd->oms_size+=len-vd->zs->avail_out;
  }
  else
  {
    memcpy((vd->oms=realloc(vd->oms,i=vd->oms_size+len))+vd->oms_size,buf,len);
    vd->oms_size=i;
  }
  if (vd->parse_state==OMS_BEGIN)
  {
    vd->oms_wanted=sizeof(OMS_HEADER_COMMON);
    vd->parse_state=OMS_HDR_COMMON;
  }
  OMS_HEADER_COMMON *hdr=(OMS_HEADER_COMMON *)vd->oms;
  while(vd->oms_size>=vd->oms_wanted)
  {
    switch(vd->parse_state)
    {
    case OMS_HDR_COMMON:
      //Получен заголовок
      vd->oms_pos=vd->oms_wanted;
      vd->oms_wanted+=sizeof(OMS_HEADER_V2);
      vd->parse_state=OMS_HDR;
      {
	switch(hdr->magic)
	{
	case 0x330D:
	  vd->oms_wanted-=2;
	  break;
	case 0x3318:
	  break;
	case 0x310D:
	  vd->oms_wanted-=sizeof(OMS_HEADER_V2)-10; //10 - размер хедера GZIP
	  vd->parse_state=OMS_GZIPHDR;
	  break;
	case 0x3218:
	L_ZINIT:
	  //Производим инициализацию ZLib
	  zeromem(vd->zs=malloc(sizeof(z_stream)),sizeof(z_stream));
	  vd->zs->zalloc = (alloc_func)zcalloc;
	  vd->zs->zfree = (free_func)zcfree;
	  vd->zs->opaque = (voidpf)0;
	  err = inflateInit2(vd->zs,-MAX_WBITS);
	  if(err!=Z_OK)
	  {
	    sprintf(s,"inflateInit2 err %d\n",err);
	    AddTextItem(vd,s,strlen(s));
	    AddBrItem(vd);
	    vd->parse_state=OMS_STOP;
	    return;
	  }
	  //Теперь после vd->oms_pos до vd->oms_size есть данные ZLib
	  //Они же есть в buf+len-(vd->oms_size-vd->oms_pos) длинной vd->oms_size-vd->oms_pos
	  //Их необходимо обработать через ZLib
	  i=vd->oms_size-vd->oms_pos;
	  if (!i) return; //Нет данных
	  buf=buf+len-i; //Новый указатель на данные
	  len=i; //Новая длинна
	  vd->oms_size=vd->oms_pos; //Возращаем размер на начало данных ZLib
	  goto L_ZBEGIN;
	default:
	  sprintf(s,"Not supported type %X\n",hdr->magic);
	  AddTextItem(vd,s,strlen(s));
	  AddBrItem(vd);
	  vd->parse_state=OMS_STOP;
	  return;
	}
      }
      break;
    case OMS_GZIPHDR:
      //Пропустили хедер гзипа
      vd->oms_pos=vd->oms_wanted;
      vd->oms_wanted+=sizeof(OMS_HEADER_V2)-2; //Заголовок V1
      vd->parse_state=OMS_HDR;
      goto L_ZINIT;
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
	freegstr(&vd->work_ref.form_id1);
	freegstr(&vd->work_ref.form_id2);
	goto L_NOSTAGE2;
      case 'A':
	vd->oms_wanted+=2;
	break;
      case 'C':
	vd->oms_pos++;
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
	vd->oms_wanted+=2;
	break;
      case 'x':
	if ((((OMS_HEADER_COMMON *)vd->oms)->magic&0xFF)==0x0D)
	{
	  vd->oms_wanted+=2;
	}
	else
	{
	  vd->oms_wanted+=3;
	}
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
	AddBeginRef(vd);
	vd->tag_l_count=2;
//	AddTextItem(vd,"<l>",3);
        vd->oms_pos++;
	goto L_NOSTAGE2;
      case 'i':
	if (!vd->tag_l_count)
	{
	  AddBeginRef(vd);
	  vd->tag_l_count=1;
	}
	vd->oms_wanted+=2;
	break;
      case 'L':
	vd->oms_wanted+=2;
	break;
      case 'P':
        vd->oms_wanted+=2;
        break;
      case 'R':
        vd->oms_wanted+=2;
        break;        
      case 'E':
	AddEndRef(vd);
        vd->ref_mode=0;
        vd->oms_pos++;
	goto L_NOSTAGE2;
      case 'Q':
	AddTextItem(vd,"\n<Q>",4);
	AddBrItem(vd);
	vd->parse_state=OMS_STOP;
	return;
      case 'Z':
	vd->oms_wanted+=2;
	break;
      default:
	sprintf(s,"Unknown tag %c\n",i);
	AddTextItem(vd,s,strlen(s));
	AddBrItem(vd);
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
      case 'A':
	i=_rshort(vd);
//	vd->work_ref.tag='A';
	vd->oms_wanted+=i;
	vd->parse_state=OMS_TAGA_STAGE3;
	goto L_STAGE3_WANTED;
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
	vd->iw=_rshort(vd); //width
	vd->ih=_rshort(vd); //height
	AddPictureItemFrame(vd,vd->iw,vd->ih);
	if (vd->tag_l_count)
	{
	  if (!(--vd->tag_l_count))
	  {
	    AddEndRef(vd);
	  }
	}
	break;
      case 'K':
	_rshort(vd); //width
	_rshort(vd); //height
	i=_rshort(vd); //index
	AddPictureItemIndex(vd,i);
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
	vd->oms_wanted+=i;
	vd->parse_state=OMS_TAGh_STAGE3;
	goto L_STAGE3_WANTED;
      case 'e':
	i=(vd->iw=_rshort(vd))+2;
	vd->work_ref.tag='e';
	vd->oms_wanted+=i;
	vd->parse_state=OMS_TAGe_STAGE3;
	goto L_STAGE3_WANTED;
      case 'p':
	i=(vd->iw=_rshort(vd))+2;
	AddBeginRef(vd);
	vd->work_ref.tag='p';
	vd->oms_wanted+=i;
	vd->parse_state=OMS_TAGp_STAGE3;
	goto L_STAGE3_WANTED;
      case 'u':
	i=(vd->iw=_rshort(vd))+2;
	AddBeginRef(vd);
	vd->work_ref.tag='u';
	vd->oms_wanted+=i;
	vd->parse_state=OMS_TAGu_STAGE3;
	goto L_STAGE3_WANTED;
      case 'x':
	if ((((OMS_HEADER_COMMON *)vd->oms)->magic&0xFF)!=0x0D) _rbyte(vd);
	i=(vd->iw=_rshort(vd))+2;
	AddBeginRef(vd);
	vd->work_ref.tag='x';
	vd->oms_wanted+=i;
	vd->parse_state=OMS_TAGx_STAGE3;
	goto L_STAGE3_WANTED;
      case 'c':
	i=(vd->iw=_rshort(vd))+2;
	AddBeginRef(vd);
	vd->work_ref.tag='c';
	vd->oms_wanted+=i;
	vd->parse_state=OMS_TAGc_STAGE3;
	goto L_STAGE3_WANTED;
      case 'r':
	i=(vd->iw=_rshort(vd))+2;
	AddBeginRef(vd);
	vd->work_ref.tag='r';
	vd->oms_wanted+=i;
	vd->parse_state=OMS_TAGr_STAGE3;
	goto L_STAGE3_WANTED;
      case 's':
	i=(vd->iw=_rshort(vd))+3;
	AddBeginRef(vd);
	vd->work_ref.tag='s';
	AddTextItem(vd,"<s>",3);
	vd->oms_wanted+=i;
	vd->parse_state=OMS_TAGs_STAGE3;
	goto L_STAGE3_WANTED;
      case 'o':
	if (vd->tag_o_count)
	{
	  vd->tag_o_count--;
	  i=(vd->iw=_rshort(vd))+2;
	  vd->work_ref.tag='s';
	  AddTextItem(vd,"<o>",3);
	  vd->oms_wanted+=i;
	  vd->parse_state=OMS_TAGo_STAGE3;
	  goto L_STAGE3_WANTED;
	}
	else
	{
	  AddTextItem(vd,"!Illegal <o>!\n",14);
	  AddBrItem(vd);
	  vd->parse_state=OMS_STOP;
	  return;
	}
      case 'i':
	i=(vd->iw=_rshort(vd))+2;
	vd->work_ref.tag='i';
	vd->oms_wanted+=i;
	vd->parse_state=OMS_TAGi_STAGE3;
	goto L_STAGE3_WANTED;
      case 'L':
	i=_rshort(vd);
	vd->work_ref.tag='L';
        vd->ref_mode=1;
	AddBeginRef(vd);
	vd->oms_wanted+=i;
	vd->parse_state=OMS_TAGL_STAGE3;
	goto L_STAGE3_WANTED;
      case 'P':
	i=_rshort(vd);
	vd->oms_wanted+=i;
	vd->parse_state=OMS_TAGP_STAGE3;
	goto L_STAGE3_WANTED;
      case 'R':
        i=_rshort(vd);
        AddPictureItemHr(vd);
        break;
      case 'Z':
	i=(vd->iw=_rshort(vd));
	vd->work_ref.tag='Z';
        vd->ref_mode=1;
	AddBeginRef(vd);
	vd->oms_wanted+=i;
	vd->parse_state=OMS_TAGZ_STAGE3;
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
    case OMS_TAGA_STAGE3:
      i=vd->oms_wanted-vd->oms_pos;
      //AddTextItem(vd,vd->oms+vd->oms_pos,i);
      vd->oms_pos=vd->oms_wanted;
      vd->oms_wanted++;
      vd->parse_state=OMS_TAG_NAME;
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
      AddPictureItem(vd,(void *)(vd->oms+vd->oms_pos));
      if (vd->tag_l_count)
      {
	if (!(--vd->tag_l_count))
	{
	  AddEndRef(vd);
	}
      }
      vd->oms_pos=vd->oms_wanted;
      vd->oms_wanted++;
      vd->parse_state=OMS_TAG_NAME;
      break;
    case OMS_TAGX_STAGE3:
      //i=vd->oms_wanted-vd->oms_pos; //Size of picture
      AddPictureItemRGBA(vd,(void *)(vd->oms+vd->oms_pos),vd->iw,vd->ih);
      vd->oms_pos=vd->oms_wanted;
      vd->oms_wanted++;
      vd->parse_state=OMS_TAG_NAME;
      break;
    case OMS_TAGh_STAGE3:
      i=vd->iw;
      replacegstr(&vd->work_ref.form_id1,vd->oms+vd->oms_pos,i);
      vd->oms_pos+=i;
      i=(vd->ih=_rshort(vd));
      vd->oms_wanted+=i;
      vd->parse_state=OMS_TAGh_STAGE4;
      break;
    case OMS_TAGh_STAGE4:
      i=vd->ih;
      replacegstr(&vd->work_ref.form_id2,vd->oms+vd->oms_pos,i);
      vd->oms_pos+=i;
      vd->oms_wanted++;
      vd->parse_state=OMS_TAG_NAME;
      break;
    case OMS_TAGe_STAGE3:
      i=vd->iw;
      replacegstr(&vd->work_ref.id,vd->oms+vd->oms_pos,i);
      vd->oms_pos+=i;
      i=(vd->ih=_rshort(vd));
      vd->oms_wanted+=i;
      vd->parse_state=OMS_TAGe_STAGE4;
      break;
    case OMS_TAGe_STAGE4:
      i=vd->ih;
      replacegstr(&vd->work_ref.value,vd->oms+vd->oms_pos,i);
//      AddEndRef(vd);
      vd->oms_pos+=i;
      vd->oms_wanted++;
      vd->parse_state=OMS_TAG_NAME;
      break;
    case OMS_TAGp_STAGE3:
      i=vd->iw;
      replacegstr(&vd->work_ref.id,vd->oms+vd->oms_pos,i);
      vd->oms_pos+=i;
      i=(vd->ih=_rshort(vd));
      vd->oms_wanted+=i;
      vd->parse_state=OMS_TAGp_STAGE4;
      break;
    case OMS_TAGp_STAGE4:
      i=vd->ih;
      replacegstr(&vd->work_ref.value,vd->oms+vd->oms_pos,i);
      AddPassInputItem(vd,vd->oms+vd->oms_pos,i);
      AddEndRef(vd);
      vd->oms_pos+=i;
      vd->oms_wanted++;
      vd->parse_state=OMS_TAG_NAME;
      break;
    case OMS_TAGu_STAGE3:
      i=vd->iw;
      replacegstr(&vd->work_ref.id,vd->oms+vd->oms_pos,i);
      vd->oms_pos+=i;
      i=(vd->ih=_rshort(vd));
      vd->oms_wanted+=i;
      vd->parse_state=OMS_TAGu_STAGE4;
      break;
    case OMS_TAGu_STAGE4:
      i=vd->ih;
      replacegstr(&vd->work_ref.value,vd->oms+vd->oms_pos,i);
      AddButtonItem(vd,vd->oms+vd->oms_pos,i);
      AddEndRef(vd);
      vd->oms_pos+=i;
      vd->oms_wanted++;
      vd->parse_state=OMS_TAG_NAME;
      break;
    case OMS_TAGx_STAGE3:
      i=vd->iw;
      replacegstr(&vd->work_ref.id,vd->oms+vd->oms_pos,i);
      vd->oms_pos+=i;
      i=(vd->ih=_rshort(vd));
      vd->oms_wanted+=i;
      vd->parse_state=OMS_TAGx_STAGE4;
      break;
    case OMS_TAGx_STAGE4:
      i=vd->ih;
      replacegstr(&vd->work_ref.value,vd->oms+vd->oms_pos,i);
      AddInputItem(vd,vd->oms+vd->oms_pos,i);
      AddEndRef(vd);
      vd->oms_pos+=i;
      vd->oms_wanted++;
      vd->parse_state=OMS_TAG_NAME;
      break;
    case OMS_TAGc_STAGE3:
      i=vd->iw;
      replacegstr(&vd->work_ref.id,vd->oms+vd->oms_pos,i);
      vd->oms_pos+=i;
      i=(vd->ih=_rshort(vd))+1;
      vd->oms_wanted+=i;
      vd->parse_state=OMS_TAGc_STAGE4;
      break;
    case OMS_TAGc_STAGE4:
      i=vd->ih;
      replacegstr(&vd->work_ref.value,vd->oms+vd->oms_pos,i);
      vd->oms_pos+=i;
      vd->work_ref.group_id=_rbyte(vd); //group id
      AddCheckBoxItem(vd);
      AddEndRef(vd);
      vd->oms_wanted++;
      vd->parse_state=OMS_TAG_NAME;
      break;
    case OMS_TAGr_STAGE3:
      i=vd->iw;
      replacegstr(&vd->work_ref.id,vd->oms+vd->oms_pos,i);
      vd->oms_pos+=i;
      i=(vd->ih=_rshort(vd))+1;
      vd->oms_wanted+=i;
      vd->parse_state=OMS_TAGr_STAGE4;
      break;
    case OMS_TAGr_STAGE4:
      i=vd->ih;
      replacegstr(&vd->work_ref.value,vd->oms+vd->oms_pos,i);
      vd->oms_pos+=i;
      vd->work_ref.group_id=_rbyte(vd); //group id
      AddRadioButton(vd);
      AddEndRef(vd);
      vd->oms_wanted++;
      vd->parse_state=OMS_TAG_NAME;
      break;
    case OMS_TAGs_STAGE3:
      i=vd->iw;
      replacegstr(&vd->work_ref.id,vd->oms+vd->oms_pos,i);
      vd->oms_pos+=i;
      _rbyte(vd);
      vd->tag_o_count=_rshort(vd);
      vd->oms_wanted++;
      vd->parse_state=OMS_TAG_NAME;
      break;
    case OMS_TAGo_STAGE3:
      i=vd->iw;
      replacegstr(&vd->work_ref.value,vd->oms+vd->oms_pos,i);
      vd->oms_pos+=i;
      i=(vd->ih=_rshort(vd))+1;
      vd->oms_wanted+=i;
      vd->parse_state=OMS_TAGo_STAGE4;
      break;
    case OMS_TAGo_STAGE4:
      i=vd->ih;
      replacegstr(&vd->work_ref.id2,vd->oms+vd->oms_pos,i);
      vd->oms_pos+=i;
      vd->work_ref.checked=_rbyte(vd); //checked/unchecked
      if (!vd->tag_o_count)
      {
	AddEndRef(vd);
      }
      vd->oms_wanted++;
      vd->parse_state=OMS_TAG_NAME;
      break;
    case OMS_TAGi_STAGE3:
      i=vd->iw;
      replacegstr(&vd->work_ref.id,vd->oms+vd->oms_pos,i);
      vd->oms_pos+=i;
      i=(vd->ih=_rshort(vd));
      vd->oms_wanted+=i;
      vd->parse_state=OMS_TAGi_STAGE4;
      break;
    case OMS_TAGi_STAGE4:
      i=vd->ih;
      replacegstr(&vd->work_ref.value,vd->oms+vd->oms_pos,i);
      AddTextItem(vd,vd->oms+vd->oms_pos,i);
      if (vd->tag_l_count)
      {
	if (!(--vd->tag_l_count))
	{
	  AddEndRef(vd);
	}
      }
      else
      {
	AddTextItem(vd,"!Illegal <i>!",13);
	AddBrItem(vd);
	vd->parse_state=OMS_STOP;
	return;
      }
      vd->oms_pos+=i;
//      _rshort(vd); //unk
      vd->oms_wanted++;
      vd->parse_state=OMS_TAG_NAME;
      break;
    case OMS_TAGL_STAGE3:
      i=vd->oms_wanted-vd->oms_pos;
      replacegstr(&vd->work_ref.id,vd->oms+vd->oms_pos,i);
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
    case OMS_TAGZ_STAGE3:
      i=vd->iw;
      AddTextItem(vd,vd->oms+vd->oms_pos,i);
      replacegstr(&vd->work_ref.id,vd->oms+vd->oms_pos,i);
      vd->oms_pos+=i;
      AddEndRef(vd);
      vd->oms_wanted++;
      vd->parse_state=OMS_TAG_NAME;
      break;
    case OMS_STOP:
      return;
    default:
      sprintf(s,"StateMachine failed %u",i);
      AddTextItem(vd,s,strlen(s));
      AddBrItem(vd);
      vd->parse_state=OMS_STOP;
      return;
    }
  }
  if (vd->zs)
  {
    if (vd->zs->avail_out==0) goto L_ZNEXT;
  }
}



