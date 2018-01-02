 /* Copyright (C) 2006 - 2008 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

/* ScriptData
SDName: Shadowfang_Keep
SD%Complete: 75
SDComment: npc_shadowfang_prisoner using escortAI for movement to door. Might need additional code in case being attacked. Add proper texts/say().
SDCategory: Shadowfang Keep
EndScriptData */

/* ContentData
npc_shadowfang_prisoner
EndContentData */


#include "EscortAI.h"
#include "def_shadowfang_keep.h"

/*######
## npc_shadowfang_prisoner
######*/

#define SAY_FREE                -1033000
#define GOSSIP_ITEM_DOOR        "Thanks, I'll follow you to the door."


class npc_shadowfang_prisoner : public CreatureScript
{
public:
    npc_shadowfang_prisoner() : CreatureScript("npc_shadowfang_prisoner")
    { }

    class npc_shadowfang_prisonerAI : public EscortAI
    {
        public:
        npc_shadowfang_prisonerAI(Creature *c) : EscortAI(c)
        {
            pInstance = ((InstanceScript*)c->GetInstanceScript());

            uint32 eCreature = c->GetEntry();

            if (eCreature == 3849)                                    //adamant
                AddWaypoint(0, -254.47, 2117.48, 81.17);
            if (eCreature == 3850)                                    //ashcrombe
                AddWaypoint(0, -252.35, 2126.71, 81.17);

            AddWaypoint(1, -253.63, 2131.27, 81.28);
            AddWaypoint(2, -249.66, 2142.45, 87.01);
            AddWaypoint(3, -248.08, 2143.68, 87.01);
            AddWaypoint(4, -238.87, 2139.93, 87.01);
            AddWaypoint(5, -235.47, 2149.18, 90.59);
            AddWaypoint(6, -239.89, 2156.06, 90.62, 0.0f, 20000);
        }
    
        InstanceScript *pInstance;
    
        void WaypointReached(uint32 i, uint32 pathID)
        override {
            if( pInstance && i == 6)
            {
                me->HandleEmoteCommand(EMOTE_ONESHOT_TALK);
                DoScriptText(SAY_FREE, me);
                pInstance->SetData(TYPE_FREE_NPC, DONE);
            }
        }
    
        void Reset() override {}
        void JustEngagedWith(Unit* who) override {}

        virtual bool GossipHello(Player* player) override
        {
            if (!pInstance)
                return false;

            if (pInstance->GetData(TYPE_FREE_NPC) != DONE && pInstance->GetData(TYPE_RETHILGORE) == DONE)
                player->ADD_GOSSIP_ITEM( GOSSIP_ICON_CHAT, GOSSIP_ITEM_DOOR, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

            SEND_PREPARED_GOSSIP_MENU(player, me);

            return true;

        }


        virtual bool GossipSelect(Player* player, uint32 menuId, uint32 gossipListId) override
        {
            uint32 const action = player->PlayerTalkClass->GetGossipOptionAction(gossipListId);
            if (action == GOSSIP_ACTION_INFO_DEF+1)
            {
                player->CLOSE_GOSSIP_MENU();
                ((EscortAI*)(me->AI()))->Start(false, false, ObjectGuid::Empty);
            }
            return true;

        }

    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_shadowfang_prisonerAI(creature);
    }
};




/*######
## AddSC
######*/

void AddSC_shadowfang_keep()
{

    new npc_shadowfang_prisoner();
}

