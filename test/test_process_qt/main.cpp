#include <QtCore/QCoreApplication>
#include <qstring.h>
#include <qdebug.h>
#include "../../jlib/win32/process.h"
#include "../../jlib/3rdparty/json/jsoncpp/json.h"
#include <locale.h>
#include "../../jlib/win32/unicodetool.h"

int main(int argc, char *argv[])
{
    setlocale(LC_ALL, "");
    QCoreApplication a(argc, argv);

    auto pinfos = jlib::win32::getProcessesInfo([](const std::wstring& msg) {
        qCritical() << jlib::win32::utf16_to_utf8(msg).data();
    }, false, false);

    if (0) {
        auto json = jlib::win32::toJson<Json::Value>(pinfos);
        auto msg = Json::StyledWriter().write(json);
        qDebug() << msg.data();
    }

    std::unordered_set<std::wstring> processes;
    for (const auto& info : pinfos) {
        if (processes.find(info.name) != processes.end()) { continue; }
        processes.insert(info.name);
        //Json::Value v;
        //snapshot[win32::utf16_to_utf8(info.name)] = win32::utf16_to_utf8(info.path);
        //v["path"] = win32::u16_to_mbcs(info.path);
        //snapshot.append(v);
        //printf("%s:%s\n", jlib::win32::utf16_to_utf8(info.name).data(), jlib::win32::utf16_to_utf8(info.path).data());
        qDebug() << jlib::win32::utf16_to_utf8(info.name).data() << jlib::win32::utf16_to_utf8(info.path).data();
    }
    //return a.exec();
}
