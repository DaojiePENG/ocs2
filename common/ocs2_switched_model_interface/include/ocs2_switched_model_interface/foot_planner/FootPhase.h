//
// Created by rgrandia on 24.04.20.
//

#pragma once

#include "ocs2_switched_model_interface/core/SwitchedModel.h"
#include "ocs2_switched_model_interface/foot_planner/SplineCpg.h"
#include "ocs2_switched_model_interface/terrain/TerrainPlane.h"

namespace switched_model {

/**
 * Linear constraint A_p * p + A_v * v + b = 0
 */
struct FootNormalConstraintMatrix {
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
  Eigen::Matrix<scalar_t, 1, 3> positionMatrix;
  Eigen::Matrix<scalar_t, 1, 3> velocityMatrix;
  scalar_t constant = 0;
};

/**
 * Base class for a planned foot phase : Stance or Swing.
 */
class FootPhase {
 public:
  FootPhase() = default;
  virtual ~FootPhase() = default;
  FootPhase(const FootPhase&) = delete;
  FootPhase& operator=(const FootPhase&) = delete;

  /** Returns the contact flag for this phase. Stance phase: True, Swing phase: false */
  virtual bool contactFlag() const = 0;

  /** Returns the unit vector pointing in the normal direction */
  virtual vector3_t normalDirectionInWorldFrame(scalar_t time) const = 0;

  /** Returns the velocity equality constraint formulated in the normal direction */
  virtual FootNormalConstraintMatrix getFootNormalConstraintInWorldFrame(scalar_t time) const = 0;
};

/**
 * Encodes a planned stance phase on a terrain plane.
 * The normal constraint makes the foot converge to the terrain plane when positionGain > 0.0
 */
class StancePhase final : public FootPhase {
 public:
  explicit StancePhase(const TerrainPlane& stanceTerrain, scalar_t positionGain = 0.0);
  ~StancePhase() override = default;

  bool contactFlag() const override { return true; };
  vector3_t normalDirectionInWorldFrame(scalar_t time) const override;
  FootNormalConstraintMatrix getFootNormalConstraintInWorldFrame(scalar_t time) const override;

 private:
  const TerrainPlane* stanceTerrain_;
  scalar_t positionGain_;
};

/**
 * Encodes a swing trajectory between two terrain planes.
 * A cubic spline is designed in both liftoff and target plane. The constraint then smoothly interpolates between the two splines.
 */
class SwingPhase final : public FootPhase {
 public:
  struct SwingEvent {
    scalar_t time;
    scalar_t velocity;
    const TerrainPlane* terrainPlane;
  };

  SwingPhase(SwingEvent liftOff, scalar_t swingHeight, SwingEvent touchDown, scalar_t positionGain = 0.0);
  ~SwingPhase() override = default;

  bool contactFlag() const override { return false; };
  vector3_t normalDirectionInWorldFrame(scalar_t time) const override;
  FootNormalConstraintMatrix getFootNormalConstraintInWorldFrame(scalar_t time) const override;
  const SplineCpg& getMotionInLiftOffFrame() const { return *liftOffMotion_; };
  const TerrainPlane& getLiftOffFrame() const { return *liftOff_.terrainPlane; };
  const SplineCpg& getMotionInTouchDownFrame() const { return *touchdownMotion_; };
  const TerrainPlane& getTouchDownFrame() const { return *touchDown_.terrainPlane; };

 private:
  void setFullSwing(scalar_t swingHeight);
  void setHalveSwing(scalar_t swingHeight);

  scalar_t getScaling(scalar_t time) const;

  SwingEvent liftOff_;
  SwingEvent touchDown_;
  std::unique_ptr<SplineCpg> liftOffMotion_;
  std::unique_ptr<SplineCpg> touchdownMotion_;
  scalar_t positionGain_;
};

}  // namespace switched_model
