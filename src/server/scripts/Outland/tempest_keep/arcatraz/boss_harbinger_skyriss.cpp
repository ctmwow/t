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
SDName: Boss_Harbinger_Skyriss
SD%Complete: 45
SDComment: CombatAI not fully implemented. Timers will need adjustments. Need more docs on how event fully work. Reset all event and force start over if fail at one point?
SDCategory: Tempest Keep, The Arcatraz
EndScriptData */

/* ContentData
boss_harbinger_skyriss
boss_harbinger_skyriss_illusion
EndContentData */


#include "arcatraz.h"

#define SAY_INTRO               -1552000
#define SAY_AGGRO               -1552001
#define SAY_KILL_1              -1552002
#define SAY_KILL_2              -1552003
#define SAY_MIND_1              -1552004
#define SAY_MIND_2              -1552005
#define SAY_FEAR_1              -1552006
#define SAY_FEAR_2              -1552007
#define SAY_IMAGE               -1552008
#define SAY_DEATH               -1552009

#define SPELL_FEAR              39415

#define SPELL_MIND_REND         36924
#define H_SPELL_MIND_REND       39017

#define SPELL_DOMINATION        37162
#define H_SPELL_DOMINATION      39019

#define H_SPELL_MANA_BURN       39020

#define SPELL_66_ILLUSION       36931                       //entry 21466
#define SPELL_33_ILLUSION       36932                       //entry 21467


class boss_harbinger_skyriss : public CreatureScript
{
public:
    boss_harbinger_skyriss() : CreatureScript("boss_harbinger_skyriss")
    { }

    class boss_harbinger_skyrissAI : public BossAI
    {
        public:
        boss_harbinger_skyrissAI(Creature* creature) : BossAI(creature, DATA_HARBINGER_SKYRISS)
        {
            HeroicMode = me->GetMap()->IsHeroic();
            IntroDone = false;
        }
    
        bool HeroicMode;
    
        bool IntroDone;
        bool IsImage33;
        bool IsImage66;
    
        uint32 Intro_Phase;
        uint32 Intro_Timer;
        uint32 MindRend_Timer;
        uint32 Fear_Timer;
        uint32 Domination_Timer;
        uint32 ManaBurn_Timer;
    
        void Reset() override 
        {
            if(!IntroDone)
                me->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_IMMUNE_TO_PC);
    
            IsImage33 = false;
            IsImage66 = false;
    
            Intro_Phase = 1;
            Intro_Timer = 5000;
            MindRend_Timer = 3000;
            Fear_Timer = 15000;
            Domination_Timer = 30000;
            ManaBurn_Timer = 25000;
            _Reset();
        }
    
        void MoveInLineOfSight(Unit *who) override 
        {
            if(!IntroDone)
            {
                return;
            }
            ScriptedAI::MoveInLineOfSight(who);
        }
    
        void JustDied(Unit* /*Killer*/) override 
        {
            DoScriptText(SAY_DEATH, me);
            _JustDied();
        }
    
