#include <ocs2_switched_model_interface/constraint/AnymalWheelsComKinoConstraintAd.h>

namespace switched_model {

AnymalWheelsComKinoConstraintAd::AnymalWheelsComKinoConstraintAd(const ad_kinematic_model_t& adKinematicModel,
                                                                 const ad_com_model_t& adComModel,
                                                                 const SwitchedModelModeScheduleManager& modeScheduleManager,
                                                                 const SwingTrajectoryPlanner& swingTrajectoryPlanner,
                                                                 ModelSettings options)
    : inequalityConstraintsCollectionPtr_(new ocs2::StateInputConstraintCollection),
      adKinematicModelPtr_(adKinematicModel.clone()),
      adComModelPtr_(adComModel.clone()),
      options_(std::move(options)),
      modeScheduleManagerPtr_(&modeScheduleManager),
      swingTrajectoryPlannerPtr_(&swingTrajectoryPlanner) {
  initializeConstraintTerms();
  collectConstraintPointers();
}

/******************************************************************************************************/
/******************************************************************************************************/
/******************************************************************************************************/
AnymalWheelsComKinoConstraintAd::AnymalWheelsComKinoConstraintAd(const AnymalWheelsComKinoConstraintAd& rhs)
    : ocs2::ConstraintBase(rhs),
      equalityStateInputConstraintCollection_(rhs.equalityStateInputConstraintCollection_),
      inequalityConstraintsCollectionPtr_(rhs.inequalityConstraintsCollectionPtr_->clone()),
      adKinematicModelPtr_(rhs.adKinematicModelPtr_->clone()),
      adComModelPtr_(rhs.adComModelPtr_->clone()),
      options_(rhs.options_),
      modeScheduleManagerPtr_(rhs.modeScheduleManagerPtr_),
      swingTrajectoryPlannerPtr_(rhs.swingTrajectoryPlannerPtr_) {
  collectConstraintPointers();
}

/******************************************************************************************************/
/******************************************************************************************************/
/******************************************************************************************************/
AnymalWheelsComKinoConstraintAd* AnymalWheelsComKinoConstraintAd::clone() const {
  return new AnymalWheelsComKinoConstraintAd(*this);
}

/******************************************************************************************************/
/******************************************************************************************************/
/******************************************************************************************************/
void AnymalWheelsComKinoConstraintAd::initializeConstraintTerms() {
  using ConstraintTerm_t = ConstraintTerm<STATE_DIM, INPUT_DIM>;

  for (int i = 0; i < NUM_CONTACT_POINTS; i++) {
    auto footName = feetNames[i];

    // Friction cone constraint
    FrictionConeConstraint::Config frictionConfig(options_.frictionCoefficient_, 25.0);
    std::unique_ptr<FrictionConeConstraint> frictionCone(new FrictionConeConstraint(std::move(frictionConfig), i));

    // EE force
    auto zeroForceConstraint = std::unique_ptr<ConstraintTerm_t>(new ZeroForceConstraint(i));

    // Foot normal Constraint
    auto footNormalConstraint = std::unique_ptr<ConstraintTerm_t>(
        new FootNormalConstraint(i, FootNormalConstraintMatrix(), *adComModelPtr_, *adKinematicModelPtr_, options_.recompileLibraries_));
    // EE InFootFrame Velocity Constraint
    auto endEffectorVelocityInFootFrameConstraint = std::unique_ptr<ConstraintTerm_t>(new EndEffectorVelocityInFootFrameConstraint(
        i, EndEffectorVelocityInFootFrameConstraintSettings(), *adComModelPtr_, *adKinematicModelPtr_, options_.recompileLibraries_));

    // Inequalities
    inequalityConstraintsCollectionPtr_->add(footName + "_FrictionCone", std::move(frictionCone));

    // State input equalities
    equalityStateInputConstraintCollection_.add(footName + "_ZeroForce", std::move(zeroForceConstraint));
    equalityStateInputConstraintCollection_.add(footName + "_EENormal", std::move(footNormalConstraint));
    equalityStateInputConstraintCollection_.add(footName + "_f_EEVel", std::move(endEffectorVelocityInFootFrameConstraint));
  }
}

/******************************************************************************************************/
/******************************************************************************************************/
/******************************************************************************************************/
void AnymalWheelsComKinoConstraintAd::collectConstraintPointers() {
  for (int i = 0; i < NUM_CONTACT_POINTS; i++) {
    auto footName = feetNames[i];

    // Inequalities
    frictionConeConstraints_[i] = &inequalityConstraintsCollectionPtr_->get<FrictionConeConstraint>(footName + "_FrictionCone");

    // State input equalities
    zeroForceConstraints_[i] = &equalityStateInputConstraintCollection_.get<ZeroForceConstraint>(footName + "_ZeroForce");
    eeNormalConstraints_[i] = &equalityStateInputConstraintCollection_.get<FootNormalConstraint>(footName + "_EENormal");
    eeVelConstraints_[i] = &equalityStateInputConstraintCollection_.get<EndEffectorVelocityInFootFrameConstraint>(footName + "_f_EEVel");
  }
}

/******************************************************************************************************/
/******************************************************************************************************/
/******************************************************************************************************/
void AnymalWheelsComKinoConstraintAd::updateStateInputEqualityConstraints(scalar_t t) {
  for (int i = 0; i < NUM_CONTACT_POINTS; i++) {
    const auto& footName = feetNames[i];
    const auto& footPhase = swingTrajectoryPlannerPtr_->getFootPhase(i, t);
    const bool inContact = footPhase.contactFlag();

    // Zero forces active for swing legs
    zeroForceConstraints_[i]->setActivity(!inContact);

    // Foot normal constraint always active
    auto& EENormalConstraint = *eeNormalConstraints_[i];
    EENormalConstraint.setActivity(true);
    EENormalConstraint.configure(footPhase.getFootNormalConstraintInWorldFrame(t));

    // Rolling InFootFrame Velocity constraint for stance legs
    auto& EEVelInFootFrameConstraint = *eeVelConstraints_[i];
    EEVelInFootFrameConstraint.setActivity(inContact);
    if (inContact) {
      // EE velocities in lateral direction (y) in foot frame should be zero.
      EndEffectorVelocityInFootFrameConstraintSettings eeVelInFootFrameConSettings(1, 3);
      eeVelInFootFrameConSettings.b << 0;
      eeVelInFootFrameConSettings.A << 0, 1, 0;
      EEVelInFootFrameConstraint.configure(eeVelInFootFrameConSettings);
    }
  }
}

/******************************************************************************************************/
/******************************************************************************************************/
/******************************************************************************************************/
void AnymalWheelsComKinoConstraintAd::updateInequalityConstraints(scalar_t t) {
  for (int i = 0; i < NUM_CONTACT_POINTS; i++) {
    const auto& footName = feetNames[i];
    const auto& footPhase = swingTrajectoryPlannerPtr_->getFootPhase(i, t);
    const bool inContact = footPhase.contactFlag();

    // Active friction cone constraint for stanceLegs
    auto& frictionConeConstraint = *frictionConeConstraints_[i];
    frictionConeConstraint.setActivity(inContact);
    if (inContact) {
      frictionConeConstraint.setSurfaceNormalInWorld(footPhase.normalDirectionInWorldFrame(t));
    }
  }
}

/******************************************************************************************************/
/******************************************************************************************************/
/******************************************************************************************************/
vector_t AnymalWheelsComKinoConstraintAd::stateInputEqualityConstraint(scalar_t t, const vector_t& x, const vector_t& u) {
  updateStateInputEqualityConstraints(t);
  return equalityStateInputConstraintCollection_.getValueAsVector(t, x, u);
}

/******************************************************************************************************/
/******************************************************************************************************/
/******************************************************************************************************/
vector_t AnymalWheelsComKinoConstraintAd::inequalityConstraint(scalar_t t, const vector_t& x, const vector_t& u) {
  updateInequalityConstraints(t);
  return inequalityConstraintsCollectionPtr_->getValue(t, x, u);
}

/******************************************************************************************************/
/******************************************************************************************************/
/******************************************************************************************************/
VectorFunctionLinearApproximation AnymalWheelsComKinoConstraintAd::stateInputEqualityConstraintLinearApproximation(scalar_t t,
                                                                                                                   const vector_t& x,
                                                                                                                   const vector_t& u) {
  updateStateInputEqualityConstraints(t);
  const size_t numConstraints = equalityStateInputConstraintCollection_.getNumConstraints(t);
  const auto constraintApproximation = equalityStateInputConstraintCollection_.getLinearApproximationAsMatrices(t, x, u);

  VectorFunctionLinearApproximation g;
  g.f = constraintApproximation.constraintValues.head(numConstraints);
  g.dfdx = constraintApproximation.derivativeState.topRows(numConstraints);
  g.dfdu = constraintApproximation.derivativeInput.topRows(numConstraints);
  return g;
}

/******************************************************************************************************/
/******************************************************************************************************/
/******************************************************************************************************/
VectorFunctionQuadraticApproximation AnymalWheelsComKinoConstraintAd::inequalityConstraintQuadraticApproximation(scalar_t t,
                                                                                                                 const vector_t& x,
                                                                                                                 const vector_t& u) {
  updateInequalityConstraints(t);
  return inequalityConstraintsCollectionPtr_->getQuadraticApproximation(t, x, u);
}

}  // namespace switched_model
