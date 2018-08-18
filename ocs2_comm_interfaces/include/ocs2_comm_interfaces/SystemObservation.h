/******************************************************************************
Copyright (c) 2017, Farbod Farshidian. All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

* Neither the name of the copyright holder nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
******************************************************************************/

#ifndef SYSTEMOBSERVATION_OCS2_H_
#define SYSTEMOBSERVATION_OCS2_H_

#include <ocs2_core/Dimensions.h>

namespace ocs2{

/**
 * This class implements single thread SLQ algorithm.
 *
 * @tparam STATE_DIM: Dimension of the state space.
 * @tparam INPUT_DIM: Dimension of the control input space.
 * @tparam LOGIC_RULES_T: Logic Rules type (default NullLogicRules).
 */
template <size_t STATE_DIM, size_t INPUT_DIM>
class SystemObservation
{
public:
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW

	typedef Dimensions<STATE_DIM, INPUT_DIM> DIMENSIONS;
	typedef typename DIMENSIONS::scalar_t			scalar_t;
	typedef typename DIMENSIONS::state_vector_t 	state_vector_t;
	typedef typename DIMENSIONS::input_vector_t 	input_vector_t;

	SystemObservation() = default;
	~SystemObservation() = default;

	void swap(SystemObservation<STATE_DIM, INPUT_DIM>& other) {

		std::swap(time_, other.time_);
		std::swap(state_, other.state_);
		std::swap(input_, other.input_);
		std::swap(subsystem_, other.subsystem_);
	}

	inline scalar_t& time() { return time_; };
	inline const scalar_t& time() const { return time_; };

	inline state_vector_t& state() { return state_; };
	inline const state_vector_t& state() const { return state_; };
	inline scalar_t& state(const size_t& i) { return state_(i); };
	inline const scalar_t& state(const size_t& i) const { return state_(i); };

	inline input_vector_t& input() { return input_; };
	inline const input_vector_t& input() const { return input_; };
	inline scalar_t& input(const size_t& i) { return input_(i); };
	inline const scalar_t& input(const size_t& i) const { return input_(i); };

	inline size_t& subsystem() { return subsystem_; };
	inline const size_t& subsystem() const { return subsystem_; };

private:
	scalar_t 		time_;
	state_vector_t 	state_;
	input_vector_t 	input_;
	size_t			subsystem_;

};

} // namespace ocs2

#endif /* SYSTEMOBSERVATION_OCS2_H_ */