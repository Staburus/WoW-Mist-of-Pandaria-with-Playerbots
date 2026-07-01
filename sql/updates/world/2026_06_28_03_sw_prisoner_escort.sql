-- NPC 53527 — Stormwind Harbor Prisoner Escort
-- Ambient event: prisoner walks from The Bravery dock to The Stockade entrance,
-- escorted by 4 summoned Stormwind City Guards (entry 68).
-- Repeats on the creature's natural respawn timer (~30 min recommended).
--
-- The C++ escort script (zone_stormwind_city.cpp) controls all movement;
-- MovementType must be 0 (idle/scripted) so WaypointMovementGenerator
-- does not compete with the EscortAI motion master.

-- ============================================================
-- STEP 1: Prisoner template — script, flags, movement
-- ============================================================
UPDATE `creature_template`
SET
    `ScriptName`  = 'npc_sw_prisoner_escort',
    `MovementType` = 0,
    -- Set UNIT_FLAG_NON_ATTACKABLE (0x2) | UNIT_FLAG_IMMUNE_TO_PC (0x100);
    -- clear UNIT_FLAG_DISABLE_MOVE (0x4) so the escort script can move the NPC.
    `unit_flags`  = (`unit_flags` | 0x2 | 0x100) & ~0x4
WHERE `entry` = 53527;

-- ============================================================
-- STEP 2: Prisoner dialog lines
-- GroupID 0 = SAY_ESCORT_BEGIN  (fired at waypoint 1)
-- GroupID 1 = SAY_ESCORT_END    (fired at final waypoint)
-- Type 12 = CHAT_MSG_MONSTER_SAY
-- ============================================================
DELETE FROM `creature_text` WHERE `CreatureID` = 53527;
INSERT INTO `creature_text`
    (`CreatureID`, `GroupID`, `ID`, `Text`, `Type`, `Language`, `Probability`, `Emote`, `Duration`, `Sound`, `SoundType`, `BroadcastTextId`, `TextRange`, `comment`)
VALUES
    (53527, 0, 0, 'You have no right to do this!',   12, 0, 100, 0, 0, 0, 0, 0, 0, 'Prisoner escort begin'),
    (53527, 1, 0, 'I''ll not forget this, I swear!', 12, 0, 100, 0, 0, 0, 0, 0, 0, 'Prisoner escort end / Stockade arrival');

-- ============================================================
-- STEP 3: Recommended respawn timer for a ~30-minute ambient cycle
-- 1800 seconds = 30 minutes. Adjust to taste.
-- ============================================================
-- UPDATE `creature`
-- SET `spawntimesecs` = 1800
-- WHERE `id` = 53527;

-- ============================================================
-- STEP 4: Verify the prisoner has a valid spawn on map 0
-- ============================================================
-- SELECT `guid`, `map`, ROUND(`position_x`,1) AS x, ROUND(`position_y`,1) AS y,
--        ROUND(`position_z`,1) AS z, `spawntimesecs`
-- FROM `creature`
-- WHERE `id` = 53527;
