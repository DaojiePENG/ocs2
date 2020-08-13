//
// Created by rgrandia on 22.10.19.
//

#pragma once

#include <Eigen/Dense>
#include <Eigen/Geometry>
#include <cmath>

namespace switched_model {

/**
 * Base to origin rotation matrix
 * @tparam Derived
 * @param [in] eulerAngles
 * @return
 */
template <typename SCALAR_T>
Eigen::Matrix<SCALAR_T, 3, 3> rotationMatrixBaseToOrigin(const Eigen::Matrix<SCALAR_T, 3, 1>& eulerAnglesXYZ) {
  // Generated code for:
  //  // inputs are the intrinsic rotation angles in RADIANTS
  //  SCALAR_T sinAlpha = sin(eulerAngles(0));
  //  SCALAR_T cosAlpha = cos(eulerAngles(0));
  //  SCALAR_T sinBeta = sin(eulerAngles(1));
  //  SCALAR_T cosBeta = cos(eulerAngles(1));
  //  SCALAR_T sinGamma = sin(eulerAngles(2));
  //  SCALAR_T cosGamma = cos(eulerAngles(2));
  //
  //  Eigen::Matrix<SCALAR_T, 3, 3> Rx, Ry, Rz;
  //  Rx << SCALAR_T(1), SCALAR_T(0), SCALAR_T(0), SCALAR_T(0), cosAlpha, -sinAlpha, SCALAR_T(0), sinAlpha, cosAlpha;
  //  Ry << cosBeta, SCALAR_T(0), sinBeta, SCALAR_T(0), SCALAR_T(1), SCALAR_T(0), -sinBeta, SCALAR_T(0), cosBeta;
  //  Rz << cosGamma, -sinGamma, SCALAR_T(0), sinGamma, cosGamma, SCALAR_T(0), SCALAR_T(0), SCALAR_T(0), SCALAR_T(1);
  //
  //  return Rx * Ry * Rz;
  Eigen::Matrix<SCALAR_T, 3, 3> o_R_b;

  // auxiliary variables
  std::array<SCALAR_T, 8> v{};

  v[0] = cos(eulerAnglesXYZ[1]);
  v[1] = cos(eulerAnglesXYZ[2]);
  o_R_b(0) = v[0] * v[1];
  v[2] = sin(eulerAnglesXYZ[0]);
  v[3] = -v[2];
  o_R_b(6) = sin(eulerAnglesXYZ[1]);
  v[4] = -o_R_b(6);
  v[5] = v[3] * v[4];
  v[6] = cos(eulerAnglesXYZ[0]);
  v[7] = sin(eulerAnglesXYZ[2]);
  o_R_b(1) = v[5] * v[1] + v[6] * v[7];
  v[4] = v[6] * v[4];
  o_R_b(2) = v[4] * v[1] + v[2] * v[7];
  v[7] = -v[7];
  o_R_b(3) = v[0] * v[7];
  o_R_b(4) = v[5] * v[7] + v[6] * v[1];
  o_R_b(5) = v[4] * v[7] + v[2] * v[1];
  o_R_b(7) = v[3] * v[0];
  o_R_b(8) = v[6] * v[0];
  return o_R_b;
}

template <typename SCALAR_T>
Eigen::Matrix<SCALAR_T, 3, 3> rotationMatrixOriginToBase(const Eigen::Matrix<SCALAR_T, 3, 1>& eulerAngles) {
  return rotationMatrixBaseToOrigin<SCALAR_T>(eulerAngles).transpose();
}

template <typename SCALAR_T>
Eigen::Quaternion<SCALAR_T> quaternionBaseToOrigin(const Eigen::Matrix<SCALAR_T, 3, 1>& eulerAngles) {
  const auto roll = eulerAngles(0);
  const auto pitch = eulerAngles(1);
  const auto yaw = eulerAngles(2);
  return Eigen::AngleAxis<SCALAR_T>(roll, Eigen::Vector3d::UnitX()) * Eigen::AngleAxis<SCALAR_T>(pitch, Eigen::Vector3d::UnitY()) *
         Eigen::AngleAxis<SCALAR_T>(yaw, Eigen::Vector3d::UnitZ());
}

template <typename SCALAR_T>
Eigen::Matrix<SCALAR_T, 3, 1> eulerAnglesFromQuaternionBaseToOrigin(const Eigen::Quaternion<SCALAR_T>& q_origin_base) {
  return q_origin_base.toRotationMatrix().eulerAngles(0, 1, 2);
}

/**
 * Calculates the skew matrix for vector cross product
 * @tparam Derived
 * @param [in] in
 * @return
 */
template <typename SCALAR_T, typename Derived>
Eigen::Matrix<SCALAR_T, 3, 3> crossProductMatrix(const Eigen::DenseBase<Derived>& in) {
  if (in.innerSize() != 3 || in.outerSize() != 1) {
    throw std::runtime_error("Input argument should be a 3-by-1 vector.");
  }

  Eigen::Matrix<SCALAR_T, 3, 3> out;
  out << SCALAR_T(0.0), -in(2), +in(1), +in(2), SCALAR_T(0.0), -in(0), -in(1), +in(0), SCALAR_T(0.0);
  return out;
}

/**
 * Computes the matrix which transforms derivatives of angular velocities in the body frame to euler angles derivatives
 * WARNING: matrix is singular when rotation around y axis is +/- 90 degrees
 * @param[in] eulerAngles: euler angles in xyz convention
 * @return M: matrix that does the transformation
 */
template <typename SCALAR_T>
Eigen::Matrix<SCALAR_T, 3, 3> angularVelocitiesToEulerAngleDerivativesMatrix(const Eigen::Matrix<SCALAR_T, 3, 1>& eulerAngles) {
  Eigen::Matrix<SCALAR_T, 3, 3> M;
  SCALAR_T sinPsi = sin(eulerAngles(2));
  SCALAR_T cosPsi = cos(eulerAngles(2));
  SCALAR_T sinTheta = sin(eulerAngles(1));
  SCALAR_T cosTheta = cos(eulerAngles(1));

  M << cosPsi / cosTheta, -sinPsi / cosTheta, SCALAR_T(0), sinPsi, cosPsi, SCALAR_T(0), -cosPsi * sinTheta / cosTheta,
      sinTheta * sinPsi / cosTheta, SCALAR_T(1);

  return M;
}

/**
 * Computes the derivative w.r.t eulerXYZ of rotating a vector from base to origin : d/d(euler) (o_R_b(euler) * v_base)
 *
 * @param[in] eulerAnglesXYZ: euler angles in xyz convention.
 * @param[in] vectorInBase: the vector in base that is rotated.
 * @return d/d(euler) (o_R_b(euler) * v_base)
 */
template <typename SCALAR_T>
Eigen::Matrix<SCALAR_T, 3, 3> rotationBaseToOriginJacobian(const Eigen::Matrix<SCALAR_T, 3, 1>& eulerAnglesXYZ,
                                                           const Eigen::Matrix<SCALAR_T, 3, 1>& vectorInBase) {
  // dependent variables
  Eigen::Matrix<SCALAR_T, 3, 3> jac;

  // auxiliary variables
  std::array<SCALAR_T, 15> v{};

  jac(0) = SCALAR_T(0.0);
  v[0] = cos(eulerAnglesXYZ[1]);
  v[1] = -SCALAR_T(1.0) * sin(eulerAnglesXYZ[1]);
  v[2] = sin(eulerAnglesXYZ[2]);
  v[3] = -v[2];
  v[4] = cos(eulerAnglesXYZ[2]);
  jac(3) = v[0] * vectorInBase[2] + v[1] * v[3] * vectorInBase[1] + v[1] * v[4] * vectorInBase[0];
  v[5] = cos(eulerAnglesXYZ[1]);
  v[6] = cos(eulerAnglesXYZ[2]);
  v[7] = -v[6];
  v[8] = -SCALAR_T(1.0) * sin(eulerAnglesXYZ[2]);
  jac(6) = v[5] * v[7] * vectorInBase[1] + v[5] * v[8] * vectorInBase[0];
  v[9] = cos(eulerAnglesXYZ[0]);
  v[10] = -v[9];
  v[11] = -sin(eulerAnglesXYZ[1]);
  v[12] = v[10] * v[11];
  v[13] = -SCALAR_T(1.0) * sin(eulerAnglesXYZ[0]);
  jac(1) =
      v[10] * v[5] * vectorInBase[2] + (v[12] * v[3] + v[13] * v[4]) * vectorInBase[1] + (v[12] * v[4] + v[13] * v[2]) * vectorInBase[0];
  v[12] = sin(eulerAnglesXYZ[0]);
  v[10] = -v[12];
  v[0] = -v[0];
  v[14] = v[10] * v[0];
  jac(4) = v[10] * v[1] * vectorInBase[2] + v[14] * v[3] * vectorInBase[1] + v[14] * v[4] * vectorInBase[0];
  v[10] = v[10] * v[11];
  v[14] = cos(eulerAnglesXYZ[0]);
  jac(7) = (v[10] * v[7] + v[14] * v[8]) * vectorInBase[1] + (v[10] * v[8] + v[14] * v[6]) * vectorInBase[0];
  v[10] = v[13] * v[11];
  jac(2) = v[13] * v[5] * vectorInBase[2] + (v[10] * v[3] + v[9] * v[4]) * vectorInBase[1] + (v[10] * v[4] + v[9] * v[2]) * vectorInBase[0];
  v[0] = v[14] * v[0];
  jac(5) = v[14] * v[1] * vectorInBase[2] + v[0] * v[3] * vectorInBase[1] + v[0] * v[4] * vectorInBase[0];
  v[14] = v[14] * v[11];
  jac(8) = (v[14] * v[7] + v[12] * v[8]) * vectorInBase[1] + (v[14] * v[8] + v[12] * v[6]) * vectorInBase[0];
  return jac;
}

}  // namespace switched_model
