/*
 * Software License Agreement (BSD License)
 *
 *  Point Cloud Library (PCL) - www.pointclouds.org
 *  Copyright (c) 2010, Willow Garage, Inc.
 *  Copyright (c) 2012-, Open Perception, Inc.
 *
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of the copyright holder(s) nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 *  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 */

#pragma once

#include "common.h"

/**
  * \file pcl/common/centroid.h
  * Define methods for centroid estimation and covariance matrix calculus
  * \ingroup common
  */

/*@{*/
namespace pcl
{

  /** \brief Compute the 3D (X-Y-Z) centroid of a set of points and return it as a 3D vector.
    * \param[in] cloud the input point cloud
    * \param[out] centroid the output centroid
    * \return number of valid points used to determine the centroid. In case of dense point clouds, this is the same as the size of input cloud.
    * \note if return value is 0, the centroid is not changed, thus not valid.
    * The last component of the vector is set to 1, this allows to transform the centroid vector with 4x4 matrices.
    * \ingroup common
    */
  template <typename PointT, typename Scalar> inline unsigned int
  compute3DCentroid (const pcl::PointCloud<PointT> &cloud, 
                     Eigen::Matrix<Scalar, 4, 1> &centroid);

  template <typename PointT> inline unsigned int
  compute3DCentroid (const pcl::PointCloud<PointT> &cloud, 
                     Eigen::Vector4f &centroid)
  {
    return (compute3DCentroid <PointT, float> (cloud, centroid));
  }

  template <typename PointT> inline unsigned int
  compute3DCentroid (const pcl::PointCloud<PointT> &cloud, 
                     Eigen::Vector4d &centroid)
  {
    return (compute3DCentroid <PointT, double> (cloud, centroid));
  }

  /** \brief Compute the 3D (X-Y-Z) centroid of a set of points using their indices and
    * return it as a 3D vector.
    * \param[in] cloud the input point cloud
    * \param[in] indices the point cloud indices that need to be used
    * \param[out] centroid the output centroid
    * \return number of valid points used to determine the centroid. In case of dense point clouds, this is the same as the size of input indices.
    * \note if return value is 0, the centroid is not changed, thus not valid.
    * The last component of the vector is set to 1, this allows to transform the centroid vector with 4x4 matrices.
    * \ingroup common
    */
  template <typename PointT, typename Scalar> inline unsigned int
  compute3DCentroid (const pcl::PointCloud<PointT> &cloud,
                     const std::vector<int> &indices, 
                     Eigen::Matrix<Scalar, 4, 1> &centroid);

  template <typename PointT> inline unsigned int
  compute3DCentroid (const pcl::PointCloud<PointT> &cloud,
                     const std::vector<int> &indices, 
                     Eigen::Vector4f &centroid)
  {
    return (compute3DCentroid <PointT, float> (cloud, indices, centroid));
  }

  template <typename PointT> inline unsigned int
  compute3DCentroid (const pcl::PointCloud<PointT> &cloud,
                     const std::vector<int> &indices, 
                     Eigen::Vector4d &centroid)
  {
    return (compute3DCentroid <PointT, double> (cloud, indices, centroid));
  }

  /** \brief Compute the 3D (X-Y-Z) centroid of a set of points using their indices and
    * return it as a 3D vector.
    * \param[in] cloud the input point cloud
    * \param[in] indices the point cloud indices that need to be used
    * \param[out] centroid the output centroid
    * \return number of valid points used to determine the centroid. In case of dense point clouds, this is the same as the size of input indices.
    * \note if return value is 0, the centroid is not changed, thus not valid.
    * The last component of the vector is set to 1, this allows to transform the centroid vector with 4x4 matrices.
    * \ingroup common
    */
  template <typename PointT, typename Scalar> inline unsigned int
  compute3DCentroid (const pcl::PointCloud<PointT> &cloud,
                     const pcl::PointIndices &indices, 
                     Eigen::Matrix<Scalar, 4, 1> &centroid);

  template <typename PointT> inline unsigned int
  compute3DCentroid (const pcl::PointCloud<PointT> &cloud,
                     const pcl::PointIndices &indices, 
                     Eigen::Vector4f &centroid)
  {
    return (compute3DCentroid <PointT, float> (cloud, indices, centroid));
  }

  template <typename PointT> inline unsigned int
  compute3DCentroid (const pcl::PointCloud<PointT> &cloud,
                     const pcl::PointIndices &indices, 
                     Eigen::Vector4d &centroid)
  {
    return (compute3DCentroid <PointT, double> (cloud, indices, centroid));
  }

  /** \brief Compute the 3x3 covariance matrix of a given set of points.
    * The result is returned as a Eigen::Matrix3f.
    * Note: the covariance matrix is not normalized with the number of
    * points. For a normalized covariance, please use
    * computeCovarianceMatrixNormalized.
    * \param[in] cloud the input point cloud
    * \param[in] centroid the centroid of the set of points in the cloud
    * \param[out] covariance_matrix the resultant 3x3 covariance matrix
    * \return number of valid points used to determine the covariance matrix.
    * In case of dense point clouds, this is the same as the size of input cloud.
    * \note if return value is 0, the covariance matrix is not changed, thus not valid.
    * \ingroup common
    */
  template <typename PointT, typename Scalar> inline unsigned int
  computeCovarianceMatrix (const pcl::PointCloud<PointT> &cloud,
                           const Eigen::Matrix<Scalar, 4, 1> &centroid,
                           Eigen::Matrix<Scalar, 3, 3> &covariance_matrix);

  template <typename PointT> inline unsigned int
  computeCovarianceMatrix (const pcl::PointCloud<PointT> &cloud,
                           const Eigen::Vector4f &centroid,
                           Eigen::Matrix3f &covariance_matrix)
  {
    return (computeCovarianceMatrix<PointT, float> (cloud, centroid, covariance_matrix));
  }

  template <typename PointT> inline unsigned int
  computeCovarianceMatrix (const pcl::PointCloud<PointT> &cloud,
                           const Eigen::Vector4d &centroid,
                           Eigen::Matrix3d &covariance_matrix)
  {
    return (computeCovarianceMatrix<PointT, double> (cloud, centroid, covariance_matrix));
  }

  /** \brief Compute normalized the 3x3 covariance matrix of a given set of points.
    * The result is returned as a Eigen::Matrix3f.
    * Normalized means that every entry has been divided by the number of points in the point cloud.
    * For small number of points, or if you want explicitly the sample-variance, use computeCovarianceMatrix
    * and scale the covariance matrix with 1 / (n-1), where n is the number of points used to calculate
    * the covariance matrix and is returned by the computeCovarianceMatrix function.
    * \param[in] cloud the input point cloud
    * \param[in] centroid the centroid of the set of points in the cloud
    * \param[out] covariance_matrix the resultant 3x3 covariance matrix
    * \return number of valid points used to determine the covariance matrix.
    * In case of dense point clouds, this is the same as the size of input cloud.
    * \ingroup common
    */
  template <typename PointT, typename Scalar> inline unsigned int
  computeCovarianceMatrixNormalized (const pcl::PointCloud<PointT> &cloud,
                                     const Eigen::Matrix<Scalar, 4, 1> &centroid,
                                     Eigen::Matrix<Scalar, 3, 3> &covariance_matrix);

  template <typename PointT> inline unsigned int
  computeCovarianceMatrixNormalized (const pcl::PointCloud<PointT> &cloud,
                                     const Eigen::Vector4f &centroid,
                                     Eigen::Matrix3f &covariance_matrix)
  {
    return (computeCovarianceMatrixNormalized<PointT, float> (cloud, centroid, covariance_matrix));
  }

  template <typename PointT> inline unsigned int
  computeCovarianceMatrixNormalized (const pcl::PointCloud<PointT> &cloud,
                                     const Eigen::Vector4d &centroid,
                                     Eigen::Matrix3d &covariance_matrix)
  {
    return (computeCovarianceMatrixNormalized<PointT, double> (cloud, centroid, covariance_matrix));
  }

