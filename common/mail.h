// Hey, Emacs, this a -*-C++-*- file !

// Copyright distributed.net 1997-1998 - All Rights Reserved
// For use in distributed.net projects only.
// Any other distribution or use of this source violates copyright.
// 
// $Log: mail.h,v $
// Revision 1.8  1998/08/10 20:29:36  cyruspatel
// Call to gethostname() is now a call to Network::GetHostName(). Updated
// send routine to reflect new NetworkInitialize()/NetworkDeinitialize()
// requirements. Removed all references to NO!NETWORK.
//
// Revision 1.7  1998/07/07 21:55:44  cyruspatel
// client.h has been split into client.h baseincs.h
//
// Revision 1.6  1998/06/14 08:12:59  friedbait
// 'Log' keywords added to maintain automatic change history
//
// 

#ifndef MAIL_H
#define MAIL_H

#if (INTSIZES == 422) //#if (CLIENT_OS == OS_WIN16)
  #define MAILBUFFSIZE 32000
  #define MAXMAILSIZE 31000
#else
  #define MAILBUFFSIZE 150000
  #define MAXMAILSIZE 125000
#endif

class MailMessage
{
public:
  u32 messagelen;
  int port;
  char fromid[255];
  char smtp[255];
  char destid[255];
  char my_hostname[255];
  char rc5id[255];
  s32 quietmode;

protected:
  char messagetext[MAILBUFFSIZE];
  int timeoffset;

public:
  MailMessage(void);
  ~MailMessage(void);
  int inittext(int x);
  void checktosend( u32 forcesend );
  void addtomessage(char *txt );

  int sendmessage(void);
  //sendmessage returns 0 if success, <0 if send error, >0 no network (defer)

  
  int prepare_smtp_message(Network * net);
  int send_smtp_edit_data (Network * net);
  static int get_smtp_line(Network * net);
  static int put_smtp_line(const char * line, unsigned int nchars , Network * net);
  static void smtp_error (Network * net, const char * message);
  static int finish_smtp_message(Network * net);
  int transform_and_send_edit_data(Network * net);
};

#endif

