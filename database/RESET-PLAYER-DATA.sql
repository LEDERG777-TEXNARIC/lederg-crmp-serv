-- LEDERG CRMP: destructive account reset for a fresh season.
-- BACK UP the database first and run only after confirming the target database.
-- Preserves world/economy locations (businesses, houses, apartments, warehouses)
-- and schema; removes player accounts and historical player logs.
START TRANSACTION;
DELETE FROM `accounts`;
DELETE FROM `log_transfermoney`;
DELETE FROM `log_vibory`;
DELETE FROM `log_player`;
DELETE FROM `log_paymoney`;
DELETE FROM `lederg_wallets`;
DELETE FROM `lederg_coin_ledger`;
DELETE FROM `lederg_coin_orders`;
DELETE FROM `lederg_shop_purchases`;
DELETE FROM `lederg_admin_user_permissions`;
DELETE FROM `lederg_admin_audit`;
COMMIT;
ALTER TABLE `accounts` AUTO_INCREMENT = 1;
