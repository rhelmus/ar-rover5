#ifndef UTILS_H
#define UTILS_H

class QFrame;
class QImage;
class QString;

QFrame *createFrameGroupWidget(const QString &title, bool sunken=false);
void changeBrightness(QImage &img, int delta);

#endif // UTILS_H
