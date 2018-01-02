/* Copyright (C) 2006 - 2008 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/* ScriptData
SDName: Boss_Mograine_And_Whitemane
SD%Complete: 75
SDComment: Event not implemented
SDCategory: Scarlet Monastery
EndScriptData */


#include "def_scarlet_monastery.h"

#define SAY_MO_AGGRO                -1189005
#define SAY_MO_KILL                 -1189006
#define SAY_MO_RESSURECTED          -1189007

#define SAY_WH_INTRO                -1189008
#define SAY_WH_KILL                 -1189009
#define SAY_WH_RESSURECT            -1189010

#define SAY_ASH_1   -1976000
#define SAY_ASH_2   -1976001
#define SAY_ASH_3   -1976002
#define SAY_ASH_4   -1976003
#define SAY_ASH_5   -1976004
#define SAY_ASH_6   -1976005
#define SAY_ASH_7   -1976006

#define SPELL_DIVINESHIELD2         1020
#define SPELL_CRUSADERSTRIKE5       35395
#define SPELL_HAMMEROFJUSTICE3      5589
#define SPELL_HOLYLIGHT6            3472
#define SPELL_CONSECRATION3         20922
#define SPELL_BLESSINGOFWISDOM      1044
#define SPELL_RETRIBUTIONAURA3      10299
#define SPELL_BLESSINGOFPROTECTION3 10278
#define SPELL_FLASHHEAL6            10916


#define SPELL_DEEPSLEEP                 9256
#define SPELL_SCARLETRESURRECTION       9232

#define SPELL_CRUSADERSTRIKE            17281
#define SPELL_HAMMEROFJUSTICE           13005
#define SPELL_HOLYSMITE6                9481
#define SPELL_HOLYFIRE5                 15265
#define SPELL_MINDBLAST6                8106

#define SPELL_POWERWORDSHIELD           6065

#define SPELL_RENEW                     6078
#define SPELL_FLASHHEAL6                10916


class boss_scarlet_commander_mograine : public CreatureScript
{
public:
    boss_scarlet_commander_mograine() : CreatureScript("boss_scarlet_commander_mograine")
    { }

    class boss_scarlet_commander_mograineAI : public ScriptedAI
    {
        public:
        boss_scarlet_commander_mograineAI(Creature *c) : ScriptedAI(c)
        {
            pInstance = (InstanceScript*)me->GetInstanceScript();
            alternative = false;
        }
    
        InstanceScript* pInstance;
        
        ObjectGuid ashbringerGUID;
        ObjectGuid fatherGUID;
    
        uint32 Heal_Timer;
        uint32 DivineShield2_Timer;
        uint32 CrusaderStrike5_Timer;
        uint32 HammerOfJustice3_Timer;
        uint32 Consecration3_Timer;
        uint32 BlessingOfWisdom_Timer;
        uint32 BlessingOfProtection3_Timer;
        uint32 eventTimer;
        
        uint8 step;
        
        bool alternative;
    
        void Reset()
        override {
            alternative = false;
            Heal_Timer = 80000;
            DivineShield2_Timer = 60000;
            CrusaderStrike5_Timer = 20000;
            HammerOfJustice3_Timer = 80000;
            Consecration3_Timer = 30000;
            BlessingOfWisdom_Timer = 45000;
            BlessingOfProtection3_Timer = 45000;
            eventTimer = 0;
            step = 0;
            ashbringerGUID = ObjectGuid::Empty;
            fatherGUID = ObjectGuid::Empty;
            
            if(pInstance)
                pInstance->HandleGameObject(ObjectGuid(pInstance->GetData64(DATA_DOOR_WHITEMANE)), false);
    
        }
    
        void JustEngagedWith(Unit *who)
        override {
            DoScriptText(SAY_MO_AGGRO, me);
            DoCast(me,SPELL_RETRIBUTIONAURA3);
        }
    
        void KilledUnit(Unit *victim)
        override {
            if (victim != me)
                DoScriptText(SAY_MO_KILL, me);
        }
        
        void SpellHit(Unit* caster, SpellInfo const* spell)
        override {
            if (spell->Id != 28441 || alternative)
                return;
    
            alternative = true;
            eventTimer = 5000;
            step = 1;
            ashbringerGUID = caster->GetGUID();
            me->SetOrientation(3.1142);
            me->SendMovementFlagUpdate();
            DoScriptText(SAY_ASH_1, me);
            me->SetReactState(REACT_PASSIVE);
            me->CombatStop();
        }
        
        void JustDied(Unit *who)
        override {
            if(!pInstance)
                return;
                
            pInstance->HandleGameObject(ObjectGuid(pInstance->GetData64(DATA_DOOR_WHITEMANE)), true);
        }
    
