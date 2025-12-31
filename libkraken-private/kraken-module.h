/*
 *  kraken-module.h - Interface to kraken extensions
 * 
 *  Copyright (C) 2003 Novell, Inc.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public
 *  License along with this library; if not, write to the Free
 *  Software Foundation, Inc., 51 Franklin Street, Suite 500, MA 02110-1335, USA.
 *
 *  Author: Dave Camp <dave@ximian.com>
 * 
 */

#ifndef KRAKEN_MODULE_H
#define KRAKEN_MODULE_H

#include <glib-object.h>
#include <gmodule.h>


G_BEGIN_DECLS

#define KRAKEN_TYPE_MODULE        (kraken_module_get_type ())
#define KRAKEN_MODULE(obj)        (G_TYPE_CHECK_INSTANCE_CAST ((obj), KRAKEN_TYPE_MODULE, KrakenModule))
#define KRAKEN_MODULE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), KRAKEN_TYPE_MODULE, KrakenModule))
#define KRAKEN_IS_MODULE(obj)     (G_TYPE_CHECK_INSTANCE_TYPE ((obj), KRAKEN_TYPE_MODULE))
#define KRAKEN_IS_MODULE_CLASS(klass) (G_TYPE_CLASS_CHECK_CLASS_TYPE ((klass), KRAKEN_TYPE_MODULE))

typedef struct _KrakenModule        KrakenModule;
typedef struct _KrakenModuleClass   KrakenModuleClass;

struct _KrakenModule {
    GTypeModule parent;

    GModule *library;

    char *path;

    void (*initialize) (GTypeModule  *module);
    void (*shutdown)   (void);

    void (*list_types) (const GType **types,
                int          *num_types);
    void (*get_modules_name_and_desc) (gchar ***strings);
};

struct _KrakenModuleClass {
    GTypeModuleClass parent;
};

GType kraken_module_get_type (void);

void   kraken_module_setup                   (void);
void   kraken_module_refresh                 (void);
GList *kraken_module_get_extensions_for_type (GType  type);
void   kraken_module_extension_list_free     (GList *list);

/* Add a type to the module interface - allows kraken to add its own modules
 * without putting them in separate shared libraries */
void   kraken_module_add_type                (GType  type);

G_END_DECLS

#endif
