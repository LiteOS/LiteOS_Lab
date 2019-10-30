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

#include <stdlib.h>
#include <string.h>

#include <config.h>
#include <common.h>
#include <vfs.h>

enum ramfs_node_type
    {
    RAMFS_NODE_DIR,
    RAMFS_NODE_FILE
    };

struct ramfs_node
    {
    enum ramfs_node_type type;
    struct ramfs_node  * parent;
    dlist_t              sablings;
    union
        {
        dlist_t          child;
        struct
            {
            char       * buffer; /* the buffer if this node is buffer */
            size_t       size;   /* the file (buffer) size if node is file */
            };
        };
    char                 name [0];
    };

struct ramfs_file
    {
    uint32_t             offset;
    struct ramfs_node  * node;
    };

static struct ramfs_node * __path_resolve (struct ramfs_node * root,
                                           const char *  path,
                                           const char ** path_unresolved)
    {
    struct ramfs_node * walk = root;
    const char        * temp;
    size_t              len;
    dlist_t           * itr;

    while (1)
        {
next:
        if (walk->type != RAMFS_NODE_DIR)
            {
            return NULL;
            }

        if (path [0] != '\0' && (!dlist_empty (&walk->child)))
            {
            temp = strstr (path, "/");

            len = temp == NULL ? strlen (path) : temp - path;

            dlist_foreach (itr, &walk->child)
                {
                struct ramfs_node * node = container_of (itr, struct ramfs_node,
                                                         sablings);

                if (strncmp (node->name, path, len) != 0)
                    {
                    continue;
                    }

                if (temp == NULL)       // !!! found
                    {
                    *path_unresolved = NULL;
                    return node;
                    }

                if (temp != NULL)       // !!! next dir
                    {
                    path = temp + 1;    // skip '/'
                    walk = node;
                    goto next;
                    }
                }
            }

        // !!! not found, found most dir.

        *path_unresolved = path;
        return walk;
        }
    }

static int ramfs_open (uintptr_t mp_data, uintptr_t * fl_data_ptr,
                       const char * path, int flags)
    {
    struct ramfs_file * file;
    struct ramfs_node * root = (struct ramfs_node *) mp_data;
    const char        * unresolved;
    struct ramfs_node * node = __path_resolve (root, path, &unresolved);
    struct ramfs_node * parent;

    /* path invalid */

    if (node == NULL)
        {
        return -1;
        }

    /* opening dir */

    if (node->type == RAMFS_NODE_DIR && unresolved == NULL)
        {
        return -1;
        }

    file = (struct ramfs_file *) malloc (sizeof (struct ramfs_file));

    file->offset = 0;

    /* if found */

    if (unresolved == NULL)
        {
        if (flags & O_APPEND)
            {
            file->offset = (uint32_t) node->size;
            }

        file->node   = node;

        *fl_data_ptr = (uintptr_t) file;

        return 0;
        }

    if ((flags & O_CREAT) == 0)
        {
        goto err_free_file;
        }

    if (node->type != RAMFS_NODE_DIR)
        {
        goto err_free_file;
        }

    if (strchr (unresolved, '/') != NULL)
        {
        goto err_free_file;
        }

    if (strlen (unresolved) >= CONFIG_MAX_FILE_NAME_LEN)
        {
        goto err_free_file;
        }

    parent = node;

    node = (struct ramfs_node *) malloc (sizeof (struct ramfs_node) +
                                         strlen (unresolved) + 1);

    if (node == NULL)
        {
        goto err_free_file;
        }

    file->node = node;

    /* it is safe to use strcpy here */

    strcpy (node->name, unresolved);

    node->type   = RAMFS_NODE_FILE;
    node->parent = parent;
    node->buffer = NULL;
    node->size   = 0;

    dlist_add (&parent->child, &node->sablings);

    *fl_data_ptr = (uintptr_t) file;

    return 0;

err_free_file:
    free (file);
    return -1;
    }

