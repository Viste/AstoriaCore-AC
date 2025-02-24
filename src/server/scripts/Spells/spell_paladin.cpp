/*
 * Copyright (C) 2016+     AzerothCore <www.azerothcore.org>, released under GNU GPL v2 license: https://github.com/azerothcore/azerothcore-wotlk/blob/master/LICENSE-GPL2
 * Copyright (C) 2008-2016 TrinityCore <http://www.trinitycore.org/>
 * Copyright (C) 2005-2009 MaNGOS <http://getmangos.com/>
 */

/*
 * Scripts for spells with SPELLFAMILY_PALADIN and SPELLFAMILY_GENERIC spells used by paladin players.
 * Ordered alphabetically using scriptname.
 * Scriptnames of files in this file should be prefixed with "spell_pal_".
 */

#include "Player.h"
#include "ScriptMgr.h"
#include "SpellScript.h"
#include "SpellAuraEffects.h"
#include "Group.h"
#include "UnitAI.h"

enum PaladinSpells
{
    SPELL_PALADIN_DIVINE_PLEA                    = 54428,
    SPELL_PALADIN_BLESSING_OF_SANCTUARY_BUFF     = 67480,
    SPELL_PALADIN_BLESSING_OF_SANCTUARY_ENERGIZE = 57319,

    SPELL_PALADIN_HOLY_SHOCK_R1                  = 20473,
    SPELL_PALADIN_HOLY_SHOCK_R1_DAMAGE           = 25912,
    SPELL_PALADIN_HOLY_SHOCK_R1_HEALING          = 25914,

    SPELL_PALADIN_BLESSING_OF_LOWER_CITY_DRUID   = 37878,
    SPELL_PALADIN_BLESSING_OF_LOWER_CITY_PALADIN = 37879,
    SPELL_PALADIN_BLESSING_OF_LOWER_CITY_PRIEST  = 37880,
    SPELL_PALADIN_BLESSING_OF_LOWER_CITY_SHAMAN  = 37881,

    SPELL_PALADIN_DIVINE_STORM                   = 53385,
    SPELL_PALADIN_DIVINE_STORM_DUMMY             = 54171,
    SPELL_PALADIN_DIVINE_STORM_HEAL              = 54172,

    SPELL_PALADIN_EYE_FOR_AN_EYE_DAMAGE          = 25997,

    SPELL_PALADIN_FORBEARANCE                    = 25771,
    SPELL_PALADIN_AVENGING_WRATH_MARKER          = 61987,
    SPELL_PALADIN_IMMUNE_SHIELD_MARKER           = 61988,

    SPELL_PALADIN_HAND_OF_SACRIFICE              = 6940,
    SPELL_PALADIN_DIVINE_SACRIFICE               = 64205,

    SPELL_PALADIN_JUDGEMENT_DAMAGE               = 54158,
    SPELL_PALADIN_JUDGEMENT_OF_JUSTICE           = 20184,
    SPELL_PALADIN_JUDGEMENT_OF_LIGHT             = 20185,
    SPELL_PALADIN_JUDGEMENT_OF_WISDOM            = 20186,
    SPELL_PALADIN_JUDGEMENT_OF_SHADOW            = 81002,
    

    SPELL_PALADIN_GLYPH_OF_SALVATION             = 63225,

    SPELL_PALADIN_RIGHTEOUS_DEFENSE_TAUNT        = 31790,

    SPELL_PALADIN_SANCTIFIED_WRATH               = 57318,
    SPELL_PALADIN_SANCTIFIED_WRATH_TALENT_R1     = 53375,

    SPELL_PALADIN_SEAL_OF_RIGHTEOUSNESS          = 25742,
    SPELL_PALADIN_SEAL_OF_CORRUPTION             = 81000,

    SPELL_PALADIN_CONCENTRACTION_AURA            = 19746,
    SPELL_PALADIN_SANCTIFIED_RETRIBUTION_R1      = 31869,
    SPELL_PALADIN_SWIFT_RETRIBUTION_R1           = 53379,

    SPELL_PALADIN_IMPROVED_CONCENTRACTION_AURA   = 63510,
    SPELL_PALADIN_IMPROVED_DEVOTION_AURA         = 63514,
    SPELL_PALADIN_SANCTIFIED_RETRIBUTION_AURA    = 63531,
    SPELL_PALADIN_AURA_MASTERY_IMMUNE            = 64364,

    SPELL_GENERIC_ARENA_DAMPENING                = 74410,
    SPELL_GENERIC_BATTLEGROUND_DAMPENING         = 74411
};

enum PaladinSpellIcons
{
    PALADIN_ICON_ID_RETRIBUTION_AURA             = 555
};

// Ours
class spell_pal_seal_of_command : public SpellScriptLoader
{
public:
    spell_pal_seal_of_command() : SpellScriptLoader("spell_pal_seal_of_command") { }

    class spell_pal_seal_of_command_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_pal_seal_of_command_AuraScript);

        bool CheckProc(ProcEventInfo& eventInfo)
        {
            if (const SpellInfo* procSpell = eventInfo.GetDamageInfo()->GetSpellInfo())
                if (procSpell->SpellIconID == 3025) // Righteous Vengeance, should not proc SoC
                    return false;
            return true;
        }

        void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
        {
            PreventDefaultAction();
            int32 targets = 3;
            if (const SpellInfo* procSpell = eventInfo.GetDamageInfo()->GetSpellInfo())
                if (procSpell->IsAffectingArea())
                    targets = 1;

            eventInfo.GetActor()->CastCustomSpell(aurEff->GetSpellInfo()->Effects[EFFECT_0].TriggerSpell, SPELLVALUE_MAX_TARGETS, targets, eventInfo.GetActionTarget(), false, NULL, aurEff);
        }

        void Register()
        {
            if (m_scriptSpellId == 20375)
            {
                DoCheckProc += AuraCheckProcFn(spell_pal_seal_of_command_AuraScript::CheckProc);
                OnEffectProc += AuraEffectProcFn(spell_pal_seal_of_command_AuraScript::HandleProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
            }
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_pal_seal_of_command_AuraScript();
    }

    class spell_pal_seal_of_command_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_pal_seal_of_command_SpellScript);

        void FilterTargets(std::list<WorldObject*>& targets)
        {
            if (const SpellValue* spellValue = GetSpellValue())
                if (spellValue->MaxAffectedTargets == 1)
                    targets.clear();
        }

        void Register()
        {
            if (m_scriptSpellId == 20424)
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_pal_seal_of_command_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_TARGET_ENEMY);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_pal_seal_of_command_SpellScript();
    }
};

