/*
 * Item Specialization Module for AzerothCore
 * Converted from Eluna Lua scripts
 * Original by FURIOZ
 */

#include "ScriptMgr.h"
#include "Player.h"
#include "Chat.h"
#include "World.h"
#include "DatabaseEnv.h"
#include "ScriptedGossip.h"
#include "Optional.h"

// Configuration
constexpr uint32 CREATURE_TRIGGER = 500001;

// Database table structure:
// CREATE TABLE IF NOT EXISTS `eluna_eunoia_itemspec` (
//   `guid` INT UNSIGNED NOT NULL,
//   `spec` VARCHAR(50) NOT NULL,
//   PRIMARY KEY (`guid`)
// ) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

// Spec definitions for each class
struct SpecData
{
    uint8 classId;
    std::vector<std::string> specs;
};

const std::vector<SpecData> CLASS_SPECS = {
    {1, {"Arms", "Fury", "Protection"}},              // Warrior
    {2, {"Holy", "Protection", "Retribution"}},       // Paladin
    {3, {"Beast Mastery", "Marksmanship", "Survival"}}, // Hunter
    {4, {"Assassination", "Combat", "Subtlety"}},     // Rogue
    {5, {"Discipline", "Holy", "Shadow"}},            // Priest
    {6, {"Blood", "Frost", "Unholy"}},                // Death Knight
    {7, {"Elemental", "Enhancement", "Restoration"}}, // Shaman
    {8, {"Arcane", "Fire", "Frost"}},                 // Mage
    {9, {"Affliction", "Demonology", "Destruction"}}, // Warlock
    {11, {"Balance", "Feral", "Restoration"}}         // Druid
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
    
    // Play save sound
    player->PlayDirectSound(1201, player);
    
    ChatHandler(player->GetSession()).PSendSysMessage("Item spec set to: {}", specName);
}

class ItemSpec_CreatureScript : public CreatureScript
{
public:
    ItemSpec_CreatureScript() : CreatureScript("ItemSpec_CreatureScript") { }

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        if (creature->GetEntry() != CREATURE_TRIGGER)
            return false;

        // Play sound
        player->PlayDirectSound(830, player);
        
        ClearGossipMenuFor(player);
        
        // Get player's class
        uint8 playerClass = player->getClass();
        
        // Show current spec
        std::string currentSpec = GetPlayerSpec(player);
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Current Spec: " + currentSpec, GOSSIP_SENDER_MAIN, 0);
        
        // Find specs for this class
        for (const auto& classSpec : CLASS_SPECS)
        {
            if (classSpec.classId == playerClass)
            {
                uint32 action = 100; // Start action IDs at 100
                for (const auto& spec : classSpec.specs)
                {
                    AddGossipItemFor(player, GOSSIP_ICON_TRAINER, "Set Spec: " + spec, GOSSIP_SENDER_MAIN, action++);
                }
                break;
            }
        }
        
        SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
    {
        if (creature->GetEntry() != CREATURE_TRIGGER)
            return false;

        CloseGossipMenuFor(player);
        
        if (action == 0)
        {
            // Just showing current spec, reopen menu
            OnGossipHello(player, creature);
            return true;
        }
        
        if (action >= 100)
        {
            // Play select sound
            player->PlayDirectSound(1184, player);
            
            // Get the spec name based on action
            uint8 playerClass = player->getClass();
            uint32 specIndex = action - 100;
            
            for (const auto& classSpec : CLASS_SPECS)
            {
                if (classSpec.classId == playerClass)
                {
                    if (specIndex < classSpec.specs.size())
                    {
                        SavePlayerSpec(player, classSpec.specs[specIndex]);
                    }
                    break;
                }
            }
        }
        
        return true;
    }
};

class ItemSpec_WorldScript : public WorldScript
{
public:
    ItemSpec_WorldScript() : WorldScript("ItemSpec_WorldScript") { }

    void OnStartup() override
    {
        // Ensure database table exists
        CharacterDatabase.Execute(
            "CREATE TABLE IF NOT EXISTS `eluna_eunoia_itemspec` ("
            "`guid` INT UNSIGNED NOT NULL,"
            "`spec` VARCHAR(50) NOT NULL,"
            "PRIMARY KEY (`guid`)"
            ") ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;"
        );
    }
};

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
            handler->SendSysMessage("Example: .itemspec set Arms");
            return false;
        }

        SavePlayerSpec(player, specName);
        return true;
    }

    static bool HandleItemSpecGetCommand(ChatHandler* handler)
    {
        Player* player = handler->GetSession()->GetPlayer();
        if (!player)
            return false;

        std::string spec = GetPlayerSpec(player);
        handler->PSendSysMessage("Current spec: %s", spec.c_str());
        return true;
    }
};

// Register all scripts
void AddSC_ItemSpec()
{
    new ItemSpec_CreatureScript();
    new ItemSpec_WorldScript();
    new ItemSpec_CommandScript();
}