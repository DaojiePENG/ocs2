//
// Created by rgrandia on 17.02.20.
//

#include "ocs2_anymal_mpc/AnymalInterface.h"

#include <ros/package.h>

namespace anymal {

std::unique_ptr<switched_model::QuadrupedPointfootInterface> getAnymalInterface(AnymalModel model, const std::string& taskFolder){
  std::cerr << "Loading task file from: " << taskFolder << std::endl;

  auto kin = getAnymalKinematics(model);
  auto kinAd = getAnymalKinematicsAd(model);
  auto com = getAnymalComModel(model);
  auto comAd = getAnymalComModelAd(model);
  return std::unique_ptr<switched_model::QuadrupedPointfootInterface>(
      new switched_model::QuadrupedPointfootInterface(*kin, *kinAd, *com, *comAd, taskFolder));
}

std::string getConfigFolder(const std::string& configName) {
  return ros::package::getPath("ocs2_anymal_mpc") + "/config/" + configName;
}

std::string getTaskFilePath(const std::string& configName) {
  return getConfigFolder(configName) + "/task.info";
}

}  // namespace anymal
