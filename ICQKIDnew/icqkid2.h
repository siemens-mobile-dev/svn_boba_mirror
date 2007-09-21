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

// Internal version - ALPHA 5.2

#ifndef _ICQ_KID_2_H_
#define _ICQ_KID_2_H_

#include "common.h"

#include <string>
#include <vector>
#include <map>
#include <set>
using namespace std;

#include <time.h>

#ifdef _WIN32
 typedef char int8_t;
 typedef short int int16_t;
 typedef long int int32_t;
 
 typedef unsigned char uint8_t;
 typedef unsigned short int uint16_t;
 typedef unsigned long int uint32_t;
#else
 #include <sys/types.h>
#endif

#include "icqkid2_constants.h"

class SSIUINEntry{
 public:
  SSIUINEntry(void)
   {
   groupid=0;
   itemid=0;
   waitauth=false;
   last_internal_ip=0;
   last_internal_port=0;
   last_external_ip=0;
   have_icon=false;
   icon_id=0;
   icon_flags=0;
   memset(icon_md5_hash, 0, 16);
   online_status=STATUS_OFFLINE;
   status_modifiers=0;
   invisible=false;
   xStatus=X_STATUS_NONE;
   isBLM=false;
   srv_relay_cap=false;
   unicode_cap=false;
   online_since=0;
   idle_since=0;
   }

  string uin;
  string nick;
  uint16_t groupid;
  string groupname;
  uint16_t itemid;
  bool waitauth;

  uint32_t last_internal_ip;
  uint32_t last_internal_port;
  uint32_t last_external_ip;

  bool have_icon;
  uint16_t icon_id;
  uint8_t icon_flags;
  uint8_t icon_md5_hash[16];
  vector<uint8_t> icon_data;

  uint32_t online_status;
  uint32_t status_modifiers;
  bool invisible;

  size_t xStatus;
  string xStatusTitle;
  string xStatusDescription;

  bool isBLM;

  bool srv_relay_cap;
  bool unicode_cap;

  string AutoAwayMessageText;
  string AutoBusyMessageText;
  string AutoNotAvailableMessageText;
  string AutoDoNotDisturbMessageText;
  string AutoFreeForChatMessageText;

  uint32_t online_since;
  uint32_t idle_since;
  uint32_t proto;
};

class SSIGroupEntry{
 public:
  uint16_t id;
  string name;
};

class ICQKidShortUserInfo{
 public:
  string Uin;
  string Nickname;
  string Firstname;
  string Lastname;
  string Email;
  int Gender;
  bool Needauth;
  uint32_t Status;
  uint16_t Age;
};

class ICQKidFullUserInfo{
 public:
  ICQKidFullUserInfo(void) : Homecountry(0), GMToffset(0), Auth(0), Webaware(0), DCpermissions(0), Pubprimarymail(0), \
         Age(0), Gender(0), Birthyear(0), Birthmonth(0), Birthday(0), Language1(0), Language2(0), Language3(0), Origcountry(0), \
         Maritalstatus(0), Allowspam(0), Codepage(0), Homepageenabled(0), Homepagecategory(0), Workcountry(0), Workoccupation(0), \
         Interest1category(0), Interest2category(0), Interest3category(0), Interest4category(0), \
         Past1category(0), Past2category(0), Past3category(0), \
         Affiliation1category(0), Affiliation2category(0), Affiliation3category(0), \
         Minage(0), Maxage(0), Searchonline(0) {}

  // Basic info
  string Uin;
  string Nickname;
  string Firstname;
  string Lastname;
  string Email;
  string Homecity;
  string Homestate;
  string Homephone;
  string Homefax;
  string Homeaddress;
  string Cellphone;
  string Homezip;
  uint16_t Homecountry;
  int8_t GMToffset; // Signed! Real value = GMT + (-1*GMToffset/2)
  uint8_t Auth;
  uint8_t Webaware;
  uint8_t DCpermissions;
  uint8_t Pubprimarymail;
  
  // More info
  uint16_t Age;
  uint8_t Gender;
  string Homepage;
  uint16_t Birthyear;
  uint8_t Birthmonth;
  uint8_t Birthday;
  uint8_t Language1;
  uint8_t Language2;
  uint8_t Language3;
  string Origcity;
  string Origstate;
  uint16_t Origcountry;
  uint8_t Maritalstatus;
  uint8_t Allowspam;
  uint16_t Codepage;
  
