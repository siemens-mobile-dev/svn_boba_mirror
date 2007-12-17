
#include "include.h"
#include "../inc/xtask_ipc.h"
#include "daemon.h"

char sieget_ipc_name[] = SIEGET_IPC_NAME;
char xtask_ipc_name[]= IPC_XTASK_NAME;

void SieGetDaemon::onCreate()
{
  SetName(DAEMON_CSM_NAME);

  tipc.name_to = sieget_ipc_name;
  tipc.name_from = sieget_ipc_name;
  tipc.data = &tint_ipc;
  tint_ipc.code = IPC_CODE_DOUBLEREQ;
  tint_ipc.csm_id = csm_id;
  GBS_SendMessage(MMI_CEPID, MSG_IPC, IPC_SIEGET_INTERNAL, &tipc);

  dialog->Create();
}

void MessageToHelper(SieGetDaemon *daemon, GBS_MSG *msg)
{
  daemon->onMessageHelper(msg);
  delete msg;
}

int SieGetDaemon::onMessage(GBS_MSG *msg)
{
  if (msg->msg==MSG_IPC )
  {
    IPC_REQ *ipc;
    if (ipc = (IPC_REQ *)msg->data0)
      // Обработка IPC сообщений
      if (!strcmp(ipc->name_to, SIEGET_IPC_NAME))
      {
        //Наше сообщение
        ProcessIPC(ipc->name_from, msg->submess, ipc->data);
      }
  }
  if (msg->msg==MSG_HELPER_TRANSLATOR)
  {
    GBS_MSG *tmp_msg = new GBS_MSG;
    *tmp_msg = *msg;
    SUBPROC((void *)MessageToHelper, this, tmp_msg);
  }
  return CSM_MSG_RESULT_CONTINUE;
}

void SieGetDaemon::onMessageHelper(GBS_MSG *msg) // HELPER
{
  if ((int)msg->data0==ENIP_DNR_HOST_BY_NAME)
      ProcessDNR((int)msg->data1);
    else
    {
      ProcessSocket((int)msg->data1, (int)msg->data0);
    }
}

void SieGetDaemon::ProcessIPC(const char *from, int submsg, void *data)
{
  if (submsg==IPC_SIEGET_INTERNAL)
  {
    SIEGET_INTERNAL_IPC *int_ipc;
    if (int_ipc = (SIEGET_INTERNAL_IPC *)data)
      switch(int_ipc->code)
      {
      case IPC_CODE_DOUBLEREQ:
        if (int_ipc->csm_id!=csm_id) // Не надо отвечать на свой запрос, а то от него другие дохнут
        {
          tipc.name_to = sieget_ipc_name;
          tipc.name_from = sieget_ipc_name;
          tipc.data = &tint_ipc;
          tint_ipc.code = IPC_CODE_DOUBLERESP;
          tint_ipc.csm_id = csm_id;
          GBS_SendMessage(MMI_CEPID, MSG_IPC, IPC_SIEGET_INTERNAL, &tipc);

          if (!dialog->csm_id)
            dialog->Create();
          else
          {
            tipc2.name_to = xtask_ipc_name;
            tipc2.name_from = sieget_ipc_name;
            tipc2.data = (void *)dialog->csm_id;
            GBS_SendMessage(MMI_CEPID, MSG_IPC, IPC_XTASK_SHOW_CSM, &tipc2);
          }
        }
        break;
      case IPC_CODE_DOUBLERESP:
        if (int_ipc->csm_id!=csm_id) // Есть кто-то кроме нас
        {
          Close();
        }
      }
  }
}

void SieGetDaemon::ProcessSocket(int sock, int event)
{
  Log::Active->PrintLn("Got socket event");
}

void SieGetDaemon::ProcessDNR(int DNR_ID)
{
  Log::Active->PrintLn("Got DNR event");
}

void SieGetDaemon::onClose()
{
  dialog->Close();
  delete this; // суицид
}


SieGetDaemon::SieGetDaemon()
{
  log = new Log();
  dialog = new SieGetDialog();
  log->PrintLn("Daemon created!");
}

extern "C"
{
  extern void kill_data(void *p, void (*func_p)(void *));
}

void Killer(void){
  extern void *ELF_BEGIN;
  kill_data(&ELF_BEGIN,(void (*)(void *))mfree_adr());
  //  ((void (*)(void *))(mfree_adr()))(&ELF_BEGIN);
}

SieGetDaemon::~SieGetDaemon()
{
  delete dialog;
  delete log;
  SUBPROC((void *)Killer);
}

