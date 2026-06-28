-- General Nazgrim (entry 54870) in Orgrimmar lacked a boarding gossip for
-- quest 31853 "All Aboard!". The original mechanic used a phased NPC at
-- Bladefist Bay + unimplemented scene spell 121545. Replaced with a C++ script
-- (npc_general_nazgrim_orgrimmar) that shows the gossip when the quest is active
-- and teleports the player to the Jade Forest landing site (map 870) on click.
UPDATE `creature_template` SET `ScriptName` = 'npc_general_nazgrim_orgrimmar' WHERE `entry` = 54870;