  /** \brief Compute the 3x3 covariance matrix of a given set of points using their indices.
    * The result is returned as a Eigen::Matrix3f.
    * Note: the covariance matrix is not normalized with the number of
    * points. For a normalized covariance, please use
    * computeCovarianceMatrixNormalized.
    * \param[in] cloud the input point cloud
    * \param[in] indices the point cloud indices that need to be used
    * \param[in] centroid the centroid of the set of points in the cloud
    * \param[out] covariance_matrix the resultant 3x3 covariance matrix
    * \return number of valid points used to determine the covariance matrix.
    * In case of dense point clouds, this is the same as the size of input indices.
    * \ingroup common
    */
  template <typename PointT, typename Scalar> inline unsigned int
  computeCovarianceMatrix (const pcl::PointCloud<PointT> &cloud,
                           const std::vector<int> &indices,
                           const Eigen::Matrix<Scalar, 4, 1> &centroid,
                           Eigen::Matrix<Scalar, 3, 3> &covariance_matrix);

  template <typename PointT> inline unsigned int
  computeCovarianceMatrix (const pcl::PointCloud<PointT> &cloud,
                           const std::vector<int> &indices,
                           const Eigen::Vector4f &centroid,
                           Eigen::Matrix3f &covariance_matrix)
  {
    return (computeCovarianceMatrix<PointT, float> (cloud, indices, centroid, covariance_matrix));
  }

  template <typename PointT> inline unsigned int
  computeCovarianceMatrix (const pcl::PointCloud<PointT> &cloud,
                           const std::vector<int> &indices,
                           const Eigen::Vector4d &centroid,
                           Eigen::Matrix3d &covariance_matrix)
  {
    return (computeCovarianceMatrix<PointT, double> (cloud, indices, centroid, covariance_matrix));
  }

  /** \brief Compute the 3x3 covariance matrix of a given set of points using their indices.
    * The result is returned as a Eigen::Matrix3f.
    * Note: the covariance matrix is not normalized with the number of
    * points. For a normalized covariance, please use
    * computeCovarianceMatrixNormalized.
    * \param[in] cloud the input point cloud
    * \param[in] indices the point cloud indices that need to be used
    * \param[in] centroid the centroid of the set of points in the cloud
    * \param[out] covariance_matrix the resultant 3x3 covariance matrix
    * \return number of valid points used to determine the covariance matrix.
    * In case of dense point clouds, this is the same as the size of input indices.
    * \ingroup common
    */
  template <typename PointT, typename Scalar> inline unsigned int
  computeCovarianceMatrix (const pcl::PointCloud<PointT> &cloud,
                           const pcl::PointIndices &indices,
                           const Eigen::Matrix<Scalar, 4, 1> &centroid,
                           Eigen::Matrix<Scalar, 3, 3> &covariance_matrix);

  template <typename PointT> inline unsigned int
  computeCovarianceMatrix (const pcl::PointCloud<PointT> &cloud,
                           const pcl::PointIndices &indices,
                           const Eigen::Vector4f &centroid,
                           Eigen::Matrix3f &covariance_matrix)
  {
    return (computeCovarianceMatrix<PointT, float> (cloud, indices, centroid, covariance_matrix));
  }

  template <typename PointT> inline unsigned int
  computeCovarianceMatrix (const pcl::PointCloud<PointT> &cloud,
                           const pcl::PointIndices &indices,
                           const Eigen::Vector4d &centroid,
                           Eigen::Matrix3d &covariance_matrix)
  {
    return (computeCovarianceMatrix<PointT, double> (cloud, indices, centroid, covariance_matrix));
  }

  /** \brief Compute the normalized 3x3 covariance matrix of a given set of points using
    * their indices.
    * The result is returned as a Eigen::Matrix3f.
    * Normalized means that every entry has been divided by the number of entries in indices.
    * For small number of points, or if you want explicitly the sample-variance, use computeCovarianceMatrix
    * and scale the covariance matrix with 1 / (n-1), where n is the number of points used to calculate
    * the covariance matrix and is returned by the computeCovarianceMatrix function.
    * \param[in] cloud the input point cloud
    * \param[in] indices the point cloud indices that need to be used
    * \param[in] centroid the centroid of the set of points in the cloud
    * \param[out] covariance_matrix the resultant 3x3 covariance matrix
    * \return number of valid points used to determine the covariance matrix.
    * In case of dense point clouds, this is the same as the size of input indices.
    * \ingroup common
    */
  template <typename PointT, typename Scalar> inline unsigned int
  computeCovarianceMatrixNormalized (const pcl::PointCloud<PointT> &cloud,
                                     const std::vector<int> &indices,
                                     const Eigen::Matrix<Scalar, 4, 1> &centroid,
                                     Eigen::Matrix<Scalar, 3, 3> &covariance_matrix);

  template <typename PointT> inline unsigned int
  computeCovarianceMatrixNormalized (const pcl::PointCloud<PointT> &cloud,
                                     const std::vector<int> &indices,
                                     const Eigen::Vector4f &centroid,
                                     Eigen::Matrix3f &covariance_matrix)
  {
    return (computeCovarianceMatrixNormalized<PointT, float> (cloud, indices, centroid, covariance_matrix));
  }

  template <typename PointT> inline unsigned int
  computeCovarianceMatrixNormalized (const pcl::PointCloud<PointT> &cloud,
                                     const std::vector<int> &indices,
                                     const Eigen::Vector4d &centroid,
                                     Eigen::Matrix3d &covariance_matrix)
  {
    return (computeCovarianceMatrixNormalized<PointT, double> (cloud, indices, centroid, covariance_matrix));
  }

  /** \brief Compute the normalized 3x3 covariance matrix of a given set of points using
    * their indices. The result is returned as a Eigen::Matrix3f.
    * Normalized means that every entry has been divided by the number of entries in indices.
    * For small number of points, or if you want explicitly the sample-variance, use computeCovarianceMatrix
    * and scale the covariance matrix with 1 / (n-1), where n is the number of points used to calculate
    * the covariance matrix and is returned by the computeCovarianceMatrix function.
    * \param[in] cloud the input point cloud
    * \param[in] indices the point cloud indices that need to be used
    * \param[in] centroid the centroid of the set of points in the cloud
    * \param[out] covariance_matrix the resultant 3x3 covariance matrix
    * \return number of valid points used to determine the covariance matrix.
    * In case of dense point clouds, this is the same as the size of input indices.
    * \ingroup common
    */
  template <typename PointT, typename Scalar> inline unsigned int
  computeCovarianceMatrixNormalized (const pcl::PointCloud<PointT> &cloud,
                                     const pcl::PointIndices &indices,
                                     const Eigen::Matrix<Scalar, 4, 1> &centroid,
                                     Eigen::Matrix<Scalar, 3, 3> &covariance_matrix);

  template <typename PointT> inline unsigned int
  computeCovarianceMatrixNormalized (const pcl::PointCloud<PointT> &cloud,
                                     const pcl::PointIndices &indices,
                                     const Eigen::Vector4f &centroid,
                                     Eigen::Matrix3f &covariance_matrix)
  {
    return (computeCovarianceMatrixNormalized<PointT, float> (cloud, indices, centroid, covariance_matrix));
  }

  template <typename PointT> inline unsigned int
  computeCovarianceMatrixNormalized (const pcl::PointCloud<PointT> &cloud,
                                     const pcl::PointIndices &indices,
                                     const Eigen::Vector4d &centroid,
                                     Eigen::Matrix3d &covariance_matrix)
  {
    return (computeCovarianceMatrixNormalized<PointT, double> (cloud, indices, centroid, covariance_matrix));
  }

  /** \brief Compute the normalized 3x3 covariance matrix and the centroid of a given set of points in a single loop.
    * Normalized means that every entry has been divided by the number of valid entries in the point cloud.
    * For small number of points, or if you want explicitly the sample-variance, scale the covariance matrix
    * with n / (n-1), where n is the number of points used to calculate the covariance matrix and is returned by this function.
    * \note This method is theoretically exact. However using float for internal calculations reduces the accuracy but increases the efficiency.
    * \param[in] cloud the input point cloud
    * \param[out] covariance_matrix the resultant 3x3 covariance matrix
    * \param[out] centroid the centroid of the set of points in the cloud
    * \return number of valid points used to determine the covariance matrix.
    * In case of dense point clouds, this is the same as the size of input cloud.
    * \ingroup common
    */
  template <typename PointT, typename Scalar> inline unsigned int
  computeMeanAndCovarianceMatrix (const pcl::PointCloud<PointT> &cloud,
                                  Eigen::Matrix<Scalar, 3, 3> &covariance_matrix,
                                  Eigen::Matrix<Scalar, 4, 1> &centroid);