  // Email info
  vector<pair<string, uint8_t> > Emails; // email and public flag
  
  // Homepage category Info
  uint8_t Homepageenabled;
  uint16_t Homepagecategory;
  string Homepagekeywords;
  
  // Work info
  string Workcity;
  string Workstate;
  string Workphone;
  string Workfax;
  string Workaddress;
  string Workzip;
  uint16_t Workcountry;
  string Workcompany;
  string Workdepartment;
  string Workposition;
  uint16_t Workoccupation;
  string Workwebpage;
  
  // Notes info
  string Notes;
  
  // Interests info
  uint16_t Interest1category;
  string Interest1;
  uint16_t Interest2category;
  string Interest2;
  uint16_t Interest3category;
  string Interest3;
  uint16_t Interest4category;
  string Interest4;
  
  // Affiliations info
  uint16_t Past1category;
  string Past1;
  uint16_t Past2category;
  string Past2;
  uint16_t Past3category;
  string Past3;
  uint16_t Affiliation1category;
  string Affiliation1;
  uint16_t Affiliation2category;
  string Affiliation2;
  uint16_t Affiliation3category;
  string Affiliation3;
  
  // Additional for white pages search
  uint16_t Minage;
  uint16_t Maxage;
  string Searchkey;
  uint8_t Searchonline;
};

class SnacCache;
class SNACData;

class ICQKid2{
 public:
  typedef enum
   {
   NONE,
   HTTP,
   SOCKS4,
   SOCKS5
   } ProxyType;
               
  ICQKid2(void);
  virtual ~ICQKid2();
  
  void setUIN(string auin) { myuin=auin; }
  void setPassword(string apwd) { mypassword=apwd; }
  void setLoginHost(string ahost="login.oscar.aol.com", int aport=5190) { loginhost=ahost; loginport=aport; }
  void setProxy(string ahost="", int aport=0, string auid="", string apwd="", ProxyType apr_type=NONE)
   {
   proxy_host=ahost;
   proxy_port=aport;
   proxy_uid=auid;
   proxy_pwd=apwd;
   proxy_type=apr_type;
   }
  
  bool doConnect(uint32_t astat=STATUS_ONLINE);
  void doDisconnect(void);
  bool setStatus(uint32_t astat);
  uint32_t getMyOnlineStatus(void) const { return online_status; }
  bool setXStatus(size_t x_stat, string title, string descr);
  
  bool registerNewUIN(string password, string & new_uin);
  bool changePassword(string new_password);
  
  bool getUserInfo(string uin, ICQKidShortUserInfo & info, bool force_update=false);
  bool getFullUserInfo(string uin, ICQKidFullUserInfo & info, bool force_update=false);  
  bool publicMyInfo(ICQKidFullUserInfo & info);
  
  bool searchByMail(string email, vector<ICQKidShortUserInfo> & result_vec);
  bool searchByUIN(string uin, vector<ICQKidShortUserInfo> & result_vec);
  bool searchByWhitePages(ICQKidFullUserInfo & info, vector<ICQKidShortUserInfo> & result_vec);
  
  class SSITransactContact {
   public:
    string uin;
    string nick;
    string groupname;
    uint16_t retflag;
  };
  
  bool addContact(string uin, string nick, string groupname, uint16_t * retflag=NULL);
  bool removeContact(string uin, uint16_t * retflag=NULL);
  bool renameContact(string uin, string nick, uint16_t * retflag=NULL);

  bool addContacts(vector<SSITransactContact> & cont_vec);
  bool removeContacts(vector<SSITransactContact> & cont_vec);
  bool renameContacts(vector<SSITransactContact> & cont_vec);
  
  bool addBLMContact(string uin);
  bool removeBLMContact(string uin);

  bool addBLMContacts(vector<string> & uins);
  bool removeBLMContacts(vector<string> & uins);

  bool addGroup(string groupname, uint16_t * retflag=NULL);
  bool removeGroup(string groupname, uint16_t * retflag=NULL);
  bool renameGroup(string groupname, string newname, uint16_t * retflag=NULL);
  
  bool updateContactList(void);

  bool authRequest(string uin, string text);
  bool authReply(string uin, string text, uint8_t aflag);
  
  bool getBuddyIcon(string uin);
  bool downloadMyIcon(void);
  bool uploadMyIcon(vector<uint8_t> & icon_data);
  bool deleteMyIcon(void) { return removeAllMyIcons(); }  
  
