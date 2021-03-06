/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "common/network/components/broadcaster.h"
#include "ciri/node.h"
#include "utils/logger_helper.h"

#define BROADCASTER_COMPONENT_LOGGER_ID "broadcaster_component"

static void *broadcaster_routine(broadcaster_state_t *const state) {
  trit_array_p hash;
  concurrent_list_node_of_neighbor_t *iter;

  if (state == NULL) {
    return NULL;
  }
  while (state->running) {
    if (state->queue->vtable->pop(state->queue, &hash) ==
        CONCURRENT_QUEUE_SUCCESS) {
      iter = state->node->neighbors->front;
      while (iter) {
        neighbor_send(&iter->data, hash);
        iter = iter->next;
      }
#if !defined(NO_DYNAMIC_ALLOCATION)
      trit_array_free(hash);
#endif
    }
  }
  return NULL;
}

bool broadcaster_init(broadcaster_state_t *const state, node_t *const node) {
  if (state == NULL || node == NULL) {
    return false;
  }
  logger_helper_init(BROADCASTER_COMPONENT_LOGGER_ID, LOGGER_DEBUG, true);
  state->running = false;
  if (INIT_CONCURRENT_QUEUE_OF(trit_array_p, state->queue) !=
      CONCURRENT_QUEUE_SUCCESS) {
    log_critical(BROADCASTER_COMPONENT_LOGGER_ID,
                 "Initializing broadcaster queue failed\n");
    return false;
  }
  state->node = node;
  return true;
}

bool broadcaster_start(broadcaster_state_t *const state) {
  if (state == NULL) {
    return false;
  }
  log_info(BROADCASTER_COMPONENT_LOGGER_ID, "Spawning broadcaster thread\n");
  state->running = true;
  if (thread_handle_create(&state->thread,
                           (thread_routine_t)broadcaster_routine, state) != 0) {
    log_critical(BROADCASTER_COMPONENT_LOGGER_ID,
                 "Spawning broadcaster thread failed\n");
    return false;
  }
  return true;
}

bool broadcaster_on_next(broadcaster_state_t *const state,
                         trit_array_p const hash) {
  if (state == NULL) {
    return false;
  }
  if (state->queue->vtable->push(state->queue, hash) !=
      CONCURRENT_QUEUE_SUCCESS) {
    log_warning(BROADCASTER_COMPONENT_LOGGER_ID,
                "Pushing to broadcaster queue failed\n");
    return false;
  }
  return true;
}

bool broadcaster_stop(broadcaster_state_t *const state) {
  bool ret = true;

  if (state == NULL) {
    return false;
  }
  log_info(BROADCASTER_COMPONENT_LOGGER_ID,
           "Shutting down broadcaster thread\n");
  state->running = false;
  if (thread_handle_join(state->thread, NULL) != 0) {
    log_error(BROADCASTER_COMPONENT_LOGGER_ID,
              "Shutting down broadcaster thread failed\n");
    ret = false;
  }
  return ret;
}

bool broadcaster_destroy(broadcaster_state_t *const state) {
  bool ret = true;

  if (state == NULL) {
    return false;
  }
  if (state->running) {
    return false;
  }
  if (DESTROY_CONCURRENT_QUEUE_OF(trit_array_p, state->queue) !=
      CONCURRENT_QUEUE_SUCCESS) {
    log_error(BROADCASTER_COMPONENT_LOGGER_ID,
              "Destroying broadcaster queue failed\n");
    ret = false;
  }
  logger_helper_destroy(BROADCASTER_COMPONENT_LOGGER_ID);
  return ret;
}
