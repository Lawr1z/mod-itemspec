USE acore_characters;

CREATE TABLE IF NOT EXISTS eluna_eunoia_itemspec ( guid INT UNSIGNED NOT NULL, spec VARCHAR(50) NOT NULL, PRIMARY KEY (guid) ) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

USE acore_world;

-- Create creature template DELETE FROM creature_template WHERE entry = 500001; INSERT INTO creature_template (entry, name, subname, minlevel, maxlevel, faction, npcflag, scale, rank, unit_class, unit_flags, type, AIName, ScriptName) VALUES (500001, 'Item Spec Manager', 'Click to set your specialization', 80, 80, 35, 1, 1, 0, 1, 2, 7, '', 'ItemSpec_CreatureScript');

-- Add NPC model DELETE FROM creature_template_model WHERE CreatureID = 500001; INSERT INTO creature_template_model (CreatureID, Idx, CreatureDisplayID, DisplayScale, Probability, VerifiedBuild) VALUES (500001, 0, 16925, 1, 1, 0);

command to create npc: .npc add 500001
