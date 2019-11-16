cmake_minimum_required(VERSION 3.10)

set(BUILDINGS_SRC
        src/buildings/cArena.h
        src/buildings/cArena.cpp
        src/buildings/cBrothel.h
        src/buildings/cBrothel.cpp
        src/buildings/cCentre.h
        src/buildings/cCentre.cpp
        src/buildings/cClinic.h
        src/buildings/cClinic.cpp
        src/buildings/cDungeon.h
        src/buildings/cDungeon.cpp
        src/buildings/cFarm.h
        src/buildings/cFarm.cpp
        src/buildings/cHouse.h
        src/buildings/cHouse.cpp
        src/buildings/cMovieStudio.h
        src/buildings/cMovieStudio.cpp
        src/buildings/queries.cpp)

set(INTERFACE_SRC
        src/interface/cColor.cpp
        src/interface/cAnimatedSurface.cpp
        src/interface/cFont.cpp
        src/interface/cInterfaceWindow.cpp
        src/interface/cSurface.cpp
        src/interface/cWindowManager.cpp
        src/interface/cMessageBox.cpp
        src/interface/cChoiceMessage.cpp
        src/interface/CGraphics.cpp
        src/interface/cImageCache.cpp
        )

set(SCREENS_SRC
        src/screens/BuildingScreenManagement.cpp
        src/screens/cScreenBank.cpp
        src/screens/cScreenBrothelManagement.cpp
        src/screens/cScreenBuildingSetup.cpp
        src/screens/cScreenDungeon.cpp
        src/screens/cScreenGallery.cpp
        src/screens/cScreenGangs.cpp
        src/screens/cScreenGetInput.cpp
        src/screens/cScreenGirlDetails.cpp
        src/screens/cScreenHouseDetails.cpp
        src/screens/cScreenItemManagement.cpp
        src/screens/cScreenLoadGame.cpp
        src/screens/cScreenLoadGame.hpp
        src/screens/cScreenMainMenu.cpp
        src/screens/cScreenMayor.cpp
        src/screens/cScreenMovieMaker.cpp
        src/screens/cScreenNewGame.cpp
        src/screens/cScreenPreparingGame.cpp
        src/screens/cScreenPrison.cpp
        src/screens/cScreenPropertyManagement.cpp
        src/screens/cScreenSettings.cpp
        src/screens/cScreenSlaveMarket.cpp
        src/screens/cScreenTown.cpp
        src/screens/cScreenTransfer.cpp
        src/screens/cScreenTurnSummary.cpp
        )

set(WIDGETS_SRC
        src/widgets/cButton.cpp
        src/widgets/cListBox.cpp
        src/widgets/cCheckBox.cpp
        src/widgets/cSlider.cpp
        src/widgets/cEditBox.cpp
        src/widgets/cTextItem.cpp
        src/widgets/cScrollBar.cpp
        src/widgets/cImageItem.cpp
        )

