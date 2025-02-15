#if !defined(AFX_WMPNETWORK_H__D77F3955_C77D_4318_9BCD_D5783F60A5F7__INCLUDED_)
#define AFX_WMPNETWORK_H__D77F3955_C77D_4318_9BCD_D5783F60A5F7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Machine generated IDispatch wrapper class(es) created by Microsoft Visual C++

// NOTE: Do not modify the contents of this file.  If this class is regenerated by
//  Microsoft Visual C++, your modifications will be overwritten.

/////////////////////////////////////////////////////////////////////////////
// CWMPNetwork wrapper class

class CWMPNetwork : public COleDispatchDriver
{
public:
	CWMPNetwork() {}		// Calls COleDispatchDriver default constructor
	CWMPNetwork(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	CWMPNetwork(const CWMPNetwork& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	long GetBandWidth();
	long GetRecoveredPackets();
	CString GetSourceProtocol();
	long GetReceivedPackets();
	long GetLostPackets();
	long GetReceptionQuality();
	long GetBufferingCount();
	long GetBufferingProgress();
	long GetBufferingTime();
	void SetBufferingTime(long nNewValue);
	long GetFrameRate();
	long GetMaxBitRate();
	long GetBitRate();
	long getProxySettings(LPCTSTR bstrProtocol);
	void setProxySettings(LPCTSTR bstrProtocol, long lProxySetting);
	CString getProxyName(LPCTSTR bstrProtocol);
	void setProxyName(LPCTSTR bstrProtocol, LPCTSTR bstrProxyName);
	long getProxyPort(LPCTSTR bstrProtocol);
	void setProxyPort(LPCTSTR bstrProtocol, long lProxyPort);
	CString getProxyExceptionList(LPCTSTR bstrProtocol);
	void setProxyExceptionList(LPCTSTR bstrProtocol, LPCTSTR pbstrExceptionList);
	BOOL getProxyBypassForLocal(LPCTSTR bstrProtocol);
	void setProxyBypassForLocal(LPCTSTR bstrProtocol, BOOL fBypassForLocal);
	long GetMaxBandwidth();
	void SetMaxBandwidth(long nNewValue);
	long GetDownloadProgress();
	long GetEncodedFrameRate();
	long GetFramesSkipped();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WMPNETWORK_H__D77F3955_C77D_4318_9BCD_D5783F60A5F7__INCLUDED_)
