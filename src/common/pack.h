/*
 * Packet types
 */

/* Packet types 0-11 are "administrative" */
#define PKT_UNDEFINED		0
#define PKT_VERIFY		1
#define PKT_REPLY		2
#define PKT_PLAY		3
#define PKT_QUIT		4
#define PKT_LEAVE		5
#define PKT_MAGIC		6
#define PKT_RELIABLE		7
#define PKT_ACK			8
#define PKT_TALK		9

#define PKT_START		10
#define PKT_END			11
#define PKT_KEEPALIVE		12


/* Packet types 20-59 are info that is sent to the client */
#define PKT_MAXSTAT		19
#define PKT_PLUSSES		20
#define PKT_AC			21
#define PKT_EXPERIENCE		22
#define PKT_GOLD		23
#define PKT_HP			24
#define PKT_SP			25
#define PKT_CHAR_INFO		26
#define PKT_VARIOUS		27
#define PKT_STAT		28
#define PKT_HISTORY		29

#define PKT_INVEN		30
#define PKT_EQUIP		31
#define PKT_TITLE		32
#define PKT_LEVEL		33
#define PKT_DEPTH		34
#define PKT_FOOD		35
#define PKT_BLIND		36
#define PKT_CONFUSED		37
#define PKT_FEAR		38
#define PKT_POISON		39

#define PKT_STATE		40
#define PKT_LINE_INFO		41
#define PKT_SPEED		42
#define PKT_STUDY		43
#define PKT_CUT			44
#define PKT_STUN		45
#define PKT_MESSAGE		46
#define PKT_CHAR		47
#define PKT_SPELL_INFO		48
#define PKT_FLOOR		49

#define PKT_SPECIAL_OTHER	50
#define PKT_STORE		51
#define PKT_STORE_INFO		52
#define PKT_TARGET_INFO		53
#define PKT_SOUND		54
#define PKT_MINI_MAP		55
#define PKT_PICKUP_CHECK	56
#define PKT_SKILLS		57
#define PKT_PAUSE		58
#define PKT_MONSTER_HEALTH	59


/* Packet types 60-64 are sent from either the client or server */
#define PKT_DIRECTION		60
#define PKT_ITEM		61
#define PKT_SELL		62
#define PKT_PARTY		63
#define PKT_SPECIAL_LINE	64


/* Extra packets */
#define PKT_MIND		67


/* Packet types 70-116 are sent from the client */
#define PKT_WALK		70
#define PKT_RUN			71
#define PKT_TUNNEL		72
#define PKT_AIM_WAND		73
#define PKT_DROP		74
#define PKT_FIRE		75
#define PKT_STAND		76
#define PKT_DESTROY		77
#define PKT_LOOK		78
#define PKT_SPELL		79

#define PKT_OPEN		80
#define PKT_PRAY		81
#define PKT_QUAFF		82
#define PKT_READ		83
#define PKT_SEARCH		84
#define PKT_TAKE_OFF		85
#define PKT_USE			86
#define PKT_THROW		87
#define PKT_WIELD		88
#define PKT_ZAP			89

#define PKT_TARGET		90
#define PKT_INSCRIBE		91
#define PKT_UNINSCRIBE		92
#define PKT_ACTIVATE		93
#define PKT_BASH		94
#define PKT_DISARM		95
#define PKT_EAT			96
#define PKT_FILL		97
#define PKT_LOCATE		98
#define PKT_MAP			99

#define PKT_SEARCH_MODE		100	
#define PKT_CLOSE		103
#define PKT_GAIN		104
#define PKT_GO_UP		105
#define PKT_GO_DOWN		106
#define PKT_PURCHASE		107
#define PKT_STORE_LEAVE		108
#define PKT_STORE_CONFIRM	109

#define PKT_DROP_GOLD		110
#define PKT_REDRAW		111
#define PKT_REST		112
#define PKT_GHOST		113
#define PKT_SUICIDE		114
#define PKT_STEAL		115
#define PKT_OPTIONS		116
#define PKT_TARGET_FRIENDLY	117
#define PKT_MASTER		118 /* dungeon master commands */
#define PKT_AUTOPHASE		119 /* automatically try to phase */

/* Packet types 121-123 are more administrative stuff */
#define PKT_FAILURE		121
#define PKT_SUCCESS		122
#define PKT_CLEAR		123

/* Packet type 150 are hacks */
#define PKT_FLUSH		150

/* Extra packets */
#define PKT_OBSERVE		160



/*
 * Possible error codes returned
 */
#define SUCCESS		0x00
#define E_VERSION	0x01
#define E_GAME_FULL	0x02
#define E_NEED_INFO	0x03
#define E_TWO_PLAYERS	0x04
#define E_IN_USE	0x08
#define E_SOCKET	0x09
#define E_INVAL		0x0A


/*
 * Some commands send to the server
 */
#define ENTER_GAME_pack	0x00
#define CONTACT_pack	0x31
