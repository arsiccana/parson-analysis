/*
 * Unit tests for parsing JSON text with parson.
 * Each Test(suite, name) is one independent test case (Criterion framework).
 */
#include <criterion/criterion.h>
#include <string.h>
#include "parson.h"

/* --- Valid input: each JSON type is parsed into the right value type --- */

Test(parse, number) {
    JSON_Value *v = json_parse_string("42.5");
    cr_assert_not_null(v);
    cr_assert_eq(json_value_get_type(v), JSONNumber);
    cr_assert_float_eq(json_value_get_number(v), 42.5, 1e-9);
    json_value_free(v);
}

Test(parse, negative_number_with_exponent) {
    JSON_Value *v = json_parse_string("-1.5e3");
    cr_assert_not_null(v);
    cr_assert_float_eq(json_value_get_number(v), -1500.0, 1e-9);
    json_value_free(v);
}

Test(parse, string) {
    JSON_Value *v = json_parse_string("\"hello\"");
    cr_assert_not_null(v);
    cr_assert_eq(json_value_get_type(v), JSONString);
    cr_assert_str_eq(json_value_get_string(v), "hello");
    json_value_free(v);
}

Test(parse, booleans_and_null) {
    JSON_Value *t = json_parse_string("true");
    JSON_Value *f = json_parse_string("false");
    JSON_Value *n = json_parse_string("null");
    cr_assert_eq(json_value_get_boolean(t), 1);
    cr_assert_eq(json_value_get_boolean(f), 0);
    cr_assert_eq(json_value_get_type(n), JSONNull);
    json_value_free(t);
    json_value_free(f);
    json_value_free(n);
}

Test(parse, object) {
    JSON_Value *v = json_parse_string("{\"name\": \"Ana\", \"age\": 24, \"student\": true}");
    JSON_Object *o = json_value_get_object(v);
    cr_assert_not_null(o);
    cr_assert_eq(json_object_get_count(o), 3);
    cr_assert_str_eq(json_object_get_string(o, "name"), "Ana");
    cr_assert_float_eq(json_object_get_number(o, "age"), 24.0, 1e-9);
    cr_assert_eq(json_object_get_boolean(o, "student"), 1);
    json_value_free(v);
}

Test(parse, array) {
    JSON_Value *v = json_parse_string("[1, \"two\", null, [3]]");
    JSON_Array *a = json_value_get_array(v);
    cr_assert_not_null(a);
    cr_assert_eq(json_array_get_count(a), 4);
    cr_assert_float_eq(json_array_get_number(a, 0), 1.0, 1e-9);
    cr_assert_str_eq(json_array_get_string(a, 1), "two");
    cr_assert_eq(json_value_get_type(json_array_get_value(a, 2)), JSONNull);
    cr_assert_eq(json_array_get_count(json_array_get_array(a, 3)), 1);
    json_value_free(v);
}

Test(parse, string_escapes) {
    JSON_Value *v = json_parse_string("\"line1\\nline2\\t\\\"quoted\\\"\"");
    cr_assert_not_null(v);
    cr_assert_str_eq(json_value_get_string(v), "line1\nline2\t\"quoted\"");
    json_value_free(v);
}

Test(parse, unicode_escape) {
    /* \u0161 is the letter 'š', encoded in UTF-8 as two bytes 0xC5 0xA1 */
    JSON_Value *v = json_parse_string("\"\\u0161\"");
    cr_assert_not_null(v);
    cr_assert_str_eq(json_value_get_string(v), "\xC5\xA1");
    json_value_free(v);
}

Test(parse, with_comments) {
    const char *text = "/* block */ {\"a\": 1 // line comment\n}";
    JSON_Value *v = json_parse_string_with_comments(text);
    cr_assert_not_null(v);
    cr_assert_float_eq(json_object_get_number(json_value_get_object(v), "a"), 1.0, 1e-9);
    json_value_free(v);
}

/* --- Invalid input: parser must return NULL instead of crashing --- */

Test(parse_invalid, malformed_inputs_return_null) {
    const char *bad[] = {
        "",                 /* empty input */
        "{",                /* unclosed object */
        "[1, 2",            /* unclosed array */
        "{\"a\" 1}",        /* missing colon */
        "\"unterminated",   /* unterminated string */
        "tru",              /* truncated literal */
        "01",               /* leading zero is not valid JSON */
        "{'a': 1}",         /* single quotes are not valid JSON */
    };
    for (size_t i = 0; i < sizeof(bad) / sizeof(bad[0]); i++) {
        JSON_Value *v = json_parse_string(bad[i]);
        cr_expect_null(v, "input should be rejected: %s", bad[i]);
        json_value_free(v); /* safe: json_value_free(NULL) does nothing */
    }
}

/*
 * FINDING: RFC 8259 (the JSON standard) does not allow a trailing comma,
 * but parson accepts it. This is intentional (parson's own tests.c expects it),
 * so this test documents the non-standard behaviour instead of failing on it.
 */
Test(parse_nonstandard, accepts_trailing_comma) {
    JSON_Value *obj = json_parse_string("{\"a\": 1,}");
    JSON_Value *arr = json_parse_string("[1, 2,]");
    cr_expect_not_null(obj);
    cr_expect_not_null(arr);
    json_value_free(obj);
    json_value_free(arr);
}

Test(parse_invalid, null_input) {
    cr_assert_null(json_parse_string(NULL));
}

Test(parse_invalid, missing_file) {
    cr_assert_null(json_parse_file("this_file_does_not_exist.json"));
}
