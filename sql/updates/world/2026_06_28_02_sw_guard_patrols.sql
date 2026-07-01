-- Stormwind City Guard (entry 1976) — District Patrol Waypoints
-- Five looping patrol paths, one per major Stormwind district.
-- TC waypoint movement uses Catmull-Rom spline smoothing internally;
-- move_flag=0 (walk) is all that is needed. No explicit spline flag exists.
--
-- IMPORTANT: All X/Y/Z coordinates are approximate and MUST be verified
-- in-game with `.npc move` or by standing at each point and using `.gps`.
-- Z values in particular will differ on actual terrain.
--
-- After importing, run the STEP 4 SELECT to find guard GUIDs by position,
-- then fill in the creature_addon inserts at STEP 5.

-- ============================================================
-- STEP 1: Clean up any previously inserted paths for these IDs
-- ============================================================
DELETE FROM `waypoint_data` WHERE `id` IN (197601, 197602, 197603, 197604, 197605);

-- ============================================================
-- STEP 2: Insert patrol paths
-- move_flag: 0 = walk (guards should walk, not run)
-- delay: milliseconds paused at this point (corner/turnaround pauses)
-- ============================================================

-- Path 197601: Trade District
-- Loops the main Trade District road (near AH, banks, vendor row).
INSERT INTO `waypoint_data` (`id`, `point`, `position_x`, `position_y`, `position_z`, `orientation`, `delay`, `move_flag`, `action`, `action_chance`) VALUES
(197601, 1,  -8636.44, 406.52, 97.33, NULL, 0,    0, 0, 100),
(197601, 2,  -8673.82, 358.19, 99.81, NULL, 0,    0, 0, 100),
(197601, 3,  -8712.55, 330.10, 97.70, NULL, 1000, 0, 0, 100), -- SE corner pause
(197601, 4,  -8753.24, 359.57, 97.00, NULL, 0,    0, 0, 100),
(197601, 5,  -8748.03, 403.82, 97.22, NULL, 0,    0, 0, 100),
(197601, 6,  -8720.04, 432.48, 98.05, NULL, 0,    0, 0, 100),
(197601, 7,  -8679.34, 448.73, 97.10, NULL, 1000, 0, 0, 100), -- NW corner pause
(197601, 8,  -8642.52, 432.25, 97.28, NULL, 0,    0, 0, 100);
-- TC loops back to point 1 automatically.

-- Path 197602: Cathedral Square
-- Circles the Cathedral of Light plaza.
INSERT INTO `waypoint_data` (`id`, `point`, `position_x`, `position_y`, `position_z`, `orientation`, `delay`, `move_flag`, `action`, `action_chance`) VALUES
(197602, 1,  -8793.52, 716.82, 109.22, NULL, 0,    0, 0, 100),
(197602, 2,  -8754.21, 695.48, 108.44, NULL, 0,    0, 0, 100),
(197602, 3,  -8731.03, 652.34, 107.55, NULL, 1000, 0, 0, 100),
(197602, 4,  -8750.85, 622.10, 107.00, NULL, 0,    0, 0, 100),
(197602, 5,  -8796.23, 608.52, 106.82, NULL, 0,    0, 0, 100),
(197602, 6,  -8836.82, 625.38, 107.22, NULL, 1000, 0, 0, 100),
(197602, 7,  -8855.04, 666.43, 108.55, NULL, 0,    0, 0, 100),
(197602, 8,  -8837.52, 707.91, 109.33, NULL, 0,    0, 0, 100);

-- Path 197603: Old Town
-- Patrols near the SI:7 headquarters and the barracks.
INSERT INTO `waypoint_data` (`id`, `point`, `position_x`, `position_y`, `position_z`, `orientation`, `delay`, `move_flag`, `action`, `action_chance`) VALUES
(197603, 1,  -8812.03, -185.33, 82.33, NULL, 0,    0, 0, 100),
(197603, 2,  -8782.55, -218.62, 81.44, NULL, 0,    0, 0, 100),
(197603, 3,  -8749.21, -249.04, 82.05, NULL, 1000, 0, 0, 100),
(197603, 4,  -8717.82, -232.43, 82.77, NULL, 0,    0, 0, 100),
(197603, 5,  -8705.03, -197.55, 83.22, NULL, 0,    0, 0, 100),
(197603, 6,  -8721.34, -165.72, 83.44, NULL, 1000, 0, 0, 100),
(197603, 7,  -8758.92, -148.54, 82.88, NULL, 0,    0, 0, 100),
(197603, 8,  -8797.55, -159.33, 82.55, NULL, 0,    0, 0, 100);

