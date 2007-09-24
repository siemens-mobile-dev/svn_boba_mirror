/***************************************************************************
 *   Copyright (C) 2007 by Alexander S. Salieff                            *
 *   salieff@mail.ru                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifdef _WIN32
 #include <winsock2.h>
 #define MSG_NOSIGNAL 0
 typedef int socklen_t;
#else
 #include <sys/types.h>
 #include <sys/socket.h>
#endif

#ifdef __FreeBSD__
# include <netinet/in.h>
#endif

#define TNETWORK_CLOSE 0
#define TNETWORK_ERR -1
#define TNETWORK_TIMEOUT -2

#include "flap.h"
#include <string.h>

#ifndef _WIN32
 #include <sys/time.h>
 #include <sys/types.h>
 #include <unistd.h>
 #include <fcntl.h>
 #include <errno.h>
#endif

/*#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>*/
                     
// ----------------=========ooooOOOOOOOOOoooo=========----------------
FlapPacket::FlapPacket(void)
{
}

// ----------------=========ooooOOOOOOOOOoooo=========----------------
FlapPacket::~FlapPacket()
{
}

// ----------------=========ooooOOOOOOOOOoooo=========----------------

bool FlapPacket::recv_from(int sock)
{
 vector<uint8_t> tmp_buf(6);
 unsigned long res;
 t_network_error=TNETWORK_ERR;
 fd_set rfds;
 struct timeval tv;

 res=0;
 if (ioctlsocket(sock,FIONREAD,&res)) return false;
 if (res<6)
 {
  tv.tv_sec=0;
  tv.tv_usec=100000; //ќжидание 100мс
  FD_ZERO(&rfds);
  FD_SET((SOCKET)sock, &rfds);
  int sel_ret = select(sock+1, &rfds, NULL, NULL, &tv);
  if (sel_ret==0) goto L_NTO;
  if (sel_ret<0) return false;
  if (!FD_ISSET(sock, &rfds)) return false; //TNETWORK_ERR;
  res=0;
  if (ioctlsocket(sock,FIONREAD,&res)) return false;
  if (res<1) return false; //ќбрыв св€зи
 }
 if (res<6)
 {
 L_NTO:
  t_network_error=TNETWORK_TIMEOUT;
  return false;
 }
 if (recv(sock,(char*)&tmp_buf[0],6,0)!=6) return false;

 if (tmp_buf[0]!='*') return false;
 frame_type = tmp_buf[1];
 memcpy(&seq_number, &tmp_buf[2], sizeof(seq_number));
 seq_number=ntohs(seq_number);
 uint16_t payload_length;
 memcpy(&payload_length, &tmp_buf[4], sizeof(payload_length));
 payload_length=ntohs(payload_length);
 payload.resize(payload_length);
 char *p=(char*)&payload[0];
 int l=payload_length;
 int i;
 while(l>0)
 {
  res=0;
  if (ioctlsocket(sock,FIONREAD,&res)) return false;
  if (!res)
  {
   tv.tv_sec=30;
   tv.tv_usec=0;
   FD_ZERO(&rfds);
   FD_SET((SOCKET)sock, &rfds);
   int sel_ret = select(sock+1, &rfds, NULL, NULL, &tv);
   if (sel_ret==0) return false;
   if (sel_ret<0) return false;
   if (!FD_ISSET(sock, &rfds)) return false; //TNETWORK_ERR;
   res=0;
   if (ioctlsocket(sock,FIONREAD,&res)) return false;
  }
  if (res<1) return false;
  if (res>(unsigned int)l) res=l;
  i=recv(sock,p,res,0);
  if (i<=0) return false;
  p+=i;
  l-=i;
 }
 return true;
}

// ----------------=========ooooOOOOOOOOOoooo=========----------------
bool FlapPacket::send_to(int sock)
{
 vector<uint8_t> tmp_buf(6);
 tmp_buf[0]='*';
 tmp_buf[1]=frame_type;
 uint16_t tmp_seq_number = htons(seq_number);
 memcpy(&tmp_buf[2], &tmp_seq_number, sizeof(tmp_seq_number));
 uint16_t payload_length = htons(payload.size());
 memcpy(&tmp_buf[4], &payload_length, sizeof(payload_length));
 t_network_error=TNETWORK_ERR;
 if (send(sock, (char*)&tmp_buf[0], 6, 0)!=6) return false;
 if(payload.size())
	if (send(sock, (char*)&payload[0], payload.size(), 0)!=(int)payload.size()) return false;
 return true;
}

// ----------------=========ooooOOOOOOOOOoooo=========----------------
SNACData::SNACData(void)
{
}

// ----------------=========ooooOOOOOOOOOoooo=========----------------
SNACData::~SNACData()
{
}

// ----------------=========ooooOOOOOOOOOoooo=========----------------
bool SNACData::decode_from(vector<uint8_t> & v, size_t start_ind)
{
 if (v.size()<(start_ind+10)) return false;
 memcpy(&service_id, &v[start_ind], sizeof(service_id));
 service_id=ntohs(service_id);
 memcpy(&subtype_id, &v[start_ind+2], sizeof(subtype_id));
 subtype_id=ntohs(subtype_id);
 memcpy(&flags, &v[start_ind+4], sizeof(flags));
 flags=ntohs(flags);
 memcpy(&req_id, &v[start_ind+6], sizeof(req_id));
 req_id=ntohl(req_id);

 data.assign(v.begin()+start_ind+10, v.end());
 return true;
}

// ----------------=========ooooOOOOOOOOOoooo=========----------------
void SNACData::encode_to(vector<uint8_t> & v, size_t start_ind)
{
 if (v.size()<(start_ind+10+data.size())) v.resize(start_ind+10+data.size());
 uint16_t tmp_service_id = htons(service_id);
 uint16_t tmp_subtype_id = htons(subtype_id);
 uint16_t tmp_flags = htons(flags);
 uint32_t tmp_req_id = htonl(req_id);

 memcpy(&v[start_ind], &tmp_service_id, sizeof(tmp_service_id));
 memcpy(&v[start_ind+2], &tmp_subtype_id, sizeof(tmp_subtype_id));
 memcpy(&v[start_ind+4], &tmp_flags, sizeof(tmp_flags));
 memcpy(&v[start_ind+6], &tmp_req_id, sizeof(tmp_req_id));
 if(data.size())
	memcpy(&v[start_ind+10], &data[0], data.size());
}

