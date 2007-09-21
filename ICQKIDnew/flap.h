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

#ifndef _FLAP_H_
#define _FLAP_H_

#include <vector>
#include <string>

#include "common.h"

using namespace std;

// Available frame types
#define FT_SIGNON     1
#define FT_DATA       2
#define FT_ERROR      3
#define FT_SIGNOFF    4
#define FT_KEEP_ALIVE 5

#define TLV_UID            0x01
#define TLV_PWD            0x02
#define TLV_CLI_STR_ID     0x03
#define TLV_BOSS_ADDR      0x05
#define TLV_AUTH_COOKIE    0x06
#define TLV_CLI_NUM_DISTR  0x14
#define TLV_CLI_NUM_ID     0x16
#define TLV_CLI_VER_MAJOR  0x17
#define TLV_CLI_VER_MINOR  0x18
#define TLV_CLI_VER_LESSER 0x19
#define TLV_CLI_NUM_BUILD  0x1a
#define TLV_CLI_LANG       0x0f
#define TLV_CLI_COUNTRY    0x0e

class FlapPacket{
 public:
  FlapPacket(void);
  ~FlapPacket();
  
  bool recv_from(int sock);
  bool send_to(int sock);

  uint8_t frame_type;
  uint16_t seq_number;
  
  vector<uint8_t> payload;
  
  int t_network_error;
};

class SNACData{
 public:
  SNACData(void);
  ~SNACData();
 
  bool decode_from(vector<uint8_t> & v, size_t start_ind=0);
  void encode_to(vector<uint8_t> & v, size_t start_ind=0);

  uint16_t service_id;
  uint16_t subtype_id;
  uint16_t flags;
  uint32_t req_id;
  
  vector<uint8_t> data;
};

class TLVField{
 public:
  TLVField(void);
  TLVField(uint8_t i, uint16_t t) { data.resize(1); data[0]=i; type=t; }
  TLVField(uint16_t i, uint16_t t) { putInt16(i); type=t; }
  TLVField(uint32_t i, uint16_t t) { putInt32(i); type=t; }
  TLVField(string s, uint16_t t) { putString(s); type=t; }
  TLVField(vector<uint8_t> & v, uint16_t t) { data=v; type=t; }
  TLVField(const uint8_t * p, size_t p_len, uint16_t t) { if (p_len>0 && p!=NULL) data.assign(p, p+p_len); else data.clear(); type=t; }
  ~TLVField();
  
  bool getAsInt16(uint16_t & i);
  void putInt16(uint16_t i);
  
  bool getAsInt32(uint32_t & i);
  void putInt32(uint32_t i);
  
  bool getAsString(string & s);
  void putString(string s);
  
  bool decode_from(vector<uint8_t> & v, size_t start_ind);
  void encode_to(vector<uint8_t> & v, size_t start_ind);
  
  // For simplify can set type manually, without accessors
  uint16_t type;
  vector<uint8_t> data;
};

class TLVPack{
 public:
  TLVPack(void);
  virtual ~TLVPack();
 
  virtual bool decode_from(vector<uint8_t> & v, size_t start_ind=0);
  virtual void encode_to(vector<uint8_t> & v, size_t start_ind=0);
  
  TLVField * findTLV(uint16_t t);
 
  vector<TLVField> data;
};

class SignOnPacket : public TLVPack {
 public:
  SignOnPacket(void);
  virtual ~SignOnPacket();
  
  virtual bool decode_from(vector<uint8_t> & v, size_t start_ind=0);
  virtual void encode_to(vector<uint8_t> & v, size_t start_ind=0);
};

class SignOffPacket : public TLVPack {
 public:
  SignOffPacket(void);
  virtual ~SignOffPacket();
  
  virtual bool decode_from(vector<uint8_t> & v, size_t start_ind=0);
  virtual void encode_to(vector<uint8_t> & v, size_t start_ind=0);
};

class BaseMsg{
 public:
  BaseMsg(void);
  virtual ~BaseMsg();
  
  uint8_t msg_cookie[8];
  uint16_t msg_channel;
  string uin;
  vector<TLVField> data;
  
 protected:
  bool decode_header(vector<uint8_t> & v, size_t start_ind=0);
  void encode_header(vector<uint8_t> & v, size_t start_ind=0);
  bool decode_data(vector<uint8_t> & v, size_t start_ind);
  void encode_data(vector<uint8_t> & v, size_t start_ind);  
};

class BaseOutgoingMsg : public BaseMsg{
 public:
  BaseOutgoingMsg(void);
  virtual ~BaseOutgoingMsg();
  
  void encode_to(vector<uint8_t> & v, size_t start_ind=0);
};

