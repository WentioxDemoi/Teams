#include "ViewModelsLocator.h"

ViewModelsLocator &ViewModelsLocator::instance()
{
  static ViewModelsLocator instance;
  return instance;
}

void ViewModelsLocator::clear() { viewModels_.clear(); }