//
// Created by rgrandia on 18.03.20.
//

#include "ocs2_switched_model_interface/logic/SwitchedModelModeScheduleManager.h"

#include "ocs2_switched_model_interface/core/MotionPhaseDefinition.h"

namespace switched_model {

SwitchedModelModeScheduleManager::SwitchedModelModeScheduleManager(std::unique_ptr<GaitSchedule> gaitSchedule,
                                                                   std::unique_ptr<SwingTrajectoryPlanner> swingTrajectory,
                                                                   std::unique_ptr<TerrainModel> terrainModel)
    : Base(ocs2::ModeSchedule()),
      gaitSchedule_(std::move(gaitSchedule)),
      swingTrajectoryPtr_(std::move(swingTrajectory)),
      terrainModel_(std::move(terrainModel)) {}

contact_flag_t SwitchedModelModeScheduleManager::getContactFlags(scalar_t time) const {
  return modeNumber2StanceLeg(this->getModeSchedule().modeAtTime(time));
}

void SwitchedModelModeScheduleManager::preSolverRunImpl(scalar_t initTime, scalar_t finalTime, const vector_t& currentState,
                                                        const ocs2::CostDesiredTrajectories& costDesiredTrajectory,
                                                        ocs2::ModeSchedule& modeSchedule) {
  const auto timeHorizon = finalTime - initTime;
  {
    auto lockedGaitSchedulePtr = gaitSchedule_.lock();
    lockedGaitSchedulePtr->advanceToTime(initTime);
    modeSchedule = lockedGaitSchedulePtr->getModeSchedule(2.0 * timeHorizon);
  }

  {
    auto lockedTerrainPtr = terrainModel_.lock();
    swingTrajectoryPtr_->update(initTime, finalTime, currentState, costDesiredTrajectory, extractContactTimingsPerLeg(modeSchedule),
                                *lockedTerrainPtr);
  }
}

}  // namespace switched_model
