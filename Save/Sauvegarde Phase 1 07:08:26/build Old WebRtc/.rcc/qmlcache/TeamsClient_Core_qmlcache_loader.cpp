#include <QtQml/qqmlprivate.h>
#include <QtCore/qdir.h>
#include <QtCore/qurl.h>
#include <QtCore/qhash.h>
#include <QtCore/qstring.h>

namespace QmlCacheGeneratedCode {
namespace _qt_qml_TeamsClient_src_QML_Main_qml { 
    extern const unsigned char qmlData[];
    extern const QQmlPrivate::AOTCompiledFunction aotBuiltFunctions[];
    const QQmlPrivate::CachedQmlUnit unit = {
        reinterpret_cast<const QV4::CompiledData::Unit*>(&qmlData), &aotBuiltFunctions[0], nullptr
    };
}
namespace _qt_qml_TeamsClient_src_QML_AuthView_qml { 
    extern const unsigned char qmlData[];
    extern const QQmlPrivate::AOTCompiledFunction aotBuiltFunctions[];
    const QQmlPrivate::CachedQmlUnit unit = {
        reinterpret_cast<const QV4::CompiledData::Unit*>(&qmlData), &aotBuiltFunctions[0], nullptr
    };
}
namespace _qt_qml_TeamsClient_src_QML_WorkspaceView_qml { 
    extern const unsigned char qmlData[];
    extern const QQmlPrivate::AOTCompiledFunction aotBuiltFunctions[];
    const QQmlPrivate::CachedQmlUnit unit = {
        reinterpret_cast<const QV4::CompiledData::Unit*>(&qmlData), &aotBuiltFunctions[0], nullptr
    };
}
namespace _qt_qml_TeamsClient_src_QML_LoadingView_qml { 
    extern const unsigned char qmlData[];
    extern const QQmlPrivate::AOTCompiledFunction aotBuiltFunctions[];
    const QQmlPrivate::CachedQmlUnit unit = {
        reinterpret_cast<const QV4::CompiledData::Unit*>(&qmlData), &aotBuiltFunctions[0], nullptr
    };
}
namespace _qt_qml_TeamsClient_src_QML_HomeView_qml { 
    extern const unsigned char qmlData[];
    extern const QQmlPrivate::AOTCompiledFunction aotBuiltFunctions[];
    const QQmlPrivate::CachedQmlUnit unit = {
        reinterpret_cast<const QV4::CompiledData::Unit*>(&qmlData), &aotBuiltFunctions[0], nullptr
    };
}
namespace _qt_qml_TeamsClient_src_QML_MessageView_qml { 
    extern const unsigned char qmlData[];
    extern const QQmlPrivate::AOTCompiledFunction aotBuiltFunctions[];
    const QQmlPrivate::CachedQmlUnit unit = {
        reinterpret_cast<const QV4::CompiledData::Unit*>(&qmlData), &aotBuiltFunctions[0], nullptr
    };
}
namespace _qt_qml_TeamsClient_src_QML_ContactItem_qml { 
    extern const unsigned char qmlData[];
    extern const QQmlPrivate::AOTCompiledFunction aotBuiltFunctions[];
    const QQmlPrivate::CachedQmlUnit unit = {
        reinterpret_cast<const QV4::CompiledData::Unit*>(&qmlData), &aotBuiltFunctions[0], nullptr
    };
}
namespace _qt_qml_TeamsClient_src_QML_CallWindow_qml { 
    extern const unsigned char qmlData[];
    extern const QQmlPrivate::AOTCompiledFunction aotBuiltFunctions[];
    const QQmlPrivate::CachedQmlUnit unit = {
        reinterpret_cast<const QV4::CompiledData::Unit*>(&qmlData), &aotBuiltFunctions[0], nullptr
    };
}

}
namespace {
struct Registry {
    Registry();
    ~Registry();
    QHash<QString, const QQmlPrivate::CachedQmlUnit*> resourcePathToCachedUnit;
    static const QQmlPrivate::CachedQmlUnit *lookupCachedUnit(const QUrl &url);
};

Q_GLOBAL_STATIC(Registry, unitRegistry)


Registry::Registry() {
    resourcePathToCachedUnit.insert(QStringLiteral("/qt/qml/TeamsClient/src/QML/Main.qml"), &QmlCacheGeneratedCode::_qt_qml_TeamsClient_src_QML_Main_qml::unit);
    resourcePathToCachedUnit.insert(QStringLiteral("/qt/qml/TeamsClient/src/QML/AuthView.qml"), &QmlCacheGeneratedCode::_qt_qml_TeamsClient_src_QML_AuthView_qml::unit);
    resourcePathToCachedUnit.insert(QStringLiteral("/qt/qml/TeamsClient/src/QML/WorkspaceView.qml"), &QmlCacheGeneratedCode::_qt_qml_TeamsClient_src_QML_WorkspaceView_qml::unit);
    resourcePathToCachedUnit.insert(QStringLiteral("/qt/qml/TeamsClient/src/QML/LoadingView.qml"), &QmlCacheGeneratedCode::_qt_qml_TeamsClient_src_QML_LoadingView_qml::unit);
    resourcePathToCachedUnit.insert(QStringLiteral("/qt/qml/TeamsClient/src/QML/HomeView.qml"), &QmlCacheGeneratedCode::_qt_qml_TeamsClient_src_QML_HomeView_qml::unit);
    resourcePathToCachedUnit.insert(QStringLiteral("/qt/qml/TeamsClient/src/QML/MessageView.qml"), &QmlCacheGeneratedCode::_qt_qml_TeamsClient_src_QML_MessageView_qml::unit);
    resourcePathToCachedUnit.insert(QStringLiteral("/qt/qml/TeamsClient/src/QML/ContactItem.qml"), &QmlCacheGeneratedCode::_qt_qml_TeamsClient_src_QML_ContactItem_qml::unit);
    resourcePathToCachedUnit.insert(QStringLiteral("/qt/qml/TeamsClient/src/QML/CallWindow.qml"), &QmlCacheGeneratedCode::_qt_qml_TeamsClient_src_QML_CallWindow_qml::unit);
    QQmlPrivate::RegisterQmlUnitCacheHook registration;
    registration.structVersion = 0;
    registration.lookupCachedQmlUnit = &lookupCachedUnit;
    QQmlPrivate::qmlregister(QQmlPrivate::QmlUnitCacheHookRegistration, &registration);
}

Registry::~Registry() {
    QQmlPrivate::qmlunregister(QQmlPrivate::QmlUnitCacheHookRegistration, quintptr(&lookupCachedUnit));
}

const QQmlPrivate::CachedQmlUnit *Registry::lookupCachedUnit(const QUrl &url) {
    if (url.scheme() != QLatin1String("qrc"))
        return nullptr;
    QString resourcePath = QDir::cleanPath(url.path());
    if (resourcePath.isEmpty())
        return nullptr;
    if (!resourcePath.startsWith(QLatin1Char('/')))
        resourcePath.prepend(QLatin1Char('/'));
    return unitRegistry()->resourcePathToCachedUnit.value(resourcePath, nullptr);
}
}
int QT_MANGLE_NAMESPACE(qInitResources_qmlcache_TeamsClient_Core)() {
    ::unitRegistry();
    return 1;
}
Q_CONSTRUCTOR_FUNCTION(QT_MANGLE_NAMESPACE(qInitResources_qmlcache_TeamsClient_Core))
int QT_MANGLE_NAMESPACE(qCleanupResources_qmlcache_TeamsClient_Core)() {
    return 1;
}
