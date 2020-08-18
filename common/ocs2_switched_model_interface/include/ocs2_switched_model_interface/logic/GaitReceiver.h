//
// Created by rgrandia on 18.03.20.
//

#pragma once

#include <mutex>

#include <ros/ros.h>

#include <ocs2_msgs/mode_schedule.h>

#include <ocs2_core/misc/Lockable.h>

#include <ocs2_oc/oc_solver/SolverSynchronizedModule.h>

#include <ocs2_switched_model_msgs/scheduled_gait_sequence.h>

#include "ocs2_switched_model_interface/core/SwitchedModel.h"
#include "ocs2_switched_model_interface/logic/GaitSchedule.h"

namespace switched_model {

class GaitReceiver : public ocs2::SolverSynchronizedModule {
 public:
  using LockableGaitSchedule = ocs2::Lockable<GaitSchedule>;

  GaitReceiver(ros::NodeHandle nodeHandle, std::shared_ptr<LockableGaitSchedule> gaitSchedulePtr, const std::string& robotName);

  void preSolverRun(scalar_t initTime, scalar_t finalTime, const vector_t& currentState,
                    const ocs2::CostDesiredTrajectories& costDesiredTrajectory) override;

  void postSolverRun(const ocs2::PrimalSolution& primalSolution) override{};

 private:
  void mpcModeSequenceCallback(const ocs2_msgs::mode_schedule::ConstPtr& msg);
  void mpcModeScheduledGaitCallback(const ocs2_msgs::mode_schedule::ConstPtr& msg);
  void mpcGaitSequenceCallback(const ocs2_switched_model_msgs::scheduled_gait_sequenceConstPtr& msg);

  ros::Subscriber mpcModeSequenceSubscriber_;
  ros::Subscriber mpcScheduledModeSequenceSubscriber_;
  ros::Subscriber mpcGaitSequenceSubscriber_;

  std::shared_ptr<LockableGaitSchedule> gaitSchedulePtr_;

  std::atomic_bool gaitUpdated_;

  std::mutex receivedGaitMutex_; // protects the setGaitAction_ variable
  std::function<void(scalar_t initTime, scalar_t finalTime, const state_vector_t& currentState,
                     const ocs2::CostDesiredTrajectories& costDesiredTrajectory)>
      setGaitAction_;
};

}  // namespace switched_model
