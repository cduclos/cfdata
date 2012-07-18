#include <CUnit/CUnit.h>
#include <CUnit/Automated.h>
#include <stdio.h>
#include "cfrefcount.c"
#include "cfbytearray.c"

/*
 * Unit tests
 */
void test_CFByteArrayInit()
{
    // First try the NULL case
    CFByteArrayInit(NULL);
    CFByteArray *byteArray = NULL;
    CFByteArrayInit(&byteArray);
    CU_ASSERT_PTR_NOT_NULL_FATAL(byteArray);
    CU_ASSERT_EQUAL(0, byteArray->m_used);
    CU_ASSERT_EQUAL(1024, byteArray->m_length);
    CU_ASSERT_PTR_NOT_NULL(byteArray->m_array);
}

void test_CFByteArrayDestroy()
{
    CFByteArray *byteArray = NULL;
    CFByteArrayInit(&byteArray);
    CU_ASSERT_PTR_NOT_NULL_FATAL(byteArray);
    CU_ASSERT_EQUAL(0, byteArray->m_used);
    CU_ASSERT_EQUAL(1024, byteArray->m_length);
    CU_ASSERT_PTR_NOT_NULL(byteArray->m_array);
    CFByteArrayDestroy(&byteArray);
    CU_ASSERT_PTR_NULL(byteArray);
}

void test_CFByteArrayCopy()
{
    CFByteArray *byteArray1 = NULL;
    CFByteArray *byteArray2 = NULL;
    CFByteArrayInit(&byteArray1);
    CU_ASSERT_PTR_NOT_NULL_FATAL(byteArray1);
    CU_ASSERT_EQUAL(0, byteArray1->m_used);
    CU_ASSERT_EQUAL(1024, byteArray1->m_length);
    CU_ASSERT_PTR_NOT_NULL(byteArray1->m_array);
    // Add some content to the array
    int i = 0;
    for (; i < 16; i++) {
        byteArray1->m_array[i] = 'a' + i;
    }
    byteArray1->m_used = 16;
    // Now copy the array
    CU_ASSERT_EQUAL(0, CFByteArrayCopy(byteArray1, &byteArray2));
    // Check the content
    for (; i < 16; i++) {
        CU_ASSERT_TRUE(byteArray2->m_array[i] == 'a' + i);
    }
    CU_ASSERT_EQUAL(byteArray1->m_array, byteArray2->m_array);
    CU_ASSERT_EQUAL(byteArray1->m_used, byteArray2->m_used);
    CU_ASSERT_EQUAL(byteArray1->m_length, byteArray2->m_length);
    CU_ASSERT_EQUAL(byteArray1->m_refcount, byteArray2->m_refcount);
    CU_ASSERT_EQUAL(2, byteArray1->m_refcount->m_userCount);

    // Finally destroy the arrays
    CFByteArrayDestroy(&byteArray1);
    CFByteArrayDestroy(&byteArray2);
}

