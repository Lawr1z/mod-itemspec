USE acore_characters; -- or your characters database name

CREATE TABLE IF NOT EXISTS eluna_eunoia_itemspec ( guid INT UNSIGNED NOT NULL, spec VARCHAR(50) NOT NULL, PRIMARY KEY (guid) ) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

USE acore_world; -- or your world database name

-- Create creature template DELETE FROM creature_template WHERE entry = 500001; INSERT INTO creature_template (entry, name, subname, minlevel, maxlevel, faction, npcflag, scale, rank, unit_class, unit_flags, type, AIName, ScriptName) VALUES (500001, 'Item Spec Manager', 'Click to set your specialization', 80, 80, 35, 1, 1, 0, 1, 2, 7, '', 'ItemSpec_CreatureScript');

-- Add NPC model DELETE FROM creature_template_model WHERE CreatureID = 500001; INSERT INTO creature_template_model (CreatureID, Idx, CreatureDisplayID, DisplayScale, Probability, VerifiedBuild) VALUES (500001, 0, 16925, 1, 1, 0);

.npc add 500001




-----------------------------------------
full readme created by ai

# ItemSpec Module for AzerothCore

A specialization preference storage system for WoW WotLK 3.3.5a servers running AzerothCore.

**Original Eluna Script by:** FURIOZ  
**Converted to C++ for AzerothCore**

---

## 📋 What Does This Module Do?

This module allows players to **save their preferred specialization** to a database. It does **NOT** change talents or switch specs - it only stores a preference that other custom scripts can use.

### Use Cases:
- 🎁 Custom loot systems that give spec-appropriate gear
- ⚔️ Boss scripts that drop items based on player spec
- 🏪 Smart vendors that show only relevant items
- 📊 Server statistics and spec popularity tracking
- 🎯 Automated reward distribution systems

---

## 🚀 Installation Guide

### Step 1: Module Setup

Create the module directory structure:

```bash
cd /path/to/azerothcore-wotlk/modules
mkdir -p mod-itemspec/src
```

### Step 2: Add Files

Create these files in your module:

**`modules/mod-itemspec/CMakeLists.txt`**
```cmake
AC_ADD_SCRIPT("${CMAKE_CURRENT_LIST_DIR}/src/ItemSpec.cpp")
```

**`modules/mod-itemspec/src/ItemSpec.cpp`**
- Copy the complete C++ code from the artifact

### Step 3: Compile AzerothCore

```bash
cd /path/to/azerothcore-wotlk/build
cmake ../ -DCMAKE_INSTALL_PREFIX=$HOME/azerothcore
make -j $(nproc)
make install
```

### Step 4: Database Setup

The database table is **automatically created** when worldserver starts!

If you want to create it manually:

```sql
USE acore_characters; -- or your characters database name

CREATE TABLE IF NOT EXISTS `eluna_eunoia_itemspec` (
  `guid` INT UNSIGNED NOT NULL,
  `spec` VARCHAR(50) NOT NULL,
  PRIMARY KEY (`guid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;
```

### Step 5: Create the NPC

Run this SQL on your **world database**:

```sql
USE acore_world; -- or your world database name

-- Create creature template
DELETE FROM `creature_template` WHERE `entry` = 500001;
INSERT INTO `creature_template` 
(`entry`, `name`, `subname`, `minlevel`, `maxlevel`, `faction`, `npcflag`, 
`scale`, `rank`, `unit_class`, `unit_flags`, `type`, `AIName`, `ScriptName`) 
VALUES 
(500001, 'Item Spec Manager', 'Click to set your specialization', 80, 80, 35, 1, 
1, 0, 1, 2, 7, '', 'ItemSpec_CreatureScript');