static int ramfs_read (uintptr_t fl_data, char * buff, size_t nbyte)
    {
    struct ramfs_file * file = (struct ramfs_file *) fl_data;
    struct ramfs_node * node = (struct ramfs_node *) file->node;

    if (node->size - file->offset < nbyte)
        {
        nbyte = node->size - file->offset;
        }

    if (nbyte != 0)
        {
        memcpy (buff, node->buffer + file->offset, nbyte);
        file->offset += nbyte;
        }

    return nbyte;
    }

static int ramfs_write (uintptr_t fl_data, const char * buff, size_t nbyte)
    {
    struct ramfs_file * file = (struct ramfs_file *) fl_data;
    struct ramfs_node * node = (struct ramfs_node *) file->node;
    char              * new;

    if (node->buffer == NULL)
        {
        node->buffer = (char *) malloc (nbyte);
        node->size   = nbyte;
        }
    else if (node->size - file->offset < nbyte)
        {
        node->size = file->offset + nbyte;

        new = (char *) realloc (node->buffer, node->size);

        if (new == NULL)
            {
            return -1;
            }

        node->buffer = new;
        }

    if (nbyte != 0)
        {
        memcpy (node->buffer + file->offset, buff, nbyte);
        file->offset += nbyte;
        }

    return nbyte;
    }

static int ramfs_lseek (uintptr_t fl_data, int off, int whence)
    {
    struct ramfs_file * file = (struct ramfs_file *) fl_data;
    struct ramfs_node * node = (struct ramfs_node *) file->node;

    switch (whence)
        {
        case 0: // SEEK_SET
            file->offset  = off;
            break;
        case 1: // SEEK_CUR
            file->offset += off;
            break;
        case 2: // SEEK_END
            file->offset  = node->size;
            break;
        default:
            return -1;
        }

    if (file->offset < 0)
        {
        file->offset = 0;
        }
    else if (file->offset > node->size)
        {
        file->offset = node->size;
        }

    return file->offset;
    }

static int ramfs_ioctl (uintptr_t fl_data, int request, va_list valist)
    {
    return -1;
    }

static int ramfs_sync (uintptr_t fl_data)
    {
    return -1;
    }

static int ramfs_stat (uintptr_t mp_data, const char * path, struct stat * stat)
    {
    struct ramfs_node * root = (struct ramfs_node *) mp_data;
    const char        * unresolved;
    struct ramfs_node * node = __path_resolve (root, path, &unresolved);

    /* path invalid */

    if (node == NULL)
        {
        return -1;
        }

    /* if not found */

    if (unresolved != NULL)
        {
        return -1;
        }

    stat->st_mode = 0777;

    /* operating dir */

    if (node->type == RAMFS_NODE_DIR)
        {
        stat->st_mode |= S_IFDIR;
        }
    else
        {
        stat->st_mode |= S_IFREG;
        }

    return 0;
    }

static int ramfs_unlink (uintptr_t mp_data, const char * path)
    {
    struct ramfs_node * root = (struct ramfs_node *) mp_data;
    const char        * unresolved;
    struct ramfs_node * node = __path_resolve (root, path, &unresolved);

    /* path invalid or not exist */

    if (node == NULL || unresolved != '\0')
        {
        return -1;
        }

    if (node->type == RAMFS_NODE_DIR)
        {
        if (!dlist_empty (&node->child))
            {
            return -1;
            }
        }
    else
        {
        if (node->buffer != NULL)
            {
            free (node->buffer);
            }
        }

    if (node == root)
        {
        return -1;
        }

    dlist_del (&node->sablings);

    free (node);

    return 0;
    }

static int ramfs_rename (uintptr_t mp_data, const char * from, const char * to)
    {
    return -1;
    }

