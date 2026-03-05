#include "stdafx.h"
#include "FaceHelper.h"

namespace ChatPublic{
	QMap<QString, QString> g_faceIdMap{
		{ ":/Res/IM/Face/1F4A4.gif", QStringLiteral("\U0001F4A4") }, //zzz
		{ ":/Res/IM/Face/1F44B.gif", QStringLiteral("\U0001F44B") }, //wave
		{ ":/Res/IM/Face/1F44D.gif", QStringLiteral("\U0001F44D") }, //thumbsup
		{ ":/Res/IM/Face/1F44E.gif", QStringLiteral("\U0001F44E") }, //thumbsdown
		{ ":/Res/IM/Face/1F44F.gif", QStringLiteral("\U0001F44F") }, //clap
		{ ":/Res/IM/Face/1F494.gif", QStringLiteral("\U0001F494") }, //broken_heart
		{ ":/Res/IM/Face/1F496.gif", QStringLiteral("\U0001F496") }, //sparkling_heart
		{ ":/Res/IM/Face/1F601.gif", QStringLiteral("\U0001F601") }, //grin
		{ ":/Res/IM/Face/1F602.gif", QStringLiteral("\U0001F602") }, //joy
		{ ":/Res/IM/Face/1F604.gif", QStringLiteral("\U0001F604") }, //smile
		{ ":/Res/IM/Face/1F606.gif", QStringLiteral("\U0001F606") }, //satisfied
		{ ":/Res/IM/Face/1F608.gif", QStringLiteral("\U0001F608") }, //smiling_imp
		{ ":/Res/IM/Face/1F609.gif", QStringLiteral("\U0001F609") }, //wink
		{ ":/Res/IM/Face/1F60C.gif", QStringLiteral("\U0001F60C") }, //relieved
		{ ":/Res/IM/Face/1F60D.gif", QStringLiteral("\U0001F60D") }, //heart_eyes
		{ ":/Res/IM/Face/1F60E.gif", QStringLiteral("\U0001F60E") }, //sunglasses
		{ ":/Res/IM/Face/1F621.gif", QStringLiteral("\U0001F621") }, //pout
		{ ":/Res/IM/Face/1F624.gif", QStringLiteral("\U0001F624") }, //triumph
		{ ":/Res/IM/Face/1F629.gif", QStringLiteral("\U0001F629") }, //weary
		{ ":/Res/IM/Face/1F62D.gif", QStringLiteral("\U0001F62D") }, //sob
		{ ":/Res/IM/Face/1F62F.gif", QStringLiteral("\U0001F62F") }, //hushed
		{ ":/Res/IM/Face/1F631.gif", QStringLiteral("\U0001F631") }, //scream
		{ ":/Res/IM/Face/1F633.gif", QStringLiteral("\U0001F633") }, //flushed
		{ ":/Res/IM/Face/1F634.gif", QStringLiteral("\U0001F634") }, //sleeping
		{ ":/Res/IM/Face/1F644.gif", QStringLiteral("\U0001F644") }, //roll_eyes
		{ ":/Res/IM/Face/1F910.gif", QStringLiteral("\U0001F910") }, //zipper_mouth_face
		{ ":/Res/IM/Face/1F914.gif", QStringLiteral("\U0001F914") }, //thinking
		{ ":/Res/IM/Face/1F91D.gif", QStringLiteral("\U0001F91D") }, //handshake
		{ ":/Res/IM/Face/1F923.gif", QStringLiteral("\U0001F923") }, //rofl
		{ ":/Res/IM/Face/1F929.gif", QStringLiteral("\U0001F929") }, //star_struck
		{ ":/Res/IM/Face/1F970.gif", QStringLiteral("\U0001F970") }, //smiling_face_with_three_hearts
		{ ":/Res/IM/Face/1F971.gif", QStringLiteral("\U0001F971") }, //yawning_face
		{ ":/Res/IM/Face/1F973.gif", QStringLiteral("\U0001F973") }, //partying_face
		{ ":/Res/IM/Face/1F974.gif", QStringLiteral("\U0001F974") }, //woozy_face
		{ ":/Res/IM/Face/1F97A.gif", QStringLiteral("\U0001F97A") }, //pleading_face
		{ ":/Res/IM/Face/2639.gif", QStringLiteral("\U00002639") }, //frowning_face
		{ ":/Res/IM/Face/263A.gif", QStringLiteral("\U0000263A") }, //relaxed
		{ ":/Res/IM/Face/270C.gif", QStringLiteral("\U0000270C") }, //v
		{ ":/Res/IM/Face/1f61c.gif", QStringLiteral("\U0001F61C") }, //stuck_out_tongue_winking_eye
		{ ":/Res/IM/Face/1F31B.gif", QStringLiteral("\U0001F31B") }, //first_quarter_moon_with_face
	};
	QString g_pattern = QString("[%1]").arg(g_faceIdMap.values().join(""));
	/*static const QString pattern(QStringLiteral("[\U00002712\U00002714\U00002716\U0000271d\U00002721\U00002728\U00002733\U00002734\U00002744\U00002747\U0000274c\U0000274e\U00002753-\U00002755\U00002757\U00002763\U00002764\U00002795-\U00002797\U000027a1\U000027b0\U000027bf\U00002934\U00002935\U00002b05-\U00002b07\U00002b1b\U00002b1c\U00002b50\U00002b55\U00003030\U0000303d\U0001f004\U0001f0cf\U0001f170\U0001f171\U0001f17e\U0001f17f\U0001f18e\U0001f191-\U0001f19a\U0001f201\U0001f202\U0001f21a\U0001f22f\U0001f232-\U0001f23a\U0001f250\U0001f251\U0001f300-\U0001f321\U0001f324-\U0001f393\U0001f396\U0001f397\U0001f399-\U0001f39b\U0001f39e-\U0001f3f0\U0001f3f3-\U0001f3f5\U0001f3f7-\U0001f4fd\U0001f4ff-\U0001f53d\U0001f549-\U0001f54e\U0001f550-\U0001f567\U0001f56f\U0001f570\U0001f573-\U0001f579\U0001f587\U0001f58a-\U0001f58d\U0001f590\U0001f595\U0001f596\U0001f5a5\U0001f5a8\U0001f5b1\U0001f5b2\U0001f5bc\U0001f5c2-\U0001f5c4\U0001f5d1-\U0001f5d3\U0001f5dc-\U0001f5de\U0001f5e1\U0001f5e3\U0001f5ef\U0001f5f3\U0001f5fa-\U0001f64f\U0001f680-\U0001f6c5\U0001f6cb-\U0001f6d0\U0001f6e0-\U0001f6e5\U0001f6e9\U0001f6eb\U0001f6ec\U0001f6f0\U0001f6f3\U0001f910-\U0001f918\U0001f980-\U0001f984\U0001f9c0\U00003297\U00003299\U000000a9\U000000ae\U0000203c\U00002049\U00002122\U00002139\U00002194-\U00002199\U000021a9\U000021aa\U0000231a\U0000231b\U00002328\U00002388\U000023cf\U000023e9-\U000023f3\U000023f8-\U000023fa\U000024c2\U000025aa\U000025ab\U000025b6\U000025c0\U000025fb-\U000025fe\U00002600-\U00002604\U0000260e\U00002611\U00002614\U00002615\U00002618\U0000261d\U00002620\U00002622\U00002623\U00002626\U0000262a\U0000262e\U0000262f\U00002638-\U0000263a\U00002648-\U00002653\U00002660\U00002663\U00002665\U00002666\U00002668\U0000267b\U0000267f\U00002692-\U00002694\U00002696\U00002697\U00002699\U0000269b\U0000269c\U000026a0\U000026a1\U000026aa\U000026ab\U000026b0\U000026b1\U000026bd\U000026be\U000026c4\U000026c5\U000026c8\U000026ce\U000026cf\U000026d1\U000026d3\U000026d4\U000026e9\U000026ea\U000026f0-\U000026f5\U000026f7-\U000026fa\U000026fd\U00002702\U00002705\U00002708-\U0000270d\U0000270f]|[#]\U000020e3|[*]\U000020e3|[0]\U000020e3|[1]\U000020e3|[2]\U000020e3|[3]\U000020e3|[4]\U000020e3|[5]\U000020e3|[6]\U000020e3|[7]\U000020e3|[8]\U000020e3|[9]\U000020e3|\U0001f1e6[\U0001f1e8-\U0001f1ec\U0001f1ee\U0001f1f1\U0001f1f2\U0001f1f4\U0001f1f6-\U0001f1fa\U0001f1fc\U0001f1fd\U0001f1ff]|\U0001f1e7[\U0001f1e6\U0001f1e7\U0001f1e9-\U0001f1ef\U0001f1f1-\U0001f1f4\U0001f1f6-\U0001f1f9\U0001f1fb\U0001f1fc\U0001f1fe\U0001f1ff]|\U0001f1e8[\U0001f1e6\U0001f1e8\U0001f1e9\U0001f1eb-\U0001f1ee\U0001f1f0-\U0001f1f5\U0001f1f7\U0001f1fa-\U0001f1ff]|\U0001f1e9[\U0001f1ea\U0001f1ec\U0001f1ef\U0001f1f0\U0001f1f2\U0001f1f4\U0001f1ff]|\U0001f1ea[\U0001f1e6\U0001f1e8\U0001f1ea\U0001f1ec\U0001f1ed\U0001f1f7-\U0001f1fa]|\U0001f1eb[\U0001f1ee-\U0001f1f0\U0001f1f2\U0001f1f4\U0001f1f7]|\U0001f1ec[\U0001f1e6\U0001f1e7\U0001f1e9-\U0001f1ee\U0001f1f1-\U0001f1f3\U0001f1f5-\U0001f1fa\U0001f1fc\U0001f1fe]|\U0001f1ed[\U0001f1f0\U0001f1f2\U0001f1f3\U0001f1f7\U0001f1f9\U0001f1fa]|\U0001f1ee[\U0001f1e8-\U0001f1ea\U0001f1f1-\U0001f1f4\U0001f1f6-\U0001f1f9]|\U0001f1ef[\U0001f1ea\U0001f1f2\U0001f1f4\U0001f1f5]|\U0001f1f0[\U0001f1ea\U0001f1ec-\U0001f1ee\U0001f1f2\U0001f1f3\U0001f1f5\U0001f1f7\U0001f1fc\U0001f1fe\U0001f1ff]|\U0001f1f1[\U0001f1e6-\U0001f1e8\U0001f1ee\U0001f1f0\U0001f1f7-\U0001f1fb\U0001f1fe]|\U0001f1f2[\U0001f1e6\U0001f1e8-\U0001f1ed\U0001f1f0-\U0001f1ff]|\U0001f1f3[\U0001f1e6\U0001f1e8\U0001f1ea-\U0001f1ec\U0001f1ee\U0001f1f1\U0001f1f4\U0001f1f5\U0001f1f7\U0001f1fa\U0001f1ff]|\U0001f1f4\U0001f1f2|\U0001f1f5[\U0001f1e6\U0001f1ea-\U0001f1ed\U0001f1f0-\U0001f1f3\U0001f1f7-\U0001f1f9\U0001f1fc\U0001f1fe]|\U0001f1f6\U0001f1e6|\U0001f1f7[\U0001f1ea\U0001f1f4\U0001f1f8\U0001f1fa\U0001f1fc]|\U0001f1f8[\U0001f1e6-\U0001f1ea\U0001f1ec-\U0001f1f4\U0001f1f7-\U0001f1f9\U0001f1fb\U0001f1fd-\U0001f1ff]|\U0001f1f9[\U0001f1e6\U0001f1e8\U0001f1e9\U0001f1eb-\U0001f1ed\U0001f1ef-\U0001f1f4\U0001f1f7\U0001f1f9\U0001f1fb\U0001f1fc\U0001f1ff]|\U0001f1fa[\U0001f1e6\U0001f1ec\U0001f1f2\U0001f1f8\U0001f1fe\U0001f1ff]|\U0001f1fb[\U0001f1e6\U0001f1e8\U0001f1ea\U0001f1ec\U0001f1ee\U0001f1f3\U0001f1fa]|\U0001f1fc[\U0001f1eb\U0001f1f8]|\U0001f1fd\U0001f1f0|\U0001f1fe[\U0001f1ea\U0001f1f9]|\U0001f1ff[\U0001f1e6\U0001f1f2\U0001f1fc]"));*/
	//本地资源转网络发送文本
	QString getFaceID(const QString &resPath)
	{
		QFileInfo fi(resPath);
		//转换为emoji编码
		if (resPath.startsWith(":/Res/IM/Face"))
		{
			return g_faceIdMap.value(resPath);
		}
		return "";
	}

