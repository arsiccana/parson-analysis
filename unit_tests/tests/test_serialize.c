/*
 * Unit tests for serialization (turning in-memory values back into text)
 * and for schema validation.
 */
#include <criterion/criterion.h>
#include <string.h>
#include <stdio.h>
#include "parson.h"

Test(serialize, compact_object) {
    JSON_Value *root = json_value_init_object();
    JSON_Object *o = json_value_get_object(root);
    json_object_set_string(o, "a", "x");
    json_object_set_number(o, "b", 1);
    char *s = json_serialize_to_string(root);
    cr_assert_str_eq(s, "{\"a\":\"x\",\"b\":1}");
    cr_assert_eq(json_serialization_size(root), strlen(s) + 1);
    json_free_serialized_string(s);
    json_value_free(root);
}

Test(serialize, pretty_object) {
    JSON_Value *root = json_parse_string("{\"a\": [1, 2]}");
    char *s = json_serialize_to_string_pretty(root);
    cr_assert_str_eq(s, "{\n    \"a\": [\n        1,\n        2\n    ]\n}");
    json_free_serialized_string(s);
    json_value_free(root);
}

Test(serialize, escapes_special_characters) {
    JSON_Value *v = json_value_init_string("a\"b\\c\n/");
    char *s = json_serialize_to_string(v);
    cr_assert_str_eq(s, "\"a\\\"b\\\\c\\n\\/\"");
    json_free_serialized_string(s);

    json_set_escape_slashes(0);
    s = json_serialize_to_string(v);
    cr_assert_str_eq(s, "\"a\\\"b\\\\c\\n/\"");
    json_free_serialized_string(s);
    json_set_escape_slashes(1); /* restore default */
    json_value_free(v);
}

Test(serialize, roundtrip_parse_serialize_parse) {
    const char *text = "{\"list\":[1,2.5,\"s\",true,false,null],\"obj\":{\"k\":\"v\"}}";
    JSON_Value *first = json_parse_string(text);
    char *s = json_serialize_to_string(first);
    JSON_Value *second = json_parse_string(s);
    cr_assert(json_value_equals(first, second));
    json_free_serialized_string(s);
    json_value_free(first);
    json_value_free(second);
}

Test(serialize, buffer_too_small_fails) {
    JSON_Value *v = json_parse_string("{\"key\": \"value\"}");
    char buf[4];
    cr_assert_eq(json_serialize_to_buffer(v, buf, sizeof(buf)), JSONFailure);
    json_value_free(v);
}

Test(serialize, file_roundtrip) {
    const char *path = "test_output.json";
    JSON_Value *v = json_parse_string("{\"n\": 3}");
    cr_assert_eq(json_serialize_to_file(v, path), JSONSuccess);
    JSON_Value *back = json_parse_file(path);
    cr_assert(json_value_equals(v, back));
    json_value_free(v);
    json_value_free(back);
    remove(path);
}

Test(validate, matching_and_non_matching_schema) {
    JSON_Value *schema = json_parse_string("{\"name\": \"\", \"age\": 0}");
    JSON_Value *good = json_parse_string("{\"name\": \"Ana\", \"age\": 24, \"extra\": true}");
    JSON_Value *bad = json_parse_string("{\"name\": \"Ana\", \"age\": \"24\"}");
    cr_assert_eq(json_validate(schema, good), JSONSuccess);
    cr_assert_eq(json_validate(schema, bad), JSONFailure);
    json_value_free(schema);
    json_value_free(good);
    json_value_free(bad);
}