static int ramfs_opendir (uintptr_t mp_data, uintptr_t * dr_data_ptr, const char * path)
    {
    struct ramfs_file * file;
    struct ramfs_node * root = (struct ramfs_node *) mp_data;
    const char        * unresolved;
    struct ramfs_node * node = __path_resolve (root, path, &unresolved);

    /* path invalid or not exist */

    if (node == NULL || *unresolved != '\0')
        {
        return -1;
        }

    if (node->type != RAMFS_NODE_DIR)
        {
        return -1;
        }

    file = (struct ramfs_file *) malloc (sizeof (struct ramfs_file));

    if (file == NULL)
        {
        return -1;
        }

    file->node   = node;
    file->offset = 0;

    *dr_data_ptr = (uintptr_t) file;

    return 0;
    }

static int ramfs_readdir (uintptr_t mp_data, uintptr_t dr_data, struct dirent * de)
    {
    struct ramfs_file * file = (struct ramfs_file *) dr_data;
    struct ramfs_node * root = (struct ramfs_node *) mp_data;
    struct ramfs_node * node = file->node;
    dlist_t           * itr;
    int                 i = 0;

    dlist_foreach (itr, &node->child)
        {
        if (i++ == file->offset)
            {
            node = container_of (itr, struct ramfs_node, sablings);

            strncpy (de->de_name, node->name, CONFIG_MAX_FILE_NAME_LEN);

            if (node->type == RAMFS_NODE_DIR)
                {
                de->de_type = "dir";
                }
            else
                {
                de->de_type = "file";
                de->de_size = node->size;
                }

            file->offset++;

            return 0;
            }
        }

    return -1;
    }

static int ramfs_closedir (uintptr_t mp_data, uintptr_t dr_data)
    {
    struct ramfs_file * file = (struct ramfs_file *) dr_data;

    free (file);

    return 0;
    }

static int ramfs_mkdir (uintptr_t mp_data, const char * path)
    {
    struct ramfs_node * root = (struct ramfs_node *) mp_data;
    const char        * unresolved;
    struct ramfs_node * node = __path_resolve (root, path, &unresolved);
    struct ramfs_node * new;

    /* path invalid or file/dir exist */

    if ((node == NULL)                     ||
        (node->type != RAMFS_NODE_DIR)     ||
        (strchr (unresolved, '/') != NULL) ||
        (unresolved == NULL))
        {
        return -1;
        }

    new = (struct ramfs_node *) malloc (sizeof (struct ramfs_node) +
                                        strlen (unresolved) + 1);

    if (new == NULL)
        {
        return -1;
        }

    new->type   = RAMFS_NODE_DIR;
    new->parent = node;

    /* set name */

    strcpy (new->name, unresolved);

    dlist_init (&new->child);
    dlist_add  (&node->child, &new->sablings);

    return 0;
    }

static int ramfs_mount (uintptr_t * mp_data_ptr, va_list valist)
    {
    struct ramfs_node * root;

    root = (struct ramfs_node *) malloc (sizeof (struct ramfs_node));

    if (root == NULL)
        {
        return -1;
        }

    root->type   = RAMFS_NODE_DIR;
    root->parent = NULL;

    dlist_init (&root->sablings);

    dlist_init (&root->child);

    *mp_data_ptr   = (uintptr_t) root;

    return 0;
    }

static const struct f_ops ramfs_fops =
    {
    ramfs_open,
    NULL,
    ramfs_read,
    ramfs_write,
    ramfs_lseek,
    ramfs_ioctl,
    ramfs_sync,
    };

static const struct m_ops ramfs_mops =
    {
    ramfs_stat,
    ramfs_unlink,
    ramfs_rename,
    ramfs_opendir,
    ramfs_readdir,
    ramfs_closedir,
    ramfs_mkdir,
    ramfs_mount,
    };

static struct file_system ramfs =
    {
    "ramfs",
    &ramfs_fops,
    &ramfs_mops,
    };

int ramfs_init (void)
    {
    return vfs_fs_register (&ramfs);
    }

