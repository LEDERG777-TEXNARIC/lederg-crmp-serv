#if defined _LEDERG_COINSHOP_INC
    #endinput
#endif
#define _LEDERG_COINSHOP_INC

#define LEDERG_COIN_MAX_TOPUP       (1000000)
#define LEDERG_COIN_MAX_BALANCE     (2000000000)

forward LEDERG_CoinBalanceLoaded(playerid, account_id);
forward LEDERG_CoinPurchaseResult(playerid, account_id, item_id, price);
forward LEDERG_CoinTopupResult(admin_playerid, target_playerid, admin_account_id, target_account_id, amount, admin_name[]);

stock bool:LEDERG_CoinLoggedIn(playerid)
{
    return IsPlayerConnected(playerid)
        && PLAYER_DATA[playerid][data_LOGGED]
        && PLAYER_DATA[playerid][data_ID] > 0;
}

stock LEDERG_CoinShowShop(playerid)
{
    SendClientMessage(playerid, 0xFF263FFF, "========== LEDERG COIN / ОФИЦИАЛЬНЫЙ МАГАЗИН ==========");
    SendClientMessage(playerid, 0xFFFFFFFF, "/coins — баланс | /coinbuy 1-3 — купить товар");
    SendClientMessage(playerid, 0xFFFFFFFF, "1. LEDERG TACTICAL SKIN (ID 287) — 120 LEDERG COIN");
    SendClientMessage(playerid, 0xFFFFFFFF, "2. LEDERG ELITE SKIN (ID 286) — 150 LEDERG COIN");
    SendClientMessage(playerid, 0xFFFFFFFF, "3. Снять розыск — 80 LEDERG COIN");
    SendClientMessage(playerid, 0xFFB5B5B5, "Пополнение: Telegram @LEDERG_ru. Укажи игровой ник и подтверждение оплаты.");
    SendClientMessage(playerid, 0xFFB5B5B5, "Не отправляй никому пароль от аккаунта.");
    return 1;
}

CMD:coins(playerid, params[])
{
    #pragma unused params
    if(!LEDERG_CoinLoggedIn(playerid))
        return SendClientMessage(playerid, 0xFF5555FF, "Войди в игровой аккаунт, чтобы посмотреть LEDERG COIN."), 1;

    new query[160];
    mysql_format(dbHandle, query, sizeof(query),
        "SELECT ledgerg_coin FROM accounts WHERE id = %d LIMIT 1",
        PLAYER_DATA[playerid][data_ID]);
    mysql_tquery(dbHandle, query, "LEDERG_CoinBalanceLoaded", "dd",
        playerid, PLAYER_DATA[playerid][data_ID]);
    return 1;
}

CMD:coinshop(playerid, params[])
{
    #pragma unused params
    if(!LEDERG_CoinLoggedIn(playerid))
        return SendClientMessage(playerid, 0xFF5555FF, "Войди в аккаунт, чтобы открыть магазин."), 1;
    LEDERG_CoinShowShop(playerid);
    return 1;
}

CMD:coinbuy(playerid, params[])
{
    if(!LEDERG_CoinLoggedIn(playerid))
        return SendClientMessage(playerid, 0xFF5555FF, "Сначала войди в игровой аккаунт."), 1;

    new item_id;
    if(sscanf(params, "d", item_id))
        return SendClientMessage(playerid, 0xFFFFFFFF, "Использование: /coinbuy 1-3 (список: /coinshop)"), 1;

    new price, skin_id = -1, wanted_reset = 0;
    switch(item_id)
    {
        case 1: { price = 120; skin_id = 287; }
        case 2: { price = 150; skin_id = 286; }
        case 3: { price = 80; wanted_reset = 1; }
        default: return SendClientMessage(playerid, 0xFF5555FF, "Такого товара нет. Открой /coinshop."), 1;
    }

    new account_id = PLAYER_DATA[playerid][data_ID];
    new query[320];

    // Atomic conditional UPDATE: coins cannot be spent twice by concurrent requests.
    if(skin_id >= 0)
    {
        mysql_format(dbHandle, query, sizeof(query),
            "UPDATE accounts SET ledgerg_coin = ledgerg_coin - %d, skin = %d WHERE id = %d AND ledgerg_coin >= %d LIMIT 1",
            price, skin_id, account_id, price);
    }
    else if(wanted_reset)
    {
        mysql_format(dbHandle, query, sizeof(query),
            "UPDATE accounts SET ledgerg_coin = ledgerg_coin - %d, wanted = 0 WHERE id = %d AND ledgerg_coin >= %d LIMIT 1",
            price, account_id, price);
    }

    mysql_tquery(dbHandle, query, "LEDERG_CoinPurchaseResult", "dddd",
        playerid, account_id, item_id, price);
    return 1;
}

