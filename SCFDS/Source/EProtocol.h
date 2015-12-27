
#include "ServerManager.h"

void EProtocolCore(cDataServer * sManager, int aIndex, BYTE HeadCode, LPBYTE aRecv);

struct PMSG_REQ_VIEW_EC_MN {
  /*<thisrel this+0x0>*/ /*|0x3|*/ struct PBMSG_HEAD2 h;
  /*<thisrel this+0x4>*/ /*|0x4|*/ int iINDEX;
  /*<thisrel this+0x8>*/ /*|0xb|*/ char szUID[11];
};
// <size 0x14>

struct PMSG_REQ_REGISTER_EVENTCHIP {
  /*<thisrel this+0x0>*/ /*|0x3|*/ struct PBMSG_HEAD2 h;
  /*<thisrel this+0x4>*/ /*|0x4|*/ int iINDEX;
  /*<thisrel this+0x8>*/ /*|0x1|*/ unsigned char Pos;
  /*<thisrel this+0x9>*/ /*|0xb|*/ char szUID[11];
};
// <size 0x14>

struct PMSG_REQ_RESET_EVENTCHIP {
  /*<thisrel this+0x0>*/ /*|0x3|*/ struct PBMSG_HEAD2 h;
  /*<thisrel this+0x4>*/ /*|0x4|*/ int iINDEX;
  /*<thisrel this+0x8>*/ /*|0xb|*/ char szUID[11];
};
// <size 0x14>

struct PMSG_REQ_REGISTER_STONES {
  /*<thisrel this+0x0>*/ /*|0x3|*/ struct PBMSG_HEAD2 h;
  /*<thisrel this+0x4>*/ /*|0x4|*/ int iINDEX;
  /*<thisrel this+0x8>*/ /*|0x1|*/ unsigned char iPosition;
  /*<thisrel this+0x9>*/ /*|0xb|*/ char szUID[11];
};
// <size 0x14>

struct PMSG_REQ_DELETE_STONES {
  /*<thisrel this+0x0>*/ /*|0x3|*/ struct PBMSG_HEAD2 h;
  /*<thisrel this+0x4>*/ /*|0x4|*/ int iINDEX;
  /*<thisrel this+0x8>*/ /*|0xb|*/ char szUID[11];
  /*<thisrel this+0x13>*/ /*|0x1|*/ char bSUCCESS;
  /*<thisrel this+0x14>*/ /*|0x4|*/ int iStoneCount;
};
// <size 0x18>

struct PMSG_REQ_2ANIV_SERIAL {
  /*<thisrel this+0x0>*/ /*|0x3|*/ struct PBMSG_HEAD2 h;
  /*<thisrel this+0x4>*/ /*|0x4|*/ int iINDEX;
  /*<thisrel this+0x8>*/ /*|0xb|*/ char szUID[11];
  /*<thisrel this+0x13>*/ /*|0x5|*/ char SERIAL1[5];
  /*<thisrel this+0x18>*/ /*|0x5|*/ char SERIAL2[5];
  /*<thisrel this+0x1d>*/ /*|0x5|*/ char SERIAL3[5];
  /*<thisrel this+0x24>*/ /*|0x4|*/ int iMEMB_GUID;
};
// <size 0x28>

struct PMSG_REQ_REG_RINGGIFT {
  /*<thisrel this+0x0>*/ /*|0x3|*/ struct PBMSG_HEAD2 h;
  /*<thisrel this+0x4>*/ /*|0x4|*/ int iINDEX;
  /*<thisrel this+0x8>*/ /*|0xb|*/ char szUID[11];
  /*<thisrel this+0x13>*/ /*|0x1|*/ unsigned char btGiftKind;
};
// <size 0x14>

struct PMSG_REQ_REG_CC_OFFLINE_GIFT {
  /*<thisrel this+0x0>*/ /*|0x3|*/ struct PBMSG_HEAD2 h;
  /*<thisrel this+0x4>*/ /*|0x4|*/ int iINDEX;
  /*<thisrel this+0x8>*/ /*|0xb|*/ char szUID[11];
  /*<thisrel this+0x14>*/ /*|0x2|*/ unsigned short wServerCode;
  /*<thisrel this+0x16>*/ /*|0xb|*/ char szNAME[11];
};
// <size 0x24>

struct PMSG_REQ_REG_DL_OFFLINE_GIFT {
  /*<thisrel this+0x0>*/ /*|0x3|*/ struct PBMSG_HEAD2 h;
  /*<thisrel this+0x4>*/ /*|0x4|*/ int iINDEX;
  /*<thisrel this+0x8>*/ /*|0xb|*/ char szUID[11];
  /*<thisrel this+0x14>*/ /*|0x2|*/ unsigned short wServerCode;
  /*<thisrel this+0x16>*/ /*|0xb|*/ char szNAME[11];
};
// <size 0x24>

