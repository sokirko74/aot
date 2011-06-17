#ifndef _string_socket_h
#define _string_socket_h



#include "../common/utilit.h"

enum NetworkErrorsEnum {
	neSuccess = 0,
	neNetworkError = 1,
	neTimeout = 2,
	neProtocolError = 3,
	neSubcorpusNotFound = 4,
	neInternalReadAccessError = 5,
	neCouldNotOpenSocket = 6,
	neCouldNotSendData = 7,
	neCouldNotReceiveData = 8,

};


enum ProtocolDensityEnum {
	pdWeakDensity = 0,
	pdMediumDensity = 1,
	pdHeavyDensity = 2,
};

#define SOCKET int
#ifdef WIN32
	struct sockaddr_in;
#endif


class CHost  
{
	string					m_HostName;
	int						m_Port;
	SOCKET					m_ListenSocket;
	bool					m_bDoubleMessage;
	string					m_LocalPath;
	ProtocolDensityEnum		m_pdProtocolDensity;

	void		AnswerByRequest (SOCKET rConnectedSocket, const struct sockaddr_in* client_addr, int TimeOut, bool& bCloseSocket );

protected:
	virtual string	ProcessSocketString(const string& S, SOCKET rConnectedSocket);

public:
	
	string				m_CorporaName;
	void			(*m_LogFunction)(const string&);
	

	CHost(bool bDoubleMessage, ProtocolDensityEnum pdProtocolDensity);
	bool			ReadFromString (string S);
	void			CreateListener();
	void			CopyAddressParametersFrom(const CHost& X);
	void			StopAcceptingThread();
	void			RunThread();
	int				CreateAndConnectSocket(string& strError) const;
	string			GetAddressStr() const;
	const string&	GetLocalPathOfIndex() const;
};

extern string GetNetworkErrorString(const NetworkErrorsEnum& t);
extern bool SendString (int client_fd, const char* Str, size_t StrLen, string& ErrorStr);
extern NetworkErrorsEnum RecieveString (int  client_fd, string& Result, int TimeOut);
extern bool SetSocketRecieveBuffer(int socket_fd, int BufferSize);
extern bool SetSocketSendBuffer(int socket_fd, int BufferSize);
extern bool SocketInitialize(bool bReadFromLocalFile);
extern bool SocketDeinitialize();
extern bool	LoadHosts (string Path, vector<CHost>& Hosts);
bool CloseSocket(int l_socket);

	
#endif