  template <typename PointT> inline unsigned int
  computeMeanAndCovarianceMatrix (const pcl::PointCloud<PointT> &cloud,
                                  Eigen::Matrix3f &covariance_matrix,
                                  Eigen::Vector4f &centroid)
  {
    return (computeMeanAndCovarianceMatrix<PointT, float> (cloud, covariance_matrix, centroid));
  }

  template <typename PointT> inline unsigned int
  computeMeanAndCovarianceMatrix (const pcl::PointCloud<PointT> &cloud,
                                  Eigen::Matrix3d &covariance_matrix,
                                  Eigen::Vector4d &centroid)
  {
    return (computeMeanAndCovarianceMatrix<PointT, double> (cloud, covariance_matrix, centroid));
  }

  /** \brief Compute the normalized 3x3 covariance matrix and the centroid of a given set of points in a single loop.
    * Normalized means that every entry has been divided by the number of entries in indices.
    * For small number of points, or if you want explicitly the sample-variance, scale the covariance matrix
    * with n / (n-1), where n is the number of points used to calculate the covariance matrix and is returned by this function.
    * \note This method is theoretically exact. However using float for internal calculations reduces the accuracy but increases the efficiency.
    * \param[in] cloud the input point cloud
    * \param[in] indices subset of points given by their indices
    * \param[out] covariance_matrix the resultant 3x3 covariance matrix
    * \param[out] centroid the centroid of the set of points in the cloud
    * \return number of valid points used to determine the covariance matrix.
    * In case of dense point clouds, this is the same as the size of input indices.
    * \ingroup common
    */
  template <typename PointT, typename Scalar> inline unsigned int
  computeMeanAndCovarianceMatrix (const pcl::PointCloud<PointT> &cloud,
                                  const std::vector<int> &indices,
                                  Eigen::Matrix<Scalar, 3, 3> &covariance_matrix,
                                  Eigen::Matrix<Scalar, 4, 1> &centroid);

  template <typename PointT> inline unsigned int
  computeMeanAndCovarianceMatrix (const pcl::PointCloud<PointT> &cloud,
                                  const std::vector<int> &indices,
                                  Eigen::Matrix3f &covariance_matrix,
                                  Eigen::Vector4f &centroid)
  {
    return (computeMeanAndCovarianceMatrix<PointT, float> (cloud, indices, covariance_matrix, centroid));
  }

  template <typename PointT> inline unsigned int
  computeMeanAndCovarianceMatrix (const pcl::PointCloud<PointT> &cloud,
                                  const std::vector<int> &indices,
                                  Eigen::Matrix3d &covariance_matrix,
                                  Eigen::Vector4d &centroid)
  {
    return (computeMeanAndCovarianceMatrix<PointT, double> (cloud, indices, covariance_matrix, centroid));
  }

  /** \brief Compute the normalized 3x3 covariance matrix and the centroid of a given set of points in a single loop.
    * Normalized means that every entry has been divided by the number of entries in indices.
    * For small number of points, or if you want explicitly the sample-variance, scale the covariance matrix
    * with n / (n-1), where n is the number of points used to calculate the covariance matrix and is returned by this function.
    * \note This method is theoretically exact. However using float for internal calculations reduces the accuracy but increases the efficiency.
    * \param[in] cloud the input point cloud
    * \param[in] indices subset of points given by their indices
    * \param[out] centroid the centroid of the set of points in the cloud
    * \param[out] covariance_matrix the resultant 3x3 covariance matrix
    * \return number of valid points used to determine the covariance matrix.
    * In case of dense point clouds, this is the same as the size of input indices.
    * \ingroup common
    */
  template <typename PointT, typename Scalar> inline unsigned int
  computeMeanAndCovarianceMatrix (const pcl::PointCloud<PointT> &cloud,
                                  const pcl::PointIndices &indices,
                                  Eigen::Matrix<Scalar, 3, 3> &covariance_matrix,
                                  Eigen::Matrix<Scalar, 4, 1> &centroid);

  template <typename PointT> inline unsigned int
  computeMeanAndCovarianceMatrix (const pcl::PointCloud<PointT> &cloud,
                                  const pcl::PointIndices &indices,
                                  Eigen::Matrix3f &covariance_matrix,
                                  Eigen::Vector4f &centroid)
  {
    return (computeMeanAndCovarianceMatrix<PointT, float> (cloud, indices, covariance_matrix, centroid));
  }

  template <typename PointT> inline unsigned int
  computeMeanAndCovarianceMatrix (const pcl::PointCloud<PointT> &cloud,
                                  const pcl::PointIndices &indices,
                                  Eigen::Matrix3d &covariance_matrix,
                                  Eigen::Vector4d &centroid)
  {
    return (computeMeanAndCovarianceMatrix<PointT, double> (cloud, indices, covariance_matrix, centroid));
  }

  /** \brief Compute the normalized 3x3 covariance matrix for a already demeaned point cloud.
    * Normalized means that every entry has been divided by the number of entries in the input point cloud.
    * For small number of points, or if you want explicitly the sample-variance, scale the covariance matrix
    * with n / (n-1), where n is the number of points used to calculate the covariance matrix and is returned by this function.
    * \note This method is theoretically exact. However using float for internal calculations reduces the accuracy but increases the efficiency.
    * \param[in] cloud the input point cloud
    * \param[out] covariance_matrix the resultant 3x3 covariance matrix
    * \return number of valid points used to determine the covariance matrix.
    * In case of dense point clouds, this is the same as the size of input cloud.
    * \ingroup common
    */
  template <typename PointT, typename Scalar> inline unsigned int
  computeCovarianceMatrix (const pcl::PointCloud<PointT> &cloud,
                           Eigen::Matrix<Scalar, 3, 3> &covariance_matrix);

  template <typename PointT> inline unsigned int
  computeCovarianceMatrix (const pcl::PointCloud<PointT> &cloud,
                           Eigen::Matrix3f &covariance_matrix)
  {
    return (computeCovarianceMatrix<PointT, float> (cloud, covariance_matrix));
  }

  template <typename PointT> inline unsigned int
  computeCovarianceMatrix (const pcl::PointCloud<PointT> &cloud,
                           Eigen::Matrix3d &covariance_matrix)
  {
    return (computeCovarianceMatrix<PointT, double> (cloud, covariance_matrix));
  }

  /** \brief Compute the normalized 3x3 covariance matrix for a already demeaned point cloud.
    * Normalized means that every entry has been divided by the number of entries in indices.
    * For small number of points, or if you want explicitly the sample-variance, scale the covariance matrix
    * with n / (n-1), where n is the number of points used to calculate the covariance matrix and is returned by this function.
    * \note This method is theoretically exact. However using float for internal calculations reduces the accuracy but increases the efficiency.
    * \param[in] cloud the input point cloud
    * \param[in] indices subset of points given by their indices
    * \param[out] covariance_matrix the resultant 3x3 covariance matrix
    * \return number of valid points used to determine the covariance matrix.
    * In case of dense point clouds, this is the same as the size of input indices.
    * \ingroup common
    */
  template <typename PointT, typename Scalar> inline unsigned int
  computeCovarianceMatrix (const pcl::PointCloud<PointT> &cloud,
                           const std::vector<int> &indices,
                           Eigen::Matrix<Scalar, 3, 3> &covariance_matrix);

  template <typename PointT> inline unsigned int
  computeCovarianceMatrix (const pcl::PointCloud<PointT> &cloud,
                           const std::vector<int> &indices,
                           Eigen::Matrix3f &covariance_matrix)
  {
    return (computeCovarianceMatrix<PointT, float> (cloud, indices, covariance_matrix));
  }