// ----------------=========ooooOOOOOOOOOoooo=========----------------
TLVField::TLVField(void)
{
}

// ----------------=========ooooOOOOOOOOOoooo=========----------------
TLVField::~TLVField()
{
}

// ----------------=========ooooOOOOOOOOOoooo=========----------------
bool TLVField::getAsInt16(uint16_t & i)
{
 uint16_t tmp_i16;
 if (sizeof(tmp_i16)>data.size()) return false;
 memcpy(&tmp_i16, &data[0], sizeof(tmp_i16));
 i = ntohs(tmp_i16);
 return true;
}

// ----------------=========ooooOOOOOOOOOoooo=========----------------
void TLVField::putInt16(uint16_t i)
{
 uint16_t tmp_i16 = htons(i);
 data.resize(sizeof(tmp_i16));
 memcpy(&data[0], &tmp_i16, sizeof(tmp_i16));
}

// ----------------=========ooooOOOOOOOOOoooo=========----------------
bool TLVField::getAsInt32(uint32_t & i)
{
 uint32_t tmp_i32;
 if (sizeof(tmp_i32)>data.size()) return false;
 memcpy(&tmp_i32, &data[0], sizeof(tmp_i32));
 i = ntohl(tmp_i32);
 return true;
}

// ----------------=========ooooOOOOOOOOOoooo=========----------------
void TLVField::putInt32(uint32_t i)
{
 uint32_t tmp_i32 = htonl(i);
 data.resize(sizeof(tmp_i32));
 memcpy(&data[0], &tmp_i32, sizeof(tmp_i32));
}

// ----------------=========ooooOOOOOOOOOoooo=========----------------
bool TLVField::getAsString(string & s)
{
 s.assign(reinterpret_cast<const char *>(&data[0]), data.size());
 return true;
}

// ----------------=========ooooOOOOOOOOOoooo=========----------------
void TLVField::putString(string s)
{
 data.assign(s.begin(), s.end());
}

// ----------------=========ooooOOOOOOOOOoooo=========----------------
bool TLVField::decode_from(vector<uint8_t> & v, size_t start_ind)
{
 if (v.size()<(start_ind+4)) return false;
 uint16_t tmp_i, length;
 memcpy(&tmp_i, &v[start_ind], sizeof(tmp_i));
 type=ntohs(tmp_i);
 memcpy(&tmp_i, &v[start_ind+sizeof(tmp_i)], sizeof(tmp_i));
 length=ntohs(tmp_i);
 if (v.size()<(start_ind+4+length)) return false;
 data.assign(v.begin()+start_ind+4, v.begin()+start_ind+4+length);
 return true;
}

// ----------------=========ooooOOOOOOOOOoooo=========----------------
void TLVField::encode_to(vector<uint8_t> & v, size_t start_ind)
{
 if (v.size()<(start_ind+4+data.size())) v.resize(start_ind+4+data.size());
 uint16_t tmp_type = htons(type);
 uint16_t tmp_length = htons(data.size());
 
 memcpy(&v[start_ind], &tmp_type, sizeof(tmp_type));
 memcpy(&v[start_ind+2], &tmp_length, sizeof(tmp_length));
 memcpy(&v[start_ind+4], &data[0], data.size());
}

// ----------------=========ooooOOOOOOOOOoooo=========----------------
TLVPack::TLVPack(void)
{
}

// ----------------=========ooooOOOOOOOOOoooo=========----------------
TLVPack::~TLVPack()
{
}

// ----------------=========ooooOOOOOOOOOoooo=========----------------
bool TLVPack::decode_from(vector<uint8_t> & v, size_t start_ind)
{
 data.clear();
 
 while(true)
  {
  if (start_ind>=v.size()) break;
  TLVField tf;
  if (!tf.decode_from(v, start_ind)) return false;
  data.push_back(tf);
  start_ind+=tf.data.size()+4; // +2 for TLVtype +2 for TLV length
  }
  
 return true;
}

// ----------------=========ooooOOOOOOOOOoooo=========----------------
void TLVPack::encode_to(vector<uint8_t> & v, size_t start_ind)
{
 for (unsigned int i=0; i<data.size(); ++i)
  {
  data[i].encode_to(v, start_ind);
  start_ind+=data[i].data.size()+4; // +2 for TLVtype +2 for TLV length
  }
}

// ----------------=========ooooOOOOOOOOOoooo=========----------------
TLVField * TLVPack::findTLV(uint16_t t)
{
 for (unsigned int i=0; i<data.size(); ++i)
  if (data[i].type==t) return &data[i];
  
 return NULL;
}

// ----------------=========ooooOOOOOOOOOoooo=========----------------
SignOnPacket::SignOnPacket(void) : TLVPack()
{
}

// ----------------=========ooooOOOOOOOOOoooo=========----------------
SignOnPacket::~SignOnPacket()
{
}

// ----------------=========ooooOOOOOOOOOoooo=========----------------
bool SignOnPacket::decode_from(vector<uint8_t> & v, size_t start_ind)
{
 if (v.size()<(start_ind+4)) return false;
 uint32_t flap_version;
 memcpy(&flap_version, &v[start_ind], sizeof(flap_version));
 flap_version = ntohl(flap_version);
 if (flap_version!=1) return false;
 start_ind+=sizeof(flap_version);
  
 return TLVPack::decode_from(v, start_ind);
}

// ----------------=========ooooOOOOOOOOOoooo=========----------------
void SignOnPacket::encode_to(vector<uint8_t> & v, size_t start_ind)
{
 if (v.size()<(start_ind+4)) v.resize(start_ind+4);
 uint32_t flap_version = 1;
 flap_version = htonl(flap_version);
 memcpy(&v[start_ind], &flap_version, sizeof(flap_version));
 start_ind+=sizeof(flap_version);
 
 TLVPack::encode_to(v, start_ind);
}

// ----------------=========ooooOOOOOOOOOoooo=========----------------
SignOffPacket::SignOffPacket(void) : TLVPack()
{
}

// ----------------=========ooooOOOOOOOOOoooo=========----------------
SignOffPacket::~SignOffPacket()
{
}

// ----------------=========ooooOOOOOOOOOoooo=========----------------
bool SignOffPacket::decode_from(vector<uint8_t> & v, size_t start_ind)
{
 return TLVPack::decode_from(v, start_ind);
}

