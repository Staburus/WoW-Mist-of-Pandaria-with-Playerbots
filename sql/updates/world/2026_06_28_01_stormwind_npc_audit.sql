-- Stormwind City NPC Audit: MoP 5.4.8 level, health, and damage corrections
-- Date: 2026-06-28
--
-- Fixes:
--   1. Faction leaders (King Varian, Genn Greymane): level 3 -> 90 (non-attackable)
--   2. City guards / PvP officers (levels 15-78): -> 90, clear legacy damage, exp -> 3
--   3. Harbor / dock NPCs (level 65): -> 90, clear legacy damage, exp -> 3
--   4. Named military NPCs (levels 57-87): -> 90, clear legacy damage
--   5. Support NPCs near max level (81-85): -> 90

-- -----------------------------------------------------------------------
-- 1. Non-attackable faction leaders (level display fix only)
-- -----------------------------------------------------------------------
UPDATE `creature_template` SET `minlevel`=90, `maxlevel`=90, `mindmg`=0, `maxdmg`=0, `attackpower`=0
WHERE `entry`=29611; -- King Varian Wrynn

UPDATE `creature_template` SET `minlevel`=90, `maxlevel`=90
WHERE `entry`=45253; -- Genn Greymane

-- -----------------------------------------------------------------------
-- 2. City guards and PvP officers (Classic/TBC exp, hardcoded damage)
-- -----------------------------------------------------------------------

-- Stormwind Guard (the only entry - spawned in 4 locations at level 15)
UPDATE `creature_template`
SET `minlevel`=90, `maxlevel`=90, `exp`=3, `mindmg`=0, `maxdmg`=0, `attackpower`=0
WHERE `entry`=1423;

-- Champions Hall PvP officers (level 55, exp 0)
UPDATE `creature_template`
SET `minlevel`=90, `maxlevel`=90, `exp`=3, `mindmg`=0, `maxdmg`=0, `attackpower`=0
WHERE `entry` IN (12779, 12780, 12781, 12783, 12784, 12785, 12805);
-- 12779 Archmage Gaiman, 12780 Sergeant Major Skyshadow,
-- 12781 Master Sergeant Biggins, 12783 Lieutenant Karter,
-- 12784 Lieutenant Jackspring, 12785 Sergeant Major Clate,
-- 12805 Officer Areyn

-- Hall of Champions guards (levels 57-58)
UPDATE `creature_template`
SET `minlevel`=90, `maxlevel`=90, `exp`=3, `mindmg`=0, `maxdmg`=0, `attackpower`=0
WHERE `entry` IN (12480, 12481);
-- 12480 Melris Malagan (58), 12481 Justine Demalier (57)

-- Stormwind Keep guards (level 75)
UPDATE `creature_template`
SET `minlevel`=90, `maxlevel`=90, `exp`=3, `mindmg`=0, `maxdmg`=0, `attackpower`=0
WHERE `entry` IN (12786, 12787);
-- 12786 Guard Quine, 12787 Guard Hammon

-- Blizzard dev NPCs in guard positions (level 55, faction 11)
UPDATE `creature_template`
SET `minlevel`=90, `maxlevel`=90, `exp`=3, `mindmg`=0, `maxdmg`=0, `attackpower`=0
WHERE `entry` IN (29292, 29293, 29294, 29295);
-- Art Peshkov, Daniel Kramer, Candace Thomas, Meghan Dawson

-- Recruiter Burns (level 78)
UPDATE `creature_template`
SET `minlevel`=90, `maxlevel`=90, `exp`=3
WHERE `entry`=36799;

-- Stormwind Harbor military (level 65-75, exp 0)
UPDATE `creature_template`
SET `minlevel`=90, `maxlevel`=90, `exp`=3
WHERE `entry` IN (54660, 54661, 54662);
-- 54660 Lieutenant Tristia (75), 54661 Captain Dirgehammer (65),
-- 54662 Knight-Lieutenant T'Maire Sydes (75)

-- -----------------------------------------------------------------------
-- 3. Harbor / dock NPCs (level 65, hardcoded Cata-era damage)
-- -----------------------------------------------------------------------
UPDATE `creature_template`
SET `minlevel`=90, `maxlevel`=90, `exp`=3, `mindmg`=0, `maxdmg`=0, `attackpower`=0
WHERE `entry` IN (29019, 29088, 29152, 29154);
-- 29019 Dockhand, 29088 Stormwind Cannoneer,
-- 29152 Stormwind Dock Worker, 29154 Thargold Ironwing