class spell_pal_divine_intervention : public SpellScriptLoader
{
public:
    spell_pal_divine_intervention() : SpellScriptLoader("spell_pal_divine_intervention") { }

    class spell_pal_divine_intervention_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_pal_divine_intervention_AuraScript);

        void HandleRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            if (!GetTarget()->IsInCombat() && GetTarget()->GetTypeId() == TYPEID_PLAYER)
                GetTarget()->RemoveAurasDueToSpell(GetTarget()->ToPlayer()->GetTeamId() == TEAM_ALLIANCE ? 57723 : 57724);
        }

        void Register()
        {
            OnEffectRemove += AuraEffectRemoveFn(spell_pal_divine_intervention_AuraScript::HandleRemove, EFFECT_0, SPELL_AURA_SCHOOL_IMMUNITY, AURA_EFFECT_HANDLE_REAL);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_pal_divine_intervention_AuraScript();
    }
};

class spell_pal_seal_of_light : public SpellScriptLoader
{
public:
    spell_pal_seal_of_light() : SpellScriptLoader("spell_pal_seal_of_light") { }

    class spell_pal_seal_of_light_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_pal_seal_of_light_AuraScript);

        bool CheckProc(ProcEventInfo& eventInfo)
        {
            // xinef: skip divine storm self hit (dummy) and righteous vengeance (0x20000000=
            return eventInfo.GetActor() != eventInfo.GetProcTarget() && (!eventInfo.GetDamageInfo()->GetSpellInfo() || !eventInfo.GetDamageInfo()->GetSpellInfo()->SpellFamilyFlags.HasFlag(0x20000000));
        }

        void Register()
        {
            DoCheckProc += AuraCheckProcFn(spell_pal_seal_of_light_AuraScript::CheckProc);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_pal_seal_of_light_AuraScript();
    }
};

class spell_pal_sacred_shield_base : public SpellScriptLoader
{
public:
    spell_pal_sacred_shield_base() : SpellScriptLoader("spell_pal_sacred_shield_base") { }

    class spell_pal_sacred_shield_base_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_pal_sacred_shield_base_AuraScript);

        void CalculateAmount(AuraEffect const* aurEff, int32& amount, bool& /*canBeRecalculated*/)
        {
            if (Unit* caster = GetCaster())
            {
                const SpellInfo* spellInfo = sSpellMgr->GetSpellInfo(GetSpellInfo()->Effects[aurEff->GetEffIndex()].TriggerSpell);
                amount = spellInfo->Effects[EFFECT_0].CalcValue();

                // +75.00% from sp bonus
                amount += CalculatePct(caster->SpellBaseDamageBonusDone(spellInfo->GetSchoolMask()), 75.0f);

                // Xinef: removed divine guardian because it will affect triggered spell with increased amount
                // Arena - Dampening
                if (AuraEffect const* dampening = caster->GetAuraEffect(SPELL_GENERIC_ARENA_DAMPENING, EFFECT_0))
                    AddPct(amount, dampening->GetAmount());
                // Battleground - Dampening
                else if (AuraEffect const* dampening = caster->GetAuraEffect(SPELL_GENERIC_BATTLEGROUND_DAMPENING, EFFECT_0))
                    AddPct(amount, dampening->GetAmount());
            }
        }

        bool CheckProc(ProcEventInfo& eventInfo)
        {
            return !(eventInfo.GetHitMask() & PROC_EX_INTERNAL_HOT) && eventInfo.GetDamageInfo()->GetDamage() > 0;
        }

        void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
        {
            PreventDefaultAction();

            if (eventInfo.GetTypeMask() & PROC_FLAG_TAKEN_SPELL_MAGIC_DMG_CLASS_POS)
            {
                Unit* caster = eventInfo.GetActor();
                const SpellInfo* procSpell = eventInfo.GetDamageInfo()->GetSpellInfo();
                if (caster && procSpell->SpellFamilyName == SPELLFAMILY_PALADIN &&
                        procSpell->SpellFamilyFlags.HasFlag(0x40000000) && caster->GetAuraEffect(SPELL_AURA_PROC_TRIGGER_SPELL, SPELLFAMILY_PALADIN, 3021, 0)) // need infusion of light
                {
                    int32 basepoints = int32(float(eventInfo.GetDamageInfo()->GetDamage()) / 12.0f);
                    // Item - Paladin T9 Holy 4P Bonus (Flash of Light)
                    if (AuraEffect const* aurEffect = caster->GetAuraEffect(67191, EFFECT_0))
                        AddPct(basepoints, aurEffect->GetAmount());

                    caster->CastCustomSpell(eventInfo.GetActionTarget(), 66922, &basepoints, nullptr, nullptr, true, 0, aurEff, caster->GetGUID());
                    return;
                }

                return;
            }

            uint32 triggered_spell_id = GetSpellInfo()->Effects[aurEff->GetEffIndex()].TriggerSpell;
            if (eventInfo.GetActionTarget()->HasSpellCooldown(triggered_spell_id))
                return;

            uint32 cooldown = eventInfo.GetProcCooldown();
            int32 basepoints = aurEff->GetAmount();

            // Item - Paladin T8 Holy 4P Bonus
            if (Unit* caster = aurEff->GetCaster())
                if (AuraEffect const* aurEffect = caster->GetAuraEffect(64895, 0))
                    cooldown = aurEffect->GetAmount() * IN_MILLISECONDS;

            eventInfo.GetActionTarget()->AddSpellCooldown(triggered_spell_id, 0, cooldown);
            eventInfo.GetActionTarget()->CastCustomSpell(eventInfo.GetActionTarget(), triggered_spell_id, &basepoints, nullptr, nullptr, true, NULL, aurEff, eventInfo.GetActionTarget()->GetGUID());
        }

        void Register()
        {
            DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_pal_sacred_shield_base_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_DUMMY);
            DoCheckProc += AuraCheckProcFn(spell_pal_sacred_shield_base_AuraScript::CheckProc);
            OnEffectProc += AuraEffectProcFn(spell_pal_sacred_shield_base_AuraScript::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_pal_sacred_shield_base_AuraScript();
    }
};


// Theirs
// 31850 - Ardent Defender
class spell_pal_ardent_defender : public SpellScriptLoader
{
public:
    spell_pal_ardent_defender() : SpellScriptLoader("spell_pal_ardent_defender") { }

