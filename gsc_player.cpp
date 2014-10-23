#include "gsc_player.hpp"

#if COMPILE_PLAYER == 1

#if COD2_VERSION == COD2_VERSION_1_0
	int playerStates = 0x086F1480; // search 'winner'
	int sizeOfPlayer = 0x28A4;
#elif COD2_VERSION == COD2_VERSION_1_2
	int playerStates = 0x08705480; // as in game initialisation "------- Game Initializati"
	int sizeOfPlayer = 0x28A4;
	// memset(&playerStates_8705480, 0, 0xA2900u);  
	// then we need a bit math: 0xA2900 / 64 = 0x28A4
#elif COD2_VERSION == COD2_VERSION_1_3
	// 8716558 pointed on that!! and that i found in setorigin() with ida decompiler
	// looked it up and it points to game_initialization_8109096()
	int playerStates = 0x087a2500;
	int sizeOfPlayer = 0x28A4;
#elif COD_VERSION == COD4_1_7
	/*
		memset((void *)0x841F260, 0, 0x9D000u);
		v836f6c4 = 138539616;
		v836f8a4 = *(_DWORD *)(v84bc268 + 12);
		memset((void *)0x84BC3A0, 0, 0xC6100u); // bottom = playerstates
	*/
	int playerStates = 0x084BC3A0;
	int sizeOfPlayer = 0x3184;
#else
	#warning int playerStates int sizeOfPlayer
	int playerStates = NULL;
	int sizeOfPlayer = NULL;
#endif

#if COD2_VERSION == COD2_VERSION_1_0
	int gentities = 0x08665480;
	int gentities_size = 560;
#elif COD2_VERSION == COD2_VERSION_1_2
	int gentities = 0x08679380;
	int gentities_size = 560;
#elif COD2_VERSION == COD2_VERSION_1_3
	int gentities = 0x08716400;
	int gentities_size = 560;
#else
	#warning int gentities int gentities_size
	int gentities = NULL;
	int gentities_size = NULL;
#endif

#define PLAYERSTATE(playerid) (playerStates + playerid * sizeOfPlayer)
#if COD2_VERSION == COD2_VERSION_1_0 || COD2_VERSION == COD2_VERSION_1_2 || COD2_VERSION == COD2_VERSION_1_3
	#define PLAYERSTATE_VELOCITY(playerid) (PLAYERSTATE(playerid) + 0x20)
#elif COD_VERSION == COD4_1_7
	#define PLAYERSTATE_VELOCITY(playerid) (PLAYERSTATE(playerid) + 40)
#else
	#warning NO PLAYERSTATE_VELOCITY!
	#define PLAYERSTATE_VELOCITY(playerid) 0
#endif

void gsc_player_velocity_set(int id) {
	float velocity[3];

	if ( ! stackGetParams("v", &velocity)) {
		printf("scriptengine> wrongs args for gsc_player_velocity_add(vector velocity);\n");
		stackPushUndefined();
		return;
	}

	float *player_0_velocity_x = (float *)(PLAYERSTATE_VELOCITY(id) + 0);
	float *player_0_velocity_y = (float *)(PLAYERSTATE_VELOCITY(id) + 4);
	float *player_0_velocity_z = (float *)(PLAYERSTATE_VELOCITY(id) + 8);
	
	*player_0_velocity_x = velocity[0];
	*player_0_velocity_y = velocity[1];
	*player_0_velocity_z = velocity[2];
	
	stackReturnInt(1);
}

void gsc_player_velocity_add(int id) {
	float velocity[3];

	if ( ! stackGetParams("v", &velocity)) {
		printf("scriptengine> wrongs args for gsc_player_velocity_add(vector velocity);\n");
		stackPushUndefined();
		return;
	}

	float *player_0_velocity_x = (float *)(PLAYERSTATE_VELOCITY(id) + 0);
	float *player_0_velocity_y = (float *)(PLAYERSTATE_VELOCITY(id) + 4);
	float *player_0_velocity_z = (float *)(PLAYERSTATE_VELOCITY(id) + 8);

	*player_0_velocity_x += velocity[0];
	*player_0_velocity_y += velocity[1];
	*player_0_velocity_z += velocity[2];

	stackReturnInt(1);
}

