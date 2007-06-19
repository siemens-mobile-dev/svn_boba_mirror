program IG;

uses
  Messages, ICQClient, SysUtils, Windows, ICQWorks, Classes, WinSock, {ExtCtrls, }SyncObjs;

{$R *.res}
//{$DEFINE DEBUG}

type
  TCB = class
    procedure myWakeMainThread(Sender: TObject);
    procedure MsgRecv(Sender: TObject; Msg, UIN: String);
    procedure OfflineMsgRecv(Sender: TObject; DateTime: TDateTime; Msg, UIN: String);
    procedure ClientLogin(Sender: TObject);
    procedure ClientLogOff(Sender: TObject);
    procedure ClientError(Sender: TObject; ErrorType: TErrorType; ErrorMsg: String);
    procedure ContactListRecv(Sender: TObject; SRVContactList: TList);
    procedure StatusChange(Sender: TObject; UIN: String; Status: LongWord; XStatus:TXStatuses);
    procedure OnlineInfo(Sender: TObject; UIN: String; Port: Word; InternalIP, ExternalIP: String; OnlineTime: TDateTime; Idle: Word; ICQVersion, MirandaVersion: LongWord; ProtoVer, UserCaps: Byte);
    procedure UserOffline(Sender: TObject; UIN: String);
    procedure AuthResponse(Sender: TObject; UIN: String; Granted: Boolean; Reason: String);
    procedure UserInfoShort(Sender: TObject; UIN, NickName, FirstName, LastName, Email: String; UserFound, AuthRequired: Boolean);
    procedure SSLChangeResponse(Sender: TObject; ErrorCode: Word);
    procedure AuthRequest(Sender: TObject; UIN, Reason: String);
    procedure MsgAck(Sender: TObject; UIN: String; ID: Word);
    procedure AdvancedMsgAck(Sender: TObject; UIN: String;
     ID: Word; AcceptType: Byte; AcceptMsg: String);
{    procedure TimerProc(Sender: TObject);}
  end;
  PKT = packed record
   uin: Longword;
   pkt_type: word;
   data_len: word;
   //Size 8
   data: array[0..16383] of byte;
  end;

  TSTCACHE = record
   uin: Longword;
   status: word;
   proto: word;
   xstatus: TXStatuses;
  end;

  TConn = class(TThread)
  private
   procedure ProcessPacket;
   procedure DOoffline;
    { Private declarations }
  public
    { Public declarations }
   procedure Execute; override;
  published
    constructor CreateIt;
    destructor Destroy; override;
  end;

const
  Statuses: array[0..12] of DWord =
  (
   S_OFFLINE,
   S_INVISIBLE,
   S_AWAY,
   S_NA,
   S_OCCUPIED,
   S_DND,
   S_DECLINE,
   S_EVIL,
   S_HOME,
   S_EAT,
   S_WORK,
   S_ONLINE,
   S_FFC
  );
  TIMEQUANT=30;
  TIMEOUT = 300*1000 div TIMEQUANT;
var
 Main_TID: DWORD;
 CB: TCB;
 ICQ: TICQClient;
// Tmr: TTimer;
 TerminateX: boolean;
 gid: integer;
 uids: array[0..8191] of integer;
 stcache: array [0..8191] of TSTCACHE;
 pk: PKT;
 PConn: TConn;
 LogLock: TCriticalSection;

{$IFDEF DEBUG}
   WSAData: TWSAData;
   listen_socket: THandle;
   server_socket: THandle;
{$ENDIF}
 ro:  OVERLAPPED;
 wo:  OVERLAPPED;
 HasThread: boolean;

procedure Log(s: string);
var
 f: textfile;
 ss: string;
