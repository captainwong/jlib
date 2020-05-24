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
        qCritical() << QString::fromLocal8Bit(jlib::win32::u16_to_mbcs(msg).data());
    }, false, false);
    auto json = jlib::win32::toJson<Json::Value>(pinfos);
    auto msg = Json::StyledWriter().write(json);
    qDebug() << msg.data();

    return a.exec();
}
