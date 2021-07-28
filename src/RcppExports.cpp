// Generated by using Rcpp::compileAttributes() -> do not edit by hand
// Generator token: 10BE3573-1514-4C36-9D1C-5A225CD40393

#include <RcppEigen.h>
#include <Rcpp.h>

using namespace Rcpp;

// Rcpp_mse
double Rcpp_mse(const Rcpp::S4& A_S4, Eigen::MatrixXd w, const Eigen::VectorXd& d, const Eigen::MatrixXd& h, const unsigned int threads);
RcppExport SEXP _RcppML_Rcpp_mse(SEXP A_S4SEXP, SEXP wSEXP, SEXP dSEXP, SEXP hSEXP, SEXP threadsSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< const Rcpp::S4& >::type A_S4(A_S4SEXP);
    Rcpp::traits::input_parameter< Eigen::MatrixXd >::type w(wSEXP);
    Rcpp::traits::input_parameter< const Eigen::VectorXd& >::type d(dSEXP);
    Rcpp::traits::input_parameter< const Eigen::MatrixXd& >::type h(hSEXP);
    Rcpp::traits::input_parameter< const unsigned int >::type threads(threadsSEXP);
    rcpp_result_gen = Rcpp::wrap(Rcpp_mse(A_S4, w, d, h, threads));
    return rcpp_result_gen;
END_RCPP
}
// Rcpp_nmf
Rcpp::List Rcpp_nmf(const Rcpp::S4& A_S4, const Rcpp::S4& At_S4, Eigen::MatrixXd w, const double tol, const bool nonneg, const double L1_w, const double L1_h, const unsigned int maxit, const bool diag, const unsigned int fast_maxit, const unsigned int cd_maxit, const double cd_tol, const bool verbose, const unsigned int threads);
RcppExport SEXP _RcppML_Rcpp_nmf(SEXP A_S4SEXP, SEXP At_S4SEXP, SEXP wSEXP, SEXP tolSEXP, SEXP nonnegSEXP, SEXP L1_wSEXP, SEXP L1_hSEXP, SEXP maxitSEXP, SEXP diagSEXP, SEXP fast_maxitSEXP, SEXP cd_maxitSEXP, SEXP cd_tolSEXP, SEXP verboseSEXP, SEXP threadsSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< const Rcpp::S4& >::type A_S4(A_S4SEXP);
    Rcpp::traits::input_parameter< const Rcpp::S4& >::type At_S4(At_S4SEXP);
    Rcpp::traits::input_parameter< Eigen::MatrixXd >::type w(wSEXP);
    Rcpp::traits::input_parameter< const double >::type tol(tolSEXP);
    Rcpp::traits::input_parameter< const bool >::type nonneg(nonnegSEXP);
    Rcpp::traits::input_parameter< const double >::type L1_w(L1_wSEXP);
    Rcpp::traits::input_parameter< const double >::type L1_h(L1_hSEXP);
    Rcpp::traits::input_parameter< const unsigned int >::type maxit(maxitSEXP);
    Rcpp::traits::input_parameter< const bool >::type diag(diagSEXP);
    Rcpp::traits::input_parameter< const unsigned int >::type fast_maxit(fast_maxitSEXP);
    Rcpp::traits::input_parameter< const unsigned int >::type cd_maxit(cd_maxitSEXP);
    Rcpp::traits::input_parameter< const double >::type cd_tol(cd_tolSEXP);
    Rcpp::traits::input_parameter< const bool >::type verbose(verboseSEXP);
    Rcpp::traits::input_parameter< const unsigned int >::type threads(threadsSEXP);
    rcpp_result_gen = Rcpp::wrap(Rcpp_nmf(A_S4, At_S4, w, tol, nonneg, L1_w, L1_h, maxit, diag, fast_maxit, cd_maxit, cd_tol, verbose, threads));
    return rcpp_result_gen;
END_RCPP
}
// Rcpp_nnls
Eigen::MatrixXd Rcpp_nnls(const Eigen::MatrixXd& a, Eigen::MatrixXd b, const unsigned int fast_maxit, const unsigned int cd_maxit, const double cd_tol, const bool nonneg);
RcppExport SEXP _RcppML_Rcpp_nnls(SEXP aSEXP, SEXP bSEXP, SEXP fast_maxitSEXP, SEXP cd_maxitSEXP, SEXP cd_tolSEXP, SEXP nonnegSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< const Eigen::MatrixXd& >::type a(aSEXP);
    Rcpp::traits::input_parameter< Eigen::MatrixXd >::type b(bSEXP);
    Rcpp::traits::input_parameter< const unsigned int >::type fast_maxit(fast_maxitSEXP);
    Rcpp::traits::input_parameter< const unsigned int >::type cd_maxit(cd_maxitSEXP);
    Rcpp::traits::input_parameter< const double >::type cd_tol(cd_tolSEXP);
    Rcpp::traits::input_parameter< const bool >::type nonneg(nonnegSEXP);
    rcpp_result_gen = Rcpp::wrap(Rcpp_nnls(a, b, fast_maxit, cd_maxit, cd_tol, nonneg));
    return rcpp_result_gen;
END_RCPP
}
// Rcpp_cdnnls
Eigen::MatrixXd Rcpp_cdnnls(const Eigen::MatrixXd& a, Eigen::MatrixXd& b, Eigen::MatrixXd x, const unsigned int cd_maxit, const double cd_tol, const bool nonneg);
RcppExport SEXP _RcppML_Rcpp_cdnnls(SEXP aSEXP, SEXP bSEXP, SEXP xSEXP, SEXP cd_maxitSEXP, SEXP cd_tolSEXP, SEXP nonnegSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< const Eigen::MatrixXd& >::type a(aSEXP);
    Rcpp::traits::input_parameter< Eigen::MatrixXd& >::type b(bSEXP);
    Rcpp::traits::input_parameter< Eigen::MatrixXd >::type x(xSEXP);
    Rcpp::traits::input_parameter< const unsigned int >::type cd_maxit(cd_maxitSEXP);
    Rcpp::traits::input_parameter< const double >::type cd_tol(cd_tolSEXP);
    Rcpp::traits::input_parameter< const bool >::type nonneg(nonnegSEXP);
    rcpp_result_gen = Rcpp::wrap(Rcpp_cdnnls(a, b, x, cd_maxit, cd_tol, nonneg));
    return rcpp_result_gen;
END_RCPP
}
// Rcpp_project
Eigen::MatrixXd Rcpp_project(const Rcpp::S4& A_S4, Eigen::MatrixXd& w, const bool nonneg, const unsigned int fast_maxit, const unsigned int cd_maxit, const double cd_tol, const double L1, const unsigned int threads);
RcppExport SEXP _RcppML_Rcpp_project(SEXP A_S4SEXP, SEXP wSEXP, SEXP nonnegSEXP, SEXP fast_maxitSEXP, SEXP cd_maxitSEXP, SEXP cd_tolSEXP, SEXP L1SEXP, SEXP threadsSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< const Rcpp::S4& >::type A_S4(A_S4SEXP);
    Rcpp::traits::input_parameter< Eigen::MatrixXd& >::type w(wSEXP);
    Rcpp::traits::input_parameter< const bool >::type nonneg(nonnegSEXP);
    Rcpp::traits::input_parameter< const unsigned int >::type fast_maxit(fast_maxitSEXP);
    Rcpp::traits::input_parameter< const unsigned int >::type cd_maxit(cd_maxitSEXP);
    Rcpp::traits::input_parameter< const double >::type cd_tol(cd_tolSEXP);
    Rcpp::traits::input_parameter< const double >::type L1(L1SEXP);
    Rcpp::traits::input_parameter< const unsigned int >::type threads(threadsSEXP);
    rcpp_result_gen = Rcpp::wrap(Rcpp_project(A_S4, w, nonneg, fast_maxit, cd_maxit, cd_tol, L1, threads));
    return rcpp_result_gen;
END_RCPP
}

static const R_CallMethodDef CallEntries[] = {
    {"_RcppML_Rcpp_mse", (DL_FUNC) &_RcppML_Rcpp_mse, 5},
    {"_RcppML_Rcpp_nmf", (DL_FUNC) &_RcppML_Rcpp_nmf, 14},
    {"_RcppML_Rcpp_nnls", (DL_FUNC) &_RcppML_Rcpp_nnls, 6},
    {"_RcppML_Rcpp_cdnnls", (DL_FUNC) &_RcppML_Rcpp_cdnnls, 6},
    {"_RcppML_Rcpp_project", (DL_FUNC) &_RcppML_Rcpp_project, 8},
    {NULL, NULL, 0}
};

RcppExport void R_init_RcppML(DllInfo *dll) {
    R_registerRoutines(dll, NULL, CallEntries, NULL, NULL);
    R_useDynamicSymbols(dll, FALSE);
}
