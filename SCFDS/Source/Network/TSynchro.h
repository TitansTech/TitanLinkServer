/*#############################################################################
# TSYNCHRO.H
#
# SCA Software International S.A.
# http://www.scasoftware.com
# scaadmin@scasoftware.com
#
# Copyright (c) 2000 SCA Software International S.A.
#
# Date: 01.02.2000
# Author: Zoran M.Todorovic
#
# This software is provided "AS IS", without a warranty of any kind.
# You are free to use/modify this code but leave this header intact.
#
#############################################################################*/

#ifndef __TSYNCHRO_H__
#define __TSYNCHRO_H__

#ifndef _WINDOWS_
# include <windows.h>
#endif

#ifndef _INC_CRTDBG
# include <ctrdbg.h>
#endif

//=============================================================================
// TSynchroBase
//
// Base class for a set of synchronization classes
//=============================================================================

class TSynchroBase {
private:
  BOOL m_bLocked;

protected:
  virtual BOOL DoLock(DWORD dwTimeout) = 0;
  virtual void DoUnlock(void) = 0;

public:
  TSynchroBase()
  {
    m_bLocked = FALSE;
  }
  virtual ~TSynchroBase()
  {
  }

  BOOL Lock(DWORD dwTimeout = 250)
  {
    m_bLocked = DoLock(dwTimeout);
    return m_bLocked;
  }
  virtual void Unlock(void)
  {
    DoUnlock();
    m_bLocked = FALSE;
  }
  virtual BOOL IsLocked(void)
  {
    return m_bLocked;
  }
};

//=============================================================================
// TSynchroTransient
//
// Wrapper that locks in constructor and unlocks in destructor. With this 
// class you do not have to think about unlocking. Just create it on stack 
// (not with new).
//=============================================================================

class TSynchroTransient {
private:
  TSynchroBase *m_pSynchro;

private:
  TSynchroTransient()
  {
  }

public:
  TSynchroTransient(TSynchroBase *pSynchro, DWORD dwTimeout = 250)
  {
    _ASSERTE(pSynchro);
    m_pSynchro = pSynchro;
    m_pSynchro->Lock(dwTimeout);
  }
  ~TSynchroTransient()
  {
    m_pSynchro->Unlock();
  }
  BOOL IsLocked(void)
  {
    return m_pSynchro->IsLocked();
  }
};

//=============================================================================
// TSynchroMutex
//
// Use mutex for synchronization
//=============================================================================

class TSynchroMutex : public TSynchroBase {
private:
  HANDLE m_hMutex;

private:
  virtual BOOL DoLock(DWORD dwTimeout)
  {
    DWORD dwCode = ::WaitForSingleObject(m_hMutex, dwTimeout);
    return (dwCode == WAIT_OBJECT_0) ? TRUE : FALSE;
  }
  virtual void DoUnlock(void)
  {
    ::ReleaseMutex(m_hMutex);
  }

public:
  TSynchroMutex(BOOL bIsOwned = FALSE, LPCTSTR szName = NULL, 
                LPSECURITY_ATTRIBUTES lpSecAttr = NULL)
  {
    m_hMutex = ::CreateMutex(lpSecAttr, bIsOwned, szName);
  }
  virtual ~TSynchroMutex()
  {
    ::CloseHandle(m_hMutex);
  }
};

//=============================================================================
// TSynchroCriticalSection
//
// Use critical section for synchronization.
//=============================================================================

class TSynchroCriticalSection : public TSynchroBase {
private:
  BOOL m_bBlock;
  CRITICAL_SECTION m_Section;

private:
  virtual BOOL DoLock(DWORD dwTimeout = 250)
  {
    UNREFERENCED_PARAMETER(dwTimeout);
    if (m_bBlock) {
      ::EnterCriticalSection(&m_Section);
      return TRUE;
    } else {
    #if (_WIN32_WINNT >= 0x0400)
    return ::TryEnterCriticalSection(&m_Section);
    #endif
    }
    return FALSE;
  }
  virtual void DoUnlock(void)
  {
    ::LeaveCriticalSection(&m_Section);
  }

public:
  TSynchroCriticalSection(BOOL bBlock = TRUE)
  {
    m_bBlock = bBlock;
    ::InitializeCriticalSection(&m_Section);
  }
  virtual ~TSynchroCriticalSection()
  {
    ::DeleteCriticalSection(&m_Section);
  }
};

#endif

/*#############################################################################
# End of file
#############################################################################*/