// ----------------=========ooooOOOOOOOOOoooo=========----------------
void SignOffPacket::encode_to(vector<uint8_t> & v, size_t start_ind)
{
 TLVPack::encode_to(v, start_ind);
}

// ----------------=========ooooOOOOOOOOOoooo=========----------------
BaseMsg::BaseMsg(void)
{
}

// ----------------=========ooooOOOOOOOOOoooo=========----------------
BaseMsg::~BaseMsg()
{
}

// ----------------=========ooooOOOOOOOOOoooo=========----------------
bool BaseMsg::decode_header(vector<uint8_t> & v, size_t start_ind)
{
 if (v.size()<(start_ind+11)) return false;
 memcpy(msg_cookie, &v[start_ind], 8);
 start_ind+=8;
 memcpy(&msg_channel, &v[start_ind], sizeof(msg_channel));
 msg_channel=ntohs(msg_channel);
 start_ind+=sizeof(msg_channel);
 uint8_t uin_length = v[start_ind++];
 if (v.size()<(start_ind+uin_length)) return false;
 uin.assign(v.begin()+start_ind, v.begin()+start_ind+uin_length);
 
 return true;
}

// ----------------=========ooooOOOOOOOOOoooo=========----------------
void BaseMsg::encode_header(vector<uint8_t> & v, size_t start_ind)
{
 if (v.size()<(start_ind+11+uin.length())) v.resize(start_ind+11+uin.length());
 memcpy(&v[start_ind], msg_cookie, 8);
 start_ind+=8;
 uint16_t tmp_msg_channel = htons(msg_channel);
 memcpy(&v[start_ind], &tmp_msg_channel, sizeof(tmp_msg_channel));
 start_ind+=sizeof(tmp_msg_channel);
 v[start_ind++]=uin.length();
 memcpy(&v[start_ind], uin.data(), uin.length());
}

// ----------------=========ooooOOOOOOOOOoooo=========----------------
bool BaseMsg::decode_data(vector<uint8_t> & v, size_t start_ind)
{
 data.clear();
 while(true)
  {
  if (start_ind>=v.size()) break;
  TLVField tmp_tlv;
  if (!tmp_tlv.decode_from(v, start_ind)) return false;
  data.push_back(tmp_tlv);
  start_ind+=tmp_tlv.data.size()+4;
  }
 return true;
}

// ----------------=========ooooOOOOOOOOOoooo=========----------------
void BaseMsg::encode_data(vector<uint8_t> & v, size_t start_ind)
{
 for (unsigned int i=0; i<data.size(); ++i)
  {
  data[i].encode_to(v, start_ind);
  start_ind+=data[i].data.size()+4; // +2 for TLVtype +2 for TLV length
  }
}

// ----------------=========ooooOOOOOOOOOoooo=========----------------
BaseOutgoingMsg::BaseOutgoingMsg(void) : BaseMsg()
{
}

// ----------------=========ooooOOOOOOOOOoooo=========----------------
BaseOutgoingMsg::~BaseOutgoingMsg()
{
}

// ----------------=========ooooOOOOOOOOOoooo=========----------------
void BaseOutgoingMsg::encode_to(vector<uint8_t> & v, size_t start_ind)
{
 encode_header(v, start_ind);
 start_ind+=11+uin.length();
 encode_data(v, start_ind);
}

// ----------------=========ooooOOOOOOOOOoooo=========----------------
BaseIncomingMsg::BaseIncomingMsg(void) : BaseMsg()
{
}

// ----------------=========ooooOOOOOOOOOoooo=========----------------
BaseIncomingMsg::~BaseIncomingMsg()
{
}

// ----------------=========ooooOOOOOOOOOoooo=========----------------
bool BaseIncomingMsg::decode_from(vector<uint8_t> & v, size_t start_ind)
{
 if (v.size()<(start_ind+15)) return false;
 if (!decode_header(v, start_ind)) return false;

 start_ind+=11+uin.length();
 start_ind+=2; // uint16_t warn_lev
 uint16_t fix_tlvs_num;
 memcpy(&fix_tlvs_num, &v[start_ind], sizeof(fix_tlvs_num));
 fix_tlvs_num=ntohs(fix_tlvs_num);
 start_ind+=sizeof(fix_tlvs_num);
 for (int i=0; i<fix_tlvs_num; ++i)
  {
  TLVField tmp_tlv;
  if (!tmp_tlv.decode_from(v, start_ind)) return false;
  start_ind+=tmp_tlv.data.size()+4;
  }

 if (!decode_data(v, start_ind)) return false;
 
 return true;
}

// ----------------=========ooooOOOOOOOOOoooo=========----------------
SimpleMsg::SimpleMsg(void)
          : charset(0x0003), sub_charset(0x0000)
            
{
}

// ----------------=========ooooOOOOOOOOOoooo=========----------------
SimpleMsg::~SimpleMsg()
{
}

// ----------------=========ooooOOOOOOOOOoooo=========----------------
bool SimpleMsg::decode_from(vector<TLVField> & tlv_v)
{
 for (unsigned int i=0; i<tlv_v.size(); ++i)
  if (tlv_v[i].type==0x02)
   {
   vector<uint8_t> & v = tlv_v[i].data;
   if (v.size()<12) return false;
   size_t start_ind=2; // skip frag_id & frag_ver
   uint16_t capa_len;
   memcpy(&capa_len, &v[start_ind], sizeof(capa_len));
   capa_len=ntohs(capa_len);
   if (v.size()<(unsigned int)(12+capa_len)) return false;
   start_ind+=sizeof(capa_len)+capa_len+2; // also skip uint8_t capa_arr[capa_len] & frag_id2 & frag_ver2
   uint16_t text_len;
   memcpy(&text_len, &v[start_ind], sizeof(text_len));
   text_len=ntohs(text_len)-4; // -4, text_len include charset len and sub_charset len
   if (v.size()<(unsigned int)(12+capa_len+text_len)) return false;
   start_ind+=sizeof(text_len);
   memcpy(&charset, &v[start_ind], sizeof(charset));
   charset=ntohs(charset);
   start_ind+=sizeof(charset);
   memcpy(&sub_charset, &v[start_ind], sizeof(sub_charset));
   sub_charset=ntohs(sub_charset);
   start_ind+=sizeof(sub_charset);
   message_str.assign(v.begin()+start_ind, v.begin()+start_ind+text_len);
   return true;
   }

 return false;
}