struct PMSG_REQ_REG_HT_OFFLINE_GIFT {
  /*<thisrel this+0x0>*/ /*|0x3|*/ struct PBMSG_HEAD2 h;
  /*<thisrel this+0x4>*/ /*|0x4|*/ int iINDEX;
  /*<thisrel this+0x8>*/ /*|0xb|*/ char szUID[11];
  /*<thisrel this+0x14>*/ /*|0x2|*/ unsigned short wServerCode;
  /*<thisrel this+0x16>*/ /*|0xb|*/ char szNAME[11];
};
// <size 0x24>

/**************************************************************************/

struct PMSG_ANS_VIEW_EC_MN {
  /*<thisrel this+0x0>*/ /*|0x3|*/ struct PBMSG_HEAD2 h;
  /*<thisrel this+0x4>*/ /*|0x4|*/ int iINDEX;
  /*<thisrel this+0x8>*/ /*|0xb|*/ char szUID[11];
  /*<thisrel this+0x13>*/ /*|0x1|*/ char bSUCCESS;
  /*<thisrel this+0x14>*/ /*|0x2|*/ short nEVENT_CHIPS;
  /*<thisrel this+0x18>*/ /*|0x4|*/ int iMUTO_NUM;
};
// <size 0x1c>

struct PMSG_ANS_REGISTER_EVENTCHIP {
  /*<thisrel this+0x0>*/ /*|0x3|*/ struct PBMSG_HEAD2 h;
  /*<thisrel this+0x4>*/ /*|0x4|*/ int iINDEX;
  /*<thisrel this+0x8>*/ /*|0x1|*/ unsigned char Pos;
  /*<thisrel this+0x9>*/ /*|0xb|*/ char szUID[11];
  /*<thisrel this+0x14>*/ /*|0x1|*/ char bSUCCESS;
  /*<thisrel this+0x16>*/ /*|0x2|*/ short nEVENT_CHIPS;
};
// <size 0x18>

struct PMSG_ANS_RESET_EVENTCHIP {
  /*<thisrel this+0x0>*/ /*|0x3|*/ struct PBMSG_HEAD2 h;
  /*<thisrel this+0x4>*/ /*|0x4|*/ int iINDEX;
  /*<thisrel this+0x8>*/ /*|0xb|*/ char szUID[11];
  /*<thisrel this+0x13>*/ /*|0x1|*/ char bSUCCESS;
};
// <size 0x14>

struct PMSG_ANS_VIEW_STONES {
  /*<thisrel this+0x0>*/ /*|0x3|*/ struct PBMSG_HEAD2 h;
  /*<thisrel this+0x4>*/ /*|0x4|*/ int iINDEX;
  /*<thisrel this+0x8>*/ /*|0xb|*/ char szUID[11];
  /*<thisrel this+0x13>*/ /*|0x1|*/ char bSUCCESS;
  /*<thisrel this+0x14>*/ /*|0x4|*/ int iStoneCount;
};
// <size 0x18>

struct PMSG_ANS_REGISTER_STONES {
  /*<thisrel this+0x0>*/ /*|0x3|*/ struct PBMSG_HEAD2 h;
  /*<thisrel this+0x4>*/ /*|0x4|*/ int iINDEX;
  /*<thisrel this+0x8>*/ /*|0x1|*/ unsigned char iPosition;
  /*<thisrel this+0x9>*/ /*|0xb|*/ char szUID[11];
  /*<thisrel this+0x14>*/ /*|0x1|*/ char bSUCCESS;
  /*<thisrel this+0x18>*/ /*|0x4|*/ int iStoneCount;
};
// <size 0x1c>

struct PMSG_ANS_DELETE_STONES {
  /*<thisrel this+0x0>*/ /*|0x3|*/ struct PBMSG_HEAD2 h;
  /*<thisrel this+0x4>*/ /*|0x4|*/ int iINDEX;
  /*<thisrel this+0x8>*/ /*|0xb|*/ char szUID[11];
  /*<thisrel this+0x13>*/ /*|0x1|*/ char bSUCCESS;
  /*<thisrel this+0x14>*/ /*|0x4|*/ int iStoneCount;
};
// <size 0x18>

struct PMSG_ANS_2ANIV_SERIAL {
  /*<thisrel this+0x0>*/ /*|0x3|*/ struct PBMSG_HEAD2 h;
  /*<thisrel this+0x4>*/ /*|0x4|*/ int iINDEX;
  /*<thisrel this+0x8>*/ /*|0xb|*/ char szUID[11];
  /*<thisrel this+0x13>*/ /*|0x1|*/ unsigned char btIsRegistered;
  /*<thisrel this+0x14>*/ /*|0x4|*/ int iGiftNumber;
};
// <size 0x18>

