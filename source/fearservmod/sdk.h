#ifndef _SDK_H
#define _SDK_H
#define LTRESULT unsigned
#define HCLIENT void*
#define HOBJECT void*
#define HDATABASE void*
#define HCATEGORY void*
#define HRECORD void*
#define HATTRIBUTE void*
#define HWEAPON void*
#define HWEAPONDATA void*
#define HAMMODATA void*
#define HAMMO void*
#define HMODELANIM uint32_t

#define HLOCALOBJ HOBJECT

#define kGearUse 2
#define kMaxChatHistory 25
#define kMaxChatLength 64
#define TRIGGER 2

#define HGEAR void*

#define GDB_DLL_NAME "GameDatabase"
#define WDB_WEAPON_fRun           "Accuracy.0.Run"
#define WDB_WEAPON_fMovementMultiplier    "MovementMultiplier"
#define WDB_WEAPON_nShotsPerClip      "ShotsPerClip"
#define WDB_WEAPON_fReloadAnimRateScale   "ReloadAnimRateScale"
#define WDB_WEAPON_nFireDelay "FireDelay"
#define WDB_GLOBAL_RECORD         "Global"
#define PLAYER_BUTE_RUNSPEED          "RunSpeed"
#define SrvDB_PlayerCat "Server/Player"
#define SrvDB_Player "Player"

#define MID_ACTIVATE_TURRET     4
#define MESSAGE_GUARANTEED 1
#define USE_AI_DATA true
#define MID_ACTIVATE_LADDER     2
#define MID_SONIC               144
#define MID_WEAPON_CHANGE       129
#define MID_MULTIPLAYER_OPTIONS         148
#define MID_START_LEVEL             185
#define MID_FRAG_SELF             213
#define MID_WEAPON_SOUND             131
#define MID_OBJECT_ALPHA            254
#define MID_RENDER_STIMULUS           251
#define MID_PLAYER_BROADCAST            146
#define MID_STIMULUS              250
#define MID_GAME_PAUSE              160
#define MID_WEAPON_RELOAD       134
#define MID_GAME_UNPAUSE            161
#define MID_PLAYER_TELEPORT       121
#define MID_SOUND_BROADCAST_DB          156
#define MID_DO_DAMAGE             217
#define MID_PLAYER_CLIENTMSG 119
#define MID_PLAYER_GEAR 170
#define MID_DECISION 239
#define MID_PLAYER_EVENT        123
#define MID_WEAPON_FIRE         133
#define MID_VOTE                191
#define MID_CLIENTCONNECTION          245
#define MID_MULTIPLAYER_UPDATE          147
#define MID_PLAYER_UPDATE 100
#define MID_OBJECT_MESSAGE 159
#define MID_SFX_MESSAGE             231
#define MID_PLAYER_RESPAWN 112
#define MID_DROP_GRENADE 128
#define MID_AMMOBOX 1004
#define MID_ADDWEAPON 1005
#define MID_ADDMOD 1008
#define MID_PICKEDUP 1006
#define MID_WEAPON_SOUND_LOOP     132
#define MID_PICKUPITEM_ACTIVATE 171
#define MID_PICKUPITEM_ACTIVATE_EX            172
#define MID_SLOWMO                193
#define MID_PLAYER_ACTIVATE 118
#define MID_PLAYER_INFOCHANGE 103
#define MID_CLEAR_PROGRESSIVE_DAMAGE      247
#define MID_PLAYER_MESSAGE 106
#define MID_PLAYER_GHOSTMESSAGE     125
#define MID_PUNKBUSTER_MSG            201
#define MID_DYNANIMPROP 162
#define USRFLG_CAN_ACTIVATE (1 << 8)
#define USRFLG_MOVEABLE (1 << 5)
#define FLAG_GOTHRUWORLD (1 << 18)
#define FLAG_SOLID (1 << 5)
#define ODBF_ACQUIREMASK 0x00FF0000
#define SFX_PICKUPITEM_ID 10
#define MID_WEAPON_FINISH           151
#define MID_WEAPON_FINISH_RAGDOLL       152
#define MID_DIFFICULTY              240
#define MID_AIDBUG                249
#define MID_ADD_GOAL              252 // Client to server
#define MID_REMOVE_GOAL             253 // Client to server



#define CLIENTUPDATE_3RDPERSON      (1<<0)
#define CLIENTUPDATE_3RDPERVAL      (1<<1)
#define CLIENTUPDATE_ALLOWINPUT     (1<<2)
#define CLIENTUPDATE_CAMERAINFO     (1<<3)
#define CLIENTUPDATE_ANIMATION      (1<<4)