    class spell_pal_ardent_defender_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_pal_ardent_defender_AuraScript);

    public:
        spell_pal_ardent_defender_AuraScript()
        {
            absorbPct = 0;
            healPct = 0;
        }

    private:
        uint32 absorbPct, healPct;

        enum Spell
        {
            PAL_SPELL_ARDENT_DEFENDER_HEAL = 66235
        };

        bool Validate(SpellInfo const* /*spellInfo*/) override
        {
            if (!sSpellMgr->GetSpellInfo(PAL_SPELL_ARDENT_DEFENDER_HEAL))
                return false;
            return true;
        }

        bool Load() override
        {
            healPct = GetSpellInfo()->Effects[EFFECT_1].CalcValue();
            absorbPct = GetSpellInfo()->Effects[EFFECT_0].CalcValue();
            return GetUnitOwner()->GetTypeId() == TYPEID_PLAYER;
        }

        void CalculateAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& /*canBeRecalculated*/)
        {
            // Set absorbtion amount to unlimited
            amount = -1;
        }

        void Absorb(AuraEffect* aurEff, DamageInfo& dmgInfo, uint32& absorbAmount)
        {
            Unit* victim = GetTarget();
            int32 remainingHealth = victim->GetHealth() - dmgInfo.GetDamage();
            uint32 allowedHealth = victim->CountPctFromMaxHealth(35);
            // If damage kills us
            if (remainingHealth <= 0 && !victim->ToPlayer()->HasSpellCooldown(PAL_SPELL_ARDENT_DEFENDER_HEAL))
            {
                // Cast healing spell, completely avoid damage
                absorbAmount = dmgInfo.GetDamage();

                uint32 defenseSkillValue = victim->GetDefenseSkillValue();
                // Max heal when defense skill denies critical hits from raid bosses
                // Formula: max defense at level + 140 (raiting from gear)
                uint32 reqDefForMaxHeal = victim->getLevel() * 5 + 140;
                float pctFromDefense = (defenseSkillValue >= reqDefForMaxHeal)
                                       ? 1.0f
                                       : float(defenseSkillValue) / float(reqDefForMaxHeal);

                int32 healAmount = int32(victim->CountPctFromMaxHealth(uint32(healPct * pctFromDefense)));
                victim->CastCustomSpell(PAL_SPELL_ARDENT_DEFENDER_HEAL, SPELLVALUE_BASE_POINT0, healAmount, victim, true, nullptr, aurEff);
                victim->ToPlayer()->AddSpellCooldown(PAL_SPELL_ARDENT_DEFENDER_HEAL, 0, 120000);
            }
            else if (remainingHealth < int32(allowedHealth))
            {
                // Reduce damage that brings us under 35% (or full damage if we are already under 35%) by x%
                uint32 damageToReduce = (victim->GetHealth() < allowedHealth)
                                        ? dmgInfo.GetDamage()
                                        : allowedHealth - remainingHealth;
                absorbAmount = CalculatePct(damageToReduce, absorbPct);
            }
        }

        void Register() override
        {
            DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_pal_ardent_defender_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
            OnEffectAbsorb += AuraEffectAbsorbFn(spell_pal_ardent_defender_AuraScript::Absorb, EFFECT_0);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_pal_ardent_defender_AuraScript();
    }
};

// 31884 - Avenging Wrath
class spell_pal_avenging_wrath : public SpellScriptLoader
{
public:
    spell_pal_avenging_wrath() : SpellScriptLoader("spell_pal_avenging_wrath") { }

    class spell_pal_avenging_wrath_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_pal_avenging_wrath_AuraScript);

        bool Validate(SpellInfo const* /*spellInfo*/)
        {
            if (!sSpellMgr->GetSpellInfo(SPELL_PALADIN_SANCTIFIED_WRATH)
                    || !sSpellMgr->GetSpellInfo(SPELL_PALADIN_SANCTIFIED_WRATH_TALENT_R1))
                return false;
            return true;
        }

        void HandleApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            Unit* target = GetTarget();
            if (AuraEffect const* aurEff = target->GetAuraEffectOfRankedSpell(SPELL_PALADIN_SANCTIFIED_WRATH_TALENT_R1, EFFECT_2))
            {
                int32 basepoints = aurEff->GetAmount();
                target->CastCustomSpell(target, SPELL_PALADIN_SANCTIFIED_WRATH, &basepoints, &basepoints, NULL, true, NULL, aurEff);
            }
        }

        void HandleRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            GetTarget()->RemoveAurasDueToSpell(SPELL_PALADIN_SANCTIFIED_WRATH);
        }

        void Register()
        {
            OnEffectApply += AuraEffectApplyFn(spell_pal_avenging_wrath_AuraScript::HandleApply, EFFECT_0, SPELL_AURA_MOD_DAMAGE_PERCENT_DONE, AURA_EFFECT_HANDLE_REAL);
            AfterEffectRemove += AuraEffectRemoveFn(spell_pal_avenging_wrath_AuraScript::HandleRemove, EFFECT_0, SPELL_AURA_MOD_DAMAGE_PERCENT_DONE, AURA_EFFECT_HANDLE_REAL);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_pal_avenging_wrath_AuraScript();
    }
};

// 37877 - Blessing of Faith
class spell_pal_blessing_of_faith : public SpellScriptLoader
{
public:
    spell_pal_blessing_of_faith() : SpellScriptLoader("spell_pal_blessing_of_faith") { }

    class spell_pal_blessing_of_faith_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_pal_blessing_of_faith_SpellScript);

        bool Validate(SpellInfo const* /*spellInfo*/)
        {
            if (!sSpellMgr->GetSpellInfo(SPELL_PALADIN_BLESSING_OF_LOWER_CITY_DRUID) || !sSpellMgr->GetSpellInfo(SPELL_PALADIN_BLESSING_OF_LOWER_CITY_PALADIN) || !sSpellMgr->GetSpellInfo(SPELL_PALADIN_BLESSING_OF_LOWER_CITY_PRIEST) || !sSpellMgr->GetSpellInfo(SPELL_PALADIN_BLESSING_OF_LOWER_CITY_SHAMAN))
                return false;
            return true;
        }

        void HandleDummy(SpellEffIndex /*effIndex*/)
        {
            if (Unit* unitTarget = GetHitUnit())
            {
                uint32 spell_id = 0;
                switch (unitTarget->getClass())
                {
                    case CLASS_DRUID:
                        spell_id = SPELL_PALADIN_BLESSING_OF_LOWER_CITY_DRUID;
                        break;
                    case CLASS_PALADIN:
                        spell_id = SPELL_PALADIN_BLESSING_OF_LOWER_CITY_PALADIN;
                        break;
                    case CLASS_PRIEST:
                        spell_id = SPELL_PALADIN_BLESSING_OF_LOWER_CITY_PRIEST;
                        break;
                    case CLASS_SHAMAN:
                        spell_id = SPELL_PALADIN_BLESSING_OF_LOWER_CITY_SHAMAN;
                        break;
                    default:
                        return; // ignore for non-healing classes
                }
                Unit* caster = GetCaster();
                caster->CastSpell(caster, spell_id, true);
            }
        }

        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_pal_blessing_of_faith_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_pal_blessing_of_faith_SpellScript();
    }
};

