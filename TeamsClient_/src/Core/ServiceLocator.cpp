#include "ServiceLocator.h"

ServiceLocator &ServiceLocator::instance()
{
    static ServiceLocator instance;
    return instance;
}