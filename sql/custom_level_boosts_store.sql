-- Level Boost products for the in-game Battle.net store
-- Run this against your WORLD database

-- Add a "Level Boosts" group to the store
INSERT INTO battle_pay_group (id, idx, name, icon, type) VALUES
(100, 10, 'Level Boosts', 0, 0)
ON DUPLICATE KEY UPDATE name='Level Boosts', idx=10;

-- Add boost products (type 2 = SERVICE, price 0 = free)
INSERT INTO battle_pay_product (id, title, description, icon, price, discount, displayId, type, choiceType, flags, flagsInfo) VALUES
(200, 'Level 60 Boost', 'Boost to level 60 with gear and 100 gold.', 0, 0, 0, 0, 2, 0, 0, 0),
(201, 'Level 70 Boost', 'Boost to level 70 with gear and 500 gold.', 0, 0, 0, 0, 2, 0, 0, 0),
(202, 'Level 85 Boost', 'Boost to level 85 with gear and 2000 gold.', 0, 0, 0, 0, 2, 0, 0, 0)
ON DUPLICATE KEY UPDATE title=VALUES(title), description=VALUES(description), type=2;

-- Product items (required - one per product for the delivery packet)
INSERT INTO battle_pay_product_items (id, itemId, count, productId) VALUES
(200, 6948, 1, 200),
(201, 6948, 1, 201),
(202, 6948, 1, 202)
ON DUPLICATE KEY UPDATE productId=VALUES(productId);

-- Shop entries linking products to the group
INSERT INTO battle_pay_entry (id, productId, groupId, idx, title, description, icon, displayId, banner, flags) VALUES
(200, 200, 100, 0, 'Level 60 Boost', 'Instantly boost your character to level 60 with appropriate gear.', 0, 0, 2, 0),
(201, 201, 100, 1, 'Level 70 Boost', 'Instantly boost your character to level 70 with appropriate gear.', 0, 0, 2, 0),
(202, 202, 100, 2, 'Level 85 Boost', 'Instantly boost your character to level 85 with appropriate gear.', 0, 0, 2, 0)
ON DUPLICATE KEY UPDATE title=VALUES(title), description=VALUES(description);