        void JustSummoned(Creature *summon) override 
        {
            if(!summon)
                return;
            if(IsImage66)
                summon->SetHealth((summon->GetMaxHealth()*33)/100);
            else
                summon->SetHealth((summon->GetMaxHealth()*66)/100);
            if(me->GetVictim())
                if(Unit *target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                    summon->EngageWithTarget(target);

            BossAI::JustSummoned(summon);
         }
    
        void KilledUnit(Unit* victim) override 
        {
            //won't yell killing pet/other unit
            if( victim->GetEntry() == 21436 )
                return;
    
            switch(rand()%2)
            {
            case 0: DoScriptText(SAY_KILL_1, me); break;
            case 1: DoScriptText(SAY_KILL_2, me); break;
            }
        }
    
        void DoSplit(uint32 val)
        {
            if( me->IsNonMeleeSpellCast(false) )
                me->InterruptNonMeleeSpells(false);
    
            DoScriptText(SAY_IMAGE, me);
    
            if( val == 66 )
                DoCast(me, SPELL_66_ILLUSION);
            else
                DoCast(me, SPELL_33_ILLUSION);
        }
    
        void UpdateAI(const uint32 diff) override 
        {
            if(!IntroDone)
            {
                if( Intro_Timer < diff )
                {
                    switch( Intro_Phase )
                    {
                        case 1:
                             DoScriptText(SAY_INTRO, me);

                             instance->HandleGameObject(instance->GetGuidData(DATA_WARDENS_SHIELD), true);
                            ++Intro_Phase;
                            Intro_Timer = 25000;
                            break;
                        case 2:
                            DoScriptText(SAY_AGGRO, me);
                            if( Unit *mellic = ObjectAccessor::GetUnit(*me, instance->GetGuidData(DATA_MELLICHAR)) )
                            {
                                //should have a better way to do this. possibly spell exist.
                                mellic->SetDeathState(JUST_DIED);
                                mellic->SetHealth(0);

                                instance->HandleGameObject(instance->GetGuidData(DATA_WARDENS_SHIELD), false);
                            }
                            ++Intro_Phase;
                            Intro_Timer = 3000;
                            break;
                        case 3:
                            me->RemoveFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_IMMUNE_TO_PC);
                            IntroDone = true;
                            break;
                    }
                }else Intro_Timer -=diff;
            }
    
            if( !UpdateVictim() )
                return;
    
            
            if( !IsImage66 && me->HealthBelowPct(66))
            {
                DoSplit(66);
                IsImage66 = true;
            }
            if( !IsImage33 && me->HealthBelowPct(33))
            {
                DoSplit(33);
                IsImage33 = true;
            }
    
            if( MindRend_Timer < diff )
            {
                if( Unit* target = SelectTarget(SELECT_TARGET_RANDOM,1) )
                    DoCast(target,HeroicMode ? H_SPELL_MIND_REND : SPELL_MIND_REND);
                else
                    DoCast(me->GetVictim(),HeroicMode ? H_SPELL_MIND_REND : SPELL_MIND_REND);
    
                MindRend_Timer = 8000;
            }else MindRend_Timer -=diff;
    
            if( Fear_Timer < diff )
            {
                if( me->IsNonMeleeSpellCast(false) )
                    return;
    
                switch(rand()%2)
                {
                case 0: DoScriptText(SAY_FEAR_1, me); break;
                case 1: DoScriptText(SAY_FEAR_2, me); break;
                }
    
                if( Unit* target = SelectTarget(SELECT_TARGET_RANDOM,1) )
                    DoCast(target,SPELL_FEAR);
                else
                    DoCast(me->GetVictim(),SPELL_FEAR);
    
                Fear_Timer = 25000;
            }else Fear_Timer -=diff;
    
            if( Domination_Timer < diff )
            {
                if( me->IsNonMeleeSpellCast(false) )
                    return;
    
                switch(rand()%2)
                {
                case 0: DoScriptText(SAY_MIND_1, me); break;
                case 1: DoScriptText(SAY_MIND_2, me); break;
                }
    
                if( Unit* target = SelectTarget(SELECT_TARGET_RANDOM,1) )
                    DoCast(target,HeroicMode ? H_SPELL_DOMINATION : SPELL_DOMINATION);
                else
                    DoCast(me->GetVictim(),HeroicMode ? H_SPELL_DOMINATION : SPELL_DOMINATION);
    
                Domination_Timer = 16000+rand()%16000;
            }else Domination_Timer -=diff;
    
            if( HeroicMode )
            {
                if( ManaBurn_Timer < diff )
                {
                    if( me->IsNonMeleeSpellCast(false) )
                        return;
    
                    if( Unit* target = SelectTarget(SELECT_TARGET_RANDOM,1) )
                        DoCast(target, H_SPELL_MANA_BURN);
    
                    ManaBurn_Timer = 16000+rand()%16000;
                }else ManaBurn_Timer -=diff;
            }
    
            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return GetArcatrazAI<boss_harbinger_skyrissAI>(creature);
    }
};


#define SPELL_MIND_REND_IMAGE   36929
#define H_SPELL_MIND_REND_IMAGE 39021


class boss_harbinger_skyriss_illusion : public CreatureScript
{
public:
    boss_harbinger_skyriss_illusion() : CreatureScript("boss_harbinger_skyriss_illusion")
    { }

    class boss_harbinger_skyriss_illusionAI : public ScriptedAI
    {
        public:
        boss_harbinger_skyriss_illusionAI(Creature *c) : ScriptedAI(c)
        {
            HeroicMode = me->GetMap()->IsHeroic();
        }
    
        bool HeroicMode;
    
        void Reset() override { }
    
        void JustEngagedWith(Unit *who) override { }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return GetArcatrazAI<boss_harbinger_skyriss_illusionAI>(creature);
    }
};


void AddSC_boss_harbinger_skyriss()
{
    new boss_harbinger_skyriss();
    new boss_harbinger_skyriss_illusion();
}