  template <typename PointT> inline unsigned int
  computeCovarianceMatrix (const pcl::PointCloud<PointT> &cloud,
                           const std::vector<int> &indices,
                           Eigen::Matrix3d &covariance_matrix)
  {
    return (computeCovarianceMatrix<PointT, double> (cloud, indices, covariance_matrix));
  }

  /** \brief Compute the normalized 3x3 covariance matrix for a already demeaned point cloud.
    * Normalized means that every entry has been divided by the number of entries in indices.
    * For small number of points, or if you want explicitly the sample-variance, scale the covariance matrix
    * with n / (n-1), where n is the number of points used to calculate the covariance matrix and is returned by this function.
    * \note This method is theoretically exact. However using float for internal calculations reduces the accuracy but increases the efficiency.
    * \param[in] cloud the input point cloud
    * \param[in] indices subset of points given by their indices
    * \param[out] covariance_matrix the resultant 3x3 covariance matrix
    * \return number of valid points used to determine the covariance matrix.
    * In case of dense point clouds, this is the same as the size of input indices.
    * \ingroup common
    */
  template <typename PointT, typename Scalar> inline unsigned int
  computeCovarianceMatrix (const pcl::PointCloud<PointT> &cloud,
                           const pcl::PointIndices &indices,
                           Eigen::Matrix<Scalar, 3, 3> &covariance_matrix);

  template <typename PointT> inline unsigned int
  computeCovarianceMatrix (const pcl::PointCloud<PointT> &cloud,
                           const pcl::PointIndices &indices,
                           Eigen::Matrix3f &covariance_matrix)
  {
    return (computeCovarianceMatrix<PointT, float> (cloud, indices, covariance_matrix));
  }

  template <typename PointT> inline unsigned int
  computeCovarianceMatrix (const pcl::PointCloud<PointT> &cloud,
                           const pcl::PointIndices &indices,
                           Eigen::Matrix3d &covariance_matrix)
  {
    return (computeCovarianceMatrix<PointT, double> (cloud, indices, covariance_matrix));
  }

  /** \brief Subtract a centroid from a point cloud and return the de-meaned representation
    * \param[in] cloud_in the input point cloud
    * \param[in] centroid the centroid of the point cloud
    * \param[out] cloud_out the resultant output point cloud
    * \ingroup common
    */
  template <typename PointT, typename Scalar> void
  demeanPointCloud (const pcl::PointCloud<PointT> &cloud_in,
                    const Eigen::Matrix<Scalar, 4, 1> &centroid,
                    pcl::PointCloud<PointT> &cloud_out);

  /** \brief Subtract a centroid from a point cloud and return the de-meaned representation
    * \param[in] cloud_in the input point cloud
    * \param[in] indices the set of point indices to use from the input point cloud
    * \param[out] centroid the centroid of the point cloud
    * \param cloud_out the resultant output point cloud
    * \ingroup common
    */
  template <typename PointT, typename Scalar> void
  demeanPointCloud (const pcl::PointCloud<PointT> &cloud_in,
                    const std::vector<int> &indices,
                    const Eigen::Matrix<Scalar, 4, 1> &centroid,
                    pcl::PointCloud<PointT> &cloud_out);

  template <typename PointT> void
  demeanPointCloud (const pcl::PointCloud<PointT> &cloud_in,
                    const std::vector<int> &indices,
                    const Eigen::Vector4f &centroid,
                    pcl::PointCloud<PointT> &cloud_out)
  {
    return (demeanPointCloud<PointT, float> (cloud_in, indices, centroid, cloud_out));
  }

  template <typename PointT> void
  demeanPointCloud (const pcl::PointCloud<PointT> &cloud_in,
                    const std::vector<int> &indices,
                    const Eigen::Vector4d &centroid,
                    pcl::PointCloud<PointT> &cloud_out)
  {
    return (demeanPointCloud<PointT, double> (cloud_in, indices, centroid, cloud_out));
  }

  /** \brief Subtract a centroid from a point cloud and return the de-meaned representation
    * \param[in] cloud_in the input point cloud
    * \param[in] indices the set of point indices to use from the input point cloud
    * \param[out] centroid the centroid of the point cloud
    * \param cloud_out the resultant output point cloud
    * \ingroup common
    */
  template <typename PointT, typename Scalar> void
  demeanPointCloud (const pcl::PointCloud<PointT> &cloud_in,
                    const pcl::PointIndices& indices,
                    const Eigen::Matrix<Scalar, 4, 1> &centroid,
                    pcl::PointCloud<PointT> &cloud_out);

  template <typename PointT> void
  demeanPointCloud (const pcl::PointCloud<PointT> &cloud_in,
                    const pcl::PointIndices& indices,
                    const Eigen::Vector4f &centroid,
                    pcl::PointCloud<PointT> &cloud_out)
  {
    return (demeanPointCloud<PointT, float> (cloud_in, indices, centroid, cloud_out));
  }

  template <typename PointT> void
  demeanPointCloud (const pcl::PointCloud<PointT> &cloud_in,
                    const pcl::PointIndices& indices,
                    const Eigen::Vector4d &centroid,
                    pcl::PointCloud<PointT> &cloud_out)
  {
    return (demeanPointCloud<PointT, double> (cloud_in, indices, centroid, cloud_out));
  }


  /** \brief Subtract a centroid from a point cloud and return the de-meaned
    * representation as an Eigen matrix
    * \param[in] cloud_in the input point cloud
    * \param[in] centroid the centroid of the point cloud
    * \param[out] cloud_out the resultant output XYZ0 dimensions of \a cloud_in as
    * an Eigen matrix (4 rows, N pts columns)
    * \ingroup common
    */
  template <typename PointT, typename Scalar> void
  demeanPointCloud (const pcl::PointCloud<PointT> &cloud_in,
                    const Eigen::Matrix<Scalar, 4, 1> &centroid,
                    Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic> &cloud_out);

  template <typename PointT> void
  demeanPointCloud (const pcl::PointCloud<PointT> &cloud_in,
                    const Eigen::Vector4f &centroid,
                    Eigen::MatrixXf &cloud_out)
  {
    return (demeanPointCloud<PointT, float> (cloud_in, centroid, cloud_out));
  }

  template <typename PointT> void
  demeanPointCloud (const pcl::PointCloud<PointT> &cloud_in,
                    const Eigen::Vector4d &centroid,
                    Eigen::MatrixXd &cloud_out)
  {
    return (demeanPointCloud<PointT, double> (cloud_in, centroid, cloud_out));
  }

  /** \brief Subtract a centroid from a point cloud and return the de-meaned
    * representation as an Eigen matrix
    * \param[in] cloud_in the input point cloud
    * \param[in] indices the set of point indices to use from the input point cloud
    * \param[in] centroid the centroid of the point cloud
    * \param[out] cloud_out the resultant output XYZ0 dimensions of \a cloud_in as
    * an Eigen matrix (4 rows, N pts columns)
    * \ingroup common
    */
  template <typename PointT, typename Scalar> void
  demeanPointCloud (const pcl::PointCloud<PointT> &cloud_in,
                    const std::vector<int> &indices,
                    const Eigen::Matrix<Scalar, 4, 1> &centroid,
                    Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic> &cloud_out);

  template <typename PointT> void
  demeanPointCloud (const pcl::PointCloud<PointT> &cloud_in,
                    const std::vector<int> &indices,
                    const Eigen::Vector4f &centroid,
                    Eigen::MatrixXf &cloud_out)
  {
    return (demeanPointCloud<PointT, float> (cloud_in, indices, centroid, cloud_out));
  }

  template <typename PointT> void
  demeanPointCloud (const pcl::PointCloud<PointT> &cloud_in,
                    const std::vector<int> &indices,
                    const Eigen::Vector4d &centroid,
                    Eigen::MatrixXd &cloud_out)
  {
    return (demeanPointCloud<PointT, double> (cloud_in, indices, centroid, cloud_out));
  }

  /** \brief Subtract a centroid from a point cloud and return the de-meaned
    * representation as an Eigen matrix
    * \param[in] cloud_in the input point cloud
    * \param[in] indices the set of point indices to use from the input point cloud
    * \param[in] centroid the centroid of the point cloud
    * \param[out] cloud_out the resultant output XYZ0 dimensions of \a cloud_in as
    * an Eigen matrix (4 rows, N pts columns)
    * \ingroup common
    */
  template <typename PointT, typename Scalar> void
  demeanPointCloud (const pcl::PointCloud<PointT> &cloud_in,
                    const pcl::PointIndices& indices,
                    const Eigen::Matrix<Scalar, 4, 1> &centroid,
                    Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic> &cloud_out);

