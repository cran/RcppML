test_that("Testing nmf", {
  
  set.seed(123)
  library(Matrix)
  setRcppMLthreads(1)
  # test that matrix factorization converges over time (SPARSE)
  A <- abs(Matrix::rsparsematrix(100, 100, 0.1))
  model_bad <- nmf(A, 5, maxit = 2, tol = 1e-10, v = F)
  model_good <- nmf(A, 5, maxit = 20, tol = 1e-10, v = F)
  model_bad_mse <- mse(A, model_bad$w, model_bad$d, model_bad$h)
  model_good_mse <- mse(A, model_good$w, model_good$d, model_good$h)
  expect_lt(model_good_mse, model_bad_mse)

  # test that non-negativity constraints are applied (SPARSE)
  model <- model_bad
  expect_equal(min(model$w) >= 0, TRUE)
  expect_equal(min(model$h) >= 0, TRUE)
  
  # test that non-negativity constraints are not applied when nonneg = FALSE (SPARSE)
  model <- nmf(A, 5, nonneg = FALSE, v = F)
  expect_equal(min(model$w) < 0, TRUE)
  expect_equal(min(model$h) < 0, TRUE)
  
  # test that diagonalization enforces symmetry (SPARSE)
  sim <- as(crossprod(A), "dgCMatrix")
  model <- nmf(sim, 5, diag = F, seed = 123, tol = 1e-6, v = F)
  model2 <- nmf(sim, 5, diag = T, seed = 123, tol = 1e-6, v = F)
  cor_model <- cor(as.vector(model$w), as.vector(t(model$h)))
  cor_model2 <- cor(as.vector(model2$w), as.vector(t(model2$h)))
  expect_lt(cor_model, cor_model2)
  expect_equal(mean(model$d), 1)

 
  # test that matrix factorization converges over time (SPARSE SYMMETRIC)
  A <- as(A, "dgCMatrix")
  model_bad <- nmf(A, 5, maxit = 2, tol = 1e-10, v = F)
  model_good <- nmf(A, 5, maxit = 20, tol = 1e-10, v = F)
  model_bad_mse <- mse(A, model_bad$w, model_bad$d, model_bad$h)
  model_good_mse <- mse(A, model_good$w, model_good$d, model_good$h)
  expect_lt(model_good_mse, model_bad_mse)
  
  # test that L1 regularization promotes factor sparsity (SPARSE)
  model_with_no_L1 <- nmf(A, 5, maxit = 5, L1 = c(0, 0), seed = 123, v = F)
  model_with_L1 <- nmf(A, 5, maxit = 5, L1 = c(0.1, 0.1), seed = 123, v = F)
  sparsity_with_no_L1 <- sum(model_with_no_L1$w == 0)
  sparsity_with_L1 <- sum(model_with_L1$w == 0)
  expect_lt(sparsity_with_no_L1, sparsity_with_L1)

  # test that setting the random seed gives identical models (SPARSE)
  A <- as(A, "dgCMatrix")
  model1 <- nmf(A, 5, maxit = 5, seed = 123, v = F)
  model1_repeat <- nmf(A, 5, maxit = 5, seed = 123, v = F)
  model2 <- nmf(A, 5, maxit = 5, seed = 234, v = F)
  expect_equal(all(model1$w == model1_repeat$w), TRUE)
  expect_equal(all(model1$w == model2$w), FALSE)
  
  # ######################################
  
  # test rank-2 specialization
  
  # test that matrix factorization converges over time (SPARSE)
  A <- abs(Matrix::rsparsematrix(100, 100, 0.1))
  model_bad <- nmf(A, 2, maxit = 1, tol = 1e-10, seed = 123, v = F)
  model_good <- nmf(A, 2, maxit = 20, tol = 1e-10, seed = 123, v = F)
  model_bad_mse <- mse(A, model_bad$w, model_bad$d, model_bad$h)
  model_good_mse <- mse(A, model_good$w, model_good$d, model_good$h)
  expect_lt(model_good_mse, model_bad_mse)
  
  # test that non-negativity constraints are applied (SPARSE)
  model <- model_bad
  expect_equal(min(model$w) >= 0, TRUE)
  expect_equal(min(model$h) >= 0, TRUE)
  
  # test that non-negativity constraints are not applied when nonneg = FALSE (SPARSE)
  model <- nmf(A, 2, nonneg = FALSE, v = F)
  expect_equal(min(model$w) < 0, TRUE)
  expect_equal(min(model$h) < 0, TRUE)
  
  # test that diagonalization enforces symmetry (SPARSE)
  sim <- as(crossprod(A), "dgCMatrix")
  model <- nmf(sim, 2, diag = F, seed = 123, tol = 1e-6, v = F)
  model2 <- nmf(sim, 2, diag = T, seed = 123, tol = 1e-6, v = F)
  cor_model <- cor(as.vector(model$w), as.vector(t(model$h)))
  cor_model2 <- cor(as.vector(model2$w), as.vector(t(model2$h)))
  expect_lt(cor_model, cor_model2)
  expect_equal(mean(model$d), 1)
  
  # test that setting the random seed gives identical models (SPARSE)
  A <- as(A, "dgCMatrix")
  model1 <- nmf(A, 2, maxit = 5, seed = 123, v = F)
  model1_repeat <- nmf(A, 2, maxit = 5, seed = 123, v = F)
  model2 <- nmf(A, 2, maxit = 5, seed = 234, v = F)
  expect_equal(all(model1$w == model1_repeat$w), TRUE)
  expect_equal(all(model1$w == model2$w), FALSE)
  
})