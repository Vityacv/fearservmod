#ifndef FEARSDK_H
#define FEARSDK_H

#define LTRESULT unsigned
#define HOBJECT void*
#define HDATABASE void*
#define HCATEGORY void*
#define HRECORD void*
#define HATTRIBUTE void*
#define HWEAPON void*
#define HWEAPONDATA void*
#define HAMMODATA void*
#define HAMMO void*

#define HLOCALOBJ HOBJECT

#define kGearUse 2
#define kMaxChatHistory 25
#define kMaxChatLength 64

#define HGEAR void*

#define GDB_DLL_NAME "GameDatabase"

#define MESSAGE_GUARANTEED 1
#define USE_AI_DATA true

#define MID_PLAYER_CLIENTMSG 119
#define MID_PLAYER_GEAR 170
#define MID_OBJECT_MESSAGE 159
#define MID_PLAYER_RESPAWN 112
#define MID_DROP_GRENADE 128
#define MID_AMMOBOX 1004
#define MID_ADDWEAPON 1005
#define MID_ADDMOD 1008
#define MID_PICKEDUP 1006
#define MID_PICKUPITEM_ACTIVATE 171
#define MID_PLAYER_ACTIVATE 118
#define MID_PLAYER_INFOCHANGE 103

#define USRFLG_CAN_ACTIVATE (1 << 8)
#define USRFLG_MOVEABLE (1 << 5)
#define FLAG_GOTHRUWORLD (1 << 18)
#define FLAG_SOLID (1 << 5)
#define ODBF_ACQUIREMASK 0x00FF0000
#define SFX_PICKUPITEM_ID 10

#define LTARRAYSIZE(a) (sizeof(a) / (sizeof((a)[0])))

inline void LTStrCpy(wchar_t* pDest, const wchar_t* pSrc,
                     uintptr_t nBufferChars) {
  if (!pDest || (nBufferChars == 0)) {
    return;
  }

  if (!pSrc) {
    pSrc = L"";
  }

  wcsncpy(pDest, pSrc, nBufferChars - 1);
  pDest[nBufferChars - 1] = '\0';
}

enum SurfaceType {
  ST_UNKNOWN = 0,      // Unknown value
  ST_AIR = 1,          // Not a surface, but not the sky either
  ST_FLESH = 2,        // Human flesh
  ST_ARMOR = 3,        // Armored Human
  ST_SKY = 110,        // Sky poly
  ST_LADDER = 200,     // Ladder volume brush
  ST_LIQUID = 201,     // Liquid volume brush
  ST_INVISIBLE = 202,  // Invisible surface

  // Set this to a known surface to fall back on if GetSurface() fails.
  ST_DEFAULT = ST_UNKNOWN
};

enum ObjFlagType {
  //! FLAG_  #defines
  OFT_Flags = 0,
  //! FLAG2_ #defines
  OFT_Flags2,
  //! User flags
  OFT_User,
  //! Client flags (Not available on server objects)
  OFT_Client
};

enum ContainerCode {
  CC_NO_CONTAINER = 0,
  CC_WATER = 1,
  CC_CORROSIVE_FLUID = 2,
  CC_FREEZING_WATER = 3,
  CC_POISON_GAS = 4,
  CC_ELECTRICITY = 5,
  CC_ENDLESS_FALL = 6,
  CC_BURN = 7,
  CC_LADDER = 8,
  CC_VOLUME = 9,
  CC_FILTER = 10,
  CC_SAFTEY_NET = 11,
  CC_TRANSITION_AREA = 12,
  CC_JUMP_VOLUME = 13,
  CC_DYNAMIC_SECTOR_VOLUME = 14,
  CC_GRAVITY = 15,
  CC_STAIRS = 16,
  CC_WIND = 17,

  CC_MAX_CONTAINER_CODES
};

enum ClientPlayerMessage {
  CP_MOTION_STATUS = 1,
  CP_DAMAGE,
  CP_WEAPON_STATUS,
  CP_FLASHLIGHT,
  CP_PLAYER_LEAN,
  CP_PHYSICSMODEL,
  CP_DAMAGE_VEHICLE_IMPACT,
  CP_LADDER_SLIDE,
  CP_STORY_CANCEL
};

enum VoteType {
  eVote_Kick,
  eVote_TeamKick,
  eVote_Ban,
  eVote_NextRound,
  eVote_NextMap,
  eVote_SelectMap,

  eVote_None,
  kNumVoteTypes = eVote_None
};

struct CLIENT_INFO {
  unsigned nPing;
  unsigned nID;
  stl::wstring sName;
  stl::string sInsignia;
};

