// Copyright 2018 IOTA Foundation

#ifndef CCLIENT_SERVICE_H_
#define CCLIENT_SERVICE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>

#include "serialization/serializer.h"

typedef struct {
  const char* host;
  size_t port;
  const char* content_type;
  int version;
  unsigned int timeout;
  serializer_t serializer;
} iota_http_service_t;

// TODO - implement + figure a way to pass serializer/serializer_enum and other
// TODO - params, should they be loaded from conf?
void init_iota_http_service(iota_http_service_t*, const char* const host,
                            size_t port);

typedef struct {
  size_t is_error;
  size_t duration;
} iota_api_result_t;

typedef struct {
  char* data;
  size_t length;
} iota_response_t;

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_SERVICE_H_
