#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <json-glib/json-glib.h>
#include "kraken-llm.h"

kraken_llm_t* kraken_llm_new(const char* model_name) {
    kraken_llm_t* self = g_new0(kraken_llm_t, 1);
    self->model_name = g_strdup(model_name ? model_name : "deepseek-coder-1.3b-instruct.Q4_K_M.gguf");
    self->api_base_url = g_strdup("http://localhost:8081/completion");
    return self;
}

void kraken_llm_free(kraken_llm_t* self) {
    if (!self) return;
    g_free(self->model_name);
    g_free(self->api_base_url);
    g_free(self);
}

static void llm_task_free(LLMTask* task) {
    if (!task) return;
    g_free(task->file_path);
    g_free(task->content);
    g_free(task->api_url);
    g_free(task->result);
    if (task->error) g_error_free(task->error);
    g_free(task);
}

static size_t write_callback(void *ptr, size_t size, size_t nmemb, void *stream) {
    GString* res = (GString*)stream;
    g_string_append_len(res, ptr, size * nmemb);
    return size * nmemb;
}

static void on_llm_thread(GTask *task, gpointer source_object, gpointer task_data, GCancellable *cancellable) {
    LLMTask* data = (LLMTask*)task_data;
    CURL *curl;
    CURLcode res;
    GString* response_string = g_string_new("");
    
    if (!data || !data->api_url) {
        g_task_return_new_error(task, G_IO_ERROR, G_IO_ERROR_FAILED, "Invalid task data");
        g_string_free(response_string, TRUE);
        return;
    }

    curl = curl_easy_init();
    if (curl) {
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        
        JsonBuilder* builder = json_builder_new();
        json_builder_begin_object(builder);
        json_builder_set_member_name(builder, "prompt");
        
        char* full_prompt = g_strdup_printf("<|im_start|>system\nYou are a helpful assistant.\n<|im_end|>\n<|im_start|>user\nSummarize the following content in one sentence:\n%s\n<|im_end|>\n<|im_start|>assistant\n", data->content ? data->content : "");
        json_builder_add_string_value(builder, full_prompt);
        g_free(full_prompt);
        
        json_builder_set_member_name(builder, "n_predict");
        json_builder_add_int_value(builder, 64);
        json_builder_set_member_name(builder, "stream");
        json_builder_add_boolean_value(builder, FALSE);
        json_builder_end_object(builder);
        
        JsonGenerator* gen = json_generator_new();
        JsonNode* root = json_builder_get_root(builder);
        json_generator_set_root(gen, root);
        char* post_data = json_generator_to_data(gen, NULL);
        
        curl_easy_setopt(curl, CURLOPT_URL, data->api_url);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_data);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, response_string);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);
        
        res = curl_easy_perform(curl);
        if (res == CURLE_OK) {
            JsonParser* parser = json_parser_new();
            if (json_parser_load_from_data(parser, response_string->str, -1, NULL)) {
                JsonNode* root_node = json_parser_get_root(parser);
                if (root_node && JSON_NODE_HOLDS_OBJECT(root_node)) {
                    JsonObject* obj = json_node_get_object(root_node);
                    if (json_object_has_member(obj, "content")) {
                        data->result = g_strdup(json_object_get_string_member(obj, "content"));
                        g_task_return_pointer(task, g_strdup(data->result), g_free);
                    } else {
                        g_task_return_new_error(task, G_IO_ERROR, G_IO_ERROR_FAILED, "Invalid server response");
                    }
                } else {
                     g_task_return_new_error(task, G_IO_ERROR, G_IO_ERROR_FAILED, "Invalid JSON root");
                }
            } else {
                g_task_return_new_error(task, G_IO_ERROR, G_IO_ERROR_FAILED, "Failed to parse LLM response");
            }
            g_object_unref(parser);
        } else {
            g_task_return_new_error(task, G_IO_ERROR, G_IO_ERROR_FAILED, "Curl failed: %s", curl_easy_strerror(res));
        }
        
        g_free(post_data);
        g_object_unref(gen);
        g_object_unref(builder);
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
    }
    
    g_string_free(response_string, TRUE);
}

