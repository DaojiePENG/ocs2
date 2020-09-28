//
// Created by rgrandia on 23.09.20.
//

#pragma once

#include "ocs2_switched_model_interface/core/WholebodyDynamics.h"

namespace anymal {
namespace tpl {

template <typename SCALAR_T>
class WholebodyDynamicsWheels : public switched_model::WholebodyDynamics<SCALAR_T> {
 public:
  using typename switched_model::WholebodyDynamics<SCALAR_T>::DynamicsTerms;

  WholebodyDynamicsWheels() = default;
  ~WholebodyDynamicsWheels() override = default;

  WholebodyDynamicsWheels* clone() const override { return new WholebodyDynamicsWheels(); };

  virtual DynamicsTerms getDynamicsTerms(const switched_model::rbd_state_s_t<SCALAR_T>& rbdState) const override;
};

}  // namespace tpl

using WholebodyDynamicsWheels = tpl::WholebodyDynamicsWheels<ocs2::scalar_t>;
using WholebodyDynamicsWheelsAd = tpl::WholebodyDynamicsWheels<ocs2::ad_scalar_t>;

}  // namespace anymal

/**
 *  Explicit instantiation, for instantiation additional types, include the implementation file instead of this one.
 */
extern template class anymal::tpl::WholebodyDynamicsWheels<ocs2::scalar_t>;
extern template class anymal::tpl::WholebodyDynamicsWheels<ocs2::ad_scalar_t>;