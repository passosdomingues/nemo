#ifndef KRAKEN_TAG_MANAGER_H
#define KRAKEN_TAG_MANAGER_H

#include <glib-object.h>
#include <gio/gio.h>
#include <kraken-core/kraken-daemon.h>

#define KRAKEN_TYPE_TAG_MANAGER kraken_tag_manager_get_type()
G_DECLARE_FINAL_TYPE (KrakenTagManager, kraken_tag_manager, KRAKEN, TAG_MANAGER, GObject)

KrakenTagManager *kraken_tag_manager_get (void);

void kraken_tag_manager_get_tags_async (KrakenTagManager *self,
                                         GAsyncReadyCallback callback,
                                         gpointer user_data);
GList *kraken_tag_manager_get_tags_finish (KrakenTagManager *self,
                                           GAsyncResult *res,
                                           GError **error);

void kraken_tag_manager_get_files_by_tag_async (KrakenTagManager *self,
                                                 const char *tag_name,
                                                 GAsyncReadyCallback callback,
                                                 gpointer user_data);
GList *kraken_tag_manager_get_files_by_tag_finish (KrakenTagManager *self,
                                                   GAsyncResult *res,
                                                   GError **error);

#endif /* KRAKEN_TAG_MANAGER_H */
