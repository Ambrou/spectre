# Distributed under the MIT License.
# See LICENSE.txt for details.

add_subdirectory(ConstitutiveRelations)

set(LIBRARY Test_ElasticPotentialEnergy)

set(LIBRARY_SOURCES
  Test_PotentialEnergy.cpp
  )

  add_test_library(
  ${LIBRARY}
  "PointwiseFunctions/Elasticity/"
  "${LIBRARY_SOURCES}"
  "ElasticPotentialEnergy;Utilities"
  )