void gsc_get_userinfo(int id)
{
	int info_base = *(int *)0x0842308C;
	int info_size = 0xB1064;
	int entity = info_base + id * info_size;
	char userinfo[1024];
	strncpy(userinfo, (char*)entity + 12, 1024);
	int cur = 0;
	bool iskey = false;
	char keyname[1024];
	int prev = cur;
	char* key;
	if(!stackGetParamString(0, &key))
	{
		printf("First param needs to be a string for get_userinfo\n");
		stackPushUndefined();
		return;
	}
	while(userinfo[cur] != '\0' && cur < 1024)
	{
		if(userinfo[cur] == '\\')
		{
			iskey = !iskey;
			if(!iskey)
			{
				strncpy(keyname, &userinfo[prev + 1], cur - prev - 1);
				keyname[cur - prev - 1] = '\0';
				prev = cur;
			}
			else if(prev != cur)
			{
				if(strcmp(key, keyname) == 0)
				{
					char keyvalue[1024];
					strncpy(keyvalue, &userinfo[prev + 1], cur - prev - 1);
					keyvalue[cur - prev - 1] = '\0';
					stackPushString(keyvalue);
					return;
				}
				prev = cur;
			}
		}
		cur++;
	}
	stackPushUndefined();
	return;
}

void gsc_set_userinfo(int id)
{
	int info_base = *(int *)0x0842308C;
	int info_size = 0xB1064;
	int entity = info_base + id * info_size;
	char userinfo[1024];
	char new_userinfo[1024];
	strncpy(userinfo, (char*)entity + 12, 1024);
	int cur = 0;
	bool iskey = false;
	char keyname[1024];
	int prev = cur;
	char* key;
	char* value;
	if(!stackGetParamString(0, &key))
	{
		printf("First param needs to be a string for get_userinfo\n");
		stackPushUndefined();
		return;
	}
	if(!stackGetParamString(1, &value))
	{
		printf("second param needs to be a string for get_userinfo\n");
		stackPushUndefined();
		return;
	}
	printf("using %s and %s for set_userinfo\n", key, value);
	printf("userinfo is: %s\n", userinfo);
	while(userinfo[cur] != '\0' && cur < 1024)
	{
		new_userinfo[cur] = userinfo[cur];
		printf("adding to userinfo: %c\n", new_userinfo[cur]);
		if(userinfo[cur] == '\\')
		{
			printf("equal to \\\n");
			iskey = !iskey;
			if(!iskey)
			{
				strncpy(keyname, &userinfo[prev + 1], cur - prev - 1);
				keyname[cur - prev - 1] = '\0';
				prev = cur;
			}
			else if(prev != cur)
			{
				if(strcmp(key, keyname) == 0)
				{
					printf("Found key you were looking for at position %d\n", prev + 1);
					strncpy(&new_userinfo[prev + 1], value, strlen(value));
					printf("Current new_userinfO: %s\n", new_userinfo);
					strncpy(&new_userinfo[prev + 1 + strlen(value)], &userinfo[cur], 1024 - strlen(&new_userinfo[prev + 1 + strlen(value)]));
					strncpy((char*)entity + 12, new_userinfo, 1024);
					printf("Final newuserinfo: %s\n", new_userinfo);
					stackPushString(new_userinfo);
					return;
				}
				prev = cur;
			}
		}
		cur++;
	}
	stackPushUndefined();
	return;
}


void gsc_player_velocity_get(int id) {
	//int currentPlayer = playerStates + id * sizeOfPlayer;
	float *vectorVelocity = (float *)PLAYERSTATE_VELOCITY(id); // (currentPlayer + 0x20);
	stackReturnVector(vectorVelocity);	
}

// aimButtonPressed (toggleads or +speed/-speed)
void gsc_player_button_ads(int id) {
	int currentPlayer = playerStates + id * sizeOfPlayer;
	unsigned char *aim_address = (unsigned char *)(currentPlayer + 0x26CD);
	int aimButtonPressed = *aim_address & 0xF0; // just the first 4 bits tell the state
	stackReturnInt(aimButtonPressed);
}

void gsc_player_button_left(int id) {
	#if COD2_VERSION == COD2_VERSION_1_0 || COD2_VERSION == COD2_VERSION_1_2 || COD2_VERSION == COD2_VERSION_1_3
		unsigned char *aim_address = (unsigned char *)(PLAYERSTATE(id) + 0x26FD);
	#elif COD_VERSION == COD4_1_7
		unsigned char *aim_address = (unsigned char *)(PLAYERSTATE(id) + 0x2FA7);
	#else
		#warning unsigned char *aim_address = (unsigned char *)(NULL);
		unsigned char *aim_address = (unsigned char *)(NULL);
	#endif
	int leftButtonPressed = (*aim_address & 0x81)==0x81;
	stackReturnInt(leftButtonPressed);
}