#define FLAG_GRAVITY            (1<<17)

#define LTARRAYSIZE(a) (sizeof(a) / (sizeof((a)[0])))

namespace stl {
struct wstring {
    wchar_t *data;
    int length;
    int capacity;
    wchar_t str[16];
};

struct string {
    char *data;
    int length;
    int capacity;
    char str[16];
};
}  // namespace stl


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


    /*!
FLAG2_ defines.
*/
    enum
{
    // WARNING: only the first 16 bits of FLAG2 are distributed to the
    // client.  If you add a new flag in the high 16 bits, the client will
    // never hear about it!

    //! Use a y-axis aligned cylinder to collide with the BSP.
    FLAG2_PLAYERCOLLIDE =     (1<<0),

    /*!
Don't render this object thru the normal stuff,
only render it when processing sky objects.
*/
    FLAG2_SKYOBJECT =       (1<<1),

    /*!
!forces an object to be considered translucent
during rendering. This should be used when
an object has parts that are translucent, but
overall the alpha is completely opaque.
*/
    FLAG2_FORCETRANSLUCENT =    (1<<2),

    /*!
Disables the client-side prediction on this object
*/
    FLAG2_DISABLEPREDICTION =   (1<<3),

    /*!
Enables transferring of dims to the client
*/
    FLAG2_SERVERDIMS    =   (1<<4),

    /*!
Indicates that objects with this flag are non-solid to other objects with this flag
*/
    FLAG2_SPECIALNONSOLID =   (1<<5),

    /*!
Indicates that objects with this flag should use rigid body simulation
*/
    FLAG2_RIGIDBODY     =   (1<<6),

    /*!
Indicates that this object should be simulated physically on the client and not adhere
to any server simulation data.
*/
    FLAG2_CLIENTRIGIDBODY =   (1<<7),

    /*!
This object behaves as a normal sky object, but should be rendered after the scene
has been drawn on top to allow it to overlay on top of the screen. Typically this is used
for effects such as blinding flares or lens flares in the sky
*/
    FLAG2_SKYOVERLAYOBJECT =    (1<<8),

    //! Steps up stairs.  This flag is ignored unless FLAG2_PLAYERCOLLIDE is set.
    FLAG2_PLAYERSTAIRSTEP   =   (1<<9),

    };


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

    enum CPWeaponStatusTyps
{
    WS_NONE,
    WS_RELOADING,
    WS_BLOCKING,
    WS_SELECT,
    WS_DESELECT,
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

enum VoteAction
{
    eVote_Start,
    eVote_Cast,
    eVote_Pass,
    eVote_Fail,       // too many no votes
    eVote_Expire,     // too much time expired
    eVote_Cancel_Players, // not enough valid players to vote
    eVote_Cancel_InProgress, // another vote is already in progress

    eVote_NoAction,
    kNumVoteActions = eVote_NoAction
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

    enum EClientConnectionState
{
    eClientConnectionState_None,
    eClientConnectionState_Tickle,
    eClientConnectionState_Error,
    eClientConnectionState_Hello,
    eClientConnectionState_KeyChallenge,
    eClientConnectionState_WaitingForAuth,
    eClientConnectionState_Overrides,
    eClientConnectionState_ContentTransfer,
    eClientConnectionState_CRCCheck,
    eClientConnectionState_LoggedIn,
    eClientConnectionState_Loading,
    eClientConnectionState_Loaded,
    eClientConnectionState_InWorld,
    eClientConnectionState_PostLoadWorld,
    };


enum EClientConnectionError
{
    eClientConnectionError_WrongPassword,
    eClientConnectionError_BadCDKey,
    eClientConnectionError_ContentTransferFailed,
    eClientConnectionError_InvalidAssets,
    eClientConnectionError_Banned,
    eClientConnectionError_TimeOut,
    eClientConnectionError_PunkBuster
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

#ifdef __GNUC__
inline float sqrtf_impl(float fVal){
    float result;
    __asm__(
        "fsqrt\n\t"   // st(0) = square root st(0)
        : "=t"(result) : "0"(fVal));
    return result;
}
inline float LTSqrt(float fVal)    {if(fVal >= 0.0f) return sqrtf_impl(fVal);
    else return 1.0f; }

#else
inline float LTSqrt(float fVal)    {if(fVal >= 0.0f) return sqrtf(fVal);
    else return 1.0f; }

#endif



//given two values this will return the larger of the two values using the < operator
template<typename T>
inline T LTMAX_I(T a, T b)          { return ((a < b) ? b : a); }
template<typename T, typename TB>
inline T LTMAX(T a, TB b)         { return LTMAX_I<T>(a, b); }

template<class T>
struct TVector3 {

