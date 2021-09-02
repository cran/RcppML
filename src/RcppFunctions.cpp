// This file is part of RcppML, a Rcpp Machine Learning library
//
// Copyright (C) 2021 Zach DeBruine <zacharydebruine@gmail.com>
//
// This source code is subject to the terms of the GNU
// Public License v. 2.0.

// pull in the RcppML library from inst/include
#include <RcppML.hpp>

//' Get the number of threads RcppML should use
//'
//' @description Get the number of threads that will be used by RcppML functions supporting parallelization with OpenMP. Use \code{\link{setRcppMLthreads}} to set the number of threads to be used.
//' 
//' @returns integer giving number of threads to be used by RcppML functions. \code{0} corresponds to all available threads, as determined by OpenMP.
//' @export
//' @seealso \code{\link{setRcppMLthreads}}
//' @author Zach DeBruine
//' @examples
//' \dontrun{
//' # set serial configuration
//' setRcppMLthreads(1)
//' getRcppMLthreads()
//'
//' # restore default parallel configuration, 
//' # letting OpenMP decide how many threads to use
//' setRcppMLthreads(0)
//' getRcppMLthreads()
//' }
//[[Rcpp::export]]
int getRcppMLthreads() {
  Rcpp::Environment env = Rcpp::Environment::global_env();
  Rcpp::List v = env["RcppMLthreads"];
  if (v[0] == R_NilValue) return 0;
  return v[0];
}

//' Set the number of threads RcppML should use
//'
//' @description The number of threads is 0 by default (corresponding to all available threads), but can be set manually using this function. If you clear environment variables or affect the "RcppMLthreads" environment variable specifically, you will need to set your number of preferred threads again.
//' 
//' @details
//' The number of threads set affects OpenMP parallelization only for functions in the RcppML package. It does not affect other R packages that use OpenMP. Parallelization is used for projection of linear factor models with rank > 2, calculation of mean squared error for linear factor models, and for divisive clustering.
//'
//' @param threads number of threads to be used in RcppML functions that are parallelized with OpenMP.
//' @export
//' @seealso \code{\link{getRcppMLthreads}}
//' @author Zach DeBruine
//' @examples
//' \dontrun{
//' # set serial configuration
//' setRcppMLthreads(1)
//' getRcppMLthreads()
//'
//' # restore default parallel configuration, 
//' # letting OpenMP decide how many threads to use
//' setRcppMLthreads(0)
//' getRcppMLthreads()
//' }
//[[Rcpp::export]]
void setRcppMLthreads(int threads) {
  Rcpp::Environment env = Rcpp::Environment::global_env();
  env["RcppMLthreads"] = threads;
}

//' RcppML: Rcpp Machine Learning Library
//'
//' @description
//' High-performance non-negative matrix factorization and linear model projection for sparse matrices, and fast non-negative least squares implementations
//'
//' @import knitr Matrix RcppEigen
//' @importFrom Rcpp evalCpp
//' @importFrom methods as canCoerce
//' @useDynLib RcppML, .registration = TRUE
//' @docType package
//' @name RcppML
//' @author Zach DeBruine
//' @aliases RcppML-package
//' @md
//'