  bool addVisible(string uin, string nick="") { return addVisInvisIgnor(VisibleList, 0x0002, uin, nick); }
  bool removeVisible(string uin, string nick="") { return removeVisInvisIgnor(VisibleList, 0x0002, uin); }

  bool addInvisible(string uin, string nick="") { return addVisInvisIgnor(InvisibleList, 0x0003, uin, nick); }
  bool removeInvisible(string uin, string nick="") { return removeVisInvisIgnor(InvisibleList, 0x0003, uin); }

//  inline bool addIgnore(string uin, string nick="") { if (!removeContact(uin)) return false; return addVisInvisIgnor(IgnoreList, 0x000E, uin, nick); }
  bool addIgnore(string uin, string nick="") { return addVisInvisIgnor(IgnoreList, 0x000E, uin, nick); }
  bool removeIgnore(string uin, string nick="") { return removeVisInvisIgnor(IgnoreList, 0x000E, uin); }

  uint8_t getMyPrivacyStatus(void) const { return myPrivSrvStatus; }
  bool setMyPrivacyStatus(uint8_t stat);

  class ICQKid2Message {
   public:
    typedef enum
     {
     USASCII,
     LOCAL8BIT,
     UCS2BE,
     UTF8
     } EncodeType;
    
    ICQKid2Message(void)
     {
     enc_type=LOCAL8BIT;
     codepage=0;
     memset(text_color, 0, 4);
     memset(bg_color, 0xff, 4);
     bg_color[3]=0;
     is_offline=false;
     }
     
    ICQKid2Message(string auin, string atext, \
                          EncodeType aenc_type=LOCAL8BIT, uint16_t acodepage=0, \
                          bool ais_offline=false, \
                          uint8_t * atext_color=NULL, uint8_t * abg_color=NULL)
     {
     uin=auin;
     text=atext;
     enc_type=aenc_type;
     codepage=acodepage;
     if (atext_color) memcpy(text_color, atext_color, 4);
     else memset(text_color, 0, 4);
     if (abg_color) memcpy(bg_color, abg_color, 4);
     else
      {
      memset(bg_color, 0xff, 4);
      bg_color[3]=0;
      }
     is_offline=ais_offline;
     }

    uint8_t cookie[8];
    bool cookie_valid;

    string uin; // from or to
    string text;
    uint8_t text_color[4]; //RGBN
    uint8_t bg_color[4]; //RGBN
    EncodeType enc_type;
    uint16_t codepage;
    bool is_offline;
    time_t timestamp;
  };

  bool sendMessage(ICQKid2Message amsg, unsigned short cookie);
  bool sendMTN(string to, uint16_t atype);
  bool mainLoop(bool & break_flag);
  bool sendMsgAutoResponse(string touin, uint8_t * msg_cookie, uint8_t type=MSG_TYPE_PLAINTEXT);

  virtual void onIdle(void) {}
  virtual void onIncomingMTN(string from, uint16_t atype) {}
  virtual void onIncomingMsg(ICQKid2Message msg) {}
  virtual void onAuthRequest(string from, string text) {}
  virtual void onAuthReply(string from, string text, uint8_t aflag) {}
  virtual void onUserNotify(string uin, uint32_t stat1, uint32_t stat2, bool invis_flag, unsigned int xs) {}
  virtual void onContactListChanged(void) {}
  virtual void onWasAdded(string from) {}
  virtual void onIconChanged(string uin) {}
  virtual void onXstatusChanged(string uin, size_t x_status, string x_title, string x_descr) {}
  virtual void onRegisterControlPicture(vector<uint8_t> & pic_data, string mime_type, string & pic_str) {}
  virtual void onIncomingAutoStatusMsg(ICQKid2Message msg, uint8_t type) {}
  virtual void onSingOff(uint16_t err_code, string err_url) {}
  virtual void onClientMsgAck(string uin, uint8_t *cookie) {}
  virtual void onServerMsgAck(string uin, uint8_t *cookie) {}

  int findCLUIN(string uin);
  int findCLUIN(uint16_t item_id);
  int findCLGroup(string groupname);
  int findCLGroup(uint16_t group_id);
  
  vector<SSIUINEntry> ContactListUins;
  vector<SSIGroupEntry> ContactListGroups;
  vector<SSIUINEntry> ContactListIcons;

