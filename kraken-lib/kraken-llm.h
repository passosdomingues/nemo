#ifndef KRAKEN_LLM_H
#define KRAKEN_LLM_H

#include <glib.h>
#include "toon.h"

typedef struct {
    char* model_name;
    char* api_base_url;
} kraken_llm_t;

typedef struct {
    char* file_path;
    char* content;
    char* api_url;
    char* result;
    GError* error;
    char* model_path;
} LLMTask;

kraken_llm_t* kraken_llm_new(const char* model_name);
void kraken_llm_free(kraken_llm_t* self);

/* Asynchronous calls to LLM */
void kraken_llm_summarize_async(kraken_llm_t* self, const char* file_path, GAsyncReadyCallback callback, gpointer user_data);
char* kraken_llm_summarize_finish(kraken_llm_t* self, GAsyncResult* res, GError** error);

void kraken_llm_embed_async(kraken_llm_t* self, const char* file_path, const char* text, GAsyncReadyCallback callback, gpointer user_data);
float* kraken_llm_embed_finish(kraken_llm_t* self, GAsyncResult* res, int* out_dim, GError** error);

void kraken_llm_extract_tags_async(kraken_llm_t* self, const char* file_path, const char* text, GAsyncReadyCallback callback, gpointer user_data);
char** kraken_llm_extract_tags_finish(kraken_llm_t* self, GAsyncResult* res, GError** error);

#endif /* KRAKEN_LLM_H */