    T x, y, z;
    TVector3()
    {}
    TVector3( const T x, const T y, const T z )
        : x(x), y(y), z(z)
    {}
    //magnitude of this vector squared
    T MagSqr() const
    {
        return x*x + y*y + z*z;
    }
    //distance to a point squared
    T DistSqr(const TVector3& b) const
    {
        return (*this - b).MagSqr();
    }
    T Mag() const
    {
        return (T)LTSqrt(MagSqr());
    }

    TVector3 Lerp(const TVector3& b, float fT) const
    {
        return TVector3(x + (b.x - x) * fT, y + (b.y - y) * fT, z + (b.z - z) * fT);
    }
    bool NearlyEquals(const TVector3& b, const T r = 0) const
    {
        //within a tolerance
        const TVector3 t = *this - b;//difference
        return t.Dot(t) <= r*r;//radius
    }
    //dot product operator
    T Dot( const TVector3& b ) const
    {
        return x*b.x + y*b.y + z*b.z;
    }

    const T& operator [] ( const int32_t i ) const
    {
        return *((&x) + i);
    }
    T& operator [] ( const int32_t i )
    {
        return *((&x) + i);
    }
    bool operator == ( const TVector3& b ) const
    {
        return (b.x==x && b.y==y && b.z==z);
    }
    bool operator != ( const TVector3& b ) const
    {
        return !(b == *this);
    }
    const TVector3 operator - () const
    {
        return TVector3( -x, -y, -z );
    }