  template <typename PointT> void
  demeanPointCloud (const pcl::PointCloud<PointT> &cloud_in,
                    const pcl::PointIndices& indices,
                    const Eigen::Vector4f &centroid,
                    Eigen::MatrixXf &cloud_out)
  {
    return (demeanPointCloud<PointT, float> (cloud_in, indices, centroid, cloud_out));
  }

  template <typename PointT> void
  demeanPointCloud (const pcl::PointCloud<PointT> &cloud_in,
                    const pcl::PointIndices& indices,
                    const Eigen::Vector4d &centroid,
                    Eigen::MatrixXd &cloud_out)
  {
    return (demeanPointCloud<PointT, double> (cloud_in, indices, centroid, cloud_out));
  }

  /** \brief General, all purpose nD centroid estimation for a set of points using their
    * indices.
    * \param cloud the input point cloud
    * \param centroid the output centroid
    * \ingroup common
    */
  template <typename PointT, typename Scalar> inline void
  computeNDCentroid (const pcl::PointCloud<PointT> &cloud, 
                     Eigen::Matrix<Scalar, Eigen::Dynamic, 1> &centroid);

  template <typename PointT> inline void
  computeNDCentroid (const pcl::PointCloud<PointT> &cloud, 
                     Eigen::VectorXf &centroid)
  {
    return (computeNDCentroid<PointT, float> (cloud, centroid));
  }

  template <typename PointT> inline void
  computeNDCentroid (const pcl::PointCloud<PointT> &cloud, 
                     Eigen::VectorXd &centroid)
  {
    return (computeNDCentroid<PointT, double> (cloud, centroid));
  }

  /** \brief General, all purpose nD centroid estimation for a set of points using their
    * indices.
    * \param cloud the input point cloud
    * \param indices the point cloud indices that need to be used
    * \param centroid the output centroid
    * \ingroup common
    */
  template <typename PointT, typename Scalar> inline void
  computeNDCentroid (const pcl::PointCloud<PointT> &cloud,
                     const std::vector<int> &indices, 
                     Eigen::Matrix<Scalar, Eigen::Dynamic, 1> &centroid);

  template <typename PointT> inline void
  computeNDCentroid (const pcl::PointCloud<PointT> &cloud, 
                     const std::vector<int> &indices, 
                     Eigen::VectorXf &centroid)
  {
    return (computeNDCentroid<PointT, float> (cloud, indices, centroid));
  }

  template <typename PointT> inline void
  computeNDCentroid (const pcl::PointCloud<PointT> &cloud, 
                     const std::vector<int> &indices, 
                     Eigen::VectorXd &centroid)
  {
    return (computeNDCentroid<PointT, double> (cloud, indices, centroid));
  }

  /** \brief General, all purpose nD centroid estimation for a set of points using their
    * indices.
    * \param cloud the input point cloud
    * \param indices the point cloud indices that need to be used
    * \param centroid the output centroid
    * \ingroup common
    */
  template <typename PointT, typename Scalar> inline void
  computeNDCentroid (const pcl::PointCloud<PointT> &cloud,
                     const pcl::PointIndices &indices, 
                     Eigen::Matrix<Scalar, Eigen::Dynamic, 1> &centroid);

  template <typename PointT> inline void
  computeNDCentroid (const pcl::PointCloud<PointT> &cloud, 
                     const pcl::PointIndices &indices, 
                     Eigen::VectorXf &centroid)
  {
    return (computeNDCentroid<PointT, float> (cloud, indices, centroid));
  }

  template <typename PointT> inline void
  computeNDCentroid (const pcl::PointCloud<PointT> &cloud, 
                     const pcl::PointIndices &indices, 
                     Eigen::VectorXd &centroid)
  {
    return (computeNDCentroid<PointT, double> (cloud, indices, centroid));
  }

 
  /** Compute the centroid of a set of points and return it as a point.
    *
    * Implementation leverages \ref CentroidPoint class and therefore behaves
    * differently from \ref compute3DCentroid() and \ref computeNDCentroid().
    * See \ref CentroidPoint documentation for explanation.
    *
    * \param[in] cloud input point cloud
    * \param[out] centroid output centroid
    *
    * \return number of valid points used to determine the centroid (will be the
    * same as the size of the cloud if it is dense)
    *
    * \note If return value is \c 0, then the centroid is not changed, thus is
    * not valid.
    *
    * \ingroup common */
  template <typename PointInT, typename PointOutT> size_t
  computeCentroid (const pcl::PointCloud<PointInT>& cloud,
                   PointOutT& centroid);

  /** Compute the centroid of a set of points and return it as a point.
    * \param[in] cloud
    * \param[in] indices point cloud indices that need to be used
    * \param[out] centroid
    * This is an overloaded function provided for convenience. See the
    * documentation for computeCentroid().
    *
    * \ingroup common */
  template <typename PointInT, typename PointOutT> size_t
  computeCentroid (const pcl::PointCloud<PointInT>& cloud,
                   const std::vector<int>& indices,
                   PointOutT& centroid);

}


// #include "centroid.hpp"




///////////////////////////////////////////////////////////////////////////////////////////
template <typename PointT, typename Scalar>
inline unsigned int pcl::compute3DCentroid(
    const pcl::PointCloud<PointT>& cloud, Eigen::Matrix<Scalar, 4, 1>& centroid) {
    if (cloud.empty()) return (0);

    // Initialize to 0
    centroid.setZero();
    // For each point in the cloud
    // If the data is dense, we don't need to check for NaN
    if (cloud.is_dense) {
        for (size_t i = 0; i < cloud.size(); ++i) {
            centroid[0] += cloud[i].x;
            centroid[1] += cloud[i].y;
            centroid[2] += cloud[i].z;
        }
        centroid /= static_cast<Scalar>(cloud.size());
        centroid[3] = 1;

        return (static_cast<unsigned int>(cloud.size()));
    }
    // NaN or Inf values could exist => check for them
    unsigned cp = 0;
    for (size_t i = 0; i < cloud.size(); ++i) {
        // Check if the point is invalid
        if (!isFinite(cloud[i])) continue;

        centroid[0] += cloud[i].x;
        centroid[1] += cloud[i].y;
        centroid[2] += cloud[i].z;
        ++cp;
    }
    centroid /= static_cast<Scalar>(cp);
    centroid[3] = 1;

    return (cp);
}

///////////////////////////////////////////////////////////////////////////////////////////
template <typename PointT, typename Scalar>
inline unsigned int pcl::compute3DCentroid(
    const pcl::PointCloud<PointT>& cloud, const std::vector<int>& indices, Eigen::Matrix<Scalar, 4, 1>& centroid) {
    if (indices.empty()) return (0);

    // Initialize to 0
    centroid.setZero();
    // If the data is dense, we don't need to check for NaN
    if (cloud.is_dense) {
        for (const int& index : indices) {
            centroid[0] += cloud[index].x;
            centroid[1] += cloud[index].y;
            centroid[2] += cloud[index].z;
        }
        centroid /= static_cast<Scalar>(indices.size());
        centroid[3] = 1;
        return (static_cast<unsigned int>(indices.size()));
    }
    // NaN or Inf values could exist => check for them
    unsigned cp = 0;
    for (const int& index : indices) {
        // Check if the point is invalid
        if (!isFinite(cloud[index])) continue;

        centroid[0] += cloud[index].x;
        centroid[1] += cloud[index].y;
        centroid[2] += cloud[index].z;
        ++cp;
    }
    centroid /= static_cast<Scalar>(cp);
    centroid[3] = 1;
    return (cp);
}

/////////////////////////////////////////////////////////////////////////////////////////////
template <typename PointT, typename Scalar>
inline unsigned int pcl::compute3DCentroid(
    const pcl::PointCloud<PointT>& cloud, const pcl::PointIndices& indices, Eigen::Matrix<Scalar, 4, 1>& centroid) {
    return (pcl::compute3DCentroid(cloud, indices.indices, centroid));
}