enum PlayerState {
  ePlayerState_None,
  ePlayerState_Alive,
  ePlayerState_Dead,
  ePlayerState_Dying_Stage1,
  ePlayerState_Dying_Stage2,
  ePlayerState_Spectator,
};

enum SpectatorMode {
  eSpectatorMode_None,
  eSpectatorMode_Fixed,
  eSpectatorMode_Tracking,
  eSpectatorMode_Clip,
  eSpectatorMode_Fly,
  eSpectatorMode_Follow,

  eSpectatorMode_Count
};

struct _LTGUID {
  unsigned a;
  unsigned short b;
  unsigned short c;
  unsigned char d[8];
};

struct _LTGUID_64 {
  __int64 a;
  __int64 b;
};

union LTGUID {
  _LTGUID guid;
  _LTGUID_64 guid_64;  // use this for fast & easy compares
};

template <class T>

struct TVector3 {
  T x, y, z;

  // typedef typename TFloatType<T>::Type float_T;
};

typedef TVector3<float> LTVector;

typedef void(*RequestURLDataCallback);

class fearData;
class appData;
class handleData;

class IDatabaseMgr {
 public:
  virtual void v1();
  virtual void v2();
  virtual void v3();
  virtual void v4();
  virtual void v5();
  virtual void v6();
  virtual void v7();
  virtual HCATEGORY GetCategory(HDATABASE, const char*);
  virtual void v9();
  virtual void v10();
  virtual void v11();
  virtual void v12();
  virtual void v13();
  virtual void v14();
  virtual HRECORD GetRecord(HCATEGORY hCategory, const char* pszRecordName);
  virtual unsigned GetNumRecords(HCATEGORY hCategory);
  virtual HRECORD GetRecordByIndex(HCATEGORY, uintptr_t);
  virtual void v18();
  virtual unsigned GetRecordIndex(HRECORD hRecord);
  virtual void v20();
  virtual void v21();
  virtual void v22();
  virtual HATTRIBUTE GetAttribute(HRECORD, const char*);
  virtual void v24();
  virtual void v25();
  virtual void v26();
  virtual void v27();
  virtual void v28();
  virtual void v29();
  virtual void v30();
  virtual void v31();
  virtual __int32 GetInt32(HATTRIBUTE, unsigned nValueIndex = 0, int nDef = 0);
  virtual void v33();
  virtual void v34();
  virtual void v35();
  virtual void v36();
  virtual void v37();
  virtual void v38();
  virtual void v39();
  virtual HRECORD GetRecordLink(HATTRIBUTE hAttribute, unsigned nValueIndex,
                                HRECORD hDefault);
};

class ILTBase {
 public:
  ILTBase() {}
  virtual ~ILTBase() = 0;
};

class ILTMessage_Read;
typedef ILTMessage_Read* CLTMsgRef_Read;
class ILTMessage_Read {
 public:
  virtual void* v1() = 0;
  virtual CLTMsgRef_Read init() = 0;
  virtual CLTMsgRef_Read free() = 0;
};

class ILTCommon {
 public:
  virtual void* v1();
  virtual void* init2();
  virtual void* v3();
  virtual LTRESULT GetObjectFlags(const HOBJECT hObj,
                                  const ObjFlagType flagType,
                                  unsigned& dwFlags);
  virtual LTRESULT SetObjectFlags(HOBJECT hObj, const ObjFlagType flagType,
                                  unsigned dwFlags, unsigned dwMask);
  virtual void* v6();
  virtual void* v7();
  virtual void* v8();
  virtual void* v9();
  virtual void* v10();
  virtual void* v11();
  virtual void* v12();
  virtual void* v13();
  virtual void* v14();
  virtual void* initCtx(void* val);
};

class ILTClient {
 public:
  virtual void* v1();
  virtual void* v2();
  virtual void* v3();
  virtual void* v4();
  virtual void* v5();
  virtual void* v6();
  virtual void* v7();
  virtual void* v8();
  // virtual void* v9();
  virtual unsigned GetObjectPos(HOBJECT hObj, LTVector* pPos);
  virtual void* v10();
  virtual void* v11();
  virtual void* v12();
  virtual void* v13();
  virtual void* v14();
  virtual void* v15();
  virtual void* v16();
  virtual void* v17();
  virtual void* v18();
  virtual void* v19();
  virtual void* v20();
  virtual ILTCommon* Common();
  virtual void* v22();
  virtual void* v23();
  virtual void* v24();
  virtual void* v25();
  virtual void* v26();
  virtual void* v27();
  virtual void* v28();
  virtual void* v29();
  virtual void* v30();
  virtual void* v31();
  virtual void* v32();
  virtual void* v33();
  void* p1;
  void* p2;
  void* p3;
  void* p4;
  void* p5;
  void* p6;
  void* p7;
  void* p8;
  void* p9;
  void* p10;
  LTRESULT (*GetLocalClientID)(unsigned* pID);
  // void* p12;
  void (*Disconnect)();
  void* p13;
  void* p14;
  void* p15;
  void* p16;
  void* p17;
  void* p18;
  void* p19;
  void* p20;
  void* p21;
  void* p22;
  void* p23;
  void* p24;
  void* p25;
  void* p26;
  void* p27;
  void* p28;
  void* p29;
  void* p30;
  void* p31;
  void* p32;
  HOBJECT (*GetClientObject)();
};

