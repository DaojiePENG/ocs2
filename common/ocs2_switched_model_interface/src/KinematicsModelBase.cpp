/*
 * KinematicsModelBase.h
 *
 *  Created on: Aug 3, 2017
 *      Author: Farbod
 */

#include "ocs2_switched_model_interface/core/KinematicsModelBase.h"

#include <ocs2_switched_model_interface/core/Rotations.h>

namespace switched_model {

/******************************************************************************************************/
/******************************************************************************************************/
/******************************************************************************************************/
template <typename SCALAR_T>
std::array<vector3_s_t<SCALAR_T>, NUM_CONTACT_POINTS> KinematicsModelBase<SCALAR_T>::positionBaseToFeetInBaseFrame(
    const joint_coordinate_s_t<SCALAR_T>& jointPositions) const {
  std::array<vector3_s_t<SCALAR_T>, NUM_CONTACT_POINTS> baseToFeetPositions;
  for (size_t i = 0; i < NUM_CONTACT_POINTS; i++) {
    baseToFeetPositions[i] = positionBaseToFootInBaseFrame(i, jointPositions);
  }
  return baseToFeetPositions;
}

/******************************************************************************************************/
/******************************************************************************************************/
/******************************************************************************************************/
template <typename SCALAR_T>
vector3_s_t<SCALAR_T> KinematicsModelBase<SCALAR_T>::footPositionInOriginFrame(size_t footIndex,
                                                                               const base_coordinate_s_t<SCALAR_T>& basePose,
                                                                               const joint_coordinate_s_t<SCALAR_T>& jointPositions) const {
  matrix3_s_t<SCALAR_T> o_R_b = rotationMatrixBaseToOrigin<SCALAR_T>(getOrientation(basePose));
  vector3_s_t<SCALAR_T> o_basePosition = getPositionInOrigin(basePose);

  vector3_s_t<SCALAR_T> b_baseToFoot = positionBaseToFootInBaseFrame(footIndex, jointPositions);
  return o_R_b * b_baseToFoot + o_basePosition;
}

/******************************************************************************************************/
/******************************************************************************************************/
/******************************************************************************************************/
template <typename SCALAR_T>
matrix3_s_t<SCALAR_T> KinematicsModelBase<SCALAR_T>::footOrientationInOriginFrame(
    size_t footIndex, const base_coordinate_s_t<SCALAR_T>& basePose, const joint_coordinate_s_t<SCALAR_T>& jointPositions) const {
  matrix3_s_t<SCALAR_T> o_R_b = rotationMatrixBaseToOrigin<SCALAR_T>(getOrientation(basePose));
  return o_R_b * footOrientationInBaseFrame(footIndex, jointPositions);
}

/******************************************************************************************************/
/******************************************************************************************************/
/******************************************************************************************************/
template <typename SCALAR_T>
std::array<vector3_s_t<SCALAR_T>, NUM_CONTACT_POINTS> KinematicsModelBase<SCALAR_T>::feetPositionsInOriginFrame(
    const base_coordinate_s_t<SCALAR_T>& basePoseInOriginFrame, const joint_coordinate_s_t<SCALAR_T>& jointPositions) const {
  matrix3_s_t<SCALAR_T> o_R_b = rotationMatrixBaseToOrigin<SCALAR_T>(getOrientation(basePoseInOriginFrame));
  vector3_s_t<SCALAR_T> o_basePosition = getPositionInOrigin(basePoseInOriginFrame);

  std::array<vector3_s_t<SCALAR_T>, 4> feetPositionsInOriginFrame;
  for (size_t i = 0; i < NUM_CONTACT_POINTS; i++) {
    vector3_s_t<SCALAR_T> b_baseToFoot = positionBaseToFootInBaseFrame(i, jointPositions);
    feetPositionsInOriginFrame[i] = o_R_b * b_baseToFoot + o_basePosition;
  }
  return feetPositionsInOriginFrame;
}

///******************************************************************************************************/
///******************************************************************************************************/
///******************************************************************************************************/
template <typename SCALAR_T>
vector3_s_t<SCALAR_T> KinematicsModelBase<SCALAR_T>::footVelocityRelativeToBaseInBaseFrame(
    size_t footIndex, const joint_coordinate_s_t<SCALAR_T>& jointPositions, const joint_coordinate_s_t<SCALAR_T>& jointVelocities) const {
  joint_jacobian_t b_baseToFootJacobian = baseToFootJacobianInBaseFrame(footIndex, jointPositions);
  return b_baseToFootJacobian.template bottomRows<3>() * jointVelocities;
}

///******************************************************************************************************/
///******************************************************************************************************/
///******************************************************************************************************/
template <typename SCALAR_T>
vector3_s_t<SCALAR_T> KinematicsModelBase<SCALAR_T>::footVelocityInBaseFrame(size_t footIndex,
                                                                             const base_coordinate_s_t<SCALAR_T>& baseTwistInBaseFrame,
                                                                             const joint_coordinate_s_t<SCALAR_T>& jointPositions,
                                                                             const joint_coordinate_s_t<SCALAR_T>& jointVelocities) const {
  vector3_s_t<SCALAR_T> b_footRelativeVelocity = footVelocityRelativeToBaseInBaseFrame(footIndex, jointPositions, jointVelocities);
  vector3_s_t<SCALAR_T> b_baseToFoot = positionBaseToFootInBaseFrame(footIndex, jointPositions);
  return b_footRelativeVelocity + getLinearVelocity(baseTwistInBaseFrame) + getAngularVelocity(baseTwistInBaseFrame).cross(b_baseToFoot);
}

///******************************************************************************************************/
///******************************************************************************************************/
///******************************************************************************************************/
template <typename SCALAR_T>
vector3_s_t<SCALAR_T> KinematicsModelBase<SCALAR_T>::footVelocityInOriginFrame(
    size_t footIndex, const base_coordinate_s_t<SCALAR_T>& basePoseInOriginFrame, const base_coordinate_s_t<SCALAR_T>& baseTwistInBaseFrame,
    const joint_coordinate_s_t<SCALAR_T>& jointPositions, const joint_coordinate_s_t<SCALAR_T>& jointVelocities) const {
  vector3_s_t<SCALAR_T> b_footVelocity = footVelocityInBaseFrame(footIndex, baseTwistInBaseFrame, jointPositions, jointVelocities);
  matrix3_s_t<SCALAR_T> o_R_b = rotationMatrixBaseToOrigin<SCALAR_T>(getOrientation(basePoseInOriginFrame));
  return o_R_b * b_footVelocity;
}

///******************************************************************************************************/
///******************************************************************************************************/
///******************************************************************************************************/
template <typename SCALAR_T>
vector3_s_t<SCALAR_T> KinematicsModelBase<SCALAR_T>::footVelocityInFootFrame(size_t footIndex,
                                                                             const base_coordinate_s_t<SCALAR_T>& baseTwistInBaseFrame,
                                                                             const joint_coordinate_s_t<SCALAR_T>& jointPositions,
                                                                             const joint_coordinate_s_t<SCALAR_T>& jointVelocities) const {
  vector3_s_t<SCALAR_T> b_footVelocity = footVelocityInBaseFrame(footIndex, baseTwistInBaseFrame, jointPositions, jointVelocities);
  matrix3_s_t<SCALAR_T> foot_R_b = footOrientationInBaseFrame(footIndex, jointPositions).transpose();
  return foot_R_b * b_footVelocity;
}

///******************************************************************************************************/
///******************************************************************************************************/
///******************************************************************************************************/
template <typename SCALAR_T>
std::array<vector3_s_t<SCALAR_T>, NUM_CONTACT_POINTS> KinematicsModelBase<SCALAR_T>::feetVelocitiesInOriginFrame(
    const base_coordinate_s_t<SCALAR_T>& basePoseInOriginFrame, const base_coordinate_s_t<SCALAR_T>& baseTwistInBaseFrame,
    const joint_coordinate_s_t<SCALAR_T>& jointPositions, const joint_coordinate_s_t<SCALAR_T>& jointVelocities) const {
  matrix3_s_t<SCALAR_T> o_R_b = rotationMatrixBaseToOrigin<SCALAR_T>(getOrientation(basePoseInOriginFrame));
  vector3_s_t<SCALAR_T> o_basePosition = getPositionInOrigin(basePoseInOriginFrame);

  std::array<vector3_s_t<SCALAR_T>, 4> feetVelocitiesInOriginFrame;
  for (size_t i = 0; i < NUM_CONTACT_POINTS; i++) {
    feetVelocitiesInOriginFrame[i] = o_R_b * footVelocityInBaseFrame(i, baseTwistInBaseFrame, jointPositions, jointVelocities);
  }
  return feetVelocitiesInOriginFrame;
}

template class KinematicsModelBase<double>;
template class KinematicsModelBase<ocs2::CppAdInterface<double>::ad_scalar_t>;

}  // end of namespace switched_model
