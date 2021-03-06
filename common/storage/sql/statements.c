/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */
#include <inttypes.h>
#include <stdio.h>
#include <string.h>

#include "common/storage/sql/defs.h"
#include "common/storage/sql/statements.h"
#include "utils/logger_helper.h"

#define SQL_STATEMENTS_ID "sql_statements"

retcode_t iota_transactions_insert_statement(const iota_transaction_t tx,
                                             char statement[],
                                             size_t statement_cap) {
  // FIXME(tsvi) - when iota_transaction PR for supporting trit_array is merged,
  // sizes should be different
  char sig_or_msg[2187 + 1];
  char address[81 + 1];
  char obsolete_tag[27 + 1];
  char bundle[81 + 1];
  char trunk[81 + 1];
  char branch[81 + 1];
  char tag[27 + 1];
  char nonce[27 + 1];
  char hash[81 + 1];
  int res;

  memcpy(sig_or_msg, tx->signature_or_message, 2187);
  sig_or_msg[2187] = '\0';
  memcpy(address, tx->address, 81);
  address[81] = '\0';

  memcpy(obsolete_tag, tx->obsolete_tag, 27);
  obsolete_tag[27] = '\0';

  memcpy(bundle, tx->bundle, 81);
  bundle[81] = '\0';
  memcpy(trunk, tx->trunk, 81);
  trunk[81] = '\0';
  memcpy(branch, tx->branch, 81);
  branch[81] = '\0';
  memcpy(tag, tx->tag, 27);
  tag[27] = '\0';

  memcpy(nonce, tx->nonce, 27);
  nonce[27] = '\0';

  memcpy(hash, tx->hash, 81);
  hash[81] = '\0';

  res = snprintf(
      statement, statement_cap,
      "INSERT INTO %s (%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s) "
      "VALUES ('%s','%s',%" PRIu64 ",'%s',%" PRIu64 ",%" PRIu64 ",%" PRIu64
      ",'%s','%s','%s','%s',%" PRIu64 ",%" PRIu64 ",%" PRIu64 ",'%s','%s')",
      TRANSACTION_TABLE_NAME, COL_SIG_OR_MSG, COL_ADDRESS, COL_VALUE,
      COL_OBSOLETE_TAG, COL_TIMESTAMP, COL_CURRENT_INDEX, COL_LAST_INDEX,
      COL_BUNDLE, COL_TRUNK, COL_BRANCH, COL_TAG, COL_ATTACHMENT_TIMESTAMP,
      COL_ATTACHMENT_TIMESTAMP_UPPER, COL_ATTACHMENT_TIMESTAMP_LOWER, COL_NONCE,
      COL_HASH, sig_or_msg, address, tx->value, obsolete_tag, tx->timestamp,
      tx->current_index, tx->last_index, bundle, trunk, branch, tag,
      tx->attachment_timestamp, tx->attachment_timestamp_upper,
      tx->attachment_timestamp_lower, nonce, hash);

  if (res < 0 || res == statement_cap) {
    log_error(SQL_STATEMENTS_ID, "Failed in creating statement, statement: %s",
              statement);
    return RC_SQL_FAILED_WRITE_STATEMENT;
  }
  return RC_OK;
}

retcode_t iota_transactions_select_statement(const char *index_col,
                                             const trit_array_p key,
                                             char statement[],
                                             size_t statement_cap) {
  int res;

  if (index_col == NULL || strcmp(index_col, "") == 0) {
    res = snprintf(statement, statement_cap, "SELECT * FROM %s",
                   TRANSACTION_TABLE_NAME);
  } else {
    char key_str[key->num_bytes];
    memcpy(key_str, key->trits, key->num_bytes);
    res = snprintf(statement, statement_cap, "SELECT * FROM %s WHERE %s = '%s'",
                   TRANSACTION_TABLE_NAME, index_col, key_str);
  }

  if (res < 0 || res == statement_cap) {
    log_error(SQL_STATEMENTS_ID, "Failed in creating statement, statement: %s",
              statement);
    return RC_SQL_FAILED_WRITE_STATEMENT;
  }
  return RC_OK;
}

retcode_t iota_transactions_exist_statement(const char *index_col,
                                            const trit_array_p key,
                                            char statement[],
                                            size_t statement_cap) {
  int res;

  if (index_col == NULL || strcmp(index_col, "") == 0) {
    res = snprintf(statement, statement_cap,
                   "SELECT '1' WHERE EXISTS(SELECT 1 FROM %s)",
                   TRANSACTION_TABLE_NAME);
  } else {
    char key_str[key->num_bytes];
    memcpy(key_str, key->trits, key->num_bytes);
    res = snprintf(statement, statement_cap,
                   "SELECT '1' WHERE EXISTS(SELECT 1 FROM %s WHERE %s = '%s')",
                   TRANSACTION_TABLE_NAME, index_col, key_str);
  }

  if (res < 0 || res == statement_cap) {
    log_error(SQL_STATEMENTS_ID, "Failed in creating statement, statement: %s",
              statement);
    return RC_SQL_FAILED_WRITE_STATEMENT;
  }
  return RC_OK;
}

retcode_t iota_transactions_update_statement(const char *index_col,
                                             const trit_array_p key,
                                             const iota_transaction_t tx,
                                             char statement[],
                                             size_t statement_cap) {
  return RC_OK;
}

retcode_t iota_transactions_select_hashes_statement(const char *index_col,
                                                    const trit_array_p key,
                                                    char statement[],
                                                    size_t statement_cap) {
  int res;

  if (index_col == NULL || strcmp(index_col, "") == 0) {
    res = snprintf(statement, statement_cap, "SELECT %s FROM %s", COL_HASH,
                   TRANSACTION_TABLE_NAME);
  } else {
    char key_str[key->num_bytes + 1];
    memcpy(key_str, key->trits, key->num_bytes);
    key_str[key->num_bytes] = 0;
    res = snprintf(statement, statement_cap, "SELECT %s FROM %s WHERE %s='%s'",
                   COL_HASH, TRANSACTION_TABLE_NAME, index_col, key_str);
  }

  if (res < 0 || res == statement_cap) {
    log_error(SQL_STATEMENTS_ID, "Failed in creating statement, statement: %s",
              statement);
    return RC_SQL_FAILED_WRITE_STATEMENT;
  }

  return RC_OK;
}
