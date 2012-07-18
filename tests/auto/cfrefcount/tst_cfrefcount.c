#include <CUnit/CUnit.h>
#include <CUnit/Automated.h>
#include <stdio.h>
#include "cfrefcount.c"

/*
 * Unit tests
 */
void test_CFRefCountInit()
{
    // First try the NULL case
    CFRefCountInit(NULL);
    CFRefCount *refCount = NULL;
    CFRefCountInit(&refCount);
    CU_ASSERT_PTR_NOT_NULL_FATAL(refCount);
    CU_ASSERT_EQUAL(0, refCount->m_userCount);
    CU_ASSERT_PTR_NULL(refCount->m_users);
    CU_ASSERT_PTR_NULL(refCount->m_last);
}

void test_CFRefCountDestroy()
{
    CFRefCount *refCount = NULL;
    CFRefCountInit(&refCount);
    CU_ASSERT_PTR_NOT_NULL_FATAL(refCount);
    CU_ASSERT_EQUAL(0, refCount->m_userCount);
    CU_ASSERT_PTR_NULL(refCount->m_users);
    CU_ASSERT_PTR_NULL(refCount->m_last);
    CFRefCountDestroy(&refCount);
    CU_ASSERT_PTR_NULL(refCount);
    // Try the NULL case
    CFRefCountDestroy(NULL);
}

void test_CFRefCountAttach()
{
    CFRefCount *refCount = NULL;
    CFRefCountInit(&refCount);
    CU_ASSERT_PTR_NOT_NULL_FATAL(refCount);
    CU_ASSERT_EQUAL(0, refCount->m_userCount);
    CU_ASSERT_PTR_NULL(refCount->m_users);
    CU_ASSERT_PTR_NULL(refCount->m_last);
    // Try attaching to a NULL refcount
    int value1 = 0, value2 = 0, value3 = 0;
    CU_ASSERT_EQUAL(-1, CFRefCountAttach(NULL, NULL));
    CU_ASSERT_EQUAL(-1, CFRefCountAttach(NULL, &value1));
    CU_ASSERT_EQUAL(-1, CFRefCountAttach(refCount, NULL));
    // Now do the attach dance
    CU_ASSERT_EQUAL(1, CFRefCountAttach(refCount, &value1));
    CU_ASSERT_EQUAL(1, refCount->m_userCount);
    CU_ASSERT_EQUAL(&value1, refCount->m_last->m_user);
    CU_ASSERT_EQUAL(2, CFRefCountAttach(refCount, &value2));
    CU_ASSERT_EQUAL(2, refCount->m_userCount);
    CU_ASSERT_EQUAL(&value2, refCount->m_last->m_user);
    CU_ASSERT_EQUAL(3, CFRefCountAttach(refCount, &value3));
    CU_ASSERT_EQUAL(3, refCount->m_userCount);
    CU_ASSERT_EQUAL(&value3, refCount->m_last->m_user);
    // Free memory
    CFRefCountDestroy(&refCount);
    CU_ASSERT_PTR_NULL(refCount);
}

void test_CFRefCountDetach()
{
    CFRefCount *refCount = NULL;
    CFRefCountInit(&refCount);
    CU_ASSERT_PTR_NOT_NULL_FATAL(refCount);
    CU_ASSERT_EQUAL(0, refCount->m_userCount);
    CU_ASSERT_PTR_NULL(refCount->m_users);
    CU_ASSERT_PTR_NULL(refCount->m_last);
    // Now do the attach dance
    int value1 = 0, value2 = 0, value3 = 0, value4 = 0x55aa55aa;
    CU_ASSERT_EQUAL(1, CFRefCountAttach(refCount, &value1));
    CU_ASSERT_EQUAL(1, refCount->m_userCount);
    CU_ASSERT_EQUAL(&value1, refCount->m_last->m_user);
    CU_ASSERT_EQUAL(2, CFRefCountAttach(refCount, &value2));
    CU_ASSERT_EQUAL(2, refCount->m_userCount);
    CU_ASSERT_EQUAL(&value2, refCount->m_last->m_user);
    CU_ASSERT_EQUAL(3, CFRefCountAttach(refCount, &value3));
    CU_ASSERT_EQUAL(3, refCount->m_userCount);
    CU_ASSERT_EQUAL(&value3, refCount->m_last->m_user);
    // Try the NULL cases
    CU_ASSERT_EQUAL(-1, CFRefCountDetach(NULL, NULL));
    CU_ASSERT_EQUAL(-1, CFRefCountDetach(refCount, NULL));
    CU_ASSERT_EQUAL(-1, CFRefCountDetach(NULL, &value1));
    // Try to detach something that is not attached
    CU_ASSERT_EQUAL(-1, CFRefCountDetach(refCount, &value4));
    // Do the detach dance
    CU_ASSERT_EQUAL(2, CFRefCountDetach(refCount, &value1));
    CU_ASSERT_EQUAL(2, refCount->m_userCount);
    CU_ASSERT_EQUAL(&value3, refCount->m_last->m_user);
    CU_ASSERT_EQUAL(1, CFRefCountDetach(refCount, &value2));
    CU_ASSERT_EQUAL(1, refCount->m_userCount);
    CU_ASSERT_EQUAL(&value3, refCount->m_last->m_user);
    CU_ASSERT_EQUAL(1, CFRefCountDetach(refCount, &value3));
    CU_ASSERT_EQUAL(1, refCount->m_userCount);
    CU_ASSERT_EQUAL(&value3, refCount->m_last->m_user);

    // Free memory
    CFRefCountDestroy(&refCount);
    CU_ASSERT_PTR_NULL(refCount);
}