// 20911 - Blessing of Sanctuary
// 25899 - Greater Blessing of Sanctuary
class spell_pal_blessing_of_sanctuary : public SpellScriptLoader
{
public:
    spell_pal_blessing_of_sanctuary() : SpellScriptLoader("spell_pal_blessing_of_sanctuary") { }

    class spell_pal_blessing_of_sanctuary_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_pal_blessing_of_sanctuary_AuraScript);

        bool Validate(SpellInfo const* /*spellInfo*/)
        {
            if (!sSpellMgr->GetSpellInfo(SPELL_PALADIN_BLESSING_OF_SANCTUARY_BUFF))
                return false;
            if (!sSpellMgr->GetSpellInfo(SPELL_PALADIN_BLESSING_OF_SANCTUARY_ENERGIZE))
                return false;
            return true;
        }

        void HandleEffectApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            Unit* target = GetTarget();
            if (Unit* caster = GetCaster())
            {
                // xinef: hack
                int32 value = 9;
                caster->CastCustomSpell(target, SPELL_PALADIN_BLESSING_OF_SANCTUARY_BUFF, &value, &value, 0, true);
            }
        }

        void HandleEffectRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            Unit* target = GetTarget();
            target->RemoveAura(SPELL_PALADIN_BLESSING_OF_SANCTUARY_BUFF, GetCasterGUID());
        }

        bool CheckProc(ProcEventInfo& /*eventInfo*/)
        {
            return GetTarget()->getPowerType() == POWER_MANA;
        }

        void HandleProc(AuraEffect const* aurEff, ProcEventInfo& /*eventInfo*/)
        {
            PreventDefaultAction();
            GetTarget()->CastSpell(GetTarget(), SPELL_PALADIN_BLESSING_OF_SANCTUARY_ENERGIZE, true, NULL, aurEff);
        }

        void Register()
        {
            AfterEffectApply += AuraEffectApplyFn(spell_pal_blessing_of_sanctuary_AuraScript::HandleEffectApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
            AfterEffectRemove += AuraEffectRemoveFn(spell_pal_blessing_of_sanctuary_AuraScript::HandleEffectRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
            DoCheckProc += AuraCheckProcFn(spell_pal_blessing_of_sanctuary_AuraScript::CheckProc);
            OnEffectProc += AuraEffectProcFn(spell_pal_blessing_of_sanctuary_AuraScript::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_pal_blessing_of_sanctuary_AuraScript();
    }
};

// 64205 - Divine Sacrifice
class spell_pal_divine_sacrifice : public SpellScriptLoader
{
public:
    spell_pal_divine_sacrifice() : SpellScriptLoader("spell_pal_divine_sacrifice") { }

    class spell_pal_divine_sacrifice_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_pal_divine_sacrifice_AuraScript);

        uint32 groupSize, minHpPct;
        int32 remainingAmount;

        bool Load()
        {
            if (Unit* caster = GetCaster())
            {
                if (caster->GetTypeId() == TYPEID_PLAYER)
                {
                    if (caster->ToPlayer()->GetGroup())
                        groupSize = caster->ToPlayer()->GetGroup()->GetMembersCount();
                    else
                        groupSize = 1;
                }
                else
                    return false;

                remainingAmount = (caster->CountPctFromMaxHealth(GetSpellInfo()->Effects[EFFECT_2].CalcValue(caster)) * groupSize);
                minHpPct = GetSpellInfo()->Effects[EFFECT_1].CalcValue(caster);
                return true;
            }
            return false;
        }

        void Split(AuraEffect* /*aurEff*/, DamageInfo& /*dmgInfo*/, uint32& splitAmount)
        {
            remainingAmount -= splitAmount;
            // break when absorbed everything it could, or if the casters hp drops below 20%
            if (Unit* caster = GetCaster())
                if (remainingAmount <= 0 || (caster->GetHealthPct() < minHpPct))
                    caster->RemoveAura(SPELL_PALADIN_DIVINE_SACRIFICE);
        }

        void Register()
        {
            OnEffectSplit += AuraEffectSplitFn(spell_pal_divine_sacrifice_AuraScript::Split, EFFECT_0);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_pal_divine_sacrifice_AuraScript();
    }
};

// 53385 - Divine Storm
class spell_pal_divine_storm : public SpellScriptLoader
{
public:
    spell_pal_divine_storm() : SpellScriptLoader("spell_pal_divine_storm") { }

    class spell_pal_divine_storm_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_pal_divine_storm_SpellScript);

        uint32 healPct;

        bool Validate(SpellInfo const* /*spellInfo*/)
        {
            if (!sSpellMgr->GetSpellInfo(SPELL_PALADIN_DIVINE_STORM_DUMMY))
                return false;
            return true;
        }

        bool Load()
        {
            healPct = GetSpellInfo()->Effects[EFFECT_1].CalcValue(GetCaster());
            return true;
        }

        void TriggerHeal()
        {
            Unit* caster = GetCaster();
            if (GetHitUnit() != caster)
                caster->CastCustomSpell(SPELL_PALADIN_DIVINE_STORM_DUMMY, SPELLVALUE_BASE_POINT0, (GetHitDamage() * healPct) / 100, caster, true);
        }

        void Register()
        {
            AfterHit += SpellHitFn(spell_pal_divine_storm_SpellScript::TriggerHeal);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_pal_divine_storm_SpellScript();
    }
};

// 54171 - Divine Storm (Dummy)
class spell_pal_divine_storm_dummy : public SpellScriptLoader
{
public:
    spell_pal_divine_storm_dummy() : SpellScriptLoader("spell_pal_divine_storm_dummy") { }