void kraken_llm_summarize_async(kraken_llm_t* self, const char* file_path, GAsyncReadyCallback callback, gpointer user_data) {
    if (!self || !file_path) {
        g_task_report_new_error(NULL, callback, user_data, kraken_llm_summarize_async, G_IO_ERROR, G_IO_ERROR_INVALID_ARGUMENT, "Invalid arguments");
        return;
    }
    GTask* task = g_task_new(NULL, NULL, callback, user_data);
    LLMTask* data = g_new0(LLMTask, 1);
    
    char* content = NULL;
    if (g_file_get_contents(file_path, &content, NULL, NULL)) {
        data->content = content;
    } else {
        data->content = g_strdup("Empty or unreadable file.");
    }
    
    data->file_path = g_strdup(file_path);
    data->api_url = g_strdup(self->api_base_url);
    g_task_set_task_data(task, data, (GDestroyNotify)llm_task_free);
    g_task_run_in_thread(task, (GTaskThreadFunc)on_llm_thread);
    g_object_unref(task);
}

char* kraken_llm_summarize_finish(kraken_llm_t* self, GAsyncResult* res, GError** error) {
    return g_task_propagate_pointer(G_TASK(res), error);
}

static void on_embed_thread(GTask *task, gpointer source_object, gpointer task_data, GCancellable *cancellable) {
    LLMTask* data = (LLMTask*)task_data;
    CURL *curl;
    CURLcode res;
    GString* response_string = g_string_new("");
    
    if (!data || !data->api_url) {
        g_task_return_new_error(task, G_IO_ERROR, G_IO_ERROR_FAILED, "Invalid task data");
        g_string_free(response_string, TRUE);
        return;
    }

    curl = curl_easy_init();
    if (curl) {
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        
        JsonBuilder* builder = json_builder_new();
        json_builder_begin_object(builder);
        json_builder_set_member_name(builder, "content");
        json_builder_add_string_value(builder, data->content ? data->content : "");
        json_builder_end_object(builder);
        
        JsonGenerator* gen = json_generator_new();
        JsonNode* root = json_builder_get_root(builder);
        json_generator_set_root(gen, root);
        char* post_url = g_strdup_printf("http://localhost:8081/embedding");
        char* post_data = json_generator_to_data(gen, NULL);
        
        curl_easy_setopt(curl, CURLOPT_URL, post_url);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_data);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, response_string);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);
        
        res = curl_easy_perform(curl);
        if (res == CURLE_OK) {
            JsonParser* parser = json_parser_new();
            if (json_parser_load_from_data(parser, response_string->str, -1, NULL)) {
                JsonNode* root_node = json_parser_get_root(parser);
                JsonObject* obj = NULL;
                
                if (root_node && JSON_NODE_HOLDS_ARRAY(root_node)) {
                    JsonArray* root_arr = json_node_get_array(root_node);
                    if (json_array_get_length(root_arr) > 0) {
                        JsonNode* first = json_array_get_element(root_arr, 0);
                        if (first && JSON_NODE_HOLDS_OBJECT(first)) {
                            obj = json_node_get_object(first);
                        }
                    }
                } else if (root_node && JSON_NODE_HOLDS_OBJECT(root_node)) {
                    obj = json_node_get_object(root_node);
                }
                
                if (obj && json_object_has_member(obj, "embedding")) {
                    JsonArray* emb_arr = json_object_get_array_member(obj, "embedding");
                    if (json_array_get_length(emb_arr) > 0) {
                        JsonNode* first_emb = json_array_get_element(emb_arr, 0);
                        if (first_emb && JSON_NODE_HOLDS_ARRAY(first_emb)) {
                            emb_arr = json_node_get_array(first_emb);
                        }
                    }

                    guint len = json_array_get_length(emb_arr);
                    float* vector = g_new0(float, len);
                    for (guint i = 0; i < len; i++) {
                        vector[i] = (float)json_array_get_double_element(emb_arr, i);
                    }
                    g_task_return_pointer(task, vector, g_free);
                } else {
                    g_task_return_new_error(task, G_IO_ERROR, G_IO_ERROR_FAILED, "No embedding member in response");
                }
            } else {
                g_task_return_new_error(task, G_IO_ERROR, G_IO_ERROR_FAILED, "Parse failed");
            }
            g_object_unref(parser);
        } else {
            g_task_return_new_error(task, G_IO_ERROR, G_IO_ERROR_FAILED, "Curl failed: %s", curl_easy_strerror(res));
        }
        
        g_free(post_data);
        g_free(post_url);
        g_object_unref(gen);
        g_object_unref(builder);
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
    }
    g_string_free(response_string, TRUE);
}