//' @title Project a linear factor model
//'
//' @description Solves the equation \eqn{A = wh} for either \eqn{h} or \eqn{w} given either \eqn{w} or \eqn{h} and \eqn{A}
//'
//' @details
//' For the classical alternating least squares matrix factorization update problem \eqn{A = wh}, the updates 
//' (or projection) of \eqn{h} is given by the equation: \deqn{w^Twh = wA_j} 
//' which is in the form \eqn{ax = b} where \eqn{a = w^Tw} \eqn{x = h} and \eqn{b = wA_j} for all columns \eqn{j} in \eqn{A}.
//'
//' Given \eqn{A}, \code{project} can solve for either \eqn{w} or \eqn{h} given the other:
//' * When given \eqn{A} and \eqn{w}, \eqn{h} is found using a highly efficient parallelization scheme.
//' * When given \eqn{A} and \eqn{h}, \eqn{w} is found without transposition of \eqn{A} (as would be the case in traditional block-pivoting matrix factorization) by accumulating the right-hand sides of linear systems in-place in \eqn{A}, then solving the systems. Note that \eqn{w} may also be found by inputting the transpose of \eqn{A} and \eqn{h} in place of \eqn{w}, (i.e. \code{A = t(A), w = h, h = NULL}). However, for most applications, the cost of a single projection in-place is less than transposition of \eqn{A}. However, for matrix factorization, it is desirable to transpose \eqn{A} if possible because many projections are needed.
//'
//' **Parallelization.** Least squares projections in factorizations of rank-3 and greater are parallelized using the number of threads set by \code{\link{setRcppMLthreads}}. 
//' By default, all available threads are used, see \code{\link{getRcppMLthreads}}. The overhead of parallization is too great for rank-1 and -2 factorization.
//'
//' **L1 Regularization.** Any L1 penalty is subtracted from \eqn{b} and should generally be scaled to \code{max(b)}, where \eqn{b = WA_j} for all columns \eqn{j} in \eqn{A}. An easy way to properly scale an L1 penalty is to normalize all columns in \eqn{w} to sum to 1. No scaling is applied in this function. Such scaling guarantees that \code{L1 = 1} gives a completely sparse solution.
//'
//' **Specializations.** There are specializations for symmetric input matrices, and for rank-1 and rank-2 projections. See documentation for \code{\link{nmf}} for theoretical details and guidance.
//' 
//' **Publication reference.** For theoretical and practical considerations, please see our manuscript: "DeBruine ZJ, Melcher K, Triche TJ (2021) 
//' High-performance non-negative matrix factorization for large single cell data." on BioRXiv.
//'
//' @inheritParams nmf
//' @param w dense matrix of factors x features giving the linear model to be projected (if \code{h = NULL})
//' @param h dense matrix of factors x samples giving the linear model to be projected (if \code{w = NULL})
//' @param L1 L1/LASSO penalty to be applied. No scaling is performed. See details.
//' @returns matrix \eqn{h} or \eqn{w}
//' @references
//' DeBruine, ZJ, Melcher, K, and Triche, TJ. (2021). "High-performance non-negative matrix factorization for large single-cell data." BioRXiv.
//' @author Zach DeBruine
//' @export
//' @seealso \code{\link{nnls}}, \code{\link{nmf}}
//' @md
//' @examples
//' \dontrun{
//' library(Matrix)
//' w <- matrix(runif(1000 * 10), 1000, 10)
//' h_true <- matrix(runif(10 * 100), 10, 100)
//' # A is the crossproduct of "w" and "h" with 10% signal dropout
//' A <- (w %*% h_true) * (rsparsematrix(1000, 100, 0.9) > 0)
//' h <- project(A, w)
//' cor(as.vector(h_true), as.vector(h))
//' 
//' # alternating projections refine solution (like NMF)
//' mse_bad <- mse(A, w, rep(1, ncol(w)), h) # mse before alternating updates
//' h <- project(A, w = w)
//' w <- project(A, h = h)
//' h <- project(A, w)
//' w <- project(A, h = h)
//' h <- project(A, w)
//' w <- t(project(A, h = h))
//' mse_better <- mse(A, w, rep(1, ncol(w)), h) # mse after alternating updates
//' mse_better < mse_bad
//'
//' # two ways to solve for "w" that give the same solution
//' w <- project(A, h = h)
//' w2 <- project(t(A), w = t(h))
//' all.equal(w, w2)
//' }
//[[Rcpp::export]]
Eigen::MatrixXd project(const Rcpp::S4& A, const Rcpp::Nullable<Rcpp::NumericMatrix> w = R_NilValue,
                        const Rcpp::Nullable<Rcpp::NumericMatrix> h = R_NilValue, const bool nonneg = true,
                        const double L1 = 0, const bool mask_zeros = false) {

  if (!A.hasSlot("i") || !A.hasSlot("p") || !A.hasSlot("x") || !A.hasSlot("Dim"))
    Rcpp::stop("'A' was not of class 'dgCMatrix'");

  RcppML::SparseMatrix A_(A);

  if ((w.isNotNull() && h.isNotNull()) || (!w.isNotNull() && !h.isNotNull()))
    Rcpp::stop("specify one of 'w' or 'h', leaving the other 'NULL'");

  if (w.isNotNull()) {
    if (mask_zeros) Rcpp::stop("'mask_zeros = TRUE' is not supported for projections of 'w'. Use 'w <- project(t(A), w = h)' instead.");
    Rcpp::NumericMatrix w_(w);
    Eigen::MatrixXd w__ = Rcpp::as<Eigen::MatrixXd>(w_);
    if (A_.rows() == w__.rows() && A_.rows() != A_.cols()) w__.transposeInPlace();
    if (A_.rows() != w__.cols()) Rcpp::stop("dimensions of 'A' and 'w' are incompatible!");
    Eigen::MatrixXd h_(w__.rows(), A_.cols());
    project(A_, w__, h_, nonneg, L1, getRcppMLthreads(), mask_zeros);
    return h_;
  } else {
    Rcpp::NumericMatrix h_(h);
    Eigen::MatrixXd h__ = Rcpp::as<Eigen::MatrixXd>(h_);
    if (A_.cols() == h__.rows() && A_.rows() != A_.cols()) h__.transposeInPlace();
    if (A_.cols() != h__.cols()) Rcpp::stop("dimensions of 'A' and 'h' are incompatible!");
    Eigen::MatrixXd w_(h__.rows(), A_.rows());
    projectInPlace(A_, h__, w_, nonneg, L1, getRcppMLthreads(), mask_zeros);
    return w_;
  }
}