//////////////////////////////////////////////////////////////////////////////////////////////
template <typename PointT, typename Scalar>
inline unsigned pcl::computeCovarianceMatrix(const pcl::PointCloud<PointT>& cloud,
    const Eigen::Matrix<Scalar, 4, 1>& centroid, Eigen::Matrix<Scalar, 3, 3>& covariance_matrix) {
    if (cloud.empty()) return (0);

    // Initialize to 0
    covariance_matrix.setZero();

    unsigned point_count;
    // If the data is dense, we don't need to check for NaN
    if (cloud.is_dense) {
        point_count = static_cast<unsigned>(cloud.size());
        // For each point in the cloud
        for (size_t i = 0; i < point_count; ++i) {
            Eigen::Matrix<Scalar, 4, 1> pt;
            pt[0] = cloud[i].x - centroid[0];
            pt[1] = cloud[i].y - centroid[1];
            pt[2] = cloud[i].z - centroid[2];

            covariance_matrix(1, 1) += pt.y() * pt.y();
            covariance_matrix(1, 2) += pt.y() * pt.z();

            covariance_matrix(2, 2) += pt.z() * pt.z();

            pt *= pt.x();
            covariance_matrix(0, 0) += pt.x();
            covariance_matrix(0, 1) += pt.y();
            covariance_matrix(0, 2) += pt.z();
        }
    }
    // NaN or Inf values could exist => check for them
    else {
        point_count = 0;
        // For each point in the cloud
        for (size_t i = 0; i < cloud.size(); ++i) {
            // Check if the point is invalid
            if (!isFinite(cloud[i])) continue;

            Eigen::Matrix<Scalar, 4, 1> pt;
            pt[0] = cloud[i].x - centroid[0];
            pt[1] = cloud[i].y - centroid[1];
            pt[2] = cloud[i].z - centroid[2];

            covariance_matrix(1, 1) += pt.y() * pt.y();
            covariance_matrix(1, 2) += pt.y() * pt.z();

            covariance_matrix(2, 2) += pt.z() * pt.z();

            pt *= pt.x();
            covariance_matrix(0, 0) += pt.x();
            covariance_matrix(0, 1) += pt.y();
            covariance_matrix(0, 2) += pt.z();
            ++point_count;
        }
    }
    covariance_matrix(1, 0) = covariance_matrix(0, 1);
    covariance_matrix(2, 0) = covariance_matrix(0, 2);
    covariance_matrix(2, 1) = covariance_matrix(1, 2);

    return (point_count);
}

//////////////////////////////////////////////////////////////////////////////////////////////
template <typename PointT, typename Scalar>
inline unsigned int pcl::computeCovarianceMatrixNormalized(const pcl::PointCloud<PointT>& cloud,
    const Eigen::Matrix<Scalar, 4, 1>& centroid, Eigen::Matrix<Scalar, 3, 3>& covariance_matrix) {
    unsigned point_count = pcl::computeCovarianceMatrix(cloud, centroid, covariance_matrix);
    if (point_count != 0) covariance_matrix /= static_cast<Scalar>(point_count);
    return (point_count);
}

//////////////////////////////////////////////////////////////////////////////////////////////
template <typename PointT, typename Scalar>
inline unsigned int pcl::computeCovarianceMatrix(const pcl::PointCloud<PointT>& cloud, const std::vector<int>& indices,
    const Eigen::Matrix<Scalar, 4, 1>& centroid, Eigen::Matrix<Scalar, 3, 3>& covariance_matrix) {
    if (indices.empty()) return (0);

    // Initialize to 0
    covariance_matrix.setZero();

    size_t point_count;
    // If the data is dense, we don't need to check for NaN
    if (cloud.is_dense) {
        point_count = indices.size();
        // For each point in the cloud
        for (size_t i = 0; i < point_count; ++i) {
            Eigen::Matrix<Scalar, 4, 1> pt;
            pt[0] = cloud[indices[i]].x - centroid[0];
            pt[1] = cloud[indices[i]].y - centroid[1];
            pt[2] = cloud[indices[i]].z - centroid[2];

            covariance_matrix(1, 1) += pt.y() * pt.y();
            covariance_matrix(1, 2) += pt.y() * pt.z();

            covariance_matrix(2, 2) += pt.z() * pt.z();

            pt *= pt.x();
            covariance_matrix(0, 0) += pt.x();
            covariance_matrix(0, 1) += pt.y();
            covariance_matrix(0, 2) += pt.z();
        }
    }
    // NaN or Inf values could exist => check for them
    else {
        point_count = 0;
        // For each point in the cloud
        for (const int& index : indices) {
            // Check if the point is invalid
            if (!isFinite(cloud[index])) continue;

            Eigen::Matrix<Scalar, 4, 1> pt;
            pt[0] = cloud[index].x - centroid[0];
            pt[1] = cloud[index].y - centroid[1];
            pt[2] = cloud[index].z - centroid[2];

            covariance_matrix(1, 1) += pt.y() * pt.y();
            covariance_matrix(1, 2) += pt.y() * pt.z();

            covariance_matrix(2, 2) += pt.z() * pt.z();

            pt *= pt.x();
            covariance_matrix(0, 0) += pt.x();
            covariance_matrix(0, 1) += pt.y();
            covariance_matrix(0, 2) += pt.z();
            ++point_count;
        }
    }
    covariance_matrix(1, 0) = covariance_matrix(0, 1);
    covariance_matrix(2, 0) = covariance_matrix(0, 2);
    covariance_matrix(2, 1) = covariance_matrix(1, 2);
    return (static_cast<unsigned int>(point_count));
}

//////////////////////////////////////////////////////////////////////////////////////////////
template <typename PointT, typename Scalar>
inline unsigned int pcl::computeCovarianceMatrix(const pcl::PointCloud<PointT>& cloud, const pcl::PointIndices& indices,
    const Eigen::Matrix<Scalar, 4, 1>& centroid, Eigen::Matrix<Scalar, 3, 3>& covariance_matrix) {
    return (pcl::computeCovarianceMatrix(cloud, indices.indices, centroid, covariance_matrix));
}

//////////////////////////////////////////////////////////////////////////////////////////////
template <typename PointT, typename Scalar>
inline unsigned int pcl::computeCovarianceMatrixNormalized(const pcl::PointCloud<PointT>& cloud,
    const std::vector<int>& indices, const Eigen::Matrix<Scalar, 4, 1>& centroid,
    Eigen::Matrix<Scalar, 3, 3>& covariance_matrix) {
    unsigned point_count = pcl::computeCovarianceMatrix(cloud, indices, centroid, covariance_matrix);
    if (point_count != 0) covariance_matrix /= static_cast<Scalar>(point_count);

    return (point_count);
}

//////////////////////////////////////////////////////////////////////////////////////////////
template <typename PointT, typename Scalar>
inline unsigned int pcl::computeCovarianceMatrixNormalized(const pcl::PointCloud<PointT>& cloud,
    const pcl::PointIndices& indices, const Eigen::Matrix<Scalar, 4, 1>& centroid,
    Eigen::Matrix<Scalar, 3, 3>& covariance_matrix) {
    unsigned int point_count = pcl::computeCovarianceMatrix(cloud, indices.indices, centroid, covariance_matrix);
    if (point_count != 0) covariance_matrix /= static_cast<Scalar>(point_count);

    return point_count;
}

