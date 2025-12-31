#ifndef KRAKEN_TOON_H
#define KRAKEN_TOON_H

#include <glib.h>
#include <json-glib/json-glib.h>

typedef struct {
    char* id;
    char** types;
    long timestamp;
} kraken_toon_entity_t;

typedef struct {
    char* predicate;
    char* object;
    float confidence;
} kraken_toon_relation_t;

typedef struct {
    kraken_toon_entity_t entity;
    GList* relations; /* List of kraken_toon_relation_t* */
    float* embedding;
    int embedding_dim;
} kraken_toon_t;

kraken_toon_t* kraken_toon_new(const char* entity_id);
void kraken_toon_add_relation(kraken_toon_t* self, const char* predicate, const char* object, float confidence);
void kraken_toon_set_embedding(kraken_toon_t* self, float* embedding, int dim);

char* kraken_toon_to_json(kraken_toon_t* self);
kraken_toon_t* kraken_toon_from_json(const char* json_data);

void kraken_toon_free(kraken_toon_t* self);

#endif /* KRAKEN_TOON_H */
