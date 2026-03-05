#ifndef FACEHELPER_H
#define FACEHELPER_H

namespace ChatPublic{
	QString getFaceID(const QString &resPath);
	QString getFacePath(const QString &faceID);
	QString replaceFace2LocRes(const QString &text, QList<QUrl> &rsltUrls);
	QString replaceLocRes2Face(const QString &text);
}
#endif // FACEHELPER_H
