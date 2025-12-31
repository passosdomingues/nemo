#include <config.h>
#include <libkraken-private/kraken-directory.h>
#include <libkraken-private/kraken-tag-directory.h>
#include <gio/gio.h>
#include <glib-object.h>
#include <stdio.h>

int main(int argc, char** argv) {
    g_type_init();
    
    KrakenDirectory *dir = kraken_directory_get_by_uri ("kraken://tags/");
    if (dir == NULL) {
        printf ("FAILED: kraken://tags/ returned NULL\n");
        return 1;
    }
    
    if (KRAKEN_IS_TAG_DIRECTORY (dir)) {
        printf ("SUCCESS: kraken://tags/ is a KrakenTagDirectory\n");
    } else {
        printf ("FAILED: kraken://tags/ is NOT a KrakenTagDirectory (actual type: %s)\n", G_OBJECT_TYPE_NAME (dir));
        return 1;
    }
    
    kraken_directory_unref (dir);
    return 0;
}
