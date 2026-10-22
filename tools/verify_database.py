#!/usr/bin/env python3
"""Safety checks for the clean LEDERG SQL package; no database connection required."""
from pathlib import Path
import re

ROOT = Path(__file__).resolve().parents[1]
SQL_PATH = ROOT / "database" / "lederg_fresh_install.sql"
REQUIRED_TABLES = {
    "accounts", "auto_business", "banlist", "business", "freekassa_payments",
    "houses", "job_business", "kvartiry", "log_paymoney", "log_player",
    "log_transfermoney", "log_vibory", "polit_team", "warehouse",
    "lederg_admin_roles", "lederg_admin_permissions", "lederg_admin_audit",
    "lederg_coin_wallets", "lederg_coin_ledger", "lederg_coin_products", "lederg_coin_orders",
}
STATIC_SEED_TABLES = {"auto_business", "business", "houses", "job_business", "kvartiry", "warehouse"}
FORBIDDEN_SEED_TABLES = {"accounts", "banlist", "freekassa_payments", "log_paymoney", "log_player", "log_transfermoney", "log_vibory", "polit_team"}

def main() -> int:
    if not SQL_PATH.is_file():
        print("FAIL: fresh schema missing")
        return 1
    sql = SQL_PATH.read_text(encoding="utf-8")
    create_names = set(m.group(1).lower() for m in re.finditer(
        r"CREATE\s+TABLE(?:\s+IF\s+NOT\s+EXISTS)?\s+[^A-Za-z_]*([A-Za-z_]\w*)", sql, re.I))
    insert_names = set(m.group(1).lower() for m in re.finditer(
        r"INSERT\s+INTO\s+[^A-Za-z_]*([A-Za-z_]\w*)", sql, re.I))
    forbidden_inserts = sorted(insert_names & FORBIDDEN_SEED_TABLES)
    checks = {
        "all legacy and new tables defined": REQUIRED_TABLES.issubset(create_names),
        "no account, ban, payment or activity-history inserts": not forbidden_inserts,
        "static game-world seed data preserved": STATIC_SEED_TABLES.issubset(insert_names),
        "legacy dump removed from current tree": not (ROOT / "gs147466.sql").exists(),
        "schema package is non-empty": SQL_PATH.stat().st_size > 10000,
        "individual admin permissions table present": "permission_code" in sql,
        "coins ledger with idempotency field present": "idempotency_key" in sql,
        "manual Telegram payment channel present": "@LEDERG_ru" in sql,
    }
    for name, ok in checks.items():
        print(("PASS" if ok else "FAIL") + ": " + name)
    if forbidden_inserts:
        print("Unexpected inserts:", ", ".join(forbidden_inserts))
    if not REQUIRED_TABLES.issubset(create_names):
        print("Missing tables:", ", ".join(sorted(REQUIRED_TABLES - create_names)))
    return 0 if all(checks.values()) else 1

if __name__ == "__main__":
    raise SystemExit(main())