    class spell_pal_divine_storm_dummy_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_pal_divine_storm_dummy_SpellScript);

        bool Validate(SpellInfo const* /*spellInfo*/)
        {
            if (!sSpellMgr->GetSpellInfo(SPELL_PALADIN_DIVINE_STORM_HEAL))
                return false;
            return true;
        }

        void CountTargets(std::list<WorldObject*>& targetList)
        {
            acore::Containers::RandomResizeList(targetList, GetSpellValue()->MaxAffectedTargets);
            _targetCount = targetList.size();
        }

        void HandleDummy(SpellEffIndex /*effIndex*/)
        {
            if (!_targetCount || ! GetHitUnit())
                return;

            int32 heal = GetEffectValue() / _targetCount;
            GetCaster()->CastCustomSpell(GetHitUnit(), SPELL_PALADIN_DIVINE_STORM_HEAL, &heal, nullptr, nullptr, true);
        }
    private:
        uint32 _targetCount;

        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_pal_divine_storm_dummy_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_pal_divine_storm_dummy_SpellScript::CountTargets, EFFECT_0, TARGET_UNIT_CASTER_AREA_RAID);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_pal_divine_storm_dummy_SpellScript();
    }
};

// 33695 - Exorcism and Holy Wrath Damage
class spell_pal_exorcism_and_holy_wrath_damage : public SpellScriptLoader
{
public:
    spell_pal_exorcism_and_holy_wrath_damage() : SpellScriptLoader("spell_pal_exorcism_and_holy_wrath_damage") { }

    class spell_pal_exorcism_and_holy_wrath_damage_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_pal_exorcism_and_holy_wrath_damage_AuraScript);

        void HandleEffectCalcSpellMod(AuraEffect const* aurEff, SpellModifier*& spellMod)
        {
            if (!spellMod)
            {
                spellMod = new SpellModifier(aurEff->GetBase());
                spellMod->op = SPELLMOD_DAMAGE;
                spellMod->type = SPELLMOD_FLAT;
                spellMod->spellId = GetId();
                spellMod->mask[1] = 0x200002;
            }

            spellMod->value = aurEff->GetAmount();
        }

        void Register()
        {
            DoEffectCalcSpellMod += AuraEffectCalcSpellModFn(spell_pal_exorcism_and_holy_wrath_damage_AuraScript::HandleEffectCalcSpellMod, EFFECT_0, SPELL_AURA_DUMMY);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_pal_exorcism_and_holy_wrath_damage_AuraScript();
    }
};

// -9799 - Eye for an Eye
class spell_pal_eye_for_an_eye : public SpellScriptLoader
{
public:
    spell_pal_eye_for_an_eye() : SpellScriptLoader("spell_pal_eye_for_an_eye") { }

    class spell_pal_eye_for_an_eye_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_pal_eye_for_an_eye_AuraScript);

        bool Validate(SpellInfo const* /*spellInfo*/)
        {
            if (!sSpellMgr->GetSpellInfo(SPELL_PALADIN_EYE_FOR_AN_EYE_DAMAGE))
                return false;
            return true;
        }

        void OnProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
        {
            PreventDefaultAction();
            // return damage % to attacker but < 50% own total health
            int32 damage = int32(std::min(CalculatePct(eventInfo.GetDamageInfo()->GetDamage(), aurEff->GetAmount()), GetTarget()->GetMaxHealth() / 2));
            GetTarget()->CastCustomSpell(SPELL_PALADIN_EYE_FOR_AN_EYE_DAMAGE, SPELLVALUE_BASE_POINT0, damage, eventInfo.GetProcTarget(), true, NULL, aurEff);
        }

        void Register()
        {
            OnEffectProc += AuraEffectProcFn(spell_pal_eye_for_an_eye_AuraScript::OnProc, EFFECT_0, SPELL_AURA_DUMMY);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_pal_eye_for_an_eye_AuraScript();
    }
};

// 54968 - Glyph of Holy Light
class spell_pal_glyph_of_holy_light : public SpellScriptLoader
{
public:
    spell_pal_glyph_of_holy_light() : SpellScriptLoader("spell_pal_glyph_of_holy_light") { }

    class spell_pal_glyph_of_holy_light_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_pal_glyph_of_holy_light_SpellScript);

        void FilterTargets(std::list<WorldObject*>& targets)
        {
            uint32 const maxTargets = GetSpellInfo()->MaxAffectedTargets;

            if (targets.size() > maxTargets)
            {
                targets.sort(acore::HealthPctOrderPred());
                targets.resize(maxTargets);
            }
        }

        void Register()
        {
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_pal_glyph_of_holy_light_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ALLY);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_pal_glyph_of_holy_light_SpellScript();
    }
};

// 63521 - Guarded by The Light
class spell_pal_guarded_by_the_light : public SpellScriptLoader
{
public:
    spell_pal_guarded_by_the_light() : SpellScriptLoader("spell_pal_guarded_by_the_light") { }

    class spell_pal_guarded_by_the_light_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_pal_guarded_by_the_light_SpellScript);

        bool Validate(SpellInfo const* /*spellInfo*/)
        {
            if (!sSpellMgr->GetSpellInfo(SPELL_PALADIN_DIVINE_PLEA))
                return false;
            return true;
        }

        void HandleScriptEffect(SpellEffIndex /*effIndex*/)
        {
            // Divine Plea
            if (Aura* aura = GetCaster()->GetAura(SPELL_PALADIN_DIVINE_PLEA))
                aura->RefreshDuration();
        }

        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_pal_guarded_by_the_light_SpellScript::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_pal_guarded_by_the_light_SpellScript();
    }
};

// 6940 - Hand of Sacrifice
class spell_pal_hand_of_sacrifice : public SpellScriptLoader
{
public:
    spell_pal_hand_of_sacrifice() : SpellScriptLoader("spell_pal_hand_of_sacrifice") { }

