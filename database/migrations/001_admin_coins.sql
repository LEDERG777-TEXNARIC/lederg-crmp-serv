-- LEDERG PROJECT — additive migration 001
-- Back up and test on staging first. This migration does not clear existing data.
CREATE TABLE IF NOT EXISTS `lederg_admin_roles` (
  `id` BIGINT UNSIGNED NOT NULL AUTO_INCREMENT, `account_name` VARCHAR(24) NOT NULL,
  `role_code` ENUM('owner','admin') NOT NULL DEFAULT 'admin', `is_active` TINYINT(1) NOT NULL DEFAULT 1,
  `granted_by` VARCHAR(24) NOT NULL DEFAULT '', `created_at` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `updated_at` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  PRIMARY KEY (`id`), UNIQUE KEY `uq_lederg_admin_account` (`account_name`),
  KEY `idx_lederg_admin_active_role` (`is_active`, `role_code`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
CREATE TABLE IF NOT EXISTS `lederg_admin_permissions` (
  `account_name` VARCHAR(24) NOT NULL, `permission_code` VARCHAR(64) NOT NULL,
  `is_granted` TINYINT(1) NOT NULL DEFAULT 1, `granted_by` VARCHAR(24) NOT NULL DEFAULT '',
  `created_at` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`account_name`, `permission_code`),
  KEY `idx_lederg_permission_code` (`permission_code`, `is_granted`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
CREATE TABLE IF NOT EXISTS `lederg_admin_audit` (
  `id` BIGINT UNSIGNED NOT NULL AUTO_INCREMENT, `actor_name` VARCHAR(24) NOT NULL,
  `action_code` VARCHAR(64) NOT NULL, `target_name` VARCHAR(24) NOT NULL DEFAULT '',
  `details` TEXT NOT NULL, `result_code` ENUM('success','denied','failed') NOT NULL DEFAULT 'success',
  `created_at` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP, PRIMARY KEY (`id`),
  KEY `idx_lederg_audit_actor_time` (`actor_name`, `created_at`),
  KEY `idx_lederg_audit_action_time` (`action_code`, `created_at`),
  KEY `idx_lederg_audit_target_time` (`target_name`, `created_at`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
CREATE TABLE IF NOT EXISTS `lederg_coin_wallets` (
  `account_name` VARCHAR(24) NOT NULL, `balance` BIGINT UNSIGNED NOT NULL DEFAULT 0,
  `updated_at` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  PRIMARY KEY (`account_name`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
CREATE TABLE IF NOT EXISTS `lederg_coin_ledger` (
  `id` BIGINT UNSIGNED NOT NULL AUTO_INCREMENT, `account_name` VARCHAR(24) NOT NULL,
  `delta` BIGINT NOT NULL, `balance_after` BIGINT UNSIGNED NOT NULL,
  `entry_type` ENUM('grant','purchase','refund','adjustment') NOT NULL,
  `source_ref` VARCHAR(80) NOT NULL DEFAULT '', `idempotency_key` VARCHAR(80) NULL,
  `actor_name` VARCHAR(24) NOT NULL DEFAULT '', `note` VARCHAR(255) NOT NULL DEFAULT '',
  `created_at` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP, PRIMARY KEY (`id`),
  UNIQUE KEY `uq_lederg_coin_idempotency` (`idempotency_key`),
  KEY `idx_lederg_coin_account_time` (`account_name`, `created_at`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
CREATE TABLE IF NOT EXISTS `lederg_coin_products` (
  `id` BIGINT UNSIGNED NOT NULL AUTO_INCREMENT, `product_code` VARCHAR(64) NOT NULL,
  `title` VARCHAR(100) NOT NULL, `description` VARCHAR(500) NOT NULL DEFAULT '',
  `price_coins` BIGINT UNSIGNED NOT NULL, `is_active` TINYINT(1) NOT NULL DEFAULT 0,
  `sort_order` INT NOT NULL DEFAULT 0, `created_at` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`id`), UNIQUE KEY `uq_lederg_coin_product_code` (`product_code`),
  KEY `idx_lederg_coin_product_active_order` (`is_active`, `sort_order`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
CREATE TABLE IF NOT EXISTS `lederg_coin_orders` (
  `id` BIGINT UNSIGNED NOT NULL AUTO_INCREMENT, `order_code` VARCHAR(40) NOT NULL,
  `buyer_name` VARCHAR(24) NOT NULL, `product_code` VARCHAR(64) NOT NULL,
  `quantity` INT UNSIGNED NOT NULL DEFAULT 1, `price_coins` BIGINT UNSIGNED NOT NULL,
  `status` ENUM('awaiting_payment','payment_verified','fulfilled','cancelled','refunded') NOT NULL DEFAULT 'awaiting_payment',
  `payment_channel` VARCHAR(80) NOT NULL DEFAULT 'Telegram @LEDERG_ru',
  `payment_reference` VARCHAR(120) NOT NULL DEFAULT '', `verified_by` VARCHAR(24) NOT NULL DEFAULT '',
  `created_at` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `updated_at` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  PRIMARY KEY (`id`), UNIQUE KEY `uq_lederg_coin_order_code` (`order_code`),
  KEY `idx_lederg_coin_order_buyer_status` (`buyer_name`, `status`),
  KEY `idx_lederg_coin_order_status_time` (`status`, `created_at`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
