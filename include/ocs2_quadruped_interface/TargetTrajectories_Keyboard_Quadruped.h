/*
 * TargetTrajectories_Keyboard_Quadruped.h
 *
 *  Created on: Aug 15, 2018
 *      Author: farbod
 */

#ifndef TARGETTRAJECTORIES_KEYBOARD_QUADRUPED_H_
#define TARGETTRAJECTORIES_KEYBOARD_QUADRUPED_H_

#include <iomanip>

#include <ocs2_robotic_tools/command/TargetTrajectories_Keyboard_Interface.h>
#include <mutex>

namespace switched_model {

/**
 * This class implements TargetTrajectories communication using ROS.
 *
 * @tparam SCALAR_T: scalar type.
 */
template <typename SCALAR_T, size_t STATE_DIM, size_t INPUT_DIM>
class TargetTrajectories_Keyboard_Quadruped : public ocs2::TargetTrajectories_Keyboard_Interface<SCALAR_T>
{
public:
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW

	enum
	{
		command_dim_ = 12
	};

	enum class COMMAND_MODE
	{
			POSITION,
			VELOCITY
	};

	using BASE = ocs2::TargetTrajectories_Keyboard_Interface<SCALAR_T>;
	using typename BASE::scalar_t;
	using typename BASE::scalar_array_t;
	using typename BASE::dynamic_vector_t;
	using typename BASE::dynamic_vector_array_t;
	using typename BASE::cost_desired_trajectories_t;
	using joint_coordinates_t = Eigen::Matrix<SCALAR_T, 12, 1>;

	/**
	 * Constructor.
	 *
	 * @param [in] robotName: The robot's name.
	 * @param [in] goalPoseLimit: Limits for the input command. It has size 12 with following entries.
	 * @param [in] command_mode: Whether to use position mode or velocity mode.
	 *
	 * goalPoseLimit(0): X
	 * goalPoseLimit(1): Y
	 * goalPoseLimit(2): Z
	 *
	 * goalPoseLimit(3): Roll
	 * goalPoseLimit(4): Pitch
	 * goalPoseLimit(5): Yaw
	 *
	 * goalPoseLimit(6): v_X
	 * goalPoseLimit(7): v_Y
	 * goalPoseLimit(8): v_Z
	 *
	 * goalPoseLimit(9): \omega_X
	 * goalPoseLimit(10): \omega_Y
	 * goalPoseLimit(11): \omega_Z
	 */
	TargetTrajectories_Keyboard_Quadruped(
				int argc, char* argv[],
				const std::string& robotName,
				scalar_t initZHeight,
				joint_coordinates_t defaultJointCoordinates,
				scalar_t targetDisplacementVelocity,
				scalar_t targetRotationVelocity,
				const scalar_array_t& goalPoseLimit =
				scalar_array_t{2.0, 1.0, 0.3, 45.0, 45.0, 360.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0},
				const COMMAND_MODE command_mode = COMMAND_MODE::POSITION)
	: BASE(argc, argv, robotName, command_dim_, goalPoseLimit),
	command_mode_(command_mode),
	initZHeight_(initZHeight),
	defaultJointCoordinates_(defaultJointCoordinates),
    targetDisplacementVelocity_(targetDisplacementVelocity),
    targetRotationVelocity_(targetRotationVelocity)
	{
		observationSubscriber_ = this->nodeHandle_->subscribe("/" + robotName + "_mpc_observation", 1,
															  &TargetTrajectories_Keyboard_Quadruped::observationCallback, this);
	}

	/**
	* Default destructor
	*/
	~TargetTrajectories_Keyboard_Quadruped() = default;

	/**
	 * From command line loaded command to desired time, state, and input.
	 *
	 * @param [out] commadLineTarget: The loaded command target.
	 * @param [in] desiredTime: Desired time to be published.
	 * @param [in] desiredState: Desired state to be published.
	 * @param [in] desiredInput: Desired input to be published.
	 */
	void toCostDesiredTimeStateInput(
			const scalar_array_t& commadLineTarget,
			scalar_t& desiredTime,
			dynamic_vector_t& desiredState,
			dynamic_vector_t& desiredInput) final {

		auto deg2rad = [](scalar_t deg) { return (deg * M_PI / 180.0); };

		desiredState.resize(command_dim_);
		if (command_mode_ == COMMAND_MODE::POSITION) {
			// reversing the order of the position and orientation.
			for (size_t j = 0; j < 3; j++) {
				// pose
				desiredState[j] = deg2rad(commadLineTarget[3 + j]);
				desiredState[3 + j] = commadLineTarget[j];
				// velocities
				desiredState[6 + j] = commadLineTarget[9 + j];
				desiredState[9 + j] = commadLineTarget[6 + j];
			}
		} else if (command_mode_ == COMMAND_MODE::VELOCITY) {
			// reversing the order of the position and orientation.
			// velocity before position and orientation
			for (size_t j = 0; j < 3; j++) {
				// velocities
				desiredState[6 + j] = commadLineTarget[3 + j];
				desiredState[9 + j] = commadLineTarget[j];
				// pose
				desiredState[j] = deg2rad(commadLineTarget[9 + j]);
				desiredState[3 + j] = commadLineTarget[6 + j];
			}
		} else {
			std::runtime_error("Unknown command mode for target!");
		}

		// time
		desiredTime = estimeTimeToTarget(desiredState[2], desiredState[3], desiredState[4]);
		// input
        // TODO(Ruben)
		desiredInput = dynamic_vector_t::Zero(INPUT_DIM);
        desiredInput[2+0] = 80.0;
        desiredInput[2+3] = 80.0;
        desiredInput[2+6] = 80.0;
        desiredInput[2+9] = 80.0;
	}

