#include "../include/records.h"
#include <assert.h>

void test_add_record() {
    Record record = create_record("Test", 123);
    assert(add_record(record) == 0);
}

void test_remove_record() {
    Record record = create_record("Test", 123);
    add_record(record);
    assert(remove_record(record.id) == 0);
}

void test_find_record() {
    Record record = create_record("Test", 123);
    add_record(record);
    Record found = find_record(record.id);
    assert(strcmp(found.name, record.name) == 0);
    assert(found.id == record.id);
}

int main() {
    test_add_record();
    test_remove_record();
    test_find_record();
    return 0;
}