    const TVector3& operator = ( const TVector3& b )
    {
        x = b.x;
        y = b.y;
        z = b.z;

        return *this;
    }
    const TVector3& operator += (const T s)
    {
        x += s;
        y += s;
        z += s;

        return *this;
    }
    const TVector3& operator += (const TVector3& b)
    {
        x += b.x;
        y += b.y;
        z += b.z;

        return *this;
    }
    const TVector3& operator -= (const T s)
    {
        x -= s;
        y -= s;
        z -= s;

        return *this;
    }
    const TVector3& operator -= (const TVector3& b)
    {
        x -= b.x;
        y -= b.y;
        z -= b.z;

        return *this;
    }
    const TVector3& operator *= (const T s)
    {
        x *= s;
        y *= s;
        z *= s;

        return *this;
    }
    const TVector3& operator *= (const TVector3& b)
    {
        x *= b.x;
        y *= b.y;
        z *= b.z;

        return *this;
    }
    const TVector3& operator /= (const T s)
    {
        const T r = 1 / s;//reciprocal

        x *= r;
        y *= r;
        z *= r;

        return *this;
    }
    const TVector3& operator /= (const TVector3& b)
    {
        x /= b.x;
        y /= b.y;
        z /= b.z;

        return *this;
    }
    const TVector3 operator + (const T s) const
    {
        return TVector3(x + s, y + s, z + s);
    }
    const TVector3 operator + (const TVector3& b) const
    {
        return TVector3(x + b.x, y + b.y, z + b.z);
    }
    friend inline const TVector3 operator + (const T s, const TVector3& v)
    {
        return v + s;
    }
    const TVector3 operator - (const T s) const
    {
        return TVector3(x - s, y - s, z - s);
    }
    const TVector3 operator - (const TVector3& b) const
    {
        return TVector3(x - b.x, y - b.y, z - b.z);
    }
    friend inline const TVector3 operator - (const T s, const TVector3& v)
    {
        return -v + s;
    }
    const TVector3 operator * (const T s) const
    {
        return TVector3(x*s, y*s, z*s);
    }
    const TVector3 operator * (const TVector3& b) const
    {
        return TVector3(x*b.x, y*b.y, z*b.z);
    }
    friend inline const TVector3 operator * (const T s, const TVector3& v)
    {
        return v * s;
    }
    const TVector3 operator / (const T s) const
    {
        const T r = 1 / s;//reciprocal

        return TVector3(x*r, y*r, z*r);
    }
    const TVector3 operator / (const TVector3& b) const
    {
        return TVector3(x/b.x, y/b.y, z/b.z);
    }
    //typedef typename TFloatType<T>::Type float_T;
};

struct LTRotation {
    union {
        float x, y, z, w;
        float m_Quat[4];
    };
};

typedef TVector3<float> LTVector;


typedef void(*RequestURLDataCallback);

#define MAX_PLAYER_NAME   16

struct NetClientData
{
    wchar_t m_szName[MAX_PLAYER_NAME];
    LTGUID  m_PlayerGuid;
    uint8_t m_nDMModelIndex;
    uint8_t m_nTeamModelIndex;
};

#define LPAGGREGATE void *
class ILTBaseClass
{
    virtual ~ILTBaseClass();
public:
    LPAGGREGATE m_pFirstAggregate;
    HOBJECT m_hObject;
    uint8_t           m_nType;
};

class SdkHandler;
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
    virtual HRECORD     GetRecord(  HDATABASE hDatabase, const char* pszCategory,const char* pszRecord);
    virtual HRECORD     GetRecordB(HCATEGORY hCategory, const char* pszRecordName); // ms compiler for some reason calling GetRecord with 3 arguments when i call with 2 so i added B letter
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
    virtual uint32_t  GetNumValues( HATTRIBUTE hAttrib );
    virtual void v30();
    virtual void v31();
    virtual __int32 GetInt32(HATTRIBUTE, unsigned nValueIndex = 0, int nDef = 0);
    virtual float     GetFloat(HATTRIBUTE hAttribute, uint32_t nValueIndex, float fDefault);
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

class ILTPhysics {
public:
    virtual void* v1();
    virtual LTRESULT IsWorldObject(HOBJECT hObj);
    virtual LTRESULT GetStairHeight(float &fHeight);
    virtual LTRESULT SetStairHeight(float fHeight);
    virtual LTRESULT GetMass(HOBJECT hObj, float *m);
    virtual LTRESULT SetMass(HOBJECT hObj, float m);
    virtual LTRESULT GetFrictionCoefficient(HOBJECT hObj, float* u);
    virtual LTRESULT SetFrictionCoefficient(HOBJECT hObj, float u);
    virtual LTRESULT GetObjectDims(HOBJECT hObj, LTVector *d);
    virtual LTRESULT SetObjectDims(HOBJECT hObj, LTVector *d, uint32_t flag);
    virtual LTRESULT GetVelocity(HOBJECT hObj, LTVector *v);
    virtual LTRESULT SetVelocity(HOBJECT hObj, LTVector& v) ;
    virtual LTRESULT GetForceIgnoreLimit(HOBJECT hObj, float &f);
    virtual LTRESULT SetForceIgnoreLimit(HOBJECT hObj, float f);
    virtual LTRESULT GetAcceleration(HOBJECT hObj, LTVector *a);
    virtual LTRESULT SetAcceleration(HOBJECT hObj, LTVector& a);
    virtual LTRESULT MoveObject(HOBJECT hObj, LTVector& p, uint32_t flag);
};

class ILTModel {
public:
    virtual void* v1();
    virtual void* v2();
    virtual void* v3();
    virtual void* v4();
    virtual void* v5();
    virtual void* v6();
    virtual void* v7();
    virtual void* v8();
    virtual void* v9();
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
    virtual void* v21();
    virtual void* v22();
    virtual void* v23();
    virtual void* v24();
    virtual void* v25();
    virtual void* v26();
    virtual void* v27();
    virtual LTRESULT GetAnimLength(HOBJECT hModel, HMODELANIM hAnim, uint32_t &length);
    virtual void* v29();
    virtual void* v30();
    virtual void* v31();
    virtual void* v32();
    virtual void* v33();
    virtual void* v34();
    virtual void* v35();
    virtual LTRESULT GetAnimIndex( HOBJECT hModel, const char *pAnimName, uint32_t & anim_index );
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
    virtual void v11();
    virtual void v12();
    virtual void v13();
    virtual void v14();
    virtual void v15();
    virtual void v16();
    virtual void v17();
    virtual ILTModel *GetModelLT();
    virtual void v19();
    virtual void v20();
    virtual ILTCommon *Common();
    virtual ILTPhysics *Physics();
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
    HCLIENT (*GetClientHandle)(uint32_t clientID);
    uint32_t (*GetClientID)(HCLIENT hClient);
    void* p15;
    void* p16;
    void* p17;
    void* p18;
    void* p19;
    void* p20;
    LTRESULT (*KickClient)(HCLIENT hClient);
};


class CAutoMessageBase_Read {
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
    virtual uint32_t ReadBits(uint32_t nBits);
    virtual uint64_t ReadBits64(uint32_t nBits);
    //virtual void v17();
    virtual void ReadData(void *pData, uint32_t nBits);
    virtual void v19();
    virtual uint32_t ReadString(char *pDest, uint32_t nMaxLen);
    virtual uint32_t ReadWString(wchar_t *pwDest, uint32_t nMaxLen);
    //virtual LTVector ReadCompLTVector();
    virtual void ReadCompLTVector(LTVector *);
    virtual LTVector ReadCompPos();
    //virtual LTRotation ReadCompLTRotation();
    virtual void ReadCompLTRotation(LTRotation *);
    virtual HOBJECT ReadObject();
    virtual LTRotation ReadYRotation();
    virtual HRECORD ReadDatabaseRecord( IDatabaseMgr *pDatabase, HCATEGORY hCategory );
};

class CAutoMessageBase_Write {
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
    void * m_CampaignBegin;//typedef std::vector< uint32, LTAllocator<uint32, LT_MEM_TYPE_OBJECTSHELL> > Campaign;
    void * m_CampaignEnd;
    //void* p23;
    //void* p24;
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
    CAutoMessageBase_Write* cntx;
    CLTMsgRef_Read pMsg;
    SdkHandler* pSdk;

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

class CWeaponDB : public IDatabaseMgr {

};

class CServerDB : public IDatabaseMgr{

};

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

class CPlayerObj{
public:
    void * p1;
    void * p2;
    HOBJECT m_hObject;
    static void objcall handlePlayerPositionMessage(CPlayerObj * arg, CAutoMessageBase_Read * pMsg);
    static void objcall updateMovement(CPlayerObj *arg);
};
class CPlayerCamera {};

class ServerVoteMgr {};
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


class GameModeMgr{
public:

};

class ServerSettings
{
public:

