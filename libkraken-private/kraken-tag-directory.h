#ifndef KRAKEN_TAG_DIRECTORY_H
#define KRAKEN_TAG_DIRECTORY_H

#include "kraken-directory.h"

#define KRAKEN_TYPE_TAG_DIRECTORY kraken_tag_directory_get_type()
#define KRAKEN_TAG_DIRECTORY(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), KRAKEN_TYPE_TAG_DIRECTORY, KrakenTagDirectory))
#define KRAKEN_TAG_DIRECTORY_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), KRAKEN_TYPE_TAG_DIRECTORY, KrakenTagDirectoryClass))
#define KRAKEN_IS_TAG_DIRECTORY(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), KRAKEN_TYPE_TAG_DIRECTORY))
#define KRAKEN_IS_TAG_DIRECTORY_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), KRAKEN_TYPE_TAG_DIRECTORY))
#define KRAKEN_TAG_DIRECTORY_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), KRAKEN_TYPE_TAG_DIRECTORY, KrakenTagDirectoryClass))

typedef struct KrakenTagDirectoryDetails KrakenTagDirectoryDetails;

typedef struct
{
	KrakenDirectory parent_slot;
	KrakenTagDirectoryDetails *details;
} KrakenTagDirectory;

typedef struct
{
	KrakenDirectoryClass parent_slot;
} KrakenTagDirectoryClass;

GType kraken_tag_directory_get_type (void);

#endif /* KRAKEN_TAG_DIRECTORY_H */
