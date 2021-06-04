//
// Created by rgrandia on 25.09.20.
//

#include "TestAnymalSwitchedModel.h"

#include <ocs2_anymal_models/AnymalModels.h>

using namespace anymal;

class AnymalBearSwitchedModelTests : public switched_model::TestAnymalSwitchedModel {
 public:
  AnymalBearSwitchedModelTests()
      : TestAnymalSwitchedModel(getAnymalKinematics(AnymalModel::Bear), getAnymalKinematicsAd(AnymalModel::Bear),
                                getAnymalComModel(AnymalModel::Bear), getAnymalComModelAd(AnymalModel::Bear),
                                getWholebodyDynamics(AnymalModel::Bear)) {}
};

TEST_F(AnymalBearSwitchedModelTests, Cost) {
  this->testCosts();
}

TEST_F(AnymalBearSwitchedModelTests, Constraints) {
  this->testConstraints();
}

TEST_F(AnymalBearSwitchedModelTests, Kinematics) {
  this->printKinematics();
}

TEST_F(AnymalBearSwitchedModelTests, ComDynamics) {
  this->printComModel();
}

TEST_F(AnymalBearSwitchedModelTests, baseDynamics) {
  this->testBaseDynamics();
}

TEST_F(AnymalBearSwitchedModelTests, EndeffectorOrientation) {
  this->testEndeffectorOrientation();
}

TEST_F(AnymalBearSwitchedModelTests, EndeffectorAlignedYAxisRandomHFEKFE) {
  this->testEndeffectorAlignedYAxisRandomHFEKFE();
}

TEST_F(AnymalBearSwitchedModelTests, EndeffectorAlignedXAxisRandomHAA) {
  this->testEndeffectorAlignedXAxisRandomHAA();
}