void test_CFByteArraySet()
{
    CFByteArray *byteArray1 = NULL;
    CFByteArrayInit(&byteArray1);
    CU_ASSERT_PTR_NOT_NULL_FATAL(byteArray1);
    CU_ASSERT_EQUAL(0, byteArray1->m_used);
    CU_ASSERT_EQUAL(1024, byteArray1->m_length);
    CU_ASSERT_PTR_NOT_NULL(byteArray1->m_array);
    // Set some elements into the array
    char test[] = "this is a test";
    unsigned int length = strlen(test);
    CU_ASSERT_EQUAL(0, CFByteArraySet(byteArray1, (void *)test, length));
    // Check the content
    CU_ASSERT_EQUAL(0, strncmp(byteArray1->m_array, test, length));
    CU_ASSERT_EQUAL(length, byteArray1->m_used);
    // Copy the array
    CFByteArray *byteArray2 = NULL;
    CFByteArrayInit(&byteArray2);
    CU_ASSERT_PTR_NOT_NULL_FATAL(byteArray2);
    CU_ASSERT_EQUAL(0, byteArray2->m_used);
    CU_ASSERT_EQUAL(1024, byteArray2->m_length);
    CU_ASSERT_PTR_NOT_NULL(byteArray2->m_array);
    CU_ASSERT_EQUAL(0, CFByteArrayCopy(byteArray1, &byteArray2));
    // Check that they are equal
    CU_ASSERT_EQUAL(byteArray1->m_array, byteArray2->m_array);
    CU_ASSERT_EQUAL(byteArray1->m_used, byteArray2->m_used);
    CU_ASSERT_EQUAL(byteArray1->m_length, byteArray2->m_length);
    CU_ASSERT_EQUAL(byteArray1->m_refcount, byteArray2->m_refcount);
    CU_ASSERT_EQUAL(2, byteArray1->m_refcount->m_userCount);
    CU_ASSERT_EQUAL(2, byteArray2->m_refcount->m_userCount);

    // Now write some text to the second array and check they detach
    char change[] = "some different text";
    unsigned int differentLength = strlen(change);
    CU_ASSERT_EQUAL(0, CFByteArraySet(byteArray2, (void *)change, differentLength));
    // Check the content
    CU_ASSERT_EQUAL(0, strncmp(byteArray1->m_array, test, length));
    CU_ASSERT_EQUAL(length, byteArray1->m_used);
    CU_ASSERT_EQUAL(0, strncmp(byteArray2->m_array, change, differentLength));
    CU_ASSERT_EQUAL(differentLength, byteArray2->m_used);

    // Check that they are different
    CU_ASSERT_NOT_EQUAL(byteArray1->m_array, byteArray2->m_array);
    CU_ASSERT_NOT_EQUAL(byteArray1->m_used, byteArray2->m_used);
    CU_ASSERT_EQUAL(byteArray1->m_length, byteArray2->m_length);
    CU_ASSERT_NOT_EQUAL(byteArray1->m_refcount, byteArray2->m_refcount);
    CU_ASSERT_EQUAL(1, byteArray1->m_refcount->m_userCount);
    CU_ASSERT_EQUAL(1, byteArray1->m_refcount->m_userCount);

    // Try the negative cases
    CFByteArray *byteArray3 = NULL;
    CFByteArrayInit(&byteArray3);
    CU_ASSERT_PTR_NOT_NULL_FATAL(byteArray3);
    CU_ASSERT_EQUAL(0, byteArray3->m_used);
    CU_ASSERT_EQUAL(1024, byteArray3->m_length);
    CU_ASSERT_PTR_NOT_NULL(byteArray3->m_array);
    CU_ASSERT_EQUAL(-1, CFByteArraySet(NULL, (void *)change, differentLength));
    CU_ASSERT_EQUAL(-1, CFByteArraySet(byteArray3, NULL, differentLength));
    CU_ASSERT_EQUAL(-1, CFByteArraySet(byteArray3, change, 0));
    CU_ASSERT_EQUAL(-1, CFByteArraySet(byteArray3, change, -10));
    // Finally destroy the arrays
    CFByteArrayDestroy(&byteArray1);
    CFByteArrayDestroy(&byteArray2);
    CFByteArrayDestroy(&byteArray3);
}

