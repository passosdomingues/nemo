/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/* 
 * Copyright (C) 2004 Red Hat, Inc
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street - Suite 500,
 * Boston, MA 02110-1335, USA.
 *
 * Author: Alexander Larsson <alexl@redhat.com>
 */

#ifndef KRAKEN_IMAGE_PROPERTIES_PAGE_H
#define KRAKEN_IMAGE_PROPERTIES_PAGE_H

#include <gtk/gtk.h>

#define KRAKEN_TYPE_IMAGE_PROPERTIES_PAGE kraken_image_properties_page_get_type()
#define KRAKEN_IMAGE_PROPERTIES_PAGE(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), KRAKEN_TYPE_IMAGE_PROPERTIES_PAGE, KrakenImagePropertiesPage))
#define KRAKEN_IMAGE_PROPERTIES_PAGE_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), KRAKEN_TYPE_IMAGE_PROPERTIES_PAGE, KrakenImagePropertiesPageClass))
#define KRAKEN_IS_IMAGE_PROPERTIES_PAGE(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), KRAKEN_TYPE_IMAGE_PROPERTIES_PAGE))
#define KRAKEN_IS_IMAGE_PROPERTIES_PAGE_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), KRAKEN_TYPE_IMAGE_PROPERTIES_PAGE))
#define KRAKEN_IMAGE_PROPERTIES_PAGE_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), KRAKEN_TYPE_IMAGE_PROPERTIES_PAGE, KrakenImagePropertiesPageClass))

typedef struct KrakenImagePropertiesPageDetails KrakenImagePropertiesPageDetails;

typedef struct {
	GtkBox parent;
	KrakenImagePropertiesPageDetails *details;
} KrakenImagePropertiesPage;

typedef struct {
	GtkBoxClass parent;
} KrakenImagePropertiesPageClass;

GType kraken_image_properties_page_get_type (void);
void  kraken_image_properties_page_register (void);

#endif /* KRAKEN_IMAGE_PROPERTIES_PAGE_H */
