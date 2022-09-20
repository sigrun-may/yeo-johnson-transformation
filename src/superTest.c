/*H***************************************************************************
 * FILENAME : superTest.c
 *
 * DESCRIPTION  :
 *
 *
 * PUBLIC FUNCTIONS :
 *
 *
 * NOTES    :
 *
 *
 * AUTHOR   :       jbrenig           START DATE    : 19 September 2022
 *
 * CHANGES  :
 *
 * DATE     WHO     DETAIL
 *
 *H*/

/*****************************************************************************
 *                               INCLUDES
 *****************************************************************************/
#include "include/testFramework.h"

/*****************************************************************************
 *                                TESTS
 *****************************************************************************/

void test_super_yj(void) {
  test_buildBoundaryBox();

  test_yj1();
  test_yj1f();
  test_yj1U();
  test_yj1Uf();

  test_yj2();
  test_yj2f();
  test_yj2U();
  test_yj2Uf();

  test_yj3();
  test_yj3f();
  test_yj3U();
  test_yj3Uf();

  test_yj4();
  test_yj4f();
  test_yj4U();
  test_yj4Uf();

  test_yjCalculation();
  test_yjCalculationf();
  test_yjCalculationU();
  test_yjCalculationUf();
}

void test_super_vi(void) {
  test_getMatrixSizeFromCsv();
  test_fillMatrixFromCsv();
  test_fillMatrixFromCsvf();
  test_importVectorTableFromCsv();
  test_importVectorTableFromCsvf();
}

void test_super_ls(void) {
  test_lsAverage();
  test_lsAveragef();
  test_lsAverageU();
  test_lsAverageUf();

  test_lsVariance();
  test_lsVariancef();
  test_lsVarianceU();
  test_lsVarianceUf();

  test_lsSkew();
  test_lsSkewf();
  test_lsSkewU();
  test_lsSkewUf();

  test_lsIsCloserToZero();
  test_lsIsCloserToZerof();

  test_lsSkewIntervalStep();
  test_lsSkewIntervalStepf();
  test_lsSkewIntervalStepU();
  test_lsSkewIntervalStepUf();

  test_lsLambdaSearch();
  test_lsLambdaSearchf();
  test_lsLambdaSearchU();
  test_lsLambdaSearchUf();
}