void gsc_player_button_right(int id) {
	#if COD2_VERSION == COD2_VERSION_1_0 || COD2_VERSION == COD2_VERSION_1_2 || COD2_VERSION == COD2_VERSION_1_3
		unsigned char *aim_address = (unsigned char *)(PLAYERSTATE(id) + 0x26FD);
	#elif COD_VERSION == COD4_1_7
		unsigned char *aim_address = (unsigned char *)(PLAYERSTATE(id) + 0x2FA7);
	#else
		#warning unsigned char *aim_address = (unsigned char *)(NULL);
		unsigned char *aim_address = (unsigned char *)(NULL);
	#endif

	int rightButtonPressed = (*aim_address & 0x7F)==0x7F;
	stackReturnInt(rightButtonPressed);
}

void gsc_player_button_forward(int id) {
	#if COD2_VERSION == COD2_VERSION_1_0 || COD2_VERSION == COD2_VERSION_1_2 || COD2_VERSION == COD2_VERSION_1_3
		unsigned char *aim_address = (unsigned char *)(PLAYERSTATE(id) + 0x26FC);
	#elif COD_VERSION == COD4_1_7
		unsigned char *aim_address = (unsigned char *)(PLAYERSTATE(id) + 0x2FA6);
	#else
		#warning unsigned char *aim_address = (unsigned char *)(NULL);
		unsigned char *aim_address = (unsigned char *)(NULL);
	#endif

	int forwardButtonPressed = (*aim_address & 0x7F)==0x7F;
	stackReturnInt(forwardButtonPressed);
}

void gsc_player_button_back(int id) {
	#if COD2_VERSION == COD2_VERSION_1_0 || COD2_VERSION == COD2_VERSION_1_2 || COD2_VERSION == COD2_VERSION_1_3
		unsigned char *aim_address = (unsigned char *)(PLAYERSTATE(id) + 0x26FC);
	#elif COD_VERSION == COD4_1_7
		unsigned char *aim_address = (unsigned char *)(PLAYERSTATE(id) + 0x2FA6);
	#else
		#warning unsigned char *aim_address = (unsigned char *)(NULL);
		unsigned char *aim_address = (unsigned char *)(NULL);
	#endif
	
	int backButtonPressed = (*aim_address & 0x81)==0x81;
	stackReturnInt(backButtonPressed);
}

void gsc_player_button_leanleft(int id) {
	#if COD2_VERSION == COD2_VERSION_1_0 || COD2_VERSION == COD2_VERSION_1_2 || COD2_VERSION == COD2_VERSION_1_3
		unsigned char *aim_address = (unsigned char *)(PLAYERSTATE(id) + 0x26E8);
	#elif COD_VERSION == COD4_1_7
		unsigned char *aim_address = (unsigned char *)(PLAYERSTATE(id) + 0x2FB4);
	#else
		#warning unsigned char *aim_address = (unsigned char *)(NULL);
		unsigned char *aim_address = (unsigned char *)(NULL);
	#endif

	int leanleftButtonPressed = (*aim_address & 0x40)==0x40;
	stackReturnInt(leanleftButtonPressed);
}

void gsc_player_button_leanright(int id) {
	#if COD2_VERSION == COD2_VERSION_1_0 || COD2_VERSION == COD2_VERSION_1_2 || COD2_VERSION == COD2_VERSION_1_3
		unsigned char *aim_address = (unsigned char *)(PLAYERSTATE(id) + 0x26E8);
	#elif COD_VERSION == COD4_1_7
		unsigned char *aim_address = (unsigned char *)(PLAYERSTATE(id) + 0x2FB4);
	#else
		#warning unsigned char *aim_address = (unsigned char *)(NULL);
		unsigned char *aim_address = (unsigned char *)(NULL);
	#endif
	
	int leanrightButtonPressed = (*aim_address & 0x80)==0x80;
	stackReturnInt(leanrightButtonPressed);
}

