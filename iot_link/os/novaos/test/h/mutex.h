
#ifndef __MUTEX_H__
#define __MUTEX_H__

typedef int mutex_t, * mutex_id;

static inline int mutex_init (mutex_id m)
    {
    *m = 0;

    return 0;
    }

static inline int mutex_destroy (mutex_id m)
    {
    return 0;
    }

static inline int mutex_lock (mutex_id m)
    {
    while (*m != 0);

    *m = 1;

    return 0;
    }

static inline int mutex_unlock (mutex_id m)
    {
    while (*m != 1);

    *m = 0;

    return 0;
    }

#endif /* __MUTEX_H__ */