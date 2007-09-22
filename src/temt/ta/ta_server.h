// Copyright, 1995-2007, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of Emergent
//
//   Emergent is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   Emergent is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.

// ta_server.h -- tcp/ip server

#ifndef TA_SERVER_H
#define TA_SERVER_H

#include "ta_base.h"
#include "ta_TA_type.h"

#ifndef __MAKETA__
# include <QtNetwork/QAbstractSocket> // for state defines
#endif

// forwards
class TemtClient;
class TemtClientAdapter;
class TemtServer;
class TemtServerAdapter;

class TA_API TemtClientAdapter: public taBaseAdapter {
  // ##IGNORE QObject for attaching events/signals for its taBase owner
friend class TemtClient;
  Q_OBJECT
public:
  inline TemtClient*	owner() {return (TemtClient*)taBaseAdapter::owner;}
  TemtClientAdapter(TemtClient* owner_): taBaseAdapter((taOABase*)owner_) {}
  
#ifndef __MAKETA__ // maketa chokes on the net class types etc.
public slots:
  void 			sock_readyRead();
  void 			sock_disconnected(); //note: we only allow one for now, so monitor it here
  void 			sock_stateChanged(QAbstractSocket::SocketState socketState);
#endif
};

class TA_API TemtClient: public taOABase { 
  // #INSTANCE #TOKENS for tcp-based remote services -- represents one connected client 
INHERITED(taOABase)
public:
  bool			connected; // #READ_ONLY #SHOW #NO_SAVE true when the client is connected
  
  inline TemtClientAdapter* adapter() {return (TemtClientAdapter*)taOABase::adapter;} // #IGNORE
  TA_BASEFUNS(TemtClient);

#ifndef __MAKETA__ // maketa chokes on the net class types etc.
  TemtServer*		server; // (will never change) set on create; NOT refcnted
  
  void			CloseClient();
  void			SetSocket(QTcpSocket* sock);

public: // slot forwardees
  void 			sock_readyRead();
  void 			sock_disconnected(); 
  void 			sock_stateChanged(QAbstractSocket::SocketState socketState);

protected:
  QPointer<QTcpSocket>	sock; // #IGNORE the socket for the connected client
#endif
private:
  void	Copy_(const TemtClient& cp);
  void	Initialize();
  void 	Destroy();
};

class TA_API TemtClient_List: public taList<TemtClient> {
public:
  TA_BASEFUNS2_NOCOPY(TemtClient_List, taList<TemtClient>);

private:
  void	Initialize() {SetBaseType(&TA_TemtClient);}
  void 	Destroy() {}
};


class TA_API TemtServerAdapter: public taBaseAdapter {
  // ##IGNORE QObject for attaching events/signals for its taBase owner
friend class TemtServer;
  Q_OBJECT
public:
  inline TemtServer*	owner() {return (TemtServer*)taBaseAdapter::owner;}
  TemtServerAdapter(TemtServer* owner_): taBaseAdapter((taOABase*)owner_) {}
  
#ifndef __MAKETA__ // maketa chokes on the net class types etc.
public slots:
  void 			server_newConnection();
#endif
};

class TA_API TemtServer: public taOABase { 
  // #INSTANCE #TOKENS Temt Server, for tcp-based remote services 
INHERITED(taOABase)
public:
  unsigned short	port; // #DEF_5360 port number to use -- each instance must have unique port
  bool			open; // #NO_SAVE #SHOW #READ_ONLY set when server is open and accepting connections
  TemtClient_List	clients; // #SHOW #NO_SAVE #READ_ONLY how many clients are connected
  
  inline TemtServerAdapter* adapter() {return (TemtServerAdapter*)taOABase::adapter;} // #IGNORE

  bool			InitServer() // initializes the server
    {bool ok = true; InitServer_impl(ok); return ok;}
  
  bool			OpenServer(); // #BUTTON #GHOST_ON_open open the server and accept connections
  void			CloseServer(bool notify = true); // #BUTTON #GHOST_OFF_open #ARGC_0 stop the server and close open connections
  
// callbacks
  void			ClientDisconnected(TemtClient* client); // #IGNORE

  SIMPLE_LINKS(TemtServer);
  TA_BASEFUNS(TemtServer);

#ifndef __MAKETA__ // maketa chokes on the net class types etc.
public: // slot forwardees
  void 			server_newConnection(); //
#endif

protected:
  QTcpServer*		server; // #IGNORE
  
  TemtClient*		m_client; // #IGNORE unitary client
  
  virtual void		InitServer_impl(bool& ok); // impl routine, set ok=f for failure
private:
  void	Copy_(const TemtServer& cp); // copying not really supported...
  void	Initialize();
  void 	Destroy();
};

#endif
