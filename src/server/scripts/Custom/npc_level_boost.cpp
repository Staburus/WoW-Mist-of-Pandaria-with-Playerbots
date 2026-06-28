#include "ScriptPCH.h"
#include "ScriptedGossip.h"
#include "Player.h"
#include "Chat.h"
#include "World.h"

#ifdef PLAYERBOTS
#include "BotFactory.h"
#include "Playerbots.h"
#include "RandomPlayerbotMgr.h"
#endif

enum LevelBoostActions
{
    ACTION_BOOST_60  = 1,
    ACTION_BOOST_70  = 2,
    ACTION_BOOST_85  = 3,
    ACTION_NEVERMIND = 99,
};

static void BoostPlayer(Player* player, uint32 newLevel)
{
    uint32 oldLevel = player->GetLevel();
    if (oldLevel >= newLevel)
    {
        ChatHandler(player->GetSession()).PSendSysMessage("|cffFF0000You are already level %u or higher.|r", newLevel);
        return;
    }

    player->GiveLevel(newLevel);
    player->InitTalentForLevel();
    player->SetUInt32Value(PLAYER_FIELD_XP, 0);

    // Strip all existing gear to make way for level-appropriate items
    for (uint8 slot = EQUIPMENT_SLOT_START; slot < EQUIPMENT_SLOT_END; ++slot)
    {
        if (slot == EQUIPMENT_SLOT_TABARD || slot == EQUIPMENT_SLOT_BODY)
            continue;

        if (Item* item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, slot))
            player->DestroyItem(INVENTORY_SLOT_BAG_0, slot, true);
    }

#ifdef PLAYERBOTS
    {
        BotFactory factory(player, newLevel);
        factory.InitEquipment(false, true);
    }
#endif

    // Give some starter gold appropriate to the boost level
    uint32 gold = 0;
    if (newLevel == 60)
        gold = 100;   // 100 gold
    else if (newLevel == 70)
        gold = 500;   // 500 gold
    else if (newLevel == 85)
        gold = 2000;  // 2000 gold

    if (gold > 0)
        player->ModifyMoney(gold * GOLD);

    player->SaveToDB();

    ChatHandler(player->GetSession()).PSendSysMessage("|cff00ff00You have been boosted to level %u with gear and %u gold!|r", newLevel, gold);
}

struct npc_level_boost : public ScriptedAI
{
    npc_level_boost(Creature* creature) : ScriptedAI(creature) { }

    bool OnGossipHello(Player* player) override
    {
        ClearGossipMenuFor(player);

        if (player->GetLevel() < 60)
            AddGossipItemFor(player, GOSSIP_ICON_BATTLE, "|TInterface\\Icons\\Achievement_Level_60:30:30|t Boost to Level 60", GOSSIP_SENDER_MAIN, ACTION_BOOST_60,
                "This will boost you to level 60 with appropriate gear. Continue?", 0, false);

        if (player->GetLevel() < 70)
            AddGossipItemFor(player, GOSSIP_ICON_BATTLE, "|TInterface\\Icons\\Achievement_Level_70:30:30|t Boost to Level 70", GOSSIP_SENDER_MAIN, ACTION_BOOST_70,
                "This will boost you to level 70 with appropriate gear. Continue?", 0, false);

        if (player->GetLevel() < 85)
            AddGossipItemFor(player, GOSSIP_ICON_BATTLE, "|TInterface\\Icons\\Achievement_Level_85:30:30|t Boost to Level 85", GOSSIP_SENDER_MAIN, ACTION_BOOST_85,
                "This will boost you to level 85 with appropriate gear. Continue?", 0, false);

        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Nevermind", GOSSIP_SENDER_MAIN, ACTION_NEVERMIND);

        SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, me->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, uint32 /*menuId*/, uint32 gossipListId) override
    {
        uint32 action = player->PlayerTalkClass->GetGossipOptionAction(gossipListId);
        ClearGossipMenuFor(player);
        CloseGossipMenuFor(player);

        switch (action)
        {
            case ACTION_BOOST_60:
                BoostPlayer(player, 60);
                break;
            case ACTION_BOOST_70:
                BoostPlayer(player, 70);
                break;
            case ACTION_BOOST_85:
                BoostPlayer(player, 85);
                break;
            case ACTION_NEVERMIND:
                break;
        }

        return true;
    }
};

void AddSC_npc_level_boost()
{
    RegisterCreatureAI(npc_level_boost);
}