class ILTServer {
 public:
  virtual void v1();
  virtual void v2();
  virtual void v3();
  virtual void v4();
  virtual void v5();
  virtual void v6();
  virtual void v7();
  virtual void v8();
  virtual LTRESULT GetObjectPos(HLOCALOBJ hObj, LTVector* pPos);
  virtual LTRESULT SetObjectPos(HLOCALOBJ hObj, const LTVector& vPos);
};

class CAutoMessageBase {
 public:
  virtual void v1();
  virtual void increasePacketCnt();
  virtual void* free();
  virtual void v4();
  virtual void v5();
  virtual void v6();
  virtual void v7();
  virtual CLTMsgRef_Read Read1();
  virtual void v9();
  virtual void WriteBits(unsigned nValue, unsigned nSize);
  virtual void WriteBits64(unsigned nValue, unsigned nSize);
  virtual void WriteData(void* pData, unsigned nSize);
  // virtual void v12();
  virtual void v13();
  virtual void v14();
  virtual void v15();
  virtual void WriteWString(const wchar_t* pwString);
  // virtual void v16();
  virtual void v17();
  virtual void v18();
  virtual void v19();
  virtual void WriteObject(HOBJECT);
  virtual void v21();
  virtual void WriteDatabaseRecord(IDatabaseMgr*, HGEAR);
};

class IGameSpyBrowser {
 public:
  virtual void v1();
  virtual void v2();
  virtual void v3();
  virtual void v4();
  virtual void v5();
  virtual void v6();
  virtual void v7();
  virtual void v8();
  virtual bool RequestURLData(const char* pszURL,
                              RequestURLDataCallback cbRequestURLData,
                              void* pCallbackParam);
};

class ClientConnectionMgr {};

class CServerMissionMgr {
 public:
  void* p1;
  void* p2;
  void* p3;
  void* p4;
  void* p5;
  void* p6;
  void* p7;
  void* p8;
  void* p9;
  void* p10;
  void* p11;
  void* p12;
  void* p13;
  void* p14;
  void* p15;
  void* p16;
  void* p17;
  void* p18;
  void* p19;
  void* p20;
  void* p21;
  void* p22;
  void* p23;
  void* p24;
  void* p25;
  void* p26;
  void* p27;
  void* p28;
  void* p29;
  void* p30;
  void* p31;
  void* p32;
  void* p33;
  void* p34;
  void* p35;
  void* p36;
  void* p37;
  void* p38;
  unsigned m_nCurrentMission;
  unsigned GetCurrentMission() const { return m_nCurrentMission; }
};

class CAutoMessage {
  CAutoMessageBase* cntx;
  CLTMsgRef_Read pMsg;
  fearData* pSdk;

 public:
  CAutoMessage();
  void Writeint32(int var);
  void Writeuint64(uint64_t var);
  void Writeuint32(unsigned var);
  void Writeuint8(unsigned char var);
  void Writeuint16(unsigned short var);
  void WriteWString(const wchar_t* pwString);
  void WriteDatabaseRecord(IDatabaseMgr* pLTDatabase, HGEAR hGear);
  void Writefloat(float var);
  void WriteObject(HOBJECT);
  void Writebool(bool);
  void Writebits(unsigned nValue, unsigned nSize);
  void WriteLTVector(const LTVector& vValue);
  template <class T>
  void WriteType(const T& tValue);
  CLTMsgRef_Read Read();
  LTRESULT SendToServer(CLTMsgRef_Read ref);
};
class CGameDatabaseMgr {};

class CWeaponDB : public IDatabaseMgr {};

class CTargetMgr {
 public:
  void* p1;
  void* p2;
  void* p3;
  void* p4;
  void* p5;
  void* p6;
  void* p7;
  void* p8;
  void* p9;
  void* p10;
  HOBJECT m_hLockedTarget;
  virtual void v1();
  virtual void v2();
  virtual HOBJECT GetTargetObject();
  void LockTarget(HOBJECT hTarget) { m_hLockedTarget = hTarget; }
};
class CPlayerCamera {};
class CPlayerMgr {
 public:
  void* p1;
  void* p2;
  void* p3;
  void* p4;
  void* p5;
  void* p6;
  void* p7;
  void* p8;
  // void* p9;
  CTargetMgr* m_pTargetMgr;
  void* p10;
  CPlayerCamera* m_pPlayerCamera;
  void* p12;
  unsigned m_ePlayerState;
  CPlayerCamera* GetPlayerCamera() const { return m_pPlayerCamera; }
  inline unsigned GetPlayerState() { return m_ePlayerState; }
  inline void SetPlayerState(unsigned var) { m_ePlayerState = var; }
  CTargetMgr* GetTargetMgr() const { return m_pTargetMgr; }
};

