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

#ifndef ZFSERVER_NETWORK_MSG_ITEM_H
#define ZFSERVER_NETWORK_MSG_ITEM_H

#include "msg.h"

namespace zfserver::network
{
	/**
	 * Msg sent to the MsgServer to signal small item actions like equiping,
	 * buying, selling, dropping...
	 */
	class MsgItem final : public Msg
	{
	public:
		enum class Action : uint32_t
		{
			/** Buy an item from a Npc shop. */
			Buy = 1,             // to server, id: idNpc, data: idItemType
			/** Sell an item to a Npc shop. */
			Sell = 2,            // to server, id: idNpc, data: idItem
			/** Drop an item on the ground. */
			Drop = 3,            // to server, x, y
			/** Use an item (might equip). */
			Use = 4,             // to server, data: position
			/** Equip an item at a specific position. */
			Equip = 5,           // to client��֪ͨװ����Ʒ
			/** Unequip an item at a specific position. */
			Unequip = 6,         // to server, data: position
			/** Split an item. */
			SplitItem = 7,       // to server, data: num
			/** Combine an item. (Arrows ?) */
			CombineItem = 8,     // to server, data: id
			/** Query the saved money. */
			QUERYMONEYSAVED = 9,	// to server/client, id: idNpc, data: nMoney
			/** Deposit money. */
			SAVEMONEY = 10,      // to server, id: idNpc, data: nMoney
			/** Withdraw money. */
			DRAWMONEY = 11,      // to server, id: idNpc, data: nMoney
			/** Drop money on the ground. */
			DROPMONEY = 12,      // to server, x, y
			/** Spend money ??? */
			SPENDMONEY = 13,     // �����壬�ݱ���
			/** Repair an item at a Npc shop. */
			REPAIR = 14,         // to server, return MsgItem_info
			/** Repair all items. */
			REPAIRALL = 15,      // to server, return REPAIRAll, or not return if no money
			/** Ident an item. */
			IDENT = 16,          // to server, return MsgItem_info
			/** Update the durability of an item. */
			DURABILITY = 17,     // to client, update durability
			/** Delete an equipment. */
			DROPEQUIPMENT = 18,  // to client, delete equipment
			/** Improve the quality of an item. */
			IMPROVE = 19,        // to server, improve equipment
			/** Upgrade the level of an item. */
			UPLEV = 20,          // to server, upleve equipment
			/** Open a booth and request all items. */
			BOOTH_QUERY = 21,    // to server, open booth, data is npc id
			/** Add an item to the booth. */
			BOOTH_ADD = 22,      // to server/client(broadcast in table), id is idItem, data is money
			/** Delete an item from the booth. */
			BOOTH_DEL = 23,      // to server/client(broadcast), id is idItem, data is npc id
			/** Buy an item from a booth. */
			BOOTH_BUY = 24,      // to server, id is idItem, data is npc id
			/** Synchronise the amount (durability) of an item. */
			SYNCHRO_AMOUNT = 25, // to client(data is amount)
			/** Use fireworks ??? */
			FIREWORKS = 26,
			/** Ping evaluation */
			CompleteTask = 27
		};

	public:
#pragma pack(push, 1)
		typedef struct
		{
			/** Generic header of all msgs */
			Msg::Header Header;
			/** The unique Id of the item */
			uint32_t UniqId;

			union
			{
				/** The data of the action. */
				uint32_t Data;
				struct
				{
					/** The Y coord of the item. */
					uint16_t PosY;
					/** The X coord of the item. */
					uint16_t PosX;
				};
			};

			/** The action Id */
			Action Action;
			/** The timestamp of the msg. */
			uint32_t Timestamp;
		}MsgInfo;
#pragma pack(pop)

	public:
		/**
		 * Create a message object from the specified buffer.
		 *
		 * @param[in] buf  the buffer to copy
		 * @param[in] len  the length in bytes of the buffer
		 */
		MsgItem(const uint8_t* buf, size_t len);

		MsgItem(MsgItem&& other) noexcept;
		MsgItem(const MsgItem& other);
		MsgItem& operator=(MsgItem&& other) noexcept;
		MsgItem& operator=(const MsgItem& other);

		/* destructor */
		~MsgItem() override = default;

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

#endif // ZFSERVER_NETWORK_MSG_ITEM_H