  vector<SSIUINEntry> VisibleList;
  vector<SSIUINEntry> InvisibleList;
  vector<SSIUINEntry> IgnoreList;

  string AutoAwayMessageText;
  string AutoBusyMessageText;
  string AutoNotAvailableMessageText;
  string AutoDoNotDisturbMessageText;
  string AutoFreeForChatMessageText;
  
  int getConnectPercentage(void) const { return connect_phase_percentage; }
  void setNetworkTimeout(int tm) { network_timeout=tm; }
  int getNetworkTimeout(void) const { return network_timeout; }
  void breakNetworkOperation(void) { network_break_flag=true; }

  bool getOfflineMessages(void);
  int pollIncomingEvents(int tmout);

  uint16_t connect_error_code;
  string connect_error_url;

  void sendXtrazRequest(string uin);

  size_t xStatus;
  string xStatusTitle;
  string xStatusDescription;

  int connect_phase_percentage;

 private:
  string proxy_host;
  int proxy_port;
  string proxy_uid;
  string proxy_pwd;
  ProxyType proxy_type;

  string myuin;
  string mypassword;
  string loginhost;
  int loginport;
  
  int sock;
  
  uint16_t flap_seq_number;
  uint32_t snac_seq_number;
  
  vector<pair<uint16_t, uint16_t> > server_services_list;
  vector<uint16_t> server_rate_groups;

  map<string, ICQKidShortUserInfo> userinfo_map;
  map<string, ICQKidFullUserInfo> userfullinfo_map;
  
  ICQKid2 * icons_service;
  
  int directConnect(string ahost, int aport);

  ICQKid2 * getInstanceForService(uint16_t family);
  
  bool doConnect_phase2(uint32_t astat, string boss_host, int boss_port, vector<uint8_t> & boss_cookie, bool short_phase=false);
  
  bool waitHello(void);
  bool sendSignOn0(void);
  bool sendMD5saltrequest(uint32_t * snac_sync);
  bool getMD5salt(uint32_t snac_sync, vector<uint8_t> & md5_salt);
  bool sendMD5authorize(uint32_t * snac_sync, vector<uint8_t> & md5_salt);
  bool getBOSSparams(uint32_t snac_sync, string & boss_host, int & boss_port, vector<uint8_t> & cookie);  
  bool sendSignOff(void);
  bool sendSignOn2(vector<uint8_t> & cookie);

  bool getServiceList(void);
  bool askServices(void);
  bool getServices(void);
  
  bool askRateLimits(uint32_t * sync_id);
  bool getRateLimits(uint32_t sync_id);
  bool sendRateLimits(void);
  
  bool askSelfInfo(uint32_t * sync_id);
  bool getSelfInfo(uint32_t sync_id);

  bool askLocationLimit(uint32_t * sync_id);
  bool getLocationLimit(uint32_t sync_id);
  bool sendLocationInfo(void);

  bool askBLMLimit(uint32_t * sync_id);
  bool getBLMLimit(uint32_t sync_id);

  bool askICBMParams(uint32_t * sync_id);
  bool getICBMParams(uint32_t sync_id);
  bool sendICBMParams(void);

  bool askPRMLimit(uint32_t * sync_id);
  bool getPRMLimit(uint32_t sync_id);

  bool askSSILimit(uint32_t * sync_id);
  bool getSSILimit(uint32_t sync_id);
  bool askSSICopy(uint32_t * sync_id);
  bool getSSICopy(uint32_t sync_id);
  bool activateSSI(void);
  
  bool sendStatus(uint32_t astat=STATUS_ONLINE, bool ext=false);
  bool sendReady(void);

  bool sendAuthResponse(string uin, string text, uint8_t aflag);
  bool sendAuthRequest(string uin, string text);
  
  bool askOfflMsgAction(uint8_t flag, uint32_t * sync_id);
  bool getOfflMsg(vector<ICQKid2Message> & offl_vec, uint32_t sync_id);
  
  bool askShortInfo(string uin, uint32_t * sync_id);
  bool getShortInfo(ICQKidShortUserInfo & info, uint32_t sync_id);
  
  bool askFullInfo(string uin, uint32_t * sync_id);
  bool getFullInfo(ICQKidFullUserInfo & info, uint32_t sync_id);
  
