-- Orgrimmar NPC Audit: MoP 5.4.8 level, health, and damage corrections
-- Date: 2026-06-28
--
-- Issues fixed:
--   1. Garrosh Hellscream (39605): level 3 → 90 (non-attackable; display fix)
--   2. Grunt Komak/Mojka (5597/5603): Health_mod 1.15 → 1.5, clear legacy damage values
--   3. General Nazgrim (54870): level 85 → 90
--   4. Gamon (6466): level 85 → 90
--   5. Darkspear Hold guards (12793-12796, level 55): → 90, clear legacy damage, exp → 3
--   6. Embassy dignitaries (12789-12791, 19850, level 62): → 90, clear legacy damage, exp → 3
--   7. Valley of Strength guards (54657-54659, level 65-70): → 90, exp → 3
--   8. Grunt Korf/Bek'rah (12797/12798, level 75): → 90, clear legacy damage, exp → 3
--   9. Silvermoon Delegation Guardian (46140, level 75): → 90, exp → 3

-- 1. Garrosh Hellscream throne room (non-attackable, level is just display)
UPDATE `creature_template` SET `minlevel`=90, `maxlevel`=90 WHERE `entry`=39605;

-- 2. Grunt Komak and Grunt Mojka: fix Health_mod and clear old-era hardcoded damage
UPDATE `creature_template`
SET `Health_mod`=1.5, `mindmg`=0, `maxdmg`=0, `attackpower`=0
WHERE `entry` IN (5597, 5603);

-- 3. General Nazgrim
UPDATE `creature_template` SET `minlevel`=90, `maxlevel`=90 WHERE `entry`=54870;

-- 4. Gamon
UPDATE `creature_template` SET `minlevel`=90, `maxlevel`=90 WHERE `entry`=6466;

-- 5. Darkspear Hold guards (level 55, Classic exp, hardcoded damage)
UPDATE `creature_template`
SET `minlevel`=90, `maxlevel`=90, `exp`=3, `mindmg`=0, `maxdmg`=0, `attackpower`=0
WHERE `entry` IN (12793, 12794, 12795, 12796);

-- 6. Embassy dignitaries (level 62, Classic/TBC exp, hardcoded damage)
UPDATE `creature_template`
SET `minlevel`=90, `maxlevel`=90, `exp`=3, `mindmg`=0, `maxdmg`=0, `attackpower`=0
WHERE `entry` IN (12789, 12790, 12791, 19850);

-- 7. Valley of Strength guards (level 65-70, Classic exp, damage already zeroed)
UPDATE `creature_template`
SET `minlevel`=90, `maxlevel`=90, `exp`=3
WHERE `entry` IN (54657, 54658, 54659);

-- 8. Grunt Korf and Grunt Bek'rah (level 75, WotLK exp, hardcoded damage)
UPDATE `creature_template`
SET `minlevel`=90, `maxlevel`=90, `exp`=3, `mindmg`=0, `maxdmg`=0, `attackpower`=0
WHERE `entry` IN (12797, 12798);

-- 9. Silvermoon Delegation Guardian (level 75, WotLK exp, damage already zeroed)
UPDATE `creature_template`
SET `minlevel`=90, `maxlevel`=90, `exp`=3
WHERE `entry`=46140;
