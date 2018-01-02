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
SDName: Boss_Mekgineer_Steamrigger
SD%Complete: 60
SDComment: Mechanics' interrrupt heal doesn't work very well, also a proper movement needs to be implemented -> summon further away and move towards target to repair.
SDCategory: Coilfang Resevoir, The Steamvault
EndScriptData */

/* ContentData
boss_mekgineer_steamrigger
mob_steamrigger_mechanic
EndContentData */


#include "def_steam_vault.h"

#define SAY_MECHANICS               -1545007
#define SAY_AGGRO_1                 -1545008
#define SAY_AGGRO_2                 -1545009
#define SAY_AGGRO_3                 -1545010
#define SAY_AGGRO_4                 -1545011
#define SAY_SLAY_1                  -1545012
#define SAY_SLAY_2                  -1545013
#define SAY_SLAY_3                  -1545014
#define SAY_DEATH                   -1545015

#define SPELL_SUPER_SHRINK_RAY      31485
#define SPELL_SAW_BLADE             31486
#define SPELL_ELECTRIFIED_NET       35107
#define H_SPELL_ENRAGE              1                       //corrent enrage spell not known

#define ENTRY_STREAMRIGGER_MECHANIC 17951


class boss_mekgineer_steamrigger : public CreatureScript
{
public:
    boss_mekgineer_steamrigger() : CreatureScript("boss_mekgineer_steamrigger")
    { }

    class boss_mekgineer_steamriggerAI : public ScriptedAI
    {
        public:
        boss_mekgineer_steamriggerAI(Creature *c) : ScriptedAI(c)
        {
            pInstance = ((InstanceScript*)c->GetInstanceScript());
            HeroicMode = me->GetMap()->IsHeroic();
        }
    
        InstanceScript *pInstance;
        bool HeroicMode;
    
        uint32 Shrink_Timer;
        uint32 Saw_Blade_Timer;
        uint32 Electrified_Net_Timer;
        bool Summon75;
        bool Summon50;
        bool Summon25;
    
        void Reset()
        override {
            Shrink_Timer = 20000;
            Saw_Blade_Timer = 15000;
            Electrified_Net_Timer = 10000;
    
            Summon75 = false;
            Summon50 = false;
            Summon25 = false;
    
            if (pInstance && me->IsAlive())
                pInstance->SetData(TYPE_MEKGINEER_STEAMRIGGER, NOT_STARTED);
        }
    
        void JustDied(Unit* Killer)
        override {
            DoScriptText(SAY_DEATH, me);
    
            if (pInstance)
                pInstance->SetData(TYPE_MEKGINEER_STEAMRIGGER, DONE);
        }
    
        void KilledUnit(Unit* victim)
        override {
            switch(rand()%3)
            {
                case 0: DoScriptText(SAY_SLAY_1, me); break;
                case 1: DoScriptText(SAY_SLAY_2, me); break;
                case 2: DoScriptText(SAY_SLAY_3, me); break;
            }
        }
    
        void JustEngagedWith(Unit *who)
        override {
            switch(rand()%3)
            {
                case 0: DoScriptText(SAY_AGGRO_1, me); break;
                case 1: DoScriptText(SAY_AGGRO_2, me); break;
                case 2: DoScriptText(SAY_AGGRO_3, me); break;
            }
    
            if (pInstance)
                pInstance->SetData(TYPE_MEKGINEER_STEAMRIGGER, IN_PROGRESS);
        }
    
        //no known summon spells exist
        void SummonMechanichs()
        {
            DoScriptText(SAY_MECHANICS, me);
    
            DoSpawnCreature(ENTRY_STREAMRIGGER_MECHANIC,5,5,0,0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 240000);
            DoSpawnCreature(ENTRY_STREAMRIGGER_MECHANIC,-5,5,0,0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 240000);
            DoSpawnCreature(ENTRY_STREAMRIGGER_MECHANIC,-5,-5,0,0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 240000);
    
            if (rand()%2)
                DoSpawnCreature(ENTRY_STREAMRIGGER_MECHANIC,5,-7,0,0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 240000);
            if (rand()%2)
                DoSpawnCreature(ENTRY_STREAMRIGGER_MECHANIC,7,-5,0,0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 240000);
        }
    
