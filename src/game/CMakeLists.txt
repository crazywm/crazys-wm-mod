set(SCREENS_SRC
        screens/BuildingScreenManagement.cpp
        screens/cScreenBank.cpp
        screens/cScreenBrothelManagement.cpp
        screens/cScreenBuildingSetup.cpp
        screens/cScreenDungeon.cpp
        screens/cScreenGallery.cpp
        screens/cScreenGangs.cpp
        screens/cScreenGirlDetails.cpp
        screens/cScreenHouseDetails.cpp
        screens/cScreenItemManagement.cpp
        screens/cScreenLoadGame.cpp
        screens/cScreenLoadGame.hpp
        screens/cScreenMainMenu.cpp
        screens/cScreenMayor.cpp
        screens/cScreenMovieMaker.cpp
        screens/cScreenMarketResearch.cpp
        screens/cScreenNewGame.cpp
        screens/cScreenPreparingGame.cpp
        screens/cScreenPrison.cpp
        screens/cScreenSettings.cpp
        screens/cScreenSlaveMarket.cpp
        screens/cScreenTown.cpp
        screens/cScreenTransfer.cpp
        screens/cScreenTurnSummary.cpp
        screens/cGameWindow.cpp
        screens/cScreenGameConfig.cpp)

set(JOBS_SRC
        jobs/WorkFreetime.cpp
        jobs/Crafting.cpp
        jobs/GenericJob.cpp
        jobs/Matron.cpp
        jobs/JobData.cpp
        jobs/Cleaning.cpp buildings/centre/CentreJobs.cpp jobs/BasicJob.cpp buildings/brothel/StripClub.cpp buildings/brothel/Escort.cpp)

set(BUILDINGS_SRC
        buildings/arena/cArena.cpp
        buildings/brothel/cBrothel.cpp
        buildings/centre/cCentre.cpp
        buildings/cDungeon.cpp
        buildings/farm/cFarm.cpp
        buildings/house/cHouse.cpp
        buildings/studio/cMovieStudio.cpp
        buildings/queries.cpp
        buildings/cBuildingManager.cpp
        buildings/brothel/WorkAdvertising.cpp
        buildings/house/Training.cpp
        buildings/brothel/WorkBeastCare.cpp
        buildings/brothel/WorkBrothelStripper.cpp
        buildings/brothel/WorkWhore.cpp
        buildings/studio/StudioCrew.cpp
        buildings/brothel/WorkCustService.cpp
        buildings/brothel/WorkExploreCatacombs.cpp
        buildings/farm/WorkFarmPonyGirl.cpp
        buildings/clinic/cClinic.cpp
        buildings/clinic/Surgery.cpp
        buildings/clinic/ClinicJobs.cpp
        buildings/brothel/WorkPeepShow.cpp
        buildings/house/WorkPersonalBedWarmer.cpp
        buildings/brothel/WorkSecurity.cpp
        buildings/brothel/WorkTorturer.cpp
        buildings/studio/StudioFilming.cpp
        buildings/centre/Therapy.cpp
        buildings/brothel/BarJobs.cpp
        buildings/farm/FarmJobs.cpp
        buildings/studio/StudioScenes.cpp
        buildings/studio/StudioOther.cpp
        buildings/arena/ArenaJobs.cpp
        buildings/house/House.cpp
)



set(CHARACTER_SRC
        character/cCustomers.cpp
        character/cPlayer.cpp
        character/ICharacter.cpp
        character/sAttribute.cpp
        character/cSkillCap.cpp
        character/sGirl.cpp
        character/traits/cTraitsCollection.cpp
        character/traits/cTraitsManager.cpp
        character/traits/cTraitSpec.cpp
        character/predicates.cpp
        character/pregnancy.cpp
        character/cGirlPool.cpp
        )

set(SCRIPTING_SRC
        scripting/cEventMapping.cpp
        scripting/cLuaScript.cpp
        scripting/cLuaState.cpp
        scripting/cScriptManager.cpp
        scripting/sLuaGirl.cpp
        scripting/sLuaParameter.cpp
        )

set(MOVIES_SRC
        buildings/studio/manager.cpp
        buildings/studio/data.cpp
        buildings/studio/filming.cpp
        )

set(SRC cEvents.cpp
        cGangs.cpp
        cGangManager.cpp
        cGirlGangFight.cpp
        cGirls.cpp
        cGirlTorture.cpp
        cGold.cpp
        cInventory.cpp
        cJobManager.cpp
        cNameList.cpp
        cObjectiveManager.cpp
        Constants.cpp
        cRandomGirl.cpp
        cRival.cpp
        cShop.cpp
        cTariff.cpp
        IGame.cpp
        gang_missions.cpp
        buildings/IBuilding.cpp
        InterfaceGlobals.cpp
        InterfaceProcesses.cpp
        Inventory.cpp
        sStorage.cpp
        XmlMisc.cpp
        cGameSettings.cpp
        combat/combat.cpp
        combat/action.cpp
        combat/combatant.cpp
        cGame.cpp
        SavesList.cpp)

add_library(WmGameObjects OBJECT ${BUILDINGS_SRC} ${SCREENS_SRC} ${JOBS_SRC} ${CHARACTER_SRC} ${SCRIPTING_SRC} ${MOVIES_SRC} ${SRC})
target_link_libraries(WmGameObjects PUBLIC WMEngineObjects pthread doctest::doctest)
set_target_properties(WmGameObjects PROPERTIES CXX_STANDARD 14)
target_include_directories(WmGameObjects PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
if (UNIX)
    target_compile_definitions(WmGameObjects PUBLIC LINUX=1)
endif(UNIX)

add_executable(WhoreMaster $<TARGET_OBJECTS:WmGameObjects> $<TARGET_OBJECTS:WMEngineObjects> main.cpp)
target_link_libraries(WhoreMaster PUBLIC WmGameObjects)

add_executable(WhoreMasterFuzz $<TARGET_OBJECTS:WmGameObjects> $<TARGET_OBJECTS:WMEngineObjects> fuzzmain.cpp)
target_link_libraries(WhoreMasterFuzz PUBLIC WmGameObjects)

add_executable(WhoreMasterTest $<TARGET_OBJECTS:WmGameObjects> $<TARGET_OBJECTS:WMEngineObjects> unittest.cpp ../engine/text/conditions_tests.cpp
        cGirlsTests.cpp
        )
target_link_libraries(WhoreMasterTest PUBLIC WmGameObjects)
target_include_directories(WhoreMasterTest PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

set_target_properties(WhoreMaster WhoreMasterFuzz WhoreMasterTest PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/../..")
