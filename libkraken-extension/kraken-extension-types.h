/*
 *  kraken-info-provider.h - Type definitions for Kraken extensions
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

/* This interface is implemented by Kraken extensions that want to 
 * provide information about files.  Extensions are called when Kraken 
 * needs information about a file.  They are passed a KrakenFileInfo 
 * object which should be filled with relevant information */

#ifndef KRAKEN_EXTENSION_TYPES_H
#define KRAKEN_EXTENSION_TYPES_H

#include <glib-object.h>

G_BEGIN_DECLS

/**
 * SECTION:kraken-extension-types
 * @Title: Types and Enums
 * @Short_description: Module initialization functions, enums, handle struct
 **/

/**
 * KrakenOperationHandle:
 *
 * Handle for asynchronous interfaces.  These are opaque handles that must
 * be unique within an extension object.  These are returned by operations
 * that return KRAKEN_OPERATION_IN_PROGRESS.
 *
 * For python extensions, the handle is a dummy struct created by the kraken
 * python bindings on the extension's behalf.  It can be used as a unique
 * key for a dict, for instance, for keeping track of multiple operations
 * at once.
 */
typedef struct _KrakenOperationHandle KrakenOperationHandle;

/**
 * KrakenOperationResult:
 * @KRAKEN_OPERATION_COMPLETE: Returned if the call succeeded, and the extension is done
 *  with the request.
 * @KRAKEN_OPERATION_FAILED: Returned if the call failed.
 * @KRAKEN_OPERATION_IN_PROGRESS: Returned if the extension has begun an async operation.
 *  For C extensions, if this is returned, the extension must set the handle parameter.
 *  For python extensions, handle is already filled, and unique, and can be used for
 *  identifying purposes within the extension.  In either case, the extension must call
 *  the callback closure when the operation is complete (complete_invoke.)
 */
typedef enum {
    KRAKEN_OPERATION_COMPLETE,
    KRAKEN_OPERATION_FAILED,
    KRAKEN_OPERATION_IN_PROGRESS
} KrakenOperationResult;

void kraken_module_initialize (GTypeModule  *module);
void kraken_module_shutdown   (void);
void kraken_module_list_types (const GType **types,
				 int          *num_types);

G_END_DECLS

#endif