CMD:givecoins(playerid, params[])
{
    if(!LEDERG_CoinLoggedIn(playerid))
        return SendClientMessage(playerid, 0xFF5555FF, "Сначала войди в аккаунт."), 1;
    if(PLAYER_DATA[playerid][data_ADMLVL] < 12)
        return SendClientMessage(playerid, 0xFF5555FF, "Нет прав для выдачи LEDERG COIN."), 1;

    new target_id, amount;
    if(sscanf(params, "ud", target_id, amount))
        return SendClientMessage(playerid, 0xFFFFFFFF, "Использование: /givecoins [игрок] [количество]"), 1;
    if(target_id == INVALID_PLAYER_ID || !LEDERG_CoinLoggedIn(target_id))
        return SendClientMessage(playerid, 0xFF5555FF, "Игрок должен быть в сети и войти в аккаунт."), 1;
    if(amount < 1 || amount > LEDERG_COIN_MAX_TOPUP)
        return SendClientMessage(playerid, 0xFF5555FF, "Количество должно быть от 1 до 1000000."), 1;

    new query[360];
    mysql_format(dbHandle, query, sizeof(query),
        "UPDATE accounts SET ledgerg_coin = ledgerg_coin + %d WHERE id = %d AND ledgerg_coin <= %d - %d LIMIT 1",
        amount, PLAYER_DATA[target_id][data_ID], LEDERG_COIN_MAX_BALANCE, amount);
    mysql_tquery(dbHandle, query, "LEDERG_CoinTopupResult", "ddddds[25]",
        playerid, target_id, PLAYER_DATA[playerid][data_ID],
        PLAYER_DATA[target_id][data_ID], amount, PLAYER_DATA[playerid][data_NAME]);
    return 1;
}

public LEDERG_CoinBalanceLoaded(playerid, account_id)
{
    new rows, fields;
    cache_get_data(rows, fields);
    if(rows < 1)
    {
        if(IsPlayerConnected(playerid) && PLAYER_DATA[playerid][data_ID] == account_id)
            SendClientMessage(playerid, 0xFF5555FF, "Аккаунт не найден в базе данных.");
        return 1;
    }

    new balance = cache_get_field_content_int(0, "ledgerg_coin", dbHandle);
    if(IsPlayerConnected(playerid)
        && PLAYER_DATA[playerid][data_LOGGED]
        && PLAYER_DATA[playerid][data_ID] == account_id)
    {
        new message[128];
        format(message, sizeof(message), "Твой баланс: {FF263F}%d LEDERG COIN{FFFFFF}. Магазин: /coinshop", balance);
        SendClientMessage(playerid, 0xFFFFFFFF, message);
    }
    return 1;
}

