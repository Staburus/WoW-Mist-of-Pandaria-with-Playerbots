-- Hallegosa (guid 571096, entry 53371) was spawning at Z=26.77 (inside Grommash Hold floor).
-- creature_template_addon already has AnimTier=3 (fly); only the Z position was wrong.
-- Move her up to Z=150, clearly above the Hold's roof (~Z=109).
UPDATE `creature` SET `position_z` = 150.0 WHERE `guid` = 571096;