//' Mean Squared Error loss of a factor model
//'
//' @description MSE of factor models \code{w} and \code{h} given sparse matrix \eqn{A}
//'
//' @details Mean squared error of a matrix factorization of the form \eqn{A = wdh} is given by \deqn{\frac{\sum_{i,j}{(A - wdh)^2}}{ij}} where \eqn{i} and \eqn{j} are the number of rows and columns in \eqn{A}. 
//'
//' Thus, this function simply calculates the cross-product of \eqn{wh} or \eqn{wdh} (if \eqn{d} is specified), 
//' subtracts that from \eqn{A}, squares the result, and calculates the mean of all values.
//' 
//' If no diagonal scaling vector is present in the model, input \code{d = rep(1, k)} where \code{k} is the rank of the model.
//' 
//' **Parallelization.** Calculation of mean squared error is performed in parallel across columns in \code{A} using the number of threads set by \code{\link{setRcppMLthreads}}. 
//' By default, all available threads are used, see \code{\link{getRcppMLthreads}}.
//' 
//' @inheritParams nmf
//' @param w dense matrix of class \code{matrix} with factors (columns) by features (rows)
//' @param d diagonal scaling vector of rank length
//' @param h dense matrix of class \code{matrix} with samples (columns) by factors (rows)
//' @return mean squared error of the factorization model
//' @export
//' @md
//' @author Zach DeBruine
//' @examples
//' \dontrun{
//' library(Matrix)
//' A <- Matrix::rsparsematrix(1000, 1000, 0.1)
//' model <- nmf(A, k = 10, tol = 0.01)
//' c_mse <- mse(A, model$w, model$d, model$h)
//' R_mse <- mean((A - model$w %*% Diagonal(x = model$d) %*% model$h)^2)
//' all.equal(c_mse, R_mse)
//' }
//[[Rcpp::export]]
double mse(const Rcpp::S4& A, Eigen::MatrixXd& w, Eigen::VectorXd& d, Eigen::MatrixXd& h, const bool mask_zeros = false) {

  if (!A.hasSlot("i") || !A.hasSlot("p") || !A.hasSlot("x") || !A.hasSlot("Dim"))
    Rcpp::stop("'A' was not of class 'dgCMatrix'");
  RcppML::SparseMatrix A_(A);
  if (w.rows() == A_.rows()) w.transposeInPlace();
  if (h.rows() == A_.cols()) h.transposeInPlace();
  if (w.rows() != h.rows()) Rcpp::stop("'w' and 'h' are not of equal rank");
  if (w.cols() != A_.rows()) Rcpp::stop("dimensions of 'w' and 'A' are incompatible");
  if (h.cols() != A_.cols()) Rcpp::stop("dimensions of 'h' and 'A' are incompatible");
  if (d.size() != w.rows()) Rcpp::stop("length of 'd' is not equal to rank of 'w' and 'h'");

  RcppML::MatrixFactorization m(w, d, h);
  m.threads = getRcppMLthreads();
  m.mask_zeros = mask_zeros;
  return m.mse(A_);
}

//' @title Bipartition a sample set
//'
//' @description Spectral biparitioning by rank-2 matrix factorization
//'
//' @details
//' Spectral bipartitioning is a popular subroutine in divisive clustering. The sign of the difference between sample loadings in factors of a rank-2 matrix factorization
//' gives a bipartition that is nearly identical to an SVD.
//' 
//' Rank-2 matrix factorization by alternating least squares is faster than rank-2-truncated SVD (i.e. _irlba_).
//'
//' This function is a specialization of rank-2 \code{\link{nmf}} with support for factorization of only a subset of samples, and with additional calculations on the factorization model relevant to bipartitioning. See \code{\link{nmf}} for details regarding rank-2 factorization.
//'
//' @inheritParams nmf
//' @param samples samples to include in bipartition, numbered from 1 to \code{ncol(A)}. Default is \code{NULL} for all samples.
//' @param calc_dist calculate the relative cosine distance of samples within a cluster to either cluster centroid. If \code{TRUE}, centers for clusters will also be calculated.
//' @return
//' A list giving the bipartition and useful statistics:
//' 	\itemize{
//'    \item v       : vector giving difference between sample loadings between factors in a rank-2 factorization
//'    \item dist    : relative cosine distance of samples within a cluster to centroids of assigned vs. not-assigned cluster
//'    \item size1   : number of samples in first cluster (positive loadings in 'v')
//'    \item size2   : number of samples in second cluster (negative loadings in 'v')
//'    \item samples1: indices of samples in first cluster
//'    \item samples2: indices of samples in second cluster
//'    \item center1 : mean feature loadings across samples in first cluster
//'    \item center2 : mean feature loadings across samples in second cluster
//'  }
//'
//' @references
//' 
//' Kuang, D, Park, H. (2013). "Fast rank-2 nonnegative matrix factorization for hierarchical document clustering." Proc. 19th ACM SIGKDD intl. conf. on Knowledge discovery and data mining.
//'
//' @author Zach DeBruine
//' 
//' @export
//' @seealso \code{\link{nmf}}, \code{\link{dclust}}
//' @md
//' @examples
//' \dontrun{
//' library(Matrix)
//' data(iris)
//' A <- as(as.matrix(iris[,1:4]), "dgCMatrix")
//' bipartition(A, calc_dist = TRUE)
//' }
//[[Rcpp::export]]
Rcpp::List bipartition(const Rcpp::S4& A, const double tol = 1e-4, const unsigned int maxit = 100, const bool nonneg = true,
                       Rcpp::Nullable<Rcpp::IntegerVector> samples = R_NilValue, Rcpp::Nullable<Rcpp::IntegerVector> seed = R_NilValue,
                       const bool verbose = false, const bool calc_dist = false, const bool diag = true) {

  if (!A.hasSlot("i") || !A.hasSlot("p") || !A.hasSlot("x") || !A.hasSlot("Dim"))
    Rcpp::stop("'A' was not of class 'dgCMatrix'");
  RcppML::SparseMatrix A_(A);

  // nullable "seed" parameter
  int seed_ = 0;
  if (seed.isNotNull()) {
    Rcpp::IntegerVector s(seed);
    seed_ = s(0);
  }
  Eigen::MatrixXd w = randomMatrix(2, A_.rows(), seed_);

  std::vector<unsigned int> samples__;
  if (samples.isNotNull()) {
    Rcpp::IntegerVector samples_(samples);
    samples__ = Rcpp::as<std::vector<unsigned int>>(samples_);
    for (unsigned int i = 0; i < samples__.size(); ++i) samples__[i] -= 1;
  } else {
    samples__ = std::vector<unsigned int>(A_.cols());
    std::iota(std::begin(samples__), std::end(samples__), 0);
  }

  bipartitionModel m = c_bipartition_sparse(A_, w, samples__, tol, nonneg, calc_dist, maxit, verbose);

  return Rcpp::List::create(Rcpp::Named("v") = m.v, Rcpp::Named("dist") = m.dist, Rcpp::Named("size1") = m.size1,
                            Rcpp::Named("size2") = m.size2, Rcpp::Named("samples1") = m.samples1, Rcpp::Named("samples2") = m.samples2,
                            Rcpp::Named("center1") = m.center1, Rcpp::Named("center2") = m.center2);

}

