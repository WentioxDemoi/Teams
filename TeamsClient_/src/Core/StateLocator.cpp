#include "StateLocator.h"

StateLocator& StateLocator::instance() {
  static StateLocator locator;
  return locator;
}

void StateLocator::clear() {
  states_.clear();
}
