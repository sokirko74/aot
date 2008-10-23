

//  include headers for processes and sockets
#ifdef WIN32
	#include <winsock2.h>
	#include <Ws2tcpip.h>
	#include <process.h>
#else
		#include <pthread.h>
		#include <arpa/inet.h>
		#include <sys/types.h>
		#include <sys/socket.h>
		#include <netinet/in.h>
		#include <netdb.h>
		#include <stdio.h>
		#define SOCKET int

#endif



//#ifndef HAVE_DECL_GETADDRINFO
//	# if !HAVE_DECL_GETADDRINFO
//	/* Translate name of a service location and/or a service name to set of
//		socket addresses.
//		For more details, see the POSIX:2001 specification */
//		extern void getaddrinfo ( const char* nodename,
//								  const char* servname,
//								  const struct addrinfo* hints,
//								  struct addrinfo **restrict res);
//	# endif
//# endif
// 
//# ifndef HAVE_DECL_FREEADDRINFO
//	# if !HAVE_DECL_FREEADDRINFO
// /* Free `addrinfo' structure AI including associated storage.
//    For more details, see the POSIX:2001 specification
//    <http://www.opengroup.org/susv3xsh/getaddrinfo.html>.  */
//		extern void freeaddrinfo (struct addrinfo *ai);
//	# endif
//# endif


#include "string_socket.h"
#include "../common/util_classes.h"
#include <errno.h>

#ifdef WIN32
	#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
	#endif
#endif





struct ERROR_STRUCT
{     
	int     errnum;     
	const char*   errstr; 
} ;  
const char msg_no_error[19] =      "No error occurred!"; 
const char msg_unknown_error[14] = "Unknown error";  
const string DDCCloseSocketString =  "ddc_close_socket";

const ERROR_STRUCT errlist[] = {     
	{EINTR,           "Socket Error! EINTR - Interrupted"},     
	{EBADF, "Socket Error! EBADF - Bad file number"},                
	{EFAULT,   "Socket Error! EFAULT - Bad address"},               
	{EINVAL,          "Socket Error! EINVAL - Invalid argument"},               
	{EMFILE,          "Socket Error! EMFILE - Too many open files"},            
	{EACCES,  "Socket Error! The address is protected, and the user is not the super-user."} ,
	{EROFS , "Socket Error! The socket inode would reside on a read-only file system."},
	{EFAULT, "Socket Error! my_addr points outside the user's accessible address space. "},
	{ENAMETOOLONG, "Socket Error! my_addr is too long."}, 
	{ENOENT, "Socket Error! The file does not exist."},
	{ENOMEM, "Socket Error! Insufficient kernel memory was available." },
	{ENOTDIR, "Socket Error! A component of the path prefix is not a directory." },

#ifndef WIN32
	{ELOOP, "Too many symbolic links were encountered in resolving my_addr" },
	{ENOTSOCK, "Argument is a descriptor for a file, not a socket."}
#endif

};


bool CloseSocket(int l_socket)

{
	bool bResult;
	#ifdef WIN32
			bResult = closesocket(l_socket) == 0;
	#else
			bResult =  close(l_socket) == 0;
	#endif
	//fprintf (stderr, "CloseSocket %i resulted %s\n", l_socket, bResult? "true":"false");
	return bResult;
};


const int FLAGS = 0;

inline bool send_buffer(int client_fd, const char* Buffer,int Length, string& Error)
{
	

	int count;
	#ifdef WIN32
		count = send(client_fd, Buffer, Length, 0);
	#else
			count = send(client_fd, Buffer, Length, MSG_NOSIGNAL);
			/*if (count == -1)
			{
				char buffer[1000];
				int err = errno;
				int result = strerror_r(err, buffer, 1000);
				if (result == -1)
					Error =  Format( "unknown error code: %i", err);
				else
					Error =  buffer;
				return false;
			}*/
	#endif
	if (count == -1)
	{
		int err = errno;
		Error = Format("%s; errno=%i", strerror(err), err);;
		return false;
	}	

	Error = "";
	return true;
};



