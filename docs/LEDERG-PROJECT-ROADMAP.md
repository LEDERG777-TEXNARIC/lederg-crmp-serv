# LEDERG PROJECT — CR:MP modernization roadmap

## Slice 1 — branding and repository hygiene
- [x] Rebrand server/game-mode titles and TextDraw logo.
- [x] Rename faction display labels while preserving IDs 5, 6, and 7.
- [x] Remove timed VK promotion without removing save operations.
- [x] Remove hard-coded DB macros; add local config templates and ignore rules.
- [ ] Rotate exposed credentials and clean sensitive Git history (deleting the latest SQL file does not purge history).
- [ ] Compile matching `gamemodes/new.amx` with compatible compiler/includes.

## Slice 2 — reproducible build
- Inventory includes, plugins, binaries, and compiler version.
- Capture baseline compiler output; pin compatible compiler and resolve warnings incrementally.
- Build maintained gamemodes/filterscripts; never deploy source/AMX mismatch.
- [x] Add repeatable GitHub Actions verification for branding and sanitized SQL structure.
- [ ] Add a reproducible Pawn/AMX build after the compatible compiler/includes are pinned.

## Slice 3 — database correctness
- [x] Publish fresh-install SQL without account rows, emails, password hashes, IPs, bans, payments, or activity logs.
- [x] Preserve static world/property seed records while resetting owners, ownership flags, stored balances, and warehouse inventory.
- [x] Add admin roles/individual permission storage/audit, LEDERG Coins wallet/ledger/products/orders schema.
- [ ] Wire roles, per-action permission enforcement, audit events, and coin transactions into Pawn handlers; compile and stage before enabling.
- Test DB connect/reconnect, registration/login, load/save, and outage handling on a disposable copy.
- Audit password storage and SQL error handling; plan migrations instead of rewriting records blindly.
- Test backup/restore for accounts, assets, houses, businesses, and faction storage.

## Slice 4 — gameplay UX
- Review GPS, org menus, leader assignment, warehouses, capture messages, map markers, welcome/donate/rules screens, and all TextDraws.
- Preserve IDs, enum values, DB columns, and persisted keys unless a migration is planned.
- Test spawn, registration, admin flows, and faction gameplay on staging.

## Slice 5 — operations and release
- Add DB/save error logs without passwords or player secrets.
- Document backups, health checks, restart behavior, and rollback.
- Keep provider ports/tunnels in host config, not hard-coded secrets.
- Keep database-foundation releases marked preview until the matching AMX compiles, staging smoke tests and restore test pass, admin permissions/coin flow are wired, and license review is complete.

## Acceptance gate
Do not deploy until the matching AMX compiles, branding checks pass, login/save flows work on staging, and exposed credentials are rotated and purged from public history.
