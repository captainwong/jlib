#include "HttpDlg.h"
#include <qmovie.h>
#include <qnetworkaccessmanager.h>
#include <qnetworkreply.h>
#include "../../QtUtils.h"
#include "../../QtStylesheet.h"
#include "../Model/HttpDlgErrorCode.h"

using namespace jlib::qt;
//
//namespace HBVideoPlatform {
//namespace common {

static QString methodToString(HttpDlg::Method method)
{
	switch (method) {
	case HttpDlg::Method::Get: return "GET";
		break;
	case HttpDlg::Method::Post:return "POST";
		break;
	case HttpDlg::Method::Put:return "PUT";
		break;
	case HttpDlg::Method::Patch:return "PATCH";
		break;
	case HttpDlg::Method::Delete:return "DELETE";
		break;
	default:return "";
		break;
	}
}

HttpDlg::HttpDlg(QWidget *parent, int timeout, int retries, HttpDlgGif gif)
	: QDialog(parent)
	, timeout(timeout)
	, time_out_sec_(timeout)
	, retrys(retrys)
	, gif_(gif)
{
	setWindowModality(Qt::WindowModal);
	setWindowFlags(Qt::FramelessWindowHint | Qt::SubWindow);
	if (!parent) {
		setAttribute(Qt::WA_TranslucentBackground);
	}

	/*if (gif == HttpDlgGif::Spinner1s_200px_gray) {
		setFixedSize(200, 200);
	} else {
		setFixedSize(630, 637);
	}*/

	setFixedSize(200, 200);

	label_ = new QLabel(this);
	label_->resize(width(), height());
	label_->move(0, 0);

	elapse_ = new QLabel(this);
	elapse_->resize(64, 64);
	elapse_->move((width() - elapse_->width()) / 2, (height() - elapse_->height()) / 2);
	elapse_->setStyleSheet(build_style(Qt::darkYellow, 12));
	elapse_->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	//elapse_->hide();

	parent ? center_to_parent(this, parent) : center_to_desktop(this);

	mgr = new QNetworkAccessManager(this);
}

HttpDlg::~HttpDlg()
{
}

void HttpDlg::get(const QUrl& url)
{
	lastRequest_ = QNetworkRequest(url);
	lastMethod_ = Method::Get;
	lastData_.clear();
	get(lastRequest_);
}

void HttpDlg::get(const QNetworkRequest& request)
{
	if (connection_) {
		disconnect(connection_);
	}
	connection_ = connect(mgr, &QNetworkAccessManager::finished, this, &HttpDlg::onFinished);
	lastRequest_ = request;
	lastMethod_ = Method::Get;
	lastData_.clear();
	reply_ = mgr->get(request);

	run();
}

void HttpDlg::post(const QUrl & url)
{
	QNetworkRequest request(url);
	request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
	lastRequest_ = request;
	lastMethod_ = Method::Post;
	lastData_.clear();
	post(request, QByteArray());
}

void HttpDlg::post(const QNetworkRequest& request)
{
	if (connection_) {
		disconnect(connection_);
	}
	connection_ = connect(mgr, &QNetworkAccessManager::finished, this, &HttpDlg::onFinished);
	lastRequest_ = request;
	lastMethod_ = Method::Post;
	lastData_.clear();
	reply_ = mgr->post(request, QByteArray());

	run();
}

void HttpDlg::post(const QNetworkRequest & request, const QByteArray & data)
{
	if (connection_) {
		disconnect(connection_);
	}
	connection_ = connect(mgr, &QNetworkAccessManager::finished, this, &HttpDlg::onFinished);
	lastRequest_ = request;
	lastMethod_ = Method::Post;
	lastData_ = data;
	reply_ = mgr->post(request, data);
	run();
}

void HttpDlg::put(const QNetworkRequest& request, const QByteArray& data)
{
	if (connection_) {
		disconnect(connection_);
	}
	connection_ = connect(mgr, &QNetworkAccessManager::finished, this, &HttpDlg::onFinished);
	lastRequest_ = request;
	lastMethod_ = Method::Put;
	lastData_ = data;
	reply_ = mgr->put(request, data);
	run();
}

void HttpDlg::patch(const QNetworkRequest& request, const QByteArray& data)
{
	if (connection_) {
		disconnect(connection_);
	}
	connection_ = connect(mgr, &QNetworkAccessManager::finished, this, &HttpDlg::onFinished);
	lastRequest_ = request;
	lastMethod_ = Method::Patch;
	lastData_ = data;
	reply_ = mgr->sendCustomRequest(request, "PATCH", data);
	run();
}

void HttpDlg::deleteResource(const QNetworkRequest& request)
{
	if (connection_) {
		disconnect(connection_);
	}
	connection_ = connect(mgr, &QNetworkAccessManager::finished, this, &HttpDlg::onFinished);
	lastRequest_ = request;
	lastMethod_ = Method::Delete;
	lastData_.clear();
	reply_ = mgr->deleteResource(request);
	run();
}

void HttpDlg::get_img(const QUrl& url)
{
	if (connection_) {
		disconnect(connection_);
	}
	connection_ = connect(mgr, &QNetworkAccessManager::finished, this, &HttpDlg::onImgFinished);
	QNetworkRequest request(url);
	reply_ = mgr->get(request);

	auto p = parentWidget();
	if (p) {
		p->setEnabled(false);
	}

	auto path = getGifPath();
	auto movie = new QMovie(path);
	label_->setMovie(movie);
	movie->start();

	time_out_sec_ = timeout;
	retry_counter = 0;
	timer_.start();
	timer_id_ = startTimer(1000);

	QDialog::exec();

	if (p) {
		p->setEnabled(true);
	}

	movie->deleteLater();
}

