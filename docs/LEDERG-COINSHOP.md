# LEDERG COIN integration

This patch targets the LEDERG CR:MP gamemode, which uses a_mysql, sscanf2, zcmd, and the PLAYER_DATA / dbHandle globals.

## Install
1. Back up the database and current gamemode.
2. Run database/lederg_coin.sql once in phpMyAdmin against the game database.
3. Copy gamemodes/LEDERG_CoinShop.inc beside gamemodes/new.pwn.
4. Add #include "LEDERG_CoinShop.inc" at the end of gamemodes/new.pwn, after existing declarations and game logic.
5. Compile the gamemode to a temporary .amx, review warnings, then test balance, a purchase, a disconnect/reconnect, and an admin top-up before deploying.

## Commands
- /coins: query the persisted balance.
- /coinshop: show catalog and manual top-up instructions.
- /coinbuy 1|2|3: buy persisted skin 287, skin 286, or remove wanted level.
- /givecoins [player] [amount]: grant 1–1,000,000 coins; requires admin level 12+. Every grant/purchase writes an audit row.

Manual top-ups are handled through Telegram @LEDERG_ru. Ask players to send their in-game nickname and payment confirmation; never ask for their password. Use /givecoins once payment is verified.

The legacy accounts table is MyISAM, so the migration intentionally adds no foreign key. Purchases use conditional UPDATE ... WHERE ledgerg_coin >= price statements to prevent negative balances and double-spending. This migration is supplied as a script; it is not run against a live database by this repository build.
