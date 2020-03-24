/*
 * AnymalWheelsKinematics.h
 *
 *  Created on: Nov 25, 2019
 *      Author: Marko Bjelonic
 */

#pragma once

#include <ocs2_switched_model_interface/core/KinematicsModelBase.h>

namespace anymal {
namespace tpl {

template <typename SCALAR_T>
class AnymalWheelsKinematics final : public switched_model::KinematicsModelBase<SCALAR_T> {
  const SCALAR_T wheelRadius_ = SCALAR_T(0.095);  // [m]

 public:
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW

  typedef switched_model::KinematicsModelBase<SCALAR_T> BASE;
  using typename BASE::joint_jacobian_t;

  enum { LF = 0, RF = 1, LH = 2, RH = 3 };

  AnymalWheelsKinematics() = default;

  ~AnymalWheelsKinematics() = default;

  AnymalWheelsKinematics<SCALAR_T>* clone() const override;

  switched_model::vector3_s_t<SCALAR_T> positionBaseToFootInBaseFrame(
      size_t footIndex, const switched_model::joint_coordinate_s_t<SCALAR_T>& jointPositions) const override;

  joint_jacobian_t baseToFootJacobianInBaseFrame(size_t footIndex,
                                                 const switched_model::joint_coordinate_s_t<SCALAR_T>& jointPositions) const override;

  switched_model::matrix3_s_t<SCALAR_T> footOrientationInBaseFrame(
      size_t footIndex, const switched_model::joint_coordinate_s_t<SCALAR_T>& jointPositions) const override;
};

}  // namespace tpl

using AnymalWheelsKinematics = tpl::AnymalWheelsKinematics<double>;
using AnymalWheelsKinematicsAd = tpl::AnymalWheelsKinematics<ocs2::CppAdInterface<double>::ad_scalar_t>;

}  // namespace anymal

/**
 *  Explicit instantiation, for instantiation additional types, include the implementation file instead of this one.
 */
extern template class anymal::tpl::AnymalWheelsKinematics<double>;
extern template class anymal::tpl::AnymalWheelsKinematics<ocs2::CppAdInterface<double>::ad_scalar_t>;
