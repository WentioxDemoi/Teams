#include "Locator_test.h"

#include "Core/ModelLocator.h"
#include "Core/ServiceLocator.h"
#include "Core/StateLocator.h"
#include "Core/ViewModelsLocator.h"

#include <QtTest/QtTest>

namespace {
class TestService : public QObject {};
class ReplacementService : public QObject {};
class TestState : public QObject {};
class ReplacementState : public QObject {};
class TestModel : public QObject {};
class ReplacementModel : public QObject {};
class TestViewModel : public QObject {};
class ReplacementViewModel : public QObject {};
}

void LocatorTest::serviceLocatorRegistersReplacesAndClears() {
  ServiceLocator& locator = ServiceLocator::instance();
  locator.clear();
  TestService service;
  TestService secondService;
  ReplacementService replacement;

  QVERIFY(locator.getService<TestService>() == nullptr);
  locator.registerService(&service);
  QCOMPARE(locator.getService<TestService>(), &service);
  QVERIFY(locator.getService<ReplacementService>() == nullptr);

  locator.registerService(&replacement);
  QCOMPARE(locator.getService<ReplacementService>(), &replacement);
  locator.registerService(&secondService);
  QCOMPARE(locator.getService<TestService>(), &secondService);

  locator.registerService<TestService>(nullptr);
  QVERIFY(locator.getService<TestService>() == nullptr);
  locator.clear();
  QVERIFY(locator.getService<ReplacementService>() == nullptr);
}

void LocatorTest::stateLocatorRegistersReplacesAndClears() {
  StateLocator& locator = StateLocator::instance();
  locator.clear();
  TestState state;
  TestState secondState;
  ReplacementState replacement;

  QVERIFY(locator.getState<TestState>() == nullptr);
  locator.registerState(&state);
  QCOMPARE(locator.getState<TestState>(), &state);
  locator.registerState(&replacement);
  QCOMPARE(locator.getState<ReplacementState>(), &replacement);
  locator.registerState(&secondState);
  QCOMPARE(locator.getState<TestState>(), &secondState);

  locator.registerState<TestState>(nullptr);
  QVERIFY(locator.getState<TestState>() == nullptr);
  locator.clear();
  QVERIFY(locator.getState<ReplacementState>() == nullptr);
}

void LocatorTest::modelLocatorRegistersReplacesAndClears() {
  ModelLocator& locator = ModelLocator::instance();
  locator.clear();
  TestModel model;
  TestModel secondModel;
  ReplacementModel replacement;

  QVERIFY(locator.getModel<TestModel>() == nullptr);
  locator.registerModel(&model);
  QCOMPARE(locator.getModel<TestModel>(), &model);
  locator.registerModel(&replacement);
  QCOMPARE(locator.getModel<ReplacementModel>(), &replacement);
  locator.registerModel(&secondModel);
  QCOMPARE(locator.getModel<TestModel>(), &secondModel);
  locator.registerModel<TestModel>(nullptr);
  QVERIFY(locator.getModel<TestModel>() == nullptr);
  locator.clear();
  QVERIFY(locator.getModel<ReplacementModel>() == nullptr);
}

void LocatorTest::viewModelsLocatorRegistersReplacesAndClears() {
  ViewModelsLocator& locator = ViewModelsLocator::instance();
  locator.clear();
  TestViewModel viewModel;
  TestViewModel secondViewModel;
  ReplacementViewModel replacement;

  QVERIFY(locator.getViewModels<TestViewModel>() == nullptr);
  locator.registerViewModels(&viewModel);
  QCOMPARE(locator.getViewModels<TestViewModel>(), &viewModel);
  locator.registerViewModels(&replacement);
  QCOMPARE(locator.getViewModels<ReplacementViewModel>(), &replacement);
  locator.registerViewModels(&secondViewModel);
  QCOMPARE(locator.getViewModels<TestViewModel>(), &secondViewModel);
  locator.registerViewModels<TestViewModel>(nullptr);
  QVERIFY(locator.getViewModels<TestViewModel>() == nullptr);
  locator.clear();
  QVERIFY(locator.getViewModels<ReplacementViewModel>() == nullptr);
}

void LocatorTest::locatorsAreSingletonsAndKeepNamespacesSeparate() {
  QVERIFY(&ServiceLocator::instance() == &ServiceLocator::instance());
  QVERIFY(&StateLocator::instance() == &StateLocator::instance());
  QVERIFY(&ModelLocator::instance() == &ModelLocator::instance());
  QVERIFY(&ViewModelsLocator::instance() == &ViewModelsLocator::instance());

  ServiceLocator::instance().clear();
  StateLocator::instance().clear();
  ModelLocator::instance().clear();
  ViewModelsLocator::instance().clear();

  TestService service;
  TestState state;
  TestModel model;
  TestViewModel viewModel;
  ServiceLocator::instance().registerService(&service);
  StateLocator::instance().registerState(&state);
  ModelLocator::instance().registerModel(&model);
  ViewModelsLocator::instance().registerViewModels(&viewModel);

  QVERIFY(ServiceLocator::instance().getService<TestState>() == nullptr);
  QVERIFY(StateLocator::instance().getState<TestService>() == nullptr);
  QVERIFY(ModelLocator::instance().getModel<TestViewModel>() == nullptr);
  QVERIFY(ViewModelsLocator::instance().getViewModels<TestModel>() == nullptr);

  ServiceLocator::instance().clear();
  StateLocator::instance().clear();
  ModelLocator::instance().clear();
  ViewModelsLocator::instance().clear();
}