void test_CFByteArrayEqual()
{
    CFByteArray *byteArray1 = NULL;
    CFByteArrayInit(&byteArray1);
    CU_ASSERT_PTR_NOT_NULL_FATAL(byteArray1);
    CU_ASSERT_EQUAL(0, byteArray1->m_used);
    CU_ASSERT_EQUAL(1024, byteArray1->m_length);
    CU_ASSERT_PTR_NOT_NULL(byteArray1->m_array);
    // Set some elements into the array
    char test[] = "this is a test";
    unsigned int length = strlen(test);
    CU_ASSERT_EQUAL(0, CFByteArraySet(byteArray1, (void *)test, length));
    // Check the content
    CU_ASSERT_EQUAL(0, strncmp(byteArray1->m_array, test, length));
    CU_ASSERT_EQUAL(length, byteArray1->m_used);
    // Copy the array
    CFByteArray *byteArray2 = NULL;
    CFByteArrayInit(&byteArray2);
    CU_ASSERT_PTR_NOT_NULL_FATAL(byteArray2);
    CU_ASSERT_EQUAL(0, byteArray2->m_used);
    CU_ASSERT_EQUAL(1024, byteArray2->m_length);
    CU_ASSERT_PTR_NOT_NULL(byteArray2->m_array);
    CU_ASSERT_EQUAL(0, CFByteArrayCopy(byteArray1, &byteArray2));
    // Check that they are equal
    CU_ASSERT_EQUAL(byteArray1->m_array, byteArray2->m_array);
    CU_ASSERT_EQUAL(byteArray1->m_used, byteArray2->m_used);
    CU_ASSERT_EQUAL(byteArray1->m_length, byteArray2->m_length);
    CU_ASSERT_EQUAL(byteArray1->m_refcount, byteArray2->m_refcount);
    CU_ASSERT_EQUAL(2, byteArray1->m_refcount->m_userCount);
    CU_ASSERT_EQUAL(2, byteArray2->m_refcount->m_userCount);
    CU_ASSERT_TRUE(CFByteArrayEqual(byteArray1, byteArray2));

    // Now write some text to the second array and check they detach
    char change[] = "some different text";
    unsigned int differentLength = strlen(change);
    CU_ASSERT_EQUAL(0, CFByteArraySet(byteArray2, (void *)change, differentLength));
    // Check the content
    CU_ASSERT_EQUAL(0, strncmp(byteArray1->m_array, test, length));
    CU_ASSERT_EQUAL(length, byteArray1->m_used);
    CU_ASSERT_EQUAL(0, strncmp(byteArray2->m_array, change, differentLength));
    CU_ASSERT_EQUAL(differentLength, byteArray2->m_used);

    // Check that they are different
    CU_ASSERT_NOT_EQUAL(byteArray1->m_array, byteArray2->m_array);
    CU_ASSERT_NOT_EQUAL(byteArray1->m_used, byteArray2->m_used);
    CU_ASSERT_EQUAL(byteArray1->m_length, byteArray2->m_length);
    CU_ASSERT_NOT_EQUAL(byteArray1->m_refcount, byteArray2->m_refcount);
    CU_ASSERT_EQUAL(1, byteArray1->m_refcount->m_userCount);
    CU_ASSERT_EQUAL(1, byteArray1->m_refcount->m_userCount);
    CU_ASSERT_FALSE(CFByteArrayEqual(byteArray1, byteArray2));

    // Try the negative cases
    CFByteArray *byteArray3 = NULL;
    CU_ASSERT_FALSE(CFByteArrayEqual(byteArray1, byteArray3));
    CU_ASSERT_FALSE(CFByteArrayEqual(byteArray3, byteArray1));
    CU_ASSERT_FALSE(CFByteArrayEqual(NULL, NULL));
    CFByteArrayInit(&byteArray3);
    CU_ASSERT_PTR_NOT_NULL_FATAL(byteArray3);
    // Check they are different
    CU_ASSERT_FALSE(CFByteArrayEqual(byteArray1, byteArray3));
    // Finally destroy the arrays
    CFByteArrayDestroy(&byteArray1);
    CFByteArrayDestroy(&byteArray2);
    CFByteArrayDestroy(&byteArray3);
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

    pSuite = CU_add_suite("CFByteArray", initialization, cleanup);
    if (NULL == pSuite) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    if (NULL == CU_add_test(pSuite, "CFByteArray - Init", test_CFByteArrayInit)) {
        CU_cleanup_registry();
        return CU_get_error();
    }
    if (NULL == CU_add_test(pSuite, "CFByteArray - Destroy", test_CFByteArrayDestroy)) {
        CU_cleanup_registry();
        return CU_get_error();
    }
    if (NULL == CU_add_test(pSuite, "CFByteArray - Copy", test_CFByteArrayCopy)) {
        CU_cleanup_registry();
        return CU_get_error();
    }
    if (NULL == CU_add_test(pSuite, "CFByteArray - Set", test_CFByteArraySet)) {
        CU_cleanup_registry();
        return CU_get_error();
    }
    if (NULL == CU_add_test(pSuite, "CFByteArray - Equal", test_CFByteArrayEqual)) {
        CU_cleanup_registry();
        return CU_get_error();
    }
    CU_set_output_filename("CFByteArray");
    CU_list_tests_to_file();
    CU_automated_run_tests();

    return 0;
}