//////////////////////////////////////////////////////////////////////////////////////////////
template <typename PointT, typename Scalar>
inline unsigned int pcl::computeCovarianceMatrix(
    const pcl::PointCloud<PointT>& cloud, Eigen::Matrix<Scalar, 3, 3>& covariance_matrix) {
    // create the buffer on the stack which is much faster than using cloud[indices[i]] and centroid as a buffer
    Eigen::Matrix<Scalar, 1, 6, Eigen::RowMajor> accu = Eigen::Matrix<Scalar, 1, 6, Eigen::RowMajor>::Zero();

    unsigned int point_count;
    if (cloud.is_dense) {
        point_count = static_cast<unsigned int>(cloud.size());
        // For each point in the cloud
        for (size_t i = 0; i < point_count; ++i) {
            accu[0] += cloud[i].x * cloud[i].x;
            accu[1] += cloud[i].x * cloud[i].y;
            accu[2] += cloud[i].x * cloud[i].z;
            accu[3] += cloud[i].y * cloud[i].y;
            accu[4] += cloud[i].y * cloud[i].z;
            accu[5] += cloud[i].z * cloud[i].z;
        }
    } else {
        point_count = 0;
        for (size_t i = 0; i < cloud.size(); ++i) {
            if (!isFinite(cloud[i])) continue;

            accu[0] += cloud[i].x * cloud[i].x;
            accu[1] += cloud[i].x * cloud[i].y;
            accu[2] += cloud[i].x * cloud[i].z;
            accu[3] += cloud[i].y * cloud[i].y;
            accu[4] += cloud[i].y * cloud[i].z;
            accu[5] += cloud[i].z * cloud[i].z;
            ++point_count;
        }
    }

    if (point_count != 0) {
        accu /= static_cast<Scalar>(point_count);
        covariance_matrix.coeffRef(0) = accu[0];
        covariance_matrix.coeffRef(1) = covariance_matrix.coeffRef(3) = accu[1];
        covariance_matrix.coeffRef(2) = covariance_matrix.coeffRef(6) = accu[2];
        covariance_matrix.coeffRef(4) = accu[3];
        covariance_matrix.coeffRef(5) = covariance_matrix.coeffRef(7) = accu[4];
        covariance_matrix.coeffRef(8) = accu[5];
    }
    return (point_count);
}

//////////////////////////////////////////////////////////////////////////////////////////////
template <typename PointT, typename Scalar>
inline unsigned int pcl::computeCovarianceMatrix(const pcl::PointCloud<PointT>& cloud, const std::vector<int>& indices,
    Eigen::Matrix<Scalar, 3, 3>& covariance_matrix) {
    // create the buffer on the stack which is much faster than using cloud[indices[i]] and centroid as a buffer
    Eigen::Matrix<Scalar, 1, 6, Eigen::RowMajor> accu = Eigen::Matrix<Scalar, 1, 6, Eigen::RowMajor>::Zero();

    unsigned int point_count;
    if (cloud.is_dense) {
        point_count = static_cast<unsigned int>(indices.size());
        for (const int& index : indices) {
            // const PointT& point = cloud[*iIt];
            accu[0] += cloud[index].x * cloud[index].x;
            accu[1] += cloud[index].x * cloud[index].y;
            accu[2] += cloud[index].x * cloud[index].z;
            accu[3] += cloud[index].y * cloud[index].y;
            accu[4] += cloud[index].y * cloud[index].z;
            accu[5] += cloud[index].z * cloud[index].z;
        }
    } else {
        point_count = 0;
        for (const int& index : indices) {
            if (!isFinite(cloud[index])) continue;

            ++point_count;
            accu[0] += cloud[index].x * cloud[index].x;
            accu[1] += cloud[index].x * cloud[index].y;
            accu[2] += cloud[index].x * cloud[index].z;
            accu[3] += cloud[index].y * cloud[index].y;
            accu[4] += cloud[index].y * cloud[index].z;
            accu[5] += cloud[index].z * cloud[index].z;
        }
    }
    if (point_count != 0) {
        accu /= static_cast<Scalar>(point_count);
        covariance_matrix.coeffRef(0) = accu[0];
        covariance_matrix.coeffRef(1) = covariance_matrix.coeffRef(3) = accu[1];
        covariance_matrix.coeffRef(2) = covariance_matrix.coeffRef(6) = accu[2];
        covariance_matrix.coeffRef(4) = accu[3];
        covariance_matrix.coeffRef(5) = covariance_matrix.coeffRef(7) = accu[4];
        covariance_matrix.coeffRef(8) = accu[5];
    }
    return (point_count);
}

//////////////////////////////////////////////////////////////////////////////////////////////
template <typename PointT, typename Scalar>
inline unsigned int pcl::computeCovarianceMatrix(const pcl::PointCloud<PointT>& cloud, const pcl::PointIndices& indices,
    Eigen::Matrix<Scalar, 3, 3>& covariance_matrix) {
    return (computeCovarianceMatrix(cloud, indices.indices, covariance_matrix));
}

//////////////////////////////////////////////////////////////////////////////////////////////
template <typename PointT, typename Scalar>
inline unsigned int pcl::computeMeanAndCovarianceMatrix(const pcl::PointCloud<PointT>& cloud,
    Eigen::Matrix<Scalar, 3, 3>& covariance_matrix, Eigen::Matrix<Scalar, 4, 1>& centroid) {
    // create the buffer on the stack which is much faster than using cloud[indices[i]] and centroid as a buffer
    Eigen::Matrix<Scalar, 1, 9, Eigen::RowMajor> accu = Eigen::Matrix<Scalar, 1, 9, Eigen::RowMajor>::Zero();
    size_t point_count;
    if (cloud.is_dense) {
        point_count = cloud.size();
        // For each point in the cloud
        for (size_t i = 0; i < point_count; ++i) {
            accu[0] += cloud[i].x * cloud[i].x;
            accu[1] += cloud[i].x * cloud[i].y;
            accu[2] += cloud[i].x * cloud[i].z;
            accu[3] += cloud[i].y * cloud[i].y; // 4
            accu[4] += cloud[i].y * cloud[i].z; // 5
            accu[5] += cloud[i].z * cloud[i].z; // 8
            accu[6] += cloud[i].x;
            accu[7] += cloud[i].y;
            accu[8] += cloud[i].z;
        }
    } else {
        point_count = 0;
        for (size_t i = 0; i < cloud.size(); ++i) {
            if (!isFinite(cloud[i])) continue;

            accu[0] += cloud[i].x * cloud[i].x;
            accu[1] += cloud[i].x * cloud[i].y;
            accu[2] += cloud[i].x * cloud[i].z;
            accu[3] += cloud[i].y * cloud[i].y;
            accu[4] += cloud[i].y * cloud[i].z;
            accu[5] += cloud[i].z * cloud[i].z;
            accu[6] += cloud[i].x;
            accu[7] += cloud[i].y;
            accu[8] += cloud[i].z;
            ++point_count;
        }
    }
    accu /= static_cast<Scalar>(point_count);
    if (point_count != 0) {
        // centroid.head<3> () = accu.tail<3> ();    -- does not compile with Clang 3.0
        centroid[0] = accu[6];
        centroid[1] = accu[7];
        centroid[2] = accu[8];
        centroid[3] = 1;
        covariance_matrix.coeffRef(0) = accu[0] - accu[6] * accu[6];
        covariance_matrix.coeffRef(1) = accu[1] - accu[6] * accu[7];
        covariance_matrix.coeffRef(2) = accu[2] - accu[6] * accu[8];
        covariance_matrix.coeffRef(4) = accu[3] - accu[7] * accu[7];
        covariance_matrix.coeffRef(5) = accu[4] - accu[7] * accu[8];
        covariance_matrix.coeffRef(8) = accu[5] - accu[8] * accu[8];
        covariance_matrix.coeffRef(3) = covariance_matrix.coeff(1);
        covariance_matrix.coeffRef(6) = covariance_matrix.coeff(2);
        covariance_matrix.coeffRef(7) = covariance_matrix.coeff(5);
    }
    return (static_cast<unsigned int>(point_count));
}