begin
 LogLock.Acquire;
 try
 ss:='Logs\IG'+inttostr(ICQ.UIN)+'.LOG';
 AssignFile(f,ss);
 {$I-}
  Append(f);
  if (IOResult<>0) then Rewrite(f);
 {$I+}
 write(f,s+#13+#10);
 CloseFile(f);
 finally
  LogLock.Release;
 end;
end;

{function fr(var b; sz: dword): dword;
var
h: THandle;
begin
 h:=GetStdHandle(STD_INPUT_HANDLE);
// Form1.Label1.Caption:=inttostr(h);
 if not PeekNamedPipe(h,@b,sz,@result,nil,nil) then
 begin
  result:=0;
  Terminate:=true;
 end
 else
 begin
  if (result<>sz) then
  begin
   result:=0;
  end
  else
  begin
   readfile(h,b,sz,result,Nil);
  end;
 end;
end;}

function fw(var b; sz: integer): dword;
var
h: THandle;
p: PKT;
//{$IFDEF DEBUG}
 i: integer;
 f: boolean;
//{$ENDIF}
begin
{$IFDEF DEBUG}
 h:=server_socket;
{$ELSE}
 h:=GetStdHandle(STD_OUTPUT_HANDLE);
{$ENDIF}
// Form1.Label2.Caption:=inttostr(h);
// o.Offset:=0;
// o.OffsetHigh:=0;
// o.hEvent:=0;
// {$IFDEF DEBUG}
 f:=writefile(h,b,sz,result,@wo);
 if (not f) then
 begin
  i:=GetLastError;
  if (i=ERROR_IO_PENDING) then
  begin
   f:=GetOverlappedResult(h,wo,result,true);
   if (not f) then result:=0;
  end;
 end;
// {$ELSE}
// writefile(h,b,sz,result,nil);
// {$ENDIF}
 move(b,p,8);
 Log('>'+inttostr(p.uin)+' '+inttostr(p.pkt_type)+' '+inttostr(p.data_len));
end;

function find_stcache(uin: Longword): integer;
begin
 result:=0;
 repeat
  if (uin=stcache[result].uin) then exit;
  inc(result);
 until result=8192;
 result:=-1;
end;

function add_stcache(uin: Longword): integer;
begin
 result:=find_stcache(0);
 if (result<0) then exit;
 stcache[result].uin:=uin;
 stcache[result].status:=$FFFF;
 stcache[result].proto:=0;
 stcache[result].xstatus:=X_STATUS_NONE;
end;

procedure TCB.MsgRecv(Sender: TObject; Msg, UIN: String);
var
 tp: PKT;
 s: string;
begin
 tp.uin:=strtoint64(UIN);
 tp.pkt_type:=3;
 s:=Msg;
 tp.data_len:=Length(s);
 move(s[1],tp.data,tp.data_len);
 fw(tp,8+tp.data_len);
// if ((s[1]='E') and (s[2]='X') and(s[3]='I') and(s[4]='T') ) then ICQ.LogOff;
end;

procedure TCB.OfflineMsgRecv(Sender: TObject; DateTime: TDateTime; Msg, UIN: String);
var
 tp: PKT;
 s: string;
begin
 tp.uin:=strtoint64(UIN);
 tp.pkt_type:=3;
 s:='<'+DateTimeToStr(DateTime)+'>'+Msg;
 tp.data_len:=Length(s);
 move(s[1],tp.data,tp.data_len);
 fw(tp,8+tp.data_len);
end;

procedure TCB.ClientLogin(Sender: TObject);
var
 tp: PKT;
 s: string;
begin
 tp.uin:=ICQ.UIN;
 tp.pkt_type:=4;
 s:='Login';
 tp.data_len:=Length(s);
 move(s[1],tp.data,tp.data_len);
 fw(tp,8+tp.data_len);
 ICQ.RequestContactList;
end;

procedure TCB.ClientLogOff(Sender: TObject);
begin
 ICQ.OnConnectionFailed:=nil;
 TerminateX:=true;
end;

procedure TCB.ClientError(Sender: TObject; ErrorType: TErrorType; ErrorMsg: String);
var
 tp: PKT;
 s: string;
begin
 tp.uin:=ICQ.UIN;
 tp.pkt_type:=6;
 s:=ErrorMsg;
 Log('Error: '+ErrorMsg);
 tp.data_len:=Length(s);
 move(s[1],tp.data,tp.data_len);
 fw(tp,8+tp.data_len);
end;

procedure TCB.ContactListRecv(Sender: TObject; SRVContactList: TList);
var
 tp: PKT;
 s: string;
 ui: TUINENTRY;
 i: integer;
 gcache: integer;
begin
 i:=0;
 gcache:=0;
 while(i<SRVContactList.Count)do
 begin
  ui:=PUINENTRY(SRVContactList.Items[i])^;
  inc(i);
  if (ui.CType=BUDDY_GROUP) then
  begin
   if (ui.CGroupID<>0) then
   begin
    if (gid=-1) then
    begin
     gid:=ui.CGroupID;
    end;
    gcache:=ui.CGroupID;
    tp.uin:=ui.CGroupID;
    tp.pkt_type:=20;
    s:=ui.Nick;
    tp.data_len:=Length(s);
    move(s[1],tp.data,tp.data_len);
    fw(tp,8+tp.data_len);
   end;
   continue;
  end;
  if (ui.UIN=0) then continue;
  if (ui.CType=BUDDY_IGNORE) then continue;
  if (ui.CGroupID<>gcache) then
  begin
   gcache:=ui.CGroupID;
   tp.uin:=ui.CGroupID;
   tp.pkt_type:=21;
   s:='';
   tp.data_len:=Length(s);
   move(s[1],tp.data,tp.data_len);
   fw(tp,8+tp.data_len);
  end;
  ICQ.AddContact(ui.UIN);
  uids[ICQ.ContactList.IndexOf(inttostr(ui.UIN))]:=ui.CTag;
  tp.uin:=ui.UIN;
  tp.pkt_type:=7;
  s:={inttostr(ui.CTag)+' '+inttostr(ui.CType)+' '+}ui.Nick;
  tp.data_len:=Length(s);
  move(s[1],tp.data,tp.data_len);
  fw(tp,8+tp.data_len);
 end;
 tp.uin:=0;
 tp.pkt_type:=7;
 s:='';
 tp.data_len:=Length(s);
 move(s[1],tp.data,tp.data_len);
 fw(tp,8+tp.data_len);
 ICQ.DestroyUINList(SRVContactList);
 ICQ.RequestOfflineMessages;
end;

procedure TCB.StatusChange(Sender: TObject; UIN: String; Status: LongWord; XStatus:TXStatuses);
var
 tp: PKT;
 i: integer;
begin
 Status:=Status and $FFFF;
// if (Status<>$FFFF) then Status:=Status and $1FF;
// s:=ICQWorks.StatusToStr(Status);
 tp.uin:=strtoint(UIN);
 tp.pkt_type:=9;
 tp.data_len:=3;
 move(Status,tp.data,tp.data_len);
 tp.data[2]:=byte(XStatus);
 i:=find_stcache(tp.uin);
 if (i<0) then
 begin
  i:=add_stcache(tp.uin);
 end
 else
 begin
  if ((stcache[i].status=Status)and(stcache[i].xstatus=XStatus)) then exit;
 end;
 if (i>=0) then
 begin
  stcache[i].status:=Status;
  stcache[i].xstatus:=XStatus;
 end;
 fw(tp,8+tp.data_len);
end;

procedure TCB.OnlineInfo(Sender: TObject; UIN: String; Port: Word; InternalIP, ExternalIP: String; OnlineTime: TDateTime; Idle: Word; ICQVersion, MirandaVersion: LongWord; ProtoVer, UserCaps: Byte);
var
 i: integer;
 u: cardinal;
begin
 u:=strtoint(UIN);
 i:=find_stcache(u);
 if (i<0) then i:=add_stcache(u);
 if (i>=0) then stcache[i].proto:=ProtoVer;
end;

procedure TCB.UserOffline(Sender: TObject; UIN: String);
var
 tp: PKT;
 s: string;
 i: integer;
begin
 s:='Offline';
 tp.uin:=strtoint(UIN);
 tp.pkt_type:=9;
 tp.data_len:=3;
 tp.data[0]:=$FF;
 tp.data[1]:=$FF;
 tp.data[2]:=byte(X_STATUS_NONE);
 i:=find_stcache(tp.uin);
 if (i<0) then
 begin
  i:=add_stcache(tp.uin);
 end
 else
 begin
  stcache[i].proto:=0;
  if (stcache[i].status=$FFFF) then exit;
 end;
 if (i>=0) then
 begin
  stcache[i].status:=$FFFF;
  stcache[i].xstatus:=X_STATUS_NONE;
  stcache[i].proto:=0;
 end;
 fw(tp,8+tp.data_len);
end;

procedure TCB.AuthResponse(Sender: TObject; UIN: String; Granted: Boolean; Reason: String);
var
 tp: PKT;
 s: string;
begin
 if (Granted) then s:='Auth Resp OK!' else s:='Auth resp failed, reason: ';
 tp.uin:=strtoint64(UIN);
 tp.pkt_type:=3;
 if (not Granted) then s:=s+' '+Reason;
 tp.data_len:=Length(s);
 move(s[1],tp.data,tp.data_len);
 fw(tp,8+tp.data_len);
end;

procedure TCB.UserInfoShort(Sender: TObject; UIN, NickName, FirstName, LastName, Email: String; UserFound, AuthRequired: Boolean);
var
 tp: PKT;
 s: string;
begin
 s:='Nick: '+NickName+#13+'FisrtName: '+FirstName+#13+'LastName: '+LastName+
 #13+'EMail: '+EMail+#13;
 if (UserFound) then s:=s+'User found'+#13 else s:=s+'User not found'+#13;
 if (AuthRequired) then s:=s+'Auth req'+#13 else s:=s+'Auth not req'+#13;
 tp.uin:=strtoint64(UIN);
 tp.pkt_type:=3;
 tp.data_len:=Length(s);
 move(s[1],tp.data,tp.data_len);
 fw(tp,8+tp.data_len);
end;

procedure TCB.SSLChangeResponse(Sender: TObject; ErrorCode: Word);
var
 tp: PKT;
 s: string;
begin
 case ErrorCode of
 ERRSSL_NOTFOUND: s:='Not found';
 ERRSSL_EXISTS: s:='Allready exists';
 ERRSSL_AUTH: s:='Not authorized';
 ERRSSL_OTHER: s:='Other error';
 ERRSSL_NOERROR: s:='Complete';
 else s:='Unknown';
 end;
 tp.uin:=0;
 tp.pkt_type:=13;
 tp.data_len:=Length(s);
 move(s[1],tp.data,tp.data_len);
 fw(tp,8+tp.data_len);
end;

procedure TCB.AuthRequest(Sender: TObject; UIN, Reason: String);
var
 tp: PKT;
 s: string;
begin
 s:='Auth REQ: '+Reason;
 tp.uin:=strtoint64(UIN);
 tp.pkt_type:=3;
 tp.data_len:=Length(s);
 move(s[1],tp.data,tp.data_len);
 fw(tp,8+tp.data_len);
end;

procedure TCB.MsgAck(Sender: TObject; UIN: String; ID: Word);
var
 tp: PKT;
 s: string;
begin
 s:='';
 tp.uin:=strtoint64(UIN);
 tp.pkt_type:=16; //Server side ACK
 tp.data_len:=Length(s);
 move(s[1],tp.data,tp.data_len);
 fw(tp,8+tp.data_len);
end;

procedure TCB.AdvancedMsgAck(Sender: TObject; UIN: String;
     ID: Word; AcceptType: Byte; AcceptMsg: String);
var
 tp: PKT;
 s: string;
begin
 s:='';
 tp.uin:=strtoint64(UIN);
 tp.pkt_type:=17; //Client side ACK
 tp.data_len:=Length(s);
 move(s[1],tp.data,tp.data_len);
 fw(tp,8+tp.data_len);
end;

function IsContactInList(lst: TStrings; uin: string): boolean;
begin
 Result:=lst.IndexOf(uin)<>-1;
end;

procedure NewUserToClient(uin: integer; nick: string);
var
 tp: PKT;
 s: string;
begin
 tp.uin:=uin;
 tp.pkt_type:=7;
 s:=nick;
 tp.data_len:=Length(s);
 move(s[1],tp.data,tp.data_len);
 fw(tp,8+tp.data_len);
 tp.uin:=0;
 tp.pkt_type:=7;
 s:='';
 tp.data_len:=Length(s);
 move(s[1],tp.data,tp.data_len);
 fw(tp,8+tp.data_len);
end;

procedure doLogOff;
begin
 if ICQ.LoggedIn then ICQ.LogOff else TerminateX:=true;
end;

procedure TConn.ProcessPacket;
var
 sd: string;
 i: integer;
 pr: integer;
begin
{   if (Tmr.Tag<300) then Tmr.Tag:=0;}
   if (TerminateX) then exit;
   Log('Header: '+inttostr(pk.uin)+' '+inttostr(pk.pkt_type)+' '+inttostr(pk.data_len));
   if (pk.data_len>16383) then
   begin
    doLogOff;
    exit;
   end;
   //Пакет принят
   sd:='';
   i:=0;
   while(i<pk.data_len)do
   begin
    sd:=sd+chr(pk.data[i]);
    inc(i);
   end;
   if (pk.pkt_type=1) then
   begin
    if not ICQ.LoggedIn then
    begin
     ICQ.UIN:=pk.uin;
     ICQ.Password:=sd;
     ICQ.Login;
    end;
    exit;
   end;
   if (not ICQ.LoggedIn) then exit;
   if (pk.pkt_type=2) then
   begin
    if ICQ.LoggedIn then
    begin
     i:=find_stcache(pk.uin);
     if (i<0) then
     begin
      i:=add_stcache(pk.uin);
     end;
     if (i>=0) then
     begin
      pr:=stcache[i].proto;
      i:=stcache[i].status;
     end
     else
     begin
      pr:=0;
      i:=$FFFF;
     end;
     if ((i=$FFFF)or(pr<9)) then
      ICQ.SendMessage(pk.uin,sd)
     else
      ICQ.SendMessageAdvanced(pk.uin,sd,Random($FFFF),false);
    end;
   end;
   if (pk.pkt_type=10) then
   begin
    ICQ.SendAuthRequest(pk.uin,sd);
   end;
   if (pk.pkt_type=11) then
   begin
    ICQ.RequestInfoShort(pk.uin);
   end;
   if (pk.pkt_type=12) then
   begin
    if (IsContactInList(ICQ.ContactList,inttostr(pk.uin))) then
    begin
     ICQ.SSLChangeStart(True);
     ICQ.SSLAddUser(gid,uids[ICQ.ContactList.IndexOf(inttostr(pk.uin))],
      inttostr(pk.uin),sd,'',True,True);
     ICQ.SSLChangeEnd;
     NewUserToClient(pk.uin,sd);
    end
    else
    begin
     ICQ.SSLChangeStart(True);
     ICQ.SSLAddUser(gid,random(32766)+1,inttostr(pk.uin),sd,'',True,False);
     ICQ.SSLChangeEnd;
     ICQ.AddContact(pk.uin);
     NewUserToClient(pk.uin,sd);
    end;
   end;
   if (pk.pkt_type=14) then
   begin
    ICQ.SendAuthResponse(pk.uin,true,sd);
   end;
   // ADDED by Kibab: status changes
   if (pk.pkt_type=15) then
   begin
    if ICQ.LoggedIn then
    begin
     ICQ.Status:=Statuses[pk.data[0]];
    end;
   end;
   if (pk.pkt_type=18) then
   begin
    pk.pkt_type:=19;
    fw(pk,8+pk.data_len);
   end;
   if (pk.pkt_type=20) then
   begin
    ICQ.RemoveContact(pk.uin);
   end;
   if (pk.pkt_type=22) then
   begin
    if (ICQ.LoggedIn) and (pk.data[0]<=byte(High(TXStatuses))) then
    begin
     ICQ.XStatus:=TXStatuses(pk.data[0]);
    end;
   end;
end;

function xreadfile(h: THandle; var p; sz: integer): dword;
var
 f: boolean;
 i: dword;
begin
// ro.Offset:=0;
// ro.OffsetHigh:=0;
// ro.hEvent:=0;
 result:=0;
 f:=ReadFile(h,p,sz,result,@ro);
 if (not f) then
 begin
  i:=GetLastError;
  if (i=ERROR_IO_PENDING) then
  begin
   i:=WaitForSingleObject(ro.hEvent,5*60000);
   if (i<>WAIT_FAILED) then
   begin
    if (i=WAIT_OBJECT_0) then
    begin
     f:=GetOverlappedResult(h,ro,result,false);
     if not f then Log('xreaderror2: '+inttostr(GetLastError));
    end
    else Log('xreader wait: '+inttostr(i));
   end
   else Log('xreadererror3: '+inttostr(GetLastError));
  end else Log('xreaderror: '+inttostr(i));
 end;
end;

procedure TConn.Execute;
var
 h: THandle;
 trf: dword;
begin
{$IFDEF DEBUG}
 h:=server_socket;
{$ELSE}
 h:=GetStdHandle(STD_INPUT_HANDLE);
{$ENDIF}
 Repeat
  pk.uin:=0;
  pk.pkt_type:=0;
  pk.data_len:=0;
  trf:=xreadfile(h,pk,8);
  if (trf<>8) then
  begin
   break;
  end;
  if (pk.data_len<>0) then
  begin
    trf:=pk.data_len;
    if (pk.data_len<16384) then trf:=xreadfile(h,pk.data,pk.data_len);
    if (trf<>pk.data_len) then
    begin
     break;
    end;
  end;
  Synchronize(ProcessPacket);
 until TerminateX;
 Log('Exit thread, TerminateX='+ booltostr(TerminateX));
 if (not TerminateX) then Synchronize(DOoffline);
end;

procedure TConn.DOoffline;
begin
 doLogOff;
end;

constructor TConn.CreateIt;
begin
  inherited Create(true);      // Create thread suspended
  Priority := TThreadPriority(tpNormal); // Set Priority Level
  FreeOnTerminate := true; // Thread Free Itself when terminated
  HasThread:=true;
  Suspended := false;         // Continue the thread
end;

destructor TConn.Destroy;
begin
   HasThread:=false;
   inherited destroy;
end;

procedure InitCache;
var i: integer;
begin
  for i:=0 to 8191 do
  begin
   stcache[i].uin:=0;
   stcache[i].status:=$FFFF; //Offline
  end;
end;

{$IFDEF DEBUG}
procedure CreateSockets;
var
 server_addr, temp_addr: 	SOCKADDR_IN;
 accepted_len: integer;
 opt: boolean;
begin
  WSAStartup(MAKEWORD(1, 1), WSAData);
  listen_socket:= socket(AF_INET, SOCK_STREAM, 0);
  setsockopt(listen_socket, SOL_SOCKET, SO_REUSEADDR, PChar(@opt), sizeof(opt));
  server_addr.sin_family      := AF_INET;
  server_addr.sin_addr.s_addr := htonl(INADDR_ANY);
  server_addr.sin_port        := htons(strtoint(ParamStr(1)));
  bind(listen_socket, server_addr, sizeof(server_addr));
  listen(listen_socket, 1);
  accepted_len:=Sizeof(temp_addr);
  server_socket := accept(listen_socket, @temp_addr, @accepted_len);
end;
{$ENDIF}


function xProcessMessage(var Msg: TMsg): Boolean;
begin
  Result := False;
  if PeekMessage(Msg, 0, 0, 0, PM_REMOVE) then
  begin
    Result := True;
    if Msg.Message <> WM_QUIT then
    begin
      begin
        TranslateMessage(Msg);
        DispatchMessage(Msg);
      end;
    end
    else
      TerminateX := True;
  end;
end;

procedure MyProcessMessages;
var
  Msg: TMsg;
begin
  while xProcessMessage(Msg) do CheckSynchronize{loop};
end;

procedure TCB.myWakeMainThread(Sender: TObject);
begin
// PostMessage(Handle, WM_NULL, 0, 0);
 PostThreadMessage(Main_TID,WM_NULL,0,0);
end;

procedure MyHandleMessage;
var
  Msg: TMsg;
begin
  if not xProcessMessage(Msg) then
  begin
    if not CheckSynchronize then WaitMessage;//Idle(Msg);
  end;
end;

procedure ExitDelay;
var
 t: integer;
begin
  for t:=0 to 300 do
  begin
   MyProcessMessages;
   sleep(10);
  end;
end;

{procedure TCB.TimerProc(Sender: TObject);
begin
 TTimer(Sender).Tag:=TTimer(Sender).Tag+1;
 if (TTimer(Sender).Tag=300) then doLogOff;
end;}

begin
 Main_TID:=GetCurrentThreadId;
 HasThread:=false;
 LogLock:=TCriticalSection.Create;
 {$IFDEF DEBUG$}
 if (ParamStr(1)='') then exit;
 {$ENDIF}
 try
//  Application.Initialize;
//  Application.Run;
  TerminateX:=false;
  {$IFDEF DEBUG}
  CreateSockets;
  if (server_socket=INVALID_HANDLE_VALUE) then exit;
  {$ENDIF}
  gid:=-1;

  ro.hEvent:=CreateEvent(nil,false,false,nil);

  CB:=TCB.Create;
  WakeMainThread:=CB.myWakeMainThread;
  if (CB=nil) then
  begin
   Log('!!! Can''t create CB !!!');
   exit;
  end;

{  Tmr:=TTimer.Create(nil);
  Tmr.Interval:=1000;
  Tmr.OnTimer:=CB.TimerProc;
  Tmr.Tag:=0;
  Tmr.Enabled:=true;}

  ICQ:=TICQClient.Create{(nil)};

  if (ICQ=nil) then
  begin
   Log('!!! Can''t create ICQ !!!');
   exit;
  end;

  ICQ.DisableDirectConnections:=true;
  ICQ.ConvertToPlaintext:=true;
  ICQ.OnConnectionFailed:=CB.ClientLogOff;
  ICQ.OnMessageRecv:=CB.MsgRecv;
  ICQ.OnLogin:=CB.ClientLogin;
  ICQ.OnError:=CB.ClientError;
  ICQ.OnServerListRecv:=CB.ContactListRecv;
  ICQ.OnStatusChange:=CB.StatusChange;
  ICQ.OnOnlineInfo:=CB.OnlineInfo;
  ICQ.OnUserOffline:=CB.UserOffline;
  ICQ.OnOfflineMsgRecv:=CB.OfflineMsgRecv;
  ICQ.OnAuthResponse:=CB.AuthResponse;
  ICQ.OnUserInfoShort:=CB.UserInfoShort;
  ICQ.OnSSLChangeResponse:=CB.SSLChangeResponse;
  ICQ.OnAuthRequest:=CB.AuthRequest;
  ICQ.OnMsgAck:=CB.MsgAck;
  ICQ.OnAdvancedMsgAck:=CB.AdvancedMsgAck;
  //  ICQ.Login;
//  MainLoop;
  InitCache;
  PConn:=TConn.CreateIt;
  if (PConn=nil) then
  begin
   Log('!!! Can''t create TConn !!!');
   exit;
  end;
  repeat
   MyHandleMessage;
  until TerminateX;
  Log('SetEvent');
  if (ro.hEvent<>0) then SetEvent(ro.hEvent);
  Log('WaitFor');
  repeat sleep(100) until(not HasThread);
//  if (HasThread and (PConn<>nil)) then PConn.WaitFor;
  Log('CloseEvent');
  if (ro.hEvent<>0) then
  begin
    CloseHandle(ro.hEvent);
    ro.hEvent:=0;
  end;
  Log('ExitDelay');
  ExitDelay;
  {$IFDEF DEBUG}
  closesocket(server_socket);
  closesocket(listen_socket);
  {$ELSE}
  closesocket(GetStdHandle(STD_OUTPUT_HANDLE));
  {$ENDIF}
  Log('Complete!');
//  PConn.Destroy;
  except
  Log('!!!Exception!!!');
  if (HasThread) then
  begin
    if (ro.hEvent<>0) then SetEvent(ro.hEvent);
    TerminateX:=true;
  end;
  halt(20);
 end;
end.

