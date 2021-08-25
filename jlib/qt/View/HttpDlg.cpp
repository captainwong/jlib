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

HttpDlg::HttpDlg(QWidget *parent, int timeout, HttpDlgGif gif)
	: QDialog(parent)
	, time_out_sec_(timeout)
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
	get(QNetworkRequest(url));
}

void HttpDlg::get(const QNetworkRequest& request)
{
	if (connection_) {
		disconnect(connection_);
	}
	connection_ = connect(mgr, &QNetworkAccessManager::finished, this, &HttpDlg::onFinished);
	reply_ = mgr->get(request);

	run();
}

void HttpDlg::post(const QUrl & url)
{
	QNetworkRequest request(url);
	request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded"); 
	post(request, QByteArray());
}

void HttpDlg::post(const QNetworkRequest& request)
{
	if (connection_) {
		disconnect(connection_);
	}
	connection_ = connect(mgr, &QNetworkAccessManager::finished, this, &HttpDlg::onFinished);
	reply_ = mgr->post(request, QByteArray());

	run();
}

void HttpDlg::post(const QNetworkRequest & request, const QByteArray & data)
{
	if (connection_) {
		disconnect(connection_);
	}
	connection_ = connect(mgr, &QNetworkAccessManager::finished, this, &HttpDlg::onFinished);
	reply_ = mgr->post(request, data);
	run();
}

void HttpDlg::put(const QNetworkRequest& request, const QByteArray& data)
{
	if (connection_) {
		disconnect(connection_);
	}
	connection_ = connect(mgr, &QNetworkAccessManager::finished, this, &HttpDlg::onFinished);
	reply_ = mgr->put(request, data);
	run();
}

void HttpDlg::patch(const QNetworkRequest& request, const QByteArray& data)
{
	if (connection_) {
		disconnect(connection_);
	}
	connection_ = connect(mgr, &QNetworkAccessManager::finished, this, &HttpDlg::onFinished);
	reply_ = mgr->sendCustomRequest(request, "PATCH", data);
	run();
}

void HttpDlg::deleteResource(const QNetworkRequest& request)
{
	if (connection_) {
		disconnect(connection_);
	}
	connection_ = connect(mgr, &QNetworkAccessManager::finished, this, &HttpDlg::onFinished);
	reply_ = mgr->deleteResource(request);
	run();
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
		result_ = HttpDlgErrorCode::Timeout;
		QDialog::reject();
	}
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
			result_ = HttpDlgErrorCode::ParseJsonError;
			break;
		}

		MYQDEBUG << reply->url() << "reply:\n" << httpStatusCode_ << root_.toStyledString().data();

	} while (false);

	killTimer(timer_id_);
	QDialog::accept();

	reply->deleteLater();
}
//
//}
//}
