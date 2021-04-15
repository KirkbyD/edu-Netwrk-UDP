#include "cBuffer.h"

#include <glm/vec3.hpp>
#include <glm/gtx/quaternion.hpp>

#define INT_SIZE sizeof(int32_t)/sizeof(char)
#define FLOAT_SIZE sizeof(float)/sizeof(char)
#define PLAYER_SIZE (INT_SIZE + FLOAT_SIZE * 9)
#define BULLET_SIZE (INT_SIZE + FLOAT_SIZE * 6)
#define VEC3_SIZE (FLOAT_SIZE * 3)

struct Player {
	int state; // [IS_ACTIVE, IS_CONNECTED, HAS_AMMO]
	glm::vec3 position;
	glm::vec3 velocity;
	glm::vec3 orientation;
};

struct Bullet {
	int state; // [ACTIVE]
	glm::vec3 position;
	glm::vec3 velocity;
};

enum MESSAGE_ID {
	JOIN, LEAVE, SEND, RECIEVE,
	REGISTER, EMAILAUTH, USERNAMEAUTH,
	REGISTERSUCCESS, REGISTERFAILURE,
	AUTHSUCCESS, AUTHFAILURE,
	DISCONNECT, DISCSUCCESS, DISCFAILURE,
	USERINPUT, GAMESCENE
};

enum INPUT_ID { FORWARD, BACKWARD, LEFT, RIGHT, BRAKE, ORIENT, SHOOT, RESPAWN };

enum PLAYER_STATES { IS_CONNECTED, IS_ACTIVE, HAS_AMMO };

class Protocol
{
private:
	Buffer buffer;

public:
	Protocol()
	{
		buffer = Buffer(0);
	}

	Protocol(unsigned int buffer_size)
	{
		buffer = Buffer(buffer_size);
	}

	//change to using bitsets. Called in client based on recieved enum
	std::vector<uint8_t> GetBuffer()
	{
		return buffer.GetBufferContent();
	}

	void UserSendMessage(std::string room, std::string message)
	{
		buffer.Clear();

		// [Header] [length] [room_name] [length] [message]

		//packet length
		buffer.writeInt32LE(0, SwapIntEndian(INT_SIZE * 4 + room.length() + message.length()));
		//message_id
		buffer.writeInt32LE(INT_SIZE, SwapIntEndian(SEND));

		//room
		buffer.writeInt32LE(INT_SIZE * 2, SwapIntEndian(room.length()));
		buffer.WriteString(INT_SIZE * 3, room);

		//message
		buffer.writeInt32LE(INT_SIZE * 3 + room.length(), SwapIntEndian(message.length()));
		buffer.WriteString(INT_SIZE * 4 + room.length(), message);

		return;
	}

	void UserRecieveMessage(std::string name, std::string room, std::string message)
	{
		buffer.Clear();

		// [Header] [length] [name] [length] [room_name] [length] [message]

		//packet length
		buffer.writeInt32LE(0, SwapIntEndian(INT_SIZE * 5 + name.length() + room.length() + message.length()));
		//message_id
		buffer.writeInt32LE(INT_SIZE, SwapIntEndian(RECIEVE));

		//name
		buffer.writeInt32LE(INT_SIZE * 2, SwapIntEndian(name.length()));
		buffer.WriteString(INT_SIZE * 3, name);

		//room
		buffer.writeInt32LE(INT_SIZE * 3 + name.length(), SwapIntEndian(room.length()));
		buffer.WriteString(INT_SIZE * 4 + name.length(), room);

		//message
		buffer.writeInt32LE(INT_SIZE * 4 + room.length() + name.length(), SwapIntEndian(message.length()));
		buffer.WriteString(INT_SIZE * 5 + room.length() + name.length(), message);

		return;
	}

	void UserJoinRoom(std::string room)
	{
		buffer.Clear();

		// [Header] [length] [room_name]

		//packet length
		buffer.writeInt32LE(0, SwapIntEndian(INT_SIZE * 3 + room.length()));
		//message_id
		buffer.writeInt32LE(INT_SIZE, SwapIntEndian(JOIN));

		//room
		buffer.writeInt32LE(INT_SIZE * 2, SwapIntEndian(room.length()));
		buffer.WriteString(INT_SIZE * 3, room);

		return;
	}

