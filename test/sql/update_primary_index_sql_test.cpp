//===----------------------------------------------------------------------===//
//
//                         Peloton
//
// update_primary_index_sql_test.cpp
//
// Identification: test/sql/update_primary_index_sql_test.cpp
//
// Copyright (c) 2015-16, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include <memory>

#include "catalog/catalog.h"
#include "common/harness.h"
#include "executor/create_executor.h"
#include "planner/create_plan.h"

#include "sql/sql_tests_util.h"

namespace peloton {
namespace test {

class UpdatePrimaryIndexSQLTests : public PelotonTest {};

TEST_F(UpdatePrimaryIndexSQLTests, UpdatePrimaryIndexTest) {
  catalog::Catalog::GetInstance()->CreateDatabase(DEFAULT_DB_NAME, nullptr);

  // Create a table first
  SQLTestsUtil::ExecuteSQLQuery(
      "CREATE TABLE test(a INT PRIMARY KEY, b INT, c INT);");

  // Insert tuples into table
  SQLTestsUtil::ExecuteSQLQuery("INSERT INTO test VALUES (1, 10, 100);");
  SQLTestsUtil::ExecuteSQLQuery("INSERT INTO test VALUES (2, 20, 200);");
  SQLTestsUtil::ExecuteSQLQuery("INSERT INTO test VALUES (3, 30, 300);");

  SQLTestsUtil::ShowTable(DEFAULT_DB_NAME, "test");

  std::vector<ResultType> result;
  std::vector<FieldInfoType> tuple_descriptor;
  std::string error_message;
  int rows_affected;

  // test small int
  SQLTestsUtil::ExecuteSQLQuery("SELECT * from test", result, tuple_descriptor,
                                rows_affected, error_message);
  // Check the return value
  EXPECT_EQ(result[6].second[0], '3');

  // Perform primary key update
  SQLTestsUtil::ExecuteSQLQuery("UPDATE test SET a=2 WHERE c=300", result,
                                tuple_descriptor, rows_affected, error_message);

  // test
  SQLTestsUtil::ExecuteSQLQuery("SELECT * from test", result, tuple_descriptor,
                                rows_affected, error_message);
  // Check the return value, it should not be changed
  EXPECT_EQ(result[6].second[0], '3');

  // Perform another primary key update
  SQLTestsUtil::ExecuteSQLQuery("UPDATE test SET a=5 WHERE c=300", result,
                                tuple_descriptor, rows_affected, error_message);

  // test
  SQLTestsUtil::ExecuteSQLQuery("SELECT * from test", result, tuple_descriptor,
                                rows_affected, error_message);
  // Check the return value, it should not be changed
  EXPECT_EQ(result[6].second[0], '5');

  // Perform normal update
  SQLTestsUtil::ExecuteSQLQuery("UPDATE test SET b=2000 WHERE c=200", result,
                                tuple_descriptor, rows_affected, error_message);

  // test update result
  SQLTestsUtil::ExecuteSQLQuery("SELECT * FROM test WHERE b=2000", result,
                                tuple_descriptor, rows_affected, error_message);
  // Check the return value
  EXPECT_EQ(result[0].second[0], '2');

  // free the database just created
  auto &txn_manager = concurrency::TransactionManagerFactory::GetInstance();
  auto txn = txn_manager.BeginTransaction();
  catalog::Catalog::GetInstance()->DropDatabaseWithName(DEFAULT_DB_NAME, txn);
  txn_manager.CommitTransaction(txn);
}

}  // namespace test
}  // namespace peloton
