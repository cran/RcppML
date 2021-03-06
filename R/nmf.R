#' @title Non-negative matrix factorization
#'
#' @description Sparse matrix factorization of the form \eqn{A = wdh} by alternating least squares with optional non-negativity constraints.
#'
#' @details
#' This fast non-negative matrix factorization (NMF) implementation decomposes a matrix \eqn{A} into lower-rank
#'  non-negative matrices \eqn{w} and \eqn{h}, with factors scaled to sum to 1 via multiplication by a diagonal, \eqn{d}: \deqn{A = wdh}
#'
#' The scaling diagonal enables symmetric factorization, convex L1 regularization, and consistent factor scalings regardless of random initialization.
#' 
#' The factorization model is randomly initialized, and \eqn{w} and \eqn{h} are updated alternately using least squares. 
#' Given \eqn{A} and \eqn{w}, \eqn{h} is updated according to the equation: \deqn{w^Twh = wA_j}
#'
#' This equation is in the form \eqn{ax = b} where \eqn{a = w^Tw}, \eqn{x = h}, and \eqn{b = wA_j} for all columns \eqn{j} in \eqn{A}.
#'
#' The corresponding update for \eqn{w} is \deqn{hh^Tw = hA^T_j}
#'
#' **Stopping criteria.** Alternating least squares projections (see \code{\link{project}} subroutine) are repeated until a stopping criteria is satisfied, which is either a maximum number of 
#' iterations or a tolerance based on the correlation distance between models (\eqn{1 - cor(w_i, w_{i-1})}) across consecutive iterations. Use the \code{tol} parameter to control the stopping criteria for alternating updates:
#' * \code{tol = 1e-2} is appropriate for approximate mean squared error determination and coarse cross-validation, useful for rank determination.
#' * \code{tol = 1e-3} to \code{1e-4} are suitable for rapid expermentation, cross-validation, and preliminary analysis.
#' * \code{tol = 1e-5} and smaller for publication-quality runs
#' * \code{tol = 1e-10} and smaller for robust factorizations at or near machine-precision
#'
#' **Parallelization.** Least squares projections in factorizations of rank-3 and greater are parallelized using the number of threads set by \code{\link{setRcppMLthreads}}. 
#' By default, all available threads are used, see \code{\link{getRcppMLthreads}}.
#' The overhead of parallization is too great to benefit rank-1 and rank-2 factorization.
#'
#' **Specializations.** There are specializations for symmetric matrices, and for rank-1 and rank-2 factorization. 
#' 
#' **L1 regularization**. L1 penalization increases the sparsity of factors, but does not change the information content of the model 
#' or the relative contributions of the leading coefficients in each factor to the model. L1 regularization only slightly increases the error of a model. 
#' L1 penalization should be used to aid interpretability. Penalty values should range from 0 to 1, where 1 gives complete sparsity. In this implementation of NMF, 
#' a scaling diagonal ensures that the L1 penalty is equally applied across all factors regardless of random initialization and the distribution of the model. 
#' Many other implementations of matrix factorization claim to apply L1, but the magnitude of the penalty is at the mercy of the random distribution and 
#' more significantly affects factors with lower overall contribution to the model. L1 regularization of rank-1 and rank-2 factorizations has no effect.
#'
#' **Rank-2 factorization.** When \eqn{k = 2}, a very fast optimized algorithm is used. Two-variable least squares solutions to the problem \eqn{ax = b} are found by direct substitution:
#' \deqn{x_1 = \frac{a_{22}b_1 - a_{12}b_2}{a_{11}a_{22} - a_{12}^2}}
#' \deqn{x_2 = \frac{a_{11}b_2 - a_{12}b_1}{a_{11}a_{22} - a_{12}^2}}
#'
#' In the above equations, the denominator is constant and thus needs to be calculated only once. Additionally, if non-negativity constraints are to be imposed, 
#' if \eqn{x_1 < 0} then \eqn{x_1 = 0} and \eqn{x_2 = \frac{b_1}{a_{11}}}. 
#' Similarly, if \eqn{x_2 < 0} then \eqn{x_2 = 0} and \eqn{x_1 = \frac{b_2}{a_{22}}}.
#'
#' Rank-2 NMF is useful for bipartitioning, and is a subroutine in \code{\link{bipartition}}, where the sign of the difference between sample loadings
#' in both factors gives the partitioning.
#'
#' **Rank-1 factorization.** Rank-1 factorization by alternating least squares gives vectors equivalent to the first singular vectors in an SVD. It is a normalization of the data to a middle point, 
#' and may be useful for ordering samples based on the most significant axis of variation (i.e. pseudotime trajectories). Diagonal scaling guarantees consistent linear 
#' scaling of the factor across random restarts.
#'
#' **Random seed and reproducibility.** Results of a rank-1 and rank-2 factorizations should be reproducible regardless of random seed. For higher-rank models, 
#' results across random restarts should, in theory, be comparable at very high tolerances (i.e. machine precision for _double_, corresponding to about \code{tol = 1e-10}). However, to guarantee
#' reproducibility without such low tolerances, set the \code{seed} argument. Note that \code{set.seed()} will not work. Only random initialization is supported, as other methods 
#' incur unnecessary overhead and sometimes trap updates into local minima.
#'
#' **Rank determination.** This function does not attempt to provide a method for rank determination. Like any clustering algorithm or dimensional reduction,
#' finding the optimal rank can be subjective. An easy way to 
#' estimate rank uses the "elbow method", where the inflection point on a plot of Mean Squared Error loss (MSE) vs. rank 
#' gives a good idea of the rank at which most of the signal has been captured in the model. Unfortunately, this inflection point
#' is not often as obvious for NMF as it is for SVD or PCA.
#' 
#' k-fold cross-validation is a better method. Missing value of imputation has previously been proposed, but is arguably no less subjective 
#' than test-training splits and requires computationally slower factorization updates using missing values, which are not supported here.
#'
#' **Symmetric factorization.** Special optimization for symmetric matrices is automatically applied. Specifically, alternating updates of \code{w} and \code{h} 
#' require transposition of \code{A}, but \code{A == t(A)} when \code{A} is symmetric, thus no up-front transposition is performed.
#'
#' **Zero-masking**. When zeros in a data structure can be regarded as "missing", \code{mask_zeros = TRUE} may be set. However, this requires a slower
#' algorithm, and tolerances will fluctuate more dramatically.
#'
#' **Publication reference.** For theoretical and practical considerations, please see our manuscript: "DeBruine ZJ, Melcher K, Triche TJ (2021) 
#' High-performance non-negative matrix factorization for large single cell data." on BioRXiv.
#'
#' @param A matrix of features-by-samples in dense or sparse format (preferred classes are "matrix" or "Matrix::dgCMatrix", respectively). Prefer sparse storage when more than half of all values are zero.
#' @param nonneg enforce non-negativity
#' @param k rank
#' @param diag scale factors in \eqn{w} and \eqn{h} to sum to 1 by introducing a diagonal, \eqn{d}. This should generally never be set to \code{FALSE}. Diagonalization enables symmetry of models in factorization of symmetric matrices, convex L1 regularization, and consistent factor scalings.
#' @param tol stopping criteria, the correlation distance between \eqn{w} across consecutive iterations, \eqn{1 - cor(w_i, w_{i-1})}
#' @param maxit stopping criteria, maximum number of alternating updates of \eqn{w} and \eqn{h}
#' @param L1 L1/LASSO penalties between 0 and 1, array of length two for \code{c(w, h)}
#' @param seed random seed for model initialization
#' @param verbose print model tolerances between iterations
#' @param mask_zeros handle zeros as missing values, available only when \code{A} is sparse
#' @return
#' A list giving the factorization model:
#' 	\itemize{
#'    \item w    : feature factor matrix
#'    \item d    : scaling diagonal vector
#'    \item h    : sample factor matrix
#'    \item tol  : tolerance between models at final update
#'    \item iter : number of alternating updates run
#'  }
#' @references
#' 
#' DeBruine, ZJ, Melcher, K, and Triche, TJ. (2021). "High-performance non-negative matrix factorization for large single-cell data." BioRXiv.
#' 
#' Lin, X, and Boutros, PC (2020). "Optimization and expansion of non-negative matrix factorization." BMC Bioinformatics.
#' 
#' Lee, D, and Seung, HS (1999). "Learning the parts of objects by non-negative matrix factorization." Nature.
#'
#' Franc, VC, Hlavac, VC, Navara, M. (2005). "Sequential Coordinate-Wise Algorithm for the Non-negative Least Squares Problem". Proc. Int'l Conf. Computer Analysis of Images and Patterns. Lecture Notes in Computer Science.
#'
#' @author Zach DeBruine
#'
#' @export
#' @seealso \code{\link{nnls}}, \code{\link{project}}, \code{\link{mse}}
#' @md
#' @examples
#' \dontrun{
#' library(Matrix)
#' # basic NMF
#' model <- nmf(rsparsematrix(1000, 100, 0.1), k = 10)
#' 
#' # compare rank-2 NMF to second left vector in an SVD
#' data(iris)
#' A <- as(as.matrix(iris[,1:4]), "dgCMatrix")
#' nmf_model <- nmf(A, 2, tol = 1e-5)
#' bipartitioning_vector <- apply(nmf_model$w, 1, diff)
#' second_left_svd_vector <- base::svd(A, 2)$u[,2]
#' abs(cor(bipartitioning_vector, second_left_svd_vector))
#' 
#' # compare rank-1 NMF with first singular vector in an SVD
#' abs(cor(nmf(A, 1)$w[,1], base::svd(A, 2)$u[,1]))
#'
#' # symmetric NMF
#' A <- crossprod(rsparsematrix(100, 100, 0.02))
#' model <- nmf(A, 10, tol = 1e-5, maxit = 1000)
#' plot(model$w, t(model$h))
#' # see package vignette for more examples
#' }
nmf <- function(A, k, tol = 1e-4, maxit = 100, verbose = TRUE, L1 = c(0, 0), seed = NULL, mask_zeros = FALSE, diag = TRUE, nonneg = TRUE) {

  if (!is.numeric(seed)) seed <- 0
  if (length(L1) == 1) L1 <- rep(L1, 2)
  if (max(L1) >= 1 || min(L1) < 0) stop("L1 penalties must be strictly in the range [0,1)")
  threads <- getRcppMLthreads()
  
  if (is(A, "sparseMatrix")) {
    A <- as(A, "dgCMatrix")
    Rcpp_nmf_sparse(A, k, tol, maxit, verbose, nonneg, L1, seed, diag, mask_zeros, threads)
  } else {
    if (canCoerce(A, "matrix")) {
      A <- as.matrix(A)
      if(mask_zeros) stop("mask_zeros = TRUE not supported when 'A' is in dense format")
      Rcpp_nmf_dense(A, k, tol, maxit, verbose, nonneg, L1, seed, diag, mask_zeros, threads)
    } else {
      stop("'A' was not coercible to a matrix")
    }
  }
}