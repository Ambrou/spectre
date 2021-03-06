// Distributed under the MIT License.
// See LICENSE.txt for details.

#include "Framework/TestingFramework.hpp"

#include "Helpers/Evolution/DiscontinuousGalerkin/Actions/ComputeTimeDerivativeImpl.tpp"

namespace TestHelpers::evolution::dg::Actions {
template void test<SystemType::Nonconservative, UseBoundaryCorrection::No, 1>();
template void test<SystemType::Conservative, UseBoundaryCorrection::No, 1>();
}  // namespace TestHelpers::evolution::dg::Actions