bool SendString (int client_fd, const char* Str, size_t  PacketLength, string& ErrorStr)
{
	try {
		if (!send_buffer(client_fd, (const char*)&PacketLength, sizeof(PacketLength), ErrorStr) )
		{
			return false;
		}

		const int buffer_len = 1000;
		if (PacketLength <= buffer_len)
		{
			if (!send_buffer(client_fd, Str, (int)PacketLength, ErrorStr))
				return false;
		}
		else
		{
			int i = 0;
			for (; i< PacketLength/buffer_len;  i++)
			{
				if (!send_buffer(client_fd, (Str + i*buffer_len), buffer_len, ErrorStr))
				{
					return false;
				}
			};

			size_t rest=PacketLength % buffer_len;
			if (!send_buffer(client_fd, (Str + i*buffer_len), (int)rest, ErrorStr))
			{
				return false;
			}
		};
		
		return true;
	}
	catch (...)
	{
		return false;
	};
};


inline NetworkErrorsEnum recieve_buffer(int client_fd, char* Buffer,int Length, int& have_read, int TimeOut )
{
	fd_set fds;
	struct timeval timeout;
	timeout.tv_sec = (TimeOut == -1) ? 1000: TimeOut;
	timeout.tv_usec = 0;
	

	FD_ZERO(&fds);
	FD_SET(client_fd,&fds);

	if (!select(client_fd+1,&fds, 0,0, &timeout)) return neTimeout;
	
	if (!FD_ISSET(client_fd,&fds)) return neCouldNotReceiveData;
	errno = 0;

	#ifdef WIN32
		have_read = recv (client_fd, Buffer, Length, 0);
	#else
		have_read = recv (client_fd, Buffer, Length, MSG_NOSIGNAL);
	#endif

	//MSG_OOB


	if (have_read <= 0) 
	{
		
		return neCouldNotReceiveData;
	}

	return neSuccess;
};



NetworkErrorsEnum RecieveString (int  client_fd, string& Result, int TimeOut)
{

	try {
		int PacketLength;
		int have_read;

		NetworkErrorsEnum Res = recieve_buffer (client_fd, (char*)&PacketLength, sizeof(PacketLength), have_read, TimeOut);
		if (Res != neSuccess )
		{
			return Res;
		}

		
		const int buffer_len = 1000;
		char buffer[buffer_len+1];

		Result = "";

		size_t  AllRead = 0;

		while (AllRead < PacketLength)
		{
			int to_read = buffer_len;
			if (PacketLength - AllRead < buffer_len)
					to_read = PacketLength - AllRead;

			
			NetworkErrorsEnum Res = recieve_buffer (client_fd, (char*)buffer, to_read, have_read, TimeOut);
			if (Res != neSuccess )
					return Res;

			AllRead += have_read;
			buffer[have_read] = 0;
			Result += buffer;
		};
		return neSuccess;
	}
	catch (...)
	{
		return neCouldNotReceiveData;
	};
};

bool SetSocketRecieveBuffer(int socket_fd, int BufferSize)
{

	if (setsockopt( socket_fd, SOL_SOCKET, SO_RCVBUF, (const char*)&BufferSize, sizeof(int) ))
	{
		return false;
	};
	return true;
};

bool SetSocketSendBuffer(int socket_fd, int BufferSize)
{
	if (setsockopt( socket_fd, SOL_SOCKET, SO_SNDBUF, (const char*)&BufferSize, sizeof(int) ))
	{
		return false;
	};
	return true;
};


string GetBindErrorLinux (int ErrCode)
{
	for (size_t i=0; i < sizeof(errlist) / sizeof(errlist[0]); i++)
		if (errlist[i].errnum == ErrCode)
			return errlist[i].errstr;
	return Format("unknown socket error (bind returned %i)", ErrCode);
};


#ifdef  WIN32

string GetBindErrorWindows (int ErrCode)
{
	switch (ErrCode)
	{
		case	WSANOTINITIALISED : return "Socket Error! A successful WSAStartup call must occur before using this function.";  
		case	WSAENETDOWN : return "Socket Error! The network subsystem has failed. ";
		case	WSAEACCES : return "Socket Error! Attempt to connect datagram socket to broadcast address failed because setsockopt option SO_BROADCAST is not enabled. ";
		case	WSAEADDRINUSE : return "Socket Error! A process on the computer is already bound to the same fully-qualified address and the socket has not been marked to allow address reuse with SO_REUSEADDR. For example, the IP address and port are bound in the af_inet case). (See the SO_REUSEADDR socket option under setsockopt.) ";
		case	WSAEADDRNOTAVAIL : return "Socket Error! The specified address is not a valid address for this computer. ";
		case	WSAEFAULT : return "Socket Error! The name or namelen parameter is not a valid part of the user address space, the namelen parameter is too small, the name parameter contains an incorrect address format for the associated address family, or the first two bytes of the memory block specified by name does not match the address family associated with the socket descriptor s. ";
		case	WSAEINPROGRESS : return "Socket Error! A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function. ";
		case	WSAEINVAL : return "Socket Error! The socket is already bound to an address. ";
		case	WSAENOBUFS : return "Socket Error! Not enough buffers available, too many connections. ";
		case	WSAENOTSOCK : return "Socket Error! The descriptor is not a socket."; 
		default  : return "Socket Error! Could not bind  the socket."; 
	};
};
#endif



