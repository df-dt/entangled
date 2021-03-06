/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __COMMON_NETWORK_URI_PARSER_H__
#define __COMMON_NETWORK_URI_PARSER_H__

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

bool uri_parse(char const *const uri, char *const scheme, size_t scheme_len,
               char *const host, size_t host_len, uint16_t *const port);

#endif  // __COMMON_NETWORK_URI_PARSER_H__