//' @title Divisive clustering
//'
//' @description Recursive bipartitioning by rank-2 matrix factorization with an efficient modularity-approximate stopping criteria
//'
//' @details
//' Divisive clustering is a sensitive and fast method for sample classification. Samples are recursively partitioned into two groups until a stopping criteria is satisfied and prevents successful partitioning. 
//'
//' See \code{\link{nmf}} and \code{\link{bipartition}} for technical considerations and optimizations relevant to bipartitioning.
//'
//' **Stopping criteria**. Two stopping criteria are used to prevent indefinite division of clusters and tune the clustering resolution to a desirable range:
//' * \code{min_samples}: Minimum number of samples permitted in a cluster
//' * \code{min_dist}: Minimum cosine distance of samples to their cluster center relative to their unassigned cluster center (an approximation of Newman-Girvan modularity)
//'
//' Newman-Girvan modularity (\eqn{Q}) is an interpretable and widely used measure of modularity for a bipartition. However, it requires the calculation of distance between all within-cluster and between-cluster sample pairs. This is computationally intensive, especially for large sample sets. 
//'
//' \code{dclust} uses a measure which linearly approximates Newman-Girvan modularity, and simply requires the calculation of distance between all samples in a cluster and both cluster centers (the assigned and unassigned center), which is orders of magnitude faster to compute. Cosine distance is used instead of Euclidean distance since it handles outliers and sparsity well.
//'
//' A bipartition is rejected if either of the two clusters contains fewer than \code{min_samples} or if the mean relative cosine distance of the bipartition is less than \code{min_dist}. 
//'
//' A bipartition will only be attempted if there are more than \code{2 * min_samples} samples in the cluster, meaning that \code{dist} may not be calculated for some clusters.
//'
//' **Reproducibility.** Because rank-2 NMF is approximate and requires random initialization, results may vary slightly across restarts. Therefore, specify a \code{seed} to guarantee absolute reproducibility.
//' 
//' Other than setting the seed, reproducibility may be improved by setting \code{tol} to a smaller number to increase the exactness of each bipartition.
//'
//' @inheritParams nmf
//' @param min_dist stopping criteria giving the minimum cosine distance of samples within a cluster to the center of their assigned vs. unassigned cluster. If \code{0}, neither this distance nor cluster centroids will be calculated.
//' @param min_samples stopping criteria giving the minimum number of samples permitted in a cluster
//' @param verbose print number of divisions in each generation
//' @param tol in rank-2 NMF, the correlation distance (\eqn{1 - R^2}) between \eqn{w} across consecutive iterations at which to stop factorization
//' @param nonneg in rank-2 NMF, enforce non-negativity
//' @param seed random seed for rank-2 NMF model initialization
//' @return
//' A list of lists corresponding to individual clusters:
//' 	\itemize{
//'    \item id      : character sequence of "0" and "1" giving position of clusters along splitting hierarchy
//'    \item samples : indices of samples in the cluster
//'    \item center  : mean feature expression of all samples in the cluster
//'    \item dist    : if applicable, relative cosine distance of samples in cluster to assigned/unassigned cluster center.
//'    \item leaf    : is cluster a leaf node
//'  }
//'
//' @author Zach DeBruine
//' 
//' @references
//' 
//' Schwartz, G. et al. "TooManyCells identifies and visualizes relationships of single-cell clades". Nature Methods (2020).
//'
//' Newman, MEJ. "Modularity and community structure in networks". PNAS (2006)  
//'
//' Kuang, D, Park, H. (2013). "Fast rank-2 nonnegative matrix factorization for hierarchical document clustering." Proc. 19th ACM SIGKDD intl. conf. on Knowledge discovery and data mining.
//'
//' @export
//' @seealso \code{\link{bipartition}}, \code{\link{nmf}}
//' @md
//' @examples
//' \dontrun{
//' library(Matrix)
//' data(USArrests)
//' A <- as(as.matrix(t(USArrests)), "dgCMatrix")
//' clusters <- dclust(A, min_samples = 2, min_dist = 0.001)
//' str(clusters)
//' }
//[[Rcpp::export]]
Rcpp::List dclust(const Rcpp::S4& A, const unsigned int min_samples, const double min_dist = 0, const bool verbose = true,
                  const double tol = 1e-4, const unsigned int maxit = 100, const bool nonneg = true, Rcpp::Nullable<Rcpp::IntegerVector> seed = R_NilValue) {

  if (!A.hasSlot("i") || !A.hasSlot("p") || !A.hasSlot("x") || !A.hasSlot("Dim"))
    Rcpp::stop("'A' was not of class 'dgCMatrix'");
  RcppML::SparseMatrix A_(A);

  // nullable "seed" parameter
  int seed_ = 0;
  if (seed.isNotNull()) {
    Rcpp::IntegerVector s(seed);
    seed_ = s(0);
  }
  RcppML::clusterModel m = RcppML::clusterModel(A_, min_samples, min_dist);
  m.nonneg = nonneg; m.verbose = verbose; m.tol = tol; m.min_dist = min_dist; m.seed = seed_; m.maxit = maxit; m.threads = getRcppMLthreads();
  m.min_samples = min_samples;

  m.dclust();

  std::vector<cluster> clusters = m.getClusters();

  Rcpp::List result(clusters.size());
  for (unsigned int i = 0; i < clusters.size(); ++i) {
    result[i] = Rcpp::List::create(Rcpp::Named("id") = clusters[i].id, Rcpp::Named("samples") = clusters[i].samples,
                                   Rcpp::Named("center") = clusters[i].center, Rcpp::Named("dist") = clusters[i].dist,
                                   Rcpp::Named("leaf") = clusters[i].leaf);
  }
  return result;
}

