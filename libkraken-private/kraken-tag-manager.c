#include "kraken-tag-manager.h"

struct _KrakenTagManager {
    GObject parent_instance;
    kraken_daemon_t *daemon;
};

G_DEFINE_TYPE (KrakenTagManager, kraken_tag_manager, G_TYPE_OBJECT)

static void kraken_tag_manager_finalize (GObject *object) {
    KrakenTagManager *self = KRAKEN_TAG_MANAGER (object);
    if (self->daemon) {
        kraken_daemon_free (self->daemon);
    }
    G_OBJECT_CLASS (kraken_tag_manager_parent_class)->finalize (object);
}

static void kraken_tag_manager_class_init (KrakenTagManagerClass *klass) {
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    object_class->finalize = kraken_tag_manager_finalize;
}

static void kraken_tag_manager_init (KrakenTagManager *self) {
    self->daemon = kraken_daemon_new ();
}

KrakenTagManager *kraken_tag_manager_get (void) {
    static KrakenTagManager *instance = NULL;
    if (g_once_init_enter (&instance)) {
        KrakenTagManager *new_instance = g_object_new (KRAKEN_TYPE_TAG_MANAGER, NULL);
        g_once_init_leave (&instance, new_instance);
    }
    return instance;
}

void kraken_tag_manager_get_tags_async (KrakenTagManager *self, GAsyncReadyCallback callback, gpointer user_data) {
    g_return_if_fail (KRAKEN_IS_TAG_MANAGER (self));
    kraken_daemon_get_tags_async (self->daemon, callback, user_data);
}

GList *kraken_tag_manager_get_tags_finish (KrakenTagManager *self, GAsyncResult *res, GError **error) {
    g_return_val_if_fail (KRAKEN_IS_TAG_MANAGER (self), NULL);
    return kraken_daemon_get_tags_finish (self->daemon, res, error);
}

void kraken_tag_manager_get_files_by_tag_async (KrakenTagManager *self, const char *tag_name, GAsyncReadyCallback callback, gpointer user_data) {
    g_return_if_fail (KRAKEN_IS_TAG_MANAGER (self));
    kraken_daemon_get_files_by_tag_async (self->daemon, tag_name, callback, user_data);
}

GList *kraken_tag_manager_get_files_by_tag_finish (KrakenTagManager *self, GAsyncResult *res, GError **error) {
    g_return_val_if_fail (KRAKEN_IS_TAG_MANAGER (self), NULL);
    return kraken_daemon_get_files_by_tag_finish (self->daemon, res, error);
}
