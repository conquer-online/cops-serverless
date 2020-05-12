//
//  Copyright (c) CptSky <cptsky@me.com>
//  All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions are met:
//
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//     * Neither the name of the organization nor the names of its contributors
//       may be used to endorse or promote products derived from this software
//       without specific prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
//  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
//  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
//  DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
//  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
//  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
//  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
//  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
//  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//

#ifndef ZFSERVER_NETWORK_MSG_ACTION_H
#define ZFSERVER_NETWORK_MSG_ACTION_H

#include "msg.h"

namespace zfserver::network
{
	/**
	 * Msg sent to the MsgServer to signal small actions like changing the
	 * direction, position, jumping, mining, etc.
	 *
	 * The MsgServer can also send those msgs to the client to signal a small
	 * action.
	 */
	class MsgAction final : public Msg
	{
	public:
		enum class Action : uint16_t
		{
			/** Set the position of an entity entering in a new map */
			EnterMap = 74,
			/** Get the items (client request). The next step is the friends. */
			GetItems = 75,
			/** Get the friends (client request). The next step is the weapon skills. */
			GetFriends = 76,
			/** Get the weapon skills (client request). The next step is the magics. */
			GetWeaponSkills = 77,
			/** Get the magics (client request). The next step is the legion attributes. */
			GetMagicSkills = 78,
			/** Change the direction of an entity */
			ChangeDirection = 79,
			/** Change the emotion of an entity */
			ChangeEmotion = 81,
			/** Signal to the server that the client entered in a portal. */
			ChangeMap = 85,
			/** Signal to the client that the player is changing map. */
			FlyMap = 86,
			/** Signal to the client that the player gained a level */
			UpLevel = 92,
			/** Signal to the client that the XP is done. */
			ClearXP = 93,
			/** Signal to both sides that the entity will reborn. */
			Reborn = 94,
			/** Delete the current role. */
			DeleteRole = 95,
			/** Set the Pk mode of an entity */
			ChangePkMode = 96,
			/** Get the legion attributes. */
			GetSyndicate = 97,
			/** Signal to both sides that the entity is mining. */
			StartMining = 99,
			/** Request a player to the server. */
			QueryPlayer = 102,
			/** Set the color of the map */
			ChangeMapLight = 104,
			/** Kick back the player at a previous position (the client answer with ACTION_SYNCHRO) */
			KickBack = 108, // to client    idUser is Player ID, unPosX unPosY is Player pos
			/** Signal to client/server a new booth. */
			CreateBooth = 111, // UID: playerID; PosX,Y: playerpos; unDir:dirofbooth; idTarget:idnpc;
			/** Destroy the booth */
			DestroyBooth = 114,
			/** Post a command to the client. */
			PostCommand = 116, // to client only
			/** Abort the current transformation. */
			AbortTransform = 118, // to server
			/** Signal to the client that the player picked up some money. */
			PickMoney = 121, // to client only
			/** Open a dialog on the client. */
			OpenDialog = 126, // to client only, open a dialog, dwData is id of dialog
			/** */
			CompleteLogin = 130,
			/** Signal to the client that the entity leaved the map. */
			LeaveMap = 132,
			/** Signal to both sides that an entity jumped. */
			Jump = 133,
			/** Signal to the client to transform in a ghost. */
			TransformGhost = 137,
			/** Synchronize the position (the client will send its coords) as answer */
			SynchronizePosition = 138,
			/** Change the face of the entity */
			ChangeFace = 142,
		};

	public:
#pragma pack(push, 1)
		typedef struct
		{
			/** Generic header of all msgs */
			Msg::Header Header;
			/** The timestamp of the creation of the packet */
			int32_t Timestamp;
			/** The unique Id of the entity */
			uint32_t UniqId;
			/** The data of the action */
			int32_t Data;
			/** The X coord of the entity */
			uint16_t PosX;
			/** The Y coord of the entity */
			uint16_t PosY;
			/** The direction of the entity */
			uint16_t Direction;
			/** The action Id */
			Action Action;
		}MsgInfo;
#pragma pack(pop)

	public:
		/**
		 * Create a message object from the specified buffer.
		 *
		 * @param[in] buf  the buffer to copy
		 * @param[in] len  the length in bytes of the buffer
		 */
		MsgAction(const uint8_t* buf, size_t len);

		MsgAction(MsgAction&& other) noexcept;
		MsgAction(const MsgAction& other);
		MsgAction& operator=(MsgAction&& other) noexcept;
		MsgAction& operator=(const MsgAction& other);

		/* destructor */
		~MsgAction() override = default;

		/**
		 * Process the message received from the client.
		 *
		 * @param[in] client      the client which has sent the message
		 * @param[in] connection  the connection on which the message was sent
		 */
		void process(Client& client, Connection& connection) override;

	private:
		MsgInfo* m_info; //!< the casted internal reference to the buffer
	};
}

#endif // ZFSERVER_NETWORK_MSG_ACTION_H