    class spell_pal_hand_of_sacrifice_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_pal_hand_of_sacrifice_AuraScript);

        int32 remainingAmount;

        bool Load()
        {
            if (Unit* caster = GetCaster())
            {
                remainingAmount = caster->GetMaxHealth();
                return true;
            }
            return false;
        }

        void Split(AuraEffect* /*aurEff*/, DamageInfo& /*dmgInfo*/, uint32& splitAmount)
        {
            remainingAmount -= splitAmount;

            if (remainingAmount <= 0)
            {
                GetTarget()->RemoveAura(SPELL_PALADIN_HAND_OF_SACRIFICE);
            }
        }

        void Register()
        {
            OnEffectSplit += AuraEffectSplitFn(spell_pal_hand_of_sacrifice_AuraScript::Split, EFFECT_0);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_pal_hand_of_sacrifice_AuraScript();
    }

    class spell_pal_hand_of_sacrifice_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_pal_hand_of_sacrifice_SpellScript);

        SpellCastResult CheckTarget()
        {
            Unit* target = GetExplTargetUnit();
            if (!target || target->HasAura(SPELL_PALADIN_DIVINE_SACRIFICE))
                return SPELL_FAILED_TARGET_AURASTATE;

            return SPELL_CAST_OK;
        }

        void Register()
        {
            OnCheckCast += SpellCheckCastFn(spell_pal_hand_of_sacrifice_SpellScript::CheckTarget);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_pal_hand_of_sacrifice_SpellScript();
    };
};

// 1038 - Hand of Salvation
class spell_pal_hand_of_salvation : public SpellScriptLoader
{
public:
    spell_pal_hand_of_salvation() : SpellScriptLoader("spell_pal_hand_of_salvation") { }

    class spell_pal_hand_of_salvation_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_pal_hand_of_salvation_AuraScript);

        void CalculateAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& /*canBeRecalculated*/)
        {
            if (Unit* caster = GetCaster())
            {
                // Glyph of Salvation
                if (caster->GetGUID() == GetUnitOwner()->GetGUID())
                    if (AuraEffect const* aurEff = caster->GetAuraEffect(SPELL_PALADIN_GLYPH_OF_SALVATION, EFFECT_0))
                        amount -= aurEff->GetAmount();
            }
        }

        void Register()
        {
            DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_pal_hand_of_salvation_AuraScript::CalculateAmount, EFFECT_1, SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_pal_hand_of_salvation_AuraScript();
    }
};

// -20473 - Holy Shock
class spell_pal_holy_shock : public SpellScriptLoader
{
public:
    spell_pal_holy_shock() : SpellScriptLoader("spell_pal_holy_shock") { }

    class spell_pal_holy_shock_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_pal_holy_shock_SpellScript);

        bool Validate(SpellInfo const* spellInfo)
        {
            SpellInfo const* firstRankSpellInfo = sSpellMgr->GetSpellInfo(SPELL_PALADIN_HOLY_SHOCK_R1);
            if (!firstRankSpellInfo)
                return false;

            // can't use other spell than holy shock due to spell_ranks dependency
            if (!spellInfo->IsRankOf(firstRankSpellInfo))
                return false;

            uint8 rank = spellInfo->GetRank();
            if (!sSpellMgr->GetSpellWithRank(SPELL_PALADIN_HOLY_SHOCK_R1_DAMAGE, rank, true) || !sSpellMgr->GetSpellWithRank(SPELL_PALADIN_HOLY_SHOCK_R1_HEALING, rank, true))
                return false;

            return true;
        }

        void HandleDummy(SpellEffIndex /*effIndex*/)
        {
            Unit* caster = GetCaster();
            if (Unit* unitTarget = GetHitUnit())
            {
                uint8 rank = GetSpellInfo()->GetRank();
                if (caster->IsFriendlyTo(unitTarget))
                    caster->CastSpell(unitTarget, sSpellMgr->GetSpellWithRank(SPELL_PALADIN_HOLY_SHOCK_R1_HEALING, rank), true);
                else
                    caster->CastSpell(unitTarget, sSpellMgr->GetSpellWithRank(SPELL_PALADIN_HOLY_SHOCK_R1_DAMAGE, rank), true);
            }
        }

        SpellCastResult CheckCast()
        {
            Unit* caster = GetCaster();
            if (Unit* target = GetExplTargetUnit())
            {
                if (!caster->IsFriendlyTo(target))
                {
                    if (!caster->IsValidAttackTarget(target))
                        return SPELL_FAILED_BAD_TARGETS;

                    if (!caster->isInFront(target))
                        return SPELL_FAILED_UNIT_NOT_INFRONT;
                }
            }
            else
                return SPELL_FAILED_BAD_TARGETS;
            return SPELL_CAST_OK;
        }

        void Register()
        {
            OnCheckCast += SpellCheckCastFn(spell_pal_holy_shock_SpellScript::CheckCast);
            OnEffectHitTarget += SpellEffectFn(spell_pal_holy_shock_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_pal_holy_shock_SpellScript();
    }
};

// 53407 - Judgement of Justice
// 20271 - Judgement of Light
// 53408 - Judgement of Wisdom
// 81001 - Judgement of Shadow
class spell_pal_judgement : public SpellScriptLoader
{
public:
    spell_pal_judgement(char const* scriptName, uint32 spellId) : SpellScriptLoader(scriptName), _spellId(spellId) { }

    class spell_pal_judgement_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_pal_judgement_SpellScript);

    public:
        spell_pal_judgement_SpellScript(uint32 spellId) : SpellScript(), _spellId(spellId) { }

        bool Validate(SpellInfo const* /*spellInfo*/)
        {
            if (!sSpellMgr->GetSpellInfo(SPELL_PALADIN_JUDGEMENT_DAMAGE)
                    || !sSpellMgr->GetSpellInfo(_spellId))
                return false;
            return true;
        }

        void HandleScriptEffect(SpellEffIndex /*effIndex*/)
        {
            uint32 spellId2 = SPELL_PALADIN_JUDGEMENT_DAMAGE;

            // some seals have SPELL_AURA_DUMMY in EFFECT_2
            Unit::AuraEffectList const& auras = GetCaster()->GetAuraEffectsByType(SPELL_AURA_DUMMY);
            for (Unit::AuraEffectList::const_iterator i = auras.begin(); i != auras.end(); ++i)
            {
                if ((*i)->GetSpellInfo()->GetSpellSpecific() == SPELL_SPECIFIC_SEAL && (*i)->GetEffIndex() == EFFECT_2)
                    if (sSpellMgr->GetSpellInfo((*i)->GetAmount()))
                    {
                        spellId2 = (*i)->GetAmount();
                        break;
                    }
            }

            GetCaster()->CastSpell(GetHitUnit(), _spellId, true);
            GetCaster()->CastSpell(GetHitUnit(), spellId2, true);

            // Judgement of the Just
            if (GetCaster()->GetAuraEffect(SPELL_AURA_ADD_FLAT_MODIFIER, SPELLFAMILY_PALADIN, 3015, 0))
                GetCaster()->CastSpell(GetHitUnit(), 68055, true);
        }

        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_pal_judgement_SpellScript::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
        }

    private:
        uint32 const _spellId;
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_pal_judgement_SpellScript(_spellId);
    }

