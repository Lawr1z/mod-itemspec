/*
 * Item Specialization Module for AzerothCore
 * ELUNA BRIDGE SYSTEM - Uses spell IDs as triggers
 */

#include "ScriptMgr.h"
#include "Player.h"
#include "Chat.h"
#include "World.h"
#include "DatabaseEnv.h"
#include "ScriptedGossip.h"
#include "SpellScript.h"
#include "SpellAuraEffects.h"
#include "SpellMgr.h"

// Configuration - High spell IDs that won't conflict
constexpr uint32 SPELL_ITEMSPEC_OPEN_UI = 3000001;     // Eluna casts this to open UI
constexpr uint32 SPELL_ITEMSPEC_SAVE_SPEC_1 = 3000002; // Save first spec
constexpr uint32 SPELL_ITEMSPEC_SAVE_SPEC_2 = 3000003; // Save second spec
constexpr uint32 SPELL_ITEMSPEC_SAVE_SPEC_3 = 3000004; // Save third spec
constexpr uint32 SPELL_ITEMSPEC_GET_CURRENT = 3000005; // Request current spec
constexpr uint32 SPELL_ITEMSPEC_PLAY_SELECT_SOUND = 3000006; // Play select sound
constexpr uint32 SPELL_ITEMSPEC_PLAY_SAVE_SOUND = 3000007;   // Play save sound

constexpr uint32 CREATURE_TRIGGER = 500001;

// Spec definitions for each class
struct SpecData
{
    uint8 classId;
    std::vector<std::string> specs;
};

const std::vector<SpecData> CLASS_SPECS = {
    {1, {"Arms", "Fury", "Protection"}},
    {2, {"Holy", "Protection", "Retribution"}},
    {3, {"Beast Mastery", "Marksmanship", "Survival"}},
    {4, {"Assassination", "Combat", "Subtlety"}},
    {5, {"Discipline", "Holy", "Shadow"}},
    {6, {"Blood", "Frost", "Unholy"}},
    {7, {"Elemental", "Enhancement", "Restoration"}},
    {8, {"Arcane", "Fire", "Frost"}},
    {9, {"Affliction", "Demonology", "Destruction"}},
    {11, {"Balance", "Feral", "Restoration"}}
};

// Helper function to get current saved spec
std::string GetPlayerSpec(Player* player)
{
    uint32 guid = player->GetGUID().GetCounter();
    QueryResult result = CharacterDatabase.Query("SELECT spec FROM eluna_eunoia_itemspec WHERE guid = {}", guid);
    
    if (result)
    {
        Field* fields = result->Fetch();
        return fields[0].Get<std::string>();
    }
    return "Unset";
}

// Helper function to save spec
void SavePlayerSpec(Player* player, const std::string& specName)
{
    uint32 guid = player->GetGUID().GetCounter();
    
    CharacterDatabase.Execute(
        "INSERT INTO eluna_eunoia_itemspec (guid, spec) VALUES ({}, '{}') "
        "ON DUPLICATE KEY UPDATE spec = '{}'",
        guid, specName, specName
    );
}

// Helper to get spec by index for player's class
std::string GetSpecByIndex(Player* player, uint8 index)
{
    uint8 playerClass = player->getClass();
    
    for (const auto& classSpec : CLASS_SPECS)
    {
        if (classSpec.classId == playerClass)
        {
            if (index < classSpec.specs.size())
            {
                return classSpec.specs[index];
            }
            break;
        }
    }
    return "";
}

