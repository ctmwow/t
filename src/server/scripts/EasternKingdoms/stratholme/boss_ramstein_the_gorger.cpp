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
SDName: Boss_Ramstein_The_Gorger
SD%Complete: 70
SDComment:
SDCategory: Stratholme
EndScriptData */


#include "def_stratholme.h"

#define SPELL_TRAMPLE       5568
#define SPELL_KNOCKOUT      17307

#define C_MINDLESS_UNDEAD   11030

#define SAY_DUHAST          -1043900

struct boss_ramstein_the_gorgerAI : public ScriptedAI
{
    boss_ramstein_the_gorgerAI(Creature *c) : ScriptedAI(c)
    {
        pInstance = (InstanceScript*)me->GetInstanceScript();
    }

    InstanceScript* pInstance;

    uint32 Trample_Timer;
    uint32 Knockout_Timer;

    void Reset()
    override {
        Trample_Timer = 3000;
        Knockout_Timer = 12000;
    }

    void EnterCombat(Unit *who)
    override {
        DoScriptText(SAY_DUHAST, me);
    }

    void JustDied(Unit* Killer)
    override {
        for(uint8 i = 0; i < 10; i++) {
            if (Creature *temp = me->SummonCreature(C_MINDLESS_UNDEAD,3969.35,-3391.87,119.11,5.91,TEMPSUMMON_TIMED_OR_DEAD_DESPAWN,1800000))
                temp->GetMotionMaster()->MovePoint(0, 4032.84, -3418.14, 116.25);
        }

        if (pInstance)
            pInstance->SetData(TYPE_RAMSTEIN,DONE);
    }

    void UpdateAI(const uint32 diff)
    override {
        //Return since we have no target
        if (!UpdateVictim())
            return;

        //Trample
        if (Trample_Timer < diff)
        {
            DoCast(me,SPELL_TRAMPLE);
            Trample_Timer = 7000;
        }else Trample_Timer -= diff;

        //Knockout
        if (Knockout_Timer < diff)
        {
            DoCast(me->GetVictim(),SPELL_KNOCKOUT);
            Knockout_Timer = 10000;
        }else Knockout_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};
CreatureAI* GetAI_boss_ramstein_the_gorger(Creature *_Creature)
{
    return new boss_ramstein_the_gorgerAI (_Creature);
}

void AddSC_boss_ramstein_the_gorger()
{
    OLDScript *newscript;
    newscript = new OLDScript;
    newscript->Name="boss_ramstein_the_gorger";
    newscript->GetAI = &GetAI_boss_ramstein_the_gorger;
    sScriptMgr->RegisterOLDScript(newscript);
}