	void UserLeaveRoom(std::string room)
	{
		buffer.Clear();

		// [Header] [length] [room_name]

		//packet length
		buffer.writeInt32LE(0, INT_SIZE * SwapIntEndian(3 + room.length()));
		//message_id
		buffer.writeInt32LE(INT_SIZE, SwapIntEndian(LEAVE));

		//room
		buffer.writeInt32LE(INT_SIZE * 2, SwapIntEndian(room.length()));
		buffer.WriteString(INT_SIZE * 3, room);

		return;
	}

	void UserRegister(std::string username, std::string email, std::string password) {
		buffer.Clear();

		// [Header] [length] [email] [length] [password]

		//packet length
		buffer.writeInt32LE(0, SwapIntEndian(INT_SIZE * 5 + username.length() + email.length() + password.length()));
		//message_id
		buffer.writeInt32LE(INT_SIZE, SwapIntEndian(REGISTER));

		//username
		buffer.writeInt32LE(INT_SIZE * 2, SwapIntEndian(username.length()));
		buffer.WriteString(INT_SIZE * 3, username);

		//email
		buffer.writeInt32LE(INT_SIZE * 3 + username.length(), SwapIntEndian(email.length()));
		buffer.WriteString(INT_SIZE * 4 + username.length(), email);

		//password
		buffer.writeInt32LE(INT_SIZE * 4 + username.length() + email.length(), SwapIntEndian(password.length()));
		buffer.WriteString(INT_SIZE * 5 + username.length() + email.length(), password);

		return;
	}

	void UserEmailAuthenticate(std::string email, std::string password) {
		buffer.Clear();

		// [Header] [length] [email] [length] [password]

		//packet length
		buffer.writeInt32LE(0, SwapIntEndian(INT_SIZE * 4 + email.length() + password.length()));
		//message_id
		buffer.writeInt32LE(INT_SIZE, SwapIntEndian(EMAILAUTH));

		//room
		buffer.writeInt32LE(INT_SIZE * 2, SwapIntEndian(email.length()));
		buffer.WriteString(INT_SIZE * 3, email);

		//message
		buffer.writeInt32LE(INT_SIZE * 3 + email.length(), SwapIntEndian(password.length()));
		buffer.WriteString(INT_SIZE * 4 + email.length(), password);

		return;
	}

	void UserNameAuthenticate(std::string email, std::string password) {
		buffer.Clear();

		// [Header] [length] [email] [length] [password]

		//packet length
		buffer.writeInt32LE(0, SwapIntEndian(INT_SIZE * 4 + email.length() + password.length()));
		//message_id
		buffer.writeInt32LE(INT_SIZE, SwapIntEndian(USERNAMEAUTH));

		//email
		buffer.writeInt32LE(INT_SIZE * 2, SwapIntEndian(email.length()));
		buffer.WriteString(INT_SIZE * 3, email);

		//password
		buffer.writeInt32LE(INT_SIZE * 3 + email.length(), SwapIntEndian(password.length()));
		buffer.WriteString(INT_SIZE * 4 + email.length(), password);

		return;
	}

	void UserDisconnect() {
		buffer.Clear();

		// [Header]

		//packet length
		buffer.writeInt32LE(0, SwapIntEndian(INT_SIZE * 2));
		//message_id
		buffer.writeInt32LE(INT_SIZE, SwapIntEndian(DISCONNECT));

		return;
	}

	void ServerRegister(std::string message)
	{
		buffer.Clear();

		// [Header] [length] [message]

		//packet length
		buffer.writeInt32LE(0, SwapIntEndian(INT_SIZE * 3 + message.length()));
		//message_id
		buffer.writeInt32LE(INT_SIZE, SwapIntEndian(REGISTER));

		//message
		buffer.writeInt32LE(INT_SIZE * 2, SwapIntEndian(message.length()));
		buffer.WriteString(INT_SIZE * 3, message);

		return;
	}

	void ServerRegSuccess(std::string message)
	{
		buffer.Clear();

		// [Header] [length] [message]

		//packet length
		buffer.writeInt32LE(0, SwapIntEndian(INT_SIZE * 3 + message.length()));
		//message_id
		buffer.writeInt32LE(INT_SIZE, SwapIntEndian(REGISTERSUCCESS));

		//message
		buffer.writeInt32LE(INT_SIZE * 2, SwapIntEndian(message.length()));
		buffer.WriteString(INT_SIZE * 3, message);

		return;
	}

