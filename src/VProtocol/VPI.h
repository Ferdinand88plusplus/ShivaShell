#pragma once

#include <memory.h>
// Vangers Packet Header type
typedef unsigned char vph_t;

// Vangers Packet Size type
typedef unsigned short vpsz_t;

//! FROM VANGERS SOURCE CODE

/*
		Client - Server interface header
*/

/*******************************************************************************
			Events
*******************************************************************************/
// Event's flags 
#define AUXILIARY_EVENT 0x80 
#define ECHO_EVENT 0x20

/// Client <-> Server object packets
const vph_t  CREATE_OBJECT = 0x02; //znfo 8-| CREATE_OBJECT eq CREATE_PERMANENT_OBJECT
const vph_t  CREATE_PERMANENT_OBJECT = 0x02;
const vph_t  DELETE_OBJECT = 0x04;
const vph_t  UPDATE_OBJECT = 0x08;
const vph_t  HIDE_OBJECT = 0x0C;

/// Client -> Server packets
const vph_t  GAMES_LIST_QUERY = 0x81;
const vph_t  TOP_LIST_QUERY = 0x82; // MP_game(1)

const vph_t  ATTACH_TO_GAME = 0x83; // Game(4)
const vph_t  RESTORE_CONNECTION = 0x84; // Game(4), ClientID(1)
const vph_t  CLOSE_SOCKET = 0x86;

const vph_t  REGISTER_NAME = 0x88; // Name(str), Password(str)
const vph_t  SERVER_TIME_QUERY = 0x89;

const vph_t  SET_WORLD = 0x8B; // World(1), world_y_size(2)
const vph_t  LEAVE_WORLD = 0x8C;
const vph_t  SET_POSITION = 0x8D; // x(2),y(2), y_half_size_of_screen(2)

const vph_t  TOTAL_PLAYERS_DATA_QUERY = 0x91;

const vph_t  SET_GAME_DATA = 0x92; // MP_game(1), GameData(Name(str), ServerData(struct))
const vph_t  GET_GAME_DATA = 0x93;

const vph_t  SET_PLAYER_DATA = 0x94; //  PlayerData
const vph_t  DIRECT_SENDING = 0x95; //  Mask(4), SendData

/// Server -> Client packets
// Responses to queries and other
const vph_t  GAMES_LIST_RESPONSE = 0xC1; // Number(1),{ game_ID(1), game_Name(str) } 
const vph_t  TOP_LIST_RESPONSE = 0xC2; // MP_game(1), Number(1),{ Name(str), rating(float) } 
const vph_t  TOTAL_LIST_OF_PLAYERS_DATA = 0xCC; // Number(1),{ client_ID(1), BodySize(2) , Body, Name(str) } 

const vph_t  ATTACH_TO_GAME_RESPONSE = 0xC3; // Game(4), Configured(1) = 1/0, GameBirthTime(4), Client_ID(1), object_ID_offsets[16](short) 
const vph_t  RESTORE_CONNECTION_RESPONSE = 0xC4; //  Log(1) = 1/0

const vph_t  SERVER_TIME = 0xC6; // time(4)
const vph_t  SERVER_TIME_RESPONSE = 0xC7; // time(4)

const vph_t  SET_WORLD_RESPONSE = 0xC8; // World(1), Response(1) = 1/0

const vph_t  GAME_DATA_RESPONSE = 0xCD; // GameData
const vph_t  DIRECT_RECEIVING = 0xCE; // ReceiveData

// Auxiliaries
const vph_t  PLAYERS_NAME = 0xD5; // Client(1), string
const vph_t  PLAYERS_POSITION = 0xCF; // Client(1),x(2),y(2), only for current world
const vph_t  PLAYERS_WORLD = 0xD1; // Client(1),world(1), for all
const vph_t  PLAYERS_STATUS = 0xD2; // Client(1),status(1), for all
const vph_t  PLAYERS_DATA = 0xD3; // Client(1),data,  for all
const vph_t  PLAYERS_RATING = 0xD4; // Client(1),rating(float), for me

