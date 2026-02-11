#include "ViewLocator.h"

ViewLocator &ViewLocator::instance()
{
  static ViewLocator instance;
  return instance;
}

void ViewLocator::clear() { views_.clear(); }