QString HttpDlg::getGifPath()
{
	switch (gif_) {
	case HttpDlg::HttpDlgGif::Spinner1s_200px_gray: return ":/jlibqt/Resources/Spinner-1s-200px_gray.gif";
	case HttpDlg::HttpDlgGif::Spinner1s_200px_blue: return ":/jlibqt/Resources/Spinner-1s-200px.gif";
	}
	return QString();
}

void HttpDlg::run()
{
	auto p = parentWidget();
	if (p) {
		p->setEnabled(false);
	}

	auto path = getGifPath();
	auto movie = new QMovie(path);
	label_->setMovie(movie);
	movie->start();

	time_out_sec_ = timeout;
	retry_counter = retrys;
	timer_.start();
	timer_id_ = startTimer(1000);

	QDialog::exec();

	if (p) {
		p->setEnabled(true);
	}

	movie->deleteLater();
}

void HttpDlg::timerEvent(QTimerEvent * e)
{
	MYQDEBUG << time_out_sec_;
	if (--time_out_sec_ > 0) {
		elapse_->setText(QString::number(time_out_sec_));
	} else {
		MYQCRITICAL << "timeout";
		disconnect(connection_);
		killTimer(timer_id_);
		if (reply_) {
			reply_->deleteLater();
		}

		MYQDEBUG << "retry_counter =" << retry_counter;

		if (retry_counter <= 0) {
			result_ = HttpDlgErrorCode::Timeout;
			QDialog::reject();
		} else {
			retry_counter--;
			connection_ = connect(mgr, &QNetworkAccessManager::finished, this, &HttpDlg::onFinished);
			switch (lastMethod_) {
			case HttpDlg::Method::Get:
				reply_ = mgr->get(lastRequest_);
				break;
			case HttpDlg::Method::Post:
				reply_ = mgr->post(lastRequest_, lastData_);
				break;
			case HttpDlg::Method::Put:
				reply_ = mgr->put(lastRequest_, lastData_);
				break;
			case HttpDlg::Method::Patch:
				reply_ = mgr->sendCustomRequest(lastRequest_, "PATCH", lastData_);
				break;
			case HttpDlg::Method::Delete:
				reply_ = mgr->deleteResource(lastRequest_);
				break;
			}

			time_out_sec_ = timeout;
			retry_counter = retrys;
			timer_.start();
			timer_id_ = startTimer(1000);
		}
	}
}

void HttpDlg::onImgFinished(QNetworkReply* reply)
{
	do {
		if (!reply) {
			MYQCRITICAL << "no reply";
			result_ = HttpDlgErrorCode::Timeout;
			break;
		}

		if (QNetworkReply::NoError != reply->error()) {
			httpReason_ = reply->errorString();
			MYQCRITICAL << httpReason_;
			result_ = HttpDlgErrorCode::Unknown;
			//break;
		}

		QVariant statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
		if (!statusCode.isValid()) {
			MYQDEBUG << "statusCode is not valid";
			result_ = HttpDlgErrorCode::HttpStatusNeq200;
			break;
		}

		httpStatusCode_ = statusCode.toInt();

		if (httpStatusCode_ != 200) {
			result_ = HttpDlgErrorCode::HttpStatusNeq200;
			httpReason_ = reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString();
			MYQCRITICAL << httpStatusCode_ << httpReason_;
			//break;
		}
		MYQDEBUG << reply->url() << "reply " << httpStatusCode_;
		pixReply_.loadFromData(reply->readAll());
		

	} while (false);

	killTimer(timer_id_);
	QDialog::accept();

	reply->deleteLater();
}

void HttpDlg::onFinished(QNetworkReply * reply)
{
	do {
		if (!reply) {
			MYQCRITICAL << "no reply";
			result_ = HttpDlgErrorCode::Timeout;
			break;
		}

		if (QNetworkReply::NoError != reply->error()) {
			httpReason_ = reply->errorString();
			MYQCRITICAL << httpReason_;
			result_ = HttpDlgErrorCode::Unknown;
			//break;
		}

		QVariant statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
		if (!statusCode.isValid()) {
			MYQDEBUG << "statusCode is not valid";
			result_ = HttpDlgErrorCode::HttpStatusNeq200;
			break;
		}

		httpStatusCode_ = statusCode.toInt();

		if (httpStatusCode_ != 200) {
			result_ = HttpDlgErrorCode::HttpStatusNeq200;
			httpReason_ = reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString();
			MYQCRITICAL << httpStatusCode_ << httpReason_;
			//break;
		}

		auto res = reply->readAll();
		Json::Reader reader;
		root_.clear();
		if (!reader.parse(res.constData(), root_)) {
			if (result_ != HttpDlgErrorCode::HttpStatusNeq200) {
				result_ = HttpDlgErrorCode::ParseJsonError;
			}
			break;
		}

		auto out = QString::fromUtf8(root_.toStyledString().data());
		MYQDEBUG << methodToString(lastMethod_) << reply->url() << "reply " << httpStatusCode_ << "\n" << out;

	} while (false);

	killTimer(timer_id_);
	QDialog::accept();

	reply->deleteLater();
}
//
//}
//}
