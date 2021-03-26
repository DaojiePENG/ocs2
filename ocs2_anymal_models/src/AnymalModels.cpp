//
// Created by rgrandia on 22.09.20.
//

#include <ocs2_anymal_models/AnymalModels.h>

#include <unordered_map>

#include <ocs2_anymal_models/bear/AnymalBearCom.h>
#include <ocs2_anymal_models/bear/AnymalBearKinematics.h>
#include <ocs2_anymal_models/bear/WholebodyDynamicsBear.h>
#include <ocs2_anymal_models/cerberus/AnymalCerberusCom.h>
#include <ocs2_anymal_models/cerberus/AnymalCerberusKinematics.h>
#include <ocs2_anymal_models/cerberus/WholebodyDynamicsCerberus.h>
#include <ocs2_anymal_models/chimera/AnymalChimeraCom.h>
#include <ocs2_anymal_models/chimera/AnymalChimeraKinematics.h>
#include <ocs2_anymal_models/chimera/WholebodyDynamicsChimera.h>
#include <ocs2_anymal_models/croc/AnymalCrocCom.h>
#include <ocs2_anymal_models/croc/AnymalCrocKinematics.h>
#include <ocs2_anymal_models/croc/WholebodyDynamicsCroc.h>
#include <ocs2_anymal_models/wheels/AnymalWheelsCom.h>
#include <ocs2_anymal_models/wheels/AnymalWheelsKinematics.h>
#include <ocs2_anymal_models/wheels_chimera/AnymalWheelsChimeraCom.h>
#include <ocs2_anymal_models/wheels_chimera/AnymalWheelsChimeraKinematics.h>