void kraken_llm_embed_async(kraken_llm_t* self, const char* file_path, const char* text, GAsyncReadyCallback callback, gpointer user_data) {
    if (!self) {
        g_task_report_new_error(NULL, callback, user_data, kraken_llm_embed_async, G_IO_ERROR, G_IO_ERROR_INVALID_ARGUMENT, "Invalid self");
        return;
    }
    GTask* task = g_task_new(NULL, NULL, callback, user_data);
    LLMTask* data = g_new0(LLMTask, 1);
    data->file_path = g_strdup(file_path);
    data->content = g_strdup(text ? text : "");
    data->api_url = g_strdup(self->api_base_url);
    g_task_set_task_data(task, data, (GDestroyNotify)llm_task_free);
    g_task_run_in_thread(task, (GTaskThreadFunc)on_embed_thread);
    g_object_unref(task);
}

float* kraken_llm_embed_finish(kraken_llm_t* self, GAsyncResult* res, int* out_dim, GError** error) {
    float* vector = g_task_propagate_pointer(G_TASK(res), error);
    if (vector && out_dim) *out_dim = 2048; 
    return vector;
}

static void on_tag_thread(GTask *task, gpointer source_object, gpointer task_data, GCancellable *cancellable) {
    LLMTask* data = (LLMTask*)task_data;
    CURL *curl;
    CURLcode res;
    GString* response_string = g_string_new("");
    
    if (!data || !data->api_url) {
        g_task_return_new_error(task, G_IO_ERROR, G_IO_ERROR_FAILED, "Invalid task data");
        g_string_free(response_string, TRUE);
        return;
    }

    curl = curl_easy_init();
    if (curl) {
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        
        JsonBuilder* builder = json_builder_new();
        json_builder_begin_object(builder);
        json_builder_set_member_name(builder, "prompt");
        
        char* full_prompt = g_strdup_printf("<|im_start|>system\nYou are a metadata extractor. You respond ONLY with a comma-separated list of 3-5 keywords. No sentences. No introductions. No apologies.\n<|im_end|>\n<|im_start|>user\nKeywords for: %s\n<|im_end|>\n<|im_start|>assistant\n", data->content ? data->content : "");
        json_builder_add_string_value(builder, full_prompt);
        g_free(full_prompt);
        
        json_builder_set_member_name(builder, "n_predict");
        json_builder_add_int_value(builder, 64);
        json_builder_set_member_name(builder, "stream");
        json_builder_add_boolean_value(builder, FALSE);
        json_builder_set_member_name(builder, "stop");
        json_builder_begin_array(builder);
        json_builder_add_string_value(builder, "<|");
        json_builder_add_string_value(builder, "\n");
        json_builder_end_array(builder);
        json_builder_end_object(builder);
        
        JsonGenerator* gen = json_generator_new();
        JsonNode* root = json_builder_get_root(builder);
        json_generator_set_root(gen, root);
        char* post_data = json_generator_to_data(gen, NULL);
        
        curl_easy_setopt(curl, CURLOPT_URL, data->api_url);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_data);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, response_string);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);
        
        res = curl_easy_perform(curl);
        if (res == CURLE_OK) {
            JsonParser* parser = json_parser_new();
            if (json_parser_load_from_data(parser, response_string->str, -1, NULL)) {
                JsonNode* root_node = json_parser_get_root(parser);
                if (root_node && JSON_NODE_HOLDS_OBJECT(root_node)) {
                    JsonObject* obj = json_node_get_object(root_node);
                    if (json_object_has_member(obj, "content")) {
                        data->result = g_strdup(json_object_get_string_member(obj, "content"));
                        g_task_return_pointer(task, g_strdup(data->result), g_free);
                    } else {
                        g_task_return_new_error(task, G_IO_ERROR, G_IO_ERROR_FAILED, "Invalid server response");
                    }
                } else {
                     g_task_return_new_error(task, G_IO_ERROR, G_IO_ERROR_FAILED, "Invalid JSON root");
                }
            } else {
                g_task_return_new_error(task, G_IO_ERROR, G_IO_ERROR_FAILED, "Failed to parse LLM response");
            }
            g_object_unref(parser);
        } else {
            g_task_return_new_error(task, G_IO_ERROR, G_IO_ERROR_FAILED, "Curl failed: %s", curl_easy_strerror(res));
        }
        
        g_free(post_data);
        g_object_unref(gen);
        g_object_unref(builder);
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
    }
    g_string_free(response_string, TRUE);
}

