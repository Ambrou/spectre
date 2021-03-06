// Distributed under the MIT License.
// See LICENSE.txt for details.

#include "Framework/TestingFramework.hpp"

#include <array>
#include <cstddef>
#include <memory>
#include <pup.h>
#include <unordered_set>
#include <vector>

#include "DataStructures/Tensor/Tensor.hpp"
#include "Domain/Block.hpp"  // IWYU pragma: keep
#include "Domain/CoordinateMaps/Affine.hpp"
#include "Domain/CoordinateMaps/CoordinateMap.hpp"
#include "Domain/CoordinateMaps/CoordinateMap.tpp"
#include "Domain/CoordinateMaps/DiscreteRotation.hpp"
#include "Domain/Creators/DomainCreator.hpp"
#include "Domain/Creators/RotatedIntervals.hpp"
#include "Domain/Domain.hpp"
#include "Domain/OptionTags.hpp"
#include "Domain/Structure/BlockNeighbor.hpp"  // IWYU pragma: keep
#include "Domain/Structure/Direction.hpp"
#include "Domain/Structure/DirectionMap.hpp"
#include "Domain/Structure/OrientationMap.hpp"
#include "Framework/TestCreation.hpp"
#include "Framework/TestHelpers.hpp"
#include "Helpers/Domain/BoundaryConditions/BoundaryCondition.hpp"
#include "Helpers/Domain/DomainTestHelpers.hpp"
#include "Parallel/RegisterDerivedClassesWithCharm.hpp"
#include "Utilities/MakeVector.hpp"

namespace domain {
namespace {
void test_rotated_intervals_construction(
    const creators::RotatedIntervals& rotated_intervals,
    const std::array<double, 1>& lower_bound,
    const std::array<double, 1>& midpoint,
    const std::array<double, 1>& upper_bound,
    const std::vector<std::array<size_t, 1>>& expected_extents,
    const std::vector<std::array<size_t, 1>>& expected_refinement_level,
    const std::vector<DirectionMap<1, BlockNeighbor<1>>>&
        expected_block_neighbors,
    const std::vector<std::unordered_set<Direction<1>>>&
        expected_external_boundaries) noexcept {
  const auto domain = rotated_intervals.create_domain();

  CHECK(domain.blocks().size() == expected_extents.size());
  CHECK(domain.blocks().size() == expected_refinement_level.size());
  CHECK(rotated_intervals.initial_extents() == expected_extents);
  CHECK(rotated_intervals.initial_refinement_levels() ==
        expected_refinement_level);

  test_domain_construction(
      domain, expected_block_neighbors, expected_external_boundaries,
      make_vector(
          make_coordinate_map_base<Frame::Logical, Frame::Inertial>(
              CoordinateMaps::Affine{-1., 1., lower_bound[0], midpoint[0]}),
          make_coordinate_map_base<Frame::Logical, Frame::Inertial>(
              CoordinateMaps::DiscreteRotation<1>{OrientationMap<1>{
                  std::array<Direction<1>, 1>{{Direction<1>::lower_xi()}}}},
              CoordinateMaps::Affine{-1., 1., midpoint[0], upper_bound[0]})));
  test_initial_domain(domain, rotated_intervals.initial_refinement_levels());

  Parallel::register_classes_in_list<
      typename domain::creators::RotatedIntervals::maps_list>();
  test_serialization(domain);
}

void test_rotated_intervals() {
  INFO("Rotated intervals");
  const std::vector<std::array<size_t, 1>> grid_points{{{4}}, {{2}}};
  const std::vector<std::array<size_t, 1>> refinement_level{{{0}}, {{0}}};
  const std::array<double, 1> lower_bound{{-1.2}};
  const std::array<double, 1> midpoint{{-0.6}};
  const std::array<double, 1> upper_bound{{0.8}};
  const OrientationMap<1> flipped{
      std::array<Direction<1>, 1>{{Direction<1>::lower_xi()}}};

  const creators::RotatedIntervals rotated_intervals{
      lower_bound,         midpoint,
      upper_bound,         std::array<bool, 1>{{false}},
      refinement_level[0], {{{{grid_points[0][0], grid_points[1][0]}}}}};
  test_rotated_intervals_construction(
      rotated_intervals, lower_bound, midpoint, upper_bound, grid_points,
      refinement_level,
      std::vector<DirectionMap<1, BlockNeighbor<1>>>{
          {{Direction<1>::upper_xi(), {1, flipped}}},
          {{Direction<1>::upper_xi(), {0, flipped}}}},
      std::vector<std::unordered_set<Direction<1>>>{
          {Direction<1>::lower_xi()}, {Direction<1>::lower_xi()}});
  test_physical_separation(rotated_intervals.create_domain().blocks());

  const creators::RotatedIntervals periodic_rotated_intervals{
      lower_bound,         midpoint,
      upper_bound,         std::array<bool, 1>{{true}},
      refinement_level[0], {{{{grid_points[0][0], grid_points[1][0]}}}}};
  test_rotated_intervals_construction(
      periodic_rotated_intervals, lower_bound, midpoint, upper_bound,
      grid_points, refinement_level,
      std::vector<DirectionMap<1, BlockNeighbor<1>>>{
          {{Direction<1>::lower_xi(), {1, flipped}},
           {Direction<1>::upper_xi(), {1, flipped}}},
          {{Direction<1>::lower_xi(), {0, flipped}},
           {Direction<1>::upper_xi(), {0, flipped}}}},
      std::vector<std::unordered_set<Direction<1>>>{{}, {}});
}

void test_rotated_intervals_factory() {
  INFO("Rotated intervals factory");
  const OrientationMap<1> flipped{
      std::array<Direction<1>, 1>{{Direction<1>::lower_xi()}}};
  const auto domain_creator = TestHelpers::test_factory_creation<
      DomainCreator<1>, domain::OptionTags::DomainCreator<1>,
      TestHelpers::domain::BoundaryConditions::
          MetavariablesWithoutBoundaryConditions<1>>(
      "RotatedIntervals:\n"
      "  LowerBound: [0.0]\n"
      "  Midpoint:   [0.5]\n"
      "  UpperBound: [1.0]\n"
      "  IsPeriodicIn: [True]\n"
      "  InitialGridPoints: [[3,2]]\n"
      "  InitialRefinement: [2]\n");
  const auto* rotated_intervals_creator =
      dynamic_cast<const creators::RotatedIntervals*>(domain_creator.get());
  test_rotated_intervals_construction(
      *rotated_intervals_creator, {{0.0}}, {{0.5}}, {{1.0}}, {{{3}}, {{2}}},
      {{{2}}, {{2}}},
      std::vector<DirectionMap<1, BlockNeighbor<1>>>{
          {{Direction<1>::lower_xi(), {1, flipped}},
           {Direction<1>::upper_xi(), {1, flipped}}},
          {{Direction<1>::lower_xi(), {0, flipped}},
           {Direction<1>::upper_xi(), {0, flipped}}}},
      std::vector<std::unordered_set<Direction<1>>>{{}, {}});
}
}  // namespace

SPECTRE_TEST_CASE("Unit.Domain.Creators.RotatedIntervals.Factory",
                  "[Domain][Unit]") {
  test_rotated_intervals();
  test_rotated_intervals_factory();
}
}  // namespace domain