class BaseIncomingMsg : public BaseMsg{
 public:
  BaseIncomingMsg(void);
  virtual ~BaseIncomingMsg();
  
  bool decode_from(vector<uint8_t> & v, size_t start_ind=0);
};

class SimpleMsg{
 public:
  SimpleMsg(void);
  ~SimpleMsg();
  
  bool decode_from(vector<TLVField> & tlv_v);
  void encode_to(vector<TLVField> & tlv_v);
  
  uint16_t charset;
  uint16_t sub_charset;
  string message_str;
};

class TLV2711{
 public:
  TLV2711(void);
  ~TLV2711();
  
  bool decode_from(vector<uint8_t> & data, size_t pos=0);
  void encode_to(vector<uint8_t> & data, size_t pos=0);
  
  bool parseXtrazRequest(void);
  bool parseXtrazResponse(void);
  
  uint16_t version;
  uint16_t cookie;
  uint8_t msg_type;
  uint8_t flags;
  uint16_t status;
  uint16_t priority;
  
  string text;
  
  uint8_t text_color[4]; //RGBN
  uint8_t bg_color[4]; //RGBN
  string text_guid;
  
  uint8_t plug_guid[16];
  uint16_t plug_func_id;
  string plug_name;
  string xtraz_script_text;  
  string xstat_sender_id;
  size_t x_status;
  string x_title;
  string x_descr;
};

class RendezVousMsg{
 public:
  RendezVousMsg(void);
  ~RendezVousMsg();
  
  bool decode_from(vector<uint8_t> & data);
  void encode_to(vector<uint8_t> & data);
  
  uint16_t type; // 0 - request, 1 - cancel, 2 - accept
  uint8_t cookie[8];
  uint8_t capability[16];
  uint32_t external_ip; // In host format
  uint16_t external_port;
  TLV2711 tlv2711;
};


class ExtendedMsg{
 public:
  ExtendedMsg(void);
  ~ExtendedMsg();
 
  bool decode_from(vector<TLVField> & tlv_v);
  void encode_to(vector<TLVField> & tlv_v);
 
  uint16_t user_class;
  uint32_t user_status;
  uint32_t online_time;
  uint32_t create_time;
  
  RendezVousMsg rv_msg;
};

class OldStyleMsg{
 public:
  OldStyleMsg(void);
  ~OldStyleMsg();
  
  bool decode_from(vector<TLVField> & tlv_v);
  void encode_to(vector<TLVField> & tlv_v);
  
  uint32_t sender_uin;
  uint8_t msg_type;
  uint8_t msg_flags;
  string message_str;
};

class SSIContactListItem{
 public:
  SSIContactListItem(void);
  ~SSIContactListItem();
  
  bool decode_from(vector<uint8_t> & v, size_t start_ind);
  void encode_to(vector<uint8_t> & v, size_t start_ind);
  
  uint16_t name_length;
  string name;
  uint16_t group_id;
  uint16_t item_id;
  uint16_t type;
  uint16_t payload_length;
  
  vector<TLVField> payload;
};

class SSIContactList{
 public:
  SSIContactList(void);
  ~SSIContactList();
  
  bool decode_from(vector<uint8_t> & v, size_t start_ind=0);
  
  uint8_t ssi_version;
  uint16_t items_count;
  uint32_t last_timestamp;
  
  vector<SSIContactListItem> items;
};

class ICBMParameters{
 public:
  ICBMParameters(void);
  ~ICBMParameters();
  
  bool decode_from(vector<uint8_t> & v, size_t start_ind=0);
  void encode_to(vector<uint8_t> & v, size_t start_ind=0);

  uint16_t channel;
  uint32_t msg_flags;
  uint16_t max_snac_size;
  uint16_t max_send_warn_lev;
  uint16_t max_recv_warn_lev;
  uint16_t min_msg_interval;
};

class DownloadBuddyIconRequest{
 public:
  DownloadBuddyIconRequest(void);
  ~DownloadBuddyIconRequest();

  void encode_to(vector<uint8_t> & v, size_t start_ind=0);
  
  string uin;
  uint8_t cmd;
  uint16_t icon_id;
  uint8_t icon_flags;
  uint8_t hash_size;
  uint8_t md5_hash[16];
};

class DownloadBuddyIconReply{
 public:
  DownloadBuddyIconReply(void);
  ~DownloadBuddyIconReply();

  bool decode_from(vector<uint8_t> & v, size_t start_ind=0);
  
  string uin;
  uint16_t icon_id;
  uint8_t icon_flags;
  uint8_t md5_hash[16];
  uint8_t cmd;
  vector<uint8_t> icon_data;
};

#endif
