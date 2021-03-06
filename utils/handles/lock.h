/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __UTILS_HANDLES_LOCK_H__
#define __UTILS_HANDLES_LOCK_H__

/**
 * We define a type lock_handle_t mapping to a system available lock primitive
 * and its associated functions, some of them might have no effect if not needed
 * by the underlying API
 */

#include <unistd.h>

#ifdef _POSIX_THREADS

#include <pthread.h>

typedef pthread_mutex_t lock_handle_t;

static inline int lock_handle_init(lock_handle_t* const lock) {
  return pthread_mutex_init(lock, NULL);
}

static inline int lock_handle_lock(lock_handle_t* const lock) {
  return pthread_mutex_lock(lock);
}

static inline int lock_handle_unlock(lock_handle_t* const lock) {
  return pthread_mutex_unlock(lock);
}

static inline int lock_handle_destroy(lock_handle_t* const lock) {
  return pthread_mutex_destroy(lock);
}

#else

#error "No lock primitive found"

#endif  // _POSIX_THREADS

/**
 * Initializes a lock
 *
 * @param lock The lock
 *
 * @return exit status
 */
static inline int lock_handle_init(lock_handle_t* const lock);

/**
 * Acquires ownership of the given lock
 *
 * @param lock The lock
 *
 * @return exit status
 */
static inline int lock_handle_lock(lock_handle_t* const lock);

/**
 * Releases ownership of the given lock
 *
 * @param lock The lock
 *
 * @return exit status
 */
static inline int lock_handle_unlock(lock_handle_t* const lock);

/**
 * Destroys the lock
 *
 * @param lock The lock
 *
 * @return exit status
 */
static inline int lock_handle_destroy(lock_handle_t* const lock);

#endif  // __UTILS_HANDLES_LOCK_H__