  void parseBasicUserInfo(vector<uint8_t> & data, ICQKidFullUserInfo & info);
  void parseMoreUserInfo(vector<uint8_t> & data, ICQKidFullUserInfo & info);
  void parseEmailUserInfo(vector<uint8_t> & data, ICQKidFullUserInfo & info);
  void parseHpagecatUserInfo(vector<uint8_t> & data, ICQKidFullUserInfo & info);
  void parseWorkUserInfo(vector<uint8_t> & data, ICQKidFullUserInfo & info);
  void parseNotesUserInfo(vector<uint8_t> & data, ICQKidFullUserInfo & info);
  void parseInterestsUserInfo(vector<uint8_t> & data, ICQKidFullUserInfo & info);
  void parseAffiliationsUserInfo(vector<uint8_t> & data, ICQKidFullUserInfo & info);
  
  bool getSearchAnswer(vector<ICQKidShortUserInfo> & result_vec, uint32_t snac_sync);
  
  bool parseIncomingMsg(ICQKid2Message & msg, vector<uint8_t> & data);
  bool parseIncomingMTN(string & from, uint16_t & type, vector<uint8_t> & data);
  bool parseAuthRequest(string & from, string & text, vector<uint8_t> & data);
  bool parseAuthReply(string & from, string & text, uint8_t & aflag, vector<uint8_t> & data);
  bool parseOnlineNotify(string & uin, uint32_t & stat, vector<uint8_t> & data, unsigned int &xs);
  bool parseOfflineNotify(string & uin, vector<uint8_t> & data);
  bool parseSSIAddNotify(vector<uint8_t> & data);
  bool parseSSIUpdateNotify(vector<uint8_t> & data);
  bool parseSSIDeleteNotify(vector<uint8_t> & data);
  bool parseWasAdded(string & from, vector<uint8_t> & data);
  bool parseXtrazResponse(string & from, size_t & x_status, string & x_title, string & x_descr, vector<uint8_t> & data);
  bool parseMsgAutoResponse(ICQKid2Message & msg, uint8_t & type, vector<uint8_t> & data);
  bool parseMsgServerResponse(vector<uint8_t> & data);

  bool sengMsgAutoRequest(string touin, uint8_t type);
  
  uint16_t getUnusedItemID(void);
  uint16_t getUnusedGroupID(void);
  bool startSSITransact(void);
  bool endSSITransact(void);
  bool getSSIEditAck(uint16_t & retflag, uint32_t sync_id);
  bool addDeleSSIUIN(SSIUINEntry uen, uint16_t flag, uint32_t * sync_id);
  bool addDeleSSIGroup(SSIGroupEntry gen, uint16_t flag, uint32_t * sync_id);
  bool updateSSIGroupContent(uint16_t group_id, uint32_t * sync_id);
  
  bool removeAllMyIcons(void);
  
  bool sub_ssi_addContact(string uin, string nick, string groupname, uint16_t * retflag);
  bool sub_ssi_removeContact(string uin, uint16_t * retflag);
  bool sub_ssi_renameContact(string uin, string nick, uint16_t * retflag);  

  bool sub_registerNewUIN_simply(ICQKid2 * regist_serv, string password, string & new_uin, string * control_str=NULL);
  bool sub_registerNewUIN_picture(ICQKid2 * regist_serv, string password, string & new_uin);
  
  bool sub_getBuddyIcon(SSIUINEntry & uen);
  
  bool haveSrvRelayCapability(vector<uint8_t> & data);
  bool haveUnicodeCapability(vector<uint8_t> & data);
  bool haveXtrazCapability(vector<uint8_t> & data, unsigned int & xs);
//  void sendXtrazRequest(string uin);
  void sendXStatusNotifyAutoResponse(string touin, uint8_t * msg_cookie);
  
  bool addVisInvisIgnor(vector<SSIUINEntry> & cont, uint16_t it_type, string uin, string nick="");
  bool removeVisInvisIgnor(vector<SSIUINEntry> & cont, uint16_t it_type, string uin);
  
  SnacCache * snac_cache;
  set<uint16_t> unknown_item_ids;
  
  int waitSNAC(SNACData * snd);
  int sendSNAC(uint16_t service_id, uint16_t subtype_id, uint32_t * req_id=NULL, vector<uint8_t> * data_ptr=NULL);
  int sendKeepAlive(void);
  
  int network_timeout;
  bool network_break_flag;

  time_t last_keepalive_timestamp;
  
  uint32_t online_status;
  
  uint8_t myPrivSrvStatus;
  uint16_t myPrivSrvStatus_item_id;
};

#endif
