#pragma once

#include <QDialog>
#include <QLabel>
#include <QElapsedTimer>
#include <QUrl>
#include <QNetworkRequest>
#include <QByteArray>
#include <qmovie.h>
#include <qnetworkaccessmanager.h>
#include <qnetworkreply.h>
#include <system_error>
#include "../QtPathHelper.h"
#include "../QtUtils.h"
#include "../QtStylesheet.h"
#include "HttpDlgErrorCode.h"
#include <jlib/3rdparty/json/jsoncpp/json.h>

namespace jlib
{
namespace qt
{

class HttpDlg : public QDialog
{
	Q_OBJECT

public:
	enum HttpDlgViewSize {
		sz_big,
		sz_small,
	};

	HttpDlg(QWidget *parent = nullptr, HttpDlgViewSize sz = sz_big, int timeOut = 10)
		: QDialog(parent)
		, sz_(sz)
		, time_out_sec_(timeOut)
	{
		setWindowModality(Qt::WindowModal);
		setWindowFlags(Qt::FramelessWindowHint);
		if (!parent) { setAttribute(Qt::WA_TranslucentBackground); }

		if (sz == sz_small) { setFixedSize(200, 200);
		} else { setFixedSize(630, 637); }

		label_ = new QLabel(this); label_->resize(width(), height()); label_->move(0, 0);

		elapse_ = new QLabel(this); elapse_->resize(180, 80);
		elapse_->move((width() - elapse_->width()) / 2, (height() - elapse_->height()) / 2);
		elapse_->setStyleSheet(build_style(Qt::darkYellow, 64));
		elapse_->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		//elapse_->hide();

		parent ? center_to_parent(this, parent) : center_to_desktop(this);
		mgr = new QNetworkAccessManager(this);
	}

	~HttpDlg() {}

	void get(const QUrl& url) {
		if (connection_) { disconnect(connection_); }
		connection_ = connect(mgr, &QNetworkAccessManager::finished, this, &HttpDlg::onFinished);
		reply_ = mgr->get(QNetworkRequest(url));
		run();
	}

	void post(const QUrl& url) {
		if (connection_) { disconnect(connection_); }
		connection_ = connect(mgr, &QNetworkAccessManager::finished, this, &HttpDlg::onFinished);
		QNetworkRequest request(url);
		request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
		reply_ = mgr->post(request, QByteArray());
		run();
	}

	void post(const QNetworkRequest& request, const QByteArray& data) {
		if (connection_) { disconnect(connection_); }
		connection_ = connect(mgr, &QNetworkAccessManager::finished, this, &HttpDlg::onFinished);
		reply_ = mgr->post(request, data);
		run();
	}

	std::error_code get_result() const { return result_; }
	Json::Value getRoot() const { return root_; }

	bool getValue(const QString& name, Json::Value& value) {
		if (root_.isMember(name.toStdString())) {
			value = root_[name.toStdString()];
			return true;
		} return false;
	}

	bool getValue(const QString& name, QString& value) {
		Json::Value val;
		if (getValue(name, val)) {
			value = QString::fromUtf8(val.asCString());
			return true;
		} return false;
	}

	bool getValue(const QString& name, int& value) {
		Json::Value val;
		if (getValue(name, val)) {
			value = val.asInt();
			return true;
		} return false;
	}

	int getHttpStatusCode() const { return httpStatusCode_; }
	QString getHttpReason() const { return httpReason_; }

#define break_if_parse_int_value_failed(json, name, default_value) \
int name = default_value; \
if (!JsoncppHelper::safelyGetIntValue(json, #name, name)) { \
	ec = HttpDlgErrorCode::ParseJsonError; \
	break; \
}

#define break_if_parse_string_value_failed(json, name, default_value) \
QString name = default_value; \
if (!JsoncppHelper::safelyGetStringValue(json, #name, name)) { \
	ec = HttpDlgErrorCode::ParseJsonError; \
	break; \
}

protected:
	void run() {
		auto path = PathHelper::program();
		path += sz_ == sz_small ? "/Skin/gif/ajax-loader-small.gif" : "/Skin/gif/preloader.gif";
		auto movie = new QMovie(path);
		label_->setMovie(movie);
		movie->start();
		timer_.start();

		auto p = parentWidget();
		if (p) { p->setEnabled(false); }

		startTimer(1000);
		QDialog::exec();
		if (p) { p->setEnabled(true); }

		movie->deleteLater();
	}

	virtual void timerEvent(QTimerEvent * e) override {
		MYQDEBUG << time_out_sec_;
		if (--time_out_sec_ > 0) { elapse_->setText(QString::number(time_out_sec_)); } 
		else {
			MYQCRITICAL << "timeout";
			disconnect(connection_); result_ = HttpDlgErrorCode::NetworkError;
			QDialog::reject();
		}
	}

private slots:
	void onFinished(QNetworkReply* reply) {
		do {
			if (!reply) {
				MYQCRITICAL << "no reply"; result_ = HttpDlgErrorCode::NetworkError;
				break;
			}

			if (QNetworkReply::NoError != reply->error()) {
				httpReason_ = reply->errorString(); MYQCRITICAL << httpReason_;
				result_ = HttpDlgErrorCode::NetworkError;
				break;
			}

			QVariant statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
			if (!statusCode.isValid()) { MYQDEBUG << "statusCode is not valid"; break; }

			httpStatusCode_ = statusCode.toInt();
			if (httpStatusCode_ != 200) {
				result_ = HttpDlgErrorCode::NetworkError;
				httpReason_ = reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString();
				MYQCRITICAL << httpStatusCode_ << httpReason_;
				break;
			}

			auto res = reply->readAll();
			Json::Reader reader; root_.clear();
			if (!reader.parse(res.constData(), root_)) {
				//result_ = HttpDlgErrorCode::ParseJsonError;
				//break;
			}

			MYQDEBUG << reply->url() << "reply:\n" << root_.toStyledString().data();
		} while (false);

		QDialog::accept();
		reply->deleteLater();
	}

private:
	std::error_code result_ = {};
	int httpStatusCode_ = 0;
	QString httpReason_ = {};
	Json::Value root_ = {};
	int time_out_sec_ = 10;
	QNetworkAccessManager* mgr = {};
	QNetworkReply* reply_ = {};
	QMetaObject::Connection connection_ = {};

	QLabel* label_ = {};
	QLabel* elapse_ = {};
	HttpDlgViewSize sz_ = sz_big;
	QElapsedTimer timer_ = {};

};

}
}