void gsc_player_button_jump(int id) {
	#if COD2_VERSION == COD2_VERSION_1_0 || COD2_VERSION == COD2_VERSION_1_2 || COD2_VERSION == COD2_VERSION_1_3
		unsigned char *aim_address = (unsigned char *)(PLAYERSTATE(id) + 0x26E9);
	#elif COD_VERSION == COD4_1_7
		unsigned char *aim_address = (unsigned char *)(PLAYERSTATE(id) + 0x2FB5);
	#else
		#warning unsigned char *aim_address = (unsigned char *)(NULL);
		unsigned char *aim_address = (unsigned char *)(NULL);
	#endif
	
	int jumpButtonPressed = (*aim_address & 0x04)==0x04;
	stackReturnInt(jumpButtonPressed);
}

/*
CoD2 = 26E8 == leanleft:40 leanright:80
CoD4 = 26E9 == jump:04
CoD2 = 26FC == forward:7f backward:81
CoD2 = 26FD == left:81 right:7f

CoD4 = 2FA6 == forward:7f backward:81
CoD4 = 2FA7 == left:81 right:7f
CoD4 = 2FB4 == leanleft:40 leanright:80
CoD4 = 2FB5 == jump:04
*/

void gsc_player_stance_get(int id) {
	int entity = gentities + id * gentities_size;
	unsigned char *stance_address = (unsigned char *)(entity + 8);
	int code = *stance_address & 0x0F; // just the last 4 bits tell the state

	char *stance = "";
	switch (code) {
		case  0: stance = "stand"; break; // also in spec
		case  2: stance = "stand"; break;
		case  4: stance = "duck"; break;
		case  6: stance = "duck"; break;
		case  8: stance = "lie"; break;
		case 10: stance = "lie"; break;
		default: printf("unknown stance for player id=%d, code=%d\n", id, code);
	}

	stackPushString(stance);
}

void gsc_player_spectatorclient_get(int id) {
	int entity = playerStates + id * sizeOfPlayer;
	int spectatorClient = *(unsigned char *)(entity + 0xCC);
	
	//printf("spectator client: %x=%d\n", entity, spectatorClient);
	
	// ups, its ALWAYS returning a real id
	// when i have 2 bots, then i got id 2, when i spec "myself" it will return 2, also when i play myself
	//if ( ! spectatorClient)
	//	return stackPushUndefined();
	
	stackPushEntity(gentities + spectatorClient * gentities_size);
}

void gsc_player_getip(int id) {
	#if COD2_VERSION == COD2_VERSION_1_0
		int info_base = *(int *)0x0841FB0C;
		int info_size = 0x78F14;
		int info_ip_offset = 0x6E5C8;
		int info_port_offset = 0x6E5A4;
	#elif COD2_VERSION == COD2_VERSION_1_2
		int info_base = *(int *)0x0842200C;
		int info_size = 0x79064;
		int info_ip_offset = 0x6E6D8;
		int info_port_offset = 0x6E6B4;	
	#elif COD2_VERSION == COD2_VERSION_1_3
		int info_base = *(int *)0x0842308C;
		int info_size = 0xB1064;
		int info_ip_offset = 0x6E6D8;
		int info_port_offset = 0x6E6B4;
	#else
		#warning gsc_player_getip() got no working addresses
		int info_base = *(int *)0x0;
		int info_size = 0x0;
		int info_ip_offset = 0x0;
		int info_port_offset = 0x0;
	#endif
	
	int info_player = info_base + id * info_size;

	int ip_a = *(unsigned char *)(info_player + info_ip_offset + 0);
	int ip_b = *(unsigned char *)(info_player + info_ip_offset + 1); // dafuq, its +1 but in IDA its +4 step :S
	int ip_c = *(unsigned char *)(info_player + info_ip_offset + 2);
	int ip_d = *(unsigned char *)(info_player + info_ip_offset + 3);
	//int port = *(unsigned char *)(info_player + info_ip_offset + 16);

	char tmp[64];
	snprintf(tmp, 64, "%d.%d.%d.%d", ip_a, ip_b, ip_c, ip_d);
	//snprintf(tmp, 64, "%d.%d.%d.%d:%d", ip_a, ip_b, ip_c, ip_d, port);
	
	stackPushString(tmp);
}

