# Changelog

## v0.1.0-preview — 2026-10-22

### Added
- Clean-install SQL schema plus LEDERG admin roles, per-account permission grants, audit events, LEDERG Coins wallets/ledger, products, and manual-payment orders.
- Database setup guide and additive migration for existing installations.
- GitHub Actions checks for branding, repository hygiene, and SQL schema structure.

### Changed
- Preserved static house/business/apartment/location seed rows while clearing previous owners, ownership flags, stored property balances, and warehouse stock.
- Removed account rows, password hashes, email/IP data, bans, payment records, and player activity logs from the current install schema.
- Removed the tracked legacy SQL dump from the current tree.

### Important limitations
- This is a source/database-foundation preview, not a production-ready server package.
- The current Pawn runtime is not yet wired to new role/permission and Coins tables; the admledpanel command, per-action permission enforcement, transactional coin granting, and store fulfilment are not claimed as implemented.
- A matching new.amx has not been compiled or validated. Run only after compile, staging login/save tests, a database restore test, credential rotation, and license review.
- Removing the SQL dump from the current tree does not erase it from Git history; rotate exposed credentials and arrange history cleanup.