public LEDERG_CoinPurchaseResult(playerid, account_id, item_id, price)
{
    if(cache_affected_rows(dbHandle) < 1)
    {
        if(IsPlayerConnected(playerid)
            && PLAYER_DATA[playerid][data_LOGGED]
            && PLAYER_DATA[playerid][data_ID] == account_id)
        {
            SendClientMessage(playerid, 0xFF5555FF, "Недостаточно LEDERG COIN или покупка не выполнена.");
        }
        return 1;
    }

    new reason[32];
    switch(item_id)
    {
        case 1: reason = "SHOP_SKIN_287";
        case 2: reason = "SHOP_SKIN_286";
        case 3: reason = "SHOP_WANTED_RESET";
        default: reason = "SHOP_UNKNOWN";
    }

    new query[400];
    mysql_format(dbHandle, query, sizeof(query),
        "INSERT INTO lederg_coin_transactions (account_id,delta,balance_after,reason,admin_name) VALUES (%d,-%d,(SELECT ledgerg_coin FROM accounts WHERE id = %d LIMIT 1),'%e','')",
        account_id, price, account_id, reason);
    mysql_tquery(dbHandle, query);

    if(!IsPlayerConnected(playerid)
        || !PLAYER_DATA[playerid][data_LOGGED]
        || PLAYER_DATA[playerid][data_ID] != account_id)
        return 1;

    if(item_id == 1 || item_id == 2)
    {
        new skin_id = (item_id == 1) ? 287 : 286;
        PLAYER_DATA[playerid][data_SKIN] = skin_id;
        SetPlayerSkin(playerid, skin_id);
        SendClientMessage(playerid, 0xFF263FFF, "Покупка успешна! LEDERG-скин сохранён на аккаунте.");
    }
    else if(item_id == 3)
    {
        PLAYER_DATA[playerid][data_WANTED] = 0;
        SetPlayerWantedLevel(playerid, 0);
        SendClientMessage(playerid, 0xFF263FFF, "Покупка успешна! Розыск снят.");
    }

    new balance_query[160];
    mysql_format(dbHandle, balance_query, sizeof(balance_query),
        "SELECT ledgerg_coin FROM accounts WHERE id = %d LIMIT 1", account_id);
    mysql_tquery(dbHandle, balance_query, "LEDERG_CoinBalanceLoaded", "dd", playerid, account_id);
    return 1;
}

public LEDERG_CoinTopupResult(admin_playerid, target_playerid, admin_account_id, target_account_id, amount, admin_name[])
{
    if(cache_affected_rows(dbHandle) < 1)
    {
        if(IsPlayerConnected(admin_playerid)
            && PLAYER_DATA[admin_playerid][data_LOGGED]
            && PLAYER_DATA[admin_playerid][data_ID] == admin_account_id)
            SendClientMessage(admin_playerid, 0xFF5555FF, "Выдача не выполнена: проверь баланс игрока и миграцию LEDERG COIN.");
        return 1;
    }

    new query[420];
    mysql_format(dbHandle, query, sizeof(query),
        "INSERT INTO lederg_coin_transactions (account_id,delta,balance_after,reason,admin_name) VALUES (%d,%d,(SELECT ledgerg_coin FROM accounts WHERE id = %d LIMIT 1),'ADMIN_TOPUP','%e')",
        target_account_id, amount, target_account_id, admin_name);
    mysql_tquery(dbHandle, query);

    if(IsPlayerConnected(admin_playerid)
        && PLAYER_DATA[admin_playerid][data_LOGGED]
        && PLAYER_DATA[admin_playerid][data_ID] == admin_account_id)
    {
        new message[112];
        format(message, sizeof(message), "Выдано %d LEDERG COIN игроку ID %d.", amount, target_playerid);
        SendClientMessage(admin_playerid, 0xFF263FFF, message);
    }

    if(IsPlayerConnected(target_playerid)
        && PLAYER_DATA[target_playerid][data_LOGGED]
        && PLAYER_DATA[target_playerid][data_ID] == target_account_id)
    {
        new message[112];
        format(message, sizeof(message), "Администратор начислил тебе %d LEDERG COIN. /coins", amount);
        SendClientMessage(target_playerid, 0xFF263FFF, message);
        new balance_query[160];
        mysql_format(dbHandle, balance_query, sizeof(balance_query),
            "SELECT ledgerg_coin FROM accounts WHERE id = %d LIMIT 1", target_account_id);
        mysql_tquery(dbHandle, balance_query, "LEDERG_CoinBalanceLoaded", "dd", target_playerid, target_account_id);
    }
    return 1;
}
