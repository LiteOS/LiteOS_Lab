/*
 * Copyright (c) [2019] Huawei Technologies Co.,Ltd.All rights reserved.
 *
 * LiteOS NOVA is licensed under the Mulan PSL v1.
 * You can use this software according to the terms and conditions of the Mulan PSL v1.
 * You may obtain a copy of Mulan PSL v1 at:
 *
 * 	http://license.coscl.org.cn/MulanPSL
 *
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY OR
 * FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v1 for more details.
 */

#include <limits.h>
#include <string.h>

#include <common.h>
#include <hal_i2c.h>
#include <irq.h>
#include <mutex.h>
#include <errno.h>
#include <init.h>
#include <warn.h>
#include <bug.h>

#ifdef CONFIG_DEVFS
#include <devfs.h>
#endif

/* locals */

static dlist_t __i2c_buses = DLIST_INIT (__i2c_buses);
static mutex_t __bus_lock;
static dlist_t __i2c_devs  = DLIST_INIT (__i2c_devs);
static mutex_t __dev_lock;

/**
 * hal_i2c_open - open a i2c bus by name
 * @name: the i2c bus name, like "i2c1", "i2c2" etc
 *
 * return: a pointer to the i2c control struct or NULL if device not found
 */

hal_i2c_t * hal_i2c_open (const char * name)
    {
    dlist_t   * itr;
    hal_i2c_t * i2c;

    WARN_ON (name == NULL,
             errno = ERRNO_HAL_I2C_NO_MATCH; return NULL,
             "Invalid name!");

    mutex_lock (&__bus_lock);

    dlist_foreach (itr, &__i2c_buses)
        {
        i2c = container_of (itr, hal_i2c_t, node);

        if (strcmp (name, i2c->name) == 0)
            {
            mutex_unlock (&__bus_lock);
            return i2c;
            }
        }

    mutex_unlock (&__bus_lock);

    errno = ERRNO_HAL_I2C_NO_MATCH;

    WARN ("Fail to open device \"%s\"!", name);

    return NULL;
    }

/**
 * hal_i2c_close - close a i2c bus by name
 * @i2c: the i2c bus
 *
 * return: 0, always for now
 */

int hal_i2c_close (hal_i2c_t * i2c)
    {
    return 0;
    }

/**
 * hal_i2c_register - register a i2c hal bus
 * @i2c:  the i2c bus to register
 * @name: the i2c bus name, like "i2c1", "i2c2" etc
 *
 * return: 0 on success, negtive value on error
 */

int hal_i2c_register (hal_i2c_t * i2c, const char * name,
                      const struct hal_i2c_methods * methods)
    {
    BUG_ON (i2c           == NULL, "Invalid i2c bus!");
    BUG_ON (name          == NULL, "Invalid i2c bus name!");
    BUG_ON (methods       == NULL, "Invalid i2c bus methods!");
    BUG_ON (methods->xfer == NULL, "Invalid i2c bus methods!");
    BUG_ON (methods->recv == NULL, "Invalid i2c bus methods!");

    if (mutex_init (&i2c->lock)  != 0)
        {
        return -1;
        }

    if (mutex_lock (&__bus_lock) != 0)
        {
        return -1;
        }

    i2c->name    = name;
    i2c->methods = methods;

    dlist_add_tail (&__i2c_buses, &i2c->node);

    mutex_unlock (&__bus_lock);

    return 0;
    }

/**
 * hal_i2c_xfer - transfer data to i2c address
 * @i2c:  the i2c channel
 * @addr: the i2c slave device address
 * @data: the data buffer
 * @size: the size of data buffer
 *
 * return: the number of bytes really transferred
 */

int hal_i2c_xfer (hal_i2c_t * i2c, uint16_t addr, const uint8_t * data,
                  uint16_t size)
    {
    int ret;

    WARN_ON (i2c == NULL,
             errno = ERRNO_HAL_I2C_ILLEGAL_ID; return -1,
             "Invalid I2C bus!");

    if (unlikely (mutex_lock (&i2c->lock) != 0))
        {
        return -1;
        }

    ret = i2c->methods->xfer (i2c, addr, data, size);

    mutex_unlock (&i2c->lock);

    return ret;
    }

/**
 * hal_i2c_recv - recieve data from a i2c channel
 * @i2c:  the i2c channel
 * @addr: the i2c slave device address
 * @data: the data buffer
 * @size: the size of data wanted
 *
 * return: the number of bytes really recieved
 */

int hal_i2c_recv (hal_i2c_t * i2c, uint16_t addr, uint8_t * data, uint16_t size)
    {
    int ret;

    WARN_ON (i2c == NULL,
             errno = ERRNO_HAL_I2C_ILLEGAL_ID; return -1,
             "Invalid I2C bus!");

    if (unlikely (mutex_lock (&i2c->lock) != 0))
        {
        return -1;
        }

    ret = i2c->methods->recv (i2c, addr, data, size);

    mutex_unlock (&i2c->lock);

    return ret;
    }

