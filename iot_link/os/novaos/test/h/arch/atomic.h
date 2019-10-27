#ifndef __HOST_ATOMIC_H__
#define __HOST_ATOMIC_H__

#include <stdbool.h>

#include <compiler.h>

static __always_inline int atomic_int_get (atomic_int * a)
    {
    return a->val;
    }

static __always_inline unsigned int atomic_uint_get (atomic_uint * a)
    {
    return a->val;
    }

static __always_inline void atomic_int_set (atomic_int * a, int v)
    {
    a->val = v;
    }

static __always_inline void atomic_uint_set (atomic_uint * a, unsigned int v)
    {
    a->val = v;
    }

static __always_inline atomic_int atomic_int_add (atomic_int * a, int v)
    {
    a->val += v;
    }

static __always_inline atomic_uint atomic_uint_add (atomic_uint * a, unsigned int v)
    {
    a->val += v;
    }

static __always_inline atomic_int atomic_int_sub (atomic_int * a, int v)
    {
    a->val -= v;
    }

static __always_inline atomic_uint atomic_uint_sub (atomic_uint * a, unsigned int v)
    {
    a->val -= v;
    }

static __always_inline atomic_int atomic_int_and (atomic_int * a, int v)
    {
    a->val &= v;
    }

static __always_inline atomic_uint atomic_uint_and (atomic_uint * a, unsigned int v)
    {
    a->val &= v;
    }

static __always_inline atomic_int atomic_int_orr (atomic_int * a, int v)
    {
    a->val |= v;
    }

static __always_inline atomic_uint atomic_uint_orr (atomic_uint * a, unsigned int v)
    {
    a->val |= v;
    }

static __always_inline atomic_int atomic_int_xor (atomic_int * a, int v)
    {
    a->val ^= v;
    }

static __always_inline atomic_uint atomic_uint_xor (atomic_uint * a, unsigned int v)
    {
    a->val ^= v;
    }

static __always_inline atomic_int atomic_int_inc (atomic_int * a)
    {
    return atomic_int_add (a, 1);
    }

static __always_inline atomic_uint atomic_uint_inc (atomic_uint * a)
    {
    return atomic_uint_add (a, 1);
    }

static __always_inline atomic_int atomic_int_dec (atomic_int * a)
    {
    return atomic_int_add (a, -1);
    }

static __always_inline atomic_uint atomic_uint_dec (atomic_uint * a)
    {
    return atomic_uint_add (a, (unsigned int) -1);
    }

static __always_inline bool atomic_int_set_eq (atomic_int * a, int o, int v)
    {
    if (a->val != o)
        {
        return false;
        }

    a->val = v;

    return true;
    }

static __always_inline bool atomic_uint_set_eq (atomic_uint * a, int o, int v)
    {
    return atomic_int_set_eq ((atomic_int *) a, (int) o, (int) v);
    }

static __always_inline bool atomic_int_add_ne (atomic_int * a, int o, int v)
    {
    if (a->val == o)
        {
        return false;
        }

    a->val += v;

    return true;
    }

static __always_inline bool atomic_uint_add_ne (atomic_uint * a,
                                                  unsigned int o, unsigned int v)
    {
    return atomic_int_add_ne ((atomic_int *) a, (int) o, (int) v);
    }

static __always_inline bool atomic_int_dec_nz (atomic_int * a)
    {
    return atomic_int_add_ne (a, 0, -1);
    }

static __always_inline bool atomic_uint_dec_nz (atomic_uint * a)
    {
    return atomic_int_add_ne ((atomic_int *) a, 0, (int) -1);
    }

#endif /* __HOST_ATOMIC_H__ */

