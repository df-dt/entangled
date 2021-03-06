/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __COMMON_STORAGE_SQL_STATEMENTS_H__
#define __COMMON_STORAGE_SQL_STATEMENTS_H__

#include <stdbool.h>
#include <stdint.h>
#include "common/errors.h"
#include "common/model/transaction.h"
#include "common/storage/connection.h"
#include "common/trinary/trit_array.h"

#ifdef __cplusplus
extern "C" {
#endif

/*index_name = column name by to compare to key*/
retcode_t iota_transactions_insert_statement(const iota_transaction_t tx,
                                             char statement[],
                                             size_t statement_cap);

retcode_t iota_transactions_select_statement(const char* index_col,
                                             const trit_array_p key,
                                             char statement[],
                                             size_t statement_cap);

retcode_t iota_transactions_exist_statement(const char* index_col,
                                            const trit_array_p key,
                                            char statement[],
                                            size_t statement_cap);

retcode_t iota_transactions_update_statement(const char* index_col,
                                             const trit_array_p key,
                                             const iota_transaction_t tx,
                                             char statement[],
                                             size_t statement_cap);

retcode_t iota_transactions_select_hashes_statement(const char* index_col,
                                                    const trit_array_p key,
                                                    char statement[],
                                                    size_t statement_cap);

#ifdef __cplusplus
}
#endif

#endif  // __COMMON_STORAGE_SQL_STATEMENTS_H__
