

#ifndef __KRAKEN_RECENT_H__
#define __KRAKEN_RECENT_H__

#include <gtk/gtk.h>
#include <libkraken-private/kraken-file.h>
#include <gio/gio.h>

void kraken_recent_add_file (KrakenFile *file,
			       GAppInfo *application);

#endif