// ----------------=========ooooOOOOOOOOOoooo=========----------------
void SimpleMsg::encode_to(vector<TLVField> & tlv_v)
{
 vector<uint8_t> v(13+message_str.length());
 uint8_t start_buf[13]={0x05, 0x01, 0x00, 0x01, 0x01, 0x01, 0x01, 0x00, 0x1a, 0x00, 0x00, 0xff, 0xff}; // lazy hack
 memcpy(&v[0], start_buf, 13);
 uint16_t data_len = message_str.length()+4; // include charset len and sub_charset len
 data_len=htons(data_len);
 memcpy(&v[7], &data_len, sizeof(data_len));
 uint16_t tmp_charset=htons(charset);
 memcpy(&v[9], &tmp_charset, sizeof(tmp_charset));
 uint16_t tmp_sub_charset=htons(sub_charset);
 memcpy(&v[11], &tmp_sub_charset, sizeof(tmp_sub_charset));
 memcpy(&v[13], message_str.data(), message_str.length());
 
 TLVField tf1(v, 0x0002);
 TLVField tf2("", 0x0006);
 
 tlv_v.clear();
 tlv_v.push_back(tf2);
 tlv_v.push_back(tf1);
}

// ----------------=========ooooOOOOOOOOOoooo=========----------------
RendezVousMsg::RendezVousMsg(void)
{
}

// ----------------=========ooooOOOOOOOOOoooo=========----------------
RendezVousMsg::~RendezVousMsg()
{
}

// ----------------=========ooooOOOOOOOOOoooo=========----------------
bool RendezVousMsg::decode_from(vector<uint8_t> & data)
{
 size_t pos=0;
 if (data.size()<(pos+sizeof(type))) return false;
 memcpy(&type, &data[pos], sizeof(type));
 type=ntohs(type);
 pos+=sizeof(type);
 
 if (data.size()<(pos+8)) return false;
 memcpy(cookie, &data[pos], 8);
 pos+=8;
 
 if (data.size()<(pos+16)) return false;
 memcpy(capability, &data[pos], 16);
 pos+=16;
 
 TLVPack tp;
 if (!tp.decode_from(data, pos)) return false;
 
 TLVField * tlv = tp.findTLV(0x0004);
 if (tlv!=NULL)
  if (!tlv->getAsInt32(external_ip)) return false;

 tlv = tp.findTLV(0x0005);
 if (tlv!=NULL)
  if (!tlv->getAsInt16(external_port)) return false;
 
 tlv = tp.findTLV(0x2711);
 if (tlv!=NULL) 
  {
  if (!tlv2711.decode_from(tlv->data)) return false;
  }
 else
  {
  return false;
  }
  
 return true;
}

// ----------------=========ooooOOOOOOOOOoooo=========----------------
void RendezVousMsg::encode_to(vector<uint8_t> & data)
{
 vector<uint8_t> cap_spec_data;
 tlv2711.encode_to(cap_spec_data);
 
 data.resize(2+8+16);
 size_t pos=0;
 
 uint16_t tmp_type=htons(type);
 memcpy(&data[pos], &tmp_type, sizeof(tmp_type));
 pos+=sizeof(tmp_type);
 
 memcpy(&data[pos], cookie, 8);
 pos+=8;
 
 memcpy(&data[pos], capability, 16);
 pos+=16;
 
 TLVPack tp;
 tp.data.push_back(TLVField(external_ip, 0x0003));
 tp.data.push_back(TLVField(external_port, 0x0005));
 tp.data.push_back(TLVField((uint16_t)0x0001, 0x000A));
 // Dont insert TLV 0x0B - meaning unknown
 tp.data.push_back(TLVField(NULL, 0, 0x000F));
 tp.data.push_back(TLVField(cap_spec_data, 0x2711));
 
 tp.encode_to(data, pos);
}

// ----------------=========ooooOOOOOOOOOoooo=========----------------
TLV2711::TLV2711(void)
{
 memset(text_color, 0, 4);
 memset(bg_color, 0xff, 4);
 bg_color[3]=0;
}

// ----------------=========ooooOOOOOOOOOoooo=========----------------
TLV2711::~TLV2711()
{
}

// ----------------=========ooooOOOOOOOOOoooo=========----------------
bool TLV2711::decode_from(vector<uint8_t> & data, size_t pos)
{
#define UNPACK_LE(arg) \
 if (data.size()<(pos+sizeof(arg))) return false; \
 memcpy(&arg, &data[pos], sizeof(arg)); /* Little Endian */ \
 pos+=sizeof(arg);

 uint16_t w_id;
 UNPACK_LE(w_id)
 if (w_id!=0x001B) return false; // Static size for valid messages
 UNPACK_LE(version)
 
 uint8_t puid_message[16]={0};
 if (data.size()<(pos+sizeof(puid_message))) return false;
 if (memcmp(&data[pos], puid_message, sizeof(puid_message))!=0) return false; // It's not a message
 pos+=sizeof(puid_message);
 
 pos+=11; // Skip unused bytes
 
 UNPACK_LE(cookie)

 pos+=12; // Zeroes chunk
 
 if (data.size()<(pos+2)) return false;
 msg_type=data[pos++];
 flags=data[pos++];
 
 UNPACK_LE(status)
 UNPACK_LE(priority)

 uint16_t msg_len;
 UNPACK_LE(msg_len)
 
 if (msg_type!=0x01 && msg_type!=0x1a && msg_type!=0xe8 && msg_type!=0xe9 && \
     msg_type!=0xea && msg_type!=0xeb && msg_type!=0xec) return false; // Plain message or Plugin message described by text string or AutoStatus requests

 if (msg_len<1) return false; // NTS can't be shorter then 1
 if (data.size()<(pos+msg_len)) return false;
 text.assign((const char *)&data[pos], msg_len-1);
 pos+=msg_len;
 
 if (msg_type==0x01) // Plain message optional data
  {
  if (data.size()<(pos+4)) return true;
  memcpy(text_color, &data[pos], 4);
  pos+=4;
  
  if (data.size()<(pos+4)) return true;
  memcpy(bg_color, &data[pos], 4);
  pos+=4;
  
  uint32_t text_guid_len;
  if (data.size()<(pos+sizeof(text_guid_len))) return true;
  memcpy(&text_guid_len, &data[pos], sizeof(text_guid_len));
  pos+=sizeof(text_guid_len);
  
  if (data.size()<(pos+text_guid_len)) return true;
  text_guid.assign((const char *)&data[pos], text_guid_len);
//  pos+=text_guid_len;
  }
 else if (msg_type==0x1a) // Plugin message described by text string
  {
  uint16_t plug_info_len;
  UNPACK_LE(plug_info_len)

  if (data.size()<(pos+16)) return false;
  memcpy(plug_guid, &data[pos], 16);
  pos+=16;
  
  uint8_t xtraz_script_guid[] = {0x3b, 0x60, 0xb3, 0xef, 0xd8, 0x2a, 0x6c, 0x45, 0xa4, 0xe0, 0x9c, 0x5a, 0x5e, 0x67, 0xe8, 0x65};
  if (memcmp(plug_guid, xtraz_script_guid, 16)!=0) return false; // I can't decode anything except xTraZ notifies
  
  UNPACK_LE(plug_func_id)
  if (plug_func_id!=0x08) return false; // Script notify
  uint32_t plug_name_len;
  UNPACK_LE(plug_name_len)
  if (data.size()<(pos+plug_name_len)) return false;
  plug_name.assign((const char *)&data[pos], plug_name_len);
  pos+=plug_name_len;
  
  pos+=15; // Skip unused bytes
  
  uint32_t plug_data_len;
  uint32_t plug_msg_len;      
  UNPACK_LE(plug_data_len)
  UNPACK_LE(plug_msg_len)

  if (data.size()<(pos+plug_msg_len)) return false;
  xtraz_script_text.assign((const char *)&data[pos], plug_msg_len);
//  pos+=plug_msg_len;
  }
  
#undef UNPACK_LE
 return true;
}

