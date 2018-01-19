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
SDName: Boss_Marli
SD%Complete: 80
SDComment: Charging healers and casters not working. Perhaps wrong Spell Timers.
SDCategory: Zul'Gurub
EndScriptData */


#include "def_zulgurub.h"

#define SAY_AGGRO               -1309005
#define SAY_TRANSFORM           -1309006
#define SAY_SPIDER_SPAWN        -1309007
#define SAY_DEATH               -1309008

#define SPELL_CHARGE              22911
#define SPELL_ASPECT_OF_MARLI     24686                     // A stun spell
#define SPELL_ENVOLWINGWEB        24110
#define SPELL_POISONVOLLEY        24099
#define SPELL_SPIDER_FORM         24084

//The Spider Spells
#define SPELL_LEVELUP             24312                     //Not right Spell.


//Spawn of Marli

class boss_marli : public CreatureScript
{
public:
    boss_marli() : CreatureScript("boss_marli")
    { }

    class boss_marliAI : public ScriptedAI
    {
        public:
        boss_marliAI(Creature *c) : ScriptedAI(c)
        {
            pInstance = ((InstanceScript*)c->GetInstanceScript());
        }
    
        InstanceScript *pInstance;
    
        uint32 SpawnStartSpiders_Timer;
        uint32 PoisonVolley_Timer;
        uint32 SpawnSpider_Timer;
        uint32 Charge_Timer;
        uint32 Aspect_Timer;
        uint32 Transform_Timer;
        uint32 TransformBack_Timer;
    
        Creature *Spider;
        bool Spawned;
        bool PhaseTwo;
    
        void Reset()
        override {
            SpawnStartSpiders_Timer = 1000;
            PoisonVolley_Timer = 15000;
            SpawnSpider_Timer = 30000;
            Charge_Timer = 1500;
            Aspect_Timer = 12000;
            Transform_Timer = 45000;
            TransformBack_Timer = 25000;
    
            Spawned = false;
            PhaseTwo = false;
    
            me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, true);
            me->ApplySpellImmune(1, IMMUNITY_EFFECT,SPELL_EFFECT_ATTACK_ME, true);
        }
    
        void JustEngagedWith(Unit *who)
        override {
            DoScriptText(SAY_AGGRO, me);
        }
    
        void JustDied(Unit* /*Killer*/)
        override {
            DoScriptText(SAY_DEATH, me);
            if(pInstance)
                pInstance->SetData(DATA_MARLI_DEATH, 0);
        }
    