    // Saved data: all game types
    stl::string   m_sGameMode;
    bool      m_bUsePassword;
    stl::wstring  m_sServerMessage;
    // Special case server setting that is saved in each gamemode.
    stl::wstring  m_sBriefingOverrideMessage;
    stl::wstring  m_sPassword;
    stl::wstring  m_sScmdPassword;
    bool      m_bAllowScmdCommands;
    uint16_t      m_nPort;
    stl::wstring  m_sBindToAddr;
    uint8_t     m_nBandwidthServer;
    uint16_t      m_nBandwidthServerCustom;
    bool      m_bLANOnly;
    bool      m_bDedicated;
    bool        m_bAllowContentDownload;
    uint32_t      m_nMaxDownloadRatePerClient;
    uint32_t      m_nMaxDownloadRateAllClients;
    uint8_t     m_nMaxSimultaneousDownloads;
    uint32_t      m_nMaxDownloadSize;
    void*   m_sRedirectURLs;
    stl::wstring  m_sContentDownloadMessage;
    bool      m_bEnableScoringLog;
    uint8_t     m_nMaxScoringLogFileAge;

    bool      m_bAllowVote[kNumVoteTypes];
    uint8_t     m_nMinPlayersForVote;
    uint8_t     m_nMinPlayersForTeamVote;
    uint8_t     m_nVoteLifetime;    //seconds
    uint8_t     m_nVoteBanDuration;   //minutes
    uint8_t     m_nVoteDelay;   //seconds


};


class CArsenal{
public:
    virtual void* v1();
    virtual void* v2();
    virtual void* v3();
    virtual void* v4();
    virtual void* v5();
    virtual void* v6();
    virtual void* v7();
    virtual void* v8();
    virtual void* v9();
    virtual void* v10();
    virtual void* v11();
    virtual void* v12();
    virtual void* v13();
    virtual void* v14();
    virtual void* v15();
    virtual void* v16();
    virtual void* v17();
    virtual void* v18();
};


class CWeapon{
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
    //void* p10;
    HOBJECT m_hModelObject;
    void* p11;
    void* p12;
    void* p13;
    void* p14;
    void* p15;
    void* p16;
    int     m_nAmmoInClip;

    float   m_fDamageFactor;  // How much damage is adjusted
    float   m_fRangeAdjust;   // How much the range is adjusted

    float   m_fLifeTime;    // New LifeTime of projectile.

    HWEAPON   m_hWeapon;
    HAMMO   m_hAmmo;

    CArsenal  *m_pArsenal;    // Arsenal the weapon is a part of...

};


class ServerConnectionMgr{};

class GameClientData{};

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
#endif
