#include "HttpDlg.h"
#include <qmovie.h>
#include <qnetworkaccessmanager.h>
#include <qnetworkreply.h>
#include <jlib/qt/QtPathHelper.h>
#include <jlib/qt/QtUtils.h>
#include <jlib/qt/QtStylesheet.h>
#include "../Model/HttpDlgErrorCode.h"

using namespace jlib::qt;

namespace HBVideoPlatform {
namespace common {

HttpDlg::HttpDlg(QWidget *parent, HttpDlgViewSize sz, int timeout)
	: QDialog(parent)
	, sz_(sz)
	, time_out_sec_(timeout)
{
	setWindowModality(Qt::WindowModal);
	setWindowFlags(Qt::FramelessWindowHint); 
	if (!parent) {
		setAttribute(Qt::WA_TranslucentBackground);
	}

	if (sz == sz_small) {
		setFixedSize(200, 200);
	} else {
		setFixedSize(630, 637);
	}

	label_ = new QLabel(this);
	label_->resize(width(), height());
	label_->move(0, 0);

	elapse_ = new QLabel(this);
	elapse_->resize(180, 80);
	elapse_->move((width() - elapse_->width()) / 2, (height() - elapse_->height()) / 2);
	elapse_->setStyleSheet(build_style(Qt::darkYellow, 64));
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
	if (connection_) {
		disconnect(connection_);
	}
	connection_ = connect(mgr, &QNetworkAccessManager::finished, this, &HttpDlg::onFinished);
	reply_ = mgr->get(QNetworkRequest(url));

	run();
}

void HttpDlg::post(const QUrl & url)
{
	if (connection_) {
		disconnect(connection_);
	}
	connection_ = connect(mgr, &QNetworkAccessManager::finished, this, &HttpDlg::onFinished);
	QNetworkRequest request(url);
	request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
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

void HttpDlg::run()
{
	auto path = PathHelper::program();
	path += sz_ == sz_small ? "/Skin/gif/ajax-loader-small.gif" : "/Skin/gif/preloader.gif";
	auto movie = new QMovie(path);
	label_->setMovie(movie);
	movie->start();

	timer_.start();

	auto p = parentWidget();
	if (p) {
		p->setEnabled(false);
	}

	startTimer(1000);
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
		result_ = HttpDlgErrorCode::NetworkError;
		QDialog::reject();
	}
}

void HttpDlg::onFinished(QNetworkReply * reply)
{
	do {
		if (!reply) {
			MYQCRITICAL << "no reply";
			result_ = HttpDlgErrorCode::NetworkError;
			break;
		}

		if (QNetworkReply::NoError != reply->error()) {
			httpReason_ = reply->errorString();
			MYQCRITICAL << httpReason_;
			result_ = HttpDlgErrorCode::NetworkError;
			break;
		}

		QVariant statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
		if (!statusCode.isValid()) {
			MYQDEBUG << "statusCode is not valid";
			break;
		}

		httpStatusCode_ = statusCode.toInt();

		if (httpStatusCode_ != 200) {
			result_ = HttpDlgErrorCode::NetworkError;
			httpReason_ = reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString();
			MYQCRITICAL << httpStatusCode_ << httpReason_;
			break;
		}

		auto res = reply->readAll();
		Json::Reader reader;
		root_.clear();
		if (!reader.parse(res.constData(), root_)) {
			//result_ = HttpDlgErrorCode::ParseJsonError;
			//break;
		}

		MYQDEBUG << reply->url() << "reply:\n" << root_.toStyledString().data();

	} while (false);

	QDialog::accept();

	reply->deleteLater();
}

}
}
