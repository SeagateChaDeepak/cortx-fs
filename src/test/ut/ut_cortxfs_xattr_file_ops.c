/*
 * Filename: cortxfs_xattr_ops.c
 * Description: Implementation tests for xattr
 *
 * Copyright (c) 2020 Seagate Technology LLC and/or its Affiliates
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU Affero General Public License for more details.
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 * For any questions about this software or licensing,
 * please email opensource@seagate.com or cortx-questions@seagate.com. 
 */

#include "cortxfs_xattr_ops.h"
#ifndef ENABLE_TSDB_ADDB
#define ENABLE_TSDB_ADDB
#endif
#include <perf/perf-counters.h>
#include <pthread.h>

pthread_key_t perfc_tls_key;
uint8_t perfc_tls_key_check = PERFC_INVALID_ID;

/**
 * Setup for xattr test group.
 */
static int xattr_ops_setup(void **state)
{
	int rc = 0;
	struct ut_xattr_env *ut_xattr_obj = NULL;

	ut_xattr_obj = calloc(sizeof(struct ut_xattr_env), 1);

	ut_assert_not_null(ut_xattr_obj);

	ut_xattr_obj->xattr = calloc(sizeof(char *), 8);

	if( ut_xattr_obj->xattr == NULL) {
		ut_assert_true(0);
		rc = -ENOMEM;
	}

	*state = ut_xattr_obj;
	rc = ut_cfs_fs_setup(state);

	ut_assert_int_equal(rc, 0);

	ut_xattr_obj->ut_cfs_obj.file_name = "test_xattr_file";
	ut_xattr_obj->ut_cfs_obj.file_inode = 0LL;

	*state = ut_xattr_obj;
	rc = ut_file_create(state);

	ut_assert_int_equal(rc, 0);

	return rc;
}

/**
 * Teardown for xattr test group
 */
static int xattr_ops_teardown(void **state)
{
	int rc = 0;

	rc = ut_file_delete(state);
	ut_assert_int_equal(rc, 0);

	rc = ut_cfs_fs_teardown(state);
	ut_assert_int_equal(rc, 0);

	free(*state);

	return rc;
}

int main(void)
{
	int rc = 0;
	char *test_log = "/var/log/cortx/test/ut/xattr_file_ops.log";

	/*
	 * uncomment below statement to enable addb traces in this UT
	 * perfc_tsdb_setup();
	 */
	printf("Xattr file Tests\n");

	rc = ut_init(test_log);
	if (rc != 0) {
		printf("ut_init failed, log path=%s, rc=%d.\n", test_log, rc);
		exit(1);
	}

	struct test_case test_list[] = {
		ut_test_case(set_exist_xattr, set_exist_xattr_setup, NULL),
		ut_test_case(set_nonexist_xattr, NULL, NULL),
		ut_test_case(replace_exist_xattr, replace_exist_xattr_setup, NULL),
		ut_test_case(replace_nonexist_xattr, NULL, NULL),
		ut_test_case(get_exist_xattr, get_exist_xattr_setup, NULL),
		ut_test_case(get_nonexist_xattr, NULL, NULL),
		ut_test_case(remove_exist_xattr, remove_exist_xattr_setup, NULL),
		ut_test_case(remove_nonexist_xattr, NULL, NULL),
		ut_test_case(listxattr_test, listxattr_test_setup,
				listxattr_test_teardown),
	};

	int test_count = sizeof(test_list)/sizeof(test_list[0]);
	int test_failed = 0;

	test_failed = ut_run(test_list, test_count, xattr_ops_setup,
				xattr_ops_teardown);

	ut_fini();

	ut_summary(test_count, test_failed);

	return 0;
}