	QString getFacePath(const QString &faceID)
	{
		return g_faceIdMap.key(faceID);
	}

	//替换表情为本地资源
	QString replaceFace2LocRes(const QString &text, QList<QUrl> &rsltUrls)
	{
		QString rslt(text);
		QVector<uint> unicode = text.toUcs4();
		//处理emoji表情替换
		QRegularExpression emojiReg = QRegularExpression(g_pattern);
		QRegularExpressionMatchIterator it = emojiReg.globalMatch(text);
		while (it.hasNext())
		{
			QRegularExpressionMatch match = it.next();
			QString findStr = match.captured();
			//QVector<uint> unicode = match.captured().toUcs4();
			QString resUrl = getFacePath(findStr);

			//替换表情ID
			if (!resUrl.isEmpty())
			{
				QUrl url = QUrl::fromLocalFile(resUrl);
				QString repleaceStr = QString("<img src = \"%1\"/>").arg(url.toString());
				rslt.replace(findStr, repleaceStr);
				rsltUrls.append(url);
			}
		}
		return rslt;
	}

	QString replaceLocRes2Face(const QString &text)
	{
		QString rslt(text);
		QRegularExpression reg("<img\\s+src=\"file::/Res/IM/Face/([0-9A-Fa-f]{4,})\\.gif\"\\s*/>");
		QRegularExpressionMatchIterator it = reg.globalMatch(text);
		while (it.hasNext())
		{
			QRegularExpressionMatch match = it.next();
			QString capStr = match.captured();//<img src="file::/Res/IM/Face/1F60D.gif" />
			QString faceName = match.captured(1);//1F60D
			QString faceId = getFaceID(QString(":/Res/IM/Face/%1.gif").arg(faceName));
			if (faceId.isEmpty())
			{
				continue;
			}
			rslt.replace(capStr, faceId);
		}
		return rslt;
	}
}