void test_CFRefCountUsage()
{
    CFRefCount *refCount = NULL;
    CFRefCountInit(&refCount);
    CU_ASSERT_PTR_NOT_NULL_FATAL(refCount);
    CU_ASSERT_EQUAL(0, refCount->m_userCount);
    CU_ASSERT_PTR_NULL(refCount->m_users);
    CU_ASSERT_PTR_NULL(refCount->m_last);
    // Now do the attach dance
    int value1 = 0, value2 = 0, value3 = 0;
    CU_ASSERT_EQUAL(1, CFRefCountAttach(refCount, &value1));
    CU_ASSERT_EQUAL(1, refCount->m_userCount);
    CU_ASSERT_EQUAL(&value1, refCount->m_last->m_user);
    CU_ASSERT_EQUAL(2, CFRefCountAttach(refCount, &value2));
    CU_ASSERT_EQUAL(2, refCount->m_userCount);
    CU_ASSERT_EQUAL(&value2, refCount->m_last->m_user);
    CU_ASSERT_EQUAL(3, CFRefCountAttach(refCount, &value3));
    CU_ASSERT_EQUAL(3, refCount->m_userCount);
    CU_ASSERT_EQUAL(&value3, refCount->m_last->m_user);
    // Do the detach dance
    CU_ASSERT_EQUAL(2, CFRefCountDetach(refCount, &value2));
    CU_ASSERT_EQUAL(2, refCount->m_userCount);
    CU_ASSERT_EQUAL(&value3, refCount->m_last->m_user);
    // Add the element back
    CU_ASSERT_EQUAL(3, CFRefCountAttach(refCount, &value2));
    CU_ASSERT_EQUAL(3, refCount->m_userCount);
    CU_ASSERT_EQUAL(&value2, refCount->m_last->m_user);
    // Take the first element
    CU_ASSERT_EQUAL(2, CFRefCountDetach(refCount, &value1));
    CU_ASSERT_EQUAL(2, refCount->m_userCount);
    CU_ASSERT_EQUAL(&value2, refCount->m_last->m_user);
    // Add the element back
    CU_ASSERT_EQUAL(3, CFRefCountAttach(refCount, &value1));
    CU_ASSERT_EQUAL(3, refCount->m_userCount);
    CU_ASSERT_EQUAL(&value1, refCount->m_last->m_user);
    // Remove them all
    CU_ASSERT_EQUAL(2, CFRefCountDetach(refCount, &value1));
    CU_ASSERT_EQUAL(2, refCount->m_userCount);
    CU_ASSERT_EQUAL(&value2, refCount->m_last->m_user);
    CU_ASSERT_EQUAL(1, CFRefCountDetach(refCount, &value3));
    CU_ASSERT_EQUAL(1, refCount->m_userCount);
    CU_ASSERT_EQUAL(&value2, refCount->m_last->m_user);


    // Free memory
    CFRefCountDestroy(&refCount);
    CU_ASSERT_PTR_NULL(refCount);
}

/*
 * initialization and cleanup
 */
int initialization()
{
    return 0;
}

int cleanup()
{
    return 0;
}

/*
 * Finally the main function
 */

int main(int argc, char **argv)
{
    CU_pSuite pSuite = NULL;
    if (CUE_SUCCESS != CU_initialize_registry())
        return CU_get_error();

    pSuite = CU_add_suite("CFRefCount", initialization, cleanup);
    if (NULL == pSuite) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    if (NULL == CU_add_test(pSuite, "CFRefCount - Init", test_CFRefCountInit)) {
        CU_cleanup_registry();
        return CU_get_error();
    }
    if (NULL == CU_add_test(pSuite, "CFRefCount - Destroy", test_CFRefCountDestroy)) {
        CU_cleanup_registry();
        return CU_get_error();
    }
    if (NULL == CU_add_test(pSuite, "CFRefCount - Attach", test_CFRefCountAttach)) {
        CU_cleanup_registry();
        return CU_get_error();
    }
    if (NULL == CU_add_test(pSuite, "CFRefCount - Detach", test_CFRefCountDetach)) {
        CU_cleanup_registry();
        return CU_get_error();
    }
    if (NULL == CU_add_test(pSuite, "CFRefCount - Usage patterns", test_CFRefCountUsage)) {
        CU_cleanup_registry();
        return CU_get_error();
    }
    CU_set_output_filename("CFRefCount");
    CU_list_tests_to_file();
    CU_automated_run_tests();

    return 0;
}
