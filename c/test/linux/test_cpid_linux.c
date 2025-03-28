// SPDX-License-Identifier: Apache-2.0

#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>

#include <cpid/cpid_linux.h>
#include <unistd.h>

#define BAD_PID 0xFFFFFFFF

void test_cpid_initialize_finalize(void) {
    cpid_handle_t handle = cpid_initialize();
    CU_ASSERT_PTR_NOT_NULL(handle);
    cpid_finalize(handle);

    cpid_finalize(NULL);
}

void test_cpid_make_uuid(void) {
    pid_t self_pid = getpid();

    cpid_handle_t handle = cpid_initialize();
    CU_ASSERT_PTR_NOT_NULL(handle);

    // happy path
    uuid_t uuid_happy1 = {0};
    uuid_t uuid_happy2 = {0};
    CU_ASSERT_EQUAL(cpid_make_uuid(handle, self_pid, 1, 1, uuid_happy1), 0);
    CU_ASSERT_EQUAL(cpid_make_uuid(handle, self_pid, 1, 1, uuid_happy2), 0);
    // check that two subsequent calls with the same input yield the same output
    CU_ASSERT_EQUAL(memcmp(uuid_happy1, uuid_happy2, sizeof(uuid_t)), 0);
    // check that the uuid is not all zeros
    int uuid_happy1_nonzero = 0;
    for (size_t i = 0; i < sizeof(uuid_t); i++) {
        if (uuid_happy1[i] != 0) {
            uuid_happy1_nonzero = 1;
            break;
        }
    }
    CU_ASSERT_EQUAL(uuid_happy1_nonzero, 1);
 
    // invalid args
    uuid_t uuid_invalid_args = {0};
    CU_ASSERT_EQUAL(cpid_make_uuid(NULL, self_pid, 1, 1, uuid_invalid_args), -1);
    CU_ASSERT_EQUAL(cpid_make_uuid(handle, self_pid, 1, 1, NULL), -1);

    cpid_finalize(handle);
}

void test_cpid_get_uuid(void) {
    pid_t self_pid = getpid();

    cpid_handle_t handle = cpid_initialize();
    CU_ASSERT_PTR_NOT_NULL(handle);

    // happy path
    uuid_t uuid_happy1 = {0};
    uuid_t uuid_happy2 = {0};
    CU_ASSERT_EQUAL(cpid_get_uuid(handle, self_pid, uuid_happy1), 0);
    CU_ASSERT_EQUAL(cpid_get_uuid(handle, self_pid, uuid_happy2), 0);
    // check that two subsequent calls for the same process yield the same output
    // INIT PID is guaranteed to be the same process
    // This test wouldn't hold for any arbitrary pid due to pid reuse
    CU_ASSERT_EQUAL(memcmp(uuid_happy1, uuid_happy2, sizeof(uuid_t)), 0);
    // check that the uuid is not all zeros
    int uuid_happy1_nonzero = 0;
    for (size_t i = 0; i < sizeof(uuid_t); i++) {
        if (uuid_happy1[i] != 0) {
            uuid_happy1_nonzero = 1;
            break;
        }
    }
    CU_ASSERT_EQUAL(uuid_happy1_nonzero, 1);

    // invalid args
    uuid_t uuid_invalid_args = {0};
    CU_ASSERT_EQUAL(cpid_get_uuid(NULL, self_pid, uuid_invalid_args), -1);
    CU_ASSERT_EQUAL(cpid_get_uuid(handle, BAD_PID, uuid_invalid_args), -1);
    CU_ASSERT_EQUAL(cpid_get_uuid(handle, self_pid, NULL), -1);

    cpid_finalize(handle);
}

void test_cpid_get_uuid_string(void) {
    pid_t self_pid = getpid();

    cpid_handle_t handle = cpid_initialize();
    CU_ASSERT_PTR_NOT_NULL(handle);

    // happy path
    uuid_string_t uuid_happy1 = {-1};
    uuid_string_t uuid_happy2 = {-1};
    CU_ASSERT_EQUAL(cpid_get_uuid_string(handle, self_pid, uuid_happy1), 0);
    CU_ASSERT_EQUAL(cpid_get_uuid_string(handle, self_pid, uuid_happy2), 0);
    // check that two subsequent calls for the same process yield the same output
    // LAUNCHD is guaranteed to be the same process
    // This test wouldn't hold for any arbitrary pid due to pid reuse
    CU_ASSERT_EQUAL(memcmp(uuid_happy1, uuid_happy2, sizeof(uuid_string_t)), 0);
    // check that the uuid is not negative ones
    int uuid_happy1_nonnegative = 0;
    for (size_t i = 0; i < sizeof(uuid_string_t) - 1; i++) {
        if (uuid_happy1[i] != (char) -1) {
            uuid_happy1_nonnegative = 1;
            break;
        }
    }
    CU_ASSERT_EQUAL(uuid_happy1_nonnegative, 1);
    // check for null termination
    CU_ASSERT_EQUAL(uuid_happy1[sizeof(uuid_string_t) - 1], '\0');

    // invalid args
    uuid_string_t uuid_string_invalid_args = {0};
    CU_ASSERT_EQUAL(cpid_get_uuid_string(NULL, self_pid, uuid_string_invalid_args), -1);
    CU_ASSERT_EQUAL(cpid_get_uuid_string(handle, self_pid, NULL), -1);

    cpid_finalize(handle);
}

int main(void) {
    CU_initialize_registry();
    CU_pSuite suite = CU_add_suite("CPID Reference Implementation Test Suite", 0, 0);

    CU_add_test(suite, "Test CPID Linux basic initialize and finalize", test_cpid_initialize_finalize);
    CU_add_test(suite, "Test CPID Linux make uuid", test_cpid_make_uuid);
    CU_add_test(suite, "Test CPID Linux get uuid", test_cpid_get_uuid);
    CU_add_test(suite, "Test CPID Linux get uuid string", test_cpid_get_uuid_string);

    CU_basic_run_tests();
    int number_of_failures = CU_get_number_of_failures();
    CU_cleanup_registry();
    return number_of_failures;
}