SOCKET create_socket (const char* HostName, int Port, bool bBind, string& strError)
{
	
	strError = "";
	struct addrinfo hints, *res, *res0; 
	int error;
	SOCKET l_socket;
	

	memset(&hints, 0, sizeof(hints));
	if (bBind)
		hints.ai_flags = AI_PASSIVE;
	hints.ai_family = PF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	error = getaddrinfo(HostName, Format("%i",Port).c_str(), &hints, &res0); 
	if (error) {
		strError = Format("Socket Error! %s", gai_strerror(error)); /*NOTREACHED*/
		return -1;
	}
	l_socket = -1;
	string cause  = "no addresses";
	for (res = res0; res; res = res->ai_next) 
	{ 
		l_socket = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		if (l_socket < 0) 
		{
			cause = "Could not create socket";
			continue;
		} 
		int Res;
		errno = 0;

		if (bBind)
		{
			int flag = 1; 

			if (setsockopt( l_socket, SOL_SOCKET, SO_REUSEADDR, (const char*)&flag, sizeof(flag) ) != 0)
			{
				cause = "Socket Error! Could not set socket option SO_REUSEADDR";
				CloseSocket(l_socket);
				l_socket = -1;
				continue;
			};
			Res = bind(l_socket, res->ai_addr, res->ai_addrlen);
		}
		else
			Res = connect(l_socket, res->ai_addr, res->ai_addrlen);

		

		if (Res < 0) 
		{
			if (bBind)
			{
				#ifdef WIN32
					cause = GetBindErrorWindows(WSAGetLastError());
				#else
					cause = GetBindErrorLinux(errno);
				#endif
			}
			else
				cause = "Could not connect";
			CloseSocket(l_socket);
			l_socket = -1;
			continue;
		} 
		break; /* okay we got one */ 
	}
	if (l_socket == -1) 
	{
		strError = Format("%s for %s:%i\n", cause.c_str(), HostName, Port);
		/*NOTREACHED*/
	}
	freeaddrinfo(res0); 
	return l_socket;

};




bool SocketInitialize(bool bReadFromLocalFile)
{
	#ifdef WIN32
		WORD    wVersionRequested;
		WSADATA wsaData;
		int     err;
		wVersionRequested = MAKEWORD(1,1);
		err = WSAStartup(wVersionRequested,&wsaData);
		if (err != 0)

		{
			//MessageBox(NULL, "Service could invoke WSAStartup()", "Concordance Socket", MB_OK);
			return false;
		}
	#endif
	
	
	return true;
};

bool SocketDeinitialize()
{
	#ifdef WIN32
		WSACleanup ();
	#endif

	return true;
};

string GetNetworkErrorString(const NetworkErrorsEnum& t)
{
	switch (t) {
		case neSuccess: return "No error";
		case neNetworkError: return "General network error";
		case neTimeout: return "Timeout elapsed";
		case neProtocolError: return "Different protocols are used";
		case neSubcorpusNotFound: return "Unknown subcorpus";
		case neInternalReadAccessError: return "Internal index access error";
		case neCouldNotOpenSocket: return "Could not open some socket";
		case neCouldNotSendData: return "Could not send data to a socket";
		case neCouldNotReceiveData: return "Could not receive data to a socket";
			
			
			
	};
	return "Unknown error code";
};


CHost::CHost(bool bDoubleMessage, ProtocolDensityEnum pdProtocolDensity)
{
	m_LogFunction = 0;
	m_pdProtocolDensity = pdProtocolDensity;
	m_bDoubleMessage = bDoubleMessage;
	m_ListenSocket = -1;
	m_Port = -1;
};



