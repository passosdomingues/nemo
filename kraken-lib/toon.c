#include <stdlib.h>
#include <string.h>
#include "toon.h"

kraken_toon_t* kraken_toon_new(const char* entity_id) {
    kraken_toon_t* self = g_new0(kraken_toon_t, 1);
    self->entity.id = g_strdup(entity_id);
    self->entity.timestamp = g_get_real_time();
    return self;
}

void kraken_toon_add_relation(kraken_toon_t* self, const char* predicate, const char* object, float confidence) {
    kraken_toon_relation_t* rel = g_new0(kraken_toon_relation_t, 1);
    rel->predicate = g_strdup(predicate);
    rel->object = g_strdup(object);
    rel->confidence = confidence;
    self->relations = g_list_append(self->relations, rel);
}

void kraken_toon_set_embedding(kraken_toon_t* self, float* embedding, int dim) {
    self->embedding = g_new(float, dim);
    memcpy(self->embedding, embedding, sizeof(float) * dim);
    self->embedding_dim = dim;
}

char* kraken_toon_to_json(kraken_toon_t* self) {
    JsonBuilder* builder = json_builder_new();
    json_builder_begin_object(builder);
    
    json_builder_set_member_name(builder, "@context");
    json_builder_add_string_value(builder, "kraken://toon/1.0");
    
    json_builder_set_member_name(builder, "entity");
    json_builder_begin_object(builder);
    json_builder_set_member_name(builder, "id");
    json_builder_add_string_value(builder, self->entity.id);
    json_builder_set_member_name(builder, "timestamp");
    json_builder_add_int_value(builder, self->entity.timestamp);
    json_builder_end_object(builder);
    
    json_builder_set_member_name(builder, "relations");
    json_builder_begin_array(builder);
    for (GList* l = self->relations; l != NULL; l = l->next) {
        kraken_toon_relation_t* rel = (kraken_toon_relation_t*)l->data;
        json_builder_begin_object(builder);
        json_builder_set_member_name(builder, "pred");
        json_builder_add_string_value(builder, rel->predicate);
        json_builder_set_member_name(builder, "obj");
        json_builder_add_string_value(builder, rel->object);
        json_builder_set_member_name(builder, "confidence");
        json_builder_add_double_value(builder, rel->confidence);
        json_builder_end_object(builder);
    }
    json_builder_end_array(builder);
    
    if (self->embedding) {
        json_builder_set_member_name(builder, "embedding");
        json_builder_begin_array(builder);
        for (int i = 0; i < self->embedding_dim; i++) {
            json_builder_add_double_value(builder, self->embedding[i]);
        }
        json_builder_end_array(builder);
    }
    
    json_builder_end_object(builder);
    
    JsonGenerator* gen = json_generator_new();
    JsonNode* root = json_builder_get_root(builder);
    json_generator_set_root(gen, root);
    char* json = json_generator_to_data(gen, NULL);
    
    g_object_unref(builder);
    g_object_unref(gen);
    
    return json;
}

void kraken_toon_free(kraken_toon_t* self) {
    g_free(self->entity.id);
    if (self->entity.types) g_strfreev(self->entity.types);
    
    for (GList* l = self->relations; l != NULL; l = l->next) {
        kraken_toon_relation_t* rel = (kraken_toon_relation_t*)l->data;
        g_free(rel->predicate);
        g_free(rel->object);
        g_free(rel);
    }
    g_list_free(self->relations);
    
    g_free(self->embedding);
    g_free(self);
}