        void UpdateAI(const uint32 diff)
        override {
            if (alternative) {
                me->SetReactState(REACT_PASSIVE);
                if (eventTimer <= diff) {
                    switch (step) {
                    case 1:
                        if (Player* ashbringer = ObjectAccessor::GetPlayer(*me, ashbringerGUID))
                            DoScriptText(SAY_ASH_2, me, ashbringer);
                            
                        eventTimer = 5000;
                        break;
                    case 2:
                        if (Creature* father = me->SummonCreature(16440, 1113.977295, 1399.238403, 30.307175, 6.281954, TEMPSUMMON_MANUAL_DESPAWN, 0)) {
                            fatherGUID = father->GetGUID();
                            father->GetMotionMaster()->MovePoint(0, 1149.960205, 1398.409058, 32.250179, 6.261882 );
                        }
                        
                        eventTimer = 18000;
                        break;
                    case 3:
                        if (Creature* father = ObjectAccessor::GetCreature(*me, fatherGUID))
                            DoScriptText(SAY_ASH_3, father);
                            
                        eventTimer = 3000;
                        break;
                    case 4:
                        DoScriptText(SAY_ASH_4, me);
                        eventTimer = 4000;
                        break;
                    case 5:
                        if (Creature* father = ObjectAccessor::GetCreature(*me, fatherGUID))
                            DoScriptText(SAY_ASH_5, father);
                            
                        eventTimer = 8000;
                        break;
                    case 6:
                        DoScriptText(SAY_ASH_6, me);
                        eventTimer = 2000;
                        break;
                    case 7:
                        if (Creature* father = ObjectAccessor::GetCreature(*me, fatherGUID))
                            father->CastSpell(me, 28697, TRIGGERED_NONE);
                            
                        eventTimer = 2000;
                        break;
                    case 8:
                        if (Creature* father = ObjectAccessor::GetCreature(*me, fatherGUID)) {
                            DoScriptText(SAY_ASH_7, father);
                            father->DisappearAndDie();
                        }
                        me->Kill(me);
                        eventTimer = 0;
                        alternative = false;
                        break;
                    default:
                        break;
                    }
                    
                    step++;
                }
                else
                    eventTimer -= diff;
                    
                return;
            }
    
            if (!UpdateVictim())
                return;
    
            //If we are <50% hp cast Arcane Bubble and start casting SPECIAL Arcane Explosion
            if (me->GetHealthPct() <= 50 && !me->IsNonMeleeSpellCast(false))
            {
                //heal_Timer
                if (Heal_Timer < diff)
                {
                    //Switch between 2 different charge methods
                    switch (rand()%2)
                    {
                        case 0:
                            DoCast(me,SPELL_HOLYLIGHT6);
                            break;
                        case 1:
                            DoCast(me,SPELL_FLASHHEAL6);
                            break;
                    }
    
                    //60 seconds until we should cast this agian
                    Heal_Timer = 60000;
                    return;
                }else Heal_Timer -= diff;
            }
    
            //DivineShield2_Timer
            if (DivineShield2_Timer < diff)
            {
                DoCast(me,SPELL_DIVINESHIELD2);
                DivineShield2_Timer = 60000;
            }else DivineShield2_Timer -= diff;
    
            //CrusaderStrike5_Timer
            if (CrusaderStrike5_Timer < diff)
            {
                DoCast(me->GetVictim(),SPELL_CRUSADERSTRIKE5);
                CrusaderStrike5_Timer = 20000;
            }else CrusaderStrike5_Timer -= diff;
    
            //HammerOfJustice3_Timer
            if (HammerOfJustice3_Timer < diff)
            {
                DoCast(me->GetVictim(),SPELL_HAMMEROFJUSTICE3);
                HammerOfJustice3_Timer = 30000;
            }else HammerOfJustice3_Timer -= diff;
    
            //Consecration3_Timer
            if (Consecration3_Timer < diff)
            {
                DoCast(me->GetVictim(),SPELL_CONSECRATION3);
                Consecration3_Timer = 20000;
            }else Consecration3_Timer -= diff;
    
            //BlessingOfWisdom_Timer
            if (BlessingOfWisdom_Timer < diff)
            {
                DoCast(me,SPELL_BLESSINGOFWISDOM);
                BlessingOfWisdom_Timer = 45000;
            }else BlessingOfWisdom_Timer -= diff;
    
            //BlessingOfProtection3_Timer
            if (BlessingOfProtection3_Timer < diff)
            {
                DoCast(me,SPELL_BLESSINGOFPROTECTION3);
                BlessingOfProtection3_Timer = 50000;
            }else BlessingOfProtection3_Timer -= diff;
    
            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new boss_scarlet_commander_mograineAI(creature);
    }
};


class boss_high_inquisitor_whitemane : public CreatureScript
{
public:
    boss_high_inquisitor_whitemane() : CreatureScript("boss_high_inquisitor_whitemane")
    { }

