#pragma once

#include <QDialog>
#include <QLabel>
#include <QElapsedTimer>
#include <QUrl>
#include <QNetworkRequest>
#include <QByteArray>
#include <json/json.h>

class QNetworkAccessManager;
class QNetworkReply;

//namespace HBVideoPlatform {
//namespace common {

class HttpDlg : public QDialog
{
	Q_OBJECT

public:
	enum class HttpDlgGif {
		Spinner1s_200px_gray,
		Spinner1s_200px_blue,

	};

	enum class Method {
		Get,
		Post,
		Put,
		Patch,
		Delete,
	};

	HttpDlg(QWidget *parent = nullptr, int timeOut = 10, int retries = 0, HttpDlgGif gif = HttpDlgGif::Spinner1s_200px_gray);
	~HttpDlg();

	void get(const QUrl& url);
	void get(const QNetworkRequest& request);
	void post(const QUrl& url);
	void post(const QNetworkRequest& request);
	void post(const QNetworkRequest& request, const QByteArray& data);
	void put(const QNetworkRequest& request, const QByteArray& data = {});
	void patch(const QNetworkRequest& request, const QByteArray& data = {});
	void deleteResource(const QNetworkRequest& request);


	std::error_code get_result() const { return result_; }

	Json::Value getRoot() const { return root_; }

	bool getValue(const QString& name, Json::Value& value) {
		if (root_.isMember(name.toStdString())) {
			value = root_[name.toStdString()];
			return true;
		}
		return false;
	}

	bool getValue(const QString& name, QString& value) {
		Json::Value val;
		if (getValue(name, val)) {
			value = QString::fromUtf8(val.asCString());
			return true;
		}
		return false;
	}

	bool getValue(const QString& name, int& value) {
		Json::Value val;
		if (getValue(name, val)) {
			value = val.asInt();
			return true;
		}
		return false;
	}

	int getHttpStatusCode() const { return httpStatusCode_; }
	QString getHttpReason() const { return httpReason_; }
	
#define break_if_parse_int_value_failed(json, name, default_value) \
int name = default_value; \
if (!JsoncppHelper::safelyGetIntValue(json, #name, name)) { \
	ec = common::HttpDlgErrorCode::ParseJsonError; \
	break; \
}

#define break_if_parse_string_value_failed(json, name, default_value) \
QString name = default_value; \
if (!JsoncppHelper::safelyGetStringValue(json, #name, name)) { \
	ec = common::HttpDlgErrorCode::ParseJsonError; \
	break; \
}

protected:
	QString getGifPath();
	void run();
	virtual void timerEvent(QTimerEvent * e) override;

private slots:
	void onFinished(QNetworkReply* reply);

private:
	std::error_code result_ = {};
	int httpStatusCode_ = 0;
	QString httpReason_ = {};
	Json::Value root_ = {};
	const int timeout;
	int time_out_sec_ = 10;
	int timer_id_ = 0;
	const int retrys;
	int retry_counter = 0;
	QNetworkAccessManager* mgr{};
	QNetworkRequest lastRequest_{};
	Method lastMethod_ = Method::Post;
	QByteArray lastData_{};
	QNetworkReply* reply_{};
	QMetaObject::Connection connection_ = {};

	QLabel* label_ = {};
	QLabel* elapse_ = {};
	HttpDlgGif gif_ = HttpDlgGif::Spinner1s_200px_gray;
	QElapsedTimer timer_ = {};

};

//}
//}