-- -----------------------------------------------------------------------
-- 4. Named military / important NPCs
-- -----------------------------------------------------------------------

-- Field Marshal Afrasiabi (level 62, Health_mod 16 elite)
UPDATE `creature_template`
SET `minlevel`=90, `maxlevel`=90, `exp`=3, `mindmg`=0, `maxdmg`=0, `attackpower`=0
WHERE `entry`=14721;

-- Major Mattingly (level 60, Health_mod 16 elite)
UPDATE `creature_template`
SET `minlevel`=90, `maxlevel`=90, `exp`=3, `mindmg`=0, `maxdmg`=0, `attackpower`=0
WHERE `entry`=14394;

-- Master Mathias Shaw - head of SI:7 (level 62)
UPDATE `creature_template`
SET `minlevel`=90, `maxlevel`=90, `exp`=3, `mindmg`=0, `maxdmg`=0, `attackpower`=0
WHERE `entry`=332;

-- Grand Admiral Jes-Tereth (level 62)
UPDATE `creature_template`
SET `minlevel`=90, `maxlevel`=90, `exp`=3, `mindmg`=0, `maxdmg`=0, `attackpower`=0
WHERE `entry`=1750;

-- Harbinger Ennarth (level 69, Health_mod 2 elite)
UPDATE `creature_template`
SET `minlevel`=90, `maxlevel`=90, `exp`=3, `mindmg`=0, `maxdmg`=0, `attackpower`=0
WHERE `entry`=19848;

-- Master Wood (level 70, Health_mod 3 elite)
UPDATE `creature_template`
SET `minlevel`=90, `maxlevel`=90, `exp`=3, `mindmg`=0, `maxdmg`=0, `attackpower`=0
WHERE `entry`=8383;

-- General Marcus Jonathan - Stormwind commander (level 87)
UPDATE `creature_template`
SET `minlevel`=90, `maxlevel`=90, `mindmg`=0, `maxdmg`=0, `attackpower`=0
WHERE `entry`=466;

-- Major Samuelson (level 84, hardcoded old damage values)
UPDATE `creature_template`
SET `minlevel`=90, `maxlevel`=90, `mindmg`=0, `maxdmg`=0, `attackpower`=0
WHERE `entry`=2439;

-- -----------------------------------------------------------------------
-- 5. Near-cap support NPCs (81-85 -> 90)
-- -----------------------------------------------------------------------
UPDATE `creature_template` SET `minlevel`=90, `maxlevel`=90 WHERE `entry`=44749; -- Supply Sergeant Graves (81)
UPDATE `creature_template` SET `minlevel`=90, `maxlevel`=90 WHERE `entry`=45226; -- Naraat the Earthspeaker (82)
UPDATE `creature_template` SET `minlevel`=90, `maxlevel`=90 WHERE `entry`=44806; -- Fargo Flintlocke (84)
UPDATE `creature_template` SET `minlevel`=90, `maxlevel`=90 WHERE `entry`=52654; -- Bwemba (85)
UPDATE `creature_template` SET `minlevel`=90, `maxlevel`=90 WHERE `entry`=52806; -- Darkspear Warrior (85)
UPDATE `creature_template` SET `minlevel`=90, `maxlevel`=90 WHERE `entry`=36674; -- Nambria (85)
UPDATE `creature_template` SET `minlevel`=90, `maxlevel`=90 WHERE `entry`=42288; -- Robby Flay (85)
UPDATE `creature_template` SET `minlevel`=90, `maxlevel`=90 WHERE `entry`=50424; -- Terran "Justice" Gregory (85)
UPDATE `creature_template` SET `minlevel`=90, `maxlevel`=90 WHERE `entry`=49748; -- Hero's Herald (85)
UPDATE `creature_template` SET `minlevel`=90, `maxlevel`=90 WHERE `entry`=55789; -- Rell Nightwind (85)
UPDATE `creature_template` SET `minlevel`=90, `maxlevel`=90 WHERE `entry`=56925; -- Farrah Facet (85)
UPDATE `creature_template` SET `minlevel`=90, `maxlevel`=90 WHERE `entry` IN (47324, 47325); -- Baradin Guards (85)