        void UpdateAI(const uint32 diff)
        override {
            if (!UpdateVictim())
                return;
    
            if( me->GetVictim() && me->IsAlive())
            {
                if (PoisonVolley_Timer < diff)
                {
                    DoCast(me->GetVictim(),SPELL_POISONVOLLEY);
                    PoisonVolley_Timer = 10000 + rand()%10000;
                }else PoisonVolley_Timer -= diff;
    
                if (!PhaseTwo && Aspect_Timer < diff)
                {
                    DoCast(me->GetVictim(),SPELL_ASPECT_OF_MARLI);
                    Aspect_Timer = 13000 + rand()%5000;
                }else Aspect_Timer -= diff;
    
                if (!Spawned && SpawnStartSpiders_Timer < diff)
                {
                    DoScriptText(SAY_SPIDER_SPAWN, me);
    
                    Unit* target = SelectTarget(SELECT_TARGET_RANDOM,0);
                    if(!target)
                        return;
    
                    Spider = me->SummonCreature(15041,target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(),0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 15000);
                    if(Spider)
                        Spider->AI()->AttackStart(target);
                    Spider = me->SummonCreature(15041,target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(),0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 15000);
                    if(Spider)
                        Spider->AI()->AttackStart(target);
                    Spider = me->SummonCreature(15041,target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(),0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 15000);
                    if(Spider)
                        Spider->AI()->AttackStart(target);
                    Spider = me->SummonCreature(15041,target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(),0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 15000);
                    if(Spider)
                        Spider->AI()->AttackStart(target);
    
                    Spawned = true;
                }else SpawnStartSpiders_Timer -= diff;
    
                if (SpawnSpider_Timer < diff)
                {
                    Unit* target = SelectTarget(SELECT_TARGET_RANDOM,0);
                    if(!target)
                        return;
    
                    Spider = me->SummonCreature(15041,target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(),0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 15000);
                    if(Spider)
                        Spider->AI()->AttackStart(target);
                    SpawnSpider_Timer = 12000 + rand()%5000;
                }else SpawnSpider_Timer -= diff;
    
                if(!PhaseTwo && Transform_Timer < diff)
                {
                    DoScriptText(SAY_TRANSFORM, me);
                    DoCast(me,SPELL_SPIDER_FORM);
                    /* FIXMEDMG
                    const CreatureTemplate *cinfo = me->GetCreatureTemplate();
                    me->SetBaseWeaponDamage(BASE_ATTACK, MINDAMAGE, (cinfo->mindmg +((cinfo->mindmg/100) * 35)));
                    me->SetBaseWeaponDamage(BASE_ATTACK, MAXDAMAGE, (cinfo->maxdmg +((cinfo->maxdmg/100) * 35)));
                    me->UpdateDamagePhysical(BASE_ATTACK); */
                    DoCast(me->GetVictim(),SPELL_ENVOLWINGWEB);
    
                    if(me->GetThreatManager().GetThreat(me->GetVictim()))
                        ModifyThreatByPercent(me->GetVictim(),-100);
    
                    PhaseTwo = true;
                    Transform_Timer = 35000 + rand()%25000;
                }else Transform_Timer -= diff;
    
                if (PhaseTwo)
                {
                    if (Charge_Timer < diff)
                    {
                        Unit* target = nullptr;
                        int i = 0 ;
                        while (i < 3)                           // max 3 tries to get a random target with power_mana
                        {
                            ++i;                                //not aggro leader
                            target = SelectTarget(SELECT_TARGET_RANDOM,1);
                            if (target)
                                if (target->GetPowerType() == POWER_MANA)
                                    i=3;
                        }
                        if (target)
                        {
                            DoCast(target, SPELL_CHARGE);
                            //me->Relocate(target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), 0);
                            //me->SendMonsterMove(target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), 0, true,1);
                            AttackStart(target);
                        }
    
                        Charge_Timer = 8000;
                    }else Charge_Timer -= diff;
    
                    if (TransformBack_Timer < diff)
                    {
                        me->SetUInt32Value(UNIT_FIELD_DISPLAYID,15220);
                        /* FIXMEDMG
                        const CreatureTemplate *cinfo = me->GetCreatureTemplate();
                        me->SetBaseWeaponDamage(BASE_ATTACK, MINDAMAGE, (cinfo->mindmg +((cinfo->mindmg/100) * 1)));
                        me->SetBaseWeaponDamage(BASE_ATTACK, MAXDAMAGE, (cinfo->maxdmg +((cinfo->maxdmg/100) * 1)));
                        me->UpdateDamagePhysical(BASE_ATTACK);
    */
                        PhaseTwo = false;
                        TransformBack_Timer = 25000 + rand()%15000;
                    }else TransformBack_Timer -= diff;
    
                }
    
                DoMeleeAttackIfReady();
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new boss_marliAI(creature);
    }
};


class mob_spawn_of_marli : public CreatureScript
{
public:
    mob_spawn_of_marli() : CreatureScript("mob_spawn_of_marli")
    { }

    class mob_spawn_of_marliAI : public ScriptedAI
    {
        public:
        mob_spawn_of_marliAI(Creature *c) : ScriptedAI(c) {}
    
        uint32 LevelUp_Timer;
    
        void Reset()
        override {
            LevelUp_Timer = 3000;
        }
    
        void JustEngagedWith(Unit *who)
        override {
        }
    
        void UpdateAI (const uint32 diff)
        override {
            //Return since we have no target
            if (!UpdateVictim() )
                return;
    
            //LevelUp_Timer
            if(LevelUp_Timer < diff)
            {
                DoCast(me,SPELL_LEVELUP);
                LevelUp_Timer = 3000;
            }else LevelUp_Timer -= diff;
    
            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new mob_spawn_of_marliAI(creature);
    }
};


void AddSC_boss_marli()
{

    new boss_marli();

    new mob_spawn_of_marli();
}