#ifdef CONFIG_DEVFS
static int __i2c_open (uintptr_t data)
    {
    hal_i2c_dev_t * dev = (hal_i2c_dev_t *) data;

    return dev->init   == NULL ? 0 : dev->init (dev);
    }

static int __i2c_close (uintptr_t data)
    {
    hal_i2c_dev_t * dev = (hal_i2c_dev_t *) data;

    return dev->deinit == NULL ? 0 : dev->deinit (dev);
    }

static int __i2c_read (uintptr_t data, char * buff, size_t nbyte)
    {
    hal_i2c_dev_t * dev = (hal_i2c_dev_t *) data;

    return hal_i2c_recv (dev->i2c, dev->addr, (uint8_t *) buff, (uint16_t) nbyte);
    }

static int __i2c_write (uintptr_t data, const char * buff, size_t nbyte)
    {
    hal_i2c_dev_t * dev = (hal_i2c_dev_t *) data;

    return hal_i2c_xfer (dev->i2c, dev->addr, (const uint8_t *) buff,
                         (uint16_t) nbyte);
    }

static const struct devfs_ops __i2c_ops =
    {
    __i2c_open,
    __i2c_close,
    __i2c_read,
    __i2c_write,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
    };
#endif

/**
 * hal_i2c_dev_register - i2c slave device register
 * @dev:  the device object
 * @bus:  the bus name the device attached to
 * @name: the i2c slave device name
 * @addr: the address of the slave device
 *
 * return: 0 on success, negtive value on error
 */

int hal_i2c_dev_register (hal_i2c_dev_t * dev, const char * bus,
                          const char * name, uint16_t addr,
                          int (* init)   (struct hal_i2c_dev *),
                          int (* deinit) (struct hal_i2c_dev *))
    {
    hal_i2c_t * i2c = hal_i2c_open (bus);

    BUG_ON (i2c == NULL, "Invalid I2C device!");

    dev->i2c    = i2c;
    dev->name   = name;
    dev->addr   = addr;
    dev->init   = init;
    dev->deinit = deinit;

    if (unlikely (mutex_lock (&__dev_lock) != 0))
        {
        hal_i2c_close (i2c);
        return -1;
        }

    dlist_add_tail (&__i2c_devs, &dev->node);

    (void) mutex_unlock (&__dev_lock);

#ifdef CONFIG_DEVFS
    return devfs_add_file (name, &__i2c_ops, (uintptr_t) dev);
#else
    return 0;
#endif
    }

/**
 * hal_i2c_dev_open - open a i2c device by name
 * @name: the slave i2c device name
 *
 * return: a pointer to the i2c control struct or NULL if device not found
 */

hal_i2c_dev_t * hal_i2c_dev_open (const char * name)
    {
    dlist_t       * itr;
    hal_i2c_dev_t * dev;

    WARN_ON (name == NULL,
             return NULL,
             "Invalid name!");

    mutex_lock (&__dev_lock);

    dlist_foreach (itr, &__i2c_devs)
        {
        dev = container_of (itr, hal_i2c_dev_t, node);

        if (strcmp (name, dev->name) == 0)
            {
            mutex_unlock (&__dev_lock);

            if ((dev->init != NULL) && (dev->init (dev) != 0))
                {
                return NULL;
                }

            return dev;
            }
        }

    mutex_unlock (&__dev_lock);

    errno = ERRNO_HAL_I2C_NO_MATCH;

    WARN ("Fail to open device \"%s\"!", name);

    return NULL;
    }

/**
 * hal_i2c_dev_xfer - transfer data to i2c slave device
 * @dev:  the slave i2c device
 * @data: the data buffer
 * @size: the size of data buffer
 *
 * return: the number of bytes really transferred
 */

int hal_i2c_dev_xfer (hal_i2c_dev_t * dev, const uint8_t * data, uint16_t size)
    {
    WARN_ON (dev == NULL,
             return -1,
             "Invalid I2C device!"
             );

    return hal_i2c_xfer (dev->i2c, dev->addr, data, size);
    }

/**
 * hal_i2c_dev_recv - recieve data from a i2c slave device
 * @dev:  the slave i2c device
 * @data: the data buffer
 * @size: the size of data wanted
 *
 * return: the number of bytes really recieved
 */

int hal_i2c_dev_recv (hal_i2c_dev_t * dev, uint8_t * data, uint16_t size)
    {
    WARN_ON (dev == NULL,
             return -1,
             "Invalid I2C device!"
             );

    return hal_i2c_recv (dev->i2c, dev->addr, data, size);
    }

/**
 * hal_i2c_init - i2c hardware abstrction library initialization routine
 *
 * return: 0 on success, negtive value on error
 */

static int hal_i2c_init (void)
    {
    if (unlikely (mutex_init (&__bus_lock) != 0))
        {
        return -1;
        }

    if (unlikely (mutex_init (&__dev_lock) != 0))
        {
        return -1;
        }

    return 0;
    }

MODULE_INIT (hal, hal_i2c_init);