/// zMod packets
const vph_t  zSERVER_VERSION_RESPONSE = 0xE1; // version(4)
const vph_t  zGAME_DATA_RESPONSE = 0xE2; // mod_id(4)
const vph_t  zTIME_RESPONSE = 0xE3; // time(4)

/*******************************************************************************
				Game's data
*******************************************************************************/
// MultiPlayer Games
enum {
	UNCONFIGURED = -1,
	VAN_WAR,
	MECHOSOMA,
	PASSEMBLOSS,
	NUMBER_MP_GAMES,

	HUNTAGE,  // Some people're still using it.
	MUSTODONT
};

// Unique Mechouses
enum {
	LastMoggy,
	QueenFrog,
	Sandoll,
	LawnMower,
	Wormaster
};

struct VAN_WAR_PRM {
	int InitialCash;
	int ArtefactsUsing;
	int InEscaveTime;
	int Color;

	int Nascency;  // Bit-wise using
	int TeamMode;
	int WorldAccess;	// 0 - all worlds, 1 - one world...
	int MaxKills;
	unsigned int MaxTime;
};

struct MECHOSOMA_PRM {
	int InitialCash;
	int ArtefactsUsing;
	int InEscaveTime;
	int Color;

	int World;
	int ProductQuantity1;
	int ProductQuantity2;
	int One_at_a_time;
	int TeamMode;
};

struct PASSEMBLOSS_PRM {
	int InitialCash;
	int ArtefactsUsing;
	int InEscaveTime;
	int Color;

	int CheckpointsNumber;

	int RandomEscave;
};

struct HUNTAGE_PRM {
	int InitialCash;
	int ArtefactsUsing;
	int InEscaveTime;
	int Color;
};

struct MUSTODONT_PRM {
	int InitialCash;
	int ArtefactsUsing;
	int InEscaveTime;
	int Color;

	int UniqueMechosName;
	int TeamMode;
};

struct ServerData {
	int InitialRND;
	int GameType;
	union {
		VAN_WAR_PRM	       Van_War;
		MECHOSOMA_PRM	   Mechosoma;
		PASSEMBLOSS_PRM    Passembloss;
		HUNTAGE_PRM		Huntage;
		MUSTODONT_PRM	    Mustodont;
	};

	ServerData() {};
};

/*******************************************************************************
			Player's data
*******************************************************************************/
struct VanVarStatistic
{
	int MaxLiveTime;//None
	int MinLiveTime;//None
	int KillFreq;//hh:mm:ss - Средний период убийств
	int DeathFreq;//hh:mm:ss - Средний период смертей
};

struct MechosomaStatistic
{
	int ItemCount1, ItemCount2;
	int MaxTransitTime;//hh:mm:ss - Время окончания гонки
	int MinTransitTime;//None
	int SneakCount;//Кол-во украденного товара
	int LostCount;//Кол-во потерянного товара
};

struct PassemblossStatistic
{
	int TotalTime;//hh:mm:ss
	int CheckpointLighting;
	int MinTime;//None
	int MaxTime;//None
};

struct MustodontStatistic
{
	int PartTime1;
	int PartTime2;
	int BodyTime;
	int MakeTime;
};

struct PlayerBody {
	unsigned char kills;
	unsigned char deaths;
	unsigned char color;
	unsigned char world;
	unsigned int beebos;
	float rating;

	unsigned char CarIndex;
	short Data0, Data1;
	unsigned int BirthTime;
	int NetID;

	union {
		VanVarStatistic VanVarStat;
		MechosomaStatistic MechosomaStat;
		PassemblossStatistic PassemblossStat;
		MustodontStatistic MustodontStat;
	};

	void clear() { memset(this, 0, sizeof(PlayerBody)); CarIndex = 255; }
	PlayerBody() { clear(); }
};
