#pragma once

#include "qt_global.h"
#include <QString>
#include <QStringList>
#include <QVariant>
#include <QVector>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>


JLIBQT_NAMESPACE_BEGIN


struct Col {
	QString name{};
	QVariant val{};

	QString bindableName() const {
		return QString("%1=:%2").arg(name).arg(name);
	}

	void bindValue(QSqlQuery& query) const {
		query.bindValue(":" + name, val);
	}
};

using Cols = QVector<Col>;

inline QString bindableCols(const Cols& cols) {
	QStringList strs;
	for (const auto& col : cols) {
		strs.append(col.bindableName());
	}
	return strs.join(",");
}

struct SqlHelper {
	QString lastQuery{};
	QSqlError lastError{};

	bool update(const QSqlDatabase& db, const QString& table, const Cols& cols, const Col& key) {
		QSqlQuery query(db);
		query.prepare(QString("update %1 set %2 where %3").arg(table).arg(bindableCols(cols)).arg(key.bindableName()));
		for (const auto& col : cols) {
			col.bindValue(query);
		}
		key.bindValue(query);
		if (query.exec()) {
			return true;
		} else {
			lastQuery = query.lastQuery();
			lastError = query.lastError();
			return false;
		}
	}
};



JLIBQT_NAMESPACE_END
