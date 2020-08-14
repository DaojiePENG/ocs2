//
// Created by rgrandia on 21.04.20.
//

#pragma once

#include "ocs2_switched_model_interface/core/SwitchedModel.h"
#include "ocs2_switched_model_interface/terrain/ConvexTerrain.h"
#include "ocs2_switched_model_interface/terrain/SignedDistanceField.h"
#include "ocs2_switched_model_interface/terrain/TerrainPlane.h"

namespace switched_model {

/**
 * This abstract class defines the interface for terrain models.
 */
class TerrainModel {
 public:
  TerrainModel() = default;
  virtual ~TerrainModel() = default;
  TerrainModel(const TerrainModel&) = delete;
  TerrainModel& operator=(const TerrainModel&) = delete;

  /** Returns a linear approximation of the terrain at the query point projected along gravity onto the terrain  */
  virtual TerrainPlane getLocalTerrainAtPositionInWorldAlongGravity(const vector3_t& positionInWorld) const = 0;

  virtual ConvexTerrain getConvexTerrainAtPositionInWorld(const vector3_t& positionInWorld) const {
    return {getLocalTerrainAtPositionInWorldAlongGravity(positionInWorld), {}};
  }

  /** Returns the signed distance field for this terrain if one is available */
  virtual const SignedDistanceField* getSignedDistanceField() const { return nullptr; }
};

}  // namespace switched_model