    class boss_high_inquisitor_whitemaneAI : public ScriptedAI
    {
        public:
        boss_high_inquisitor_whitemaneAI(Creature *c) : ScriptedAI(c)
        {
            pInstance = (InstanceScript*)me->GetInstanceScript();
        }
    
        InstanceScript* pInstance;
    
        uint32 Healing_Timer;
        uint32 Renew_Timer;
        uint32 PowerWordShield_Timer;
        uint32 CrusaderStrike_Timer;
        uint32 HammerOfJustice_Timer;
        uint32 HolySmite6_Timer;
        uint32 HolyFire5_Timer;
        uint32 MindBlast6_Timer;
    
        void Reset()
        override {
            Healing_Timer = 0;
            Renew_Timer= 0;
            PowerWordShield_Timer = 2000;
            CrusaderStrike_Timer = 12000;
            HammerOfJustice_Timer = 18000;
            HolySmite6_Timer = 10000;
            HolyFire5_Timer = 20000;
            MindBlast6_Timer = 6000;
        }
    
        void JustEngagedWith(Unit *who)
        override {
            DoScriptText(SAY_WH_INTRO, me);
        }
    
        void KilledUnit(Unit *victim)
        override {
            DoScriptText(SAY_WH_KILL, me);
        }
    
        void UpdateAI(const uint32 diff)
        override {
            if (!UpdateVictim())
                return;
    
            /*
            //This is going to be a routine to make the resurrection event...
            if (me->isAlive && me->isAlive)
            {
            me->Relocate(1163.113370,1398.856812,32.527786,3.171014);
    
            DoScriptText(SAY_WH_RESSURECT, me);
    
            DoCast(me->GetVictim(),SPELL_DEEPSLEEP);
            DoCast(m-creature->GetGUID(51117),SPELL_SCARLETRESURRECTION)
            }
            */
    
            //If we are <75% hp cast healing spells at self and Mograine
            if (me->GetHealthPct() <= 75 )
            {
                if (Healing_Timer < diff)
                {
                    DoCast(me,SPELL_FLASHHEAL6);
    
                    //22-32 seconds until we should cast this agian
                    Healing_Timer = urand(22 * SECOND * IN_MILLISECONDS, 32 * SECOND * IN_MILLISECONDS);
                    return;
                }else Healing_Timer -= diff;
            }
    
            if (me->GetHealthPct() <= 30)
            {
                if (Renew_Timer < diff)
                {
                    DoCast(me,SPELL_RENEW);
                    Renew_Timer = 30000;
                }else Renew_Timer -= diff;
            }
    
            //PowerWordShield_Timer
            if (PowerWordShield_Timer < diff)
            {
                DoCast(me,SPELL_POWERWORDSHIELD);
                PowerWordShield_Timer = 25000;
            }else PowerWordShield_Timer -= diff;
    
            //CrusaderStrike_Timer
            if (CrusaderStrike_Timer < diff)
            {
                DoCast(me->GetVictim(),SPELL_CRUSADERSTRIKE);
                CrusaderStrike_Timer = 15000;
            }else CrusaderStrike_Timer -= diff;
    
            //HammerOfJustice_Timer
            if (HammerOfJustice_Timer < diff)
            {
                DoCast(me->GetVictim(),SPELL_HAMMEROFJUSTICE);
                HammerOfJustice_Timer = 12000;
            }else HammerOfJustice_Timer -= diff;
    
            //HolySmite6_Timer
            if (HolySmite6_Timer < diff)
            {
                DoCast(me->GetVictim(),SPELL_HOLYSMITE6);
                HolySmite6_Timer = 10000;
            }else HolySmite6_Timer -= diff;
    
            //HolyFire5_Timer
            if (HolyFire5_Timer < diff)
            {
                DoCast(me->GetVictim(),SPELL_HOLYFIRE5);
                HolyFire5_Timer = 15000;
            }else HolyFire5_Timer -= diff;
    
            //MindBlast6_Timer
            if (MindBlast6_Timer < diff)
            {
                DoCast(me->GetVictim(),SPELL_MINDBLAST6);
                MindBlast6_Timer = 8000;
            }else MindBlast6_Timer -= diff;
    
            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new boss_high_inquisitor_whitemaneAI(creature);
    }
};


void AddSC_boss_mograine_and_whitemane()
{

    new boss_scarlet_commander_mograine();

    new boss_high_inquisitor_whitemane();
}