set(JOBS_SRC
        src/jobs/WorkAdvertising.cpp
        src/jobs/WorkArenaJousting.cpp
        src/jobs/WorkArenaMagicDual.cpp
        src/jobs/WorkArenaRacing.cpp
        src/jobs/WorkBaker.cpp
        src/jobs/WorkBarCook.cpp
        src/jobs/WorkBarmaid.cpp
        src/jobs/WorkBarPiano.cpp
        src/jobs/WorkBarSinger.cpp
        src/jobs/WorkBarStripper.cpp
        src/jobs/WorkBarWaitress.cpp
        src/jobs/WorkBeastCapture.cpp
        src/jobs/WorkBeastCare.cpp
        src/jobs/WorkBlacksmith.cpp
        src/jobs/WorkBoobJob.cpp
        src/jobs/WorkBreastReduction.cpp
        src/jobs/WorkBrewer.cpp
        src/jobs/WorkBrothelMassusse.cpp
        src/jobs/WorkBrothelStripper.cpp
        src/jobs/WorkWhore.cpp
        src/jobs/WorkButcher.cpp
        src/jobs/WorkCameraMage.cpp
        src/jobs/WorkCatacombRancher.cpp
        src/jobs/WorkCentreAngerManagement.cpp
        src/jobs/WorkCentreExTherapy.cpp
        src/jobs/WorkCentreManager.cpp
        src/jobs/WorkCentreTherapy.cpp
        src/jobs/WorkChairMan.cpp
        src/jobs/WorkCityGuard.cpp
        src/jobs/WorkCleanArena.cpp
        src/jobs/WorkCleanCentre.cpp
        src/jobs/WorkCleanHouse.cpp
        src/jobs/WorkCleaning.cpp
        src/jobs/WorkCobbler.cpp
        src/jobs/WorkCombatTraining.cpp
        src/jobs/WorkComunityService.cpp
        src/jobs/WorkCosmeticSurgery.cpp
        src/jobs/WorkCounselor.cpp
        src/jobs/WorkCrystalPurifier.cpp
        src/jobs/WorkCureDiseases.cpp
        src/jobs/WorkCustService.cpp
        src/jobs/WorkDoctor.cpp
        src/jobs/WorkDoctore.cpp
        src/jobs/WorkEscort.cpp
        src/jobs/WorkExploreCatacombs.cpp
        src/jobs/WorkFakeOrgasm.cpp
        src/jobs/WorkFarmer.cpp
        src/jobs/WorkFarmHand.cpp
        src/jobs/WorkFarmManager.cpp
        src/jobs/WorkFarmMarketer.cpp
        src/jobs/WorkFarmPonyGirl.cpp
        src/jobs/WorkFarmResearch.cpp
        src/jobs/WorkFarmVeterinarian.cpp
        src/jobs/WorkFeedPoor.cpp
        src/jobs/WorkFightArenaGirls.cpp
        src/jobs/WorkFightBeast.cpp
        src/jobs/WorkFilmAction.cpp
        src/jobs/WorkFilmAnal.cpp
        src/jobs/WorkFilmBeast.cpp
        src/jobs/WorkFilmBondage.cpp
        src/jobs/WorkFilmBuk.cpp
        src/jobs/WorkFilmChef.cpp
        src/jobs/WorkFilmDirector.cpp
        src/jobs/WorkFilmDominatrix.cpp
        src/jobs/WorkFilmFootJob.cpp
        src/jobs/WorkFilmGroup.cpp
        src/jobs/WorkFilmHandJob.cpp
        src/jobs/WorkFilmLesbian.cpp
        src/jobs/WorkFilmMast.cpp
        src/jobs/WorkFilmMusic.cpp
        src/jobs/WorkFilmOral.cpp
        src/jobs/WorkFilmPromoter.cpp
        src/jobs/WorkFilmPublicBDSM.cpp
        src/jobs/WorkFilmRandom.cpp
        src/jobs/WorkFilmSex.cpp
        src/jobs/WorkFilmStagehand.cpp
        src/jobs/WorkFilmStrip.cpp
        src/jobs/WorkFilmTease.cpp
        src/jobs/WorkFilmThroat.cpp
        src/jobs/WorkFilmTitty.cpp
        src/jobs/WorkFluffer.cpp
        src/jobs/WorkFreetime.cpp
        src/jobs/WorkGardener.cpp
        src/jobs/WorkGetAbort.cpp
        src/jobs/WorkGetAssJob.cpp
        src/jobs/WorkGetFacelift.cpp
        src/jobs/WorkGetFertility.cpp
        src/jobs/WorkGetTubesTied.cpp
        src/jobs/WorkGetVaginalRejuvenation.cpp
        src/jobs/WorkHallDealer.cpp
        src/jobs/WorkHallEntertainer.cpp
        src/jobs/WorkHallXXXEntertainer.cpp
        src/jobs/WorkHeadGirl.cpp
        src/jobs/WorkHealing.cpp
        src/jobs/WorkHouseCook.cpp
        src/jobs/WorkHousePet.cpp
        src/jobs/WorkHouseVacation.cpp
        src/jobs/WorkIntern.cpp
        src/jobs/WorkJanitor.cpp
        src/jobs/WorkJeweler.cpp
        src/jobs/WorkLiposuction.cpp
        src/jobs/WorkMakeItem.cpp
        src/jobs/WorkMakePotions.cpp
        src/jobs/WorkMatron.cpp
        src/jobs/WorkMechanic.cpp
        src/jobs/WorkMilk.cpp
        src/jobs/WorkMilker.cpp
        src/jobs/WorkNurse.cpp
        src/jobs/WorkPeepShow.cpp
        src/jobs/WorkPersonalBedWarmer.cpp
        src/jobs/WorkPersonalTraining.cpp
        src/jobs/WorkPhysicalSurgery.cpp
        src/jobs/WorkRancher.cpp
        src/jobs/WorkRecruiter.cpp
        src/jobs/WorkRehab.cpp
        src/jobs/WorkRepairShop.cpp
        src/jobs/WorkSecurity.cpp
        src/jobs/WorkShepherd.cpp
        src/jobs/WorkSleazyBarmaid.cpp
        src/jobs/WorkSleazyWaitress.cpp
        src/jobs/WorkSOBisexual.cpp
        src/jobs/WorkSOLesbian.cpp
        src/jobs/WorkSOStraight.cpp
        src/jobs/WorkTailor.cpp
        src/jobs/WorkTorturer.cpp
        )