private:
    uint32 const _spellId;
};

// 20425 - Judgement of Command
class spell_pal_judgement_of_command : public SpellScriptLoader
{
public:
    spell_pal_judgement_of_command() : SpellScriptLoader("spell_pal_judgement_of_command") { }

    class spell_pal_judgement_of_command_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_pal_judgement_of_command_SpellScript);

        void HandleDummy(SpellEffIndex /*effIndex*/)
        {
            if (Unit* unitTarget = GetHitUnit())
                if (SpellInfo const* spell_proto = sSpellMgr->GetSpellInfo(GetEffectValue()))
                    GetCaster()->CastSpell(unitTarget, spell_proto, true, nullptr);
        }

        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_pal_judgement_of_command_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_pal_judgement_of_command_SpellScript();
    }
};

// -633 - Lay on Hands
class spell_pal_lay_on_hands : public SpellScriptLoader
{
public:
    spell_pal_lay_on_hands() : SpellScriptLoader("spell_pal_lay_on_hands") { }

    class spell_pal_lay_on_hands_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_pal_lay_on_hands_SpellScript);

        bool Validate(SpellInfo const* /*spell*/)
        {
            if (!sSpellMgr->GetSpellInfo(SPELL_PALADIN_FORBEARANCE))
                return false;
            if (!sSpellMgr->GetSpellInfo(SPELL_PALADIN_AVENGING_WRATH_MARKER))
                return false;
            if (!sSpellMgr->GetSpellInfo(SPELL_PALADIN_IMMUNE_SHIELD_MARKER))
                return false;
            return true;
        }

        bool Load()
        {
            _manaAmount = 0;
            return true;
        }

        SpellCastResult CheckCast()
        {
            Unit* caster = GetCaster();
            if (Unit* target = GetExplTargetUnit())
                if (caster == target)
                    if (target->HasAura(SPELL_PALADIN_FORBEARANCE) || target->HasAura(SPELL_PALADIN_AVENGING_WRATH_MARKER) || target->HasAura(SPELL_PALADIN_IMMUNE_SHIELD_MARKER))
                        return SPELL_FAILED_TARGET_AURASTATE;

            // Xinef: Glyph of Divinity
            if (Unit* target = GetExplTargetUnit())
                if (target->getPowerType() == POWER_MANA)
                    _manaAmount = target->GetPower(POWER_MANA);

            return SPELL_CAST_OK;
        }

        void HandleScript()
        {
            Unit* caster = GetCaster();
            Unit* target = GetHitUnit();
            if (caster == target)
            {
                caster->CastSpell(caster, SPELL_PALADIN_FORBEARANCE, true);
                caster->CastSpell(caster, SPELL_PALADIN_AVENGING_WRATH_MARKER, true);
                caster->CastSpell(caster, SPELL_PALADIN_IMMUNE_SHIELD_MARKER, true);
            }
            // Xinef: Glyph of Divinity
            else if (target && caster->HasAura(54939) && GetSpellInfo()->Id != 633 && _manaAmount > 0) // excluding first rank
            {
                _manaAmount = target->GetPower(POWER_MANA) - _manaAmount;
                if (_manaAmount > 0)
                    caster->CastCustomSpell(54986 /*Energize*/, SPELLVALUE_BASE_POINT1, _manaAmount, caster, true);
            }
        }

        void Register()
        {
            OnCheckCast += SpellCheckCastFn(spell_pal_lay_on_hands_SpellScript::CheckCast);
            AfterHit += SpellHitFn(spell_pal_lay_on_hands_SpellScript::HandleScript);
        }

        int32 _manaAmount;
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_pal_lay_on_hands_SpellScript();
    }
};

// 31789 - Righteous Defense
class spell_pal_righteous_defense : public SpellScriptLoader
{
public:
    spell_pal_righteous_defense() : SpellScriptLoader("spell_pal_righteous_defense") { }

    class spell_pal_righteous_defense_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_pal_righteous_defense_SpellScript);

        bool Validate(SpellInfo const* /*spellInfo*/)
        {
            if (!sSpellMgr->GetSpellInfo(SPELL_PALADIN_RIGHTEOUS_DEFENSE_TAUNT))
                return false;
            return true;
        }

        SpellCastResult CheckCast()
        {
            Unit* caster = GetCaster();
            if (caster->GetTypeId() != TYPEID_PLAYER)
                return SPELL_FAILED_DONT_REPORT;

            if (Unit* target = GetExplTargetUnit())
            {
                if (!target->IsFriendlyTo(caster) || target->getAttackers().empty())
                    return SPELL_FAILED_BAD_TARGETS;
            }
            else
                return SPELL_FAILED_BAD_TARGETS;

            return SPELL_CAST_OK;
        }

        void HandleTriggerSpellLaunch(SpellEffIndex effIndex)
        {
            PreventHitDefaultEffect(effIndex);
        }

        void HandleTriggerSpellHit(SpellEffIndex effIndex)
        {
            PreventHitDefaultEffect(effIndex);
            if (Unit* target = GetHitUnit())
                GetCaster()->CastSpell(target, SPELL_PALADIN_RIGHTEOUS_DEFENSE_TAUNT, true);
        }

        void Register()
        {
            OnCheckCast += SpellCheckCastFn(spell_pal_righteous_defense_SpellScript::CheckCast);
            //! WORKAROUND
            //! target select will be executed in hitphase of effect 0
            //! so we must handle trigger spell also in hit phase (default execution in launch phase)
            //! see issue #3718
            OnEffectLaunchTarget += SpellEffectFn(spell_pal_righteous_defense_SpellScript::HandleTriggerSpellLaunch, EFFECT_1, SPELL_EFFECT_TRIGGER_SPELL);
            OnEffectHitTarget += SpellEffectFn(spell_pal_righteous_defense_SpellScript::HandleTriggerSpellHit, EFFECT_1, SPELL_EFFECT_TRIGGER_SPELL);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_pal_righteous_defense_SpellScript();
    }
};