// Spell Script: Open UI (3000001)
// Eluna casts this when player interacts with NPC or uses item
class spell_itemspec_open_ui : public SpellScript
{
    PrepareSpellScript(spell_itemspec_open_ui);

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        if (Unit* caster = GetCaster())
        {
            if (Player* player = caster->ToPlayer())
            {
                // Play open sound
                player->PlayDirectSound(830, player);
                
                // This is where Eluna would receive the trigger
                // The actual UI opening is handled by client-side addon
                // No need to send anything - Eluna already knows to open UI
            }
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_itemspec_open_ui::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// Spell Script: Save Spec 1 (3000002)
class spell_itemspec_save_spec_1 : public SpellScript
{
    PrepareSpellScript(spell_itemspec_save_spec_1);

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        if (Unit* caster = GetCaster())
        {
            if (Player* player = caster->ToPlayer())
            {
                std::string spec = GetSpecByIndex(player, 0);
                if (!spec.empty())
                {
                    SavePlayerSpec(player, spec);
                    // Eluna will handle the UI update
                }
            }
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_itemspec_save_spec_1::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// Spell Script: Save Spec 2 (3000003)
class spell_itemspec_save_spec_2 : public SpellScript
{
    PrepareSpellScript(spell_itemspec_save_spec_2);

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        if (Unit* caster = GetCaster())
        {
            if (Player* player = caster->ToPlayer())
            {
                std::string spec = GetSpecByIndex(player, 1);
                if (!spec.empty())
                {
                    SavePlayerSpec(player, spec);
                }
            }
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_itemspec_save_spec_2::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// Spell Script: Save Spec 3 (3000004)
class spell_itemspec_save_spec_3 : public SpellScript
{
    PrepareSpellScript(spell_itemspec_save_spec_3);

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        if (Unit* caster = GetCaster())
        {
            if (Player* player = caster->ToPlayer())
            {
                std::string spec = GetSpecByIndex(player, 2);
                if (!spec.empty())
                {
                    SavePlayerSpec(player, spec);
                }
            }
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_itemspec_save_spec_3::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// Spell Script: Get Current Spec (3000005)
// Returns current spec - Eluna can query the DB after this
class spell_itemspec_get_current : public SpellScript
{
    PrepareSpellScript(spell_itemspec_get_current);

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        if (Unit* caster = GetCaster())
        {
            if (Player* player = caster->ToPlayer())
            {
                // Just a trigger - Eluna will query DB
                // Or we could store in a temporary variable Eluna can access
                std::string spec = GetPlayerSpec(player);
                // The spec is now in DB, Eluna reads it
            }
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_itemspec_get_current::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// Spell Script: Play Select Sound (3000006)
class spell_itemspec_play_select_sound : public SpellScript
{
    PrepareSpellScript(spell_itemspec_play_select_sound);

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        if (Unit* caster = GetCaster())
        {
            if (Player* player = caster->ToPlayer())
            {
                player->PlayDirectSound(1184, player);
            }
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_itemspec_play_select_sound::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// Spell Script: Play Save Sound (3000007)
class spell_itemspec_play_save_sound : public SpellScript
{
    PrepareSpellScript(spell_itemspec_play_save_sound);

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        if (Unit* caster = GetCaster())
        {
            if (Player* player = caster->ToPlayer())
            {
                player->PlayDirectSound(1201, player);
            }
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_itemspec_play_save_sound::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// World Script: Create table and register spell scripts
class ItemSpec_WorldScript : public WorldScript
{
public:
    ItemSpec_WorldScript() : WorldScript("ItemSpec_WorldScript") { }

    void OnStartup() override
    {
        // Create database table
        CharacterDatabase.Execute(
            "CREATE TABLE IF NOT EXISTS `eluna_eunoia_itemspec` ("
            "`guid` INT UNSIGNED NOT NULL,"
            "`spec` VARCHAR(50) NOT NULL,"
            "PRIMARY KEY (`guid`)"
            ") ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;"
        );
    }
};

// Optional: Creature script for gossip fallback
class ItemSpec_CreatureScript : public CreatureScript
{
public:
    ItemSpec_CreatureScript() : CreatureScript("ItemSpec_CreatureScript") { }

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        if (creature->GetEntry() != CREATURE_TRIGGER)
            return false;

        // Cast spell to trigger Eluna
        player->CastSpell(player, SPELL_ITEMSPEC_OPEN_UI, true);
        player->PlayerTalkClass->SendCloseGossip();
        
        return true;
    }
};

// Command Script for testing/manual use
class ItemSpec_CommandScript : public CommandScript
{
public:
    ItemSpec_CommandScript() : CommandScript("ItemSpec_CommandScript") { }

    std::vector<Acore::ChatCommands::ChatCommandBuilder> GetCommands() const override
    {
        using namespace Acore::ChatCommands;
        
        static ChatCommandTable commandTable =
        {
            { "itemspec set", HandleItemSpecSetCommand, SEC_PLAYER, Console::No },
            { "itemspec get", HandleItemSpecGetCommand, SEC_PLAYER, Console::No },
        };

        return commandTable;
    }

    static bool HandleItemSpecSetCommand(ChatHandler* handler, std::string const& specName)
    {
        Player* player = handler->GetSession()->GetPlayer();
        if (!player)
            return false;

        if (specName.empty())
        {
            handler->SendSysMessage("Usage: .itemspec set <specname>");
            return false;
        }

        SavePlayerSpec(player, specName);
        player->PlayDirectSound(1201, player);
        handler->PSendSysMessage("Item spec set to: {}", specName);

        return true;
    }

    static bool HandleItemSpecGetCommand(ChatHandler* handler)
    {
        Player* player = handler->GetSession()->GetPlayer();
        if (!player)
            return false;

        std::string spec = GetPlayerSpec(player);
        handler->PSendSysMessage("Current spec: {}", spec);
        return true;
    }
};

// Register all scripts
void AddSC_ItemSpec()
{
    new ItemSpec_WorldScript();
    new ItemSpec_CreatureScript();
    new ItemSpec_CommandScript();
    
    // Register spell scripts
    RegisterSpellScript(spell_itemspec_open_ui);
    RegisterSpellScript(spell_itemspec_save_spec_1);
    RegisterSpellScript(spell_itemspec_save_spec_2);
    RegisterSpellScript(spell_itemspec_save_spec_3);
    RegisterSpellScript(spell_itemspec_get_current);
    RegisterSpellScript(spell_itemspec_play_select_sound);
    RegisterSpellScript(spell_itemspec_play_save_sound);
}
