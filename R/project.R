#' @title Project a linear factor model
#'
#' @description Solves the equation \eqn{A = wh} for either \eqn{h} or \eqn{w} given either \eqn{w} or \eqn{h} and \eqn{A}
#'
#' @details
#' For the classical alternating least squares matrix factorization update problem \eqn{A = wh}, the updates 
#' (or projection) of \eqn{h} is given by the equation: \deqn{w^Twh = wA_j} 
#' which is in the form \eqn{ax = b} where \eqn{a = w^Tw} \eqn{x = h} and \eqn{b = wA_j} for all columns \eqn{j} in \eqn{A}.
#'
#' Given \eqn{A}, \code{project} can solve for either \eqn{w} or \eqn{h} given the other:
#' * When given \eqn{A} and \eqn{w}, \eqn{h} is found using a highly efficient parallelization scheme.
#' * When given \eqn{A} and \eqn{h}, \eqn{w} is found without transposition of \eqn{A} (as would be the case in traditional block-pivoting matrix factorization) by accumulating the right-hand sides of linear systems in-place in \eqn{A}, then solving the systems. Note that \eqn{w} may also be found by inputting the transpose of \eqn{A} and \eqn{h} in place of \eqn{w}, (i.e. \code{A = t(A), w = h, h = NULL}). However, for most applications, the cost of a single projection in-place is less than transposition of \eqn{A}. However, for matrix factorization, it is desirable to transpose \eqn{A} if possible because many projections are needed.
#'
#' **Parallelization.** Least squares projections in factorizations of rank-3 and greater are parallelized using the number of threads set by \code{\link{setRcppMLthreads}}. 
#' By default, all available threads are used, see \code{\link{getRcppMLthreads}}. The overhead of parallization is too great for rank-1 and -2 factorization.
#'
#' **L1 Regularization.** Any L1 penalty is subtracted from \eqn{b} and should generally be scaled to \code{max(b)}, where \eqn{b = WA_j} for all columns \eqn{j} in \eqn{A}. An easy way to properly scale an L1 penalty is to normalize all columns in \eqn{w} to sum to 1. No scaling is applied in this function. Such scaling guarantees that \code{L1 = 1} gives a completely sparse solution.
#'
#' **Specializations.** There are specializations for symmetric input matrices, and for rank-1 and rank-2 projections. See documentation for \code{\link{nmf}} for theoretical details and guidance.
#' 
#' **Publication reference.** For theoretical and practical considerations, please see our manuscript: "DeBruine ZJ, Melcher K, Triche TJ (2021) 
#' High-performance non-negative matrix factorization for large single cell data." on BioRXiv.
#'
#' @inheritParams nmf
#' @param w dense matrix of factors x features giving the linear model to be projected (if \code{h = NULL})
#' @param h dense matrix of factors x samples giving the linear model to be projected (if \code{w = NULL})
#' @param L1 L1/LASSO penalty to be applied. No scaling is performed. See details.
#' @returns matrix \eqn{h} or \eqn{w}
#' @references
#' DeBruine, ZJ, Melcher, K, and Triche, TJ. (2021). "High-performance non-negative matrix factorization for large single-cell data." BioRXiv.
#' @author Zach DeBruine
#' @export
#' @seealso \code{\link{nnls}}, \code{\link{nmf}}
#' @md
#' @examples
#' \dontrun{
#' library(Matrix)
#' w <- matrix(runif(1000 * 10), 1000, 10)
#' h_true <- matrix(runif(10 * 100), 10, 100)
#' # A is the crossproduct of "w" and "h" with 10% signal dropout
#' A <- (w %*% h_true) * (rsparsematrix(1000, 100, 0.9) > 0)
#' h <- project(A, w)
#' cor(as.vector(h_true), as.vector(h))
#' 
#' # alternating projections refine solution (like NMF)
#' mse_bad <- mse(A, w, rep(1, ncol(w)), h) # mse before alternating updates
#' h <- project(A, w = w)
#' w <- project(A, h = h)
#' h <- project(A, w)
#' w <- project(A, h = h)
#' h <- project(A, w)
#' w <- t(project(A, h = h))
#' mse_better <- mse(A, w, rep(1, ncol(w)), h) # mse after alternating updates
#' mse_better < mse_bad
#'
#' # two ways to solve for "w" that give the same solution
#' w <- project(A, h = h)
#' w2 <- project(t(A), w = t(h))
#' all.equal(w, w2)
#' }
project <- function(A, w = NULL, h = NULL, nonneg = TRUE, L1 = 0, mask_zeros = FALSE) {

  threads <- getRcppMLthreads()
  if ((is.null(w) && is.null(h)) || (!is.null(w) && !is.null(h)))
    stop("specify one of 'w' or 'h', leaving the other 'NULL'")
  if (!is.null(w) && mask_zeros) stop("'mask_zeros = TRUE' is not supported for projections of 'h'. Use 'w' <- project(t(A), w = h)' instead.")

  # get 'A' in either sparse or dense matrix format
  if (is(A, "sparseMatrix")) {
    A <- as(A, "dgCMatrix")
  } else if (canCoerce(A, "matrix")) {
    A <- as.matrix(A)
  } else stop("'A' was not coercible to a matrix")

  # check that dimensions of w or h are compatible with A
  if (!is.null(w)) {
    if (nrow(A) == nrow(w) && nrow(A) != ncol(A)) w <- t(w)
    if (nrow(A) != ncol(w)) stop("dimensions of 'A' and 'w' are incompatible!")
  } else {
    if (ncol(A) == nrow(h) && nrow(A) != ncol(A)) h <- t(h)
    if (ncol(A) != ncol(h)) stop("dimensions of 'A' and 'h' are incompatible!")
  }

  # select backend based on whether 'A' is dense or sparse
  if (class(A) == "dgCMatrix") {
    if (!is.null(w)) {
      Rcpp_projectW_sparse(A, w, nonneg, L1, threads, mask_zeros)
    } else {
      Rcpp_projectH_sparse(A, h, nonneg, L1, threads, mask_zeros)
    }
  } else {
    if (!is.null(w)) {
      Rcpp_projectW_dense(A, w, nonneg, L1, threads, mask_zeros)
    } else {
      Rcpp_projectH_dense(A, h, nonneg, L1, threads, mask_zeros)
    }
  }
}