//////////////////////////////////////////////////////////////////////////////////////////////
template <typename PointT, typename Scalar>
inline unsigned int pcl::computeMeanAndCovarianceMatrix(const pcl::PointCloud<PointT>& cloud,
    const std::vector<int>& indices, Eigen::Matrix<Scalar, 3, 3>& covariance_matrix,
    Eigen::Matrix<Scalar, 4, 1>& centroid) {
    // create the buffer on the stack which is much faster than using cloud[indices[i]] and centroid as a buffer
    Eigen::Matrix<Scalar, 1, 9, Eigen::RowMajor> accu = Eigen::Matrix<Scalar, 1, 9, Eigen::RowMajor>::Zero();
    size_t point_count;
    if (cloud.is_dense) {
        point_count = indices.size();
        for (const int& index : indices) {
            // const PointT& point = cloud[*iIt];
            accu[0] += cloud[index].x * cloud[index].x;
            accu[1] += cloud[index].x * cloud[index].y;
            accu[2] += cloud[index].x * cloud[index].z;
            accu[3] += cloud[index].y * cloud[index].y;
            accu[4] += cloud[index].y * cloud[index].z;
            accu[5] += cloud[index].z * cloud[index].z;
            accu[6] += cloud[index].x;
            accu[7] += cloud[index].y;
            accu[8] += cloud[index].z;
        }
    } else {
        point_count = 0;
        for (const int& index : indices) {
            if (!isFinite(cloud[index])) continue;

            ++point_count;
            accu[0] += cloud[index].x * cloud[index].x;
            accu[1] += cloud[index].x * cloud[index].y;
            accu[2] += cloud[index].x * cloud[index].z;
            accu[3] += cloud[index].y * cloud[index].y; // 4
            accu[4] += cloud[index].y * cloud[index].z; // 5
            accu[5] += cloud[index].z * cloud[index].z; // 8
            accu[6] += cloud[index].x;
            accu[7] += cloud[index].y;
            accu[8] += cloud[index].z;
        }
    }

    accu /= static_cast<Scalar>(point_count);
    // Eigen::Vector3f vec = accu.tail<3> ();
    // centroid.head<3> () = vec;//= accu.tail<3> ();
    // centroid.head<3> () = accu.tail<3> ();    -- does not compile with Clang 3.0
    centroid[0] = accu[6];
    centroid[1] = accu[7];
    centroid[2] = accu[8];
    centroid[3] = 1;
    covariance_matrix.coeffRef(0) = accu[0] - accu[6] * accu[6];
    covariance_matrix.coeffRef(1) = accu[1] - accu[6] * accu[7];
    covariance_matrix.coeffRef(2) = accu[2] - accu[6] * accu[8];
    covariance_matrix.coeffRef(4) = accu[3] - accu[7] * accu[7];
    covariance_matrix.coeffRef(5) = accu[4] - accu[7] * accu[8];
    covariance_matrix.coeffRef(8) = accu[5] - accu[8] * accu[8];
    covariance_matrix.coeffRef(3) = covariance_matrix.coeff(1);
    covariance_matrix.coeffRef(6) = covariance_matrix.coeff(2);
    covariance_matrix.coeffRef(7) = covariance_matrix.coeff(5);

    return (static_cast<unsigned int>(point_count));
}

//////////////////////////////////////////////////////////////////////////////////////////////
template <typename PointT, typename Scalar>
inline unsigned int pcl::computeMeanAndCovarianceMatrix(const pcl::PointCloud<PointT>& cloud,
    const pcl::PointIndices& indices, Eigen::Matrix<Scalar, 3, 3>& covariance_matrix,
    Eigen::Matrix<Scalar, 4, 1>& centroid) {
    return (computeMeanAndCovarianceMatrix(cloud, indices.indices, covariance_matrix, centroid));
}


//////////////////////////////////////////////////////////////////////////////////////////////
template <typename PointT, typename Scalar>
void pcl::demeanPointCloud(const pcl::PointCloud<PointT>& cloud_in, const Eigen::Matrix<Scalar, 4, 1>& centroid,
    pcl::PointCloud<PointT>& cloud_out) {
    cloud_out = cloud_in;

    // Subtract the centroid from cloud_in
    for (size_t i = 0; i < cloud_in.points.size(); ++i) {
        cloud_out.points[i].x -= static_cast<float>(centroid[0]);
        cloud_out.points[i].y -= static_cast<float>(centroid[1]);
        cloud_out.points[i].z -= static_cast<float>(centroid[2]);
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////
template <typename PointT, typename Scalar>
void pcl::demeanPointCloud(const pcl::PointCloud<PointT>& cloud_in, const std::vector<int>& indices,
    const Eigen::Matrix<Scalar, 4, 1>& centroid, pcl::PointCloud<PointT>& cloud_out) {
    cloud_out.header = cloud_in.header;
    cloud_out.is_dense = cloud_in.is_dense;
    if (indices.size() == cloud_in.points.size()) {
        cloud_out.width = cloud_in.width;
        cloud_out.height = cloud_in.height;
    } else {
        cloud_out.width = static_cast<uint32_t>(indices.size());
        cloud_out.height = 1;
    }
    cloud_out.resize(indices.size());

    // Subtract the centroid from cloud_in
    for (size_t i = 0; i < indices.size(); ++i) {
        cloud_out[i].x = static_cast<float>(cloud_in[indices[i]].x - centroid[0]);
        cloud_out[i].y = static_cast<float>(cloud_in[indices[i]].y - centroid[1]);
        cloud_out[i].z = static_cast<float>(cloud_in[indices[i]].z - centroid[2]);
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////
template <typename PointT, typename Scalar>
void pcl::demeanPointCloud(const pcl::PointCloud<PointT>& cloud_in, const pcl::PointIndices& indices,
    const Eigen::Matrix<Scalar, 4, 1>& centroid, pcl::PointCloud<PointT>& cloud_out) {
    return (demeanPointCloud(cloud_in, indices.indices, centroid, cloud_out));
}


//////////////////////////////////////////////////////////////////////////////////////////////
template <typename PointT, typename Scalar>
void pcl::demeanPointCloud(const pcl::PointCloud<PointT>& cloud_in, const Eigen::Matrix<Scalar, 4, 1>& centroid,
    Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic>& cloud_out) {
    size_t npts = cloud_in.size();

    cloud_out = Eigen::Matrix<Scalar, 4, Eigen::Dynamic>::Zero(4, npts); // keep the data aligned

    for (size_t i = 0; i < npts; ++i) {
        cloud_out(0, i) = cloud_in[i].x - centroid[0];
        cloud_out(1, i) = cloud_in[i].y - centroid[1];
        cloud_out(2, i) = cloud_in[i].z - centroid[2];
        // One column at a time
        // cloud_out.block<4, 1> (0, i) = cloud_in.points[i].getVector4fMap () - centroid;
    }

    // Make sure we zero the 4th dimension out (1 row, N columns)
    // cloud_out.block (3, 0, 1, npts).setZero ();
}

//////////////////////////////////////////////////////////////////////////////////////////////
template <typename PointT, typename Scalar>
void pcl::demeanPointCloud(const pcl::PointCloud<PointT>& cloud_in, const std::vector<int>& indices,
    const Eigen::Matrix<Scalar, 4, 1>& centroid, Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic>& cloud_out) {
    size_t npts = indices.size();

    cloud_out = Eigen::Matrix<Scalar, 4, Eigen::Dynamic>::Zero(4, npts); // keep the data aligned

    for (size_t i = 0; i < npts; ++i) {
        cloud_out(0, i) = cloud_in[indices[i]].x - centroid[0];
        cloud_out(1, i) = cloud_in[indices[i]].y - centroid[1];
        cloud_out(2, i) = cloud_in[indices[i]].z - centroid[2];
        // One column at a time
        // cloud_out.block<4, 1> (0, i) = cloud_in.points[indices[i]].getVector4fMap () - centroid;
    }

    // Make sure we zero the 4th dimension out (1 row, N columns)
    // cloud_out.block (3, 0, 1, npts).setZero ();
}

//////////////////////////////////////////////////////////////////////////////////////////////
template <typename PointT, typename Scalar>
void pcl::demeanPointCloud(const pcl::PointCloud<PointT>& cloud_in, const pcl::PointIndices& indices,
    const Eigen::Matrix<Scalar, 4, 1>& centroid, Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic>& cloud_out) {
    return (pcl::demeanPointCloud(cloud_in, indices.indices, centroid, cloud_out));
}


/////////////////////////////////////////////////////////////////////////////////////////////
template <typename PointT, typename Scalar>
inline void pcl::computeNDCentroid(const pcl::PointCloud<PointT>& cloud, const pcl::PointIndices& indices,
    Eigen::Matrix<Scalar, Eigen::Dynamic, 1>& centroid) {
    return (pcl::computeNDCentroid(cloud, indices.indices, centroid));
}