class CPlayerStats {
 public:
  void* v1;
  unsigned m_nHealth;
  void* v3;
  unsigned m_nMaxHealth;
  unsigned GetHealth() { return m_nHealth; }
  unsigned GetMaxHealth() { return m_nMaxHealth; }
};

class CClientInfoMgr {
 public:
  void* p1;
  void* p2;
  void* p3;
  void* p4;
  void* p5;
  void* p6;
  void* p7;
  void* p8;
  void* p9;
  void* p10;
  void* p11;
  void* p12;
  void* p13;
  void* p14;
  CLIENT_INFO* m_pClients;
  CLIENT_INFO* GetFirstClient() const { return m_pClients; }
};

class CInterfaceMgr {
 public:
  void* p1;
  void* p2;
  unsigned m_eGameState;
  unsigned GetGameState() { return m_eGameState; }
};

class CHUDChatInput {};

class CLTGUIEditCtrl {};

class CHUDChatMsgQueue {};

class CCharacterFX {};
class CPickupItemFX {};
class CSFXMgr {};
class CGameClientShell {};

class CMoveMgr {
 public:
  HOBJECT GetObject() const { return m_hObject; }
  void* v1;
  void* v2;
  void* v3;
  void* v4;
  HOBJECT m_hObject;
  // LTList<Pusher*> m_Pushers;
  void* v5;

  LTVector m_vWantedDims;

  // Movement state.
  unsigned m_dwControlFlags;
  unsigned m_dwLastControlFlags;

  bool m_bBodyInLiquid;
  bool m_bSwimmingOnSurface;
  bool m_bCanSwimJump;

  ContainerCode m_eBodyContainerCode;  // Body container code

  bool m_bLoading;
  LTVector m_vSavedVel;

  // HPOLY     m_hStandingOnPoly;
  void* p6;
  void* p7;
  void* p8;
  SurfaceType m_eStandingOnSurface;
  SurfaceType m_eBodyContainerSurface;
  LTVector m_vGroundNormal;
  bool m_bOnGround;
  bool m_bOnLift;
  bool m_bFalling;
  bool m_bRunLock;
  bool m_bDuckLock;
  bool m_bUsingPlayerModel;
  bool m_bForceToServerPos;
  float m_fBaseMoveAccel;
  float m_fMoveAccelMultiplier;
  float m_fLastOnGroundY;
  bool m_bJumped;
  bool m_bSwimJumped;
  LTVector m_vTotalCurrent;
  float m_fTotalViscosity;
  bool m_bGravityOverride;
  float m_fTotalContainerGravity;
  // Movement speeds.
  float m_fJumpVel;
  float m_fJumpMultiplier;
  float m_fSwimVel;
  float m_fWalkVel;
  float m_fRunVel;
  float m_fCrawlVel;
  float m_fMoveMultiplier;
  float m_fForceIdleVel;
  float m_fGravity;
  bool m_bSwimmingJump;
  bool m_bFirstAniUpdate;
  bool m_bAllowMovement;
  bool m_bWaterAffectsSpeed;
};

class ObjectDetectorLink {
 public:
  void* v1;
  void* v2;
  void* v3;
  HOBJECT m_hObject;
  void* m_pDetector;
  ObjectDetectorLink* m_pPrev;
  ObjectDetectorLink* m_pNext;
};

class CClientWeaponMgr {};

class CClientWeapon {
 public:
  virtual void v1();
  virtual void v2();
  virtual void v3();
  virtual void v4();
  virtual void v5();
  virtual void v6();
  virtual void Init(HWEAPON);
};

class CUserProfile {
 public:
  void* v1;
  void* v2;
  void* v3;
  void* v4;
  void* v5;
  void* v6;
  void* v7;
  void* v8;
  stl::wstring m_sPlayerName;
  stl::string m_sPlayerPatch;
  LTGUID m_PlayerGuid;
  unsigned char m_nDMPlayerModel;
  unsigned char m_nTeamPlayerModel;
};
class CProfileMgr {
 public:
  void* v1;
  CUserProfile m_profile;
  CUserProfile* GetCurrentProfile() { return &m_profile; }
};

#endif /* FEARSDK_H */