// 20154, 21084 - Seal of Righteousness - melee proc dummy (addition ${$MWS*(0.022*$AP+0.044*$SPH)} damage)
class spell_pal_seal_of_righteousness : public SpellScriptLoader
{
public:
    spell_pal_seal_of_righteousness() : SpellScriptLoader("spell_pal_seal_of_righteousness") { }

    class spell_pal_seal_of_righteousness_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_pal_seal_of_righteousness_AuraScript);

        bool Validate(SpellInfo const* /*spellInfo*/)
        {
            if (!sSpellMgr->GetSpellInfo(SPELL_PALADIN_SEAL_OF_RIGHTEOUSNESS))
                return false;
            return true;
        }

        bool CheckProc(ProcEventInfo& eventInfo)
        {
            Unit* target = eventInfo.GetProcTarget();
            if (!target)
                return false;

            return target->IsAlive() && !eventInfo.GetTriggerAuraSpell() && (eventInfo.GetDamageInfo()->GetDamage() || (eventInfo.GetHitMask() & PROC_EX_ABSORB));
        }

        void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
        {
            PreventDefaultAction();

            float ap = GetTarget()->GetTotalAttackPowerValue(BASE_ATTACK);
            int32 holy = GetTarget()->SpellBaseDamageBonusDone(SPELL_SCHOOL_MASK_HOLY);
            holy += eventInfo.GetProcTarget()->SpellBaseDamageBonusTaken(SPELL_SCHOOL_MASK_HOLY);

            // Xinef: Libram of Divine Purpose
            if (AuraEffect* aurEff = GetTarget()->GetDummyAuraEffect(SPELLFAMILY_PALADIN, 2025, EFFECT_0))
                holy += aurEff->GetAmount();

            int32 bp = std::max<int32>(0, int32((ap * 0.022f + 0.044f * holy) * GetTarget()->GetAttackTime(BASE_ATTACK) / 1000));
            GetTarget()->CastCustomSpell(SPELL_PALADIN_SEAL_OF_RIGHTEOUSNESS, SPELLVALUE_BASE_POINT0, bp, eventInfo.GetProcTarget(), true, NULL, aurEff);
        }

        void Register()
        {
            DoCheckProc += AuraCheckProcFn(spell_pal_seal_of_righteousness_AuraScript::CheckProc);
            OnEffectProc += AuraEffectProcFn(spell_pal_seal_of_righteousness_AuraScript::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_pal_seal_of_righteousness_AuraScript();
    }
};

// 81000 - Seal of Corruption - melee proc dummy (addition ${$MWS*(0.022*$AP+0.044*$SPH)} damage)
class spell_pal_seal_of_corruption : public SpellScriptLoader
{
public:
    spell_pal_seal_of_corruption() : SpellScriptLoader("spell_pal_seal_of_corruption") { }

    class spell_pal_seal_of_corruption_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_pal_seal_of_corruption_AuraScript);

        bool Validate(SpellInfo const* /*spellInfo*/)
        {
            if (!sSpellMgr->GetSpellInfo(SPELL_PALADIN_SEAL_OF_CORRUPTION))
                return false;
            return true;
        }

        bool CheckProc(ProcEventInfo& eventInfo)
        {
            Unit* target = eventInfo.GetProcTarget();
            if (!target)
                return false;

            return target->IsAlive() && !eventInfo.GetTriggerAuraSpell() && (eventInfo.GetDamageInfo()->GetDamage() || (eventInfo.GetHitMask() & PROC_EX_ABSORB));
        }

        void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
        {
            PreventDefaultAction();

            float ap = GetTarget()->GetTotalAttackPowerValue(BASE_ATTACK);
            int32 holy = GetTarget()->SpellBaseDamageBonusDone(SPELL_SCHOOL_MASK_SHADOW);
            holy += eventInfo.GetProcTarget()->SpellBaseDamageBonusTaken(SPELL_SCHOOL_MASK_SHADOW);

            // Xinef: Libram of Divine Purpose
            if (AuraEffect* aurEff = GetTarget()->GetDummyAuraEffect(SPELLFAMILY_PALADIN, 2025, EFFECT_0))
                holy += aurEff->GetAmount();

            int32 bp = std::max<int32>(0, int32((ap * 0.022f + 0.044f * SPELL_SCHOOL_MASK_SHADOW) * GetTarget()->GetAttackTime(BASE_ATTACK) / 1000));
            GetTarget()->CastCustomSpell(SPELL_PALADIN_SEAL_OF_CORRUPTION, SPELLVALUE_BASE_POINT0, bp, eventInfo.GetProcTarget(), true, NULL, aurEff);
        }

        void Register()
        {
            DoCheckProc += AuraCheckProcFn(spell_pal_seal_of_corruption_AuraScript::CheckProc);
            OnEffectProc += AuraEffectProcFn(spell_pal_seal_of_corruption_AuraScript::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_pal_seal_of_corruption_AuraScript();
    }
};

void AddSC_paladin_spell_scripts()
{
    // Ours
    new spell_pal_seal_of_command();
    new spell_pal_divine_intervention();
    new spell_pal_seal_of_light();
    new spell_pal_sacred_shield_base();

    // Theirs
    new spell_pal_ardent_defender();
    new spell_pal_avenging_wrath();
    new spell_pal_blessing_of_faith();
    new spell_pal_blessing_of_sanctuary();
    new spell_pal_divine_sacrifice();
    new spell_pal_divine_storm();
    new spell_pal_divine_storm_dummy();
    new spell_pal_exorcism_and_holy_wrath_damage();
    new spell_pal_eye_for_an_eye();
    new spell_pal_glyph_of_holy_light();
    new spell_pal_guarded_by_the_light();
    new spell_pal_hand_of_sacrifice();
    new spell_pal_hand_of_salvation();
    new spell_pal_holy_shock();
    new spell_pal_judgement("spell_pal_judgement_of_justice", SPELL_PALADIN_JUDGEMENT_OF_JUSTICE);
    new spell_pal_judgement("spell_pal_judgement_of_light", SPELL_PALADIN_JUDGEMENT_OF_LIGHT);
    new spell_pal_judgement("spell_pal_judgement_of_wisdom", SPELL_PALADIN_JUDGEMENT_OF_WISDOM);
    new spell_pal_judgement("spell_pal_judgement_of_shadow", SPELL_PALADIN_JUDGEMENT_OF_SHADOW);
    new spell_pal_judgement_of_command();
    new spell_pal_lay_on_hands();
    new spell_pal_righteous_defense();
    new spell_pal_seal_of_righteousness();
    new spell_pal_seal_of_corruption();
}