namespace anymal {

std::string toString(AnymalModel model) {
  static const std::unordered_map<AnymalModel, std::string> map{{AnymalModel::Bear, "bear"},
                                                                {AnymalModel::Cerberus, "cerberus"},
                                                                {AnymalModel::Chimera, "chimera"},
                                                                {AnymalModel::Croc, "croc"},
                                                                {AnymalModel::Wheels, "wheels"},
                                                                {AnymalModel::WheelsChimera, "wheels_chimera"}};
  return map.at(model);
}

AnymalModel stringToAnymalModel(const std::string& name) {
  static const std::unordered_map<std::string, AnymalModel> map{{"bear", AnymalModel::Bear},
                                                                {"cerberus", AnymalModel::Cerberus},
                                                                {"chimera", AnymalModel::Chimera},
                                                                {"croc", AnymalModel::Croc},
                                                                {"wheels_chimera", AnymalModel::WheelsChimera}};
  return map.at(name);
}

std::unique_ptr<switched_model::KinematicsModelBase<ocs2::scalar_t>> getAnymalKinematics(AnymalModel model) {
  switch (model) {
    case AnymalModel::Bear:
      return std::unique_ptr<switched_model::KinematicsModelBase<ocs2::scalar_t>>(new AnymalBearKinematics());
    case AnymalModel::Cerberus:
      return std::unique_ptr<switched_model::KinematicsModelBase<ocs2::scalar_t>>(new AnymalCerberusKinematics());
    case AnymalModel::Chimera:
      return std::unique_ptr<switched_model::KinematicsModelBase<ocs2::scalar_t>>(new AnymalChimeraKinematics());
    case AnymalModel::Croc:
      return std::unique_ptr<switched_model::KinematicsModelBase<ocs2::scalar_t>>(new AnymalCrocKinematics());
    case AnymalModel::Wheels:
      return std::unique_ptr<switched_model::KinematicsModelBase<ocs2::scalar_t>>(new AnymalWheelsKinematics());
    case AnymalModel::WheelsChimera:
      return std::unique_ptr<switched_model::KinematicsModelBase<ocs2::scalar_t>>(new AnymalWheelsChimeraKinematics());
    default:
      throw std::runtime_error("[AnymalModels] unkown model");
  }
}

std::unique_ptr<switched_model::KinematicsModelBase<ocs2::ad_scalar_t>> getAnymalKinematicsAd(AnymalModel model) {
  switch (model) {
    case AnymalModel::Bear:
      return std::unique_ptr<switched_model::KinematicsModelBase<ocs2::ad_scalar_t>>(new AnymalBearKinematicsAd());
    case AnymalModel::Cerberus:
      return std::unique_ptr<switched_model::KinematicsModelBase<ocs2::ad_scalar_t>>(new AnymalCerberusKinematicsAd());
    case AnymalModel::Chimera:
      return std::unique_ptr<switched_model::KinematicsModelBase<ocs2::ad_scalar_t>>(new AnymalChimeraKinematicsAd());
    case AnymalModel::Croc:
      return std::unique_ptr<switched_model::KinematicsModelBase<ocs2::ad_scalar_t>>(new AnymalCrocKinematicsAd());
    case AnymalModel::Wheels:
      return std::unique_ptr<switched_model::KinematicsModelBase<ocs2::ad_scalar_t>>(new AnymalWheelsKinematicsAd());
    case AnymalModel::WheelsChimera:
      return std::unique_ptr<switched_model::KinematicsModelBase<ocs2::ad_scalar_t>>(new AnymalWheelsChimeraKinematicsAd());
    default:
      throw std::runtime_error("[AnymalModels] unkown model");
  }
}

std::unique_ptr<switched_model::ComModelBase<ocs2::scalar_t>> getAnymalComModel(AnymalModel model) {
  switch (model) {
    case AnymalModel::Bear:
      return std::unique_ptr<switched_model::ComModelBase<ocs2::scalar_t>>(new AnymalBearCom());
    case AnymalModel::Cerberus:
      return std::unique_ptr<switched_model::ComModelBase<ocs2::scalar_t>>(new AnymalCerberusCom());
    case AnymalModel::Chimera:
      return std::unique_ptr<switched_model::ComModelBase<ocs2::scalar_t>>(new AnymalChimeraCom());
    case AnymalModel::Croc:
      return std::unique_ptr<switched_model::ComModelBase<ocs2::scalar_t>>(new AnymalCrocCom());
    case AnymalModel::Wheels:
      return std::unique_ptr<switched_model::ComModelBase<ocs2::scalar_t>>(new AnymalWheelsCom());
    case AnymalModel::WheelsChimera:
      return std::unique_ptr<switched_model::ComModelBase<ocs2::scalar_t>>(new AnymalWheelsChimeraCom());
    default:
      throw std::runtime_error("[AnymalModels] unkown model");
  }
}

std::unique_ptr<switched_model::ComModelBase<ocs2::ad_scalar_t>> getAnymalComModelAd(AnymalModel model) {
  switch (model) {
    case AnymalModel::Bear:
      return std::unique_ptr<switched_model::ComModelBase<ocs2::ad_scalar_t>>(new AnymalBearComAd());
    case AnymalModel::Cerberus:
      return std::unique_ptr<switched_model::ComModelBase<ocs2::ad_scalar_t>>(new AnymalCerberusComAd());
    case AnymalModel::Chimera:
      return std::unique_ptr<switched_model::ComModelBase<ocs2::ad_scalar_t>>(new AnymalChimeraComAd());
    case AnymalModel::Croc:
      return std::unique_ptr<switched_model::ComModelBase<ocs2::ad_scalar_t>>(new AnymalCrocComAd());
    case AnymalModel::Wheels:
      return std::unique_ptr<switched_model::ComModelBase<ocs2::ad_scalar_t>>(new AnymalWheelsComAd());
    case AnymalModel::WheelsChimera:
      return std::unique_ptr<switched_model::ComModelBase<ocs2::ad_scalar_t>>(new AnymalWheelsChimeraComAd());
    default:
      throw std::runtime_error("[AnymalModels] unkown model");
  }
}

std::unique_ptr<switched_model::WholebodyDynamics<ocs2::scalar_t>> getWholebodyDynamics(AnymalModel model) {
  switch (model) {
    case AnymalModel::Bear:
      return std::unique_ptr<switched_model::WholebodyDynamics<ocs2::scalar_t>>(new WholebodyDynamicsBear());
    case AnymalModel::Cerberus:
      return std::unique_ptr<switched_model::WholebodyDynamics<ocs2::scalar_t>>(new WholebodyDynamicsCerberus());
    case AnymalModel::Chimera:
      return std::unique_ptr<switched_model::WholebodyDynamics<ocs2::scalar_t>>(new WholebodyDynamicsChimera());
    case AnymalModel::Croc:
      return std::unique_ptr<switched_model::WholebodyDynamics<ocs2::scalar_t>>(new WholebodyDynamicsCroc());
    case AnymalModel::Wheels:
      throw std::runtime_error("[getWholebodyDynamics] not implemented for wheels");
    case AnymalModel::WheelsChimera:
      throw std::runtime_error("[getWholebodyDynamics] not implemented for wheels");
    default:
      throw std::runtime_error("[AnymalModels] unkown model");
  }
}

std::unique_ptr<switched_model::WholebodyDynamics<ocs2::ad_scalar_t>> getWholebodyDynamicsAd(AnymalModel model) {
  switch (model) {
    case AnymalModel::Bear:
      return std::unique_ptr<switched_model::WholebodyDynamics<ocs2::ad_scalar_t>>(new WholebodyDynamicsBearAd());
    case AnymalModel::Cerberus:
      return std::unique_ptr<switched_model::WholebodyDynamics<ocs2::ad_scalar_t>>(new WholebodyDynamicsCerberusAd());
    case AnymalModel::Chimera:
      return std::unique_ptr<switched_model::WholebodyDynamics<ocs2::ad_scalar_t>>(new WholebodyDynamicsChimeraAd());
    case AnymalModel::Croc:
      return std::unique_ptr<switched_model::WholebodyDynamics<ocs2::ad_scalar_t>>(new WholebodyDynamicsCrocAd());
    case AnymalModel::Wheels:
      throw std::runtime_error("[getWholebodyDynamicsAd] not implemented for wheels");
    case AnymalModel::WheelsChimera:
      throw std::runtime_error("[getWholebodyDynamicsAd] not implemented for wheels");
    default:
      throw std::runtime_error("[AnymalModels] unkown model");
  }
}

}  // namespace anymal
