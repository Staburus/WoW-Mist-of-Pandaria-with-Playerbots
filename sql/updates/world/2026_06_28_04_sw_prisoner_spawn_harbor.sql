-- 2026_06_28_04_sw_prisoner_spawn_harbor.sql
--
-- PROBLEM: NPC 53527 (Prisoner, guid 302870) spawned at Cathedral Square
--          (-8809, 742, 97.9), but EscortAI WP1 is at the Harbor dock
--          (-8392, 1338, 7.5).  The ~727-unit gap made MMAP pathfinding
--          fail silently on the first move, leaving the prisoner stationary.
--
-- FIX: Relocate the DB spawn to the Harbor dock, just north of WP1,
--      so the NPC walks only a few steps before WaypointReached(1) fires.
--      No C++ changes required — only the spawn position changes.

UPDATE `creature`
SET
    `position_x`  = -8392.0,
    `position_y`  = 1356.0,
    `position_z`  = 7.5,
    `orientation` = 3.54,      -- roughly south-southwest, toward the harbor ramp exit
    `wander_distance` = 0,
    `MovementType` = 0
WHERE `guid` = 302870;

-- Verify
SELECT `guid`,
       ROUND(`position_x`, 2) AS x,
       ROUND(`position_y`, 2) AS y,
       ROUND(`position_z`, 2) AS z,
       ROUND(`orientation`, 3) AS o,
       `MovementType`
FROM `creature`
WHERE `guid` = 302870;