void gsc_player_getping(int id) {
	#if COD2_VERSION == COD2_VERSION_1_0
		int info_base = *(int *)0x0841FB0C;
		int info_size = 0x78F14;
		int info_ip_offset = 0x6E5C8;
		int info_port_offset = 0x6E5A4;
	#elif COD2_VERSION == COD2_VERSION_1_2
		int info_base = *(int *)0x0842200C;
		int info_size = 0x79064;
		int info_ip_offset = 0x6E6D8;
		int info_port_offset = 0x6E6B4;
	#elif COD2_VERSION == COD2_VERSION_1_3
		int info_base = *(int *)0x0842308C;
		int info_size = 0xB1064;
		int info_ip_offset = 0x6E6D8;
		int info_port_offset = 0x6E6B4;
	#else
		#warning gsc_player_getip() got no working addresses
		int info_base = *(int *)0x0;
		int info_size = 0x0;
		int info_ip_offset = 0x0;
		int info_port_offset = 0x0;
	#endif
	
	int info_player = info_base + id * info_size;
	int ping = *(unsigned int *)(info_player + info_port_offset);
	stackPushInt(ping);
}

void gsc_player_ClientCommand(int id) {
	stackPushInt(ClientCommand(id));
}

void gsc_player_getLastConnectTime(int id) {
	#if COD2_VERSION == COD2_VERSION_1_0
		int info_start = *(int *)0x0841FB04;
		int info_base = *(int *)0x0841FB0C;
		int info_size = 0x78F14;
		int info_connecttime_offset = 0x20D14;
	#elif COD2_VERSION == COD2_VERSION_1_2
		int info_start = *(int *)0x08422004;
		int info_base = *(int *)0x0842200C;
		int info_size = 0x79064;
		int info_connecttime_offset = 0x20E24;
	#elif COD2_VERSION == COD2_VERSION_1_3
		int info_start = *(int *)0x08423084;
		int info_base = *(int *)0x0842308C;
		int info_size = 0xB1064;
		int info_connecttime_offset = 0x20E24;
	#else
		#warning gsc_player_getLastConnectTime() got no working addresses
		int info_start = *(int *)0x0;
		int info_base = *(int *)0x0;
		int info_size = 0x0;
		int info_connecttime_offset = 0x0;
	#endif

	int info_player = info_base + id * info_size;
	int lastconnect = info_start - *(unsigned int *)(info_player + info_connecttime_offset);
	stackPushInt(lastconnect);
}

void gsc_player_getLastMSG(int id) {
	#if COD2_VERSION == COD2_VERSION_1_0
		int info_start = *(int *)0x0841FB04;
		int info_base = *(int *)0x0841FB0C;
		int info_size = 0x78F14;
		int info_lastmsg_offset = 0x20D10;
	#elif COD2_VERSION == COD2_VERSION_1_2
		int info_start = *(int *)0x08422004;
		int info_base = *(int *)0x0842200C;
		int info_size = 0x79064;
		int info_lastmsg_offset = 0x20E20;
	#elif COD2_VERSION == COD2_VERSION_1_3
		int info_start = *(int *)0x08423084;
		int info_base = *(int *)0x0842308C;
		int info_size = 0xB1064;
		int info_lastmsg_offset = 0x20E20;
	#else
		#warning gsc_player_getlastmsg() got no working addresses
		int info_start = *(int *)0x0;
		int info_base = *(int *)0x0;
		int info_size = 0x0;
		int info_lastmsg_offset = 0x0;
	#endif

	int info_player = info_base + id * info_size;
	int lastmsg = info_start - *(unsigned int *)(info_player + info_lastmsg_offset);
	stackPushInt(lastmsg);
}

// entity functions (could be in own file, but atm not many pure entity functions)

void gsc_entity_setalive(int id) { // as in isAlive?
	int isAlive;

	if ( ! stackGetParams("i", &isAlive)) {
		printf("scriptengine> ERROR: gsc_player_setalive(): param \"isAlive\"[1] has to be an integer!\n");
		stackPushUndefined();
		return;
	}
	
	*(char *)(gentities + gentities_size*id + 353) = isAlive;	
	stackReturnInt(1);
}

void gsc_entity_setbounds(int id) {
	float width, height;

	if ( ! stackGetParams("ff", &width, &height)) {
		printf("scriptengine> ERROR: please specify width and height to gsc_entity_setbounds()\n");
		stackPushUndefined();
		return;
	}

	*(float*)(gentities + gentities_size*id + 280) = height;
	*(float*)(gentities + gentities_size*id + 276) = width;
	*(float*)(gentities + gentities_size*id + 272) = width;
	*(float*)(gentities + gentities_size*id + 264) = -width;
	*(float*)(gentities + gentities_size*id + 260) = -width;
	
	printf("id=%d height=%f width=%f\n", id, height, width);
	stackReturnInt(1);
}

#endif