void kraken_llm_extract_tags_async(kraken_llm_t* self, const char* file_path, const char* text, GAsyncReadyCallback callback, gpointer user_data) {
    if (!self) {
        g_task_report_new_error(NULL, callback, user_data, kraken_llm_extract_tags_async, G_IO_ERROR, G_IO_ERROR_INVALID_ARGUMENT, "Invalid self");
        return;
    }
    GTask* task = g_task_new(NULL, NULL, callback, user_data);
    LLMTask* data = g_new0(LLMTask, 1);
    data->file_path = g_strdup(file_path);
    data->content = g_strdup(text ? text : "");
    data->api_url = g_strdup(self->api_base_url);
    g_task_set_task_data(task, data, (GDestroyNotify)llm_task_free);
    g_task_run_in_thread(task, (GTaskThreadFunc)on_tag_thread);
    g_object_unref(task);
}

char** kraken_llm_extract_tags_finish(kraken_llm_t* self, GAsyncResult* res, GError** error) {
    char* result = g_task_propagate_pointer(G_TASK(res), error);
    if (!result) return NULL;
    
    /* Clean up result from possible markdown or garbage */
    char* end_token = strstr(result, "<|");
    if (end_token) *end_token = '\0';
    g_strstrip(result);
    
    char** tags = g_strsplit(result, ",", 0);
    GPtrArray* clean_tags = g_ptr_array_new_with_free_func(g_free);
    
    for (int i = 0; tags[i] != NULL; i++) {
        char* t = g_strstrip(g_strdup(tags[i]));
        /* Filter out common LLM junk and labels */
        if (strlen(t) < 2 || 
            g_str_has_prefix(t, "Technical") || 
            g_str_has_prefix(t, "Topical") ||
            g_str_has_prefix(t, "I'm sorry") ||
            g_str_has_prefix(t, "Sure") ||
            g_str_has_prefix(t, "Here are") ||
            strstr(t, "Summarized") != NULL) {
            g_free(t);
            continue;
        }
        g_ptr_array_add(clean_tags, t);
    }
    g_ptr_array_add(clean_tags, NULL);
    
    char** final_tags = (char**)g_ptr_array_steal(clean_tags, NULL);
    g_ptr_array_unref(clean_tags);
    g_strfreev(tags);
    g_free(result);
    return final_tags;
}