  cost_desired_trajectories_t toCostDesiredTrajectories(const scalar_array_t& commadLineTarget) final {


	  ocs2::SystemObservation<STATE_DIM, INPUT_DIM> observation;
	  ::ros::spinOnce();
	  {
		  std::lock_guard<std::mutex> lock(latestObservationMutex_);
		  ocs2::RosMsgConversions<STATE_DIM, INPUT_DIM>::readObservationMsg(*latestObservation_, observation);
	  }

	  // Convert commandline target to base desired
	  scalar_t desiredTime;
	  dynamic_vector_t desiredBaseState;
	  dynamic_vector_t desiredInput;
	  toCostDesiredTimeStateInput(commadLineTarget, desiredTime, desiredBaseState, desiredInput);

	  // Trajectory to publish
	  cost_desired_trajectories_t costDesiredTrajectories(2);

	  // Desired time trajectory
	  scalar_array_t& tDesiredTrajectory = costDesiredTrajectories.desiredTimeTrajectory();
	  tDesiredTrajectory.resize(2);
	  tDesiredTrajectory[0] = observation.time();
	  tDesiredTrajectory[1] = observation.time() + desiredTime;

	  // Desired state trajectory
	  typename cost_desired_trajectories_t::dynamic_vector_array_t& xDesiredTrajectory =
			  costDesiredTrajectories.desiredStateTrajectory();
	  xDesiredTrajectory.resize(2);
	  xDesiredTrajectory[0].resize(STATE_DIM);
	  xDesiredTrajectory[0].setZero();
	  xDesiredTrajectory[0].segment(0, 12) = observation.state().segment(0, 12);
	  xDesiredTrajectory[0].segment(12, 12) = defaultJointCoordinates_;

	  xDesiredTrajectory[1].resize(STATE_DIM);
	  xDesiredTrajectory[1].setZero();
	  // Roll and pitch are absolute
	  xDesiredTrajectory[1].segment(0, 2) = desiredBaseState.segment(0, 2);
	  // Yaw relative to current
	  xDesiredTrajectory[1][2] = observation.state()[2] + desiredBaseState[2];
	  // base x, y relative to current state
	  xDesiredTrajectory[1].segment(3, 2) = observation.state().segment(3, 2) + desiredBaseState.segment(3, 2);
	  // base z relative to initialization
	  xDesiredTrajectory[1][5] = initZHeight_ + desiredBaseState[5];
	  // target velocities
	  xDesiredTrajectory[1].segment(6, 6) = desiredBaseState.segment(6, 6);
	  // joint angle from initialization
	  xDesiredTrajectory[1].segment(12, 12) = defaultJointCoordinates_;

	  // Desired input trajectory
	  typename cost_desired_trajectories_t::dynamic_vector_array_t& uDesiredTrajectory =
			  costDesiredTrajectories.desiredInputTrajectory();
	  uDesiredTrajectory.resize(2);
	  uDesiredTrajectory[0] = desiredInput;
	  uDesiredTrajectory[1] = desiredInput;

	  return costDesiredTrajectories;
  }

  void observationCallback(const ocs2_msgs::mpc_observation::ConstPtr& msg) {
	  std::lock_guard<std::mutex> lock(latestObservationMutex_);
	  latestObservation_ = msg;
  }


 private:

  scalar_t estimeTimeToTarget(scalar_t dyaw, scalar_t dx, scalar_t dy) const {
	  scalar_t rotationTime = std::abs(dyaw) / targetRotationVelocity_;
	  scalar_t displacement = std::sqrt(dx*dx + dy*dy);
	  scalar_t displacementTime = displacement / targetDisplacementVelocity_;

	  return std::max(rotationTime, displacementTime);
	}

  COMMAND_MODE command_mode_;

  ros::Subscriber observationSubscriber_;

  std::mutex latestObservationMutex_;
  ocs2_msgs::mpc_observation::ConstPtr latestObservation_;
  scalar_t initZHeight_;
  joint_coordinates_t defaultJointCoordinates_;

  scalar_t targetDisplacementVelocity_;
  scalar_t targetRotationVelocity_;
};

} // end of namespace switched_model

#endif /* TARGETTRAJECTORIES_KEYBOARD_QUADRUPED_H_ */
