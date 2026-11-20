/*
 * Unit tests for building and modifying JSON values in memory
 * (objects, arrays, dot notation) and for comparing values.
 */
#include <criterion/criterion.h>
#include <string.h>
#include <stdio.h>
#include "parson.h"

Test(object, set_and_get) {
    JSON_Value *root = json_value_init_object();
    JSON_Object *o = json_value_get_object(root);
    cr_assert_eq(json_object_set_string(o, "city", "Belgrade"), JSONSuccess);
    cr_assert_eq(json_object_set_number(o, "year", 2026), JSONSuccess);
    cr_assert_eq(json_object_set_boolean(o, "ok", 1), JSONSuccess);
    cr_assert_eq(json_object_set_null(o, "nothing"), JSONSuccess);
    cr_assert_eq(json_object_get_count(o), 4);
    cr_assert_str_eq(json_object_get_string(o, "city"), "Belgrade");
    cr_assert(json_object_has_value_of_type(o, "nothing", JSONNull));
    json_value_free(root);
}

Test(object, overwrite_existing_key) {
    JSON_Value *root = json_value_init_object();
    JSON_Object *o = json_value_get_object(root);
    json_object_set_number(o, "x", 1);
    json_object_set_number(o, "x", 2);
    cr_assert_eq(json_object_get_count(o), 1);
    cr_assert_float_eq(json_object_get_number(o, "x"), 2.0, 1e-9);
    json_value_free(root);
}

Test(object, remove_and_clear) {
    JSON_Value *root = json_parse_string("{\"a\": 1, \"b\": 2, \"c\": 3}");
    JSON_Object *o = json_value_get_object(root);
    cr_assert_eq(json_object_remove(o, "b"), JSONSuccess);
    cr_assert_eq(json_object_get_count(o), 2);
    cr_assert_not(json_object_has_value(o, "b"));
    cr_assert_eq(json_object_remove(o, "missing"), JSONFailure);
    cr_assert_eq(json_object_clear(o), JSONSuccess);
    cr_assert_eq(json_object_get_count(o), 0);
    json_value_free(root);
}

Test(object, missing_key_returns_defaults) {
    JSON_Value *root = json_value_init_object();
    JSON_Object *o = json_value_get_object(root);
    cr_assert_null(json_object_get_string(o, "nope"));
    cr_assert_float_eq(json_object_get_number(o, "nope"), 0.0, 1e-9);
    cr_assert_eq(json_object_get_boolean(o, "nope"), -1);
    json_value_free(root);
}

Test(object, many_keys_force_resize) {
    /* Adding many keys forces the internal hash table to grow. */
    JSON_Value *root = json_value_init_object();
    JSON_Object *o = json_value_get_object(root);
    char key[16];
    for (int i = 0; i < 1000; i++) {
        snprintf(key, sizeof(key), "k%d", i);
        cr_assert_eq(json_object_set_number(o, key, i), JSONSuccess);
    }
    cr_assert_eq(json_object_get_count(o), 1000);
    cr_assert_float_eq(json_object_get_number(o, "k777"), 777.0, 1e-9);
    json_value_free(root);
}

Test(dotnotation, set_and_get_nested) {
    JSON_Value *root = json_value_init_object();
    JSON_Object *o = json_value_get_object(root);
    cr_assert_eq(json_object_dotset_string(o, "user.address.city", "Novi Sad"), JSONSuccess);
    cr_assert_str_eq(json_object_dotget_string(o, "user.address.city"), "Novi Sad");
    cr_assert_not_null(json_object_get_object(o, "user"));
    cr_assert(json_object_dothas_value(o, "user.address"));
    cr_assert_eq(json_object_dotremove(o, "user.address.city"), JSONSuccess);
    cr_assert_not(json_object_dothas_value(o, "user.address.city"));
    json_value_free(root);
}

Test(array, append_replace_remove) {
    JSON_Value *root = json_value_init_array();
    JSON_Array *a = json_value_get_array(root);
    json_array_append_number(a, 1);
    json_array_append_string(a, "two");
    json_array_append_boolean(a, 0);
    json_array_append_null(a);
    cr_assert_eq(json_array_get_count(a), 4);

    cr_assert_eq(json_array_replace_number(a, 1, 2), JSONSuccess);
    cr_assert_float_eq(json_array_get_number(a, 1), 2.0, 1e-9);

    cr_assert_eq(json_array_remove(a, 0), JSONSuccess);
    cr_assert_eq(json_array_get_count(a), 3);
    cr_assert_float_eq(json_array_get_number(a, 0), 2.0, 1e-9);

    cr_assert_eq(json_array_clear(a), JSONSuccess);
    cr_assert_eq(json_array_get_count(a), 0);
    json_value_free(root);
}

Test(array, out_of_bounds) {
    JSON_Value *root = json_parse_string("[1, 2]");
    JSON_Array *a = json_value_get_array(root);
    cr_assert_null(json_array_get_value(a, 5));
    cr_assert_eq(json_array_remove(a, 5), JSONFailure);
    cr_assert_eq(json_array_replace_number(a, 5, 1), JSONFailure);
    json_value_free(root);
}

Test(value, deep_copy_is_equal_and_independent) {
    JSON_Value *orig = json_parse_string("{\"a\": [1, 2, {\"b\": \"c\"}]}");
    JSON_Value *copy = json_value_deep_copy(orig);
    cr_assert(json_value_equals(orig, copy));
    /* changing the copy must not change the original */
    json_object_set_number(json_value_get_object(copy), "a", 5);
    cr_assert_not(json_value_equals(orig, copy));
    json_value_free(orig);
    json_value_free(copy);
}

Test(value, equals_ignores_key_order) {
    JSON_Value *x = json_parse_string("{\"a\": 1, \"b\": 2}");
    JSON_Value *y = json_parse_string("{\"b\": 2, \"a\": 1}");
    cr_assert(json_value_equals(x, y));
    json_value_free(x);
    json_value_free(y);
}