bool CHost::ReadFromString (string S)
{
	StringTokenizer tok(S.c_str(), " \t\r\n");
	int ItemNo = 0;
	while (tok()) 
	{
			string Item = tok.val();
			if (ItemNo == 0)
				m_CorporaName = Item;
			else
			if (ItemNo == 1)
				m_HostName = Item;
			else
			if (ItemNo == 2)
			{
				m_Port = atoi(Item.c_str());
				m_LocalPath = tok.get_rest();
				Trim (m_LocalPath);
				break;
			};
			ItemNo++;
	};

	if (m_HostName.empty()) return false;

	return true;
};

int	CHost::CreateAndConnectSocket(string& strError) const
{
	int l_socket =  create_socket(m_HostName.c_str(), m_Port, false, strError);
	//fprintf (stderr, "create_and_connect %s:%i resulted in %i\n", HostName, Port, l_socket);
	return l_socket;
};


bool LoadHosts (string Path, vector<CHost>& Hosts)
{
	if (!FileExists(Path.c_str()))
	{
		fprintf (stderr, "No access to %s in LoadHosts\n", Path.c_str());
		return false;
	};
	FILE * fp = fopen (Path.c_str(), "r");
	if (!fp) 
	{
		fprintf (stderr, "Could not open file %s\n", Path.c_str());
		return false;
	};
	char buffer[1000];
	int LineNo = 0;
	while (fgets(buffer, 1000, fp))
	{
		LineNo++;
		string Q = buffer;
		Trim(Q);
		if (Q.empty()) continue;
		if (Q[0] == '/') continue;
		CHost T(false, pdMediumDensity);
		if (!T.ReadFromString(Q)) 
		{
			fprintf (stderr, "Could not read line No %i\n", LineNo);
			fclose(fp);
			return false;
		};
		
		Hosts.push_back(T);
	};
	
	fclose(fp);

	return true;
};

void CHost::AnswerByRequest (SOCKET rConnectedSocket, const struct sockaddr_in* client_addr, int TimeOut, bool& bCloseSocket )
{
	bCloseSocket = false;
	string S;

	if (m_pdProtocolDensity  == pdHeavyDensity)
	{
		m_LogFunction(Format("%s recieves a string (Timeout=%i)", m_CorporaName.c_str(), TimeOut ));
	}

	
	NetworkErrorsEnum Res = RecieveString(rConnectedSocket, S, TimeOut);
							
	if (Res != neSuccess)
	{
			m_LogFunction(Format("Error: Cannot recieve string for corpus \"%s\" (error:%s)\n",m_CorporaName.c_str(), GetNetworkErrorString(Res).c_str()));
			return;
	};


	if (m_pdProtocolDensity  == pdHeavyDensity)
	{
		const char* src_addr = inet_ntoa(client_addr->sin_addr);
		m_LogFunction(Format("%s recv \"%s\" from %s", m_CorporaName.c_str(), S.c_str(),src_addr ));
	}

	string Result;
	if (S == DDCCloseSocketString )
	{
		// closing socket
		bCloseSocket = true;
		Result = "ok";
	}
	else
		Result  = ProcessSocketString(S, rConnectedSocket);

	// send the answer
	if (m_pdProtocolDensity  == pdHeavyDensity)
	{
		m_LogFunction(Format("%s has finished processing", m_CorporaName.c_str() ));
	}

	string ErrorStr;
	if (!SendString(rConnectedSocket, Result.c_str(), Result.length(), ErrorStr))
	{
		m_LogFunction(Format("Error! Cannot send string: %s", ErrorStr.c_str()));
	}
	else
	if (m_pdProtocolDensity  == pdHeavyDensity)
	{
		const char* src_addr = inet_ntoa(client_addr->sin_addr);
		m_LogFunction(Format("%s send back the result to %s (%i bytes)", m_CorporaName.c_str(), src_addr, Result.size() ));
	}

}

void	CHost::StopAcceptingThread() 
{
	if (m_ListenSocket == -1)
		return;
	else
	{
		// I have tried to send a special closing message to m_ListenSocket,
		// which should have a special processing in AnswerByRequest,
		// but I failed to close a socket correctly, so I would like to do the following:
		// (even knowing that m_ListenSocket was created in a different thread)

		int l = m_ListenSocket;
		m_ListenSocket = -1;
		CloseSocket(l);
		
	};
};

string	CHost::ProcessSocketString(const string& S, SOCKET rConnectedSocket) 
{
	assert(false);
	return "";
};
//#ifndef socklen_t
//	typedef size_t socklen_t;
//#endif