-- Path 197604: Dwarven District
-- Loops past the forge, gunshop, and the district's main thoroughfare.
INSERT INTO `waypoint_data` (`id`, `point`, `position_x`, `position_y`, `position_z`, `orientation`, `delay`, `move_flag`, `action`, `action_chance`) VALUES
(197604, 1,  -8273.24, 401.55, 115.11, NULL, 0,    0, 0, 100),
(197604, 2,  -8237.03, 382.33, 115.66, NULL, 0,    0, 0, 100),
(197604, 3,  -8215.52, 347.82, 116.00, NULL, 1000, 0, 0, 100),
(197604, 4,  -8231.04, 318.55, 115.33, NULL, 0,    0, 0, 100),
(197604, 5,  -8268.33, 302.52, 114.88, NULL, 0,    0, 0, 100),
(197604, 6,  -8308.04, 316.82, 115.00, NULL, 1000, 0, 0, 100),
(197604, 7,  -8325.52, 351.24, 115.55, NULL, 0,    0, 0, 100),
(197604, 8,  -8308.04, 386.82, 115.33, NULL, 0,    0, 0, 100);

-- Path 197605: Mage Quarter
-- Patrols near the Slaughtered Lamb, mage tower, and the quarter's road.
INSERT INTO `waypoint_data` (`id`, `point`, `position_x`, `position_y`, `position_z`, `orientation`, `delay`, `move_flag`, `action`, `action_chance`) VALUES
(197605, 1,  -8987.52, 432.82, 97.77, NULL, 0,    0, 0, 100),
(197605, 2,  -8962.04, 401.55, 97.22, NULL, 0,    0, 0, 100),
(197605, 3,  -8934.33, 380.24, 96.88, NULL, 1000, 0, 0, 100),
(197605, 4,  -8912.82, 398.55, 97.00, NULL, 0,    0, 0, 100),
(197605, 5,  -8904.24, 432.13, 97.44, NULL, 0,    0, 0, 100),
(197605, 6,  -8918.55, 467.33, 97.22, NULL, 1000, 0, 0, 100),
(197605, 7,  -8948.24, 482.55, 97.88, NULL, 0,    0, 0, 100),
(197605, 8,  -8978.82, 465.04, 97.66, NULL, 0,    0, 0, 100);

-- ============================================================
-- STEP 3: Enable waypoint movement on the guard template
-- MovementType 2 = WaypointMovementGenerator
-- Individual path assignment is done via creature_addon (STEP 5).
-- ============================================================
UPDATE `creature_template` SET `MovementType` = 2 WHERE `entry` = 1976;

-- ============================================================
-- STEP 4: Identify guard spawns by position (run this SELECT first)
-- Use the output to fill in STEP 5 with the correct GUIDs.
-- Each guard in a given district should get that district's path_id.
-- ============================================================
-- SELECT `guid`, ROUND(`position_x`,1) AS x, ROUND(`position_y`,1) AS y, ROUND(`position_z`,1) AS z
-- FROM `creature`
-- WHERE `id` = 1976
-- ORDER BY `position_y` DESC;

-- ============================================================
-- STEP 5: Assign paths per spawn via creature_addon
-- Replace the example GUIDs below with your actual values from STEP 4.
-- Guards sharing a district should share a path_id.
-- Offset guards in the SAME district to different starting points
-- using SET currentwaypoint = N in the creature table to stagger them
-- so they do not stack on top of each other.
-- ============================================================
-- Example (replace GUIDs with real values):
--
-- DELETE FROM `creature_addon` WHERE `guid` IN (11111, 22222, 33333, 44444, 55555);
-- INSERT INTO `creature_addon` (`guid`, `path_id`, `mount`, `bytes1`, `bytes2`, `emote`, `auras`) VALUES
-- (11111, 197601, 0, 0, 0, 0, NULL),  -- Trade District guard A
-- (22222, 197601, 0, 0, 0, 0, NULL),  -- Trade District guard B (stagger via currentwaypoint)
-- (33333, 197602, 0, 0, 0, 0, NULL),  -- Cathedral Square guard
-- (44444, 197603, 0, 0, 0, 0, NULL),  -- Old Town guard
-- (55555, 197604, 0, 0, 0, 0, NULL);  -- Dwarven District guard
--
-- To stagger guards sharing a path so they don't stack at point 1 on startup:
-- UPDATE `creature` SET `currentwaypoint` = 4 WHERE `guid` = 22222;

-- ============================================================
-- STEP 6: Verify no movement-blocking flags are set on guards.
-- UNIT_FLAG_DISABLE_MOVE (0x4) hard-roots the unit.
-- UNIT_FLAG_STUNNED      (0x40000) also stops movement.
-- Result should be empty. If rows appear, clear the flags.
-- ============================================================
-- SELECT c.`guid`, c.`unit_flags`, ct.`unit_flags` AS template_flags
-- FROM `creature` c
-- JOIN `creature_template` ct ON ct.`entry` = c.`id`
-- WHERE c.`id` = 1976
--   AND (c.`unit_flags` & (0x4 | 0x40000) != 0
--        OR ct.`unit_flags` & (0x4 | 0x40000) != 0);
--
-- Fix if needed:
-- UPDATE `creature`          SET `unit_flags` = `unit_flags` & ~(0x4 | 0x40000) WHERE `id` = 1976;
-- UPDATE `creature_template` SET `unit_flags` = `unit_flags` & ~(0x4 | 0x40000) WHERE `entry` = 1976;
