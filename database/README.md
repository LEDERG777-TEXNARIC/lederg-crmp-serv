# Database setup

## Fresh installation
1. Create a new, empty database.
2. Import database/lederg_fresh_install.sql.
3. Copy server.cfg.example and scriptfiles/mysql_settings.example.ini to local runtime filenames; keep credentials on the server only.
4. Compile gamemodes/new.pwn with a compatible legacy Pawn compiler and deploy the matching AMX.
5. Test registration/login, persistence, organization menus/GPS/storage, autosave, and reconnect behavior on a disposable staging database.

The fresh-install file contains no account rows, password hashes, emails, IPs, bans, payment records, or old activity logs. Static world/property seed rows are preserved, but owners, ownership flags, stored balances, and warehouse inventories are reset.

## Existing installation
Do not import the fresh-install file into a live database. Back up and test this additive migration first:

    mysql -h HOST -u USER -p DATABASE < database/migrations/001_admin_coins.sql

The migration adds storage for per-account admin roles and permission grants, admin audit events, LEDERG Coins wallets/ledger, shop products, and manual-payment orders. The Pawn runtime is not yet wired to these tables in this preview; /admledpanel, permission enforcement, coin granting, purchase fulfilment, and refunds are not claimed as live.

## LEDERG Coins flow
The schema supports a manual payment workflow using Telegram @LEDERG_ru. Wallet changes and ledger entries must be made atomically in one server-side transaction. Never edit lederdag_coin_wallets.balance alone; that breaks the audit trail. Products are inactive by default.

## Secrets and recovery
The replaced legacy dump contained player/account and transaction data. Removing it from the current tree does not erase Git history. Rotate any DB/RCON credentials that were ever committed and arrange history cleanup before treating the public repository as fully sanitized.
