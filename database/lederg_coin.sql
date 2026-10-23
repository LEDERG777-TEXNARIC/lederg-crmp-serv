-- LEDERG COIN migration for the existing accounts table.
-- Run once in phpMyAdmin against the same database used by the game server.
-- The existing accounts table uses MyISAM; keep the auditable ledger as InnoDB.
ALTER TABLE accounts
    ADD COLUMN IF NOT EXISTS ledgerg_coin INT UNSIGNED NOT NULL DEFAULT 0;

CREATE TABLE IF NOT EXISTS lederg_coin_transactions (
    id BIGINT UNSIGNED NOT NULL AUTO_INCREMENT,
    account_id INT NOT NULL,
    delta INT NOT NULL,
    balance_after INT UNSIGNED NOT NULL DEFAULT 0,
    reason VARCHAR(64) NOT NULL,
    admin_name VARCHAR(24) NOT NULL DEFAULT '',
    created_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    PRIMARY KEY (id),
    KEY idx_lc_account_created (account_id, created_at)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci;
