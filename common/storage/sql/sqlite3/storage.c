/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common/storage/sql/defs.h"
#include "common/storage/sql/statements.h"
#include "common/storage/storage.h"
#include "utils/logger_helper.h"

#include <logger.h>
#include <sqlite3.h>

#define SQLITE3_LOGGER_ID "stor_sqlite3"

int select_hashes_cb(void* hashes, int num_cols, char** col_values,
                     char** col_names);

retcode_t iota_stor_init() {
  logger_helper_init(SQLITE3_LOGGER_ID, LOGGER_INFO, true);
  return RC_OK;
}

retcode_t iota_stor_destroy() {
  logger_helper_destroy(SQLITE3_LOGGER_ID);
  return RC_OK;
}

int select_transactions_cb(void* txs, int num_cols, char** col_values,
                           char** col_names) {
  char* end;
  size_t c = 0;
  iota_transactions_pack* pack;

  if (num_cols != NUM_COLS) {
    log_error(SQLITE3_LOGGER_ID, "Failed to retrieve columns");
    return RC_SQLITE3_FAILED_SELECT_DB;
  }

  pack = (iota_transactions_pack*)txs;
  if (pack->num_loaded < pack->max_txs) {
    iota_transaction_t curr_transaction = pack->txs[pack->num_loaded++];

    memcpy(curr_transaction->signature_or_message, col_values[c++], 2187);
    memcpy(curr_transaction->address, col_values[c++], 81);
    curr_transaction->value = strtol(col_values[c++], &end, 10);
    memcpy(curr_transaction->obsolete_tag, col_values[c++], 27);
    curr_transaction->timestamp = strtol(col_values[c++], &end, 10);
    curr_transaction->current_index = strtol(col_values[c++], &end, 10);
    curr_transaction->last_index = strtol(col_values[c++], &end, 10);
    memcpy(curr_transaction->bundle, col_values[c++], 81);
    memcpy(curr_transaction->trunk, col_values[c++], 81);
    memcpy(curr_transaction->branch, col_values[c++], 81);
    memcpy(curr_transaction->tag, col_values[c++], 27);
    curr_transaction->attachment_timestamp = strtol(col_values[c++], &end, 10);
    curr_transaction->attachment_timestamp_upper =
        strtol(col_values[c++], &end, 10);
    curr_transaction->attachment_timestamp_lower =
        strtol(col_values[c++], &end, 10);
    memcpy(curr_transaction->nonce, col_values[c++], 27);
    memcpy(curr_transaction->hash, col_values[c++], 81);
  }

  return 0;
}

int check_transactions_exist_cb(void* exist_arg, int num_cols,
                                char** col_values, char** col_names) {
  bool* exist = (bool*)exist_arg;
  *exist = true;
  return 0;
}

retcode_t iota_stor_store(const connection_t* const conn,
                          const iota_transaction_t data_in) {
  char statement[MAX_STORE_STATEMENT_SIZE];
  iota_transactions_insert_statement(data_in, statement,
                                     MAX_STORE_STATEMENT_SIZE);

  char* err_msg = 0;
  int rc = sqlite3_exec((sqlite3*)conn->db, statement, 0, 0, &err_msg);

  if (rc != SQLITE_OK) {
    log_error(SQLITE3_LOGGER_ID, "Failed in statement", statement);
    sqlite3_free(err_msg);
    return RC_SQLITE3_FAILED_INSERT_DB;
  }

  return RC_OK;
}
retcode_t iota_stor_load(const connection_t* const conn, const char* col_name,
                         const trit_array_p key, iota_transactions_pack* pack) {
  char statement[MAX_SELECT_STATEMENT_SIZE];
  iota_transactions_select_statement(col_name, key, statement,
                                     MAX_SELECT_STATEMENT_SIZE);

  char* err_msg = 0;
  int rc = sqlite3_exec((sqlite3*)conn->db, statement, select_transactions_cb,
                        (void*)pack, &err_msg);

  if (rc != SQLITE_OK) {
    log_error(SQLITE3_LOGGER_ID, "Failed in statement", statement);
    sqlite3_free(err_msg);
    return RC_SQLITE3_FAILED_INSERT_DB;
  }
  return RC_OK;
}
retcode_t iota_stor_exist(const connection_t* const conn,
                          const char* index_name, const trit_array_p key,
                          bool* exist) {
  char statement[MAX_EXIST_STATEMENT_SIZE];
  iota_transactions_exist_statement(index_name, key, statement,
                                    MAX_EXIST_STATEMENT_SIZE);

  char* err_msg = 0;
  *exist = false;

  int rc = sqlite3_exec((sqlite3*)conn->db, statement,
                        check_transactions_exist_cb, (void*)exist, &err_msg);

  if (rc != SQLITE_OK) {
    log_error(SQLITE3_LOGGER_ID, "Failed in statement", statement);
    sqlite3_free(err_msg);
    return RC_SQLITE3_FAILED_INSERT_DB;
  }

  return RC_OK;
}

extern retcode_t iota_stor_load_hashes(const connection_t* const conn,
                                       const char* col_name,
                                       const trit_array_p key,
                                       iota_hashes_pack* pack) {
  char statement[MAX_SELECT_STATEMENT_SIZE];
  iota_transactions_select_hashes_statement(col_name, key, statement,
                                            MAX_SELECT_STATEMENT_SIZE);

  char* err_msg = 0;

  int rc = sqlite3_exec((sqlite3*)conn->db, statement, select_hashes_cb,
                        (void*)pack, &err_msg);

  if (rc != SQLITE_OK) {
    log_error(SQLITE3_LOGGER_ID, "Failed in statement", statement);
    sqlite3_free(err_msg);
    return RC_SQLITE3_FAILED_INSERT_DB;
  }
  return RC_OK;
}

int select_hashes_cb(void* hashes, int num_cols, char** col_values,
                     char** col_names) {
  iota_hashes_pack* pack;
  int len;
  pack = (iota_hashes_pack*)hashes;
  if (pack->num_loaded < pack->max_hashes) {
    len = strlen(col_values[pack->num_loaded]);
    pack->hashes[pack->num_loaded]->num_bytes = len;
    memcpy(pack->hashes[pack->num_loaded]->trits, col_values[pack->num_loaded],
           len);
    pack->num_loaded++;
  }

  return 0;
}

retcode_t iota_stor_update(const connection_t* const conn,
                           const char* index_name, const trit_array_p key,
                           const iota_transaction_t data_in) {
  // TODO - implement
  return RC_SQLITE3_FAILED_NOT_IMPLEMENTED;
}
