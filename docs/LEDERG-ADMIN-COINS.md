# LEDERG CRMP — Admin, coins and shop design

## Current implementation status

- `database/lederg_crmp_clean.sql` is the clean-baseline import: it preserves the legacy game-world tables and their map/business/property seed data, removes legacy account rows and historic player transaction/election rows, and adds LEDERG economy/admin tables.
- `database/RESET-PLAYER-DATA.sql` is a separate destructive reset for a database that has already been migrated.
- The SQL permission catalogue currently contains 120 granular permission keys. This is a permission data model, **not 120 completed gameplay commands**. The Pawn command handlers and UI must be connected and tested before advertising the full panel as operational.
- The panel command requested is `/admledpanel` (typed in-game as `/admledpanel`); a dialog-based Pawn panel is the compatibility-first approach for this legacy client.

## Admin panel rules

1. The server must check login state and permission on every action, not just hide unavailable buttons in the menu.
2. A player can receive explicit allow/deny overrides in `lederg_admin_user_permissions`; role defaults are stored in `lederg_admin_role_permissions`.
3. The owner role is intended to receive every catalogue permission. Never allow an ordinary admin to assign themselves the owner role or modify owner permissions.
4. Each successful sensitive action must append an audit row with actor, target, action, reason/details, and server timestamp.
5. Destructive and economy actions require confirmation, bounded numeric inputs, and target validation. Prevent self-escalation and prevent staff from changing an equal/higher-ranked administrator unless they have a dedicated permission.
6. The legacy `accounts.admlvl` remains the compatibility source for the existing command set until a staged migration is tested. Do not silently remove its checks before the new permission checks are wired into each command.
7. Suggested categories: player moderation, player state, vehicles, properties/businesses, organizations, server operations, economy/shop, staff management, reports, audit.

## LEDERG Coins: manual Telegram payment workflow

- Public contact: [@LEDERG_ru](https://t.me/LEDERG_ru). The game/site should create a pending order and show its order code, coin amount, account nickname, and the contact link.
- The player messages the owner and supplies the order code. The owner verifies payment outside the game and checks that the order is still pending.
- The owner marks the order paid only after checking the payment. The server-side operation must atomically mark the order paid, credit the wallet, and insert a ledger row with a unique reference such as `order:<order_code>`.
- A repeated approval must not credit coins twice. Use a transaction/row lock or conditional update from pending to paid, and a unique ledger reference. If the transaction fails, roll back both the order status and wallet credit.
- Manual grants should record the administrator, reason, and a unique reference when one exists. Never ask players to send card numbers, passwords, login codes, or other credentials in Telegram.
- Do not hard-code exchange rates or advertise real-money purchase until the owner sets final prices, refund rules, age/region restrictions, and the relevant legal/tax terms.

## Suggested starter shop items

The SQL seed contains VIP durations and cosmetic items as examples. Confirm that each reward can actually be granted by this particular Pawn gamemode before enabling it. Unimplemented items should remain disabled in production.

## Safe rollout

1. Back up the target database and test import/restore on a disposable database.
2. Import `database/lederg_crmp_clean.sql` into a **new empty database**. It contains world seed rows but no old account rows. Do not import over the live database without a backup.
3. Deploy a matching compiled `new.amx` only after the Pawn source is compiled with the project's compatible compiler/includes.
4. Test new registration, login, account save/load, all faction menus/storage, admin permission denial, coin grant/charge, double-approval rejection, shop purchase and restart persistence.
5. Rotate all credentials that were ever committed and clean repository history separately. Removing a secret from the current branch does not erase it from older commits.