        void UpdateAI(const uint32 diff)
        override {
            if (!UpdateVictim())
                return;
    
            if (Shrink_Timer < diff)
            {
                DoCast(me->GetVictim(),SPELL_SUPER_SHRINK_RAY);
                Shrink_Timer = 20000;
            }else Shrink_Timer -= diff;
    
            if (Saw_Blade_Timer < diff)
            {
                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM,1))
                    DoCast(target,SPELL_SAW_BLADE);
                else
                    DoCast(me->GetVictim(),SPELL_SAW_BLADE);
    
                Saw_Blade_Timer = 15000;
            } else Saw_Blade_Timer -= diff;
    
            if (Electrified_Net_Timer < diff)
            {
                DoCast(me->GetVictim(),SPELL_ELECTRIFIED_NET);
                Electrified_Net_Timer = 10000;
            }
            else Electrified_Net_Timer -= diff;
    
            if (!Summon75)
            {
                if ((me->GetHealthPct()) < 75)
                {
                    SummonMechanichs();
                    Summon75 = true;
                }
            }
    
            if (!Summon50)
            {
                if ((me->GetHealthPct()) < 50)
                {
                    SummonMechanichs();
                    Summon50 = true;
                }
            }
    
            if (!Summon25)
            {
                if ((me->GetHealthPct()) < 25)
                {
                    SummonMechanichs();
                    Summon25 = true;
                }
            }
    
            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new boss_mekgineer_steamriggerAI(creature);
    }
};


#define SPELL_DISPEL_MAGIC          17201
#define SPELL_REPAIR                31532
#define H_SPELL_REPAIR              37936

#define MAX_REPAIR_RANGE            (13.0f)                 //we should be at least at this range for repair
#define MIN_REPAIR_RANGE            (7.0f)                  //we can stop movement at this range to repair but not required


class mob_steamrigger_mechanic : public CreatureScript
{
public:
    mob_steamrigger_mechanic() : CreatureScript("mob_steamrigger_mechanic")
    { }

    class mob_steamrigger_mechanicAI : public ScriptedAI
    {
        public:
        mob_steamrigger_mechanicAI(Creature *c) : ScriptedAI(c)
        {
            pInstance = ((InstanceScript*)c->GetInstanceScript());
            HeroicMode = me->GetMap()->IsHeroic();
        }
    
        InstanceScript* pInstance;
        bool HeroicMode;
    
        uint32 Repair_Timer;
    
        void Reset()
        override {
            Repair_Timer = 2000;
        }
    
        void MoveInLineOfSight(Unit* who)
        override {
            //react only if attacked
            return;
        }
    
        void JustEngagedWith(Unit *who) override { }
    
        void UpdateAI(const uint32 diff)
        override {
            if (Repair_Timer < diff)
            {
                if (pInstance && pInstance->GetData64(DATA_MEKGINEERSTEAMRIGGER) && pInstance->GetData(TYPE_MEKGINEER_STEAMRIGGER) == IN_PROGRESS)
                {
                    if (Unit* pMekgineer = ObjectAccessor::GetUnit((*me), ObjectGuid(pInstance->GetData64(DATA_MEKGINEERSTEAMRIGGER))))
                    {
                        if (me->IsWithinDistInMap(pMekgineer, MAX_REPAIR_RANGE))
                        {
                            //are we already channeling? Doesn't work very well, find better check?
                            if (!me->GetUInt32Value(UNIT_CHANNEL_SPELL))
                            {
                                //me->GetMotionMaster()->MovementExpired();
                                //me->GetMotionMaster()->MoveIdle();
    
                                DoCast(me,HeroicMode ? H_SPELL_REPAIR : SPELL_REPAIR, true);
                            }
                            Repair_Timer = 5000;
                        }
                        else
                        {
                            //me->GetMotionMaster()->MovementExpired();
                            //me->GetMotionMaster()->MoveFollow(pMekgineer,0,0);
                        }
                    }
                }else Repair_Timer = 5000;
            }else Repair_Timer -= diff;
    
            if (!UpdateVictim())
                return;
    
            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new mob_steamrigger_mechanicAI(creature);
    }
};


void AddSC_boss_mekgineer_steamrigger()
{

    new boss_mekgineer_steamrigger();

    new mob_steamrigger_mechanic();
}