struct PMSG_ANS_REG_RINGGIFT {
  /*<thisrel this+0x0>*/ /*|0x3|*/ struct PBMSG_HEAD2 h;
  /*<thisrel this+0x4>*/ /*|0x4|*/ int iINDEX;
  /*<thisrel this+0x8>*/ /*|0xb|*/ char szUID[11];
  /*<thisrel this+0x13>*/ /*|0x1|*/ unsigned char btIsRegistered;
  /*<thisrel this+0x14>*/ /*|0x1|*/ unsigned char btGiftSection;
  /*<thisrel this+0x15>*/ /*|0x1|*/ unsigned char btGiftKind;
};
// <size 0x18>

struct PMSG_ANS_REG_CC_OFFLINE_GIFT {
  /*<thisrel this+0x0>*/ /*|0x3|*/ struct PBMSG_HEAD2 h;
  /*<thisrel this+0x4>*/ /*|0x4|*/ int iINDEX;
  /*<thisrel this+0x8>*/ /*|0xb|*/ char szUID[11];
  /*<thisrel this+0x14>*/ /*|0x2|*/ unsigned short wServerCode;
  /*<thisrel this+0x16>*/ /*|0xb|*/ char szNAME[11];
  /*<thisrel this+0x24>*/ /*|0x4|*/ int iResultCode;
  /*<thisrel this+0x28>*/ /*|0x32|*/ char szGIFT_NAME[50];
};
// <size 0x5c>

struct PMSG_ANS_REG_DL_OFFLINE_GIFT {
  /*<thisrel this+0x0>*/ /*|0x3|*/ struct PBMSG_HEAD h;
  /*<thisrel this+0x4>*/ /*|0x4|*/ int iINDEX;
  /*<thisrel this+0x8>*/ /*|0xb|*/ char szUID[11];
  /*<thisrel this+0x14>*/ /*|0x2|*/ unsigned short wServerCode;
  /*<thisrel this+0x16>*/ /*|0xb|*/ char szNAME[11];
  /*<thisrel this+0x24>*/ /*|0x4|*/ int iResultCode;
  /*<thisrel this+0x28>*/ /*|0x32|*/ char szGIFT_NAME[50];
};
// <size 0x5c>

struct PMSG_ANS_REG_HT_OFFLINE_GIFT {
  /*<thisrel this+0x0>*/ /*|0x3|*/ struct PBMSG_HEAD2 h;
  /*<thisrel this+0x4>*/ /*|0x4|*/ int iINDEX;
  /*<thisrel this+0x8>*/ /*|0xb|*/ char szUID[11];
  /*<thisrel this+0x14>*/ /*|0x2|*/ unsigned short wServerCode;
  /*<thisrel this+0x16>*/ /*|0xb|*/ char szNAME[11];
  /*<thisrel this+0x24>*/ /*|0x4|*/ int iResultCode;
  /*<thisrel this+0x28>*/ /*|0x32|*/ char szGIFT_NAME[50];
};
// <size 0x5c>

void EGAnsEventChipInfo(cDataServer * sManager, int aIndex,PMSG_REQ_VIEW_EC_MN * lpMsg);
void EGAnsRegEventChipInfo(cDataServer * sManager, int aIndex,PMSG_REQ_REGISTER_EVENTCHIP * lpMsg);
void EGAnsResetEventChip(cDataServer * sManager, int aIndex,PMSG_REQ_RESET_EVENTCHIP * lpMsg);
void EGAnsEventStoneInfo(cDataServer * sManager, int aIndex,PMSG_REQ_VIEW_EC_MN * lpMsg);
void EGAnsRegEventStoneInfo(cDataServer * sManager, int aIndex,PMSG_REQ_REGISTER_STONES * lpMsg);
void EGAnsResetStoneInfo(cDataServer * sManager, int aIndex,PMSG_REQ_RESET_EVENTCHIP * lpMsg);
void EGAns2AnivRegSerial(cDataServer * sManager, int aIndex,PMSG_REQ_2ANIV_SERIAL * lpMsg);
void EGAnsDeleteStones(cDataServer * sManager, int aIndex,PMSG_REQ_DELETE_STONES * lpMsg);
void EGAnsRegRingGift(cDataServer * sManager, int aIndex,PMSG_REQ_REG_RINGGIFT * lpMsg);
void EGAnsRegCCOfflineGift(cDataServer * sManager, int aIndex,PMSG_REQ_REG_CC_OFFLINE_GIFT * lpMsg);
void EGAnsRegDLOfflineGift(cDataServer * sManager, int aIndex,PMSG_REQ_REG_DL_OFFLINE_GIFT * lpMsg);
void EGAnsRegHTOfflineGift(cDataServer * sManager, int aIndex,PMSG_REQ_REG_HT_OFFLINE_GIFT * lpMsg);
