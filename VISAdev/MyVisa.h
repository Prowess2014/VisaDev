//
// MyVisa.h: Rewrite the myvisa head
//
// File Name:			MyVisa.h
// Function:			Complete Tektronix oscilliscope operation methods.
// Version V 1.0
// Created:				08/17/2015
// Last Modified Time:	08/17/2015
// Author:				zy gong
// Email:				zy.gong.pumc@gmail.com
//////////////////////////////////////////////////////////////////////

#if !defined(_MYVISA_H_INCLUDED_)
#define _MYVISA_H_INCLUDED_

#pragma once
#pragma comment(lib,"visa32.lib")		// load visa32.lib
#include "visa.h"						// visa header
#include "windows.h"
// macros
#define BUF_SIZE	20020
class CMyVisa
{
public:
	CMyVisa();
	virtual ~CMyVisa();
public:
	// variables
	ViSession m_defaultRM;		// default visa resource
	ViSession m_instr;			// device visa resource
	ViStatus  m_status;			// status 
	ViUInt32  m_retCnt;

	char		m_buf[BUF_SIZE];	// buf
	HANDLE		m_hEvent;			// event handler, to avoid conflicts between read and write
	ViUInt32	m_timeout;
	// resource management:
	int OpenDefaultRM();		// open default visa resource manager
	int	Open(char *viRsrc, int timeout);	// open dev resource	
	char *Find(int *retCnt = VI_NULL);			// find dev list
	int Close();				// close dev resource
	int CloseRM();				// close resource manager
	// 
public:
	// attribution
	int GetAttr(void);			// get Attr
	int SetAttr(void);			// set Attr

	// IO operations : basic read and writing
	int	 Write(char *cmdStr, int len);		// write 
	int	 Write(char *cmdStr);		// write overload
	char *Read(int ReadLen);		// read

	// IO operations: formated read and writing
	int FMTWrite(char *cmdStr);
	char *FMTRead(char *cmdStr);
	char *Query(char *cmdStr, int maxlen, int *retCnt = VI_NULL);
	int Flush(void);

	// Handling events
	int QueueEvnet(void);
	int CallbackEvent(void);
	// lock and unlock
	int Lock(bool bLock = true);		// Obtain or Relinquish a lock on the specified resource
	//
};
#endif // !defined(_MYVISA_H_INCLUDED_)
