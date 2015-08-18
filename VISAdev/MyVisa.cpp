
#include "MyVisa.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CMyVisa::CMyVisa()
{
	memset(m_buf, 0, sizeof(m_buf));
	m_instr = VI_NULL;
	m_defaultRM = VI_NULL;

	m_hEvent = INVALID_HANDLE_VALUE;
	m_timeout = 1000;
}

CMyVisa::~CMyVisa()
{
	if(m_instr != 0) Close();

	if(m_hEvent != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hEvent);
		m_hEvent = INVALID_HANDLE_VALUE;
	}
}

//////////////////////////////////////////////////////////////////////
// Open the Default Resource Manager
//////////////////////////////////////////////////////////////////////

int CMyVisa::OpenDefaultRM()
{
	if(m_hEvent == INVALID_HANDLE_VALUE)
 		m_hEvent = CreateEvent(NULL, FALSE, TRUE, NULL);

	m_status = viOpenDefaultRM(&m_defaultRM);
	if(m_status < VI_SUCCESS)
		return -1;

	return VI_SUCCESS;
}

//////////////////////////////////////////////////////////////////////
// Open the device whose resource ID assigned by viRsrc
// Paramaters: viRsrc-assign device's resource ID
//			   timeout-the open action's timeout time
//////////////////////////////////////////////////////////////////////
int	CMyVisa::Open(char *viRsrc, int timeout)
{
	if(OpenDefaultRM() < VI_SUCCESS)
		return -1;
	
	m_status = viOpen(m_defaultRM, viRsrc, VI_NULL, timeout, &m_instr);	
	if(m_status < VI_SUCCESS)
	{
		CloseRM();
		return -2;
	}
	
	//Set the timeout time
	m_status = viSetAttribute (m_instr, VI_ATTR_TMO_VALUE, timeout);
	m_timeout = timeout;
	return VI_SUCCESS;
}

//////////////////////////////////////////////////////////////////////
// Close device
//////////////////////////////////////////////////////////////////////
int CMyVisa::Close()
{
	// At first wait the completion of read or write action
	//WaitForSingleObject(m_hEvent, INFINITE);
	// Close viSession
	m_status = viClose(m_instr);
	if(m_status < VI_SUCCESS)
		return -1;
	// Close default resource manager
	if(!CloseRM())
		return -2;
	
//	SetEvent(m_hEvent);
	
	if(m_hEvent != INVALID_HANDLE_VALUE)
	{
		m_hEvent = INVALID_HANDLE_VALUE;
	}

	return VI_SUCCESS;
}
//////////////////////////////////////////////////////////////////////
// Close default resource manager 
//////////////////////////////////////////////////////////////////////
int CMyVisa::CloseRM()
{
	m_status = viClose(m_defaultRM);
	if(m_status < VI_SUCCESS)
		return -1;

	m_instr = VI_NULL;
	m_defaultRM = VI_NULL;
	
	return VI_SUCCESS;
}

//////////////////////////////////////////////////////////////////////
// Write data
// Parameters:cmdStr-the content to write
//			  len-the content's len
//////////////////////////////////////////////////////////////////////
int	CMyVisa::Write(char *cmdStr)
{
	if(m_instr == VI_NULL)
		return -1;
	ViUInt32 nCmdLen = strlen (cmdStr);
	m_status = viLock(m_instr,VI_EXCLUSIVE_LOCK,m_timeout,VI_NULL,VI_NULL);
	if(m_status < VI_SUCCESS) return -2;
	m_status = viWrite(m_instr,(ViBuf) cmdStr, nCmdLen, &m_retCnt);
	//SetEvent(m_hEvent);		// notify that the write action is complete.
	if(m_status < VI_SUCCESS) return -3;
	m_status = viUnlock(m_instr);
	if(m_status < VI_SUCCESS) return -4;
	return 1;
}

//////////////////////////////////////////////////////////////////////
// Read data
// Parameters:maxlen-max length of reading data
//			  retCnt-the actual length of read data
//////////////////////////////////////////////////////////////////////
char* CMyVisa::Read(int ReadLen)
{

	ViPBuf pbuf = (ViPBuf) malloc(ReadLen);

	viRead(m_instr, pbuf, ReadLen, &m_retCnt);
	if (m_retCnt <= 0)
		return VI_NULL;
	if(m_status < VI_SUCCESS)
		memset(m_buf,'\0', BUF_SIZE);
	else
		//strcpy(m_buf, (char*)pbuf);
		memcpy(m_buf, (char*)pbuf, m_retCnt);
	//viFlush(m_instr, VI_WRITE_BUF | VI_READ_BUF_DISCARD);//Tek示波器语句，在读之前要擦一下

	//if(retCnt != NULL) *retCnt = m_retCnt;
	free(pbuf);
	return m_buf;
}

int CMyVisa::Flush(void)
{
	viFlush(m_instr, VI_WRITE_BUF | VI_READ_BUF_DISCARD);//Tek示波器语句，在读之前要擦一下
	return 0;
}

char *CMyVisa::Query(char *cmdStr,int maxlen, int *recCnt)
{

	ViPBuf pbuf = (ViPBuf) malloc(maxlen);
	ViUInt32 nCmdLen = strlen (cmdStr);
	m_status = viWrite(m_instr,(ViBuf) cmdStr, nCmdLen, &m_retCnt);
	if(m_status < VI_SUCCESS) return VI_NULL;
	viRead(m_instr, pbuf, maxlen, &m_retCnt);
	if (m_retCnt <= 0)
		return VI_NULL;
	if(m_status < VI_SUCCESS)
		memset(m_buf,'\0', BUF_SIZE);
	else
	{
		memcpy(m_buf, (char*)pbuf, m_retCnt);
		//*recCnt = m_retCnt;
	}

	return m_buf;
}