set(SRC
    cCustomers.cpp
    cEvents.cpp
    cGameScript.cpp
    cGangs.cpp
    cGirlGangFight.cpp
    cGirls.cpp
    cGirlTorture.cpp
    cGold.cpp
    cInventory.cpp
    cJobManager.cpp
    CLog.cpp
    cLuaMenu.cpp
    cLuaScript.cpp
    cNameList.cpp
    cPlayer.cpp
        cRival.cpp
    cRng.cpp
    cScriptManager.cpp
    cScripts.cpp
    cScriptUtils.cpp
    cTariff.cpp
    cTrainable.cpp
    cTraits.cpp
    cTriggers.cpp
    DirPath.cpp
    FileList.cpp
    GameFlags.cpp
    InterfaceGlobals.cpp
    InterfaceProcesses.cpp
    main.cpp
    sConfig.cpp
        tinystr.cpp
    tinyxml.cpp
    tinyxmlerror.cpp
    tinyxmlparser.cpp
    XmlMisc.cpp
    XmlUtil.cpp
    SDL_anigif.c

    cCustomers.h
    cEvents.h
    cGameScript.h
    cGangs.h
        cGirlGangFight.h
    cGirls.h
    cGirlTorture.h
    cGold.h
    cInventory.h
    cJobManager.h
    CLog.h
    cLuaMenu.h
    cLuaScript.h
    cNameList.h
    Constants.h
    Constants.cpp
    cPlayer.h
        cRival.h
    cRng.h
    cScriptManager.h
    cScripts.h
    cScriptUtils.h
    cTariff.h
    cTrainable.h
    cTraits.h
    cTriggers.h
    cXmlWidget.h
    DirPath.h
    FileList.h
    GameFlags.h
        InterfaceProcesses.h
    linux.h
    main.h
    MasterFile.h
    resource.h
    Revision.h
    sConfig.h
    SDL_anigif.h
        tinystr.h
    tinyxml.h
    XmlMisc.h
    XmlUtil.h
        src/cShop.cpp
        cRandomGirl.cpp
        cRandomGirl.h
        cObjectiveManager.cpp
        cObjectiveManager.hpp
        src/IBuilding.hpp
        src/IBuilding.cpp src/Game.cpp src/Game.hpp src/sStorage.cpp src/sStorage.hpp src/Inventory.cpp src/Inventory.hpp src/utils/streaming_random_selection.hpp src/sGirl.cpp src/sGirl.hpp src/buildings/queries.hpp src/gang_missions.hpp src/gang_missions.cpp src/utils/algorithms.hpp)

# build own lua
add_library(lua lua/src/lapi.h
        lua/src/print.c
        lua/src/lzio.h
        lua/src/lzio.c
        lua/src/lvm.h
        lua/src/lvm.c
        lua/src/lundump.h
        lua/src/lundump.c
        lua/src/lualib.h
        lua/src/luaconf.h
        lua/src/lua.h
        lua/src/lua.c
        lua/src/ltm.h
        lua/src/ltm.c
        lua/src/ltablib.c
        lua/src/ltable.h
        lua/src/ltable.c
        lua/src/lstrlib.c
        lua/src/lstring.h
        lua/src/lstring.c
        lua/src/lstate.h
        lua/src/lstate.c
        lua/src/lparser.h
        lua/src/lparser.c
        lua/src/loslib.c
        lua/src/lopcodes.h
        lua/src/lopcodes.c
        lua/src/lobject.h
        lua/src/lobject.c
        lua/src/loadlib.c
        lua/src/lmem.h
        lua/src/lmem.c
        lua/src/lmathlib.c
        lua/src/llimits.h
        lua/src/llex.h
        lua/src/llex.c
        lua/src/liolib.c
        lua/src/linit.c
        lua/src/lgc.h
        lua/src/lgc.c
        lua/src/lfunc.c
        lua/src/lfunc.h
        lua/src/ldump.c
        lua/src/ldo.h
        lua/src/ldo.c
        lua/src/ldebug.h
        lua/src/ldebug.c
        lua/src/ldblib.c
        lua/src/lcode.c
        lua/src/lcode.h
        lua/src/lbaselib.c
        lua/src/lauxlib.h
        lua/src/lauxlib.c
        lua/src/lapi.c
        lua/src/lapi.h)

target_include_directories(lua PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lua/src)

# make imported targets for SDL
find_package(SDL 1.2)
add_library(SDL INTERFACE)
target_link_libraries(SDL INTERFACE ${SDL_LIBRARY})
message(STATUS ${SDL_INCLUDE_DIR})
target_include_directories(SDL INTERFACE ${SDL_INCLUDE_DIR})

find_package(SDL_image 1.2 REQUIRED)
add_library(SDL_image INTERFACE)
target_link_libraries(SDL_image INTERFACE ${SDL_IMAGE_LIBRARIES} )
target_include_directories(SDL INTERFACE ${SDL_IMAGE_INCLUDE_DIRS})

find_package(SDL_ttf 2.0 REQUIRED)
add_library(SDL_ttf INTERFACE)
target_link_libraries(SDL INTERFACE ${SDL_TTF_LIBRARIES})
target_include_directories(SDL_ttf INTERFACE ${SDL_TTF_INCLUDE_DIRS})

# there is no find_package(SDL_gfx), unfortunately.
add_executable(WhoreMaster ${SRC} ${BUILDINGS_SRC} ${SCREENS_SRC} ${WIDGETS_SRC} ${JOBS_SRC} ${INTERFACE_SRC})
target_link_libraries(WhoreMaster SDL SDL_gfx SDL_ttf lua SDL_image)
set_target_properties(WhoreMaster PROPERTIES CXX_STANDARD 14)

if (UNIX)
    target_compile_definitions(WhoreMaster PUBLIC LINUX=1)
endif(UNIX)

target_include_directories(WhoreMaster PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/src)

# libsdl-ttf2.0-dev
