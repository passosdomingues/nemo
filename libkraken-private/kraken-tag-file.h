#ifndef KRAKEN_TAG_FILE_H
#define KRAKEN_TAG_FILE_H

#include "kraken-file.h"

typedef struct _KrakenDirectory KrakenDirectory;

#define KRAKEN_TYPE_TAG_FILE kraken_tag_file_get_type()
#define KRAKEN_TAG_FILE(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), KRAKEN_TYPE_TAG_FILE, KrakenTagFile))
#define KRAKEN_TAG_FILE_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), KRAKEN_TYPE_TAG_FILE, KrakenTagFileClass))
#define KRAKEN_IS_TAG_FILE(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), KRAKEN_TYPE_TAG_FILE))
#define KRAKEN_IS_TAG_FILE_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), KRAKEN_TYPE_TAG_FILE))
#define KRAKEN_TAG_FILE_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), KRAKEN_TYPE_TAG_FILE, KrakenTagFileClass))

typedef struct KrakenTagFileDetails KrakenTagFileDetails;

typedef struct
{
	KrakenFile parent_slot;
	KrakenTagFileDetails *details;
} KrakenTagFile;

typedef struct
{
	KrakenFileClass parent_slot;
} KrakenTagFileClass;

GType kraken_tag_file_get_type (void);
KrakenFile *kraken_tag_file_new (KrakenDirectory *directory, const char *name);

#endif /* KRAKEN_TAG_FILE_H */
