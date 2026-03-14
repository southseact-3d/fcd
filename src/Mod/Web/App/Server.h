#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <QByteArray>
#include <QEvent>
#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>


namespace Web
{

class Firewall
{
public:
    Firewall();
    virtual ~Firewall();
    virtual bool filter(const QByteArray&) const = 0;

public:
    static Firewall* getInstance();
    static void setInstance(Firewall*);

private:
    static Firewall* instance;
};

class FirewallPython: public Firewall
{
public:
    explicit FirewallPython(const Py::Object&);
    ~FirewallPython() override;
    bool filter(const QByteArray&) const override;

private:
    Py::Object obj;
};

class ServerEvent: public QEvent
{
public:
    ServerEvent(QTcpSocket* socket, const QByteArray&);
    ~ServerEvent() override;

    QTcpSocket* socket() const;
    const QByteArray& request() const;

private:
    QTcpSocket* sock;
    QByteArray text;
};

/**
 * The Server class implements a simple TCP server.
 */
class AppServer: public QTcpServer
{
    Q_OBJECT

public:
    explicit AppServer(bool direct = false, QObject* parent = nullptr);

protected:
    void incomingConnection(qintptr socket) override;
    void customEvent(QEvent* e) override;

private:
    std::string handleRequest(QByteArray);
    static std::string runPython(const QByteArray&);
    std::string getRequest(const std::string&) const;

private Q_SLOTS:
    void readClient();
    void discardClient();

private:
    bool direct;
    Py::Object module;
};

}  // namespace Web

#endif  // Web_SERVER_H