	void ServerRegFail(std::string message)
	{
		buffer.Clear();

		// [Header] [length] [message]

		//packet length
		buffer.writeInt32LE(0, SwapIntEndian(INT_SIZE * 3 + message.length()));
		//message_id
		buffer.writeInt32LE(INT_SIZE, SwapIntEndian(REGISTERFAILURE));

		//message
		buffer.writeInt32LE(INT_SIZE * 2, SwapIntEndian(message.length()));
		buffer.WriteString(INT_SIZE * 3, message);

		return;
	}

	void ServerEmailAuthenticate(std::string message)
	{
		buffer.Clear();

		// [Header] [length] [message]

		//packet length
		buffer.writeInt32LE(0, SwapIntEndian(INT_SIZE * 3 + message.length()));
		//message_id
		buffer.writeInt32LE(INT_SIZE, SwapIntEndian(EMAILAUTH));

		//message
		buffer.writeInt32LE(INT_SIZE * 2, SwapIntEndian(message.length()));
		buffer.WriteString(INT_SIZE * 3, message);

		return;
	}

	void ServerNameAuthenticate(std::string message)
	{
		buffer.Clear();

		// [Header] [length] [message]

		//packet length
		buffer.writeInt32LE(0, SwapIntEndian(INT_SIZE * 3 + message.length()));
		//message_id
		buffer.writeInt32LE(INT_SIZE, SwapIntEndian(USERNAMEAUTH));

		//message
		buffer.writeInt32LE(INT_SIZE * 2, SwapIntEndian(message.length()));
		buffer.WriteString(INT_SIZE * 3, message);

		return;
	}

	void ServerAuthSuccess(std::string message)
	{
		buffer.Clear();

		// [Header] [length] [message]

		//packet length
		buffer.writeInt32LE(0, SwapIntEndian(INT_SIZE * 3 + message.length()));
		//message_id
		buffer.writeInt32LE(INT_SIZE, SwapIntEndian(AUTHSUCCESS));

		//message
		buffer.writeInt32LE(INT_SIZE * 2, SwapIntEndian(message.length()));
		buffer.WriteString(INT_SIZE * 3, message);

		return;
	}

	void ServerAuthFailure(std::string message)
	{
		buffer.Clear();

		// [Header] [length] [message]

		//packet length
		buffer.writeInt32LE(0, SwapIntEndian(INT_SIZE * 3 + message.length()));
		//message_id
		buffer.writeInt32LE(INT_SIZE, SwapIntEndian(AUTHFAILURE));

		//message
		buffer.writeInt32LE(INT_SIZE * 2, SwapIntEndian(message.length()));
		buffer.WriteString(INT_SIZE * 3, message);

		return;
	}

	void ServerDisconnect(std::string message)
	{
		buffer.Clear();

		// [Header] [length] [message]

		//packet length
		buffer.writeInt32LE(0, SwapIntEndian(INT_SIZE * 3 + message.length()));
		//message_id
		buffer.writeInt32LE(INT_SIZE, SwapIntEndian(DISCONNECT));

		//message
		buffer.writeInt32LE(INT_SIZE * 2, SwapIntEndian(message.length()));
		buffer.WriteString(INT_SIZE * 3, message);

		return;
	}

	void ServerDiscSuccess(std::string message)
	{
		buffer.Clear();

		// [Header] [length] [message]

		//packet length
		buffer.writeInt32LE(0, SwapIntEndian(INT_SIZE * 3 + message.length()));
		//message_id
		buffer.writeInt32LE(INT_SIZE, SwapIntEndian(DISCSUCCESS));

		//message
		buffer.writeInt32LE(INT_SIZE * 2, SwapIntEndian(message.length()));
		buffer.WriteString(INT_SIZE * 3, message);

		return;
	}

	void ServerDiscFailure(std::string message)
	{
		buffer.Clear();

		// [Header] [length] [message]

		//packet length
		buffer.writeInt32LE(0, SwapIntEndian(INT_SIZE * 3 + message.length()));
		//message_id
		buffer.writeInt32LE(INT_SIZE, SwapIntEndian(DISCFAILURE));

		//message
		buffer.writeInt32LE(INT_SIZE * 2, SwapIntEndian(message.length()));
		buffer.WriteString(INT_SIZE * 3, message);

		return;
	}