// ----------------=========ooooOOOOOOOOOoooo=========----------------
bool TLV2711::parseXtrazRequest(void)
{
  if (xtraz_script_text.find("<QUERY>")==xtraz_script_text.npos) return false;
  if (xtraz_script_text.find("</QUERY>")==xtraz_script_text.npos) return false;
  if (xtraz_script_text.find("<NOTIFY>")==xtraz_script_text.npos) return false;
  if (xtraz_script_text.find("</NOTIFY>")==xtraz_script_text.npos) return false;

  string::size_type start_i = xtraz_script_text.find("&lt;PluginID&gt;");
  if (start_i==xtraz_script_text.npos) return false;
  start_i+=16;
  string::size_type end_i = xtraz_script_text.find("&lt;/PluginID&gt;", start_i);
  if (end_i==xtraz_script_text.npos) return false;
  string str_plug_id=string(xtraz_script_text, start_i, end_i-start_i);
  if (str_plug_id!="srvMng") return false;

  start_i = xtraz_script_text.find("<NOTIFY>&lt;srv&gt;&lt;id&gt;", end_i+17);
  if (start_i==xtraz_script_text.npos) return false;
  start_i+=29;
  end_i = xtraz_script_text.find("&lt;/id&gt;", start_i);
  if (end_i==xtraz_script_text.npos) return false;
  string str_serv_id=string(xtraz_script_text, start_i, end_i-start_i);
  if (str_serv_id!="cAwaySrv") return false;

  start_i = xtraz_script_text.find("&lt;req&gt;&lt;id&gt;", end_i+11);
  if (start_i==xtraz_script_text.npos) return false;
  start_i+=21;
  end_i = xtraz_script_text.find("&lt;/id&gt;", start_i);
  if (end_i==xtraz_script_text.npos) return false;
  string str_req_id=string(xtraz_script_text, start_i, end_i-start_i);
  if (str_req_id!="AwayStat") return false;
  
  start_i = xtraz_script_text.find("&lt;senderId&gt;", end_i+11);
  if (start_i==xtraz_script_text.npos) return false;
  start_i+=16;
  end_i = xtraz_script_text.find("&lt;/senderId&gt;", start_i);
  if (end_i==xtraz_script_text.npos) return false;
  xstat_sender_id=string(xtraz_script_text, start_i, end_i-start_i);
  
  return true;
}

// ----------------=========ooooOOOOOOOOOoooo=========----------------
bool TLV2711::parseXtrazResponse(void)
{
  if (xtraz_script_text.find("<NR>")==xtraz_script_text.npos) return false;
  if (xtraz_script_text.find("</NR>")==xtraz_script_text.npos) return false;
  if (xtraz_script_text.find("<RES>")==xtraz_script_text.npos) return false;
  if (xtraz_script_text.find("</RES>")==xtraz_script_text.npos) return false;

  string::size_type start_i = xtraz_script_text.find("&lt;uin&gt;");
  if (start_i==xtraz_script_text.npos) return false;
  start_i+=11;
  string::size_type end_i = xtraz_script_text.find("&lt;/uin&gt;", start_i);
  if (end_i==xtraz_script_text.npos) return false;
  xstat_sender_id=string(xtraz_script_text, start_i, end_i-start_i);

  start_i = xtraz_script_text.find("&lt;index&gt;", end_i+12);
  if (start_i==xtraz_script_text.npos) return false;
  start_i+=13;
  end_i = xtraz_script_text.find("&lt;/index&gt;", start_i);
  if (end_i==xtraz_script_text.npos) return false;
  string str_ind=string(xtraz_script_text, start_i, end_i-start_i);
  x_status=atoi(str_ind.c_str());

  start_i = xtraz_script_text.find("&lt;title&gt;", end_i+14);
  if (start_i==xtraz_script_text.npos) return false;
  start_i+=13;
  end_i = xtraz_script_text.find("&lt;/title&gt;", start_i);
  if (end_i==xtraz_script_text.npos) return false;
  x_title=string(xtraz_script_text, start_i, end_i-start_i);
  
  start_i = xtraz_script_text.find("&lt;desc&gt;", end_i+14);
  if (start_i==xtraz_script_text.npos) return false;
  start_i+=12;
  end_i = xtraz_script_text.find("&lt;/desc&gt;", start_i);
  if (end_i==xtraz_script_text.npos) return false;
  x_descr=string(xtraz_script_text, start_i, end_i-start_i);
  
  return true;
}

