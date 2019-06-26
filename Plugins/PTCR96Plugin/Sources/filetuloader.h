#ifndef FILETULOADER_H
#define FILETULOADER_H

#include <QObject>
#include <QTime>
#include <QTimer>

class FileTULoader : public QObject
{
    Q_OBJECT
public:
    struct TUStruct {
        QTime time;
        double dist;
        double speed;
        double hAngle;
        double vAngle;

        TUStruct() {
            dist = 0.0;
            speed = 0.0;
            hAngle = 0.0;
            vAngle = 0.0;
        }
    };

    explicit FileTULoader(QObject *parent = 0);
    ~FileTULoader();

private:
    QList<TUStruct> tuList;
    double prevH, prevV;
    QTime currTime;
    QTimer timer;
    int currIndex;

signals:
    void playedTime(QString time);
    void info(QString infStr);
    void opuPos(double hor, double vert);

private slots:
    void tuTick();

public slots:
    bool openFile(QString filename);
    void play();
    void stop();

};

#endif // FILETULOADER_H
