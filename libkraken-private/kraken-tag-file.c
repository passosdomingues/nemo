#include <config.h>
#include "kraken-tag-file.h"
#include "kraken-file-private.h"
#include <glib/gi18n.h>

G_DEFINE_TYPE (KrakenTagFile, kraken_tag_file, KRAKEN_TYPE_FILE);

static void kraken_tag_file_init (KrakenTagFile *self) {
    KrakenFile *file = KRAKEN_FILE (self);

    file->details->got_file_info = TRUE;
    file->details->mime_type = g_ref_string_new_intern ("x-directory/normal");
    file->details->type = G_FILE_TYPE_DIRECTORY;
    file->details->size = 0;
    file->details->file_info_is_up_to_date = TRUE;

    file->details->got_link_info = TRUE;
    file->details->link_info_is_up_to_date = TRUE;
}

static void kraken_tag_file_class_init (KrakenTagFileClass *klass) {
    KrakenFileClass *file_class = KRAKEN_FILE_CLASS (klass);
    file_class->default_file_type = G_FILE_TYPE_DIRECTORY;
}

KrakenFile *kraken_tag_file_new (KrakenDirectory *directory, const char *name) {
    KrakenTagFile *self = g_object_new (KRAKEN_TYPE_TAG_FILE, NULL);
    KrakenFile *file = KRAKEN_FILE (self);
    
    file->details->directory = kraken_directory_ref (directory);
    file->details->name = g_ref_string_new (name);
    kraken_file_set_display_name (file, name, NULL, TRUE);
    
    return file;
}
