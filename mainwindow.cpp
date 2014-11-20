#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <iostream>
#include <curl/curl.h>
#include <curl/easy.h>
#include <openssl/md5.h>
#include <string.h>

CURL * curl;

std::string urlencode(const std::string & data) {
    char * ret = curl_easy_escape(curl, data.c_str(), data.size());
    std::string s(ret);
    curl_free(ret);
    return s;
}

std::string n2h(unsigned char c) {
    if (c < 10) return std::string(1,(char)'0'+c);
    return std::string(1,(char)'a'+c-10);
}

std::string md5hex(const std::string & data) {
    unsigned char md [16];
    MD5((unsigned char *)data.c_str(), data.size(), md);

    std::string s;
    for (int i = 0; i < 16; i++) {
        s += n2h(md[i]>>4) + n2h(md[i]&0xF);
    }
    return s;
}

std::string buildurlcode(std::string id, std::string phonenum, std::string cc) {
    std::string tokenData_token   = "PdA2DJyKoUrwLw1Bg6EIhzh502dF9noR9uFCllGk1413401214298"; //{phone};

    std::string token = md5hex(tokenData_token + phonenum);

    std::string params = "cc=" + cc + "&in=" + phonenum + "&lc=US&lg=en&mcc=000&mnc=000&method=sms&id=" + md5hex(id) + "&token=" + token;
    std::string url = "https://v.whatsapp.net/v2/code?" + params;

    std::cout << url << std::endl;

    return url;
}

std::string buildurlreg(std::string id, std::string phonenum, std::string cc, std::string code) {
    std::string tokenData_token   = "PdA2DJyKoUrwLw1Bg6EIhzh502dF9noR9uFCllGk1413401214298"; //{phone};

    std::string token = md5hex(tokenData_token + phonenum);

    std::string params = "cc=" + cc + "&in=" + phonenum + "&id=" + md5hex(id) + "&code=" + code;
    std::string url = "https://v.whatsapp.net/v2/register?" + params;

    std::cout << url << std::endl;

    return url;
}


struct t_buf {
    int size;
    char buffer[16*1024];
};
size_t write_data(void *ptr, size_t size, size_t nmemb, t_buf *stream) {
    if (size*nmemb + stream->size > sizeof(stream->buffer))
        return -1;

    memcpy(&stream->buffer[stream->size], ptr, size*nmemb);
    return size*nmemb;
}

std::string dohttpget(std::string url) {
    t_buf res; memset(&res,0,sizeof(res));

    const char useragent[] = "WhatsApp/2.12.49 S40Version/14.26 Device/Nokia302";
    struct curl_slist *chunk = NULL;
    chunk = curl_slist_append(chunk, "Accept: text/json");

    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, useragent);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &res);
    if (CURLE_OK == curl_easy_perform(curl)) {
        return std::string(res.buffer);
    }
    else return "";
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    curl = curl_easy_init();

    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

std::string getField(const std::string & data, const std::string & field) {
    size_t p = data.find("\"" + field + "\"");
    if (p == std::string::npos) return "";

    std::string d = data.substr(p + ("\"" + field + "\"").size());
    while (d.size() > 0 && d[0] == ' ') d = d.substr(1);
    if (d.size() == 0 || d[0] != ':') return "";
    d = d.substr(1);

    while (d.size() > 0 && d[0] == ' ') d = d.substr(1);
    if (d.size() == 0 || d[0] != '"') return "";
    d = d.substr(1);

    p = d.find("\"");
    if (p == std::string::npos) return "";

    return d.substr(0,p);
}

void MainWindow::on_buttonSendSMS_clicked()
{
    std::string req = buildurlcode("0000000000",ui->editPhoneNumber->text().toStdString(),ui->editPrefixNumber->text().toStdString());
    std::string res = dohttpget(req);
    std::string status = getField(res, "status");
    std::string extra;
    if (status == "fail") extra = " (Reason: " + getField(res, "reason") + ")";
    ui->statusBar->showMessage(QString::fromStdString("Status: " + status + extra));

    ui->rawOut->setText(QString::fromStdString(res));
    std::cout << res << std::endl;
}



void MainWindow::on_buttonGetPassword_clicked()
{
    std::string req = buildurlreg("0000000000",ui->editPhoneNumber->text().toStdString(),ui->editPrefixNumber->text().toStdString(), ui->editSMSCode->text().toStdString());
    std::string res = dohttpget(req);

    std::string status = getField(res, "status");
    std::string pw = getField(res, "pw");
    std::string extra;
    if (status == "fail") extra = " (Reason: " + getField(res, "reason") + ")";

    ui->editPassword->setText(QString::fromStdString(pw));
    ui->statusBar->showMessage(QString::fromStdString("Status: " + status + extra));

    ui->rawOut->setText(QString::fromStdString(res));

/*		self.pvars = ["status", "login", "pw", "type", "expiration", "kind", "price", "cost", "currency", "price_expiration",
					  "reason","retry_after"]

*/

}