void   CHost::RunThread()
{
	m_ListenSocket = -1;
	string strError; 

	m_ListenSocket = create_socket(m_HostName.c_str(), m_Port, true, strError);
	if (m_ListenSocket == -1)
	{
		string s = Format("%s [corpus name = %s] \n", strError.c_str(), m_CorporaName.c_str());
		m_LogFunction (  s );
		//fprintf (stderr, "%s\n", s.c_str());
		return ;
	};

	SetSocketRecieveBuffer (m_ListenSocket, 100000);

	// set maximal length of pending queue to 5 
	int listen_val = listen(m_ListenSocket, 5);
	if (listen_val == -1) {
		m_LogFunction ("Could not create the socket queue");
		CloseSocket(m_ListenSocket);
		m_ListenSocket = -1;
	}

	m_LogFunction ("waiting for accept");
	size_t NumberOfSocketAccepts = 0;
	for (;;)
	{
		NumberOfSocketAccepts++;

		if (!(NumberOfSocketAccepts%100))
			m_LogFunction (Format("QueryCount = %i", NumberOfSocketAccepts));

		
		struct sockaddr_in client_addr;
		/*#ifdef WIN32
			int client_addr_len = sizeof(sockaddr_in);
		#else
			socklen_t client_addr_len = sizeof(sockaddr_in);
		#endif*/
		socklen_t client_addr_len = sizeof(sockaddr_in);
	
		SOCKET rConnectedSocket = accept(m_ListenSocket, (struct sockaddr *)&client_addr, &client_addr_len);

		if (m_ListenSocket == -1)  // the thread  should be stopped
		{
			if (rConnectedSocket  != -1) 
				CloseSocket(rConnectedSocket);
			break;
		};

		if (rConnectedSocket == -1) 
		{
			m_LogFunction ("Error:Could not accept a  socket ");
			continue;
		}
	

		try
		{
			//  if we have passed "accept", then Timeout can be very short
			bool bCloseSocket;
			AnswerByRequest(rConnectedSocket, &client_addr, 10, bCloseSocket);
			if (bCloseSocket) 
			{
				CloseSocket(rConnectedSocket);
				break;
			};

			if (m_bDoubleMessage)
					AnswerByRequest(rConnectedSocket, &client_addr, 30, bCloseSocket);

			if (!CloseSocket(rConnectedSocket))
				m_LogFunction ( Format("cannot close socket for %s", m_CorporaName.c_str())  );

		}
		catch(...)
		{
			m_LogFunction("Error: An exception occurred");
			m_LogFunction("Closing accepting socket");
			if (!CloseSocket(rConnectedSocket))
				m_LogFunction ( Format("cannot close socket for %s", m_CorporaName.c_str())  );;
		};

			
	}

	if (m_ListenSocket != -1)
	{
		m_LogFunction(Format("close socket for %s %s", m_CorporaName.c_str(), GetAddressStr().c_str()));

		if (!CloseSocket(m_ListenSocket))
			m_LogFunction ( Format("cannot close socket for %s", m_CorporaName.c_str())  );
		else
			m_ListenSocket = -1;

	};

};

#ifdef WIN32
	void  _cdecl ListenerThread( void* pParam )
#else
	void*   ListenerThread( void* pParam )
#endif
{
	CHost* pHost = (CHost* )pParam;

	#ifdef WIN32
		CoInitialize(NULL);
	#endif

	if ( SocketInitialize(false) )
	{
		pHost->RunThread();	
		SocketDeinitialize();
	}
	else
		pHost->m_LogFunction("Service could invoke AfxSocketInit()");
	

	#ifdef WIN32 
		CoUninitialize();
		_endthread(); 
		return;	
	#else 
		return NULL; 
	#endif

};

void CHost::CreateListener() 
{

	m_LogFunction( Format ("Create Listener %s (%s)", m_CorporaName.c_str(), GetAddressStr().c_str()) );
	#ifdef WIN32
		_beginthread( ListenerThread, 0, (void*)this );
	#else
		pthread_t aThread;
		pthread_create(&aThread, NULL, ListenerThread, (void*)this);
	#endif

};

string	CHost::GetAddressStr() const
{
	if (m_HostName.empty())
		return "(null)";
	else
		return Format("%s:%i", m_HostName.c_str(), m_Port);
};

const string&	CHost::GetLocalPathOfIndex() const
{
	return m_LocalPath;
};

void CHost::CopyAddressParametersFrom(const CHost& X)
{
	m_HostName = X.m_HostName;
	m_Port = X.m_Port;
	m_LocalPath = X.m_LocalPath;
	m_CorporaName = X.m_CorporaName;
};