	void UserInput(int id, std::vector<int> v_inputs, std::vector<glm::vec3> v_orientations) {

		buffer.Clear();

		// [Header] [length] [id] [input]

		//packet length
		buffer.writeInt32LE(0, SwapIntEndian(INT_SIZE * 5 + INT_SIZE * v_inputs.size() + VEC3_SIZE * v_orientations.size()));
		//message_id
		buffer.writeInt32LE(INT_SIZE, SwapIntEndian(USERINPUT));

		//id
		buffer.writeInt32LE(INT_SIZE * 2, SwapIntEndian(id));

		//v_inputs (int input)[FORWARD, BACKWARD, TURN_LEFT, TURN_RIGHT, FIRE]
		buffer.writeInt32LE(INT_SIZE * 3, SwapIntEndian(v_inputs.size()));
		for (size_t i = 0; i < v_inputs.size(); i++) {
			buffer.writeInt32LE(INT_SIZE * 4 + (i * INT_SIZE) + (0), SwapIntEndian(v_inputs[i]));
		}

		//v_orientations (glm::vec3 orientation)
		buffer.writeInt32LE((INT_SIZE * 4 + (v_inputs.size() * INT_SIZE)), SwapIntEndian(v_orientations.size()));
		for (size_t i = 0; i < v_orientations.size(); i++)
		{
			buffer.writeFloatLE(INT_SIZE * 5 + (v_inputs.size() * INT_SIZE) + (i * VEC3_SIZE) + (0), SwapFloatEndian(v_orientations[i].x));
			buffer.writeFloatLE(INT_SIZE * 5 + (v_inputs.size() * INT_SIZE) + (i * VEC3_SIZE) + (FLOAT_SIZE), SwapFloatEndian(v_orientations[i].y));
			buffer.writeFloatLE(INT_SIZE * 5 + (v_inputs.size() * INT_SIZE) + (i * VEC3_SIZE) + (FLOAT_SIZE * 2), SwapFloatEndian(v_orientations[i].z));
		}
		return;
	}