//' @title Non-negative matrix factorization
//'
//' @description Sparse matrix factorization of the form \eqn{A = wdh} by alternating least squares with optional non-negativity constraints.
//'
//' @details
//' This fast non-negative matrix factorization (NMF) implementation decomposes a matrix \eqn{A} into lower-rank
//'  non-negative matrices \eqn{w} and \eqn{h}, with factors scaled to sum to 1 via multiplication by a diagonal, \eqn{d}: \deqn{A = wdh}
//'
//' The scaling diagonal enables symmetric factorization, convex L1 regularization, and consistent factor scalings regardless of random initialization.
//' 
//' The factorization model is randomly initialized, and \eqn{w} and \eqn{h} are updated alternately using least squares. 
//' Given \eqn{A} and \eqn{w}, \eqn{h} is updated according to the equation: \deqn{w^Twh = wA_j}
//'
//' This equation is in the form \eqn{ax = b} where \eqn{a = w^Tw}, \eqn{x = h}, and \eqn{b = wA_j} for all columns \eqn{j} in \eqn{A}.
//'
//' The corresponding update for \eqn{w} is \deqn{hh^Tw = hA^T_j}
//'
//' **Stopping criteria.** Alternating least squares projections (see \code{\link{project}} subroutine) are repeated until a stopping criteria is satisfied, which is either a maximum number of 
//' iterations or a tolerance based on the correlation distance between models (\eqn{1 - cor(w_i, w_{i-1})}) across consecutive iterations. Use the \code{tol} parameter to control the stopping criteria for alternating updates:
//' * \code{tol = 1e-2} is appropriate for approximate mean squared error determination and coarse cross-validation, useful for rank determination.
//' * \code{tol = 1e-3} to \code{1e-4} are suitable for rapid expermentation, cross-validation, and preliminary analysis.
//' * \code{tol = 1e-5} and smaller for publication-quality runs
//' * \code{tol = 1e-10} and smaller for robust factorizations at or near machine-precision
//'
//' **Parallelization.** Least squares projections in factorizations of rank-3 and greater are parallelized using the number of threads set by \code{\link{setRcppMLthreads}}. 
//' By default, all available threads are used, see \code{\link{getRcppMLthreads}}.
//' The overhead of parallization is too great to benefit rank-1 and rank-2 factorization.
//'
//' **Specializations.** There are specializations for symmetric matrices, and for rank-1 and rank-2 factorization. 
//' 
//' **L1 regularization**. L1 penalization increases the sparsity of factors, but does not change the information content of the model 
//' or the relative contributions of the leading coefficients in each factor to the model. L1 regularization only slightly increases the error of a model. 
//' L1 penalization should be used to aid interpretability. Penalty values should range from 0 to 1, where 1 gives complete sparsity. In this implementation of NMF, 
//' a scaling diagonal ensures that the L1 penalty is equally applied across all factors regardless of random initialization and the distribution of the model. 
//' Many other implementations of matrix factorization claim to apply L1, but the magnitude of the penalty is at the mercy of the random distribution and 
//' more significantly affects factors with lower overall contribution to the model. L1 regularization of rank-1 and rank-2 factorizations has no effect.
//'
//' **Rank-2 factorization.** When \eqn{k = 2}, a very fast optimized algorithm is used. Two-variable least squares solutions to the problem \eqn{ax = b} are found by direct substitution:
//' \deqn{x_1 = \frac{a_{22}b_1 - a_{12}b_2}{a_{11}a_{22} - a_{12}^2}}
//' \deqn{x_2 = \frac{a_{11}b_2 - a_{12}b_1}{a_{11}a_{22} - a_{12}^2}}
//'
//' In the above equations, the denominator is constant and thus needs to be calculated only once. Additionally, if non-negativity constraints are to be imposed, 
//' if \eqn{x_1 < 0} then \eqn{x_1 = 0} and \eqn{x_2 = \frac{b_1}{a_{11}}}. 
//' Similarly, if \eqn{x_2 < 0} then \eqn{x_2 = 0} and \eqn{x_1 = \frac{b_2}{a_{22}}}.
//'
//' Rank-2 NMF is useful for bipartitioning, and is a subroutine in \code{\link{bipartition}}, where the sign of the difference between sample loadings
//' in both factors gives the partitioning.
//'
//' **Rank-1 factorization.** Rank-1 factorization by alternating least squares gives vectors equivalent to the first singular vectors in an SVD. It is a normalization of the data to a middle point, 
//' and may be useful for ordering samples based on the most significant axis of variation (i.e. pseudotime trajectories). Diagonal scaling guarantees consistent linear 
//' scaling of the factor across random restarts.
//'
//' **Random seed and reproducibility.** Results of a rank-1 and rank-2 factorizations should be reproducible regardless of random seed. For higher-rank models, 
//' results across random restarts should, in theory, be comparable at very high tolerances (i.e. machine precision for _double_, corresponding to about \code{tol = 1e-10}). However, to guarantee
//' reproducibility without such low tolerances, set the \code{seed} argument. Note that \code{set.seed()} will not work. Only random initialization is supported, as other methods 
//' incur unnecessary overhead and sometimes trap updates into local minima.
//'
//' **Rank determination.** This function does not attempt to provide a method for rank determination. Like any clustering algorithm or dimensional reduction,
//' finding the optimal rank can be subjective. An easy way to 
//' estimate rank uses the "elbow method", where the inflection point on a plot of Mean Squared Error loss (MSE) vs. rank 
//' gives a good idea of the rank at which most of the signal has been captured in the model. Unfortunately, this inflection point
//' is not often as obvious for NMF as it is for SVD or PCA.
//' 
//' k-fold cross-validation is a better method. Missing value of imputation has previously been proposed, but is arguably no less subjective 
//' than test-training splits and requires computationally slower factorization updates using missing values, which are not supported here.
//'
//' **Symmetric factorization.** Special optimization for symmetric matrices is automatically applied. Specifically, alternating updates of \code{w} and \code{h} 
//' require transposition of \code{A}, but \code{A == t(A)} when \code{A} is symmetric, thus no up-front transposition is performed.
//'
//' **Zero-masking**. When zeros in a data structure can be regarded as "missing", \code{mask_zeros = TRUE} may be set. However, this requires a slower
//' algorithm, and tolerances will fluctuate more dramatically.
//'
//' **Publication reference.** For theoretical and practical considerations, please see our manuscript: "DeBruine ZJ, Melcher K, Triche TJ (2021) 
//' High-performance non-negative matrix factorization for large single cell data." on BioRXiv.
//'
//' @param A Sparse matrix of features x samples, class "Matrix::dgCMatrix"
//' @param nonneg enforce non-negativity
//' @param k rank
//' @param diag scale factors in \eqn{w} and \eqn{h} to sum to 1 by introducing a diagonal, \eqn{d}. This should generally never be set to \code{FALSE}. Diagonalization enables symmetry of models in factorization of symmetric matrices, convex L1 regularization, and consistent factor scalings.
//' @param update_in_place use a slower algorithm for updates of \eqn{w} which avoids transposition of \eqn{A}
//' @param tol stopping criteria, \eqn{1 - cor(w_i, w_{i-1})}, the correlation distance between \eqn{w} across consecutive iterations
//' @param maxit stopping criteria, maximum number of alternating updates of \eqn{w} and \eqn{h}
//' @param L1 L1/LASSO penalties between 0 and 1, array of length two for \code{c(w, h)}
//' @param seed random seed for model initialization
//' @param verbose print model tolerances between iterations
//' @param mask_zeros handle zeros as missing values
//' @return
//' A list giving the factorization model:
//' 	\itemize{
//'    \item w    : feature factor matrix
//'    \item d    : scaling diagonal vector
//'    \item h    : sample factor matrix
//'    \item tol  : tolerance between models at final update
//'    \item iter : number of alternating updates run
//'  }
//' @references
//' 
//' DeBruine, ZJ, Melcher, K, and Triche, TJ. (2021). "High-performance non-negative matrix factorization for large single-cell data." BioRXiv.
//' 
//' Lin, X, and Boutros, PC (2020). "Optimization and expansion of non-negative matrix factorization." BMC Bioinformatics.
//' 
//' Lee, D, and Seung, HS (1999). "Learning the parts of objects by non-negative matrix factorization." Nature.
//'
//' Franc, VC, Hlavac, VC, Navara, M. (2005). "Sequential Coordinate-Wise Algorithm for the Non-negative Least Squares Problem". Proc. Int'l Conf. Computer Analysis of Images and Patterns. Lecture Notes in Computer Science.
//'
//' @author Zach DeBruine
//'
//' @export
//' @seealso \code{\link{nnls}}, \code{\link{project}}, \code{\link{mse}}
//' @md
//' @examples
//' \dontrun{
//' library(Matrix)
//' # basic NMF
//' model <- nmf(rsparsematrix(1000, 100, 0.1), k = 10)
//' 
//' # compare rank-2 NMF to second left vector in an SVD
//' data(iris)
//' A <- as(as.matrix(iris[,1:4]), "dgCMatrix")
//' nmf_model <- nmf(A, 2, tol = 1e-5)
//' bipartitioning_vector <- apply(nmf_model$w, 1, diff)
//' second_left_svd_vector <- base::svd(A, 2)$u[,2]
//' abs(cor(bipartitioning_vector, second_left_svd_vector))
//' 
//' # compare rank-1 NMF with first singular vector in an SVD
//' abs(cor(nmf(A, 1)$w[,1], base::svd(A, 2)$u[,1]))
//'
//' # symmetric NMF
//' A <- crossprod(rsparsematrix(100, 100, 0.02))
//' model <- nmf(A, 10, tol = 1e-5, maxit = 1000)
//' plot(model$w, t(model$h))
//' # see package vignette for more examples
//' }
//[[Rcpp::export]]
Rcpp::List nmf(const Rcpp::S4& A, const unsigned int k, const double tol = 1e-4, const unsigned int maxit = 100,
               const bool verbose = true, const bool nonneg = true,
               const Rcpp::NumericVector L1 = Rcpp::NumericVector::create(0, 0),
               Rcpp::Nullable<Rcpp::IntegerVector> seed = R_NilValue, const bool update_in_place = false,
               const bool diag = true, const bool mask_zeros = false) {

  if (!A.hasSlot("i") || !A.hasSlot("p") || !A.hasSlot("x") || !A.hasSlot("Dim"))
  Rcpp::stop("'A' was not of class 'dgCMatrix'");
  RcppML::SparseMatrix A_(A);

  if (L1.size() != 2)
    Rcpp::stop("L1 must be a vector of length 2");

  // nullable "seed" parameter
  int seed_ = 0;
  if (seed.isNotNull()) {
    Rcpp::IntegerVector s(seed);
    seed_ = s(0);
  }
  RcppML::MatrixFactorization m(k, A_.rows(), A_.cols(), seed_);

  // set model parameters
  m.tol = tol; m.updateInPlace = update_in_place; m.nonneg = nonneg; m.L1_w = L1(0); m.L1_h = L1(1);
  m.maxit = maxit; m.diag = diag; m.verbose = verbose; m.mask_zeros = mask_zeros;
  m.threads = getRcppMLthreads();

  // fit the model by alternating least squares
  m.fit(A_);

  return Rcpp::List::create(Rcpp::Named("w") = m.matrixW().transpose(), Rcpp::Named("d") = m.vectorD(),
                            Rcpp::Named("h") = m.matrixH(), Rcpp::Named("tol") = m.fit_tol(), Rcpp::Named("iter") = m.fit_iter());
}