-- Add NPC model
DELETE FROM `creature_template_model` WHERE `CreatureID` = 500001;
INSERT INTO `creature_template_model` 
(`CreatureID`, `Idx`, `CreatureDisplayID`, `DisplayScale`, `Probability`, `VerifiedBuild`) 
VALUES 
(500001, 0, 16925, 1, 1, 0);
```

### Step 6: Start Server

```bash
./worldserver
```

Check the logs for:
```
>> Loaded custom script: ItemSpec_CreatureScript
>> Loaded custom script: ItemSpec_CommandScript
```

### Step 7: Spawn the NPC

In-game as GM:

```
.npc add 500001
```

Or spawn permanently via SQL (use `.gps` to get coordinates first):

```sql
INSERT INTO `creature` 
(`id`, `map`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`) 
VALUES 
(500001, 0, -8833.38, 622.45, 94.26, 3.61, 300);
-- Replace coordinates with your desired location
```

---

## 🎮 How to Use

### For Players:

#### Method 1: Use the NPC
1. Find the **Item Spec Manager** NPC
2. Talk to it
3. See your current saved spec
4. Click on a spec to save it
5. Confirmation message + sound plays

#### Method 2: Use Commands
```
.itemspec set Arms        # Set your spec to Arms
.itemspec get             # Check current saved spec
```

### Available Specs by Class:

| Class | Specializations |
|-------|----------------|
| **Warrior** | Arms, Fury, Protection |
| **Paladin** | Holy, Protection, Retribution |
| **Hunter** | Beast Mastery, Marksmanship, Survival |
| **Rogue** | Assassination, Combat, Subtlety |
| **Priest** | Discipline, Holy, Shadow |
| **Death Knight** | Blood, Frost, Unholy |
| **Shaman** | Elemental, Enhancement, Restoration |
| **Mage** | Arcane, Fire, Frost |
| **Warlock** | Affliction, Demonology, Destruction |
| **Druid** | Balance, Feral, Restoration |

---

## 🔧 For Developers: Using Stored Specs

### Query Player Spec in Your Scripts

```cpp
#include "DatabaseEnv.h"

std::string GetPlayerItemSpec(Player* player)
{
    uint32 guid = player->GetGUID().GetCounter();
    QueryResult result = CharacterDatabase.Query(
        "SELECT spec FROM eluna_eunoia_itemspec WHERE guid = {}", guid
    );
    
    if (result)
    {
        Field* fields = result->Fetch();
        return fields[0].Get<std::string>();
    }
    return "Unset";
}
```

### Example: Custom Loot Script

```cpp
class MyBossLoot : public CreatureScript
{
public:
    void OnDeath(Creature* creature, Unit* killer) override
    {
        if (Player* player = killer->ToPlayer())
        {
            std::string spec = GetPlayerItemSpec(player);
            
            if (spec == "Arms")
                player->AddItem(12345, 1); // 2H Weapon
            else if (spec == "Protection")
                player->AddItem(12346, 1); // Shield
            // etc...
        }
    }
};
```

### Example: Check All Players' Specs

```sql
-- See what specs are most popular
SELECT spec, COUNT(*) as count 
FROM eluna_eunoia_itemspec 
GROUP BY spec 
ORDER BY count DESC;
```

---

## 🐛 Troubleshooting

### Module doesn't load
- Check `worldserver.log` for errors
- Verify module folder: `ls -la modules/mod-itemspec/`
- Recompile: `make clean && make -j $(nproc)`

### Commands don't work
- Verify in logs: `>> Loaded custom script: ItemSpec_CommandScript`
- Try: `.help itemspec` to see if commands registered
- Restart worldserver after compilation

### NPC shows no gossip menu
- Check creature ScriptName: 
  ```sql
  SELECT entry, name, ScriptName FROM creature_template WHERE entry = 500001;
  ```
- Should show: `ItemSpec_CreatureScript`
- Update if needed:
  ```sql
  UPDATE creature_template SET ScriptName = 'ItemSpec_CreatureScript' WHERE entry = 500001;
  ```
- Reload: `.reload creature_template`
- Delete and respawn NPC

### NPC won't spawn ("has no model")
- Add model to `creature_template_model` table (see Step 5)
- Try different model IDs: 16925, 17122, 19646, 21263

### Messages show as "%s" or "{}"
- This was fixed in latest version
- Make sure you're using the updated `ItemSpec.cpp`
- Recompile after updating

### Database errors
- Check your database names in `worldserver.conf`
- Common names: `acore_characters`, `acore_world`
- Verify table exists:
  ```sql
  SHOW TABLES LIKE 'eluna_eunoia_itemspec';
  ```

---

## 📊 Database Structure

**Table:** `eluna_eunoia_itemspec` (in characters database)

| Column | Type | Description |
|--------|------|-------------|
| `guid` | INT UNSIGNED | Player's unique ID (Primary Key) |
| `spec` | VARCHAR(50) | Saved specialization name |

**Example data:**
```
+-------+---------------+
| guid  | spec          |
+-------+---------------+
| 12345 | Arms          |
| 67890 | Holy          |
| 11111 | Beast Mastery |
+-------+---------------+
```

---

## 🎵 Sound Effects

The module plays these WoW sounds:
- **830** - Menu open sound
- **1184** - Spec selection sound
- **1201** - Spec save confirmation sound

---

## 📝 Configuration

Change the NPC Entry ID by editing `ItemSpec.cpp`:

```cpp
constexpr uint32 CREATURE_TRIGGER = 500001; // Change this number
```

Then recompile.

---

## ⚠️ Important Notes

- This module **does NOT change your actual talent spec**
- It only stores a preference in the database
- Other scripts must be written to use this data
- Players can set any spec name via commands (validation optional)

---

## 🔄 Version Compatibility

Tested on:
- ✅ AzerothCore master branch
- ✅ AzerothCore 3.3.5a

Should work on any AzerothCore version with minor adjustments.

---

## 📜 Credits

**Original Eluna Scripts:** FURIOZ  
**Discord:** https://discord.gg/azrv6brSgh  
**Converted to C++:** AzerothCore Community

---

## 📄 License

Free to use for your private server. Credit appreciated!

---

## 🆘 Support

If you encounter issues:
1. Check the troubleshooting section above
2. Review worldserver logs for errors
3. Verify all SQL was executed correctly
4. Make sure module compiled without errors

---

**Enjoy your ItemSpec module!** 🎮✨