// ----------------=========ooooOOOOOOOOOoooo=========----------------
void TLV2711::encode_to(vector<uint8_t> & data, size_t pos)
{
#define PACK_LE(arg) \
 if (data.size()<(pos+sizeof(arg))) data.resize(pos+sizeof(arg)); \
 memcpy(&data[pos], &arg, sizeof(arg)); /* Little Endian */ \
 pos+=sizeof(arg);

 uint16_t w_id=0x001B;
 PACK_LE(w_id)
 PACK_LE(version)

 data.resize(pos+16);
 memset(&data[pos], 0, 16); // Plugin - none
 pos+=16;
 
 data.resize(pos+25);
 memset(&data[pos], 0, 25);
 data[pos+2]=3; // client capabilities flags - LE
 data[pos+6]=4; // Unknown
 data[pos+9]=14; // Length2 of ex.header - LE
 pos+=25;
 
 PACK_LE(msg_type)
 PACK_LE(flags)
 PACK_LE(status)
 PACK_LE(priority)
 
 uint16_t msg_len=text.length()+1; // NTS
 PACK_LE(msg_len)
 
 data.resize(pos+msg_len);
 memcpy(&data[pos], text.c_str(), msg_len); // NTS
 pos+=msg_len;
 
 if (msg_type==0x01)
  {
  if (data.size()<(pos+4)) data.resize(pos+4);
  memcpy(&data[pos], text_color, 4);
  pos+=4;

  if (data.size()<(pos+4)) data.resize(pos+4);
  memcpy(&data[pos], bg_color, 4);
  pos+=4;
  
  if (text_guid.empty()) return;
  
  uint32_t text_guid_len = text_guid.length();
  PACK_LE(text_guid_len)

  if (data.size()<(pos+text_guid_len)) data.resize(pos+text_guid_len);
  memcpy(&data[pos], text_guid.data(), text_guid_len);
//  pos+=text_guid_len;
  
  return; // Plain text, encoding finished
  }
 else if (msg_type==0x1a) // Plugin message described by text string
  {  
  uint16_t plug_info_len=16+2+4+plug_name.length()+15;
  PACK_LE(plug_info_len)
 
  data.resize(pos+16);
  memcpy(&data[pos], plug_guid, 16);
  pos+=16;

  PACK_LE(plug_func_id)
 
  uint32_t plug_name_len=plug_name.length();
  PACK_LE(plug_name_len);
 
  data.resize(pos+plug_name_len);
  memcpy(&data[pos], plug_name.data(), plug_name_len);
  pos+=plug_name_len;
 
  data.resize(pos+15);
  memset(&data[pos], 0, 15);
  data[pos+2]=1; // Mean unknown
  pos+=15;
 
  uint32_t plug_msg_len=xtraz_script_text.length();
  uint32_t plug_data_len=plug_msg_len+sizeof(plug_msg_len);
 
  PACK_LE(plug_data_len)
  PACK_LE(plug_msg_len)
 
  data.resize(pos+plug_msg_len);
  memcpy(&data[pos], xtraz_script_text.data(), plug_msg_len);
//  pos+=plug_msg_len;
  }
#undef PACK_LE
}

// ----------------=========ooooOOOOOOOOOoooo=========----------------
ExtendedMsg::ExtendedMsg(void)
{
}

// ----------------=========ooooOOOOOOOOOoooo=========----------------
ExtendedMsg::~ExtendedMsg()
{
}

// ----------------=========ooooOOOOOOOOOoooo=========----------------
bool ExtendedMsg::decode_from(vector<TLVField> & tlv_v)
{
 TLVPack tp;
 tp.data=tlv_v;
 
 user_class=0;
 TLVField * tlv = tp.findTLV(0x0001);
 if (tlv!=NULL)
  if (!tlv->getAsInt16(user_class)) return false;
 
 user_status=0;
 tlv = tp.findTLV(0x0006);
 if (tlv!=NULL)
  if (!tlv->getAsInt32(user_status)) return false;
 
 online_time=0;
 tlv = tp.findTLV(0x000f);
 if (tlv!=NULL)
  if (!tlv->getAsInt32(online_time)) return false;
 
 create_time=0;
 tlv = tp.findTLV(0x0003);
 if (tlv!=NULL)
  if (!tlv->getAsInt32(create_time)) return false;
 
 tlv = tp.findTLV(0x0005);
 if (tlv!=NULL)
  {
  if (!rv_msg.decode_from(tlv->data)) return false;
  }
 else
  {
  return false;
  }
  
 return true;
}

// ----------------=========ooooOOOOOOOOOoooo=========----------------
void ExtendedMsg::encode_to(vector<TLVField> & tlv_v)
{
 tlv_v.clear();
 vector<uint8_t> rv_data;
 rv_msg.encode_to(rv_data);
 tlv_v.push_back(TLVField(rv_data, 0x0005));
}

// ----------------=========ooooOOOOOOOOOoooo=========----------------
OldStyleMsg::OldStyleMsg(void)
{
 msg_type=0x01; // MTYPE_PLAIN
 msg_flags=0x01; // MFLAG_NORMAL
}

// ----------------=========ooooOOOOOOOOOoooo=========----------------
OldStyleMsg::~OldStyleMsg()
{
}

// ----------------=========ooooOOOOOOOOOoooo=========----------------
bool OldStyleMsg::decode_from(vector<TLVField> & tlv_v)
{
 for (unsigned int i=0; i<tlv_v.size(); ++i)
  if (tlv_v[i].type==0x05)
   {
   if (tlv_v[i].data.size()<9) return false;
   memcpy(&sender_uin, &tlv_v[i].data[0], sizeof(sender_uin)); // LITTLE ENDIAN!!!
   msg_type=tlv_v[i].data[4];
   msg_flags=tlv_v[i].data[5];
   uint16_t text_len;
   memcpy(&text_len, &tlv_v[i].data[6], sizeof(text_len)); // LITTLE ENDIAN!!!
   if (text_len<1) return false;
   if (tlv_v[i].data.size()<(unsigned int)(8+text_len)) return false;
   message_str.assign(reinterpret_cast<const char *>(&tlv_v[i].data[8]), text_len-1);
   return true;
   }
 
 return false;
}