//' @title Non-negative least squares
//'
//' @description Solves the equation \code{a %*% x = b} for \code{x} subject to \eqn{x > 0}.
//'
//' @details 
//' This is a very fast implementation of non-negative least squares (NNLS), suitable for very small or very large systems.
//'
//' **Algorithm**. Sequential coordinate descent (CD) is at the core of this implementation, and requires an initialization of \eqn{x}. There are two supported methods for initialization of \eqn{x}:
//' 1. **Zero-filled initialization** when \code{fast_nnls = FALSE} and \code{cd_maxit > 0}. This is generally very efficient for well-conditioned and small systems.
//' 2. **Approximation with FAST** when \code{fast_nnls = TRUE}. Forward active set tuning (FAST), described below, finds an approximate active set using unconstrained least squares solutions found by Cholesky decomposition and substitution. To use only FAST approximation, set \code{cd_maxit = 0}.
//'
//' \code{a} must be symmetric positive definite if FAST NNLS is used, but this is not checked.
//'
//' See our BioRXiv manuscript (references) for benchmarking against Lawson-Hanson NNLS and for a more technical introduction to these methods.
//' 
//' **Coordinate Descent NNLS**. Least squares by **sequential coordinate descent** is used to ensure the solution returned is exact. This algorithm was 
//' introduced by Franc et al. (2005), and our implementation is a vectorized and optimized rendition of that found in the NNLM R package by Xihui Lin (2020).
//' 
//' **FAST NNLS.** Forward active set tuning (FAST) is an exact or near-exact NNLS approximation initialized by an unconstrained 
//' least squares solution. Negative values in this unconstrained solution are set to zero (the "active set"), and all 
//' other values are added  to a "feasible set". An unconstrained least squares solution is then solved for the 
//' "feasible set", any negative values in the resulting solution are set to zero, and the process is repeated until 
//' the feasible set solution is strictly positive. 
//' 
//' The FAST algorithm has a definite convergence guarantee because the 
//' feasible set will either converge or become smaller with each iteration. The result is generally exact or nearly 
//' exact for small well-conditioned systems (< 50 variables) within 2 iterations and thus sets up coordinate
//' descent for very rapid convergence. The FAST method is similar to the first phase of the so-called "TNT-NN" algorithm (Myre et al., 2017), 
//' but the latter half of that method relies heavily on heuristics to refine the approximate active set, which we avoid by using 
//' coordinate descent instead.
//' 
//' @param a symmetric positive definite matrix giving coefficients of the linear system
//' @param b matrix giving the right-hand side(s) of the linear system
//' @param L1 L1/LASSO penalty to be subtracted from \code{b}
//' @param fast_nnls initialize coordinate descent with a FAST NNLS approximation
//' @param cd_maxit maximum number of coordinate descent iterations
//' @param cd_tol stopping criteria, difference in \eqn{x} across consecutive solutions over the sum of \eqn{x}
//' @return vector or matrix giving solution for \code{x}
//' @export
//' @author Zach DeBruine
//' @seealso \code{\link{nmf}}, \code{\link{project}}
//' @md
//' 
//' @references
//' 
//' DeBruine, ZJ, Melcher, K, and Triche, TJ. (2021). "High-performance non-negative matrix factorization for large single-cell data." BioRXiv.
//' 
//' Franc, VC, Hlavac, VC, and Navara, M. (2005). "Sequential Coordinate-Wise Algorithm for the Non-negative Least Squares Problem. Proc. Int'l Conf. Computer Analysis of Images and Patterns."
//'
//' Lin, X, and Boutros, PC (2020). "Optimization and expansion of non-negative matrix factorization." BMC Bioinformatics.
//' 
//' Myre, JM, Frahm, E, Lilja DJ, and Saar, MO. (2017) "TNT-NN: A Fast Active Set Method for Solving Large Non-Negative Least Squares Problems". Proc. Computer Science.
//'
//' @examples 
//' \dontrun{
//' # compare solution to base::solve for a random system
//' X <- matrix(runif(100), 10, 10)
//' a <- crossprod(X)
//' b <- crossprod(X, runif(10))
//' unconstrained_soln <- solve(a, b)
//' nonneg_soln <- nnls(a, b)
//' unconstrained_err <- mean((a %*% unconstrained_soln - b)^2)
//' nonnegative_err <- mean((a %*% nonneg_soln - b)^2)
//' unconstrained_err
//' nonnegative_err
//' all.equal(solve(a, b), nnls(a, b))
//' 
//' # example adapted from multiway::fnnls example 1
//' X <- matrix(1:100,50,2)
//' y <- matrix(101:150,50,1)
//' beta <- solve(crossprod(X)) %*% crossprod(X, y)
//' beta
//' beta <- nnls(crossprod(X), crossprod(X, y))
//' beta
//' }
//[[Rcpp::export]]
Eigen::MatrixXd nnls(const Eigen::MatrixXd& a, Eigen::MatrixXd b, unsigned int cd_maxit = 100, const double cd_tol = 1e-8, const bool fast_nnls = false, const double L1 = 0) {

  if (a.rows() != a.cols()) Rcpp::stop("'a' is not symmetric");
  if (a.rows() != b.rows()) Rcpp::stop("dimensions of 'b' and 'a' are not compatible!");
  if (L1 != 0) b.array() -= L1;

  Eigen::LLT<Eigen::MatrixXd> a_llt;
  Eigen::MatrixXd x(b.rows(), b.cols());
  if (fast_nnls) a_llt = a.llt();
  for (unsigned int col = 0; col < b.cols(); ++col) {

    if (fast_nnls) {
      // initialize with unconstrained least squares solution
      x.col(col) = a_llt.solve(b.col(col));
      // iterative feasible set reduction while unconstrained least squares solutions at feasible indices contain negative values
      while ((x.col(col).array() < 0).any()) {
        Eigen::VectorXi gtz_ind = find_gtz(x, col); // get indices in "x" greater than zero (the "feasible set")
        Eigen::VectorXd bsub = subvec(b, gtz_ind, col); // subset "a" and "b" to those indices in the feasible set
        Eigen::MatrixXd asub = submat(a, gtz_ind, gtz_ind);
        Eigen::VectorXd xsub = asub.llt().solve(bsub); // solve for those indices in "x"
        x.setZero();
        for (unsigned int i = 0; i < gtz_ind.size(); ++i) x(gtz_ind(i), col) = xsub(i);
      }
      b.col(col) -= a * x.col(col); // adjust gradient for current solution
    }

    // refine FAST solution by coordinate descent, or find solution from zero-initialized "x" matrix
    if (cd_maxit > 0) {
      double tol = 1;
      for (unsigned int it = 0; it < cd_maxit && (tol / b.rows()) > cd_tol; ++it) {
        tol = 0;
        for (unsigned int i = 0; i < b.rows(); ++i) {
          double diff = b(i, col) / a(i, i);
          if (-diff > x(i, col)) {
            if (x(i, col) != 0) {
              b.col(col) -= a.col(i) * -x(i, col);
              tol = 1;
              x(i, col) = 0;
            }
          } else if (diff != 0) {
            x(i, col) += diff;
            b.col(col) -= a.col(i) * diff;
            tol += std::abs(diff / (x(i, col) + TINY_NUM));
          }
        }
      }
    }
  }
  return x;
}