	void GameScene(int id, std::vector<Player> v_players, std::vector<Bullet> v_bullets, int player_num) {

		buffer.Clear();

		// [Header] [length] [amount of players] [players] [amount of bulletss] [bullets]

		//packet length
		buffer.writeInt32LE(0, SwapIntEndian(INT_SIZE * 7 + PLAYER_SIZE * v_players.size() + BULLET_SIZE * v_bullets.size()));
		//message_id
		buffer.writeInt32LE(INT_SIZE, SwapIntEndian(GAMESCENE));

		//id
		buffer.writeInt32LE(INT_SIZE * 2, SwapIntEndian(id));

		//v_players (int state, glm::vec3 position, glm::vec3 velocity, glm::vec3 orientation)
		buffer.writeInt32LE(INT_SIZE * 3, SwapIntEndian(v_players.size()));
		for (size_t i = 0; i < v_players.size(); i++)
		{
			buffer.writeInt32LE(INT_SIZE * 4 + (i * PLAYER_SIZE) + (0), SwapIntEndian(v_players[i].state));
			buffer.writeFloatLE(INT_SIZE * 4 + (i * PLAYER_SIZE) + (INT_SIZE), SwapFloatEndian(v_players[i].position.x));
			buffer.writeFloatLE(INT_SIZE * 4 + (i * PLAYER_SIZE) + (INT_SIZE + FLOAT_SIZE), SwapFloatEndian(v_players[i].position.y));
			buffer.writeFloatLE(INT_SIZE * 4 + (i * PLAYER_SIZE) + (INT_SIZE + FLOAT_SIZE * 2), SwapFloatEndian(v_players[i].position.z));
			buffer.writeFloatLE(INT_SIZE * 4 + (i * PLAYER_SIZE) + (INT_SIZE + FLOAT_SIZE * 3), SwapFloatEndian(v_players[i].velocity.x));
			buffer.writeFloatLE(INT_SIZE * 4 + (i * PLAYER_SIZE) + (INT_SIZE + FLOAT_SIZE * 4), SwapFloatEndian(v_players[i].velocity.y));
			buffer.writeFloatLE(INT_SIZE * 4 + (i * PLAYER_SIZE) + (INT_SIZE + FLOAT_SIZE * 5), SwapFloatEndian(v_players[i].velocity.z));
			buffer.writeFloatLE(INT_SIZE * 4 + (i * PLAYER_SIZE) + (INT_SIZE + FLOAT_SIZE * 6), SwapFloatEndian(v_players[i].orientation.x));
			buffer.writeFloatLE(INT_SIZE * 4 + (i * PLAYER_SIZE) + (INT_SIZE + FLOAT_SIZE * 7), SwapFloatEndian(v_players[i].orientation.y));
			buffer.writeFloatLE(INT_SIZE * 4 + (i * PLAYER_SIZE) + (INT_SIZE + FLOAT_SIZE * 8), SwapFloatEndian(v_players[i].orientation.z));
		}

		//v_bullets (int state, glm::vec3 position, glm::vec3 velocity)
		buffer.writeInt32LE((INT_SIZE * 4 + (v_players.size() * PLAYER_SIZE)), SwapIntEndian(v_bullets.size()));
		for (size_t i = 0; i < v_bullets.size(); i++)
		{
			buffer.writeInt32LE(INT_SIZE * 5 + (v_players.size() * PLAYER_SIZE) + (i * BULLET_SIZE) + (0), SwapIntEndian(v_bullets[i].state));
			buffer.writeFloatLE(INT_SIZE * 5 + (v_players.size() * PLAYER_SIZE) + (i * BULLET_SIZE) + (INT_SIZE), SwapFloatEndian(v_bullets[i].position.x));
			buffer.writeFloatLE(INT_SIZE * 5 + (v_players.size() * PLAYER_SIZE) + (i * BULLET_SIZE) + (INT_SIZE + FLOAT_SIZE), SwapFloatEndian(v_bullets[i].position.y));
			buffer.writeFloatLE(INT_SIZE * 5 + (v_players.size() * PLAYER_SIZE) + (i * BULLET_SIZE) + (INT_SIZE + FLOAT_SIZE * 2), SwapFloatEndian(v_bullets[i].position.z));
			buffer.writeFloatLE(INT_SIZE * 5 + (v_players.size() * PLAYER_SIZE) + (i * BULLET_SIZE) + (INT_SIZE + FLOAT_SIZE * 3), SwapFloatEndian(v_bullets[i].velocity.x));
			buffer.writeFloatLE(INT_SIZE * 5 + (v_players.size() * PLAYER_SIZE) + (i * BULLET_SIZE) + (INT_SIZE + FLOAT_SIZE * 4), SwapFloatEndian(v_bullets[i].velocity.y));
			buffer.writeFloatLE(INT_SIZE * 5 + (v_players.size() * PLAYER_SIZE) + (i * BULLET_SIZE) + (INT_SIZE + FLOAT_SIZE * 5), SwapFloatEndian(v_bullets[i].velocity.z));
		}

		//what player this message is directed at
		buffer.writeInt32LE(INT_SIZE * 5 + (v_players.size() * PLAYER_SIZE) + (v_bullets.size() * BULLET_SIZE), SwapIntEndian(INT_SIZE));
		buffer.writeInt32LE(INT_SIZE * 6 + (v_players.size() * PLAYER_SIZE) + (v_bullets.size() * BULLET_SIZE), SwapIntEndian(player_num));

		return;
	}

	int SwapIntEndian(int value)
	{
		// We need to grab the first byte an move it to the last
		// Bytes in order: A B C D
		char A = value >> 24;
		char B = value >> 16;
		char C = value >> 8;
		char D = value;

		// OR the data into our swapped variable
		int swapped = 0;
		swapped |= D << 24;
		swapped |= C << 16;
		swapped |= B << 8;
		swapped |= A;

		return swapped;
	}

	float SwapFloatEndian(float value)
	{
		// We need to grab the first byte an move it to the last
		// Bytes in order: A B C D
		unsigned char const* p = reinterpret_cast<unsigned char const*>(&value);
		char A = p[3];
		char B = p[2];
		char C = p[1];
		char D = p[0];

		// OR the data into our swapped variable
		float swapped = 0;
		uint8_t b[] = { D,C,B,A };
		memcpy(&swapped, &b, sizeof(swapped));

		return swapped;
	}

	short SwapShortEndian(int value)
	{
		// We need to grab the first byte an move it to the last
		// Bytes in order: A B
		char A = value >> 8;
		char B = value;

		// OR the data into our swapped variable
		int swapped = 0;
		swapped |= B << 8;
		swapped |= A;

		return swapped;
	}

	void ClearBuffer()
	{
		buffer.Clear();
	}
};