// ----------------=========ooooOOOOOOOOOoooo=========----------------
void OldStyleMsg::encode_to(vector<TLVField> & tlv_v)
{
 TLVField tlv;
 tlv.type=0x0005;
 tlv.data.resize(8+message_str.length()+1);
 memcpy(&tlv.data[0], &sender_uin, sizeof(sender_uin)); // LITTLE ENDIAN!!!
 tlv.data[4]=msg_type;
 tlv.data[5]=msg_flags;
 uint16_t text_len=message_str.length()+1;
 memcpy(&tlv.data[6], &text_len, sizeof(text_len)); // LITTLE ENDIAN!!!
 memcpy(&tlv.data[8], message_str.c_str(), text_len);

 tlv_v.clear();
 tlv_v.push_back(tlv);
}

// ----------------=========ooooOOOOOOOOOoooo=========----------------
SSIContactListItem::SSIContactListItem(void)
{
}

// ----------------=========ooooOOOOOOOOOoooo=========----------------
SSIContactListItem::~SSIContactListItem()
{
}

// ----------------=========ooooOOOOOOOOOoooo=========----------------
bool SSIContactListItem::decode_from(vector<uint8_t> & v, size_t start_ind)
{
 if (v.size()<(start_ind+2)) return false;
 memcpy(&name_length, &v[start_ind], sizeof(name_length));
 name_length=ntohs(name_length);
 start_ind+=sizeof(name_length);
 
 if (v.size()<(start_ind+name_length)) return false;
 name.assign(reinterpret_cast<const char *>(&v[start_ind]), name_length);
 start_ind+=name.length();
 
 if (v.size()<(start_ind+2)) return false;
 memcpy(&group_id, &v[start_ind], sizeof(group_id));
 group_id=ntohs(group_id);
 start_ind+=sizeof(group_id);

 if (v.size()<(start_ind+2)) return false;
 memcpy(&item_id, &v[start_ind], sizeof(item_id));
 item_id=ntohs(item_id);
 start_ind+=sizeof(item_id);

 if (v.size()<(start_ind+2)) return false;
 memcpy(&type, &v[start_ind], sizeof(type));
 type=ntohs(type);
 start_ind+=sizeof(type);

 if (v.size()<(start_ind+2)) return false;
 memcpy(&payload_length, &v[start_ind], sizeof(payload_length));
 payload_length=ntohs(payload_length);
 start_ind+=sizeof(payload_length);
 
 if (v.size()<(start_ind+payload_length)) return false;
 
 size_t start_ind_fix = start_ind;
 payload.clear();
 while(true)
  {
  if (start_ind>(start_ind_fix+payload_length)) return false;
  if (start_ind==(start_ind_fix+payload_length)) break;
  TLVField tlv;
  if (!tlv.decode_from(v, start_ind)) return false;
  payload.push_back(tlv);
  start_ind+=tlv.data.size()+4; // +2 for TLVtype +2 for TLV length
  }
  
 return true;
}

// ----------------=========ooooOOOOOOOOOoooo=========----------------
void SSIContactListItem::encode_to(vector<uint8_t> & v, size_t start_ind)
{
 if (v.size()<(start_ind+2)) v.resize(start_ind+2);
 name_length=name.length();
 uint16_t tmp_name_length = htons(name_length);
 memcpy(&v[start_ind], &tmp_name_length, sizeof(tmp_name_length));
 start_ind += sizeof(tmp_name_length);

 if (v.size()<(start_ind+name_length)) v.resize(start_ind+name_length);
 memcpy(&v[start_ind], name.data(), name_length);
 start_ind += name_length;
 
 if (v.size()<(start_ind+2)) v.resize(start_ind+2);
 uint16_t tmp_group_id = htons(group_id);
 memcpy(&v[start_ind], &tmp_group_id, sizeof(tmp_group_id));
 start_ind += sizeof(tmp_group_id);
 
 if (v.size()<(start_ind+2)) v.resize(start_ind+2);
 uint16_t tmp_item_id = htons(item_id);
 memcpy(&v[start_ind], &tmp_item_id, sizeof(tmp_item_id));
 start_ind += sizeof(tmp_item_id);

 if (v.size()<(start_ind+2)) v.resize(start_ind+2);
 uint16_t tmp_type = htons(type);
 memcpy(&v[start_ind], &tmp_type, sizeof(tmp_type));
 start_ind += sizeof(tmp_type);
 
 payload_length=0;
 
 for (unsigned int i=0; i<payload.size(); ++i) 
  payload_length += payload[i].data.size()+4; // +2 for TLVtype +2 for TLV length

 if (v.size()<(start_ind+2)) v.resize(start_ind+2);
 uint16_t tmp_payload_length = htons(payload_length);
 memcpy(&v[start_ind], &tmp_payload_length, sizeof(tmp_payload_length));
 start_ind += sizeof(tmp_payload_length);
 
 for (unsigned int i=0; i<payload.size(); ++i) 
  {
  payload[i].encode_to(v, start_ind);
  start_ind += payload[i].data.size()+4; // +2 for TLVtype +2 for TLV length
  }
}

// ----------------=========ooooOOOOOOOOOoooo=========----------------
SSIContactList::SSIContactList(void)
{
}

// ----------------=========ooooOOOOOOOOOoooo=========----------------
SSIContactList::~SSIContactList()
{
}

// ----------------=========ooooOOOOOOOOOoooo=========----------------
bool SSIContactList::decode_from(vector<uint8_t> & v, size_t start_ind)
{
 if (v.size()<(start_ind+1)) return false;
  
 ssi_version=v[start_ind];
 start_ind++;
 
 if (v.size()<(start_ind+2)) return false;
 memcpy(&items_count, &v[start_ind], sizeof(items_count));
 items_count=ntohs(items_count);
 start_ind+=sizeof(items_count);
 
 for (int i=0; i<items_count; ++i)
  {
  SSIContactListItem clit;
  if (!clit.decode_from(v, start_ind)) return false;
  items.push_back(clit);
  start_ind += 2+clit.name_length+2+2+2+2+clit.payload_length;
  }

 if (v.size()<(start_ind+4)) return false;
 memcpy(&last_timestamp, &v[start_ind], sizeof(last_timestamp));
 last_timestamp=ntohl(last_timestamp);
 
 return true;
}

// ----------------=========ooooOOOOOOOOOoooo=========----------------
ICBMParameters::ICBMParameters(void)
{
}

// ----------------=========ooooOOOOOOOOOoooo=========----------------
ICBMParameters::~ICBMParameters()
{
}

// ----------------=========ooooOOOOOOOOOoooo=========----------------
bool ICBMParameters::decode_from(vector<uint8_t> & v, size_t start_ind)
{
 if (v.size()<(start_ind+16)) return false;
 
 memcpy(&channel, &v[start_ind], sizeof(channel));
 channel=ntohs(channel);
 start_ind+=sizeof(channel);

 memcpy(&msg_flags, &v[start_ind], sizeof(msg_flags));
 msg_flags=ntohl(msg_flags);
 start_ind+=sizeof(msg_flags);
 
 memcpy(&max_snac_size, &v[start_ind], sizeof(max_snac_size));
 max_snac_size=ntohs(max_snac_size);
 start_ind+=sizeof(max_snac_size);

 memcpy(&max_send_warn_lev, &v[start_ind], sizeof(max_send_warn_lev));
 max_send_warn_lev=ntohs(max_send_warn_lev);
 start_ind+=sizeof(max_send_warn_lev);

 memcpy(&max_recv_warn_lev, &v[start_ind], sizeof(max_recv_warn_lev));
 max_recv_warn_lev=ntohs(max_recv_warn_lev);
 start_ind+=sizeof(max_recv_warn_lev);

 memcpy(&min_msg_interval, &v[start_ind], sizeof(min_msg_interval));
 min_msg_interval=ntohs(min_msg_interval);
 
 return true;
}

// ----------------=========ooooOOOOOOOOOoooo=========----------------
void ICBMParameters::encode_to(vector<uint8_t> & v, size_t start_ind)
{
 if (v.size()<(start_ind+16)) v.resize(start_ind+16);
 
 uint16_t tmp_channel=htons(channel);
 memcpy(&v[start_ind], &tmp_channel, sizeof(tmp_channel));
 start_ind+=sizeof(tmp_channel);
 
 uint32_t tmp_msg_flags=htonl(msg_flags);
 memcpy(&v[start_ind], &tmp_msg_flags, sizeof(tmp_msg_flags));
 start_ind+=sizeof(tmp_msg_flags);
 
 uint16_t tmp_max_snac_size=htons(max_snac_size);
 memcpy(&v[start_ind], &tmp_max_snac_size, sizeof(tmp_max_snac_size));
 start_ind+=sizeof(tmp_max_snac_size);

 uint16_t tmp_max_send_warn_lev=htons(max_send_warn_lev);
 memcpy(&v[start_ind], &tmp_max_send_warn_lev, sizeof(tmp_max_send_warn_lev));
 start_ind+=sizeof(tmp_max_send_warn_lev);

 uint16_t tmp_max_recv_warn_lev=htons(max_recv_warn_lev);
 memcpy(&v[start_ind], &tmp_max_recv_warn_lev, sizeof(tmp_max_recv_warn_lev));
 start_ind+=sizeof(tmp_max_recv_warn_lev);

 uint16_t tmp_min_msg_interval=htons(min_msg_interval);
 memcpy(&v[start_ind], &tmp_min_msg_interval, sizeof(tmp_min_msg_interval));
 start_ind+=sizeof(tmp_min_msg_interval);
 
 v[start_ind++]=0x00;
 v[start_ind++]=0x00;
}

// ----------------=========ooooOOOOOOOOOoooo=========----------------
DownloadBuddyIconRequest::DownloadBuddyIconRequest(void)
{
 cmd=1;
 hash_size=16;
}

// ----------------=========ooooOOOOOOOOOoooo=========----------------
DownloadBuddyIconRequest::~DownloadBuddyIconRequest()
{
}

// ----------------=========ooooOOOOOOOOOoooo=========----------------
void DownloadBuddyIconRequest::encode_to(vector<uint8_t> & v, size_t start_ind)
{
 size_t vec_size=1+uin.length()+4+1+hash_size;
 if (v.size()<(start_ind+vec_size)) v.resize(start_ind+vec_size);
 
 v[start_ind]=uin.length();
 start_ind++;
 memcpy(&v[start_ind], uin.data(), uin.length());
 start_ind+=uin.length();
 
 v[start_ind]=cmd;
 start_ind++;
 
 uint16_t tmp_icon_id=htons(icon_id);
 memcpy(&v[start_ind], &tmp_icon_id, sizeof(tmp_icon_id));
 start_ind+=sizeof(tmp_icon_id);

 v[start_ind]=icon_flags;
 start_ind++;
 
 v[start_ind]=hash_size;
 start_ind++;

 memcpy(&v[start_ind], md5_hash, 16);
// start_ind+=16;
}

// ----------------=========ooooOOOOOOOOOoooo=========----------------
DownloadBuddyIconReply::DownloadBuddyIconReply(void)
{
}

// ----------------=========ooooOOOOOOOOOoooo=========----------------
DownloadBuddyIconReply::~DownloadBuddyIconReply()
{
}

// ----------------=========ooooOOOOOOOOOoooo=========----------------
bool DownloadBuddyIconReply::decode_from(vector<uint8_t> & v, size_t start_ind)
{
 if (v.size()<(start_ind+1)) return false;
 uint8_t uin_len = v[start_ind];
 start_ind++;

 if (v.size()<(start_ind+uin_len)) return false;
 uin.assign(v.begin()+start_ind, v.begin()+start_ind+uin_len);
 start_ind+=uin_len;
 
 if (v.size()<(start_ind+2)) return false;
 memcpy(&icon_id, &v[start_ind], 2);
 icon_id=ntohs(icon_id);
 start_ind+=2;
 
 if (v.size()<(start_ind+1)) return false;
 icon_flags=v[start_ind];
 start_ind++;
 
 if (v.size()<(start_ind+1)) return false;
 uint8_t hash_size=v[start_ind];
 start_ind++;
 
 if (hash_size!=16) return false;
 
 if (v.size()<(start_ind+16)) return false;
 memcpy(md5_hash, &v[start_ind], 16);
 start_ind+=16;

 if (v.size()<(start_ind+1)) return false;
 cmd=v[start_ind];
 start_ind++;
 
 // Just skip fields which aren't needed for us
 start_ind+=20;
 
 uint16_t icon_size;
 if (v.size()<(start_ind+sizeof(icon_size))) return false;
 memcpy(&icon_size, &v[start_ind], sizeof(icon_size));
 icon_size=ntohs(icon_size);
 start_ind+=sizeof(icon_size);

 if (v.size()<(start_ind+icon_size)) return false;
 icon_data.assign(v.begin()+start_ind, v.begin()+start_ind+icon_size);
 
 return true;
}
